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
 *	Substructure handlers
 *
 *	1.00	Handlers For ROOM PLAYER OBJECT
 *	1.02	Exit Handlers Added For GENEXIT
 *	1.03	Message And Conditional Exit Drivers Added
 *	1.10	New SubTypes CONTAINER and CHAIN
 *	1.12	BACKTRACK System Added - Avoids Players Sticking On UserItems
 *	1.13	Fixed bug in UnGenExit
 *	1.14	Added INHERIT
 *	1.15    Fixed bug in UnUserFlag
 *	1.16	Register Optimisations Added
 *	1.17	SetIn SetOut SetHere added
 *	1.18	BACKTRACK system dumped
 *	1.19	USERTEXT added
 *	1.20	Tidying for release 5.06
 *	1.21	Added USERFLAG2 Entries, Added working CondExit system(at last)
 *	1.22	Strict ANSIfication
 */

#include "System.h"

Module "Substructure Handler";
Version "1.22";
Author "----*(A)";


int MakeRoom(ITEM *x)
{
	register ROOM *a;
	if(FindSub(x,KEY_ROOM))
		return(-1);	/* Already is */
	a=(ROOM *)AllocSub(x,KEY_ROOM,sizeof(ROOM));
	a->rm_Short=AllocText("Room");
	a->rm_Long=AllocText("Description");
	a->rm_Flags=0;
	return(0);
}

int UnRoom(ITEM *i)
{
	register ROOM *x=(ROOM *)FindSub(i,KEY_ROOM);
	if(x==NULL)
		return(-1);
	FreeText(x->rm_Short);
	FreeText(x->rm_Long);
	FreeSub(i,(SUB *)x);
	return(0);
}

int MakeObject(ITEM *x)
{
	register OBJECT *a;
	if(FindSub(x,KEY_OBJECT))
		return(-1);	/* Already is */
	a=(OBJECT *)AllocSub(x,KEY_OBJECT,sizeof(OBJECT));
	a->ob_Text[0]=AllocText("<unset>");
	a->ob_Text[1]=AllocText("");
	a->ob_Text[2]=AllocText("");
	a->ob_Text[3]=AllocText("");
	a->ob_Size=0;
	a->ob_Weight=0;
	a->ob_Flags=0;
	return(0);
}

int UnObject(ITEM *i)
{
	register OBJECT *x=(OBJECT *)FindSub(i,KEY_OBJECT);
	if(x==NULL)
		return(-1);
	FreeText(x->ob_Text[0]);
	FreeText(x->ob_Text[1]);
	FreeText(x->ob_Text[2]);
	FreeText(x->ob_Text[3]);
	FreeSub(i,(SUB *)x);
	return(0);
}

int MakePlayer(ITEM *x)
{
	register PLAYER *a;
	if(FindSub(x,KEY_PLAYER))
		return(-1);	/* Already is */
	a=(PLAYER *)AllocSub(x,KEY_PLAYER,sizeof(PLAYER));
	a->pl_UserKey=-1;
	a->pl_Size=0;
	a->pl_Weight=0;
	a->pl_Flags=0;
	a->pl_Level=0;
	a->pl_Score=0;
	a->pl_Strength=0;
	return(0);
}

int UnPlayer(ITEM *i)
{
	register PLAYER *x=(PLAYER *)FindSub(i,KEY_PLAYER);
	if(x==NULL)
		return(-1);
	FreeSub(i,(SUB *)x);
	return(0);
}

int MakeGenExit(ITEM *x)
{
	register GENEXIT *a;
	if(FindSub(x,KEY_GENEXIT))	/* Only one genexit a room */
		return(-1);	/* Already is */
	a=(GENEXIT *)AllocSub(x,KEY_GENEXIT,sizeof(GENEXIT));
	a->ge_Dest[0]=0;
	a->ge_Dest[1]=0;
	a->ge_Dest[2]=0;
	a->ge_Dest[3]=0;
	a->ge_Dest[4]=0;
	a->ge_Dest[5]=0;
	a->ge_Dest[6]=0;
	a->ge_Dest[7]=0;
	a->ge_Dest[8]=0;
	a->ge_Dest[9]=0;
	a->ge_Dest[10]=0;
	a->ge_Dest[11]=0;
	return(0);
}

int UnGenExit(ITEM *i)
{
	register short ct=0;
	register GENEXIT *x=(GENEXIT *)FindSub(i,KEY_GENEXIT);
	if(x==NULL)
		return(-1);
	while(ct<12)
	{
		if(x->ge_Dest[ct])
			UnlockItem(x->ge_Dest[ct]); /* Exits should be locked on add */
		ct++;
	}
	FreeSub(i,(SUB *)x);
	return(0);
}

