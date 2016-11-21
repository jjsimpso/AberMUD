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
 *	Commands To Do With Containers
 *
 *	1.00		AGC		Original Release
 *	1.01		AGC		Forgot to make it use NameCFlag type stuff - only noticed for 5.21.4
 */

#include "System.h"

Module "Container Commands";
Version "1.01";
Author "----*(A)";

void Cmd_ContainerShow(i)
ITEM *i;
{
	ITEM *a;
	CONTAINER *c;
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
	c=ContainerOf(a);
	if(c==NULL)
	{
		SendItem(i,"%s is not a container.\n",CNameOf(a));
		return;
	}
	SendItem(i,"Container Name : %s\n",CNameOf(a));
	SendItem(i,"Volume: %d\n",c->co_Volume);

	while(ct<16)
	{
		if(c->co_Flags&(1<<ct))
			SendItem(i,"%s ",CBitName(ct));
		else
			if(strcmp(CBitName(ct),"{unset}"))
				SendItem(i,"-%s ",CBitName(ct));
		ct++;
	}
	SendItem(i,"\n");
}

void Cmd_SetCFlag(i)
ITEM *i;
{
	ITEM *a;
	CONTAINER *c;
	int flag;
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
	c=ContainerOf(a);
	if(c==NULL)
	{
		SendItem(i,"%s is not a container.\n",CNameOf(a));
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
			flag=FindCBit(WordBuffer+1);
			if(flag==-1)
			{
				SendItem(i,"Unknown flag %s.\n",WordBuffer+1);
				return;
			}
			c->co_Flags&=~(1<<flag);
		}
		else
		{
			flag=FindCBit(WordBuffer);
			if(flag==-1)
			{
				SendItem(i,"Unknown flag %s.\n",WordBuffer);
				return;
			}
			c->co_Flags|=(1<<flag);

		}
	}
}

void Cmd_SetVolume(i)
ITEM *i;
{
	int n;
	ITEM *a;
	CONTAINER *c;
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
	c=ContainerOf(a);
	if(c==NULL)
	{
		SendItem(i,"%s is not a container.\n",CNameOf(a));
		return;
	}
	SkipPrep();
	n=GetNumber();
	if(n==-1)
	{
		SendItem(i,"Bad Volume Value.\n");
		return;
	}
	c->co_Volume=n;
}


