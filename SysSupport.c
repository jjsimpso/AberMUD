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
#include "User.h"

extern USER UserList[];

Module  "System Support";
Version "1.17";
Author  "Alan Cox";

/*
 *	Useful routines we use a lot, these use the low level facilities to
 *	provide handy facilities we use a lot, and to save the programmer
 *	going potty....
 *
 *	1.00	PlayerOf,RoomOf,ObjectOf SetName SendItem and basics added
 *	1.01	RemovePlayer,ExitPlayer extracted from CommandDriver
 *	1.02	Added DoesAction and Broadcast type drivers
 *	1.03	Added Exit Support Code
 *	1.04	Drivers for Prompt and Edit packets
 *	1.05	Fixed Carry when Quit - Items now dropped
 *	1.06	Understands BOSS1/2 defines
 *	1.07	Added BACKTRACK and UserOf
 *	1.08	Added Gfx Driver support
 *	1.09	Added SNOOP support
 *	1.10	IsDarkFor implemented and moved to DarkLight.c
 *	1.11	#<id> support added
 *	1.12	Added register optimisations and short array references
 *	1.13	Added Setin Setout Sethere
 *	1.14	Clear Item1/Item2/Me correctly on player exit (still wrong on stacked cases)
 *	1.15	Support for ACKnowledge on clear.
 *		Protected CNameOf from crashes on long names (returns lowercase)
 *	1.16	RemoveUser doesn't remove blank names
 *	1.17	Strict ANSIfication
 */

PLAYER *PlayerOf(x)	/* Return player pointer of item (or NULL) */
ITEM *x;
{
	return((PLAYER *)FindSub(x,KEY_PLAYER));
}

OBJECT *ObjectOf(x)	/* Guess ... */
ITEM *x;
{
	return((OBJECT *)FindSub(x,KEY_OBJECT));
}

ROOM *RoomOf(x)		/* Keep guessing */
ITEM *x;
{
	return((ROOM *)FindSub(x,KEY_ROOM));
}

USERFLAG *UserFlagOf(x)	/* Fun this */
ITEM *x;
{
	return((USERFLAG *)FindSub(x,KEY_USERFLAG));
}

USERFLAG *UserFlag2Of(x)
ITEM *x;
{
	return((USERFLAG *)FindSub(x,KEY_USERFLAG2));
}

CONTAINER *ContainerOf(x)	/* Dum de dum */
ITEM *x;
{
	return((CONTAINER *)FindSub(x,KEY_CONTAINER));
}

int UserOf(x)			/* DIFFERENT (STOP!!!) - returns an integer
				   holding player number -1 if not a user */
ITEM *x;
{
	register PLAYER *p=PlayerOf(x);
	if(p==NULL)
		return(-1);
	return(p->pl_UserKey);
}

int IsUser(x)			/* Tests for all of the above */
ITEM *x;
{
	return(UserOf(x)!=-1);
}

int IsRoom(i)
ITEM *i;
{
	return(RoomOf(i)!=NULL);
}

int IsPlayer(i)
ITEM *i;
{
	return(PlayerOf(i)!=NULL);
}

int IsObject(i)
ITEM *i;
{
	return(ObjectOf(i)!=NULL);
}

/*
 *	This low level text driver is used to send SNOOP data.	You shouldn't
 *	need to touch this bit
 */

void SendItemDirect(i,x)
ITEM *i;
char *x;
{
	register char *y=x;
	short u;
	PORT *pr;
	register char v;
	register PLAYER *p=PlayerOf(i);
	if(p==NULL)
		return;
	if(p->pl_UserKey==-1)
		return;
	u=p->pl_UserKey;
	pr=UserList[u].us_Port;
	while(strlen(y)>510)
	{
		v=y[510];
		y[510]=0;
		SendTPacket(pr,PACKET_SNOOPTEXT,y);
		y[510]=v;
		y+=510;
	}
	SendTPacket(pr,PACKET_SNOOPTEXT,y);
}

/*
 *	Send a message to an item (VarArgs)
 *
 */
void SendItem(x,a,b,c,d,e,f)
ITEM *x;
char *a,*b,*c,*d,*e,*f;
{
	register PLAYER *p=PlayerOf(x);
	if(p==NULL)		/* Dont send to non players */
	{
		SendUser(-1,a,b,c,d,e,f);
		goto l1;
	}
	SendUser(p->pl_UserKey,a,b,c,d,e,f);	/* Send message   */
l1:	SnoopCheckString(x,UserLastLine);	/* Check snooping */
}

