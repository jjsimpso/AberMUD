 /****************************************************************************\
 *									      *
 *			C R E A T O R    O F   L E G E N D S		      *
 *				(AberMud Version 5)			      *
 *									      *
 *  The Creator Of Legends System is (C) Copyright 1989 Alan Cox, All Rights  *
 *  Reserved.		  						      *
 *									      *
 \****************************************************************************/

/*
 *	Container Control
 *
 *	1.00		AGC		Container Logic
 *	1.01		AGC		Added recursion checks
 *	1.02		AGC		Fixed weight limit on player bug
 */

#include "System.h"

Module  "Containers";
Version "1.02";
Author  "Alan Cox";


/*
 *	Find the weight of an item's contents
 */


static int WeightRec(x,d)
ITEM *x;
int d;
{
	int n=WeightOf(x);
	register ITEM *o;
	if(d>32)
		return(0);	/* Depth too great */
	o=O_CHILDREN(x);
	while(o)
	{
		n+=WeightRec(o,d+1);
		o=O_NEXT(o);
	}
	return(n);
}


int WeighUp(x)
ITEM *x;
{
	return(WeightRec(x,0));
}

/*
 *	Return the weight of an item alone, not including its contents
 */

int WeightOf(x)
ITEM *x;
{
	OBJECT *o=ObjectOf(x);
	PLAYER *p=PlayerOf(x);
	if(o)
		return(o->ob_Weight);
	if(p)
		return(p->pl_Weight);
	return(0);
}

/*
 *	Sort out the size of an items contents
 */

static int SizeRec(x,d)
ITEM *x;
int d;
{
	ITEM *o;
	int n=0;
	o=O_CHILDREN(x);
	if(d>32)
		return(0);
	while(o)
	{
		n+=SizeOfRec(o,d);
		o=O_NEXT(o);
	}
	return(n);
}

int SizeContents(x)
ITEM *x;
{
	return(SizeRec(x,0));
}

/*
 *	Calculate size of an item, allowing for soft flags etc.
 */

int SizeOfRec(o,d)
ITEM *o;
int d;
{
	OBJECT *a=ObjectOf(o);
	PLAYER *b=PlayerOf(o);
	CONTAINER *c=ContainerOf(o);
	if(((c)&&(c->co_Flags&CO_SOFT))||(!c))
	{
		if(a)
			return(a->ob_Size+SizeRec(o,d+1));
		if(b)
			return(b->pl_Size+SizeRec(o,d+1));
		return(SizeRec(o,d+1));
	}
	if(a)
		return(a->ob_Size);
	if(b)
		return(b->pl_Size);
	return(0);
}	
/*
 * CanPlace: BUGS. CanPlace only enforces weight limiting in the direct
 * parent. Thus an item placed into a container a player holds directly will
 * fail. Normal operations are such that a player must hold an item to do this.
 * Be aware of the limitations in special cases. If you need to stick entries
 * in the tables of the form NOT CANPLACE <item> $ME MESSAGE {Cant carry it}
 * when moving from item->item through the player
 */

int CanPlace(x,y)
ITEM *x;
ITEM *y;
{
	ITEM *z=O_PARENT(x);
	CONTAINER *c=ContainerOf(y);
	PLAYER *p=PlayerOf(y);
	int cap;
	int wt;
	if((c==NULL)&&(p==NULL))
		return(0);	/* Fits Fine */
	XPlace(x,NULL);		/* Avoid disturbing figures */
	if(c)
		cap=SizeContents(y);
	wt=WeighUp(y);
	XPlace(x,z);
	if(c)
	{	
		cap=c->co_Volume-cap;
		cap-=SizeOfRec(x,0);	/* - size of item going in */
		if(cap<0)
			return(-1);	/* Too big to fit */
	}
	if(p)
		if(wt+WeighUp(x)-WeightOf(y)>p->pl_Level*10+400)
			return(-2);	/* Too heavy */
	if(p)
	{
		short ct=0;
		ITEM *step=O_CHILDREN(y);
		while(step)
		{
			ct++;
			step=O_NEXT(step);
		}
		if(ct>9)
			return(-3);	/* Too much */
	}
	return(0);
}
