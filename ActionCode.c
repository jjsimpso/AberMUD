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

Module  "Action Drivers";
Version "1.20";
Author  "Alan Cox";

/*
 *	1.10		Current Command Functions
 *	1.11		SetIn SetOut SetHere fixed along with AutoVerb
 *	1.12		GetUT SetUT Cat added
 *	1.13		Added SetClass/UnSetClass/BitClear/BitSet/BitTest etc.
 *	1.14		Fixed bug with DOCLASS and TAKEOUT, made all saves use
 *			real name field only.
 *	1.15		Added Ordinates to WHATO/GETO
 *	1.16		Fixed HURT killing on an attached item
 *      1.17            INC and GETNEXT
 *	1.18		Fixed IT setting by POBJ.
 *	1.19		Merged in Amiga support
 *	1.20		ANSIfication cleanup
 */

short ClassMask;
LINE *ClassLine;
short ClassMode1,ClassMode2;

void Act_Get(void)
{
	ITEM *i=ArgItem();
	ITEM *s=O_PARENT(Me());
	OBJECT *o=ObjectOf(i);
	if(o==NULL)
	{
		if(!(ClassMode1||ClassMode2))
			SendItem(Me(),"You can't take that!\n");
		return;
	}
	if(O_PARENT(i)==Me())
	{
		SendItem(Me(),"You are already carrying %s.\n",NameOf(i));
		return;
	}
	if((o->ob_Flags&OB_CANGET)==0)
	{
		if(!(ClassMode1||ClassMode2))
			SendItem(Me(),"You can't take that.\n");
		return;
	}
	else
	{		
		switch(CanPlace(i,Me()))
		{
			case -1:SendItem(Me(),"%s is too large to handle.\n",
					CNameOf(i));
				return;
			case -2:SendItem(Me(),"%s is too heavy for you to carry.\n",
					CNameOf(i));
				return;
			case -3:if(!(ClassMode1||ClassMode2))
					SendItem(Me(),"You can't carry any more individual items.\n");
				return;
		}
		SendItem(Me(),"You take %s.\n",NameOf(i));
		DoesTo(Me(),5,i,"takes");
		s=O_PARENT(Me());
		Place(Me(),i);
		DoesTo(Me(),5,i,"picks up");
		Place(Me(),s);
		Place(i,Me());
		return;
	}
}

void Act_Drop(void)
{
	ITEM *i=ArgItem();
	ITEM *s;
	if(O_PARENT(i)!=Me())
	{
		SendItem(Me(),"You are not carrying %s.\n",NameOf(i));
		return;
	}
	Place(i,O_PARENT(Me()));
	DoesTo(Me(),5,i,"drops");
	s=O_PARENT(Me());
	Place(Me(),i);
	DoesTo(Me(),5,i,"puts down");
	Place(Me(),s);
	SendItem(Me(),"You drop %s.\n",NameOf(i));
}

void Act_Wear(void)
{
	ITEM *i=ArgItem();
	OBJECT *o=ObjectOf(i);
	if(O_PARENT(i)!=Me())
	{
		SendItem(Me(),"You are not carrying %s.\n",NameOf(i));
		return;
	}
	if((o==NULL)||((o->ob_Flags&OB_CANWEAR)==0))
	{
		if(!(ClassMode1||ClassMode2))
			SendItem(Me(),"You can't wear %s.\n",NameOf(i));
		return;
	}
	if(o->ob_Flags&OB_WORN)
	{
		if(!(ClassMode1||ClassMode2))
			SendItem(Me(),"You are already wearing %s.\n",NameOf(i));
		return;
	}
	o->ob_Flags|=OB_WORN;
	DoesTo(Me(),5,i,"wears");
	SendItem(Me(),"You wear %s.\n",NameOf(i));
}

void Act_Remove(void)
{
	ITEM *i=ArgItem();
	OBJECT *o=ObjectOf(i);
	if(O_PARENT(i)!=Me())
	{
		SendItem(Me(),"You are not carrying %s.\n",NameOf(i));
		return;
	}
	if((o==NULL)||((o->ob_Flags&OB_WORN)==0))
	{
		if(!(ClassMode1||ClassMode2))
			SendItem(Me(),"You are not wearing %s.\n",NameOf(i));
		return;
	}
	o->ob_Flags&=~OB_WORN;
	DoesTo(Me(),5,i,"takes off");
	SendItem(Me(),"You take off %s.\n",NameOf(i));
}

void Act_Create(void)
{
	ITEM *i=ArgItem();
	OBJECT *o=ObjectOf(i);
	Place(i,O_PARENT(Me()));
	if(o)
		o->ob_Flags&=~OB_DESTROYED;
}

void Act_Destroy(void)
{
	ITEM *i=ArgItem();
	OBJECT *o=ObjectOf(i);
	Place(i,NULL);
	if(o)
		o->ob_Flags|=OB_DESTROYED;
	Disintegrate(i);	/* if its a clone its a dead clone */
}

void Act_Swap(void)
{
	ITEM *a,*b,*c;
	a=ArgItem();
	b=ArgItem();
	c=O_PARENT(a);
	Place(a,O_PARENT(b));
	Place(b,c);
}

void Act_Place(void)
{
	ITEM *a=ArgItem();
	ITEM *b=ArgItem();
	Place(a,b);
}

void Act_PutIn(void)
{
	ITEM *a=ArgItem();
	ITEM *b=ArgItem();
	ITEM *s;
	CONTAINER *c=ContainerOf(b);
	if((c==NULL)||((c->co_Flags&CO_CANPUTIN)==0))
	{
		SendItem(Me(),"You can't put things in %s.\n",NameOf(b));
		ClassMode1=0;ClassMode2=0;
		return;
	}
	if(O_PARENT(a)!=Me())
	{
		SendItem(Me(),"You are not carrying %s.\n",NameOf(a));
		return;
	}
	if((c->co_Flags&CO_CLOSES)&&(O_STATE(b)!=0))
	{
		SendItem(Me(),"%s is closed.\n",CNameOf(b));
		ClassMode1=0;ClassMode2=0;	/* Cut off ALL */
		return;
	}
	if(CanPlace(a,b)<0)
	{
		SendItem(Me(),"%s will not fit in %s.\n",CNameOf(a),NameOf(b));
		return;
	}
	Place(a,b);
	SendItem(Me(),"You put %s in %s.\n",NameOf(a),NameOf(b));
	DoesTo(Me(),4,b,"puts %s in",NameOf(a));
	s=O_PARENT(Me());
	Place(Me(),a);
	DoesTo(Me(),5,b,"puts %s in",NameOf(a));
	Place(Me(),b);
	DoesTo(Me(),5,b,"puts %s in",NameOf(a));	
	Place(Me(),s);
}