int IsCalled(i,s)		/* Check item name against string */
ITEM *i;
char *s;
{
	if(stricmp(TextOf(i->it_Name),s)==0)
		return(1);
	return(0);
}

void SetName(i,s)	/* Change the name of an item. This is how all text works */
			/* YOU MUST FREE the old text before creating the new */
register ITEM *i;
char *s;
{
	FreeText(i->it_Name);
	i->it_Name=AllocText(s);
}
	
int ArchWizard(i)		/* Check for debugging persona */
register ITEM *i;
{
	if(IsCalled(i,"Anarchy"))
		return(1);
	if(IsCalled(i,"Debugiit"))
		return(1);
	if(IsCalled(i,BOSS1))
		return(1);
	if(IsCalled(i,BOSS2))
		return(1);
	if(IsCalled(i,BOSS3))
		return(1);
	if(IsCalled(i,BOSS4))
		return(1);
	if(IsCalled(i,BOSS5))
		return(1);
	return(0);
}

char *NameOf(x)		/* Return the name of an item */
ITEM *x;
{
	return(TextOf(x->it_Name));
}

char *CNameOf(x)	/* Return the name of an item with 1st letter capital
			   size limit of 128 bytes */
ITEM *x;
{
	static char bf[128];
	if(strlen(NameOf(x))>127)
		return(NameOf(x));
	strcpy(bf,NameOf(x));
	if(*bf)
	{
		if(islower(*bf))
			*bf=toupper(*bf);
	}
	return(bf);
}

short LevelOf(x)	/* Return level of user */
ITEM *x;
{
	register PLAYER *p=PlayerOf(x);
	if(p==NULL)
		return(0);
	if((ArchWizard(x))&&(p->pl_Level<10000))
		return(10000);
	return(p->pl_Level);
}

void Place(x,y)		/* Place item X at Y, doing all links/unlinks needed */
register ITEM *x,*y;
{
	if(IsObject(x))
		ObjectOf(x)->ob_Flags&=~OB_WORN;	/* Clear worn when move */
	if(!O_FREE(x))
		UnlinkItem(x);
	LinkItem(x,y);
}

void XPlace(x,y)	/* Place item X at Y, doing all links/unlinks needed */
register ITEM *x,*y;	/* But only doing a temporary movement while thinking */
{
	if(!O_FREE(x))
		UnlinkItem(x);
	LinkItem(x,y);
}

/*
 *	Remove a user from the game system.
 *
 */

void RemoveUser(u)
unsigned int u;
{
	extern ITEM *Item1,*Item2;
	register ITEM *i;
	register ITEM *j;
	SendUser(-2, "");
	if(UserList[(unsigned short)u].us_Item!=NULL)
	{
		i=O_CHILDREN(UserList[(unsigned short)u].us_Item);
		while(i)	/* Drop all items */
		{
			j=O_NEXT(i);	/* Before Move! - NEXT will change otherwise */
			Place(i,O_PARENT(UserList[(unsigned short)u].us_Item));
			i=j;
		}
#ifdef ATTACH
		if(UserList[u].us_RealPerson&&UserList[u].us_Port!=NULL)
		{
			Act_UnAlias();
			return;
		}
#endif
		if(UserList[u].us_Port!=NULL)
			SendUser(-2, "");
		i=UserList[(unsigned short)u].us_Item;
		UnUserFlag(i);				/* Remove properties */
		UnlinkItem(i);
		UnlockItem(i);
		KillEventQueue(i);
		UnPlayer(i);
		i->it_Perception=-1;			/* Delayed Expunge in case */
		StopAllSnoops(i);
		StopSnoopsOn(i);
		KillIOH(i);
		UnUserText(i);
		if(Debugger==i)
			Debugger=NULL;
		if(Me()==i)
			SetMe(NULL);
		if(Item1==i)
			Item1=NULL;
		if(Item2==i)
			Item2=NULL;
		FreeItem(i);				/* Remove it all */
	}
	UserList[(unsigned short)u].us_Item=NULL;
	if(*UserList[u].us_Name)
		FreeWord(UserList[(unsigned short)u].us_Name,WD_NOUN);
	strcpy(UserList[(unsigned short)u].us_Name,"");
	if(UserList[(unsigned short)u].us_Port!=NULL)
		CloseMPort(UserList[(unsigned short)u].us_Port);	/* Bye bye */
	UserList[u].us_Port=NULL;		/* Port now free */
}

