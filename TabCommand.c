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

Module  "Table Control Commands";
Version "1.01";
Author  "Alan Cox";

/*
 *	Commands for editing the tables.
 *
 *	1.00	AGC	Original Commands
 *	1.01	AGC 	Support For Named Tables
 */


void Cmd_ListTables(i)
ITEM *i;
{
	TABLE *t=TableList;
	if(!ArchWizard(i))
	{
		SendItem(i,"Two small, one large and a multiplication..\n");
		return;
	}
	while(t)
	{
		SendItem(i,"%d)\t%s\n",t->tb_Number,TextOf(t->tb_Name));
		t=t->tb_Next;
	}
}

void Cmd_AddTable(i)
ITEM *i;
{
	TABLE *t;
	FILE *f;
	int n;
	if(!ArchWizard(i))
	{
		SendItem(i,"Ask a carpenter..\n");
		return;
	}
	n=GetNumber();
	if(n==-1)
	{
		SendItem(i,"Which Table Though ?\n");
		return;
	}
	t=FindTable(n);
	if(t)
	{
		if(t->tb_First)
		{
			SendItem(i,"Table Exists!\n");
			return;
		}
	}
	if(t==NULL)
	{
		if(!GetParsedWord())
		{
			SendItem(i,"You must specify a table name.\n");
			return;
		}
		if(FindTableByName(WordBuffer)!=-1)
		{
			SendItem(i,"Table name already in use.\n");
			return;
		}
		t=NewTable(n,WordBuffer);
	}
	GetAll();
	f=fopen(WordBuffer,"r");
	if(f==NULL)
	{
		SendItem(i,"Can't open file : %s.\n",WordBuffer);
		return;
	}
	EncodeTable(i,t,f);
	fclose(f);
}

void Cmd_DeleteTable(i)
ITEM *i;
{
	TABLE *t;
	int n;
	if(!ArchWizard(i))
	{
		SendItem(i,"Ask a carpenter..\n");
		return;
	}
	if(GetParsedWord()==NULL)
	{
		SendItem(i,"Which Table ?\n");
		return;
	}
	if(isdigit(*WordBuffer))
	{
		sscanf(WordBuffer,"%d",&n);
	}
	else
	{
		n=FindTableByName(WordBuffer);
		if(n==-1)
		{
			SendItem(i,"No Such Table.\n");
			return;
		}
	}
	t=FindTable(n);
	if(t==NULL)
	{
		SendItem(i,"No Such Table.\n");
		return;
	}	
	DeleteTable(t);
}

void Cmd_NewTable(i)
ITEM *i;
{
	TABLE *t;
	int n;
	if(!ArchWizard(i))
	{
		SendItem(i,"Ask a carpenter..\n");
		return;
	}
	n=GetNumber();
	if(n==-1)
	{
		SendItem(i,"Which Table Though ?\n");
		return;
	}
	t=FindTable(n);
	if(t)
	{
		if(t->tb_First)
		{
			SendItem(i,"Table Exists!\n");
			return;
		}
	}
	if(t==NULL)
	{
		if(!GetParsedWord())
		{
			SendItem(i,"You must specify a table name.\n");
			return;
		}
		if(FindTableByName(WordBuffer)!=-1)
		{
			SendItem(i,"Table name already in use.\n");
			return;
		}
		t=NewTable(n,WordBuffer);
	}
}

void Cmd_NameTable(i)
ITEM *i;
{
	TABLE *t;
	int n;
	if(!ArchWizard(i))
	{
		SendItem(i,"I hearby christen this table 'Fred'.\n");
		return;
	}
	n=GetNumber();
	if(n==-1)
	{
		SendItem(i,"Which table though ?\n");
		return;
	}
	t=FindTable(n);
	if(t==NULL)
	{
		SendItem(i,"No such table.\n");
		return;
	}
	if(!GetParsedWord())
	{
		SendItem(i,"You must specify a table name.\n");
		return;
	}
	if(FindTableByName(WordBuffer)!=-1)
	{
		SendItem(i,"Table name already in use.\n");
		return;
	}
	FreeText(t->tb_Name);
	t->tb_Name=AllocText(WordBuffer);
}