void Act_TakeOut(void)
{
	ITEM *a=ArgItem();
	ITEM *s;
	ITEM *b=ArgItem();
	CONTAINER *c=ContainerOf(b);
	if((c==NULL)||((c->co_Flags&CO_CANGETOUT)==0))
	{
		SendItem(Me(),"You can't take things from %s.\n",NameOf(b));
		ClassMode1=0;ClassMode2=0;
		return;
	}
	if((c->co_Flags&CO_CLOSES)&&(O_STATE(b)!=0))
	{
		SendItem(Me(),"%s is closed.\n",CNameOf(b));
		ClassMode1=0;ClassMode2=0;
		return;
	}
	if(!IsObject(a))
	{
		if(!(ClassMode1||ClassMode2))
			SendItem(Me(),"You can't take %s.\n",CNameOf(a));
		return;
	}
	if((ObjectOf(a)->ob_Flags&OB_CANGET)==0)
	{
		if(!(ClassMode1||ClassMode2))
			SendItem(Me(),"You can't take %s.\n",CNameOf(a));
		return;
	}		
	switch(CanPlace(a,Me()))
	{
		case -1:SendItem(Me(),"%s is too large for you to manage.\n",
				CNameOf(a));
			return;
		case -2:SendItem(Me(),"%s is too heavy for you to manage.\n",
				CNameOf(a));
		case -3:if(!(ClassMode1||ClassMode2))
				SendItem(Me(),"You can't carry any more individual items.\n");
			return;
	}
	Place(a,Me());
	SendItem(Me(),"You take %s from %s.\n",NameOf(a),NameOf(b));
	DoesTo(Me(),4,b,"takes %s from",NameOf(a));
	s=O_PARENT(Me());
	Place(Me(),a);
	DoesTo(Me(),5,b,"takes %s from",NameOf(a));
	Place(Me(),b);
	DoesTo(Me(),5,b,"takes %s from",NameOf(a));	
	Place(Me(),s);
}
	
void Act_CopyOF(void)
{
	ITEM *i=ArgItem();
	int n=ArgNum();
	SetFlag(ArgNum(),GetUserFlag(i,n));
}

void Act_CopyFO(void)
{
	int n=ArgNum();
	ITEM *i=ArgItem();
	SetUserFlag(i,ArgNum(),GetFlag(n));
}

void Act_CopyFF(void)
{
	int a=ArgNum();
	int b=ArgNum();
	SetFlag(b,GetFlag(a));
}

void Act_WhatO(void)
{
	extern ITEM *Item1,*Item2;
	int a=ArgNum();
	int o=Ord1;
	if(a==1)
	{
		Item1=FindMaster(LevelOf(Me()),(short)Adj1,(short)Noun1);
		if(!Item1)
			return;
		while(--o)
		{
			Item1=NextMaster(LevelOf(Me()),Item1,(short)Adj1,
				(short)Noun1);
			if(!Item1)
				return;
		}
		return;
	}		
	else
	{
		o=Ord2;
		Item2=FindMaster(LevelOf(Me()),(short)Adj2,(short)Noun2);
		if(!Item2)
			return;
		while(--o)
		{
			Item2=NextMaster(LevelOf(Me()),Item2,(short)Adj2,
				(short)Noun2);
			if(!Item2)
				return;
		}
		return;
	}
}

void Act_GetO(void)
{
	extern ITEM *Item1,*Item2;
	int o=Ord1;
	int a=ArgNum();
	ITEM *b=ArgItem();
	if(a==1)
	{
		Item1=FindIn(LevelOf(Me()),b,(short)Adj1,(short)Noun1);
		if(!Item1)
			return;
		while(--o)
		{
			Item1=NextIn(LevelOf(Me()),Item1,(short)Adj1,
				(short)Noun1);
			if(!Item1)
				return;
		}
		return;
	}
	else
	{
		Item2=FindIn(LevelOf(Me()),b,(short)Adj2,(short)Noun2);
		if(!Item2)
			return;
		o=Ord2;
		while(--o)
		{
			Item2=NextIn(LevelOf(Me()),Item1,(short)Adj2,
				(short)Noun2);
			if(!Item2)
				return;
		}
		return;
	}

}

void Act_Weigh(void)
{
	ITEM *i=ArgItem();
	int n=ArgNum();
	SetFlag(n,WeighUp(i));
}

void Act_Set(void)
{
	SetFlag(ArgNum(),255);
}

void Act_Clear(void)
{
	SetFlag(ArgNum(),0);
}

void Act_PSet(void)
{
	PLAYER *p=PlayerOf(ArgItem());
	int n=ArgNum();
	if(p)
		p->pl_Flags|=(1<<n);
}

void Act_PClear(void)
{
	PLAYER *p=PlayerOf(ArgItem());
	int n=ArgNum();
	if(p)
		p->pl_Flags&=~(1<<n);
}

void Act_Let(void)
{
	int a=ArgNum();
	SetFlag(a,ArgNum());
}

void Act_Add(void)
{
	int a=ArgNum();
	SetFlag(a,GetFlag(a)+ArgNum());
}

void Act_Sub(void)
{
	int a=ArgNum();
	SetFlag(a,GetFlag(a)-ArgNum());
}

void Act_AddF(void)
{
	int a=ArgNum();
	SetFlag(a,GetFlag(a)+GetFlag(ArgNum()));
}

void Act_SubF(void)
{
	int a=ArgNum();
	SetFlag(a,GetFlag(a)-GetFlag(ArgNum()));
}

void Act_Mul(void)
{
	int a=ArgNum();
	SetFlag(a,GetFlag(a)*ArgNum());
}

void Act_Div(void)
{
	int a=ArgNum();
	int b=ArgNum();
	if(b==0)
	{
		Log("Division By Zero");
		SendItem(Me(),"Division By Zero!\n");
		return;
	}
	SetFlag(a,GetFlag(a)/b);
}

void Act_MulF(void)
{
	int a=ArgNum();
	SetFlag(a,GetFlag(a)*GetFlag(ArgNum()));
}

void Act_DivF(void)
{
	int a=ArgNum();
	int b=GetFlag(ArgNum());
	if(b==0)
	{
		Log("Division By Zero");
		SendItem(Me(),"Division By Zero.\n");
		return;
	}
	SetFlag(a,GetFlag(a)/b);
}

void Act_Mod(void)
{
	int a=ArgNum();
	int b=ArgNum();
	if(b==0)
	{
		SendItem(Me(),"Division By Zero In MOD.\n");
		Log("Division By 0 In MOD.\n");
		return;
	}
	SetFlag(a,GetFlag(a)%b);
}

void Act_ModF(void)
{
	int a=ArgNum();
	int b=GetFlag(ArgNum());
	if(b==0)
	{
		SendItem(Me(),"Division By Zero In MODF.\n");
		Log("Division By 0 In MODF.\n");
		return;
	}
	SetFlag(a,GetFlag(a)%b);
}

void Act_Random(void)
{
	int a=ArgNum();
	unsigned int b=ArgNum();
	unsigned int b2;
	unsigned short v;
regen:	b2=b;
	v=rand()%32768;
	if(b==0)
	{
		SendItem(Me(),"Invalid Random Range.\n");
		Log("Invalid Random Range.\n");
		return;
	}
	b=32768/b;
	if(b==0)
	{
		SendItem(Me(),"Invalid Random Range.\n");
		Log("Random Too Large.\n");
		return;
	}
	if(v/b==b2)
	{
		b=b2;
		goto regen;
	}
	SetFlag(a,(v/b));
}

void Act_Move(void)
{
	Cmd_MoveDirn(Me(),GetFlag(ArgNum()));
}

void Act_Goto(void)
{
	ITEM *i=ArgItem();
	DoesAction(Me(),4,"has left.\n");
	Place(Me(),i);
	DoesAction(Me(),4,"arrives.\n");
}

void Act_Weight(void)
{
	ITEM *i=ArgItem();
	OBJECT *o=ObjectOf(i);
	PLAYER *p=PlayerOf(i);
	int n=ArgNum();
	if(o)
		o->ob_Weight=n;
	if(p)
		p->pl_Weight=n;
}	

