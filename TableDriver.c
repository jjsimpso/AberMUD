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
#include <string.h>

Module "Table Drivers";
Version "1.18";
Author "Alan Cox";

TABLE *TableList=NULL;		/* Linked list of tables */
TPTR TxtArg=NULL;		/* {$} */
TPTR TxtArg2=NULL;		/* {$2} */
char TxBuf[512];		/* Working buffer */
#ifdef BOOT_DEBUG
int System_Debug=1;		/* Decide if debugging starts ON */
#else
int System_Debug=0;
#endif
static short FlagData[512];	/* The flags - change number here and in GetFlag
				   SetFlag, if desired. */
short Traf[4]={-1,-1,-1,-1};	/* four flags max being tracked */

static unsigned short *DataPointer;	/* Where we are in table  */
LINE *CurrentLine;			/* Line we are executing  */
TABLE *CurrentTable;			/* Table we are executing */
ITEM *Item1,*Item2;			/* $1 and $2 data values  */
int Noun1,Adj1,Noun2,Adj2,Verb,Prep;	/* The word codes needed  */
int Ord1,Ord2;
ITEM *Debugger;				/* Dest for flag traps    */

/*
 *	1.00	AGC	Created
 *	1.01-1.05 AGC	Added lots of action calls
 *	1.06	AGC	Added DAEMON entry points
 *	1.07	AGC	Added background executes
 *	1.08	AGC	Misc Bug Fixes and added stack protection
 *	1.09	AGC	Next batch of commands added
 *	1.10	AGC	Commands added in the right place this time
 *	1.11	AGC	Fixed debugging crashes	(NOT RIGHT STILL) and mended NOT
 *	1.12	AGC	Allowed DAEMON etc to nest DOCLASS calls.
 *	1.13	AGC	Made END and KILLOFF do DONE so they don't crash(as often)
 *	1.14	AGC	Added Flag Tracking Code
 *	1.15	AGC	Added Ordinate Code
 *	1.16	AGC	Fixed UArgNum 512 byte error areas problem.
 *			Also added DEBUG 2 - second level debugging
 *	1.17	AGC	5.08 changes for item bound tables
 *	1.18	AGC	Fixed stupid bugs assuming top bit of address was 0
 *	1.19	AGC	Private Daemon Table. Superclasses
 */

void WipeFlags()
{
	short ct=0;
	while(ct<512)
		FlagData[ct++]=0;	/* Clear all the flags */
}

int GetFlag(n)
int n;
{
	if(n<0||n>=512)
	{
		Log("Flag Out Of Range");
		SendItem(Me(),"Flag Out Of Range [%d].\n",n);
		return(-1);
	}
	return((int)FlagData[n]);	/* Get the value of a flag */
}

void PCurrentLine()
{
	char x[512];
	if(Debugger&&CurrentLine)
	{
		Decompress(CurrentLine,x);	/* Print it to DEBUG */
		SendItem(Debugger,"%d:%s:%s\n",(int)CurrentTable->tb_Number,
			CNameOf(Me()),x);
	}
}

void FPCurrentLine()
{
	char x[512];
	if(Debugger&&CurrentLine)
	{
		Decompress(CurrentLine,x);	/* Print it to DEBUG */
		fprintf(stderr,"%d:%s:%s\n",(int)CurrentTable->tb_Number,
			CNameOf(Me()),x);
	}
}

void SetFlag(n,m)			/* Set the value of a flag */
int n,m;
{
	if((n<0)||(n>511))
	{
		Log("Flag Out Of Range");
		SendItem(Me(),"Flag Out Of Range [%d].\n",n);
		return;
	}
	if(Traf[0]==n)
		if(Debugger)
		{
			SendItem(Debugger,"Flag %d -> %d\n",n,m);
			PCurrentLine();
		}
	if(Traf[1]==n)
		if(Debugger)
		{
			SendItem(Debugger,"Flag %d -> %d\n",n,m);
			PCurrentLine();
		}
	if(Traf[2]==n)
		if(Debugger)
		{
			SendItem(Debugger,"Flag %d -> %d\n",n,m);
			PCurrentLine();
		}
	if(Traf[3]==n)
		if(Debugger)
		{
			SendItem(Debugger,"Flag %d -> %d\n",n,m);
			PCurrentLine();
		}
	FlagData[n]=m;
}

