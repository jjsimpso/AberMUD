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
 *	Class groupings
 *
 *	1.00	Initial release
 *	1.01	Strict ANSIfication
 */

Module  "Classes";
Version "1.01";
Author  "AGC";


static TXT *ClassNames[16];

void SetClassTxt(short c, TXT *x)
{
	if(ClassNames[c])
		FreeText(ClassNames[c]);
	if(x)
		ClassNames[c]=x;
	else
		ClassNames[c]=NULL;
}

void SetClassName(short c, char *x)
{
	if(ClassNames[c])
		FreeText(ClassNames[c]);
	if(x)
		ClassNames[c]=AllocText(x);
	else
		ClassNames[c]=NULL;
}

TXT *GetClassTxt(short c)
{
	return(ClassNames[c]);
}

char *GetClassName(short c)
{
	if(c==-1)
		return("0");		/* NO CLASSES */
	if(ClassNames[c])
		return(TextOf(ClassNames[c]));
	else
		return("<UNSET>");
}

short WhichClass(char *x)
{
	short ct=0;	
	while(ct<16)
	{
		if(ClassNames[ct])
			if(stricmp(TextOf(ClassNames[ct]),x)==0)
				return(ct);
		ct++;
	}
	return(-1);
}


void ClassDescStr(ITEM *i, short f)
{
	short ct=0;
	while(ct<16)
	{
		if(f&1)
		{
			SendItem(i,"%s ",GetClassName(ct));
		}
		f>>=1;
		ct++;
	}
}

/*
 *	Commands for the above
 */

void Cmd_ListClass(ITEM *i)
{
	short c=0;
	if(!ArchWizard(i))
	{
		SendItem(i,"What ?\n");
		return;
	}
	while(c<16)
	{
		SendItem(i,"%2d) %s\n",(int)c,GetClassName(c));
		c++;
	}
}

void Cmd_NameClass(ITEM *i)
{
	short c;
	if(!ArchWizard(i))
	{
		SendItem(i,"What ?\n");
		return;
	}
	if((c=GetNumber())==-1)
	{
		SendItem(i,"Which class number.\n");
		return;
	}
	if((c<0)||(c>15))
	{
		SendItem(i,"Class numbers are 0-15.\n");
		return;
	}
	GetAll();
	if(strlen(WordBuffer))
		SetClassName(c,WordBuffer);
	else
		SetClassName(c,NULL);
}

void Cmd_SetClass(ITEM *i)
{
	ITEM *j;
	short f=0;
	short c=0;
	if(!ArchWizard(i))
	{
		SendItem(i,"What ?\n");
		return;
	}
	j=FindSomething(i,O_PARENT(i));
	if(j==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	f=j->it_Class;
	while(GetParsedWord())
	{
		c=WhichClass(WordBuffer);
		if(c==-1)
		{
			SendItem(i,"Unknown class %s.\n",WordBuffer);
			return;
		}
		f|=1<<c;
	}
	j->it_Class=f;
}

void Cmd_UnsetClass(ITEM *i)
{
	ITEM *j;
	short f=0;
	short c=0;
	if(!ArchWizard(i))
	{
		SendItem(i,"What ?\n");
		return;
	}
	j=FindSomething(i,O_PARENT(i));
	if(j==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	f=j->it_Class;
	while(GetParsedWord())
	{
		c=WhichClass(WordBuffer);
		if(c==-1)
		{
			SendItem(i,"Unknown class %s.\n",WordBuffer);
			return;
		}
		f&=~(1<<c);
	}
	j->it_Class=f;
}
