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

/*
 *	Everything to do with editing in general
 *
 *	1.00	AGC	Original Version
 *	1.01	AGC	Added more commands
 *	1.02	AGC	Automatic word numbers
 *	1.03	AGC	More commands
 *	1.04	AGC 	More commands again
 *	1.05	AGC 	5.06 tidy up
 *	1.06	AGC	5.07 changes - named tables etc
 *	1.07	AGC	Addword speeded up using sorted word lists
 *	1.08	AGC	Fixed verb addition, added more info to status
 *	1.09	AGC	TabName function made globally useable.. should move file
 *	1.10	AGC	Support for Superclassing
 */

Module "Editing Routines";
Version "1.09";
Author "Alan Cox";


extern USER UserList[];

char CmdBuffer[512];	/* No longer static - shared by BSX commands */


void Cmd_Exorcise(i)
ITEM *i;
{
	ITEM *j=FindSomething(i,O_PARENT(i));
	if(!ArchWizard(i))
	{
		SendItem(i,"Huh ?\n");
		return;
	}
	if(!j)
	{
		SendItem(i,"What is that ?\n");
		return;
	}
	if(!IsUser(j)&&IsPlayer(j))
	{
		SendItem(i,"You can only exorcise users.\n");
		return;
	}
	if(IsPlayer(j))
	{
		if(UserList[UserOf(j)].us_Item!=j)
		{
			SendItem(i,"%s has been exorcised.\n",CNameOf(j));
			UnlockItem(j);
			PlayerOf(j)->pl_UserKey=-1;
			SendItem(i,"Warning: A noun for the player name may still be present.\n");
			SendItem(i,"Character is now an ordinary mobile.\n");
			return;
		}
	}
#ifdef IS_AN_EXPERT
	UnlockItem(j);
	SendItem(i,"One lock broken on item.\n");
	SendItem(i,"This may leave your universe corrupted!\n");
#else
	SendItem(i,"IS_AN_EXPERT option must be set to compile in Exorcise item.\n");
#endif
}
	
void Cmd_Abort(ITEM *i)
{
	if(ArchWizard(i))
	{
		Log("Aborted by order of %s",TextOf(i->it_Name));
		exit(0);
	}
	SendItem(i,"You look around frantically for a big red off button, but \
can't find one!\n");
}


void Cmd_AddWord(ITEM *i, short type)
{
	int x;
	if(ArchWizard(i))
	{
		if(GetWord()==(WLIST *)(-1))
		{
			SendItem(i,"Yes, but what word shall I add ?\n");
			return;
		}
		strcpy(CmdBuffer,WordBuffer);
		x=GetNumber();
		if(x==-1)
		{
			x=FindFreeWord(type);
			if(x==-1)
			{
				SendItem(i,"You must specify a code for the word.\n");
				return;
			}
		}
		if(FindInList(WordList,CmdBuffer,type))
		{
			SendItem(i,"'%s' is already defined.\n",CmdBuffer);
			return;
		}
		AddWord(CmdBuffer,(short)x,(short)type);
	}
	else
	 SendItem(i,"Wouldn't it be easier to use the ones I've given you.\n");
}

int FindFreeWord(short t)
{
	int n=0;
	WLIST *w;
	if(t==WD_VERB)
		n=200;
	w=WordList;
	while(w)
	{
		if(n<0)
			return(-1);
/*
 *	Since the list is sorted we can spot gaps easily.
 */			
		if(w->wd_Type==t)	
		{
			if(w->wd_Code>n+1)
				return(n+1);
			if(t!=WD_VERB||w->wd_Code>199)
				n=w->wd_Code;
		}
		w=w->wd_Next;
	}
	return(n+1);
}
	
void Cmd_AddVerb(i)
ITEM *i;
{
	Cmd_AddWord(i,(short)WD_VERB);
}

void Cmd_AddNoun(i)
ITEM *i;
{
	Cmd_AddWord(i,(short)WD_NOUN);
}

void Cmd_AddAdj(i)
ITEM *i;
{
	Cmd_AddWord(i,(short)WD_ADJ);
}

void Cmd_AddPrep(i)
ITEM *i;
{
	Cmd_AddWord(i,(short)WD_PREP);
}

void Cmd_AddPronoun(i)
ITEM *i;
{
	Cmd_AddWord(i,(short)WD_PRONOUN);
}

void Cmd_AddOrdinate(i)
ITEM *i;
{
	Cmd_AddWord(i,WD_ORDIN);
}