static ITEM *CurrentPlayer;		/* $ME */
static ITEM *ActorPlayer;		/* $AC */

/*
 *	A fake item which does the set up...
 */
TXT DummyTxt={"<GLOBAL DUMMY>",-1,NULL};
ITEM DummyItem={NULL,NULL,NULL,NULL,-1,-1,-1,-1,0,0,0,0,NULL,NULL,NULL,NULL,NULL,NULL};
	
ITEM *Me()
{
	DummyItem.it_Name=&DummyTxt;	/* Set up item */
	if(CurrentPlayer==NULL)
		return(&DummyItem);	/* No player so is dummy */
	return(CurrentPlayer);		/* Else is player */
}

void SetMe(i)			/* Set current player ($ME) */
ITEM *i;
{
	CurrentPlayer=i;
}

ITEM *Actor()
{
	DummyItem.it_Name=&DummyTxt;
	if(ActorPlayer==NULL)
		return(&DummyItem);
	return(ActorPlayer);
}


int ArgNum()		/* Read numeric (number/flag) argument from line */
{
	int a=((int)*DataPointer++);
	if((a>=30000)&&(a<30512))
		return(GetFlag(a-30000));
	else
		return(a);
}

unsigned int UArgNum()		/* Read word, unmodified from text */
{
	unsigned int a=((unsigned int)(*DataPointer++));
	return(a);
}

int ArgWord()		/* Read word from line */
{
	return((int)(*DataPointer++));
}

ITEM *ArgItem()		/* Read item from line */
{
	ITEM *x=(ITEM *)PairArg(DataPointer);
	DataPointer+=2;
	if(x==(ITEM *)1)
		x=Item1;	/* $1 */
	if(x==(ITEM *)3)	/* $2 */
		x=Item2;	
	if(x==(ITEM *)5)	/* $ME */
		x=Me();
	if(x==(ITEM *)7)	/* $AC */
		x=Actor();
	if(x==(ITEM *)9)	/* $RM */
		x=O_PARENT(Me());
	if(x==NULL)
	{
		Broadcast("Invalid Item Reference: -> Using ME as dummy.\n",0);
		FPCurrentLine();
		Log("Invalid Item Reference: -> Using ME as dummy.\n");
		return(Me());
	}
	return(x);
}

TPTR ArgText()		/* Read a text argument */
{
	TPTR x=(TPTR)PairArg(DataPointer);
	DataPointer+=2;
	if(x==(TPTR )1)
	{
		x=TxtArg;		/* {$} */
		WordPtr=NULL;
	}
	if(x==(TPTR )3)
	{
		x=TxtArg2;		/* {$2} */
		WordPtr=NULL;
	}
	return(x);
}

void ParseArgs(v)		/* Break input using parser */
int v;
{
	Verb=v;
	GetRestOfInput(WordPtr,TxBuf);
	if(TxtArg)
		FreeText(TxtArg);	/* Free any old {$} */
	while(*WordPtr)
		if(isspace(*WordPtr))
			WordPtr++;
		else
			break;
	TxtArg=AllocText(WordPtr);	/* Set new {$} */
	Ord1=GetOrd();
	if(GetThing(&Adj1,&Noun1)==-1)
	{
		Adj1=-1;		/* Parse arg 1 */
		Noun1=-1;
	}
	if(TxtArg2)			/* Delete old and create new {$2} */
		FreeText(TxtArg2);
	while(*WordPtr)
		if(isspace(*WordPtr))
			WordPtr++;
		else
			break;
	TxtArg2=AllocText(WordPtr);
	Prep=GetPrep();
	Ord2=GetOrd();
	if(GetThing(&Adj2,&Noun2)==-1)
	{
		Adj2=-1;		/* Parse arg 2 */
		Noun2=-1;
	}
	Item1=FindAnItem(Adj1,Noun1,Ord1);	/* Find $1 and $2 */
	if(Item1)
		SetItData(-1,Item1,(short)Adj1,(short)Noun1);
	Item2=FindAnItem(Adj2,Noun2,Ord2);
	if(Item2)
		SetItData(-1,Item2,(short)Adj2,(short)Noun2);
}

#define WORDMATCH(a,b)	(((a)==-1)||((a)==(b))||(((a)==-2)&&((b)==-1)))