void Act_Size(void)
{
	ITEM *i=ArgItem();
	OBJECT *o=ObjectOf(i);
	PLAYER *p=PlayerOf(i);
	int n=ArgNum();
	if(o)
		o->ob_Size=n;
	if(p)
		p->pl_Size=n;

}

void Act_OSet(void)
{
	OBJECT *o=ObjectOf(ArgItem());
	int n=ArgNum();
	if(o)
		o->ob_Flags|=(1<<n);
}

void Act_OClear(void)
{
	OBJECT *o=ObjectOf(ArgItem());
	int n=ArgNum();
	if(o)
		o->ob_Flags&=~(1<<n);
}

void Act_RSet(void)
{
	ROOM *r=RoomOf(ArgItem());
	int n=ArgNum();
	if(r)
		r->rm_Flags|=(1<<n);
}

void Act_RClear(void)
{
	ROOM *r=RoomOf(ArgItem());
	int n=ArgNum();
	if(r)
		r->rm_Flags&=~(1<<n);
}

void Act_CSet(void)
{
	CONTAINER *c=ContainerOf(ArgItem());
	int n=ArgNum();
	if(c)
		c->co_Flags|=(1<<n);
}

void Act_CClear(void)
{
	CONTAINER *c=ContainerOf(ArgItem());
	int n=ArgNum();
	if(c)
		c->co_Flags&=~(1<<n);
}

void Act_PutBy(void)
{
	ITEM *a=ArgItem();
	Place(a,O_PARENT(ArgItem()));
}

void Act_Inc(void)
{
	ITEM *i=ArgItem();
	if(O_STATE(i)>2)
		return;
	SetState(i,(short)(O_STATE(i)+1));
	SynchChain(i);
}

void Act_Dec(void)
{
	ITEM *i=ArgItem();
	if(O_STATE(i)<1)
		return;
	SetState(i,(short)(O_STATE(i)-1));
	SynchChain(i);
}

void Act_SetState(void)
{
	ITEM *i=ArgItem();
	int n=ArgNum();
	if(n<0) n=0;
	if(n>3) n=3;
	SetState(i,(short)(n));
	SynchChain(i);
}

void Act_Prompt(void)
{
	char *a=TextOf(ArgText());
	SetPrompt(Me(),a);
}

void Act_Print(void)
{
	SendItem(Me(),"%d",GetFlag(ArgNum()));
}

void Act_Score(void)
{
	PLAYER *p=PlayerOf(Me());
	SendItem(Me(),"Your score is %ld.\n",p->pl_Score);
}

void Act_Message(void)
{
	SendItem(Me(),"%s\n",TextOf(ArgText()));
}

void Act_Msg(void)
{
	SendItem(Me(),"%s",TextOf(ArgText()));
}

void Act_MessageTo(void)
{
	ITEM *i=ArgItem();
	SendItem(i,"%s\n",TextOf(ArgText()));
}

void Act_MsgTo(void)
{
	ITEM *i=ArgItem();
	SendItem(i,"%s",TextOf(ArgText()));
}

static ITEM *NextVisible(ITEM *i)
{
	i=O_NEXT(i);
	if(i==NULL)
		return(NULL);
	while(i!=NULL && !CanSee(LevelOf(Me()),i))
		i=O_NEXT(i);
	return(i);
}

void Act_ListObj(void)
{
	ITEM *i=O_CHILDREN(ArgItem());
	int n=0;
	while(i!=NULL && !CanSee(LevelOf(Me()),i))
		i=O_NEXT(i);
	if(i==NULL)
	{
		SendItem(Me(),"nothing");
	}
	else
	{
		while(i)
		{
			if(n==0)
				n=1;
			else
			{
				if(NextVisible(i))
					SendItem(Me(),", ");
				else
					SendItem(Me()," and ");
			}
			SendItem(Me(),"%s",NameOf(i));
			if(IsObject(i)&&ObjectOf(i)->ob_Flags&OB_WORN)
				SendItem(Me(),"(worn)");
			i=NextVisible(i);
		}
	}
}

void Act_ListAt(void)
{
	ITEM *i=ArgItem();
	i=O_CHILDREN(i);
	while(i)
	{
		if(CanSee(LevelOf(Me()),i))
			DescribeItem(Me(),i);
		i=O_NEXT(i);
	}
}

void Act_Inven(void)
{
	ITEM *i=O_CHILDREN(Me());
	int n=0;
	SendItem(Me(),"You are carrying ");
	while(i!=NULL && !CanSee(LevelOf(Me()),i))
		i=O_NEXT(i);
	if(i==NULL)
	{
		SendItem(Me(),"nothing.\n");
	}
	else
	{
		while(i)
		{
			if(n==0)
				n=1;
			else
			{
				if(NextVisible(i))
					SendItem(Me(),", ");
				else
					SendItem(Me()," and ");
			}
			SendItem(Me(),"%s",NameOf(i));
			if(IsObject(i)&&ObjectOf(i)->ob_Flags&OB_WORN)
				SendItem(Me(),"(worn)");
			i=NextVisible(i);
		}
		SendItem(Me(),".\n");
	}
}

void Act_Desc(void)
{
	Cmd_Look(Me());
}

void Act_End(void)
{
	int u;
	if((u=UserOf(Me()))==-1)
		return;
	SendItem(Me(),"%s\n",TextOf(ArgText()));
#ifdef ATTACH
	if(UserList[u].us_RealPerson)
	{
		Act_UnAlias();
		return;
	}
#endif
	SendTPacket(UserList[u].us_Port,PACKET_CLEAR,
		"\nBye Bye....\n");
	RemoveUser(u);
	SetMe(NULL);
}

void Act_Done(void){;}
void Act_NotDone(void){;}

void Act_Ok(void)
{
	SendItem(Me(),"Ok.\n");
	Act_Done();
}

void Act_Abort(void)
{
	exit(0);
}

void Act_Save(void)
{
	static UFF SaveUFF;
	PLAYER *p=PlayerOf(Me());
	int n=UserOf(Me());
	if(n==-1)
		return;
#ifdef ATTACH
	if(UserList[n].us_RealPerson)
		return;
#endif
	UserList[n].us_Record=LoadPersona(UserList[n].us_Name,&SaveUFF);
	strcpy(SaveUFF.uff_Name,UserList[n].us_Name);
	SaveUFF.uff_Perception=Me()->it_Perception;
	SaveUFF.uff_ActorTable=Me()->it_ActorTable;
	SaveUFF.uff_ActionTable=Me()->it_ActionTable;
	SaveUFF.uff_Size=p->pl_Size;
	SaveUFF.uff_Weight=p->pl_Weight;
	SaveUFF.uff_Strength=p->pl_Strength;
	SaveUFF.uff_Flags=p->pl_Flags;
	SaveUFF.uff_Level=p->pl_Level;
	SaveUFF.uff_Score=p->pl_Score;
	SaveUFF.uff_Flag[0]=GetUserFlag(Me(),0);
	SaveUFF.uff_Flag[1]=GetUserFlag(Me(),1);
	SaveUFF.uff_Flag[2]=GetUserFlag(Me(),2);
	SaveUFF.uff_Flag[3]=GetUserFlag(Me(),3);
	SaveUFF.uff_Flag[4]=GetUserFlag(Me(),4);
	SaveUFF.uff_Flag[5]=GetUserFlag(Me(),5);
	SaveUFF.uff_Flag[6]=GetUserFlag(Me(),6);
	SaveUFF.uff_Flag[7]=GetUserFlag(Me(),7);
	SaveUFF.uff_Flag[8]=GetUserFlag(Me(),14);
	SaveUFF.uff_Flag[9]=GetUserFlag(Me(),15);
	strncpy(SaveUFF.uff_Password,UserList[n].us_Password,8);
	if(UserList[n].us_Record==-1)
	{
		UserList[n].us_Record=SaveNewPersona(&SaveUFF);
	}
	else
		SavePersona(&SaveUFF,UserList[n].us_Record);
}