char *TabName(x)
int x;
{
	static char a[8];
	TABLE *t=FindTable(x);
	if(!t||t->tb_Name==NULL)
	{
		sprintf(a,"%3d",x);
		return(a);
	}
	return(TextOf(t->tb_Name));
}
	
void Cmd_ItemInfo(i)
ITEM *i;
{
	ITEM *b;
	SUB *s;
	if(!ArchWizard(i))
	{
		SendItem(i,"'I' means inventory, does 'II' mean a dicky keyboard ?\n");
		return;
	}
	b=FindSomething(i,O_PARENT(i));
	if(!b)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	SendItem(i,"Item: #%d %s\n",ItemNumber(LevelOf(i),b),CNameOf(b));
	if(O_PARENT(b))
		SendItem(i,"Parent: #%d %s\n",ItemNumber(LevelOf(i),O_PARENT(b)),
					CNameOf(O_PARENT(b)));
	else
		SendItem(i,"Parent: None\n");
	if(O_CHILDREN(b))
		SendItem(i,"First Child: %s\n",CNameOf(O_CHILDREN(b)));
	else
		SendItem(i,"First Child: None\n");
	if(O_NEXT(b))
		SendItem(i,"Next Chained: %s\n",CNameOf(O_NEXT(b)));
	else
		SendItem(i,"Next Chained: None\n");
	SendItem(i,"Adjective : %3d  Noun: %3d  Actor: %s  Action: %s  State: %3d\n",
		(int)b->it_Adjective,(int)b->it_Noun,
		TabName((int)b->it_ActorTable),
		TabName((int)b->it_ActionTable),(int)b->it_State);
	SendItem(i,"Perception: %3d  Lock: %3d\nClasses: ",
		(int)b->it_Perception,(int)b->it_Users);
	ClassDescStr(i,b->it_Class);
	SendItem(i,"\n");
	s=b->it_Properties;
	while(s)
	{
		switch(s->pr_Key)
		{
			case KEY_ROOM:SendItem(i,"Room");
					break;
			case KEY_OBJECT:SendItem(i,"Object");
					break;
			case KEY_PLAYER:SendItem(i,"Player");
					break;
			case KEY_GENEXIT:SendItem(i,"GenExit");
					break;
			case KEY_MSGEXIT:SendItem(i,"MsgExit");
					break;
			case KEY_CONDEXIT:SendItem(i,"CondExit");
					break;
			case KEY_CONTAINER:SendItem(i,"Container");
					break;
			case KEY_CHAIN:SendItem(i,"Chain -> #%d %s",
				ItemNumber(LevelOf(i),
					((CHAIN *)(s))->ch_Chained),
				CNameOf(((CHAIN *)(s))->ch_Chained));
					break;
			case KEY_USERFLAG:SendItem(i,"UserFlag");
					break;
			case KEY_BACKTRACK:SendItem(i,"BackTrack");
					break;
			case KEY_SNOOP:SendItem(i,"Snoop");
					break;
			case KEY_SNOOPBACK:
					SendItem(i,"SnoopBack");
					break;
			case KEY_ROPE:
					SendItem(i,"Rope");
					break;
			case KEY_DUPED:
					SendItem(i,"Dup of %s",
				CNameOf(((DUP *)(s))->du_Master));
					break;
			case KEY_TIECHAIN:
					SendItem(i,"Tied");
					break;
			case KEY_INHERIT:
					SendItem(i,"Inerit from #%d %s",
				ItemNumber(LevelOf(i),
					((INHERIT *)(s))->in_Master),
				CNameOf(((INHERIT *)(s))->in_Master));
					break;
			case KEY_INOUTHERE:
					SendItem(i,"InOutHere");
					break;
			case KEY_USERTEXT:
					SendItem(i,"UserText");
					break;
			case KEY_USERFLAG2:
					SendItem(i,"UserFlag2");
					break;
					
			default:SendItem(i,"Unknown - %d\n",s->pr_Key);
		}			
		SendItem(i,"\n");
		s=s->pr_Next;
	}
}

void Cmd_ListItems(i)
ITEM *i;
{
	ITEM *a=ItemList;
	if(ArchWizard(i))
	{
		while(a)
		{
			SendItem(i,"%s\n",CNameOf(a));
			a=a->it_MasterNext;
		}
		SendItem(i,"%ld items.\n",CountItems());
	}
	else
		SendItem(i,"Try INVentory.\n");
}