void ExitUser(u)	/* Kicks user back to login */
unsigned int u;
{
	extern ITEM *Item1,*Item2;
	register ITEM *i=O_CHILDREN(UserList[(unsigned short)u].us_Item);
	register ITEM *j;
	while(i)
	{
		j=O_NEXT(i);	/* Before Move! - NEXT will change otherwise */
		Place(i,O_PARENT(UserList[(unsigned short)u].us_Item));
		i=j;
	}
#ifdef ATTACH
	if(UserList[u].us_RealPerson)
	{
		PlayerOf(UserList[u].us_Item)->pl_UserKey=-1;
		UnlockItem(UserList[u].us_Item);
		UserList[u].us_Item=UserList[u].us_RealPerson;
	}
#endif
	i=UserList[(short)u].us_Item;
	UnUserFlag(i);
	UnlinkItem(i);
	UnlockItem(i);
	KillEventQueue(i);
	UnPlayer(i);
	StopAllSnoops(i);
	StopSnoopsOn(i);
	KillIOH(i);
	UnUserText(i);
	if(Debugger==i)
		Debugger=NULL;
	if(Me()==i)
		SetMe(NULL);
	if(Item1==i)
		Item1=NULL;
	if(Item2==i)
		Item2=NULL;
	i->it_Perception=-1;		/* Delayed Expunge */
	FreeItem(i);
	UserList[(unsigned short)u].us_Item=NULL;
	FreeWord(UserList[(unsigned short)u].us_Name,WD_NOUN);
	UserList[(unsigned short)u].us_State=AWAIT_NAME;
}
	
int CountUsers()		/* Count number of users on system */
{
	register short ct=0;
	register short c2=0;
	while(ct<MAXUSER)
	{
		if(UserList[ct].us_Port)
			c2++;
		ct++;
	}
	return((int)c2);
}

int IsBlind(i)	/* Basic checks */
ITEM *i;
{
	register PLAYER *p=PlayerOf(i);
	if(p==NULL)
		return(0);
	if(p->pl_Flags&PL_BLIND)
		return(1);
	return(0);
}

int IsDeaf(i)
ITEM *i;
{
	register PLAYER *p=PlayerOf(i);
	if(p==NULL)
		return(0);
	if(p->pl_Flags&PL_DEAF)
		return(1);
	return(0);
}


void ByeBye(i,m)	/* Clear a user with a message */
ITEM *i;
char *m;
{
	if(UserOf(i)==-1)
		return;
	UserList[UserOf(i)].us_State=AWAIT_ACK;
	SendTPacket(UserList[UserOf(i)].us_Port,PACKET_CLEAR,m);
}


static int AnyMore(u,i)
ITEM *u;
ITEM *i;
{
l1:	if(!O_NEXT(i))
		return(0);
	else
	{
		if((IsObject(O_NEXT(i)))&&
			(ObjectOf(O_NEXT(i))->ob_Flags&OB_NOSEECARRY))
		{
			i=O_NEXT(i);
			goto l1;
		}
		if(!CanSee(LevelOf(u),O_NEXT(i)))
		{
			i=O_NEXT(i);
			goto l1;
		}
	}
	return(1);
}

static ITEM *NextFor(u,i)
ITEM *u;
ITEM *i;
{
l1:	if(!O_NEXT(i))
		return(NULL);
	else
	{
		i=O_NEXT(i);
		if((IsObject(i))&&
			(ObjectOf(i)->ob_Flags&OB_NOSEECARRY))
		{
			goto l1;
		}
		if(!CanSee(LevelOf(u),i))
		{
			goto l1;
		}
	}
	return(i);
}