void Act_NewText(void)
{
	WordPtr=NULL;
	if(UserOf(Me())>=0)
	{
		PCONTEXT *p=GetContext((short)UserOf(Me()));
		p->pa_It[0]=-1;
		p->pa_It[1]=-1;
		p->pa_Them[0]=-1;
		p->pa_Them[1]=-1;
		p->pa_Him[0]=-1;
		p->pa_Him[1]=-1;
		p->pa_Her[0]=-1;
		p->pa_Her[1]=-1;
		p->pa_There[0]=-1;
		p->pa_There[1]=-1;
	}
}

void Act_Process(void)
{
	TABLE *t=FindTable(ArgNum());
	if(t)
		ExecTable(t);
}

void Act_DoClass(void)
{
	ITEM *i=ArgItem();
	short cm=ArgNum();
	short num=ArgNum();
	ClassMask=(cm!=-1)?1<<cm:0;
	ClassLine=CurrentLine->li_Next;	/* Line to doclass from */
	if(num==1)
	{
		Item1=FindInByClass(LevelOf(Me()),i,(short)(1<<cm));
		if(Item1)
			ClassMode1=1;
		else
			ClassMode1=0;
	}
	else
	{
		Item2=FindInByClass(LevelOf(Me()),i,(short)(1<<cm));
		if(Item2)
			ClassMode2=1;
		else
			ClassMode2=0;
	}
}

void Act_Give(void)
{
	ITEM *a=ArgItem();
	ITEM *b=ArgItem();
	if(O_PARENT(a)!=Me())
	{
		SendItem(Me(),"You are not carrying %s.\n",NameOf(a));
		return;
	}
	if(CanPlace(a,b)<0)
	{
		SendItem(Me(),"%s can't carry %s.\n",CNameOf(b),NameOf(a));
		return;
	}
	Place(a,b);
	SendItem(b,"%s gives you %s.\n",CNameOf(Me()),NameOf(a));
}


void Act_DoesAction(void)
{
	ITEM *i=ArgItem();
	TPTR t=ArgText();
	int  n=ArgNum();
	DoesAction(i,(short)n,"%s",TextOf(t));
}

void Act_DoesTo(void)
{
	ITEM *i=ArgItem();
	TPTR t=ArgText();
	ITEM *j=ArgItem();
	int  n=ArgNum();
	DoesTo(i,(short)n,j,"%s",TextOf(t));
}

void Act_DoesToPlayer(void)
{
	ITEM *i=ArgItem();
	TPTR t=ArgText();
	ITEM *j=ArgItem();
	int  n=ArgNum();
	DoesToPlayer(i,(short)n,j,"%s",TextOf(t));
}

void Act_PObj(void)
{
	ITEM *i=ArgItem();
	OBJECT *o=ObjectOf(i);
	int n=ArgNum();
	if((n<0)||(n>3))
		return;
	if(o)
	{
		SendItem(Me(),"%s",TextOf(o->ob_Text[n]));
		if(UserOf(Me())!=-1)
			SetItData((short)UserOf(Me()),i,i->it_Adjective,i->it_Noun);
	}
}

void Act_PLoc(void)
{
	ITEM *i=ArgItem();
	ROOM *r=RoomOf(i);
	int n=ArgNum();
	if(r)
	{
		if(UserOf(Me())!=-1)
			SetItData((short)UserOf(Me()),i,i->it_Adjective,i->it_Noun);
		if(!n)
			SendItem(Me(),"%s",r->rm_Short);
		else
			SendItem(Me(),"%s",r->rm_Long);
	}
}

void Act_PName(void)
{
	ITEM *i=ArgItem();
	PLAYER *p=PlayerOf(i);
	if(CanSee(LevelOf(Me()),i)==0)
	{
		if(p)
			SendItem(Me(),"someone");
		else
			SendItem(Me(),"something");
	}
	else
	{
		SendItem(Me(),NameOf(i));
		SetItData(-1,i,i->it_Adjective,i->it_Noun);
	}
}

void Act_PCName(void)
{
	ITEM *i=ArgItem();
	PLAYER *p=PlayerOf(i);
	if(CanSee(LevelOf(Me()),i)==0)
	{
		if(p)
			SendItem(Me(),"Someone");
		else
			SendItem(Me(),"Something");
	}
	else
	{
		SendItem(Me(),CNameOf(i));
		SetItData(-1,i,i->it_Adjective,i->it_Noun);
	}
}

void Act_Daemon(void)
{
	ITEM *i=ArgItem();
	int v=ArgWord();
	int n1=ArgWord();
	RunDaemon(i,v,n1,ArgWord());
}

void Act_AllDaemon(void)
{
	int v=ArgWord();
	int n=ArgWord();
	AllDaemon(v,n,ArgWord());
}

void Act_HDaemon(void)
{
	ITEM *i=ArgItem();
	int v=ArgWord();
	int n1=ArgWord();
	HDaemon(i,v,n1,ArgWord());
}

void Act_When(void)
{
	int time=ArgNum();
	int table=ArgNum();
	AddEvent((unsigned long)time,(short)table);
}

void Act_SetName(void)
{
	ITEM *i=ArgItem();
	TPTR t=ArgText();
	SetName(i,TextOf(t));
}

void Act_Dup(void)
{
	ITEM *i=ArgItem();	/* source item   */
	short f=ArgNum();	/* 1 = true dup  */
	short g=ArgNum();	/* $(x)=new item */
	ITEM *n;
	n=Clone_Item(i,f);
	if(g==1)
		Item1=n;
	else
		Item2=n;
}


void Act_Points(void)
{
	PLAYER *p=PlayerOf(Me());
	if(p)
		p->pl_Score+=ArgNum();
}

void Act_Hurt(void)
{
	PLAYER *p=PlayerOf(Me());
	if(p==NULL)
	{
		ArgNum();
		return;
	}
	p->pl_Strength-=ArgNum();
	if(p->pl_Strength<0)
	{
		int u=UserOf(Me());
		if(u==-1)
			Place(Me(),NULL);
		else
		{
#ifdef ATTACH
			if(UserList[u].us_RealPerson==NULL)
			{
#endif

			  if(UserList[u].us_Record!=-1)
			    {
				UFF SaveUFF;
				UserList[u].us_Record=LoadPersona(
					UserList[u].us_Name,&SaveUFF);
#ifdef PANSY_MODE
				p->pl_Score/=2;
				p->pl_Strength=10;
#else
#ifdef REGISTER
				p->pl_Score=-1L;
#else
				strcpy(SaveUFF.uff_Name," ");
#endif
#endif
				SaveUFF.uff_Perception=Me()->it_Perception;
				SaveUFF.uff_ActorTable=Me()->it_ActorTable;
				SaveUFF.uff_ActionTable=Me()->it_ActionTable;
				SaveUFF.uff_Size=p->pl_Size;
				SaveUFF.uff_Weight=p->pl_Weight;
				SaveUFF.uff_Strength=p->pl_Strength;
				SaveUFF.uff_Flags=p->pl_Flags;
				SaveUFF.uff_Level=p->pl_Level;
				SaveUFF.uff_Score=p->pl_Score;
				SaveUFF.uff_Flag[9]=GetUserFlag(Me(),15);
				strncpy(SaveUFF.uff_Password,UserList[u].us_Password,8);
				UserList[u].us_Record=SavePersona(&SaveUFF,
					UserList[u].us_Record);
			      }
			  SendItem(Me(),
				"You seem to have died from your injuries...");
			  SendTPacket(UserList[u].us_Port,PACKET_CLEAR,
				"\nBye Bye....\n");
			  RemoveUser(u);
#ifdef ATTACH
			}
			else
				Place(Me(),NULL);
#endif
		}
		SetMe(NULL);
	}
}


