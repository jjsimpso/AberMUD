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
Module	"Table Compiler";
Version "1.10";
Author	"Alan Cox";


/*
 *	1.00	AGC	Created this file
 *	1.01	AGC	Added support for {$}
 *	1.02	AGC	Added $RM $ME $1 $2
 *	1.03	AGC	Added {$2}
 *	1.04	AGC	Added NONE
 *	1.05	AGC	Added reverse compiler for table editing
 *	1.06	AGC	Added the code to handle named flags
 *	1.07	AGC	Added #item
 *	1.08	AGC	Classes
 *	1.09	AGC	Added the BitName fields
 */

/*
 *	Routines to convert between source and line data
 *
 */


static unsigned short DataBuffer[1024];	/* A Line Buffer */
static short DPtr=0;
static char LineBuffer[512];
static char *LinePtr=LineBuffer;

static char *GetText2()
{
	extern char *strtok2();
	if(LinePtr==LineBuffer)
	{
		LinePtr++;
		return(strtok2(LineBuffer,"\t {","}"));
	}
	return(strtok2(NULL," \t{","}"));
}


static char *GetText()
{
	char *a;
l1:	a=GetText2();
	if(a==NULL)
		return(a);
	if(a==(char *)-1)
		return(a);
	if(strlen(a)==0)
		goto l1;
	return(a);
}
	
static char *GetToken2()
{
	extern char *strtok2();
	if(LinePtr==LineBuffer)
	{
		LinePtr++;
		return(strtok2(LineBuffer," \n\t,"," \n\t,"));
	}
	return(strtok2(NULL," \n\t,"," \n\t,"));
}

static char *GetToken()
{
	char *a;
l1:	a=GetToken2();
	if(a==NULL)
		return(a);
	if(strlen(a)==0) goto l1;
	return(a);
}

static int WriteDb(n)
int n;
{
	if(DPtr==1024)
		return(-1);
	DataBuffer[DPtr++]=n;
	return(0);
}

static int EncodeFlag(i)
ITEM *i;
{
	char *a=GetToken();
	int b;
	int shf=0;
	if(a==NULL)
	{
		SendItem(i,"Missing Numeric Argument.\n");
		return(-1);
	}
	if((*a=='F')||(*a=='f'))
	{
		a++;
		shf=30000;
	}
	if(*a=='@')
	{
		b=GetFlagByName(a);
		if(b==-1)
		{
			SendItem(i,"Invalid Flag Name - %s.\n",a);
			return(-1);
		}
	}
	else
	{
		if(sscanf(a,"%d",&b)==0)
		{
			SendItem(i,"Invalid Numeric Argument - %s.\n",a);
			return(-1);
		}
	}
	if(WriteDb(b+shf)==-1)
	{
		SendItem(i,"Line Too Complex.\n");
		return(-1);
	}
	return(0);
}
	
static int EncodeClass(i)
ITEM *i;
{
	char *a=GetToken();
	short b=WhichClass(a);
	if(stricmp(a,"0")==0)
	{
		if(WriteDb(-1)==-1)
		{
			SendItem(i,"Line Too Complex.\n");
			return(-1);
		}
		return(0);
	}
	if(b==-1)
	{
		SendItem(i,"Unknown class %s.\n",a);
		return(-1);
	}
	if(WriteDb(b)==-1)
	{
		SendItem(i,"Line Too Complex.\n");
		return(-1);
	}
	return(0);
}	

static int EncodeRFlag(i)
ITEM *i;
{
	char *a=GetToken();
	int b;
	if(a==NULL)
	{
		SendItem(i,"Missing Room Bit Flag.\n");
		return(-1);
	}
	b=FindRBit(a);
	if(b==-1)
	{
		SendItem(i,"Unknown Room Bit Flag '%s'.\n",a);
		return(-1);
	}
	if(WriteDb(b)==-1)
	{
		SendItem(i,"Line Too Complex.\n");
		return(-1);
	}
	return(0);
}

static int EncodeOFlag(i)
ITEM *i;
{
	char *a=GetToken();
	int b;
	if(a==NULL)
	{
		SendItem(i,"Missing Object Bit Flag.\n");
		return(-1);
	}
	b=FindOBit(a);
	if(b==-1)
	{
		SendItem(i,"Unknown Object Bit Flag '%s'.\n",a);
		return(-1);
	}
	if(WriteDb(b)==-1)
	{
		SendItem(i,"Line Too Complex.\n");
		return(-1);
	}
	return(0);
}

