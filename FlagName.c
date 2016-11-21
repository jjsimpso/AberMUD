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
 *	Controller to handle names of bit flags (player/object etc)
 */

#include "System.h"

Module "BitFlag Names";
Version "1.00";
Author "----*(A)";

/*
 *	1.00	AGC	Initial Release For 5.07
 *	1.01	AGC	Core Dump Cure Time
 *
 */

char *PBitNames[16];
char *OBitNames[16];
char *RBitNames[16];
char *CBitNames[16];

static void ListBitSet(i,l)
ITEM *i;
char **l;
{
	int ct= -1;
	while(++ct<16)
		SendItem(i,"%d\t%s\n",ct,l[ct]?l[ct]:"{unset}");
}


static void SetBitName(i,l)
ITEM *i;
char **l;
{
	int n=GetNumber();
	if(n==-1)
	{
		SendItem(i,"You must specify a bit number.\n");
		return;
	}
	if(n<0||n>15)
	{
		SendItem(i,"Number Out Of Range (0-15)\n");
		return;
	}
	if(GetParsedWord()==NULL)
	{
		SendItem(i,"You must specify a name for the bitflag.\n");
		return;
	}
	if(l[n])
		free(l[n]);
	l[n]=malloc(strlen(WordBuffer)+1);
	if(!l[n])
		Error("SetBitName: Out Of Memory");
	strcpy(l[n],WordBuffer);
}

static char *BitName(l,n)
char **l;
int n;
{
	return(l[n]?l[n]:"{unset}");	/* Note: Several things check for this
						string - be careful if you
						change it at all. */
}

static int WhichBitField(l,x)
char **l;
char *x;
{
	int ct= -1;
	while(++ct<16)
	{
		if(l[ct] &&stricmp(l[ct],x)==0)
			return(ct);
	}
	return(-1);
}

int FindRBit(x)
char *x;
{
	return(WhichBitField(RBitNames,x));
}

int FindPBit(x)
char *x;
{
	return(WhichBitField(PBitNames,x));
}

int FindCBit(x)
char *x;
{
	return(WhichBitField(CBitNames,x));
}

int FindOBit(x)
char *x;
{
	return(WhichBitField(OBitNames,x));
}

char *RBitName(n)
int n;
{
	return(BitName(RBitNames,n));
}

char *PBitName(n)
int n;
{
	return(BitName(PBitNames,n));
}

char *CBitName(n)
int n;
{
	return(BitName(CBitNames,n));
}

char *OBitName(n)
int n;
{
	return(BitName(OBitNames,n));
}

/*
 *	Commands for using these facilities
 */

void Cmd_RBitName(i)
ITEM *i;
{
	SetBitName(i,RBitNames);
}

void Cmd_PBitName(i)
ITEM *i;
{
	SetBitName(i,PBitNames);
}

void Cmd_CBitName(i)
ITEM *i;
{
	SetBitName(i,CBitNames);
}

void Cmd_OBitName(i)
ITEM *i;
{
	SetBitName(i,OBitNames);
}

void Cmd_ListRBits(i)
ITEM *i;
{
	ListBitSet(i,RBitNames);
}

void Cmd_ListPBits(i)
ITEM *i;
{
	ListBitSet(i,PBitNames);
}

void Cmd_ListCBits(i)
ITEM *i;
{
	ListBitSet(i,CBitNames);
}

void Cmd_ListOBits(i)
ITEM *i;
{
	ListBitSet(i,OBitNames);
}