int ArgMatch(l)		/* Decide if line should be run */
LINE *l;
{
	if(!WORDMATCH(l->li_Verb,Verb))
		return(0);
	if(!WORDMATCH(l->li_Noun1,Noun1))
		return(0);
	if(!WORDMATCH(l->li_Noun2,Noun2))
		return(0);
	return(1);
}

int ExecBackground(t,i)		/* Run a table in background (WHEN) */
TABLE *t;
ITEM *i;
{
	int n;
	ITEM *p=CurrentPlayer;
	CurrentPlayer=i;
	n=ExecTable(t);
	CurrentPlayer=p;
	return(n);
}

int ExecTable(t)		/* Run a table */
TABLE *t;
{
	static short TCount=0;
	short lcm1=ClassMode1,lcm2=ClassMode2;	/* Lots of junk to save   */
	register LINE *lcl=ClassLine;			/* For recursions PROCESS */
	short lcmsk=ClassMask;
	int stat=-1;
	LINE *l=t->tb_First;			/* First line to run */
	TABLE *ot=CurrentTable;
	LINE *ol=CurrentLine;
	unsigned short *op=DataPointer;
	ITEM *lfp=GetNextPointer();		/* Stack DOCLASS search info */
	ClassMode1=0;
	ClassMode2=0;
	ClassLine=NULL;
	ClassMask=0;	/* Clear DOCLASS prev */
	TCount++;
	if(TCount>40)	/* Stack Max */
	{
		TCount=0;
		SendItem(Me(),"STACK OVERFLOW: KERNEL RESET\n");
		printf("STACK OVERFLOW: KERNEL RESET\n");
		longjmp(Oops,1);		/* Escape to top level ! */
	}
	CurrentTable=t;
l1:	while(l)			/* For each line */
	{
		CurrentLine=l;
		if(ArgMatch(l))		/* If it should be run */
		{
			if(System_Debug)
			{
				static char x[512];
				Decompress(l,x);	/* Print it to DEBUG */
				printf("%d:%s:%s\n",(int)t->tb_Number,
							CNameOf(Me()),
							x);
			}
			stat=0;
			DataPointer=l->li_Data;
			if((stat=RunLine(l))!=0)	/* Run the line */
				break;
		}
		l=l->li_Next;
	}
	if(ClassMode1)		/* Check for DOCLASS logic */
	{
		Item1=NextInByClass(LevelOf(Me()),Item1,ClassMask);
		if(!Item1)
		{
			ClassMode1=0;
		}
		else
		{
			l=ClassLine;	/* Rescanner */
			goto l1;
		}
	}
	if(ClassMode2)
	{
		Item2=NextInByClass(LevelOf(Me()),Item2,ClassMask);
		if(!Item2)
		{
			ClassMode2=0;
		}
		else
		{
			l=ClassLine;	/* Rescanner */
			goto l1;
		}
	}
	if(stat==-10)		/* stat -10 return means rescan table */
	{
		l=t->tb_First;
		goto l1;
	}
	DataPointer=op;		/* Put it all back */
	CurrentLine=ol;
	CurrentTable=ot;
	ClassMode1=lcm1;
	ClassMode2=lcm2;
	ClassLine=lcl;
	ClassMask=lcmsk;
	TCount--;
	SetNextPointer(lfp);	/* Restore DOCLASS search context */
	return(stat);		/* Return answer */
}