CONDEXIT *MakeCondExit(ITEM *x, ITEM *to, short table, short exit)
{
	register CONDEXIT *a;
	a=(CONDEXIT *)FindCondExit(x,exit);
	if(a)
		UnCondExit(x,a);
	a=(CONDEXIT *)AllocSub(x,KEY_CONDEXIT,sizeof(CONDEXIT));
	a->ce_ExitNumber=exit;
	a->ce_Table=table;
	a->ce_Dest=to;
	if(to)
		LockItem(to);	/* Lock since referred to */
	return(a);
}

CONDEXIT *MakeNLCondExit(ITEM *x, ITEM *to, short table, short exit)
{
	register CONDEXIT *a;
	a=(CONDEXIT *)FindCondExit(x,exit);
	if(a)
		UnCondExit(x,a);
	a=(CONDEXIT *)AllocSub(x,KEY_CONDEXIT,sizeof(CONDEXIT));
	a->ce_ExitNumber=exit;
	a->ce_Table=table;
	a->ce_Dest=to;
	return(a);
}

int UnCondExit(ITEM *i, register CONDEXIT *x)	/* Sub to give which one to do*/
{
	if(x->ce_Dest!=NULL)
		UnlockItem(x->ce_Dest);
	FreeSub(i,(SUB *)x);
	return(0);
}

CONDEXIT *FindCondExit(ITEM *i, short d)
{
	register CONDEXIT *a=(CONDEXIT *)FindSub(i,KEY_CONDEXIT);
	while(a)
	{
		if(a->ce_ExitNumber==d)
			return(a);
		a=(CONDEXIT *)NextSub((SUB *)a,KEY_CONDEXIT);
	}
	return(NULL);
}

MSGEXIT *MakeMsgExit(ITEM *x, ITEM *to, short d, char *msg)
{
	register MSGEXIT *a;
	a=FindMsgExit(x,d);
	if(a)
		UnMsgExit(x,a);
	a=(MSGEXIT *)AllocSub(x,KEY_MSGEXIT,sizeof(MSGEXIT));
	a->me_Dest=to;
	if(to)
		LockItem(a->me_Dest);
	a->me_ExitNumber=d;
	a->me_Text=AllocText(msg);
	return(a);
}

MSGEXIT *MakeNLMsgExit(ITEM *x, ITEM *to, short d, char *msg)
{
	register MSGEXIT *a;
	a=FindMsgExit(x,d);
	if(a)
		UnMsgExit(x,a);
	a=(MSGEXIT *)AllocSub(x,KEY_MSGEXIT,sizeof(MSGEXIT));
	a->me_Dest=to;
	a->me_ExitNumber=d;
	a->me_Text=AllocText(msg);
	return(a);
}

int UnMsgExit(ITEM *i, register MSGEXIT *x)
{
	if(x->me_Dest)
		UnlockItem(x->me_Dest);
	FreeText(x->me_Text);
	FreeSub(i,(SUB *)x);
	return(0);
}

MSGEXIT *FindMsgExit(ITEM *i, short d)
{
	register MSGEXIT *a=(MSGEXIT *)FindSub(i,KEY_MSGEXIT);
	while(a)
	{
		if(a->me_ExitNumber==d)
			return(a);
		a=(MSGEXIT *)NextSub((SUB *)a,KEY_MSGEXIT);
	}
	return(NULL);
}

int AddChain(ITEM *i, ITEM *t)
{
	register CHAIN *c;
	if(FindChain(i,t))
		return(1);
	c=(CHAIN *)AllocSub(i,KEY_CHAIN,sizeof(CHAIN));
	c->ch_Chained=t;
	LockItem(t);
	return(1);
}

int AddNLChain(ITEM *i, ITEM *t)
{
	register CHAIN *c;
	if(FindChain(i,t))
		return(1);
	c=(CHAIN *)AllocSub(i,KEY_CHAIN,sizeof(CHAIN));
	c->ch_Chained=t;
	LockItem(t);
	return(1);
}

CHAIN *FindChain(ITEM *i, ITEM *x)
{
	register CHAIN *c;
	c=(CHAIN *)FindSub(i,KEY_CHAIN);
	while(c)
	{
		if(c->ch_Chained==x)
			return(c);
		c=(CHAIN *)NextSub((SUB *)c,KEY_CHAIN);
	}
	return(NULL);
}

int RemoveChain(ITEM *i, register ITEM *x)
{
	register CHAIN *c;
	c=FindChain(i,x);
	if(c==NULL)
		return(-1);
	else
	{
		UnlockItem(x);
		FreeSub(i,(SUB *)c);
	}
	return(0);
}
	

