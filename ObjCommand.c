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
 *	Commands To Do With Objects
 *
 *	1.00	AGC	Original Version
 *	1.01	AGC	Updated to 5.06
 *	1.02	AGC	5.07 user named bit flags added
 */

#include "System.h"

Module "Object Commands";
Version "1.02";
Author "----*(A)";

/* Changed From ShowObject for 8 char significance */


void Cmd_ObjectShow(i)
ITEM *i;
{
	int ct=0;
	ITEM *a;
	OBJECT *o;
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
	o=ObjectOf(a);
	if(o==NULL)
	{
		SendItem(i,"%s is not an object.\n",CNameOf(a));
		return;
	}
	SendItem(i,"Object Name : %s\n",CNameOf(a));
	SendItem(i,"Text(0): %s\n",TextOf(o->ob_Text[0]));
	SendItem(i,"Text(1): %s\n",TextOf(o->ob_Text[1]));
	SendItem(i,"Text(2): %s\n",TextOf(o->ob_Text[2]));
	SendItem(i,"Text(3): %s\n",TextOf(o->ob_Text[3]));
	SendItem(i,"Size   : %d     Weight : %d\n",
		o->ob_Size,o->ob_Weight);
	while(ct<16)
	{
		if(o->ob_Flags&(1<<ct))
			SendItem(i,"%s ",OBitName(ct));
		else
			if(strcmp(OBitName(ct),"{unset}"))
				SendItem(i,"-%s ",OBitName(ct));
		ct++;
	}
	SendItem(i,"\n");
}

void Cmd_SetOFlag(i)
ITEM *i;
{
	int c=0;
	ITEM *a;
	OBJECT *o;
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
	o=ObjectOf(a);
	if(o==NULL)
	{
		SendItem(i,"%s is not an object.\n",CNameOf(a));
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
			c=FindOBit(WordBuffer+1);
			if(c==-1)
			{
				SendItem(i,"Unknown flag %s.\n",WordBuffer+1);
				return;
			}
			o->ob_Flags&=~(1<<c);
		}
		else
		{
			c=FindOBit(WordBuffer);
			if(c==-1)
			{
				SendItem(i,"Unknown flag %s.\n",WordBuffer);
				return;
			}
			o->ob_Flags|=(1<<c);
		}

	}
}

void Cmd_SetDesc(i)
ITEM *i;
{
	int siz;
	int n;
	ITEM *a;
	OBJECT *o;
	char *BigBuffer;
	FILE *f;
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
	o=ObjectOf(a);
	if(o==NULL)
	{
		SendItem(i,"%s is not an object.\n",CNameOf(a));
		return;
	}
	SkipPrep();
	n=GetNumber();
	if(n==-1)
	{
		SendItem(i,"Which description number.\n");
		return;
	}
	if((n<0)||(n>3))
	{
		SendItem(i,"Description out of range.\n");
		return;
	}
	GetAll();
	if(!strlen(WordBuffer))
	{
		SendEdit(i,"SetDesc #%d %s %d %s",ItemNumber(LevelOf(i),a),
			CNameOf(a),(int)n,TextOf(o->ob_Text[n]));
		return;
	}
	if(*WordBuffer=='*')
	{
		f=fopen(WordBuffer+1,"r");
		if(f==NULL)
		{
			SendItem(i,"Sorry - can't open the file.\n");
			return;
		}
		BigBuffer=malloc(2048);
		if(BigBuffer==NULL)
		{
			SendItem(i,"Not enough memory.\n");
			return;
		}
		if((siz=fread(BigBuffer,1,2048,f))==2048)
		{
			fclose(f);
			free(BigBuffer);
			SendItem(i,"Entry too large.\n");
			return;
		}
		fclose(f);
		if(BigBuffer[siz-1]=='\n')
			BigBuffer[siz-1]=0;
		BigBuffer[siz]=0;	/* Convert data to C string */
		BigBuffer[2047]=0;	/* protection */
		FreeText(o->ob_Text[n]);
		o->ob_Text[n]=AllocText(BigBuffer);
		free(BigBuffer);
	}
	else
	{
		FreeText(o->ob_Text[n]);
		o->ob_Text[n]=AllocText(WordBuffer);
	}
}

void Cmd_SetOSize(i)
ITEM *i;
{
	int n;
	ITEM *a;
	OBJECT *o;
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
	o=ObjectOf(a);
	if(o==NULL)
	{
		SendItem(i,"%s is not an object.\n",CNameOf(a));
		return;
	}
	SkipPrep();
	n=GetNumber();
	if(n==-1)
	{
		SendItem(i,"Bad Size Value.\n");
		return;
	}
	o->ob_Size=n;
}

void Cmd_SetOWeight(i)
ITEM *i;
{
	int n;
	ITEM *a;
	OBJECT *o;
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
	o=ObjectOf(a);
	if(o==NULL)
	{
		SendItem(i,"%s is not an object.\n",CNameOf(a));
		return;
	}
	SkipPrep();
	n=GetNumber();
	if(n==-1)
	{
		SendItem(i,"Bad Weight Value.\n");
		return;
	}
	o->ob_Weight=n;
}

void Cmd_SetActor(i)
ITEM *i;
{
	ITEM *a;
	int n;
	if(!ArchWizard(i))
	{
		SendItem(i,"Pardon ?\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	SkipPrep();
	if(GetParsedWord()==NULL)
	{
		SendItem(i,"Which table ?\n");
		return;
	}
	if(isdigit(*WordBuffer))
		sscanf(WordBuffer,"%d",&n);
	else
		n=FindTableByName(WordBuffer);
	if(n==-1)
	{
		SendItem(i,"No Such Table.\n");
		return;
	}
	a->it_ActorTable=n;
}


void Cmd_SetAction(i)
ITEM *i;
{
	ITEM *a;
	int n;
	if(!ArchWizard(i))
	{
		SendItem(i,"Pardon ?\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	SkipPrep();
	if(GetParsedWord()==NULL)
	{
		SendItem(i,"Which table ?\n");
		return;
	}
	if(isdigit(*WordBuffer))
		sscanf(WordBuffer,"%d",&n);
	else
		n=FindTableByName(WordBuffer);
	if(n==-1)
	{
		SendItem(i,"No Such Table.\n");
		return;
	}
	a->it_ActionTable=n;
}