static int EncodePFlag(i)
ITEM *i;
{
	char *a=GetToken();
	int b;
	if(a==NULL)
	{
		SendItem(i,"Missing Player Bit Flag.\n");
		return(-1);
	}
	b=FindPBit(a);
	if(b==-1)
	{
		SendItem(i,"Unknown Player Bit Flag '%s'.\n",a);
		return(-1);
	}
	if(WriteDb(b)==-1)
	{
		SendItem(i,"Line Too Complex.\n");
		return(-1);
	}
	return(0);
}

static int EncodeCFlag(i)
ITEM *i;
{
	char *a=GetToken();
	int b;
	if(a==NULL)
	{
		SendItem(i,"Missing Container Bit Flag.\n");
		return(-1);
	}
	b=FindCBit(a);
	if(b==-1)
	{
		SendItem(i,"Unknown Container Bit Flag '%s'.\n",a);
		return(-1);
	}
	if(WriteDb(b)==-1)
	{
		SendItem(i,"Line Too Complex.\n");
		return(-1);
	}
	return(0);
}
	
static int EncodeNumber(i)
ITEM *i;
{
	char *a=GetToken();
	int b;
	int shf=0;
	if(a==NULL)
	{
		SendItem(i,"Missing Numeric Argument.\n");
		return(-1);
	}
	if((*a=='F')||(*a=='f'))
	{
		a++;
		shf=30000;
	}
	if(sscanf(a,"%d",&b)==0)
	{
		SendItem(i,"Invalid Numeric Argument - %s.\n",a);
		return(-1);
	}
	if(WriteDb(b+shf)==-1)
	{
		SendItem(i,"Line Too Complex.\n");
		return(-1);
	}
	return(0);
}

static int EncodeProcess(i)
ITEM *i;
{
	char *a=GetToken();
	int b;
	int shf=0;
	if(a==NULL)
	{
		SendItem(i,"Missing Table Name/Number.\n");
		return(-1);
	}
	if((*a=='F'||*a=='f')&&isdigit(a[1]))
	{
		a++;
		shf=30000;
	}
	if(!shf&&!isdigit(*a))
	{
		b=FindTableByName(a);
		if(b==-1)
		{
			SendItem(i,"Unknown table %s\n",a);
			return(-1);
		}
		if(WriteDb(b)==-1)
		{
			SendItem(i,"Line Too Complex.\n");
			return(-1);
		}
		return(0);
	}
	if(sscanf(a,"%d",&b)==0)
	{
		SendItem(i,"Invalid Numeric Argument - %s.\n",a);
		return(-1);
	}
	if(WriteDb(b+shf)==-1)
	{
		SendItem(i,"Line Too Complex.\n");
		return(-1);
	}
	return(0);
}

static char *WorType[]=
{
	"",
	"noun","preposition","pronoun","class","verb","adjective","noise word"
};
	
static int EncodeWord(i,type)
ITEM *i;
short type;
{
	char *a=GetToken();
	WLIST *b;
	if(a==NULL)
	{
		SendItem(i,"Missing %s argument.\n",WorType[type]);
		return(-1);
	}
	if(stricmp(a,"ANY")==0)
	{
		if(WriteDb(-1)==-1)
		{
			SendItem(i,"Line Too Complex.\n");
			return(-1);
		}
		return(0);
	}
	if(stricmp(a,"NONE")==0)
	{
		if(WriteDb(-2)==-1)
		{
			SendItem(i,"Line Too Complex.\n");
			return(-1);
		}
		return(0);
	}
	if(type==-1)
	{
		b=FindInList(WordList,a,WD_VERB);
		if(!b)
			b=FindInList(WordList,a,WD_ADJ);
	}
	else
		b=FindInList(WordList,a,type);
	if(b==NULL)
	{
		SendItem(i,"Unknown %s '%s'.\n",WorType[type],a);
		return(-1);
	}
	if(WriteDb(b->wd_Code)==-1)
	{
		SendItem(i,"Line Too Complex.\n");
		return(-1);
	}
	return(0);
}	

static ITEM *LockList[256];
static short LockCount;

int RememberToLockItem(i)
ITEM *i;
{
	if(LockCount==256) return(-1);
	LockList[LockCount++]=i;
	return(0);
}

void LockLockList()
{
	short x=0;
	while(x<LockCount)
		LockItem(LockList[x++]);
}

