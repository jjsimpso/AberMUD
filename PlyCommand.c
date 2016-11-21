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
 *	Commands To Do With Players
 *
 *	1.00	AGC	Original Version
 *	1.01	AGC	Upgraded to 5.06
 *	1.02	AGC	5.07 Version using user defined bitnames
 *      1.03    AGC     Remote Addresses in Users
 *	1.04	AGC	Fixed formatting for >9 users.
 *	1.05	AGC	Added Slot Status Info
 */

#include "System.h"
#include "User.h"

extern USER UserList[];

Module "Player Commands";
Version "1.05";
Author "Alan Cox";



void Cmd_PlayerShow(i)
ITEM *i;
{
	ITEM *a;
	PLAYER *o;
	int ct=0;
	if(!ArchWizard(i))
	{
		SendItem(i,"Try EXAMINE.\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	o=PlayerOf(a);
	if(o==NULL)
	{
		SendItem(i,"%s is not a character.\n",CNameOf(a));
		return;
	}
	SendItem(i,"Character Name : %s\n",CNameOf(a));
	SendItem(i,"UserKey : %d ",o->pl_UserKey);
	SendItem(i,"Size    : %d ",o->pl_Size);
	SendItem(i,"Weight  : %d \n",o->pl_Weight);
	SendItem(i,"Strength: %d ",o->pl_Strength);
	SendItem(i,"Level   : %d ",o->pl_Level);
	SendItem(i,"Score   : %d\n",o->pl_Score);
	ct=0;
	while(ct<16)
	{
		if(o->pl_Flags&(1<<ct))
			SendItem(i,"%s ",PBitName(ct));
		else
			if(strcmp(PBitName(ct),"{unset}"))
				SendItem(i,"-%s ",PBitName(ct));
		ct++;
	}
	SendItem(i,"\n");
}

void Cmd_SetPFlag(i)
ITEM *i;
{
	int c=0;
	ITEM *a;
	PLAYER *o;
	if(!ArchWizard(i))
	{
		SendItem(i,"No can do....\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	o=PlayerOf(a);
	if(o==NULL)
	{
		SendItem(i,"%s is not a character.\n",CNameOf(a));
		return;
	}
	SkipPrep();
	while(GetParsedWord())
	{
		if(!strlen(WordBuffer))
		{
			SendItem(i,"What flag though ?\n");
			return;
		}
		if(*WordBuffer=='-')
		{
			c=FindPBit(WordBuffer+1);
			if(c==-1)
			{
				SendItem(i,"Unknown flag %s.\n",WordBuffer+1);
				return;
			}
			o->pl_Flags&=~(1<<c);
		}
		else
		{
			c=FindPBit(WordBuffer);
			if(c==-1)
			{
				SendItem(i,"Unknown flag %s.\n",WordBuffer);
				return;
			}
			o->pl_Flags|=(1<<c);
		}
	}
}

void Cmd_SetPSize(i)
ITEM *i;
{
	int n;
	ITEM *a;
	PLAYER *p;
	if(!ArchWizard(i))
	{
		SendItem(i,"No!\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	p=PlayerOf(a);
	if(p==NULL)
	{
		SendItem(i,"%s is not a character.\n",CNameOf(a));
		return;
	}
	SkipPrep();
	n=GetNumber();
	if(n==-1)
	{
		SendItem(i,"Bad Size Value.\n");
		return;
	}
	p->pl_Size=n;
}

void Cmd_SetPWeight(i)
ITEM *i;
{
	int n;
	ITEM *a;
	PLAYER *p;
	if(!ArchWizard(i))
	{
		SendItem(i,"No!\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	p=PlayerOf(a);
	if(p==NULL)
	{
		SendItem(i,"%s is not a character.\n",CNameOf(a));
		return;
	}
	SkipPrep();
	n=GetNumber();
	if(n==-1)
	{
		SendItem(i,"Bad Weight Value.\n");
		return;
	}
	p->pl_Weight=n;
}

void Cmd_SetPStrength(i)
ITEM *i;
{
	int n;
	ITEM *a;
	PLAYER *p;
	if(!ArchWizard(i))
	{
		SendItem(i,"No!\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	p=PlayerOf(a);
	if(p==NULL)
	{
		SendItem(i,"%s is not a character.\n",CNameOf(a));
		return;
	}
	SkipPrep();
	n=GetNumber();
	if(n==-1)
	{
		SendItem(i,"Bad Strength Value.\n");
		return;
	}
	p->pl_Strength=n;
}

void Cmd_SetPLevel(i)
ITEM *i;
{
	int n;
	ITEM *a;
	PLAYER *p;
	if(!ArchWizard(i))
	{
		SendItem(i,"No!\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	p=PlayerOf(a);
	if(p==NULL)
	{
		SendItem(i,"%s is not a character.\n",CNameOf(a));
		return;
	}
	SkipPrep();
	n=GetNumber();
	if(n==-1)
	{
		SendItem(i,"Bad Level Value.\n");
		return;
	}
	p->pl_Level=n;
}

void Cmd_SetPScore(i)
ITEM *i;
{
	long n;
	ITEM *a;
	PLAYER *p;
	if(!ArchWizard(i))
	{
		SendItem(i,"No!\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	p=PlayerOf(a);
	if(p==NULL)
	{
		SendItem(i,"%s is not a character.\n",CNameOf(a));
		return;
	}
	SkipPrep();
	n=GetNumber();
	if(n==-1)
	{
		SendItem(i,"Bad Score Value.\n");
		return;
	}
	p->pl_Score=n;
}

void Cmd_Users(i)
ITEM *i;
{
	int a=0;
	int v=GetNumber();
	while(a<MAXUSER)
	{
		if((UserList[a].us_Port)&&
		   (UserList[a].us_Item)&&
		   (CanSee(LevelOf(i),UserList[a].us_Item)))
		{
			if(Arch(i))
			{
				SendItem(i,"%-2d) %-20s %-40s  (%d)\n",a,
					UserList[a].us_Name,
					UserList[a].us_UserName,
					UserList[a].us_State);
			}
			else
				SendItem(i,"%-40s %s\n",UserList[a].us_Name,
					UserList[a].us_UserName);
		}
		else
		{
			if(Arch(i)&&v==1)
			{
				SendItem(i,"%-2d) %-20s %-40s  (%d)\n",a,
					"<free slot>","<unset>",
					 UserList[a].us_State);
			}
			if(Arch(i)&&v==2)
			{
				SendItem(i,"%-2d) %-20s %-40s  (%d)\n",a,
					"<free slot>",isalpha(*UserList[a].us_Name)?UserList[a].us_UserName:"<unset>",
					 UserList[a].us_State);
			}
		}
		a++;
	}
}