int RunLine(x)			/* Execute a line of database */
register LINE *x;
{
	register int c,v,neg;
	while((c=ArgNum())!=CMD_EOL)	/* End marker ? */
	{
		neg=0;
		if(c==203)		/* NOT */
		{
			neg=1;		/* Note in NOT mode */
			c=ArgNum();
			if(c==CMD_EOL)	/* Trap NOT as last command! */
				break;
		}
		v=1;
		if(System_Debug==2)
			printf("Action: %s\n",Cnd_Table[c]);
/* Apple A/UX can't handle big switch statements (sigh)
   oh for a real compiler */
		if(c<200) {
		switch(c)	/* Run correct condition/action */
		{
/*
 *			BASIC CONDITIONS
 */
			case 0:v=Cnd_At();break;
			case 1:v=Cnd_NotAt();break;
			case 2:v=Cnd_Present();break;
			case 3:v=Cnd_Absent();break;
			case 4:v=Cnd_Worn();break;
			case 5:v=Cnd_NotWorn();break;
			case 6:v=Cnd_Carried();break;
			case 7:v=Cnd_NotCarr();break;
			case 8:v=Cnd_IsAt();break;
			case 9:v=Cnd_IsNotAt();break;
			case 10:v=Cnd_IsBy();break;
			case 11:v=Cnd_IsNotBy();break;
			case 12:v=Cnd_Zero();break;
			case 13:v=Cnd_NotZero();break;
			case 14:v=Cnd_Eq();break;
			case 15:v=Cnd_NotEq();break;
			case 16:v=Cnd_Gt();break;
			case 17:v=Cnd_Lt();break;
			case 18:v=Cnd_EqF();break;
			case 19:v=Cnd_NeF();break;
			case 20:v=Cnd_LtF();break;
			case 21:v=Cnd_GtF();break;
			case 22:v=Cnd_IsIn();break;
			case 23:v=Cnd_IsNotIn();break;
			case 24:v=Cnd_Adj1();break;
			case 25:v=Cnd_Adj2();break;
			case 26:v=Cnd_Noun1();break;
			case 27:v=Cnd_Noun2();break;
			case 28:v=Cnd_Prep();break;
			case 29:v=Cnd_Chance();break;
			case 30:v=Cnd_IsPlayer();break;
			case 31:v=Cnd_IsUser();break;
			case 32:v=Cnd_IsRoom();break;
			case 33:v=Cnd_IsObject();break;
			case 34:v=Cnd_State();break;
			case 35:v=Cnd_PFlag();break;
			case 36:v=Cnd_OFlag();break;
			case 37:v=Cnd_CanPut();break;
			case 38:v=Cnd_RFlag();break;
			case 39:v=Cnd_Level();break;
			case 40:v=Cnd_IfDeaf();break;
			case 41:v=Cnd_IfBlind();break;
			case 42:v=Cnd_Arch();break;
/*
 *		MAIN ACTION LIST
 */
			case 43:Act_Get();break;
			case 44:Act_Drop();break;
			case 45:Act_Remove();break;
			case 46:Act_Wear();break;
			case 47:Act_Create();break;
			case 48:Act_Destroy();break;
			case 49:Act_PutO();break;	
			case 50:Act_Swap();break;
			case 51:Act_Place();break;
			case 52:Act_PutIn();break;
			case 53:Act_TakeOut();break;
			case 54:Act_CopyOF();break;
			case 55:Act_CopyFO();break;
			case 56:Act_CopyFF();break;
			case 57:Act_WhatO();break;
			case 58:Act_GetO();break;
			case 59:Act_Weigh();break;
			case 60:Act_Set();break;
			case 61:Act_Clear();break;
			case 62:Act_PSet();break;
			case 63:Act_PClear();break;
			case 64:Act_Let();break;
			case 65:Act_Add();break;
			case 66:Act_Sub();break;
			case 67:Act_AddF();break;
			case 68:Act_SubF();break;
			case 69:Act_Mul();break;
			case 70:Act_Div();break;
			case 71:Act_MulF();break;
			case 72:Act_DivF();break;
			case 73:Act_Mod();break;
			case 74:Act_ModF();break;
			case 75:Act_Random();break;
			case 76:Act_Move();break;
			case 77:Act_Goto();break;
			case 78:Act_Weight();break;
			case 79:Act_Size();break;
			case 80:Act_OSet();break;
			case 81:Act_OClear();break;
			case 82:Act_RSet();break;
			case 83:Act_RClear();break;
			case 84:Act_PutBy();break;
			case 85:Act_Inc();break;
			case 86:Act_Dec();break;
			case 87:Act_SetState();break;
			case 88:Act_Prompt();break;
			case 89:Act_Print();break;
			case 90:Act_Score();break;
			case 91:Act_Message();break;
			case 92:Act_Msg();break;
			case 93:Act_ListObj();break;
			case 94:Act_ListAt();break;
			case 95:Act_Inven();break;
			case 96:Act_Desc();break;
			case 97:Act_End();return(1);
			case 98:return(1);		/* DONE */
			case 99:return(-1);		/* NOTDONE */
			case 100:Act_Ok();return(1);
			case 101:Act_Abort();break;
			case 102:Act_Save();break;
			case 103:Act_Parse();break;
			case 104:Act_NewText();break;
			case 105:Act_Process();break;
			case 106:Act_DoClass();break;
			case 107:Act_Give();break;
			case 108:Act_SetUText();break;
			case 109:Act_DoesAction();break;
			case 110:Act_DoesTo();break;
			case 111:Act_DoesToPlayer();break;
			case 112:Act_PObj();break;
			case 113:Act_PLoc();break;
			case 114:Act_PName();break;
			case 115:Act_PCName();break;
			case 116:Act_Daemon();break;
			case 117:Act_AllDaemon();break;
			case 118:Act_HDaemon();break;
			case 119:Act_When();break;
			case 120:Act_SetName();break;
			case 121:Act_Dup();break;
			case 122:Act_Frig();break;
			case 123:Act_Points();break;
			case 124:Act_Hurt();break;
			case 125:Act_Cured();break;
			case 126:Act_KillOff();return(1);
			case 127:Act_AutoVerb();break;
			case 128:v=Act_If1();break;
			case 129:v=Act_If2();break;
			case 130:Act_Bug();break;
			case 131:Act_Typo();break;
			case 132:Act_NArg();break;
			case 133:v=Act_IsMe();break;
			case 134:Act_Broadcast();break;
			case 135:v=Cnd_IsCalled();break;
			case 136:v=Cnd_Is();break;
			case 137:Act_SetMe();break;
			case 138:Act_Pronouns();break;
			case 139:v=Cnd_ChanceLev();break;
			case 140:Act_Exits();break;
			case 141:Act_PWChange();break;
			case 142:Act_Snoop();break;
			case 143:Act_UnSnoop();break;
			case 144:Act_GetUText();break;
			case 145:Act_Cat();break;
			case 146:Act_Become();break;
			case 147:Act_Alias();break;
			case 148:Act_UnAlias();break;
			case 149:Act_Field();break;
			case 150:Act_NeedField();break;
			case 151:Act_Unveil();break;
			case 152:Act_Debug();break;
			case 153:Act_GetScore();break;
			case 154:Act_GetStr();break;
			case 155:Act_GetLev();break;
			case 156:Act_SetScore();break;
			case 157:Act_SetLev();break;
			case 158:Act_SetStr();break;
			case 159:Act_Shell();break;
			case 160:Act_CSet();break;
			case 161:Act_CClear();break;
			case 162:v=Cnd_CFlag();break;
			case 163:v=Cnd_CanSee();break;
			case 164:return(-10);
			case 165:Act_Means();break;
			case 166:Act_TreeDaemon();break;
			case 167:Act_SetIn();break;
			case 168:Act_SetOut();break;
			case 169:Act_SetHere();break;
			case 170:Act_CanGoto();break;
/*
 *		Must re-write these one day  -  but are they needed ?
 */
			case 171:Act_Mobiles();break;
			case 172:Act_Dir();break;
			case 173:Act_Rooms();break;
/*
 *		ITE/EXIT MANIPULATORS
 */
			case 174:Act_ChainDaemon();break;
			case 175:Act_CanGoBy();break;
			case 176:Act_SetIFlag();break;
			case 177:Act_GetIFlag();break;
			case 178:Act_ClearIFlag();break;
			case 180:Act_WhereTo();break;
			case 181:Act_DoorExit();break;
/*
 *		ROPE LOGIC - NOT YET REWRITTEN
 */
			case 182:v=Cnd_CanMoveRope();break;
			case 183:Act_PlaceRope();break;
			case 184:v=Cnd_IsRope();break;
			case 185:v=Cnd_IsTied();break;
			case 186:Act_RopePrev();break;
			case 187:Act_RopeNext();break;
			case 188:Act_Tie();break;
			case 189:Act_Untie();break;
			case 190:Act_Join();break;
			case 191:Act_CutRope();break;
			case 192:Act_Distance();break;
			case 193:Act_WhichWay();break;
/*
 *		ASSORTED ADDITIONS
 */
			case 194:v=Cnd_ClassAt();break;
			case 195:v=Cnd_DupOf();break;
			case 196:Act_MasterOf();break;
			case 197:Act_TiedTo();break;
			case 198:Act_Comment();break;
			case 199:Act_ComVocab();break;
		}
	} else {  /* Sigh.. */
		switch(c){
			case 200:Act_Command();break;
			case 201:Act_BSXScene();break;
			case 202:Act_BSXObject();break;
			case 204:v=Cnd_IfDark();break;
			case 205:Act_Visibility();break;
			case 206:Act_GetParent();break;
			case 207:Act_GetNext();break;
			case 208:Act_GetChild();break;
			case 209:Act_PExit();break;
			case 210:Act_SetDesc();break;
			case 211:Act_SetLong();break;
			case 212:Act_SetShort();break;
			case 213:Act_GetLong();break;
			case 214:Act_GetShort();break;
			case 215:Act_GetDesc();break;
			case 216:Act_GetName();break;
			case 217:Act_Swat();break;
			case 218:Act_Flat();break;
			case 219:Act_FindMaster();break;
			case 220:Act_NextMaster();break;
			case 221:Act_FindIn();break;
			case 222:Act_NextIn();break;
			case 223:Act_LenText();break;
/*
 *	5.21 extensions for superclasses
 */
			case 224:v=Cnd_ProcSubject();break;
			case 225:v=Cnd_ProcObject();break;
			case 226:v=Cnd_ProcDaemon();break;
			case 227:v=Cnd_GetSuper();break;
			case 228:Act_SetSuper();break;
			case 229:v=Cnd_Member();break;
/*
 *		Blizzad.uni uses this function, so we have to implement it
 */
			case 231:Act_Cls();break;

/*
 *		Aber5 extensions for better game control
 *		variable saving etc.
 */
			case 243:v=Cnd_IsClass();break;
			case 244:v=Cnd_SubStr();break;
			case 245:Act_GetIn();break;
			case 246:Act_GetOut();break;
			case 247:Act_GetHere();break;
			case 248:Act_Log();break;
			case 249:Act_SetClass();break;
			case 250:Act_UnSetClass();break;
			case 251:Act_BitClear();break;
			case 252:Act_BitSet();break;
			case 253:v=Cnd_BitTest();break;
			case 254:Act_SPrint();break;
			case 255:Act_User();break;
			case 256:Act_SetI();break;
			case 257:Act_CDaemon();break;
			case 258:v=Cnd_Delete();break;
			case 259:v=Cnd_ULoad();break;
			case 260:v=Cnd_USave();break;
			case 261:v=Cnd_FLoad();break;
			case 262:v=Cnd_FSave();break;
			case 263:Act_Getvis();break;
			case 264:Act_MessageTo();break;
			case 265:Act_MsgTo();break;
			case 266:Act_RwhoDeclareUp();break;
			case 267:Act_RwhoDeclareDown();break;
			case 268:Act_RwhoLogin();break;
			case 269:Act_RwhoLogout();break;
			case 270:Act_RwhoDeclareAlive();break;
			case 271:Act_SetExit();break;
			case 272:Act_DelExit();break;
/*			case 273:Act_GetExit();break;*/
			case 274:Act_ForkDump();break;
			default:SendItem(Me(),"Error Invalid Action(%d)\n",c);
		}
	}
		if(v==neg)	/* Check for failed break */
			break;
	}
	return(0);
}