static void ShowInventory(i,x)
ITEM *i,*x;
{
	short f=0;
	if(x==NULL)
	{
		SendItem(i,".\n");
		return;
	}
	while(x)
	{
		if(f)
		{
			if(!AnyMore(i,x))
				SendItem(i," and ");
			else
				SendItem(i,",");
		}
		if(!((IsObject(x))&&(ObjectOf(x)->ob_Flags&OB_NOSEECARRY)))
		{
			if(!f)
				SendItem(i," carrying ");
			SendItem(i,NameOf(x));
			f=1;
		}
		if(!AnyMore(i,x))
		{
			SendItem(i,".\n");
			return;
		}
		x=NextFor(i,x);
	}
}

void DescribeItem(i,x)	/* Describe an item (AberMUD style) */
ITEM *i;
ITEM *x;
{
	OBJECT *o=ObjectOf(x);
	PLAYER *p=PlayerOf(x);
	if(CanSee(LevelOf(i),x)==0)
		return;
	if(o)
	{
		if(o->ob_Flags&OB_FLANNEL)	/* Not flannels */
			return;
		SendItem(i,"%s\n",TextOf(o->ob_Text[O_STATE(x)]));
		if(UserOf(i)>=0)
			SetItData((short)UserOf(i),x,x->it_Adjective,x->it_Noun);
		return;
	}
	if(p)
	{
		SendItem(i,"%s %s",CNameOf(x),GetHereMsg(x));	/* Players */
		if(UserOf(i)>=0)
			SetItData((short)UserOf(i),x,x->it_Adjective,x->it_Noun);
		ShowInventory(i,O_CHILDREN(x));
		/* Will do inventory too later */
	}
}

void DoesAction(user,flag,msg,p1,p2,p3,p4,p5,p6)
register ITEM *user;
int flag;	/* 1=Someone 0=Nothing if cant see */
		/* 2=Nothing if deaf */
		/* 4=Must Be Present */
char *msg,*p1,*p2,*p3,*p4,*p5,*p6;
{
/*
 *	See the DOESACTION database action documentation
 */
	register short ct=0;
	register ITEM *i;
	while(ct<MAXUSER)	/* For each user */
	{
		i=UserList[ct].us_Item;
		if((i)&&(i!=user))	/* Each but me ... */
		{
			if(flag&4)	/* Must be present */
			{
				if(O_PARENT(i)!=O_PARENT(user))
					goto ne;
			}
			if(IsDeaf(i)&&(flag&2))	/* Must be able to here */
				goto ne;
			if((IsBlind(i))||(CanSee(LevelOf(i),user)==0))
			{	/* Cant see */
				if((flag&1)==0)
					goto ne;	/* No one */
				SendItem(i,"Someone ");	/* Someone */
				SendItem(i,msg,p1,p2,p3,p4,p5,p6);
				if(!strchr(msg,'\n'))
					SendItem(i,"\n");
				goto ne;
			}
			SendItem(i,"%s ",CNameOf(user));
			SendItem(i,msg,p1,p2,p3,p4,p5,p6);	/* Send message */
			if(UserOf(i)>=0)
				SetItData((short)UserOf(i),
					user,user->it_Adjective,user->it_Noun);
			if(!strchr(msg,'\n'))
				SendItem(i,"\n");
		}
ne:		ct++;
	}
}		

void DoesTo(user,flag,thing,msg,p1,p2,p3,p4,p5,p6)
register ITEM *user;
int flag;
ITEM *thing;
/* 1=Someone 0=Nothing if cant see 	*/
/* 2=Nothing if deaf 			*/
/* 4=Must Be Present 			*/
/* 8=Aware if target			*/
char *msg,*p1,*p2,*p3,*p4,*p5,*p6;
{
/*
 *	See database Documentation here
 */
	register short ct=0;
	register ITEM *i;
	while(ct<MAXUSER)
	{
		i=UserList[ct].us_Item;
		if((i)&&(i!=user))
		{
			if(flag&4)
			{
				if(O_PARENT(i)!=O_PARENT(user))
					goto ne;
			}
			if(thing==i&&(flag&8))
				goto uncs;
			if(IsDeaf(i)&&(flag&2))
				goto ne;
			if((IsBlind(i))||((CanSee(LevelOf(i),user)==0)&&
				(CanSee(LevelOf(i),thing)==0)))
			{
				if(flag&1)
					goto ne;
			}
uncs:			if((IsBlind(i))||(CanSee(LevelOf(i),user)==0))
			{
				if(IsPlayer(user))
					SendItem(i,"Someone ");
				else
					SendItem(i,"Something ");
				SendItem(i,msg,p1,p2,p3,p4,p5,p6);
				goto it1;
			}
			SendItem(i,"%s ",CNameOf(user));
			if(UserOf(i)>=0)
				SetItData((short)UserOf(i),
					user,user->it_Adjective,user->it_Noun);
			SendItem(i,msg,p1,p2,p3,p4,p5,p6);
it1:			if(CanSee(LevelOf(i),thing)==0||IsBlind(i))
			{
				if(IsPlayer(thing))
					SendItem(i," someone.\n");
				else
					SendItem(i," something.\n");
			}
			else
			{
				if(UserOf(i)>=0)
					SetItData((short)UserOf(i),
						thing,thing->it_Adjective,thing->it_Noun);
				SendItem(i," %s.\n",NameOf(thing));
			}
		}
ne:		ct++;
	}
}		