void Act_Cured(void)
{
	PLAYER *p=PlayerOf(Me());
	p->pl_Strength+=ArgNum();
}

void Act_KillOff(void)
{
	PLAYER *p=PlayerOf(Me());
	int n=UserOf(Me());
	if(n==-1)
		return;
#ifdef ATTACH
	if(UserList[n].us_RealPerson)	/* Attached */
	{
		Place(Me(),NULL);
		SendItem(Me(),"%s\n",ArgText());
		Act_UnAlias();
		return;
	}
#endif
	if(UserList[n].us_Record!=-1)
	{
		UFF SaveUFF;
		UserList[n].us_Record=LoadPersona(UserList[n].us_Name,&SaveUFF);
		strcpy(SaveUFF.uff_Name," ");
		SaveUFF.uff_Perception=Me()->it_Perception;
		SaveUFF.uff_ActorTable=Me()->it_ActorTable;
		SaveUFF.uff_ActionTable=Me()->it_ActionTable;
		SaveUFF.uff_Size=p->pl_Size;
		SaveUFF.uff_Weight=p->pl_Weight;
		SaveUFF.uff_Strength=p->pl_Strength;
		SaveUFF.uff_Flags=p->pl_Flags;
		SaveUFF.uff_Level=p->pl_Level;
		SaveUFF.uff_Score=p->pl_Score;
		strncpy(SaveUFF.uff_Password,UserList[n].us_Password,8);
		UserList[n].us_Record=SavePersona(&SaveUFF,
			UserList[n].us_Record);
	}
	Act_End();
}


int Act_If1(void)
{
	return(Item1!=NULL);
}

int Act_If2(void)
{
	return(Item2!=NULL);
}

void Act_Bug(void)
{
	Log("Bug Entry: %s - %s\n",CNameOf(Me()),TextOf(ArgText()));
}

void Act_Typo(void)
{
	Log("Typo: %s - %s\n",CNameOf(Me()),TextOf(ArgText()));
}


int Act_IsMe(void)
{
	return(ArgItem()==Me());
}

void Act_Broadcast(void)
{
	char *t=TextOf(ArgText());
	Broadcast(t,ArgNum());
}

int Cnd_IsCalled(void)
{
	ITEM *i=ArgItem();
	char *t=TextOf(ArgText());
	if(stricmp(NameOf(i),t)==0)
	{
		return(1);
	}
	return(0);
}

void Act_SetMe(void)
{
	SetMe(ArgItem());
}

static void Pitem(short a, short n)
{
	ITEM *i=FindIn(LevelOf(Me()),Me(),a,n);
	if(!i)
		i=FindIn(LevelOf(Me()),O_PARENT(Me()),a,n);
	if(i)
		SendItem(Me(),CNameOf(i));
	else
	{
		i=FindMaster(LevelOf(Me()),a,n);
		if((i)&&(UserOf(i)!=-1))
			SendItem(Me(),CNameOf(i));
	}
}

static void Pglobal(short a, short n)
{
	ITEM *i=FindMaster(LevelOf(Me()),a,n);
	if(i)
		SendItem(Me(),CNameOf(i));
}

void Act_Pronouns(void)
{
	PCONTEXT *p;
	if(UserOf(Me())<0)
		return;
	SendItem(Me(),"Me       : %s\n",CNameOf(Me()));
	SendItem(Me(),"It       : ");
	p=GetContext((short)UserOf(Me()));
	Pitem(p->pa_It[0],p->pa_It[1]);
	SendItem(Me(),"\nThem     : ");
	Pitem(p->pa_Them[0],p->pa_Them[1]);
	SendItem(Me(),"\nHim      : ");
	Pitem(p->pa_Him[0],p->pa_Him[1]);
	SendItem(Me(),"\nHer      : ");
	Pitem(p->pa_Her[0],p->pa_Her[1]);
	SendItem(Me(),"\nThere    : ");
	Pglobal(p->pa_There[0],p->pa_There[1]);
	SendItem(Me(),"\n\n");
}

void Act_Exits(void)
{
	Cmd_Exits(Me(),ArgItem());
}

void Act_PWChange(void)
{
	short s=UserOf(Me());
	if(s==-1)
		return;
	SendUser(s,"What is your current password ?\n");
	SendNPacket(UserList[s].us_Port,PACKET_ECHO,1,0,0,0);
	UserList[s].us_State=AWAIT_PWVERIFY;
}

void PWVerify(short u,char *v)
{
	if(strncmp(UserList[u].us_Password,v,8))
	{
		SendUser(u,"Sorry. no.\n");
		UserList[u].us_State=AWAIT_COMMAND;
		SendNPacket(UserList[u].us_Port,PACKET_ECHO,0,0,0,0);
		PermitInput(u);
		return;
	}
	SendUser(u,"New Password ?\n");
	UserList[u].us_State=AWAIT_PWNEW;
	PermitInput((int)u);
}

void PWNew(short u,char *v)
{
	UserList[u].us_UserPtr=malloc(9);
	strncpy(UserList[u].us_UserPtr,v,8);
	SendUser(u,"Again to make sure it is correct\n");
	UserList[u].us_State=AWAIT_PWVERNEW;
	PermitInput((int)u);
}

void PWNewVerify(short u,char *v)
{
	if(strcmp(v,UserList[u].us_UserPtr))
	{
		SendUser(u,"Incorrect\n");
	}
	else
	{
		SendUser(u,"Password Changed\n");
		strncpy(UserList[u].us_Password,UserList[u].us_UserPtr,8);
	}
	free(UserList[u].us_UserPtr);
	UserList[u].us_State=AWAIT_COMMAND;
	SendNPacket(UserList[u].us_Port,PACKET_ECHO,0,0,0,0);
	PermitInput((int)u);
}

void Act_Snoop(void)
{
	StartSnoop(Me(),ArgItem(),SN_PLAYER);
}

void Act_UnSnoop(void)
{
	short v=ArgNum();
	ITEM *i=ArgItem();
	if(v)
		StopAllSnoops(Me());
	else
		StopSnoopOn(Me(),i);
}

void Act_Debug(void)
{
	System_Debug=ArgNum();
}

void Act_GetScore(void)
{
	PLAYER *p=PlayerOf(ArgItem());
	SetFlag(ArgNum(),p?p->pl_Score:0);
}

void Act_GetStr(void)
{
	PLAYER *p=PlayerOf(ArgItem());
	SetFlag(ArgNum(),p?p->pl_Strength:0);
}

void Act_GetLev(void)
{
	PLAYER *p=PlayerOf(ArgItem());
	SetFlag(ArgNum(),p?p->pl_Level:0);
}

