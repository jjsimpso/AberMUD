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
 *	Dark/Light model
 *
 *	1.00	Original Version
 *	1.01	ANSIfied
 */

#include "System.h"

Module "DarkLight";
Author "Alan Cox";
Version "1.01";


/*
 *	Decide if item 'i' is in the dark.
 */

int IsDarkFor(ITEM *i)
{
	ITEM *p;
	short rct=5;
	if((p=O_PARENT(i))==NULL)
	{
/*
 *	A question for the philosophers - is nothingness dark or not.....
 *	For game purposes we say it isn't, and the player gets 'The void' etc
 */	
		return(1);
	}
/*
 *	We go up the tree first looking for higher lights
 */
reup:	if(O_PARENT(p))
	{
		if(ContainerOf(p))
		{
			if((ContainerOf(p)->co_Flags&CO_SEETHRU)||
			   ((ContainerOf(p)->co_Flags&CO_CLOSES)&&(!O_STATE(p))))
			{
				p=O_PARENT(p);
				if(--rct)	/* MAX 5 Levels of upping */
					goto reup;
			}
			return(RecCheckDark(p,(short)0));
		}
		else
		{
			p=O_PARENT(p);
			if(--rct)
				goto reup;
		}
	}
	return(RecCheckDark(p,(short)0));
}


/*
 *	Scan recursively downwards looking for light sources
 */

int RecCheckDark(ITEM *item, short depth)
{
	ROOM *r;
	OBJECT *o;
	ITEM *x;
	CONTAINER *c;
	if(depth>31)		/* Recursion Limit of 32 */
		return(1);
	r=RoomOf(item);
	if((r)&&((r->rm_Flags&RM_DARK)==0))
		return(0);	/* lit by room - easy case */
	o=ObjectOf(item);
	if((o)&&(IsLit(item)))
		return(0);	/* Do we light it up ?*/
	x=O_CHILDREN(item);
	while(x)		/* See if the contents do */
	{
		c=ContainerOf(x);
		if((c)&&(c->co_Flags&CO_CLOSES)&&(O_STATE(x)==0))
			goto l2;	/* Open so can see in */
		if((c)&&((c->co_Flags&CO_SEETHRU)==0))
			goto nx;	/* Opaque item */
l2:		if(RecCheckDark(x,(short)(depth+1))==0)
			return(0);	/* Light found */
nx:		x=O_NEXT(x);
	}
	return(1);	/* REALLY DARK */
}		
	
