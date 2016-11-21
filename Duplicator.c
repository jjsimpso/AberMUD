 /****************************************************************************\
 *									      *
 *			C R E A T O R    O F   L E G E N D S		      *
 *				(AberMud Version 5)			      *
 *									      *
 *  The Creator Of Legends System is (C) Copyright 1989 Alan Cox, All Rights  *
 *  Reserved.		  						      *
 *									      *
 \****************************************************************************/

#include "System.h"

/*
 *	1.00	AGC		Created File
 *	1.01	AGC		Added IOH to duplicatables
 *	1.02	AGC		Fixed IOH dup
 *	1.03	AGC		Fixed CondExit dup
 *	1.04	AGC		Knows about tables and superclassing
 *	1.05	AGC		ANSIfication
 */
 
Module  "Duplicator";
Version "1.05";
Author  "Alan Cox";

/*
 *	This routine is basically a software Xerox machine, one item goes in
 *	and two of them come out. The copy can be either a true copy or one
 *	which will be destroyed later.
 */

int Clone_Room(ITEM *i, ITEM *j)
{
	ROOM *r,*s;
	if(MakeRoom(i)==-1)
		return(-1);
	r=(ROOM *)FindSub(i,KEY_ROOM);
	s=(ROOM *)FindSub(j,KEY_ROOM);
	if((!r)||(!s))
		Error("CloneRoom: Not Room!");
	FreeText(r->rm_Short);
	FreeText(r->rm_Long);
	r->rm_Flags=s->rm_Flags;
	r->rm_Short=AllocText(TextOf(s->rm_Short));
	r->rm_Long=AllocText(TextOf(s->rm_Long));
	return(0);
}

int Clone_Object(ITEM *i, ITEM *j)
{
	short x=0;
	OBJECT *r,*s;
	if(MakeObject(i)==-1)
		return(-1);
	r=(OBJECT *)FindSub(i,KEY_OBJECT);
	s=(OBJECT *)FindSub(j,KEY_OBJECT);
	if((!r)||(!s))
		Error("CloneObject: Not Object!");
	FreeText(r->ob_Text[0]);
	FreeText(r->ob_Text[1]);
	FreeText(r->ob_Text[2]);
	FreeText(r->ob_Text[3]);
	r->ob_Flags=s->ob_Flags;
	r->ob_Size=s->ob_Size;
	r->ob_Weight=s->ob_Weight;
	while(x<4)
	{
		r->ob_Text[x]=AllocText(TextOf(s->ob_Text[x]));
		x++;
	}
	return(0);
}

int Clone_Player(ITEM *i, ITEM *j)
{
	PLAYER *r,*s;
	if(MakePlayer(i)==-1)
		return(-1);
	r=(PLAYER *)FindSub(i,KEY_PLAYER);
	s=(PLAYER *)FindSub(j,KEY_PLAYER);
	if((!r)||(!s))
		Error("ClonePlayer: Not Player!");
	r->pl_Flags=s->pl_Flags;
	r->pl_UserKey=-1;	/* Never DUP user handles! */
	r->pl_Size=s->pl_Size;
	r->pl_Weight=s->pl_Weight;
	r->pl_Level=s->pl_Level;
	r->pl_Score=s->pl_Score;
	r->pl_Strength=s->pl_Strength;
	return(0);
}

int Clone_GenExit(ITEM *i, ITEM *j)
{
	short ct=0;
	GENEXIT *r,*s;
	if(MakeGenExit(i)==-1)
		return(-1);
	r=(GENEXIT *)FindSub(i,KEY_GENEXIT);
	s=(GENEXIT *)FindSub(j,KEY_GENEXIT);
	if((!r)||(!s))
		Error("CloneGenExit: Not GenExit!");
	while(ct<12)
	{
		r->ge_Dest[ct]=s->ge_Dest[ct];
		if(r->ge_Dest[ct])
			LockItem(r->ge_Dest[ct]);
		ct++;
	}
	return(0);
}

int Clone_MsgExit(ITEM *i, MSGEXIT *s)
{
	MSGEXIT *r;
	if((r=MakeMsgExit(i,s->me_Dest,s->me_ExitNumber,TextOf(s->me_Text)))
		==NULL)
		return(-1);
	return(0);
}

int Clone_Chain(ITEM *i, CHAIN *c)
{
	AddChain(i,c->ch_Chained);
	return(0);
}

int Clone_Container(ITEM *i, CONTAINER *c)
{
	CONTAINER *r=BeContainer(i);
	if(!r)
		return(-1);
	r->co_Volume=c->co_Volume;
	r->co_Flags=c->co_Flags;
	return(0);
}

int Clone_UserFlag(ITEM *i, USERFLAG *u)
{
	short ct=0;
	while(ct<8)
	{
		SetUserFlag(i,ct,u->uf_Flags[ct]);
		ct++;
	}
	ct=0;
	while(ct<8)
	{
		SetUserItem(i,ct,u->uf_Items[ct]);
		ct++;
	}
	return(0);
}

int Clone_UserText(ITEM *i, ITEM *j)
{
	int ct=0;
	while(ct<8)
	{
		SetUText(j,ct,GetUText(i,ct));
		ct++;
	}
	return(0);
}