void DoesToPlayer(user,flag,thing,msg,p1,p2,p3,p4,p5,p6)
register ITEM *user;
int flag;
ITEM *thing;
/* 1=Someone 0=Nothing if cant see 	*/
/* 2=Nothing if deaf 			*/
/* 4=Must Be Present 			*/
/* 8=Always if target */
char *msg,*p1,*p2,*p3,*p4,*p5,*p6;
{
	register int ct=0;
	register ITEM *i;
	while(ct<MAXUSER)
	{
		i=UserList[ct].us_Item;
		if((i)&&(i!=user))
		{
			if(flag&4)
			{
				if(O_PARENT(i)!=O_PARENT(user))
					goto ne;
			}
			if((flag&8)&&i==thing)
				goto uncst;
			if(IsDeaf(i)&&(flag&2))
				goto ne;
			if(IsBlind(i)||(CanSee(LevelOf(i),user)==0&&
				CanSee(LevelOf(i),thing)==0))
			{
				if(flag&1)
					goto ne;
			}
uncst:			if((IsBlind(i))||(CanSee(LevelOf(i),user)==0))
			{
				if(IsPlayer(user))
					SendItem(i,"Someone ");
				else
					SendItem(i,"Something ");
				SendItem(i,msg,p1,p2,p3,p4,p5,p6);
				goto it1;
			}
			SendItem(i,"%s ",CNameOf(user));
			if(UserOf(i)>=0)
				SetItData((short)UserOf(i),
					user,user->it_Adjective,user->it_Noun);
			SendItem(i,msg,p1,p2,p3,p4,p5,p6);
it1:			if(thing==i)
				SendItem(i," you.\n");
			else
			{
				if(CanSee(LevelOf(i),thing)==0||IsBlind(i))
				{
					if(IsPlayer(i))
						SendItem(i," someone.\n");
					else
						SendItem(i," something.\n");
				}
				else
				{
					if(UserOf(i)>=0)
						SetItData((short)UserOf(i),
						thing,thing->it_Adjective,thing->it_Noun);
					SendItem(i," %s.\n",NameOf(thing));
				}
			}
		}
ne:		ct++;
	}
}		


ITEM *FindSomething(i,defloc)
ITEM *i;
ITEM *defloc;
{
/*
 *	Standard routine for finding item, checking specific room first, then 
 *	whole game, used by editing commands.
 */
	register ITEM *a;
	char *r;
	int refid=0;
	int ad,no;
	r=WordPtr;
	GetParsedWord();
	if(*WordBuffer=='#')	/* #n item */
	{
		if(sscanf(WordBuffer,"#%d",&refid)==0)
			return(NULL);
	}
	else
		WordPtr=r;
	if(GetThing(&ad,&no)==-1)
	{
		return(NULL);
	}
	if(refid==0)
	{
		a=FindIn(LevelOf(i),defloc,(short)ad,(short)no);
		if(a)
			return(a);
		a=FindMaster(LevelOf(i),(short)ad,(short)no);
		return(a);
	}
	else
	{
		a=FindMaster(LevelOf(i),(short)ad,(short)no);
		while((--refid)&&(a))
			a=NextMaster(LevelOf(i),a,(short)ad,(short)no);
		return(a);
	}
}

void SetPrompt(i,p)	/* Set the prompt a user has on input line */
ITEM *i;
char *p;
{
	short v=(short)UserOf(i);
	if(v!=-1)
	{
		SendTPacket(UserList[v].us_Port,PACKET_SETPROMPT,p);
	}
}

