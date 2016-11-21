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
 *	ObjectEdit.c	-   Prompted Object Edit/Create (New For 5.05)
 *
 *		
 *	AGC	1.00		Created This File
 *		1.01		Fixed bug about #2 item, and prevented
 *				crashed if item is unobjected.
 *		1.02		5.07 release version with bit flag names
 *		1.03		Asks for desc 0 - restores prompt correctly
 *				on errors.
 */

#include "System.h"
#include "User.h"

Module  "Object Edit";
Version "1.03";
Author  "Alan Cox";

extern USER UserList[];

static ITEM *O_Item;		/* Thing being edited */

void Cmd_ObjEdit(ITEM *i)
{
	register short ct;
	short u;
	ITEM *j;
	if(!ArchWizard(i))
	{
		SendItem(i,"Eh ?\n");
		return;
	}
	u=UserOf(i);
	if(u==-1)
		return;
	ct=0;
	while(ct<MAXUSER)		/* Stop use by 2 at once */
	{
		if((UserList[ct].us_State>=AWAIT_OEDIT)&&
		   (UserList[ct].us_State<=AWAIT_OEND))
		{
			SendItem(i,"Sorry - Object editing in progress.\n");
			SendUser(ct,"%s tried to use the object editor.\n",
				CNameOf(i));
			return;
		}
		ct++;
	}
	j=FindSomething(i,O_PARENT(i));	
	if(!j)
	{
		SendItem(i,"What is that ?\n");
		return;
	}
	if(!IsObject(j))
	{
		SendItem(i,"%s is not an object.\n",CNameOf(j));
		return;
	}
	LockItem(j);
	SetPrompt(i,"NAME>");
	SendEdit(i,"%s",NameOf(j));
	O_Item=j;
	UserList[u].us_State=AWAIT_OE1;
}	

#define UOF(x)		(UserList[x].us_Item)
#define UST(x,y)	(UserList[x].us_State=(y))
#define UBUMP(x)	(UserList[x].us_State++)

void Objedit_1(short u, char *t)
{
	ITEM *i=UOF(u);
	if(strlen(t))
	{
		SetName(O_Item,t);
	}
	SetPrompt(i,"STATE>");
	SendEdit(i,"%d",O_STATE(O_Item));
	UBUMP(u);
	PermitInput(u);
}

void Objedit_2(short u, char *t)
{
	ITEM *i=UOF(u);
	int v;
	if(sscanf(t,"%d",&v))
	{
		if((v<0)||(v>3))
		{
			SendUser(u,"State out of range.\n");
			SetPrompt(i,"-}--- ");
			PermitInput(u);
			return;
		}
		SetState(O_Item,(short)v);
	}
	if(!IsObject(O_Item))
	{
		SendItem(i,"%s is no longer an object!\n",CNameOf(O_Item));
		UserList[u].us_State=AWAIT_COMMAND;
		UnlockItem(O_Item);
		SetPrompt(i,"-}--- ");
		PermitInput(u);
		return;
	}
	SetPrompt(i,"EXAMINE>");
	SendEdit(i,"%s",TextOf(ObjectOf(O_Item)->ob_Text[3]));
	UBUMP(u);
	PermitInput(u);
}

void Objedit_3(short u, char *t)
{
	ITEM *i=UOF(u);
	if(!IsObject(O_Item))
	{
		SendItem(i,"%s is no longer an object!\n",CNameOf(O_Item));
		UserList[u].us_State=AWAIT_COMMAND;
		SetPrompt(i,"-}--- ");
		PermitInput(u);
		UnlockItem(O_Item);
		return;
	}
	if(strlen(t))
	{
		FreeText(ObjectOf(O_Item)->ob_Text[3]);
		ObjectOf(O_Item)->ob_Text[3]=AllocText(t);
	}
	SetPrompt(i,"LOCATION>");
	if(O_PARENT(O_Item))
		SendEdit(i,"%s",CNameOf(O_PARENT(O_Item)));
	else
		SendEdit(i,"NONE");
	UBUMP(u);
	PermitInput(u);
}