ITEM *FindAnItem(ad,no,ord)	/* Find an item in normal path (ROOM/CARRIED) */
int ad,no,ord;
{
	ITEM *a;
	a=FindIn(LevelOf(Me()),O_PARENT(Me()),(short)ad,(short)no);
	if(!a)
		goto l2;
	while(--ord)
	{
		a=NextIn(LevelOf(Me()),a,(short)ad,(short)no);
		if(!a)
			goto l2;
	}
	return(a);
l2:	a=FindIn(LevelOf(Me()),Me(),(short)ad,(short)no);
	if(!a)
		return(a);
	while(--ord)
	{
		a=NextIn(LevelOf(Me()),a,(short)ad,(short)no);
		if(!a)
			return(a);
	}
	return(a);
}

int FindTableByName(x)
char *x;
{
	TABLE *t=TableList;
	while(t)
	{
		if(stricmp(TextOf(t->tb_Name),x)==0)
			return(t->tb_Number);
		t=t->tb_Next;
	}
	return(-1);
}

TABLE *FindTable(n)		/* Find a table by number */
int n;
{
	TABLE *t=TableList;
	while(t)
	{
		if(t->tb_Number==n)
			return(t);
		t=t->tb_Next;
	}
	return(NULL);
}

TABLE *NewTable(n,name)	/* Create a table */
int n;
char *name;
{
	TABLE *l=Allocate(TABLE);
	l->tb_Number=n;
	l->tb_Next=TableList;
	l->tb_First=NULL;
	if(name)
		l->tb_Name=AllocText(name);
	TableList=l;
	return(l);
}

