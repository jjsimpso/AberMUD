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
 *	Daemon Actions
 *
 *
 *	1.00	AGC	Original Release
 *	1.10	AGC	Added TreeDaemon and ChainDaemons
 *	1.11	AGC	Added CDaemon
 *	1.12	AGC	Private Daemon Table Handling
 *
 */

#include "System.h"
#include "User.h"
extern USER UserList[];


Module  "Daemons";
Version "1.12";
Author  "Alan Cox";

/*
 *	Actually carry out a DAEMON or part thereof
 */

void RunDaemon(i,v,n1,n2)
ITEM *i;
int v,n1,n2;
{
	int vsv=Verb,n1sv=Noun1,n2sv=Noun2;	/* Stack the words */
	Verb=v;			/* Set word to DAEMON words */
	Noun1=n1;
	Noun2=n2;
	UserDaemon(i);		/* Run table - see tabledriver.c */
	Verb=vsv;
	Noun1=n1sv;
	Noun2=n2sv;	/* Restore them */
}

/*	NOTE: Daemons are defined to always execute before anything else
 *	that may be pending. They can be nested to a degree depending on
 *	system stack - Stack Overflow will result if this occurs
 *	{$} is defined to remain constant through all daemons.
 *	Any item may be the root of a Daemon action, and becomes Me() for
 *	that duration.
 */

/*	Daemon Drivers: There are three current group drivers:
 *	HDAEMON,DAEMON,ALLDAEMON
 *	HDAEMON and ALLDAEMON affect ONLY Users!, this is forced by the
 *	CPU power of the system, which would virtually die on a literal
 *	ALLDAEMON of a big game. Like earlier AberMuds this doesn't affect
 *	attached mobiles, as these become Users while attached. It does
 *	however affect snooped items. An identical affect will be observed
 *	by DoesAction() and similar calls. Later releases of the kernel may
 *	make snooped items users, and redesign the UserList to be expanding
 *
 * 1.1:	New Daemons TREEDAEMON, affects all contents of item, and CHAINDAEMON
 *	affects all chained items of item, but not item itself. TREEDAEMON
 *	affects only users.
 */

/*
 *	A DAEMON to every user in the game
 */
void AllDaemon(v,n1,n2)
int v,n1,n2;
{
	int ct=0;
	while(ct<MAXUSER)
	{
		if((UserList[ct].us_Port)&&(UserList[ct].us_Item))
		{
			RunDaemon(UserList[ct].us_Item,v,n1,n2);
		}
		ct++;
	}
}

/*
 *	A DAEMON to every user present
 */
void HDaemon(i,v,n1,n2)
ITEM *i;
int v,n1,n2;
{
	int ct=0;
	while(ct<MAXUSER)
	{
		if((UserList[ct].us_Port)&&(UserList[ct].us_Item))
		{
			if(O_PARENT(UserList[ct].us_Item)==O_PARENT(i))
				RunDaemon(UserList[ct].us_Item,v,n1,n2);
		}
		ct++;
	}
}

/*
 *	DAEMON to everything within an item (every USER)
 */

void TreeDaemon(i,v,n1,n2)
ITEM *i;
int v,n1,n2;
{
	int ct=0;
	while(ct<MAXUSER)
	{
		if((UserList[ct].us_Port)&&(UserList[ct].us_Item))
		{
			if(Contains(i,UserList[ct].us_Item))
				RunDaemon(UserList[ct].us_Item,v,n1,n2);
		}
		ct++;
	}
}

/*
 *	Daemon to everything chained to current item
 */

void ChainDaemon(i,v,n1,n2)
ITEM *i;
int v,n1,n2;
{
	CHAIN *c=(CHAIN *)FindSub(i,KEY_CHAIN);
	while(c)
	{
		RunDaemon(c->ch_Chained,v,n1,n2);
		c=(CHAIN *)NextSub((SUB *)c,KEY_CHAIN);
	}
}

void CDaemon(i,v,n1,n2)
ITEM *i;
int v,n1,n2;
{
	int ct=0;
	while(ct<MAXUSER)
	{
		if((UserList[ct].us_Port)&&(UserList[ct].us_Item))
		{
			if(O_PARENT(UserList[ct].us_Item)==i)
				RunDaemon(UserList[ct].us_Item,v,n1,n2);
		}
		ct++;
	}
}