void SynchChain(ITEM *i)
{
	register CHAIN *c;
	c=(CHAIN *)FindSub(i,KEY_CHAIN);
	while(c)
	{
		SetState(c->ch_Chained,O_STATE(i));
		c=(CHAIN *)NextSub((SUB *)c,KEY_CHAIN);
	}
	return;
}

CONTAINER *BeContainer(ITEM *x)
{
	register CONTAINER *c=(CONTAINER *)FindSub(x,KEY_CONTAINER);
	if(c)
		return(NULL);
	c=(CONTAINER *)AllocSub(x,KEY_CONTAINER,sizeof(CONTAINER));
	c->co_Volume=0;
	c->co_Flags=0;
	return(c);
}

int UnContainer(ITEM *x)
{	
	CONTAINER *c;
	if((c=(CONTAINER *)FindSub(x,KEY_CONTAINER))==NULL)
		return(-1);
	FreeSub(x,(SUB *)c);
	return(0);
}

int GetUserFlag(ITEM *i, int n)
{
	USERFLAG *x;
	if(n<8)
		x=(USERFLAG *)FindSub(i,KEY_USERFLAG);
	else
		x=(USERFLAG *)FindSub(i,KEY_USERFLAG2);
	if(x==NULL)
		return(0);
	if((n<0)||(n>15))
	{
		Log("Userflag out of range");
		return(0);
	}
	return(x->uf_Flags[(short)(n%8)]);
}

ITEM *GetUserItem(ITEM *i, int n)
{
	USERFLAG *x;
	if(n<8)
		x=(USERFLAG *)FindSub(i,KEY_USERFLAG);
	else
		x=(USERFLAG *)FindSub(i,KEY_USERFLAG2);
	if(x==NULL)
		return(0);
	if((n<0)||(n>15))
	{
		Log("UserItem out of range");
		return(0);
	}
	return(x->uf_Items[(short)(n%8)]);
}

void SetUserFlag(ITEM *i, int n, int m)
{
	register USERFLAG *x;
	if(n<8)
		x=(USERFLAG *)FindSub(i,KEY_USERFLAG);
	else
		x=(USERFLAG *)FindSub(i,KEY_USERFLAG2);
	if(x==NULL)
	{
		x=(USERFLAG *)AllocSub(i,n<8?KEY_USERFLAG:KEY_USERFLAG2,sizeof(USERFLAG));
		InitUserFlag(x);
	}
	if((n<0)||(n>15))
	{
		Log("Userflag out of range");
		return;
	}
	x->uf_Flags[n%8]=m;
}

void SetUserItem(ITEM *i, int n, ITEM *m)
{
	register USERFLAG *x;
	if(n<8)
		x=(USERFLAG *)FindSub(i,KEY_USERFLAG);
	else
		x=(USERFLAG *)FindSub(i,KEY_USERFLAG2);
	if(x==NULL)
	{
		x=(USERFLAG *)AllocSub(i,n<8?KEY_USERFLAG:KEY_USERFLAG2,
				sizeof(USERFLAG));
		InitUserFlag(x);
	}
	if((n<0)||(n>15))
	{
		Log("UserItem out of range");
		return;
	}
	if(x->uf_Items[n%8])
	{
		UnlockItem(x->uf_Items[n%8]);
	}
	if(m)
	{
		LockItem(m);
	}
	x->uf_Items[n%8]=m;
}

int UnUserFlag(ITEM *x)
{
	register USERFLAG *u=(USERFLAG *)FindSub(x,KEY_USERFLAG);
	UnUserBlock(u,x);
	u=(USERFLAG *)FindSub(x,KEY_USERFLAG2);
	if(u==NULL)
		return(-1);
	UnUserBlock(u,x);
	return(0);
}


int UnUserBlock(USERFLAG *u, ITEM *x)
{
	register short ct=0;
	if(u==NULL)
		return(-1);
	while(ct<8)
	{
		if(u->uf_Items[ct])
		{
			UnlockItem(u->uf_Items[ct]);
		}
		ct++;
	}
	FreeSub(x,(SUB *)u);
	return(0);
}

void InitUserFlag(register USERFLAG *x)
{
	x->uf_Items[0]=NULL;
	x->uf_Items[1]=NULL;
	x->uf_Items[2]=NULL;
	x->uf_Items[3]=NULL;
	x->uf_Items[4]=NULL;
	x->uf_Items[5]=NULL;
	x->uf_Items[6]=NULL;
	x->uf_Items[7]=NULL;
	x->uf_Flags[0]=0;
	x->uf_Flags[1]=0;
	x->uf_Flags[2]=0;
	x->uf_Flags[3]=0;
	x->uf_Flags[4]=0;
	x->uf_Flags[5]=0;
	x->uf_Flags[6]=0;
	x->uf_Flags[7]=0;
}

