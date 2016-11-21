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
 *	Flag handlers
 *
 *	1.00	Initial release
 *	1.01	Strict ANSIfication
 */

Module  "Flag Controller";
Version "1.01";
Author  "Alan Cox";

char *FlagName[512];	/* Crude but effective ! */


/*
 *	Given a name, find the flag number
 */

int GetFlagByName(char *x)
{
	short c=0;
	while(c<512)
	{
		if((FlagName[c])&&(stricmp(FlagName[c],x)==0))
			return(c);
		c++;
	}
	return(-1);
}

/*
 *	Set the name of a flag
 */

void SetFlagName(short f, char *x)
{
	if(f<0||f>511)
		return;
	if(FlagName[f]!=NULL)
		free(FlagName[f]);
	if(x==NULL)
	{
		FlagName[f]=NULL;
		return;
	}
	if((FlagName[f]=malloc(strlen(x)+1))==NULL)
		Error("FlagController: No Free Memory.\n");
	strcpy(FlagName[f],x);
}

/*
 *	Get the name of a flag
 */

char *GetFlagName(short f)
{
	static char b[8];
	if((f>=0)&&(f<512)&&(FlagName[f]))
		return(FlagName[f]);
	else
	{
		sprintf(b,"%d",f);
		return(b);
	}
}

/*
 *	The actual flag naming command function
 */

void Cmd_NameFlag(ITEM *i)
{
	short n=GetNumber();
	char *c=WordBuffer;
	if(n==-1)
	{
		SendItem(i,"Which flag number ?\n");
		return;
	}
	GetAll();
	if(!strlen(c))
	{
		SendItem(i,"You must specify a name for the flag.\n");
		return;
	}
	if(*c!='@')
	{
		SendItem(i,"Flags must start with the '@' character.\n");
		return;
	}
	if(GetFlagByName(c)!=-1)
	{
		SendItem(i,"But flag %d is already called '%s'.\n",
			GetFlagByName(c),c);
		return;
	}
	if((n<0)||(n>511))
	{
		SendItem(i,"Flag numbers are from 0-511 inclusive.\n");
		return;
	}
	SetFlagName(n,c);
	SendItem(i,"Flag %d is now called %s.\n",n,c);
}

/*
 *	UnName a flag - command
 */

void Cmd_UnNameFlag(ITEM *i)
{
	short n=GetNumber();
	if(n==-1)
	{
		SendItem(i,"Which flag number ?\n");
		return;
	}
	if((n<0)||(n>511))
	{
		SendItem(i,"Flag numbers are from 0-511 inclusive.\n");
		return;
	}
	SetFlagName(n,NULL);
	SendItem(i,"Flag %d is now un-named.\n",n);
}

/*
 *	The listflag command
 */

void Cmd_ListFlags(ITEM *i)
{
/*
 *	LISTFLAG lists all flags, LISTFLAG @NAME lists flag 'name'
 *	LISTFLAG n lists flag n
 */
	int v;
	GetAll();
	if(!strlen(WordBuffer))
	{
		short c=0;
		while(c<512)
		{
			if(FlagName[c])
				SendItem(i,"%-3d   %s\n",c,GetFlagName(c)+1);
			c++;
		}
		return;
	}
	if(sscanf(WordBuffer,"%d",&v)==0)
	{
		if(*WordBuffer=='@')
		{
			if(GetFlagByName(WordBuffer))
				SendItem(i,"%-3d     %s\n",
					GetFlagByName(WordBuffer),WordBuffer);
			return;
		}
		else
			SendItem(i,"Flag names always start with '@'.\n");
	}
	else
	{
		if(FlagName[v])
		{
			SendItem(i,"%-3d     %s\n",v,GetFlagName((short)v));
		}
	}
}
