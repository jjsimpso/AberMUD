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

Module "Search Routines";
Version "1.01";
Author "Alan Cox";

/*
 *	1.00	AGC	Initial Version
 *	1.01	AGC	Made to understand bit classes
 */

static void PrintLine(l)
LINE *l;
{
	LINE *t=CurrentLine;
	CurrentLine=l;
	PCurrentLine();
	CurrentLine=t;
}

void CheckLineFor(line,s,st)
LINE *line;
ITEM *s;
int st;
{
	register char *TLine;
	unsigned short *x;
	x=line->li_Data;
	while(*x!=CMD_EOL)
	{
		int n=*x++;
		int ct=0;
		TLine=Cnd_Table[n];
		while(TLine[ct]!=' ')
		{
			ITEM *i;
			TPTR t;
			switch(TLine[ct])
			{
				case 'I':
					i=(ITEM *)PairArg(x);
					x+=2;
					if(s&&i==s)
					{
						PrintLine(line);	
						return;
					}
					break;
				case '$':;
				case 'T':t=(TPTR )PairArg(x);
					 x+=2;
					 break;
				case '3':x++;break;
				case 'F':if(*x++==st)
					 {
					     PrintLine(line);
					     return;	
					 }
					 break;
				case 'N':if(*x++==st+30000&&st!=-1)
					 {
						PrintLine(line);
						return;
					 }
					 break;
				case 'W':;
				case 'w':;	/* THIS IS WRONG FOR NOW */
				case 'Z':;
				case 'B':;
				case 't':;
				case 'p':x++;
					 break;
				case 'v':x++;
					 break;
				case 'a':x++;
					 break;
				case 'n':x++;
					 break;
				case 'C':;
				case 'R':;
				case 'O':;
				case 'P':;
				case 'c':x++;
					 break;
				default:
					Error("Invalid Entry In Cnd_Table");
			}
			ct++;
		}
	}
}

void CheckTableFor(t,s,st)
TABLE *t;
ITEM *s;
int st;
{
	LINE *l=t->tb_First;
	while(l)
	{
		CheckLineFor(l,s,st);
		l=l->li_Next;
	}
}

void CheckAllFor(i,s,st)
ITEM *i,*s;
int st;
{
	TABLE *t=TableList;
	TABLE *p;
	ITEM *tmp=Debugger;
	Debugger=i;
	while(t)
	{
		p=CurrentTable;
		CurrentTable=t;
		SendItem(Debugger,"Checking Table %d\n",t->tb_Number);
		CheckTableFor(t,s,st);	
		t=t->tb_Next;
	}
	Debugger=tmp;
	CurrentTable=p;
}