int MakeInherit(ITEM *x, ITEM *y)
{
	register INHERIT *a;
	if((a=(INHERIT *)FindSub(x,KEY_INHERIT))!=NULL)
	{
		a->in_Master=y;
		return(0); /* Already is */
	}
	a=(INHERIT *)AllocSub(x,KEY_INHERIT,sizeof(INHERIT));
	a->in_Master=y;
	LockItem(y);
	return(0);
}

int UnInherit(ITEM *i)
{
	register INHERIT *x=(INHERIT *)FindSub(i,KEY_INHERIT);
	if(x==NULL)
		return(-1);
	UnlockItem(x->in_Master);
	FreeSub(i,(SUB *)x);
	return(0);
}

ITEM *Inheritor(ITEM *a)
{
	INHERIT *b;
	b=(INHERIT *)FindSub(a,KEY_INHERIT);
	if(b==NULL)
		return(NULL);
	return(b->in_Master);
}


static USERTEXT *GetUT(ITEM *i)
{
	register USERTEXT *u=(USERTEXT *)FindSub(i,KEY_USERTEXT);
	if(u)
		return(u);
	u=(USERTEXT *)AllocSub(i,KEY_USERTEXT,sizeof(USERTEXT));
	u->ut_Text[0]=AllocText("");
	u->ut_Text[1]=AllocText("");
	u->ut_Text[2]=AllocText("");
	u->ut_Text[3]=AllocText("");
	u->ut_Text[4]=AllocText("");
	u->ut_Text[5]=AllocText("");
	u->ut_Text[6]=AllocText("");
	u->ut_Text[7]=AllocText("");
	return(u);
}

void UnUserText(ITEM *i)
{
	USERTEXT *u=GetUT(i);
	if(u)
	{
		FreeText(u->ut_Text[0]);
		FreeText(u->ut_Text[1]);
		FreeText(u->ut_Text[2]);
		FreeText(u->ut_Text[3]);
		FreeText(u->ut_Text[4]);
		FreeText(u->ut_Text[5]);
		FreeText(u->ut_Text[6]);
		FreeText(u->ut_Text[7]);
		FreeSub(i,(SUB *)u);
	}
}
		
void SetUText(ITEM *i, int n, TPTR t)
{
	USERTEXT *u=GetUT(i);
	FreeText(u->ut_Text[n]);
	u->ut_Text[n]=AllocText(TextOf(t));
}	

TPTR GetUText(ITEM *i, int n)
{
	return(GetUT(i)->ut_Text[n]);
}

INOUTHERE *FindIOH(ITEM *i)
{
	return((INOUTHERE *)FindSub(i,KEY_INOUTHERE));
}

void KillIOH(ITEM *i)
{
	SUB *s=FindSub(i,KEY_INOUTHERE);
	if(s)
		FreeSub(i,s);
}

INOUTHERE *GetIOH(ITEM *i)
{
	register INOUTHERE *r=(INOUTHERE *)FindSub(i,KEY_INOUTHERE);
	if(r)
		return(r);
	r=(INOUTHERE *)AllocSub(i,KEY_INOUTHERE,sizeof(INOUTHERE));
	r->io_InMsg=AllocText("arrives");
	r->io_OutMsg=AllocText("goes");
	r->io_HereMsg=AllocText("is here");
	return(r);
}

void SetInMsg(ITEM *it, char *x)
{
	INOUTHERE *i=GetIOH(it);
	FreeText(i->io_InMsg);
	i->io_InMsg=AllocText(x);
}

void SetOutMsg(ITEM *it, char *x)
{
	INOUTHERE *i=GetIOH(it);
	FreeText(i->io_OutMsg);
	i->io_OutMsg=AllocText(x);
}

void SetHereMsg(ITEM *it, char *x)
{
	INOUTHERE *i=GetIOH(it);
	FreeText(i->io_HereMsg);
	i->io_HereMsg=AllocText(x);
}

char *GetInMsg(ITEM *it)
{
	INOUTHERE *i=FindIOH(it);
	if(i)
		return(TextOf(i->io_InMsg));
	else
		return("arrives");
}

char *GetOutMsg(ITEM *it)
{
	INOUTHERE *i=FindIOH(it);
	if(i)
		return(TextOf(i->io_OutMsg));
	else
		return("goes");
}

char *GetHereMsg(ITEM *it)
{
	INOUTHERE *i=FindIOH(it);
	if(i)
		return(TextOf(i->io_HereMsg));
	else
		return("is here");
}