void Act_SetScore(void)
{
	PLAYER *p=PlayerOf(ArgItem());
	int n=ArgNum();
	if(p)
		p->pl_Score=GetFlag(n);
}

void Act_SetStr(void)
{
	PLAYER *p=PlayerOf(ArgItem());
	int n=ArgNum();
	if(p)
		p->pl_Strength=GetFlag(n);
}

void Act_SetLev(void)
{
	PLAYER *p=PlayerOf(ArgItem());
	int n=ArgNum();
	if(p)
		p->pl_Level=GetFlag(n);
}

void Act_Shell(void)
{
/***
 *
 *	WARNING: THIS BIT IS VERY MACHINE SPECIFIC!!!!!
 *
 ***/
 	int u;
#ifdef SECURE
	return;
#else
/* Low security setting provides basic facilities of cat file
   and date - just those which the supplied db uses - to all
   The theory is arches cant do much damage with it. For linking
   to user services either add them here or see the USER action
  */
	char *a=TextOf(ArgText());
	if(strncmp(a,"cat",3)==0||strncmp(a,"/bin/cat",8)==0)
	{
		char *b=strchr(a,' ');
		char x[256];
		FILE *f;
		if(b)
		{
			if((f=fopen(b+1,"r"))!=NULL)
			{
				while(fgets(x,255,f)!=NULL)
					SendItem(Me(),x);
				fclose(f);
			}
		}
		return;
	}
	if(strcmp(a,"date")==0||strcmp(a,"/bin/date")==0)
	{
		long t;
		time(&t);
		SendItem(Me(),ctime(&t));
		return;
	}
	u=UserOf(Me());
	if(u==-1)
	{
		SendItem(Me(),"Refused: Not User\n");
		return;
	}
	if(strchr(UserList[u].us_UserName,':')!=NULL)
	{	/* Login as Internet: or Janet: or similar (not a local) */
		SendItem(Me(),"Refused:Security\n");
		return;
	}
#ifdef UNIX
/* Optionally put limits on local names allowed to use it here */
	{
		char x[128];
		FILE *p=popen(a,"r");
		while(fgets(x,127,p))
			SendItem(Me(),"%s",x);
		pclose(p);
	}
#endif
#endif
}

void Act_TreeDaemon(void)
{
	ITEM *i=ArgItem();
	int v=ArgWord();
	int n1=ArgWord();
	TreeDaemon(i,v,n1,ArgWord());
}

void Act_ChainDaemon(void)
{
	ITEM *i=ArgItem();
	int v=ArgWord();
	int n1=ArgWord();
	ChainDaemon(i,v,n1,ArgWord());
}

void Act_Means(void)
{
	Verb=ArgWord();
	Noun1=ArgWord();
	Noun2=ArgWord();
	if(ArgNum())
	{
		short Noun3=Noun1;	/* Selected SWAPword */
		Noun1=Noun2;
		Noun2=Noun3;
	}
}

void Act_CanGoto(void)
{
	ITEM *i=ArgItem();
	int n=ArgNum();
	SetFlag(n,CanGoto(Me(),i));
}

void Act_CanGoBy(void)
{
	ITEM *i=ArgItem();
	int n=ArgNum();
	SetFlag(n,CanGoto(Me(),O_PARENT(i)));
}


void Act_GetIFlag(void)
{
	extern ITEM *Item1,*Item2;
	ITEM *i=ArgItem();
	int n=ArgNum();
	if(ArgNum()!=1)
		Item2=GetUserItem(i,n);
	else
		Item1=GetUserItem(i,n);
}

void Act_SetIFlag(void)
{
	ITEM *i=ArgItem();
	int x=ArgNum();
	ITEM *n=ArgItem();
	SetUserItem(i,x,n);
}

void Act_ClearIFlag(void)
{
	ITEM *i=ArgItem();
	int x=ArgNum();
	SetUserItem(i,x,NULL);
}

void Act_Parse(void)
{
	char *a=TextOf(ArgText());
	char *ow=WordPtr;
	int v;
	short oa1=Adj1,oa2=Adj2,on1=Noun1,on2=Noun2,ov=Verb,op=Prep;
	ITEM *oi1=Item1,*oi2=Item2;
	TXT *t=AllocText(a);
	WordPtr=TextOf(t);
	v=GetVerb();
	UserAction(Me(),v);
	Adj1=oa1;Adj2=oa2;Noun1=on1;Noun2=on2;Verb=ov;Prep=op;
	Item1=oi1;Item2=oi2;
	WordPtr=ow;
	FreeText(t);
}

void Act_Comment(void)
{
	ArgText();
}

void Act_ComVocab(void)
{
	char *ow=WordPtr;
	TABLE *t;
	short oa1=Adj1,oa2=Adj2,on1=Noun1,on2=Noun2,ov=Verb,op=Prep;
	ITEM *oi1=Item1,*oi2=Item2;
	Verb=ArgNum();	
	Adj1=ArgNum();
	Noun1=ArgNum();
	Prep=ArgNum();
	Adj2=ArgNum();
	Noun2=ArgNum();
	Item1=FindAnItem(Adj1,Noun1,1);
	Item2=FindAnItem(Adj2,Noun2,1);
	t=FindTable(Me()->it_ActorTable);
	if(t)
		ExecTable(t);
	Adj1=oa1;Adj2=oa2;Noun1=on1;Noun2=on2;Verb=ov;
	Item1=oi1;Item2=oi2;Prep=op;
	WordPtr=ow;
}

void Act_Command(void)
{
	char *ow=WordPtr;
	TABLE *t;
	ITEM *itemp;
	short oa1=Adj1,oa2=Adj2,on1=Noun1,on2=Noun2,ov=Verb,op=Prep;
	ITEM *oi1=Item1,*oi2=Item2;
	Verb=ArgNum();	
	itemp=ArgItem();
	Prep=ArgNum();
	Item2=ArgItem();
	Item1=itemp;		/* So can specify vb $2 with $1 etc */
	Adj1=Item1->it_Adjective;
	Noun1=Item1->it_Noun;
	Adj2=Item2->it_Adjective;
	Noun2=Item2->it_Noun;
	t=FindTable(Me()->it_ActorTable);
	if(t)
		ExecTable(t);
	Adj1=oa1;Adj2=oa2;Noun1=on1;Noun2=on2;Verb=ov;
	Item1=oi1;Item2=oi2;Prep=op;
	WordPtr=ow;
}


void Act_AutoVerb(void)
{
	short w=ArgWord();
	short u=UserOf(Me());
	if(u<0)
		return;
	GetContext(u)->pa_Verb=w;
}

int Cnd_ClassAt(void)
{
	ITEM *i=ArgItem();
	short c=ArgNum();
	if(FindInByClass(LevelOf(Me()),i,(short)(1<<c)))
		return(1);
	return(0);
}

int Cnd_DupOf(void)
{
	ITEM *i=ArgItem();
	ITEM *j=ArgItem();
	DUP *d;
	d=(DUP *)FindSub(i,KEY_DUPED);
	if(!d)
		return(0);
	if(d->du_Master==j)
		return(1);
	return(0);
}

void Act_MasterOf(void)
{
	ITEM *i=ArgItem();
	short n=ArgNum();
	DUP *d;
	d=(DUP *)FindSub(i,KEY_DUPED);
	if(!d)
	{
		if(n==1)
			Item1=NULL;
		else
			Item2=NULL;
		return;
	}
	if(n==1)
		Item1=d->du_Master;
	else
		Item2=d->du_Master;
}