static int EncodeItem(i)
ITEM *i;
{
	char *a=GetToken();
	char *b;
	ITEM *x;
	WLIST *ad,*no;
	int refid=1;
	static WLIST DummyNullWord={"ANY",WD_ADJ,-1,NULL};
	if(a==NULL)
	{
		SendItem(i,"Object expected.\n");
		return(-1);
	}
	if(stricmp(a,"$1")==0)
	{
		x=(ITEM *)1;
		goto l1;
	}
	if(stricmp(a,"$2")==0)
	{
		x=(ITEM *)3;
		goto l1;
	}
	if(stricmp(a,"$ME")==0)
	{
		x=(ITEM *)5;
		goto l1;
	}
	if(stricmp(a,"$AC")==0)
	{
		x=(ITEM *)7;
		goto l1;
	}
	if(stricmp(a,"$RM")==0)
	{
		x=(ITEM *)9;
		goto l1;
	}
	b=GetToken();
	if(*a=='#')
	{
		if(sscanf(a,"#%d",&refid)==0)
		{
			SendItem(i,"Bad # identity %s.\n",a);
			return(-1);
		}
		a=b;
		b=GetToken();
	}
	if(a==NULL||b==NULL)
	{
		SendItem(i,"Object expected.\n");
		return(-1);
	}
	if(stricmp(a,"ANY")==0)
		ad=&DummyNullWord;
	else
		ad=FindInList(WordList,a,WD_ADJ);
	if(ad==NULL)
	{
		SendItem(i,"Unknown Word %s.\n",a);
		return(-1);
	}
	no=FindInList(WordList,b,WD_NOUN);
	if(no==NULL)
	{
		SendItem(i,"Unknown Word %s.\n",b);
		return(-1);
	}
	x=FindMaster(32767,ad->wd_Code,no->wd_Code);
	if(refid>1)
	{
		while((--refid)&&(x))
			x=NextMaster(32767,x,ad->wd_Code,no->wd_Code);
		if(x==NULL)
			SendItem(i,"# out of range for item %s %s.\n",a,b);
	}
	if(x==NULL)
	{
		SendItem(i,"Unknown Item %s %s.\n",a,b);
		return(-1);
	}
	if(RememberToLockItem(x))
		return(-1);	/* Lock the reference */
l1:	if(WriteDb((unsigned short)(((unsigned int)(x))/65536L))==-1)
	{
		SendItem(i,"Line Too Complex.\n");
		return(-1);
	}
	if(WriteDb((unsigned short)(((unsigned int)(x))%65536L))==-1)
	{
		SendItem(i,"Line Too Complex.\n");
		return(-1);
	}
	return(0);
}



static int EncodeText(i,n)
ITEM *i;
int n;
{
	TPTR a;
	char *b;
	b=GetText();
	if(b==NULL)
	{
		SendItem(i,"Text Argument Missing.\n");
		return(-1);
	}
	if(b==(char *)-1)
	{
		SendItem(i,"{ expected.\n");
		return(-1);
	}
	if(stricmp(b,"$")==0)
		a=(TPTR)1;
	else
	{
		if(stricmp(b,"$2")==0)
			a=(TPTR)3;
		else
		{
			if(n)
			
				a=AllocComment(b);
			else
				a=AllocText(b);
		}
	}
	if(WriteDb((unsigned short)(((unsigned int)(a))/65536L))==-1)
	{
		SendItem(i,"Line Too Complex.\n");
		return(-1);
	}
	if(WriteDb((unsigned short)(((unsigned int)(a))%65536L))==-1)
	{
		SendItem(i,"Line Too Complex.\n");
		return(-1);
	}
	return(0);
}

static int EncodeCommand(i,n)
ITEM *i;
short n;	/* Command Code */
{
	int v;
	char *d=Cnd_Table[n];
	if(WriteDb(n)==-1)
	{
		SendItem(i,"Line Too Complex.\n");
		return(-1);
	}
	while(*d!=' ')
	{
		switch(*d)
		{
			case 'w':v=EncodeWord(i,-1);break;
			case 'W':v=EncodeNumber(i);break;
			case 'Z':v=EncodeNumber(i);break;
			case 'I':v=EncodeItem(i);break;
			case 'F':v=EncodeFlag(i);break;
			case 'N':v=EncodeNumber(i);break;
			case 'B':v=EncodeNumber(i);break;
			case 'a':v=EncodeWord(i,WD_ADJ);break;
			case 'n':v=EncodeWord(i,WD_NOUN);break;
			case 'p':v=EncodeWord(i,WD_PREP);break;
			case 'v':v=EncodeWord(i,WD_VERB);break;
			case 'T':v=EncodeText(i,0);break;
			case '$':v=EncodeText(i,1);break;
			case 't':v=EncodeProcess(i);break;
			case 'C':v=EncodeClass(i);break;
			case 'c':v=EncodeCFlag(i);break;
			case 'R':v=EncodeRFlag(i);break;
			case 'O':v=EncodeOFlag(i);break;
			case 'P':v=EncodePFlag(i);break;
			default:
				Error("Invalid Entry in Cnd_Table");
		}
		if(v==-1)
			return(-1);
		d++;
	}
	return(0);
}