LINE *NewLine(t,n)	/* Create a line */
TABLE *t;
int n;
{
	LINE *a=Allocate(LINE);
	LINE *p=NULL;
	LINE *l=t->tb_First;
	if(l==NULL)
		goto skisc;
	while(n)
	{
		p=l;
		l=l->li_Next;
		if(l==NULL)
			break;
		n--;
	}
skisc:	if(p)
	{
		p->li_Next=a;
		a->li_Next=l;
	}
	else
	{
		a->li_Next=t->tb_First;
		t->tb_First=a;
	}
	a->li_Data=Allocate(unsigned short);
	a->li_Data[0]=CMD_EOL;
	return(a);
}

LINE *FindLine(t,n)		/* Find specific line */
TABLE *t;
int n;
{
	LINE *l=t->tb_First;
	while(n)
	{
		if(l==NULL)
			return(l);
		l=l->li_Next;
		n--;
	}
	return(l);
}

int DeleteLine(t,n)		/* Delete a line (lowest level) */
TABLE *t;
int n;
{
	LINE *l=FindLine(t,n);
	LINE *p;
	if(n==0)
	{
		if(l==NULL)
			return(-1);
		t->tb_First=l->li_Next;
	}
	else
	{
		p=FindLine(t,n-1);
		if(l==NULL)
			return(-1);	/* No Such Line */
		p->li_Next=l->li_Next;
	}
	free((char *)l->li_Data);
	free((char *)l);
	return(0);
}