int Cnd_IfDark(void)
{
	if(IsDarkFor(Me()))
		return(1);
	return(0);
}


void Act_Visibility(void)
{
	ITEM *a=ArgItem();
	a->it_Perception=ArgNum();
}

void Act_GetParent(void)
{
	ITEM *a=ArgItem();
	int n=ArgNum();
	if(n==1)
		Item1=O_PARENT(a);
	else
		Item2=O_PARENT(a);
}

void Act_GetNext(void)
{
	ITEM *a=ArgItem();
	int n=ArgNum();
	if(n==1)
		Item1=O_NEXT(a);
	else
		Item2=O_NEXT(a);
}

void Act_GetChild(void)
{
	ITEM *a=ArgItem();
	int n=ArgNum();
	if(n==1)
		Item1=O_CHILDREN(a);
	else
		Item2=O_CHILDREN(a);
}

void Act_PExit(void)
{
	SendItem(Me(),"%s",ExitName(ArgNum()));
}

void Act_SetDesc(void)
{
	register OBJECT *o;
	ITEM *i=ArgItem();
	int n=ArgNum();
	char *t1=TextOf(ArgText());
	char *t2=TextOf(ArgText());
	char *t3=TextOf(ArgText());
	char *xb=malloc(512*3);
	if(xb==NULL)
		Error("Out of memory");
	strcpy(xb,t1);
	strcat(xb,t2);
	strcat(xb,t3);
	o=ObjectOf(i);
	if(o==NULL)
	{
		free(xb);
		return;
	}
	if((n<0)||(n>3))
	{
		SendItem(Me(),"SETDESC range!\n");
		Log("SETDESC range error.\n");
		free(xb);
		return;
	}
	FreeText(o->ob_Text[n]);
	o->ob_Text[n]=AllocText(xb);
	free(xb);
}

void Act_SetLong(void)
{
	register ROOM *r;
	ITEM *i=ArgItem();
	char *t1=TextOf(ArgText());
	char *t2=TextOf(ArgText());
	char *t3=TextOf(ArgText());
	char *xb=malloc(512*3);
	if(xb==NULL)
		Error("Out of memory");
	strcpy(xb,t1);
	strcat(xb,t2);
	strcat(xb,t3);
	r=RoomOf(i);
	if(r==NULL)
	{
		free(xb);
		return;
	}
	FreeText(r->rm_Long);
	r->rm_Long=AllocText(xb);
	free(xb);
}

void Act_SetShort(void)
{
	register ROOM *r;
	ITEM *i=ArgItem();
	char *t1=TextOf(ArgText());
	char *t2=TextOf(ArgText());
	char *t3=TextOf(ArgText());
	char *xb=malloc(512*3);
	if(xb==NULL)
		Error("Out of memory");
	strcpy(xb,t1);
	strcat(xb,t2);
	strcat(xb,t3);
	r=RoomOf(i);
	if(r==NULL)
	{
		free(xb);
		return;
	}
	FreeText(r->rm_Short);
	r->rm_Short=AllocText(xb);
	free(xb);
}

void Act_GetLong(void)
{
	register ROOM *r=RoomOf(ArgItem());
	if(!r)
		return;
	if(TxtArg)
		FreeText(TxtArg);
	TxtArg=AllocText(TextOf(r->rm_Long));
}

void Act_GetShort(void)
{
	register ROOM *r=RoomOf(ArgItem());
	if(!r)
		return;
	if(TxtArg)
		FreeText(TxtArg);
	TxtArg=AllocText(TextOf(r->rm_Short));
}

void Act_GetDesc(void)
{
	register OBJECT *o=ObjectOf(ArgItem());
	int n=ArgNum();
	if(!o)
		return;
	if((n<0)||(n>3))
		return;
	if(TxtArg)
		FreeText(TxtArg);
	TxtArg=AllocText(TextOf(o->ob_Text[n]));
}

void Act_GetName(void)
{
	register ITEM *i=ArgItem();
	if(!i)
		return;
	if(TxtArg)
		FreeText(TxtArg);
	TxtArg=AllocText(NameOf(i));
}

void Act_Swat(void)
{
	TXT *t=TxtArg;
	TxtArg=TxtArg2;
	TxtArg2=t;
}

void Act_Flat(void)
{
	char x[16];
	sprintf(x,"%d",GetFlag(ArgNum()));
	if(TxtArg)
		FreeText(TxtArg);
	TxtArg=AllocText(x);
}

void Act_SetIn(void)
{
	char *x=TextOf(ArgText());
	SetInMsg(Me(),x);
}

void Act_SetOut(void)
{
	char *x=TextOf(ArgText());
	SetOutMsg(Me(),x);
}

void Act_SetHere(void)
{
	char *x=TextOf(ArgText());
	SetHereMsg(Me(),x);
}

void Act_FindMaster(void)
{
	short ad,no;
	short d=ArgNum();
	ad=(d==1)?Adj1:Adj2;
	no=(d==1)?Noun1:Noun2;
	d=ArgNum();
	if(d==1)
	{
		Item1=FindMaster((short)LevelOf(Me()),ad,no);
	}
	else
		Item2=FindMaster((short)LevelOf(Me()),ad,no);
}

void Act_NextMaster(void)
{
	ITEM *p=ArgItem();
	short ad,no;
	short d=ArgNum();
	ad=(d==1)?Adj1:Adj2;
	no=(d==1)?Noun1:Noun2;
	d=ArgNum();
	if(d==1)
	{
		Item1=NextMaster((short)LevelOf(Me()),p,ad,no);
	}
	else
		Item2=NextMaster((short)LevelOf(Me()),p,ad,no);
}

void Act_FindIn(void)
{
	ITEM *i=ArgItem();
	short ad,no;
	short d=ArgNum();
	ad=(d==1)?Adj1:Adj2;
	no=(d==1)?Noun1:Noun2;
	d=ArgNum();
	if(d==1)
	{
		Item1=FindIn((short)LevelOf(Me()),i,ad,no);
	}
	else
		Item2=FindIn((short)LevelOf(Me()),i,ad,no);
}

void Act_NextIn(void)
{
	ITEM *i=ArgItem();
	short ad,no;
	short d=ArgNum();
	ad=(d==1)?Adj1:Adj2;
	no=(d==1)?Noun1:Noun2;
	d=ArgNum();
	if(d==1)
	{
		Item1=NextIn((short)LevelOf(Me()),i,ad,no);
	}
	else
		Item2=FindIn((short)LevelOf(Me()),i,ad,no);
}

void Act_LenText(void)
{
	char *t=TextOf(ArgText());
	SetFlag(ArgNum(),strlen(t));
}

void Act_Field(void)
{
	int u=UserOf(Me());
	if(u>-1)
	{
		SendUser(-2, "");
		SendNPacket(UserList[u].us_Port,PACKET_SETFIELD,ArgNum(),0,0,0);
	}
	else
		ArgNum();	/* Must always use args */
}

void Act_Distance(void){ArgItem();ArgItem();ArgNum();}
void Act_WhichWay(void){ArgItem();ArgItem();ArgNum();}

void Act_Become(void)
{
	char *x=TextOf(ArgText());
	int u=UserOf(Me());
	if(u!=-1)
	{
		ExitUser(u);
		Name_Got(u,x);	/* Fake a Login: response */
	}
}