ITEM *ExitOf(i,d)	/* Return the exit in direction d. This routine should
			   be in InsAndOuts.c and may move one day */
ITEM *i;
unsigned int d;
{
	register ITEM *x;
	GENEXIT *g;
	g=(GENEXIT *)FindSub(i,KEY_GENEXIT);
	if(g==NULL)
		return(NULL);
	if(d>11)
		return(NULL);
	x=g->ge_Dest[(unsigned short)d];
	if(x==NULL)
		return(NULL);
	if(IsRoom(x))
		return(x);
	if(O_STATE(x)!=0)
		return(NULL);
	return(O_PARENT(x));
}

ITEM *DoorOf(i,d)		/* Find the door in direction - comments as above */
ITEM *i;
unsigned int d;
{
	register ITEM *x;
	register GENEXIT *g;
	g=(GENEXIT *)FindSub(i,KEY_GENEXIT);
	if(g==NULL)
		return(NULL);
	x=g->ge_Dest[(unsigned short)d];
	if(x==NULL)
		return(NULL);
	if(IsRoom(x))
		return(NULL);
	return(x);
}


void Broadcast(msg,flag)
int flag;	/* 1=Someone 0=Nothing if cant see */
		/* 2=Nothing if deaf */
		/* 4=Must Be Present */
register char *msg;
{
/*
 *	Send a message to 'everyone'
 */
	register short ct=0;
	register ITEM *i;
	while(ct<MAXUSER)
	{
		i=UserList[ct].us_Item;
		if(i)
		{
			if(!(IsDeaf(i)&&(flag&2)))
				SendItem(i,"%s\n",msg);
		}
		ct++;
	}
}

void SendEdit(i,x,a1,a2,a3,a4,a5,a6,a7)
ITEM *i;
char *x;
char *a1,*a2,*a3,*a4,*a5,*a6,*a7;	/* VARARGS */
{
/*
 *	Send a string to an item, to be edited.
 */
	static char sbf[512];
	short u=(short)UserOf(i);
	if(u==-1)
		return;
	sprintf(sbf,x,a1,a2,a3,a4,a5,a6,a7);
	SendTPacket(UserList[u].us_Port,PACKET_EDIT,sbf);
}

void TimeOut(u)
int u;
{
/*
 *	Time out controller
 */
	char x[MAXNAME+30];
	if(u<0)
		return;
	if(UserList[(unsigned short)u].us_Port!=NULL)
		CloseMPort(UserList[(unsigned short)u].us_Port);	/* Bye bye */
	UserList[u].us_Port=NULL;				/* Port now free */
	if(*UserList[u].us_Name==0)
	{
		return;
	}
	sprintf(x,"%s has been timed out.",UserList[u].us_Name);
	RemoveUser(u);
	Broadcast(x,0);
}

void SetUserTitle(i,p,x)	/* AberMUD gfx drivers */
ITEM *i;
char *p,*x;
{
	char a[128];
	short v=(short)UserOf(i);
	if(v!=-1)
	{
		sprintf(a,p,x);
		SendTPacket(UserList[v].us_Port,PACKET_SETTITLE,a);
	}
}


int IsLit(i)		/* Check if an item is lit - should be in DarkLight.c */
ITEM *i;
{
	register OBJECT *o=ObjectOf(i);
	if(!o)
		return(0);
	if(o->ob_Flags&OB_LIGHTSOURCE)
		return(1);
	if(o->ob_Flags&OB_LIGHT0)
		return((O_STATE(i)==0));
	return(0);
}

int IsUnique(short pe, short a, short n)	
/* True if adj noun refer to unique item  (for perception pe) */
{
	register ITEM *x=FindMaster(pe,a,n);
	if(x==NULL)
		return(1);
	if(NextMaster(pe,x,a,n))
		return(0);
	return(1);
}

int ItemNumber(short pe, ITEM *i)		/* Get number (#n) of an item */
{
	register ITEM *j;
	register short ct=2;
	j=FindMaster(pe,i->it_Adjective,i->it_Noun);
	if(j==i)
		return(1);
	if(j==NULL)
		return(-1);
	while((j=NextMaster(pe,j,i->it_Adjective,i->it_Noun)))
	{
		if(i==j)
			return(ct);
		ct++;
	}
	return(-1);
}