static int EncodeLine(i)
ITEM *i;
{
	char *a;
	short c;
	DPtr=0;
	LockCount=0;
	if(EncodeWord(i,WD_VERB)==-1)
		return(-1);
	if(EncodeWord(i,WD_NOUN)==-1)
		return(-1);
	if(EncodeWord(i,WD_NOUN)==-1)
		return(-1);
	while((a=GetToken())!=NULL)
	{
		if(!strlen(a))
			continue;
		c=FindCnd(a);
		if(c==-1)
		{
			SendItem(i,"Unknown Condition/Action '%s'.\n",a);
			return(-1);
		}
		if(EncodeCommand(i,c)==-1)
			return(-1);
	}
	WriteDb(CMD_EOL);
	LockLockList();
	return(0);
}

void LoadLineBuffer(x)
char *x;
{
	strcpy(LineBuffer,x);
	LinePtr=LineBuffer;
}

int EncodeEntry(i,l)
ITEM *i;
LINE *l;
{
	int ct=3;
	if(EncodeLine(i)==-1)
		return(-1);
	free((char *)l->li_Data);
	l->li_Data=(unsigned short *)malloc((DPtr-3)*sizeof(unsigned short));
	if(l->li_Data==NULL)
	{
		SendItem(i,"Out Of Memory.\n");
		return(-1);
	}
	l->li_Verb=DataBuffer[0];
	l->li_Noun1=DataBuffer[1];
	l->li_Noun2=DataBuffer[2];
	while(ct<DPtr)
	{
		l->li_Data[ct-3]=DataBuffer[ct];
		ct++;
	}
	return(0);
}

int EncodeTable(i,t,f)
ITEM *i;
TABLE *t;
FILE *f;
{
	LINE *l;
	char *lp=LineBuffer;
	while(fgets(lp,511,f))
	{
l3:		if((*lp)&&(lp[strlen(lp)-2]=='+'))
		{
			lp+=strlen(lp)-2;		/* Continuation */
			if(lp-LineBuffer>400) goto l2;
			if(!fgets(lp,511,f))
				goto l2;
			goto l3;
		}
l2:		LineBuffer[511]=0;
		LinePtr=LineBuffer;
		lp=LineBuffer;
		l=NewLine(t,32767);
		if(EncodeEntry(i,l)==-1)
			return(-1);
	}
	return(0);
}

static void DiscItem(dp)
unsigned short *dp;
{
	extern unsigned long PairArg();
	ITEM *a=(ITEM *)PairArg(dp);
	if((a!=(ITEM *)1)&&(a!=(ITEM *)3)&&(a!=(ITEM *)5)
			 &&(a!=(ITEM *)7)&&(a!=(ITEM *)9))
		UnlockItem(a);
}

static void DiscText(dp,n)
unsigned short *dp;
int n;
{
	TPTR a=(TPTR )(*dp*65536L+(*(dp+1)));
	if(a==(TPTR )1)
		return;
	if(a==(TPTR )3)
		return;
	if(n)
		FreeComment(a);
	else
		FreeText(a);
}

void WipeLine(l)
LINE *l;
{
	unsigned short *dp=l->li_Data;
	char *a;
	while(*dp!=CMD_EOL)
	{
		a=Cnd_Table[*dp];
		dp++;
		while(*a!=' ')
		{
			switch(*a++)
			{
				case 'W':;
				case 'w':;
				case 'Z':;
				case 'R':dp++;break;
				case 'O':dp++;break;
				case 'P':dp++;break;
				case 'c':dp++;break;
				case 'C':dp++;break;
				case 'N':dp++;break;
				case 'F':dp++;break;
				case 'B':dp++;break;
				case 'a':dp++;break;
				case 'n':dp++;break;
				case 'p':dp++;break;
				case 'v':dp++;break;
				case 't':dp++;break;
				case 'I':DiscItem(dp);
					 dp+=2;
					 break;
				case 'T':DiscText(dp,0);
					dp+=2;
					break;
				case '$':DiscText(dp,1);
					dp+=2;
					break;
				default:
					Error("Cnd_Table: Invalid Entry");
			}
		}
	}
}