void Act_Alias(void)
{
#ifdef ATTACH
	short ct=0;
	ITEM *i=ArgItem();
	int u=UserOf(Me());
	if(i==Me())
	{
		SendItem(Me(),"Umm.. fine.\n");
		return;
	}
	if(IsUser(i))
	{
		SendItem(Me(),"You cannot attach to another player!\n");
		return;
	}
	while(ct<MAXUSER)
	{
		if(UserList[ct].us_RealPerson==i)
		{
			SendItem(Me(),"You cannot attach to another player \
(even if they are out!)\n");
			return;
		}
		ct++;
	}
	if(!IsPlayer(i))
	{
		SendItem(Me(),"You can only attach to players!\n");
		return;
	}
	if(UserList[u].us_RealPerson==NULL)
	{
		UserList[u].us_RealPerson=UserList[u].us_Item;
	}
	PlayerOf(i)->pl_UserKey=PlayerOf(Me())->pl_UserKey;
	UserList[u].us_Item=i;
	LockItem(i);
	SendItem(i,"Attaching to %s.\n",CNameOf(i));
#endif
}

void Act_UnAlias(void)
{
#ifdef ATTACH
	int u=UserOf(Me());
	if(u!=-1)
	{
		if(UserList[u].us_RealPerson)
		{
			SendItem(Me(),"Detaching from %s\n",CNameOf(Me()));
			PlayerOf(Me())->pl_UserKey=-1;
			UnlockItem(UserList[u].us_Item);
			UserList[u].us_Item=UserList[u].us_RealPerson;
			UserList[u].us_RealPerson=NULL;
		}
	}
#endif
}

void Act_SetUText(void)
{
	ITEM *i=ArgItem();
	int n=ArgNum();
	TXT *t=ArgText();
	n%=8;
	SetUText(i,n,t);
}

void Act_GetUText(void)
{
	ITEM *i=ArgItem();
	int n=ArgNum();
	n%=8;
	if(TxtArg)
		FreeText(TxtArg);
	TxtArg=AllocText(TextOf(GetUText(i,n)));
}

void Act_Cat(void)
{
	TPTR a=ArgText();
	TPTR b=ArgText();
	TPTR c=ArgText();
	char *bf=malloc(512*3);
	if(!bf)
		Error("Out Of Memory");
	strcpy(bf,TextOf(a));
	strcat(bf,TextOf(b));
	strcat(bf,TextOf(c));
	if(TxtArg)
		FreeText(TxtArg);
	TxtArg=AllocText(bf);
	free(bf);
}

/*
 *	Unused / Unimplemented functions
 */

void Act_PutO(void){ArgItem();}	/* No longer relevant */
void Act_Frig(void){;}

void Act_NArg(void)
{
	int f=ArgNum();
	int n=ArgNum();
	int v;
	sscanf(TextOf(n==1?TxtArg:TxtArg2),"%d",&v);
	SetFlag(f,v);
}

void Act_NeedField(void){ArgNum();}


void Act_Unveil(void)
{
	int n=ArgNum();
	unsigned int a,b,c,d,e,f,g,h;
	srand(n);
	if(!TxtArg)
		return;
	sscanf(TextOf(TxtArg),"%x,%x,%x,%x,%x,%x,%x,%x",
		&a,&b,&c,&d,&e,&f,&g,&h);
	if(rand()%256!=a)
	{
		Log("Failed UNVEIL");
		return;
	}
	if(rand()%256!=b)
	{
		Log("Failed UNVEIL");
		return;
	}
	if(rand()%256!=c)
	{
		Log("Failed UNVEIL");
		return;
	}
	if(rand()%256!=d)
	{
		Log("Failed UNVEIL");
		return;
	}
	if(rand()%256!=e)
	{
		Log("Failed UNVEIL");
		return;
	}
	if(rand()%256!=f)
	{
		Log("Failed UNVEIL");
		return;
	}
	if(rand()%256!=g)
	{
		Log("Failed UNVEIL");
		return;
	}
	if(rand()%256!=h)
	{
		Log("Failed UNVEIL");
		return;
	}
	SetName(Me(),BOSS1);
}

int Cnd_SubStr(void)
{
	char *a=TextOf(ArgText());
	char *b=TextOf(ArgText());
	while(*a)
	{
		if(strncmp(a,b,strlen(b))==0)
			return(1);
		a++;
	}
	return(0);
}

void Act_GetIn(void)
{
	ITEM *i=ArgItem();
	if(TxtArg)
		FreeText(TxtArg);
	TxtArg=AllocText(GetInMsg(i));
}

void Act_GetOut(void)
{
	ITEM *i=ArgItem();
	if(TxtArg)
		FreeText(TxtArg);
	TxtArg=AllocText(GetOutMsg(i));
}

void Act_GetHere(void)
{
	ITEM *i=ArgItem();
	if(TxtArg)
		FreeText(TxtArg);
	TxtArg=AllocText(GetHereMsg(i));
}

void Act_Log(void)
{
	char *a=TextOf(ArgText());
	char *b=TextOf(ArgText());
	char *c=TextOf(ArgText());
	Log("%s%s%s",a,b,c);
}

void Act_SetClass(void)
{
	ITEM *i=ArgItem();
	unsigned short v=1<<ArgNum();
	i->it_Class|=v;
}

void Act_UnSetClass(void)
{
	ITEM *i=ArgItem();
	unsigned short v=1<<ArgNum();
	i->it_Class&=~v;
}

void Act_BitClear(void)
{
	int v=ArgNum();
	int x=ArgNum();
	SetFlag(v,GetFlag(v)&~(1<<x));
}

void Act_BitSet(void)
{
	int v=ArgNum();
	int x=ArgNum();
	SetFlag(v,GetFlag(v)|(1<<x));
}

int Cnd_BitTest(void)
{
	int v=ArgNum();
	int x=ArgNum();
	if((GetFlag(v)&(1<<x))==0)
		return(0);
	return(1);
}

void Act_SPrint(void)
{
	char bf[32];
	sprintf(bf,"%hd",GetFlag(ArgNum()));
	if(TxtArg)
		FreeText(TxtArg);
	TxtArg=AllocText(bf);
}

void Act_User(void)
{
	short n1,n2,n3;
	ITEM *i1,*i2;
	TPTR t;
	n1=ArgNum();
	n2=ArgNum();
	n3=ArgNum();
	i1=ArgItem();
	i2=ArgItem();
	t=ArgText();
	UserVector(n1,n2,n3,i1,i2,t);
}

void Act_Mobiles(void){;}
void Act_Dir(void){;}
void Act_Rooms(void){;}

/*
 *	Rope logic will appear later - much later probably (if ever)
 *
 */

void Act_TiedTo(void){ArgItem();ArgNum();}
void Act_PlaceRope(void){ArgItem();ArgItem();}
int Cnd_IsRope(void){ArgItem();return(0);}
int Cnd_IsTied(void){ArgItem();return(0);}
void Act_RopePrev(void){ArgItem();ArgNum();}
void Act_RopeNext(void){ArgItem();ArgNum();}
void Act_Tie(void){ArgItem();ArgItem();}
void Act_Untie(void){ArgItem();ArgItem();}
void Act_Join(void){ArgItem(),ArgItem();}
void Act_CutRope(void){ArgItem(),ArgItem();}
int Cnd_CanMoveRope(void){ArgItem();return(1);}



void Act_Cls(void)
{
	SendItem(Me(),"\014\n");
}



void Act_GetVis(void)
{
	ITEM *i=ArgItem();
	int f=ArgNum();
	SetFlag(f,i->it_Perception);
}

