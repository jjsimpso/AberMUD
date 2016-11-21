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
 *	1.00		AGC		Created File
 *	1.01		AGC		Updated to cover userflag2
 *	1.02		AGC		ANSIfication
 */
 
Module  "Utility Commands";
Version "1.02";
Author  "Alan Cox";

void Cmd_DoorPair(ITEM *i)
{
	char doorname[560];
	int ad,no;
	ITEM *door1,*door2,*from,*to;
	GENEXIT *g1,*g2;
	short dirn;
	if(!ArchWizard(i))
	{
		SendItem(i,"Que ?\n");
		return;
	}
	if(GetThing(&ad,&no)==-1)
	{
		SendItem(i,"There are unidentified words in your item.\n");
		return;
	}
	from=FindSomething(i,O_PARENT(i));
	if(!from)
	{
		SendItem(i,"I don't know where you want the exit to go from.\n");
		return;
	}
	dirn=GetVerb();
	if((dirn<99)||(dirn>110))
	{
		SendItem(i,"I don't recognise that direction.\n");
		return;
	}
	to=FindSomething(i,O_PARENT(i));
	if(!to)
	{
		SendItem(i,"I don't know where you want the exit to go to.\n");
		return;
	}
	if(ad==-1)
		sprintf(doorname,"the %s",FindWText(no,WD_NOUN));
	else
		sprintf(doorname,"the %s %s",FindWText(ad,WD_ADJ),
					     FindWText(no,WD_NOUN));
	door1=CreateItem(doorname,ad,no);
	door2=CreateItem(doorname,ad,no);
	MakeObject(door1);
	MakeObject(door2);
	ObjectOf(door1)->ob_Flags=OB_FLANNEL;
	ObjectOf(door2)->ob_Flags=OB_FLANNEL;
	Place(door1,from);
	Place(door2,to);
	AddChain(door1,door2);
	AddChain(door2,door1);
	door1->it_ActionTable=2;
	door2->it_ActionTable=2;
	FreeText(ObjectOf(door1)->ob_Text[0]);
	FreeText(ObjectOf(door1)->ob_Text[1]);
	FreeText(ObjectOf(door2)->ob_Text[0]);
	FreeText(ObjectOf(door2)->ob_Text[1]);
	ObjectOf(door1)->ob_Text[0]=AllocText("The door is open. ");
	ObjectOf(door1)->ob_Text[1]=AllocText("The door is closed. ");
	ObjectOf(door2)->ob_Text[0]=AllocText("The door is open. ");
	ObjectOf(door2)->ob_Text[1]=AllocText("The door is closed. ");
	g1=(GENEXIT *)FindSub(from,KEY_GENEXIT);
	if(!g1)
	{
		if(MakeGenExit(from))
		{
			SendItem(i,"Couldn't create GENEXIT subinfo.\n");
			return;
		}
		g1=(GENEXIT *)FindSub(from,KEY_GENEXIT);
	}
	g2=(GENEXIT *)FindSub(to,KEY_GENEXIT);
	if(!g2)
	{
		if(MakeGenExit(to))
		{
			SendItem(i,"Couldn't create GENEXIT subinfo.\n");
			return;
		}
		g2=(GENEXIT *)FindSub(to,KEY_GENEXIT);
	}
	if(g1->ge_Dest[dirn-100])
		UnlockItem(g1->ge_Dest[dirn-100]);
	if(g2->ge_Dest[BackExit(dirn-100)])
		UnlockItem(g2->ge_Dest[BackExit(dirn-100)]);
	LockItem(door1);
	LockItem(door2);
	g1->ge_Dest[dirn-100]=door2;
	g2->ge_Dest[BackExit(dirn-100)]=door1;
	SendItem(i,"Exits created.\n");
}
	
/*
 *	Printer Commands
 */

void Cmd_ShowAllObjects(ITEM *i)
{
	register int ct;
	register OBJECT *o;
	register ITEM *b=ItemList;
	while(b)
	{
		if((o=ObjectOf(b))!=NULL)
		{
			ShowItemData(i,b);
			SendItem(i,"Text(0): %s\n",TextOf(o->ob_Text[0]));
			SendItem(i,"Text(1): %s\n",TextOf(o->ob_Text[1]));
			SendItem(i,"Text(2): %s\n",TextOf(o->ob_Text[2]));
			SendItem(i,"Text(3): %s\n",TextOf(o->ob_Text[3]));
			SendItem(i,"Size   : %d     Weight : %d\n",
				o->ob_Size,o->ob_Weight);
			ct=0;
			while(ct<16)
			{
				if(o->ob_Flags&(1<<ct))
					SendItem(i,"%s ",OBitName(ct));
				else
					if(strcmp(OBitName(ct),"{unset}"))
						SendItem(i,"-%s ",OBitName(ct));
				ct++;
			}
			SendItem(i,"\n\n-------------------------\n");
		}
		b=b->it_MasterNext;
	}
}

void Cmd_ShowAllPlayers(ITEM *i)
{
	register int ct;
	register PLAYER *o;
	register ITEM *b=ItemList;
	while(b)
	{
		if((o=PlayerOf(b))!=NULL)
		{
			ShowItemData(i,b);
			SendItem(i,"UserKey : %d ",o->pl_UserKey);
			SendItem(i,"Size    : %d ",o->pl_Size);
			SendItem(i,"Weight  : %d \n",o->pl_Weight);
			SendItem(i,"Strength: %d ",o->pl_Strength);
			SendItem(i,"Level   : %d ",o->pl_Level);
			SendItem(i,"Score   : %d\n",o->pl_Score);
			ct=0;
			while(ct<16)
			{
				if(o->pl_Flags&(1<<ct))
					SendItem(i,"%s ",PBitName(ct));
				else
					if(strcmp(PBitName(ct),"{unset}"))
						SendItem(i,"-%s ",PBitName(ct));
				ct++;
			}
			SendItem(i,"\n\n-------------------------\n");

		}
		b=b->it_MasterNext;
	}
}

void Cmd_ShowAllRooms(ITEM *i)
{
	register int ct;
	register ROOM *r;
	register ITEM *b=ItemList;
	while(b)
	{
		if((r=RoomOf(b))!=NULL)
		{
			ShowItemData(i,b);
			SendItem(i,"Short Text: %s\n",TextOf(r->rm_Short));
			SendItem(i,"Long Text :\n%s\n",TextOf(r->rm_Long));
			ct=0;
			while(ct<16)
			{
				if(r->rm_Flags&(1<<ct))
					SendItem(i,"%s ",RBitName(ct));
				else
					if(strcmp(RBitName(ct),"{unset}"))
						SendItem(i,"-%s ",RBitName(ct));
				ct++;
			}
			SendItem(i,"\n\n");
			Cmd_Exits(i,b);	/* Show exits */
			SendItem(i,"\n\n-------------------------\n");
		}
		b=b->it_MasterNext;
	}
}

int ShowItemData(ITEM *i, ITEM *b)
{
	register SUB *s;
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
	SendItem(i,"Adjective : %3d  Noun: %3d  Actor: %3d  Action: %3d  State: %3d\n",
		(int)b->it_Adjective,(int)b->it_Noun,(int)b->it_ActorTable,
		(int)b->it_ActionTable,(int)b->it_State);
	SendItem(i,"Perception: %3d  Lock: %3d  Classes: %x\n",
		(int)b->it_Perception,(int)b->it_Users,(int)b->it_Class);
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
			default:
					SendItem(i,"Unknown (%d)",s->pr_Key);
		}			
		SendItem(i,"\n");
		s=s->pr_Next;
	}
	return(0);
}