void Cmd_SetState(i)
ITEM *i;
{
	ITEM *a;
	int v;
	if(!ArchWizard(i))
	{
		SendItem(i,"Wasn't he an Egyptian god ?\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	SkipPrep();
	if((v=GetNumber())==-1)
	{
		SendItem(i,"To which state ?\n");
		return;
	}
	if((v<0)||(v>3))
	{
		SendItem(i,"States range from 0-3\n");
		return;
	}
	a->it_State=v;	/* Doesnt go thru full SetState() drivers */
}

void Cmd_SetPerception(i)
ITEM *i;
{
	ITEM *a;
	int v;
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
	if((v=GetNumber())==-1)
	{
		SendItem(i,"To what value ?\n");
		return;
	}
	if((v<0)||(v>32765))
	{
		SendItem(i,"Perception ranges from 0-32765\n");
		return;
	}
	a->it_Perception=v;	/* Doesnt go thru full SetState() drivers */
}


void Cmd_SetName(i)
ITEM *i;
{
	ITEM *a;
	if(!ArchWizard(i))
	{
		SendItem(i,"Sorry no name changing round here.\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	SkipPrep();
	GetAll();
	if(!strlen(WordBuffer))
	{
		SendEdit(i,"SetName #%d %s %s",ItemNumber(LevelOf(Me()),a),CNameOf(a),NameOf(a));
		return;
	}
	SetName(a,WordBuffer);
}

void Cmd_NewItem(i)
ITEM *i;
{
	int ad,no;
	ITEM *it;
	char namebuf[128];
	if(!ArchWizard(i))
	{
		if((GetParsedWord())&&(stricmp(WordBuffer,"draught")==0))
		{
			SendItem(i,"You wave your arms around and create a draught.\n");
			return;
		}
		SendItem(i,"Create! The only thing you could create is a draught.\n");
		return;
	}
	if(GetThing(&ad,&no)==-1)
	{
		SendItem(i,"There are undefined words in your item.\n");
		return;
	}
	if(no==-1)
	{
		SendItem(i,"Invalid noun.\n");
		return;
	}
	if(ad==-1)
		sprintf(namebuf,"the %s",FindWText(no,WD_NOUN));
	else
		sprintf(namebuf,"the %s %s",FindWText(ad,WD_ADJ),
					    FindWText(no,WD_NOUN));
	it=CreateItem(namebuf,ad,no);
	SendItem(i,"%s has been created.(AD=%d)(NO=%d)\n",CNameOf(it),ad,no);
}

void Cmd_DelItem(i)
ITEM *i;
{
	ITEM *a;
	if(!ArchWizard(i))
	{
		SendItem(i,"Sure.....\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	if(a->it_Users)
	{
		SendItem(i,"The item still has %d locks into reality\n",
			a->it_Users);
		return;
	}
	UnUserFlag(a);
	UnGenExit(a);
	KillIOH(a);
	UnUserText(a);
	if(O_PROPERTIES(a))
	{
		SendItem(i,"The item still has properties.\n");
		return;
	}
	if(!O_EMPTY(a))
	{
		SendItem(i,"And what fate shall befall those things that lie within it ?\n");
		return;
	}
	UnlinkItem(a);
	FreeItem(a);
	SendItem(i,"It is no more.\n");
}

void Cmd_BeRoom(i)
ITEM *i;
{
	ITEM *a;
	if(!ArchWizard(i))
	{
		SendItem(i,"Everything round here will be what it wants!\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	if(RoomOf(a))
	{
		SendItem(i,"It already is.\n");
		return;
	}
	MakeRoom(a);
	SendItem(i,"%s is now a room.\n",NameOf(a));
}

void Cmd_BeObject(i)
ITEM *i;
{
	char *t=WordPtr;	/* Remember so we can call EditObj later */
	ITEM *a;
	if(!ArchWizard(i))
	{
		SendItem(i,"Everything round here will be what it wants!\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	if(ObjectOf(a))
	{
		SendItem(i,"It already is.\n");
		return;
	}
	MakeObject(a);
	SendItem(i,"%s is now an object.\n",NameOf(a));
	WordPtr=t;
	Cmd_ObjEdit(i);	/* Edit it up... */
}

void Cmd_BePlayer(i)
ITEM *i;
{
	ITEM *a;
	if(!ArchWizard(i))
	{
		SendItem(i,"Everything round here will be what it wants!\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	if(PlayerOf(a))
	{
		SendItem(i,"It already is.\n");
		return;
	}
	MakePlayer(a);
	SendItem(i,"%s is now a player.\n",NameOf(a));
}

void Cmd_BeContainer(i)
ITEM *i;
{
	ITEM *a;
	if(!ArchWizard(i))
	{
		SendItem(i,"Everything round here will be what it wants!\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	if(ContainerOf(a))
	{
		SendItem(i,"It already is.\n");
		return;
	}
	BeContainer(a);
	SendItem(i,"%s is now a container.\n",NameOf(a));
}

void Cmd_UnRoom(i)
ITEM *i;
{
	ITEM *a;
	if(!ArchWizard(i))
	{
		SendItem(i,"Everything round here will be what it wants!\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	if(!RoomOf(a))
	{
		SendItem(i,"That is not a room anyway.\n");
		return;
	}
	UnRoom(a);
	SendItem(i,"%s is no longer a room.\n",NameOf(a));
}

void Cmd_UnObject(i)
ITEM *i;
{
	ITEM *a;
	if(!ArchWizard(i))
	{
		SendItem(i,"Everything round here will be what it wants!\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	if(!ObjectOf(a))
	{
		SendItem(i,"That is not an object.\n");
		return;
	}
	UnObject(a);
	SendItem(i,"%s is no longer an object.\n",NameOf(a));
}

void Cmd_UnPlayer(i)
ITEM *i;
{
	ITEM *a;
	if(!ArchWizard(i))
	{
		SendItem(i,"Everything round here will be what it wants!\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	if(!PlayerOf(a))
	{
		SendItem(i,"That is not a player.\n");
		return;
	}
	if(PlayerOf(a)->pl_UserKey!=-1)
	{
		SendItem(i,"\
That cannot be done, for it is said that upon the time that a user is not a\n\
player, the fourth guru shall descend upon the world...\n");
		return;
	}
	UnPlayer(a);
	SendItem(i,"%s is no longer a player.\n",NameOf(a));
}

void Cmd_UnContainer(i)
ITEM *i;
{
	ITEM *a;
	if(!ArchWizard(i))
	{
		SendItem(i,"Everything round here will be what it wants!\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	if(!ContainerOf(a))
	{
		SendItem(i,"That is not a container.\n");
		return;
	}
	UnContainer(a);
	SendItem(i,"%s is no longer a container.\n",NameOf(a));
}

void Cmd_SaveUniverse(i)
ITEM *i;
{
	int x=UserOf(i);
	int v;
	if(!ArchWizard(i))
	{
		SendItem(i,"Who do you think you are , superman ?\n");
		return;
	}
	if(CountUsers()!=1)
	{
		SendItem(i,"Sorry, you must be alone in the game to do this. \n");
		return;
	}
	DisintegrateAll();
	DisintegrateAll();	/* 2nd in case it mops up 1st lot */
	SendItem(i,"You have been removed from the universe. \n");
	SetPrompt(i,"Relogin : ");
	ExitUser(x);
	GetAll();
	v=SaveSystem(WordBuffer);
	switch(v)
	{
		case 0:SendUser(x,"Save completed.\n");break;
		case -1:SendUser(x,"Save failed: Can't create .bak file.\n");
			break;
		case -2:SendUser(x,"Save failed: Cant't open save file.\n");
			break;
		default:SendUser(x,"Save failed: Unix Error Code=%d.\n",
					v);
	}
	SetMe(NULL);
	AddEvent(0,1);	/* Run Table 1 on re-entry */
}

void Cmd_StatMe(i)
ITEM *i;
{
	if(!ArchWizard(i))
		SendItem(i,"Go away, Peasant!\n");
}

void Cmd_ListWord(i)
ITEM *i;
{
	int wtype=-1;
	int wcd=1;
	char *wptr;
	extern WLIST *WordList;
	WLIST *w;
	if(!ArchWizard(i))
	{
		SendItem(i,"When the boss is away, the mortals will play....\n");
		return;
	}
	GetAll();
	wptr=WordBuffer;
	if(*wptr=='#')
	{
		if(sscanf(wptr+1,"%d",&wcd)==0)
		{
			SendItem(i,"Invalid number!\n");
			return;
		}
		goto l2;
	}
	if(*wptr=='$')
	{
		wptr++;
		switch(*wptr)
		{
			case 'V':wtype=WD_VERB;wptr++;break;
			case 'N':wtype=WD_NOUN;wptr++;break;
			case 'A':wtype=WD_ADJ;wptr++;break;
			case 'P':wtype=WD_PREP;wptr++;break;
			case 'n':wtype=WD_NOISE;wptr++;break;
			case 'p':wtype=WD_PRONOUN;wptr++;break;
		}
	}
/*
 *	Now list matches
 */
	w=WordList;
	while(w)
	{
		if(((w->wd_Type==wtype)||(wtype==-1))&&
		  ((strcmp(wptr,"")==0)||
		   (stricmp(wptr,w->wd_Text)==0)))
		{
			SendItem(i,"%s %c %d\n",
				w->wd_Text,
				"?NPpCVAnO"[w->wd_Type],
				w->wd_Code);
		}
		w=w->wd_Next;
	}
	return;
l2:	w=WordList;
	while(w)
	{
		if(w->wd_Code==wcd)
		{
			SendItem(i,"%s %c %d\n",
				w->wd_Text,
				"?NPpCVAnO"[w->wd_Type],
				w->wd_Code);
		}
		w=w->wd_Next;
	}
}

void Cmd_DelWord(ITEM *i, short type)
{
	if(ArchWizard(i))
	{
		if(GetWord()==(WLIST *)(-1))
		{
			SendItem(i,"Yes, but what word shall I delete ?\n");
			return;
		}
		strcpy(CmdBuffer,WordBuffer);
		if(FindInList(WordList,CmdBuffer,type)==0)
		{
			SendItem(i,"'%s' is not defined.\n",CmdBuffer);
			return;
		}
		FreeWord(CmdBuffer,(short)type);
	}
	else
		SendItem(i,"No chance buster.\n");
}

void Cmd_DelVerb(i)
ITEM *i;
{
	Cmd_DelWord(i,(short)WD_VERB);
}

void Cmd_DelPronoun(i)
ITEM *i;
{
	Cmd_DelWord(i,(short)WD_PRONOUN);
}

void Cmd_DelNoun(i)
ITEM *i;
{
	Cmd_DelWord(i,(short)WD_NOUN);
}

void Cmd_DelAdj(i)
ITEM *i;
{
	Cmd_DelWord(i,(short)WD_ADJ);
}

void Cmd_DelPrep(i)
ITEM *i;
{
	Cmd_DelWord(i,(short)WD_PREP);
}

void Cmd_DelOrdinate(i)
ITEM *i;
{
	Cmd_DelWord(i,WD_ORDIN);
}

void Cmd_Rename(i)
ITEM *i;
{
	int ad,no;
	ITEM *x;
	if(!ArchWizard(i))
	{
		SendItem(i,"What makes you think you can rename things?\n");
		return;
	}
	x=FindSomething(i,O_PARENT(i));
	if(!x)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	SkipPrep();
	if(GetThing(&ad,&no)==-1)
	{
		SendItem(i,"There are undefined words in your new name.\n");
		return;
	}
	O_ADJECTIVE(x)=ad;
	O_NOUN(x)=no;
	SendItem(i,"It has been changed.\n");
}

void Cmd_Chain(i)
ITEM *i;
{
	ITEM *x,*y;
	if(!ArchWizard(i))
	{
		SendItem(i,"What makes you think you can chain things?\n");
		return;
	}
	x=FindSomething(i,O_PARENT(i));
	if(!x)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	SkipPrep();
	y=FindSomething(i,O_PARENT(i));
	if(!y)
	{
		SendItem(i,"I don't know what you are trying to chain it to.\n");
		return;
	}
	AddChain(x,y);
	SendItem(i,"%s is now chained to %s.\n",CNameOf(x),NameOf(y));
}

void Cmd_UnChain(i)
ITEM *i;
{
	ITEM *x,*y;
	if(!ArchWizard(i))
	{
		SendItem(i,"What makes you think you can unchain things?\n");
		return;
	}
	x=FindSomething(i,O_PARENT(i));
	if(!x)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	SkipPrep();
	y=FindSomething(i,O_PARENT(i));
	if(!y)
	{
		SendItem(i,"I don't know what you are trying to unchain.\n");
		return;
	}
	if(RemoveChain(x,y))
	{
		SendItem(i,"%s is not chained to %s.\n",CNameOf(x),NameOf(y));
		return;
	}
	SendItem(i,"%s is no longer chained to %s.\n",CNameOf(x),NameOf(y));
}

void Cmd_UFlagShow(i)
ITEM *i;
{
	ITEM *x,*y;
	int c=0;;
	if(!ArchWizard(i))
	{
		SendItem(i,"What are you gibbering about now ?\n");
		return;
	}
	x=FindSomething(i,O_PARENT(i));
	if(!x)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}	
	while(c<15)
	{
		SendItem(i,"Flag %d : %d    Flag %d : %d    ",
			c,GetUserFlag(x,c),c+1,GetUserFlag(x,c+1));
		SendItem(i,"Flag %d : %d    Flag %d : %d\n",
			c+2,GetUserFlag(x,c+2),c+3,GetUserFlag(x,c+3));
		c+=4;
	}
	c=0;
	while(c<15)
	{
		y=GetUserItem(x,c);
		if(y!=NULL)
			SendItem(i,"Entry %d : %s\n",c,CNameOf(y));
		c++;
	}
	c=0;
	while(c<8)
	{
		SendItem(i,"Text %d : %s\n",c,GetUText(x,c));
		c++;
	}
}

void Cmd_SetUFlag(i)
ITEM *i;
{
	ITEM *x;
	int c,n;
	if(!ArchWizard(i))
	{
		SendItem(i,"What makes you think you can do that ?\n");
		return;
	}
	x=FindSomething(i,O_PARENT(i));
	if(!x)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}	
	SkipPrep();
	c=GetNumber();
	if(c==-1)
	{
		SendItem(i,"You must specify which flag (0-15)\n");
		return;
	}
	if((c<0)||(c>15))
	{
		SendItem(i,"Flag range is 0-15\n");
		return;
	}
	n=GetNumber();
	if(n==-1)
	{
		SendItem(i,"You must specify a number to set it to.\n");
		return;
	}
	SetUserFlag(x,c,n);
}

void Cmd_SetUItem(i)
ITEM *i;
{
	ITEM *x,*n;
	int c;
	if(!ArchWizard(i))
	{
		SendItem(i,"What makes you think you can do that ?\n");
		return;
	}
	x=FindSomething(i,O_PARENT(i));
	if(!x)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}	
	SkipPrep();
	c=GetNumber();
	if(c==-1)
	{
		SendItem(i,"You must specify which flag (0-7)\n");
		return;
	}
	if((c<0)||(c>15))
	{
		SendItem(i,"Flag range is 0-15\n");
		return;
	}
	n=FindSomething(i,O_PARENT(i));
	if(n==NULL)
	{
		SendItem(i,"Setting to NULL item.\n");
	}
	SetUserItem(x,c,n);
}

void Cmd_ShowFlag(i)
ITEM *i;
{
	int st,en;
	short s1=0,s2=0;
	if(!ArchWizard(i))
	{
		SendItem(i,"You have no flags worth showing to anyone!\n");
		return;
	}
	GetAll();
	switch(sscanf(WordBuffer,"%d,%d",&st,&en))
	{
		case 1:en=st;break;
		case 2:break;
		default:SendItem(i,"You must specify which flags (from,to)\n");
			return;
	}
	if((st<0)||(en>511)||(en<st))
	{
		SendItem(i,"The range specified is invalid.\n");
		return;
	}
	s1=st;
	while(s1<=en)
	{
		SendItem(i,"%-3d=%-5d        ",s1,GetFlag(s1));
		if(s2%4==3)
			SendItem(i,"\n");
		s2++;
		s1++;
	}
	if(s2%4!=0)			/* Print CR unless just done so */
		SendItem(i,"\n");
}

void Cmd_SetFlag(i)
ITEM *i;
{
	int fl,v;
	if(!ArchWizard(i))
	{
		SendItem(i,"Try setting jelly - it's more your level.\n");
		return;
	}
	GetAll();
	if(sscanf(WordBuffer,"%d %d",&fl,&v)!=2)
	{
		SendItem(i,"You must specify a flag and a value.\n");
		return;
	}
	if((fl<0)||(fl>511))
	{
		SendItem(i,"Flag out of range.\n");
		return;
	}
	SetFlag(fl,v);
}

void Cmd_Share(i)
ITEM *i;
{
	ITEM *a,*b;
	if(!ArchWizard(i))
	{
		SendItem(i,"Share and enjoy...\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	SkipPrep();
	b=FindSomething(i,O_PARENT(i));
	if(b==NULL)
	{
		SendItem(i,"I don't know what you want %s to inherit.\n",
			NameOf(a));
		return;
	}
	if(Inheritor(b))
	{
		SendItem(i,"Warning: %s inherits from %s. This inheritance\
 will not be passed onto %s.\n",CNameOf(b),NameOf(Inheritor(b)),
			NameOf(a));
	}
	if(Inheritor(a))
	{
		SendItem(i,"%s no longer inherits from %s.\n",
			CNameOf(a),NameOf(Inheritor(a)));
	}
	MakeInherit(a,b);
	SendItem(i,"%s now inherits from %s.\n",CNameOf(a),NameOf(b));
}

void Cmd_UnShare(i)
ITEM *i;
{
	ITEM *a;
	if(!ArchWizard(i))
	{
		SendItem(i,"Ok, don't share and enjoy then...\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	UnInherit(a);
	SendItem(i,"%s no longer inherits.\n",CNameOf(a));
}	

void Cmd_Status(i)
ITEM *i;
{
	extern long MemFree();
	long n[25];
	ITEM *w=ItemList;
	SUB *s;
	long t;
	time(&t);
	SendItem(i,"Creator Of Legends - Version 5.21\n\n");
	SendItem(i,"System Status at %s\n",ctime(&t));
#ifdef CHECK_TXT
	SendItem(i,"Text Pointer Validation: ON\n");
#else
	SendItem(i,"Text Pointer Validation: OFF\n");
#endif
#ifdef CHECK_ITEM
	SendItem(i,"Item Pointer Validation: ON\n");
#else
	SendItem(i,"Item Pointer Validation: OFF\n");
#endif
	n[0]=0;n[1]=0;n[2]=0;n[3]=0;n[4]=0;
	n[5]=0;n[6]=0;n[7]=0;n[8]=0;n[9]=0;
	n[10]=0;n[11]=0;n[12]=0;n[13]=0;n[14]=0;
	n[15]=0;n[16]=0;n[17]=0;n[18]=0;n[19]=0;
	n[20]=0;n[21]=0;n[22]=0;n[23]=0;n[24]=0;
	while(w)
	{
		s=w->it_Properties;
		while(s)
		{
			if(s->pr_Key==KEY_INHERIT)
				n[0]++;
			else
				n[s->pr_Key]++;
			s=s->pr_Next;
		}
		w=w->it_MasterNext;
	}
	SendItem(i,"Number Of Items       : %ld\n",CountItems());
	SendItem(i,"Number Of Rooms       : %ld\n",n[KEY_ROOM]);
	SendItem(i,"Number Of Objects     : %ld\n",n[KEY_OBJECT]);
	SendItem(i,"Number Of Players     : %ld\n",n[KEY_PLAYER]);
	SendItem(i,"Number Of GenExits    : %ld\n",n[KEY_GENEXIT]);
	SendItem(i,"Number Of MsgExits    : %ld\n",n[KEY_MSGEXIT]);
	SendItem(i,"Number Of CondExits   : %ld\n",n[KEY_CONDEXIT]);
	SendItem(i,"Number Of Containers  : %ld\n",n[KEY_CONTAINER]);
	SendItem(i,"Number Of Chains      : %ld\n",n[KEY_CHAIN]);
	SendItem(i,"Number Of UserFlags   : %ld\n",n[KEY_USERFLAG]);
	SendItem(i,"Number Of Snoops      : %ld\n",n[KEY_SNOOP]);
	SendItem(i,"Number Of InoutHeres  : %ld\n",n[KEY_INOUTHERE]);
	SendItem(i,"Number Of Dups        : %ld\n",n[KEY_DUPED]);
	SendItem(i,"Number Of UserTexts   : %ld\n",n[KEY_USERTEXT]);
	SendItem(i,"Number Of UserFlag2s  : %ld\n",n[KEY_USERFLAG2]);
	SendItem(i,"Number Of Shares      : %ld\n",n[0]);
	SendItem(i,"Number Of Schedules   : %ld\n",CountSchedules());
	if(MemFree()!=-1)
		SendItem(i,"Free Heap Space       : %ld\n",MemFree());
}


void Cmd_TrackFlag(i)
ITEM *i;
{
	int v=GetNumber();
	short c=0;
	if(!ArchWizard(i))
	{
		SendItem(i,"What ?\n");
		return;
	}
	Debugger=i;	/* Set debug destination */
	if(v==-1)
	{
		SendItem(i,"Which flag ?\n");
		return;
	}
	if(v<0||v>511)
	{
		SendItem(i,"Flag number out of range, range is 0-511\n");
		return;
	}
	while(c<4)
	{
		if(Traf[c]==-1)
		{
			Traf[c]=v;
			SendItem(i,"Tracking flag %d\n",v);
			return;
		}
		c++;
	}
	SendItem(i,"\
You can only track four flags at a time - stop tracking one with untrack.\n");
}

void Cmd_UnTrackFlag(i)
ITEM *i;
{
	int v=GetNumber();
	short c=0;
	if(!ArchWizard(i))
	{
		SendItem(i,"What ?\n");
		return;
	}
	if(v==-1)
	{
		SendItem(i,"Which flag ?\n");
		return;
	}
	while(c<4)
	{
		if(Traf[c]==v)
		{
			SendItem(i,"Stopped tracking flag %d\n",v);
			Traf[c]=-1;
			return;
		}
		c++;
	}
	SendItem(i,"You are not tracking flag %d\n",v);
}

void Cmd_ListTrack(i)
ITEM *i;
{
	short c=0;
	short f=0;
	if(!ArchWizard(i))
	{
		SendItem(i,"What ?\n");
		return;
	}
	while(c<4)
	{
		if(Traf[c]!=-1)
		{
			if(f==0)
			{
				f=1;
				SendItem(i,"You are tracking flag(s) %d",Traf[c]);
			}
			else
				SendItem(i,",%d",Traf[c]);
		}
		c++;
	}
	if(f)
		SendItem(i,".\n");
	else
		SendItem(i,"You are tracking no flags\n");
}

void Cmd_Debugger(i)
ITEM *i;
{
	if(ArchWizard(i))
		Debugger=i;
	else
		SendItem(i,"Such delusions of grandeur...\n");
}

void Cmd_FindItem(i)
ITEM *i;
{
	ITEM *j=FindSomething(i,O_PARENT(i));
	if(!ArchWizard(i))
	{
		SendItem(i,"You will have too look for yourself chum!\n");
		return;
	}
	if(j==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	CheckAllFor(i,j,-1);
}

void Cmd_FindFlag(i)
ITEM *i;
{
	int v=GetNumber();
	if(!ArchWizard(i))
	{
		SendItem(i,"You will have too look for yourself chum!\n");
		return;
	}
	if(v==-1)
	{
		SendItem(i,"Which flag ?\n");
		return;
	}
	if(v<0||v>511)
	{
		SendItem(i,"Flags range from 0-511\n");
		return;
	}
	CheckAllFor(i,NULL,v);
}

void Cmd_Which(i)
ITEM *i;
{
	ITEM *a=FindSomething(i,O_PARENT(i));
	if(!ArchWizard(i))
		SendItem(i,"Pardon ?\n");
	else
	{
		if(a)
			SendItem(i,"#%d %s\n",ItemNumber(LevelOf(i),a),NameOf(a));
		else
			SendItem(i,"None.\n");
	}
}

void Cmd_ShowSuperClass(i)
ITEM *i;
{
	ITEM *a=FindSomething(i,O_PARENT(i));
	ITEM *b;
	int ct=20;
	if(!ArchWizard(i))
	{
		SendItem(i,"Go to school.\n");
		return;
	}
	if(a==NULL)
	{
		SendItem(i,"What is that.\n");
		return;
	}
	if(a->it_Superclass==NULL)
	{
		SendItem(i,"%s is unclassed.\n", CNameOf(a));
		return;
	}
	b=a->it_Superclass;
	while(b)
	{
		SendItem(i,"%s has superclass %s\n",CNameOf(a),NameOf(b));
		a=b;
		b=b->it_Superclass;
		ct--;
		if(ct==0)
		{
			SendItem(i,"Classing too deep.\n");
			return;
		}
	}
}

void Cmd_SetSuperClass(i)
ITEM *i;
{
	ITEM *a=FindSomething(i,O_PARENT(i));
	ITEM *b;
	if(!ArchWizard(i))
	{
		SendItem(i,"Go to school.\n");
		return;
	}
	if(a==NULL)
	{
		SendItem(i,"What ?\n");	
		return;
	}
	b=FindSomething(i,O_PARENT(i));
	if(b==NULL)
	{
		SendItem(i,"Unknown superclass object.\n");
		return;
	}

	if(a->it_Superclass!=NULL)
	{
		UnlockItem(a->it_Superclass);
		a->it_Superclass=NULL;
	}
	if(b==a)
	{
		SendItem(i,"Item self classed.\n");
		return;
	}
	LockItem(b);
	a->it_Superclass=b;
	SendItem(i,"Ok.\n");
}	