int WipeDeleteLine(t,n)		/* Delete a line (lowest level) */
TABLE *t;
int n;
{
	LINE *l=FindLine(t,n);
	LINE *p;
	if(n==0)
	{
		if(l==NULL)
			return(-1);
		t->tb_First=l->li_Next;
	}
	else
	{
		p=FindLine(t,n-1);
		if(l==NULL)
			return(-1);	/* No Such Line */
		p->li_Next=l->li_Next;
	}
	WipeLine(l);
	free((char *)l->li_Data);
	free((char *)l);
	return(0);
}

int UserAction(u,v)
ITEM *u;
int v;		/* Pregot Verb */
{	
	TABLE *t;
	int ct=0;	/* Safety on superclass loops */
	ITEM *i1;
	ITEM *i2;

	CurrentPlayer=u;
	ParseArgs(v);

	i1=Item1;
	i2=Item2;

	if(Item2)
	{
		while(i2 && ct<20)
		{
			ct++;
			if(i2->it_ObjectTable)
			{
				if(ExecTable(i2->it_ObjectTable)==1)
					return(1);
			}
			i2=i2->it_Superclass;
		}
			
	}
	if(Item1)
	{
		ct=0;
		while(i1 && ct<20)
		{
			ct++;
			if(i1->it_SubjectTable)
			{
				if(ExecTable(i1->it_SubjectTable)==1)
					return(1);
			}
			i1=i1->it_Superclass;
		}
	}
	t=FindTable(u->it_ActorTable);
	if(t==NULL)
		return(-1);
	return(ExecTable(t));
}

int UserDaemon(i)
ITEM *i;
{
	TABLE *t;
	ITEM *op=CurrentPlayer;
	ITEM *oa=ActorPlayer;
	ITEM *o1=Item1,*o2=Item2;
	ITEM *ip=i;
	int n=0;
	int ct=0;

	ActorPlayer=CurrentPlayer;
	CurrentPlayer=i;


	while(ip && n!=1 && ct<20)
	{
		t=ip->it_DaemonTable;
		if(t!=NULL)
			n=ExecTable(t);
		ct++;
		ip=ip->it_Superclass;
	}
	if(n!=1)
	{
		t=FindTable(i->it_ActionTable);
		if(t!=NULL)
			ExecTable(t);	/* System table last */
	}
		
/*
 *	We have to restore NULL if the person we are returning to has just
 *	gone and died. If this is the case his item pointer will be invalid.
 *	This relies on the fact that no creation occurs during the daemons too
 *	(Basically this one is a problem, and a little dodgy at the moment).
 */
	if(((op!=i)||(CurrentPlayer!=NULL))&&(ValidItem(op)))
		CurrentPlayer=op;
	ActorPlayer=oa;
	Item1=o1;
	Item2=o2;
	return(n);
}

int RandPerc()		/* Random %age chance */
{
	return((rand()>>7)%100);
}
