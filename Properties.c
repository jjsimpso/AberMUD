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
 *	Arbitary item properties
 */

#include "System.h"		/* System includes and options */

Module "Properties";
Version "1.00";
Author "----*(A)";

/*
 *	1.00	AGC	Initial test version
 */

PropTable *PropertyList;

void FreeAllProperties(i)
ITEM *i;
{
	SHORTPROPLIST *sp=(SHORTPROPLIST *)FindSub(i,KEY_SHORTPROPLIST);
	ITEMPROPLIST *ip=(ITEMPROPLIST *)FindSub(i,KEY_ITEMPROPLIST);
	TEXTPROPLIST *tp=(TEXTPROPLIST *)FindSub(i,KEY_TEXTPROPLIST);
	if(sp!=NULL)
	{
		free(sp->Data);
		FreeSub(i,sp);
	}
	if(ip!=NULL)
	{
		free(ip->Data);
		FreeSub(i,ip);
	}
	if(tp!=NULL)
	{
		free(tp->Data);
		FreeSub(i,tp);
	}
}
	
SHORTPROPLIST *GetShortPropPointer(i)
ITEM *i;
{
	SHORTPROPLIST *sp=(SHORTPROPLIST *)FindSub(i,KEY_SHORTPROPLIST);
	if(sp==NULL)
	{
		sp=(SHORTPROPLIST *)AllocSub(i,KEY_SHORTPROPLIST,sizeof(SHORTPROPLIST));
		sp->Size=4;	/* Start with 4 then move on */
		sp->Used=0;
		sp->Data=(SHORTPROPLIST *)malloc(sizeof(SHORTPROPLIST)*sp->Size);
		if(sp->Data==NULL)
			Error("Out of memory allocating a shortprop");
	}
	return(sp);
}

TEXTPROPLIST *GetTextPropPointer(i)
ITEM *i;
{
	TEXTPROPLIST *sp=(TEXTPROPLIST *)FindSub(i,KEY_TEXTPROPLIST);
	if(sp==NULL)
	{
		sp=(TEXTPROPLIST *)AllocSub(i,KEY_TEXTPROPLIST,sizeof(TEXTPROPLIST));
		sp->Size=4;	/* Start with 4 then move on */
		sp->Used=0;
		sp->Data=(TEXTPROPLIST *)malloc(sizeof(TEXTPROPLIST)*sp->Size);
		if(sp->Data==NULL)
			Error("Out of memory allocating a textprop");
	}
	return(sp);
}

ITEMPROPLIST *GetItemPropPointer(i)
ITEM *i;
{
	ITEMPROPLIST *sp=(ITEMPROPLIST *)FindSub(i,KEY_ITEMPROPLIST);
	if(sp==NULL)
	{
		sp=(ITEMPROPLIST *)AllocSub(i,KEY_ITEMPROPLIST,sizeof(ITEMPROPLIST));
		sp->Size=4;	/* Start with 4 then move on */
		sp->Used=0;
		sp->Data=(ITEMPROPLIST *)malloc(sizeof(ITEMPROPLIST)*sp->Size);
		if(sp->Data==NULL)
			Error("Out of memory allocating an itemprop");
	}
	return(sp);
}


void SetShortProp(i,p,n)
ITEM *i;
short p;
short n;
{
	SHORTPROPLIST *sp=GetShortPropPointer(i);
	int ct=0;
	while(ct<sp->sp_Used)
	{
		if(sp->Data[ct].ps_Ident==p)
		{
			sp->Data[ct].ps_Data=n;
			return;
		}
		ct++;
	}
	if(sp->sp_Used<sp->sp_Size)
	{
		sp->Data[sp->sp_Used].ps_Ident=p;
		sp->Data[sp->sp_Used++].ps_Data=n;
		LockPropertyNumber(p);
		return;
	}
	sp->sp_Size*=2;
	sp->sp_Data=(SHORTPROPLIST *)realloc(sp->sp_Data,sizeof(SHORTPROPLIST)*sp->sp_Size);
	if(sp->sp_Data==NULL)
		Error("Out of memory growing shortproplist");
	sp->Data[sp->sp_Used].ps_Ident=p;
	sp->Data[sp->sp_Used++].ps_Data=n;
	LockPropertyNumber(p);
}

void SetItemProp(i,p,n)
ITEM *i;
short p;
short n;
{
	ITEMPROPLIST *sp=GetItemPropPointer(i);
	int ct=0;
	while(ct<sp->sp_Used)
	{
		if(sp->Data[ct].ps_Ident==p)
		{
			sp->Data[ct].ps_Data=n;
			return;
		}
		ct++;
	}
	if(sp->sp_Used<sp->sp_Size)
	{
		sp->Data[sp->sp_Used].ps_Ident=p;
		sp->Data[sp->sp_Used++].ps_Data=n;
		LockPropertyNumber(p);
		return;
	}
	sp->sp_Size*=2;
	sp->sp_Data=(ITEMPROPLIST *)realloc(sp->sp_Data,sizeof(ITEMPROPLIST)*sp->sp_Size);
	if(sp->sp_Data==NULL)
		Error("Out of memory growing itemproplist");
	sp->Data[sp->sp_Used].ps_Ident=p;
	sp->Data[sp->sp_Used++].ps_Data=n;
	LockPropertyNumber(p);
}

void SetTextProp(i,p,n)
ITEM *i;
short p;
short n;
{
	TEXTPROPLIST *sp=GetTextPropPointer(i);
	int ct=0;
	while(ct<sp->sp_Used)
	{
		if(sp->Data[ct].ps_Ident==p)
		{
			sp->Data[ct].ps_Data=n;
			return;
		}
		ct++;
	}
	if(sp->sp_Used<sp->sp_Size)
	{
		sp->Data[sp->sp_Used].ps_Ident=p;
		sp->Data[sp->sp_Used++].ps_Data=n;
		LockPropertyNumber(p);
		return;
	}
	sp->sp_Size*=2;
	sp->sp_Data=(TEXTPROPLIST *)realloc(sp->sp_Data,sizeof(TEXTPROPLIST)*sp->sp_Size);
	if(sp->sp_Data==NULL)
		Error("Out of memory growing textproplist");
	sp->Data[sp->sp_Used].ps_Ident=p;
	sp->Data[sp->sp_Used++].ps_Data=n;
	LockPropertyNumber(p);
}