void DeleteTable(t)	/* Free Up A Table Entry, except for header */
TABLE *t;
{
	LINE *l=t->tb_First;
	LINE *n;
	t->tb_First=NULL;
	FreeText(t->tb_Name);
	t->tb_Name=AllocText("(unset)");
	while(l)
	{
		n=l->li_Next;
		WipeLine(l);
		free((char *)l->li_Data);
		free((char *)l);
		l=n;
	}
}

/*
 *	Free a table header. Only use this on item bound tables, with
 *	no table data!
 */

void FreeTableHeader(t)
TABLE *t;
{
	FreeText(t->tb_Name);
	free((char *)t);
}

unsigned long PairArg(x)
unsigned short *x;
{
	return((*x)<<16|x[1]);
}

char *NumText(n)
int n;
{
	static char x[16];
	if(n>29999)
		sprintf(x,"F%d",n-30000);
	else
		sprintf(x,"%d",n);
	return(x);
}

/*
 *	BUG:
 *
 * 	This routine should check the line does not overflow the 512 char
 *	buffer. What it does if this happens now is crash, what it should
 *	do is also a problem.
 */
	
void Decompress(line,buffer)
LINE *line;
char *buffer;
{
	register char *TLine;
	unsigned short *x;
	TABLE *tmp;
	sprintf(buffer,"%s %s %s ",FindWText(line->li_Verb,WD_VERB),
				  FindWText(line->li_Noun1,WD_NOUN),
				  FindWText(line->li_Noun2,WD_NOUN));
	x=(unsigned short *)(line->li_Data);
	while(*x!=CMD_EOL)
	{
		int n=*x++;
		int ct=0;
		strcat(buffer,Cnd_Table[n]+8);
		strcat(buffer," ");
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
					switch((int)i)
					{
						case 1:strcat(buffer,"$1");
							break;
						case 3:strcat(buffer,"$2");
							break;
						case 5:strcat(buffer,"$ME");
							break;
						case 7:strcat(buffer,"$AC");
							break;
						case 9:strcat(buffer,"$RM");
							break;
						default:
						if(!IsUnique(32766,
							i->it_Adjective,
							i->it_Noun))
						sprintf(buffer+strlen(buffer),"#%d ",ItemNumber(32766,
							i));
						if(i->it_Adjective!=-1)
						{
							strcat(buffer,FindWText(i->it_Adjective,WD_ADJ));
							strcat(buffer," ");
						}
						else
							strcat(buffer,"ANY ");
						strcat(buffer,FindWText(i->it_Noun,WD_NOUN));
					}
					break;
				case '$':;
				case 'T':t=(TPTR )PairArg(x);
					 x+=2;
					 if(t==(TPTR )1)
						strcat(buffer,"{$}");
					 else
					 {
						if(t==(TPTR) 3)
							strcat(buffer,"{$2}");
						else
						 {
							strcat(buffer,"{");
							strcat(buffer,TextOf(t));
							strcat(buffer,"}");
						 }
					 }
					 break;
				case 'F':strcat(buffer,GetFlagName(*x++));
					 break;
				case 'N':;
				case 'W':;
				case 'w':;	/* THIS IS WRONG FOR NOW */
				case 'Z':;
				case 'B':strcat(buffer,NumText(*x++));
					 break;
				case 'p':strcat(buffer,FindWText((short)*x++,WD_PREP));
					 break;
				case 'v':strcat(buffer,FindWText((short)*x++,WD_VERB));
					 break;
				case 'a':strcat(buffer,FindWText((short)*x++,WD_ADJ));
					 break;
				case 'n':strcat(buffer,FindWText((short)*x++,WD_NOUN));
					 break;
				case 'C':strcat(buffer,GetClassName(*x++));
					 break;
				case 'c':strcat(buffer,CBitName(*x++));
					 break;
				case 'O':strcat(buffer,OBitName(*x++));
					 break;
				case 'R':strcat(buffer,RBitName(*x++));
					 break;
				case 'P':strcat(buffer,PBitName(*x++));
					 break;
				case 't':tmp=FindTable(*x);
					 if(tmp&&
						strcmp(TextOf(tmp->tb_Name),
							"(unset)"))
						strcat(buffer,
							TextOf(tmp->tb_Name));
					 else
						strcat(buffer,NumText(*x));
					 x++;
					 break;
				default:
					Error("Invalid Entry In Cnd_Table");
			}
			strcat(buffer," ");
			ct++;
		}
	}
}