int Clone_Inherit(ITEM *i, INHERIT *h)
{
	MakeInherit(i,h->in_Master);
	return(0);
}

ITEM *Clone_Item(ITEM *i, short f)	/* 1=true dup 0=normal dup */
{
	DUP *d;
	ITEM *n=CreateItem(NameOf(i),i->it_Adjective,i->it_Noun);
	SUB *s;
	n->it_ActorTable=i->it_ActorTable;
	n->it_ActionTable=i->it_ActionTable;
	n->it_State=i->it_State;
	n->it_Class=i->it_Class;
	n->it_Perception=i->it_Perception;
	n->it_SubjectTable=NULL;
	n->it_ObjectTable=NULL;
	n->it_DaemonTable=NULL;
	/* Cheat: We want the clone to behave as the original, but that
	   would mean copying the tables which is slow and uses lots of
	   memory. We use the new subclassing feature to cheat and make
	   the original superclass of the clones. */
	LockItem(i);
	n->it_Superclass=i;

	if(IsRoom(i))
		Clone_Room(n,i);
	if(IsPlayer(i))
		Clone_Player(n,i);
	if(IsObject(i))
		Clone_Object(n,i);
	if(FindSub(i,KEY_GENEXIT))
		Clone_GenExit(n,i);
	s=i->it_Properties;
	while(s)
	{
		switch(s->pr_Key)
		{
		case KEY_MSGEXIT:Clone_MsgExit(n,(MSGEXIT *)s);
				 break;
		case KEY_CONTAINER:
				Clone_Container(n,(CONTAINER *)s);
				break;
		case KEY_CHAIN:
				Clone_Chain(n,(CHAIN *)s);
				break;
		case KEY_USERFLAG:
		case KEY_USERFLAG2:
				Clone_UserFlag(n,(USERFLAG *)s);
				break;
		case KEY_INHERIT:
				Clone_Inherit(n,(INHERIT *)s);
				break;
		case KEY_USERTEXT:
				Clone_UserText(i,n);
				break;
		case KEY_INOUTHERE:
				SetInMsg(n,TextOf(((INOUTHERE *)s)->io_InMsg));
				SetOutMsg(n,TextOf(((INOUTHERE *)s)->io_OutMsg));
				SetHereMsg(n,TextOf(((INOUTHERE *)s)->io_HereMsg));
				break;
		case KEY_CONDEXIT:
				MakeCondExit(n,((CONDEXIT *)s)->ce_Dest,
					((CONDEXIT *)s)->ce_Table,
					((CONDEXIT *)s)->ce_ExitNumber);
				break;
		}
		s=s->pr_Next;
	}
	if(!f)
	{
		d=(DUP *)AllocSub(n,KEY_DUPED,sizeof(DUP));
		d->du_Master=i;
		LockItem(i);
	}
	return(n);
}

/*
 *	Check if an item is a copy
 */

int Duped(ITEM *x)
{
	if(FindSub(x,KEY_DUPED))
		return(1);
	return(0);
}

/*
 *	Destroy a copied item totally.
 */

int Disintegrate(ITEM *i)
{
	SUB *s;
	if(!Duped(i))
		return(-1);
	Place(i,NULL);
	while((s=i->it_Properties)!=NULL)
	{
		switch(s->pr_Key)
		{
		case KEY_ROOM:
			UnRoom(i);
			break;
		case KEY_OBJECT:
			UnObject(i);
			break;
		case KEY_PLAYER:
			UnPlayer(i);
			break;
		case KEY_GENEXIT:
			UnGenExit(i);
			break;
		case KEY_MSGEXIT:
			UnMsgExit(i,(MSGEXIT *)s);
			break;
		case KEY_CHAIN:
			RemoveChain(i,((CHAIN *)(s))->ch_Chained);
			break;
		case KEY_USERFLAG:
		case KEY_USERFLAG2:
			UnUserFlag(i);
			break;
		case KEY_CONDEXIT:
			UnCondExit(i,(CONDEXIT *)s);
			break;
		case KEY_CONTAINER:
			UnContainer(i);
			break;
		case KEY_INHERIT:
			UnInherit(i);
			break;
		case KEY_SNOOP:;
		case KEY_SNOOPBACK:
			StopAllSnoops(i);
			StopSnoopsOn(i);
			break;
		case KEY_DUPED:
			UnlockItem(((DUP *)(s))->du_Master);
			FreeSub(i,s);
			break;
		case KEY_USERTEXT:
			UnUserText(i);
			break;
		case KEY_INOUTHERE:
			KillIOH(i);
			break;
			
		default:fprintf(stderr,"SUBID=%d\n",s->pr_Key);
			Error("Disintegrate: Prop Problem");
		}
	}
	KillEventQueue(i);
	if(FreeItem(i)<0)
	{
		i->it_Perception=-1;	/* Queue deletion */
		return(-1);
	}
	if(i==Me())
		SetMe(NULL);
	if(i==Item1)
		Item1=NULL;
	if(i==Item2)
		Item2=NULL;
	return(0);
}

/*
 *	Destroy every copied item in the universe
 */

void DisintegrateAll(void)
{
	ITEM *i=ItemList;
	ITEM *j;
	while(i)
	{
		j=i->it_MasterNext;
		Disintegrate(i);
		i=j;
	}
}
