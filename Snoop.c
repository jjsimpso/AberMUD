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
 *	Snoop handlers
 *
 *	1.00	Initial release
 *	1.01	Strict ANSIfication
 */

Module  "Snooping";
Version "1.01";
Author  "----* (A)";

/*
 *	Snoop control functions - full multiple depths 
 *	Limit set at 2	
 */

int StartSnoop(ITEM *from, ITEM *to, short type)
{
	SNOOP *s;
	SNOOPBACK *b;
	if(from==to)
		return(-1);	/* No Self Snoop Please */
	s=(SNOOP *)AllocSub(to,KEY_SNOOP,sizeof(SNOOP));
	b=(SNOOPBACK *)AllocSub(from,KEY_SNOOPBACK,sizeof(SNOOPBACK));
	b->sb_SnoopKey=s;
	s->sn_BackPtr=b;
	b->sb_Snooped=to;
	s->sn_Snooper=from;
	LockItem(from);
	LockItem(to);
	s->sn_String[0]='|';
	s->sn_String[1]=0;
	s->sn_Ident=type;
	return 0;
}

void StopSnoop(ITEM *i, SNOOP *s)
{
	UnlockItem(s->sn_Snooper);
	UnlockItem(i);
	FreeSub(s->sn_Snooper,(SUB *)s->sn_BackPtr);
	FreeSub(i,(SUB *)s);
}

void StopSnoopOn(ITEM *i, ITEM *at)
{
	SNOOPBACK *b;
	b=(SNOOPBACK *)FindSub(i,KEY_SNOOP);
	while(b)
	{
		if(b->sb_Snooped==at)
			StopSnoop(b->sb_Snooped,b->sb_SnoopKey);
		b=(SNOOPBACK *)NextSub((SUB *)b,KEY_SNOOPBACK);
	}
}

void StopAllSnoops(ITEM *i)	/* Stop all snoops by I */
{
	SNOOPBACK *b;
	while((b=(SNOOPBACK *)FindSub(i,KEY_SNOOPBACK))!=NULL)
	{
		StopSnoop(b->sb_Snooped,b->sb_SnoopKey);
	}
}

void StopSnoopsOn(ITEM *x)
{
	SNOOP *s;
	while((s=(SNOOP *)FindSub(x,KEY_SNOOP))!=NULL)
	{
		SendItem(s->sn_Snooper,"You can no longer snoop on %s.\n",
			NameOf(x));
		StopSnoop(x,s);		
	}
}	

void SnoopCheckString(ITEM *i, char *x)
{
	SnoopCheckRec(i,x,0);
}

/*
 *	Primary Snoop Validating Logic
 *
 *	We limit the depth to 10 (CURR 2) layers of recursion -> which you can get esp
 *	if a watches b watches a!
 */

int SnoopCheckRec(ITEM *i, char *x, short d)
{
	short c=d+1;
	SNOOP *s;
	if(d>1)
		return(-1);
	s=(SNOOP *)FindSub(i,KEY_SNOOP);
	while(s)
	{
/*		while(c--)
			SendItemDirect(s->sn_Snooper,s->sn_String);*/
		c=d;
		SendItemDirect(s->sn_Snooper,x);
		SnoopCheckRec(s->sn_Snooper,x,(short)(d+1));
		s=(SNOOP *)NextSub((SUB *)s,KEY_SNOOP);
	}
	return(0);
}
		