void Objedit_4(short u, char *t)
{
	ITEM *i=UOF(u);
	ITEM *j;
	if(!IsObject(O_Item))
	{
		SendItem(i,"%s is no longer an object!\n",CNameOf(O_Item));
		UserList[u].us_State=AWAIT_COMMAND;
		SetPrompt(i,"-}--- ");
		PermitInput(u);
		UnlockItem(O_Item);
		return;
	}
	if(strlen(t))
	{
		if(stricmp(t,"NONE")==0)
		{
			Place(O_Item,NULL);
		}
		else
		{
			WordPtr=t;
			j=FindSomething(i,O_PARENT(i));
			if(!j)
			{
				SendUser(u,"Unknown Location.\n");
				SetPrompt(i,"-}--- ");
				PermitInput(u);
				return;
			}
			Place(O_Item,j);
		}
	}
	SetPrompt(i,"SIZE>");
	SendEdit(i,"%d",ObjectOf(O_Item)->ob_Size);
	UBUMP(u);
	PermitInput(u);
}


void Objedit_5(short u, char *t)
{
	ITEM *i=UOF(u);
	int v;
	if(!IsObject(O_Item))
	{
		SendItem(i,"%s is no longer an object!\n",CNameOf(O_Item));
		UserList[u].us_State=AWAIT_COMMAND;
		SetPrompt(i,"-}--- ");
		PermitInput(u);
		UnlockItem(O_Item);
		return;
	}
	if(sscanf(t,"%d",&v))
	{
		ObjectOf(O_Item)->ob_Size=v;
	}
	SetPrompt(i,"WEIGHT>");
	SendEdit(i,"%d",ObjectOf(O_Item)->ob_Weight);
	UBUMP(u);
	PermitInput(u);
}

char *FlagText(ITEM *i)
{
	static char buf[512];
	int ct=0;
	OBJECT *o=ObjectOf(i);
	*buf=0;
	while(ct<16)
	{
		if(o->ob_Flags&(1<<ct))
			strcat(buf,OBitName(ct));
		else
			if(strcmp(OBitName(ct),"{unset}"))
			{
				strcat(buf,"-");
				strcat(buf,OBitName(ct));
			}
		strcat(buf," ");
		ct++;
	}
	return(buf);
}

void Objedit_6(short u, char *t)
{
	ITEM *i=UOF(u);
	int v;
	if(!IsObject(O_Item))
	{
		SendItem(i,"%s is no longer an object!\n",CNameOf(O_Item));
		UserList[u].us_State=AWAIT_COMMAND;
		SetPrompt(i,"-}--- ");
		PermitInput(u);
		UnlockItem(O_Item);
		return;
	}
	if(sscanf(t,"%d",&v))
	{
		ObjectOf(O_Item)->ob_Weight=v;
	}
	SetPrompt(i,"FLAGS>");
	SendEdit(i,"%s",FlagText(O_Item));
	UBUMP(u);
	PermitInput(u);
}



void Objedit_7(short u, char *t)
{
	char bf[300];
	ITEM *i=UOF(u);
	if(strlen(t))
	{
		sprintf(bf,"#%d %s",ItemNumber(LevelOf(i),O_Item),
				CNameOf(O_Item));
		strcat(bf," ");
		strcat(bf,t);
		WordPtr=bf;
		Cmd_SetOFlag(i);
	}
	UBUMP(u);
	SetPrompt(i,"Description 0>");
	PermitInput(u);
}


void Objedit_8(short u, char *t)
{
	OBJECT *o;
	ITEM *i=UOF(u);
	if(!IsObject(O_Item))
	{
		SendItem(i,"%s is no longer an object!\n",CNameOf(O_Item));
		UserList[u].us_State=AWAIT_COMMAND;
		SetPrompt(i,"-}--- ");
		PermitInput(u);
		UnlockItem(O_Item);
		return;
	}
	o=ObjectOf(O_Item);
	UST(u,AWAIT_COMMAND);
	if(*t)
	{
		FreeText(o->ob_Text[0]);
		o->ob_Text[0]=AllocText(t);
	}
	UnlockItem(O_Item);
	SetPrompt(i,"-}--- ");
	PermitInput(u);
}


