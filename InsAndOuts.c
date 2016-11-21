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

Module "Ins And Outs";
Version "1.06";
Author "----*(A)";

/*	Exit Drivers
 *
 *	1.00	Basic Exit Control GENEXIT Handlers
 *	1.01	Added MSGEXIT Control
 *	1.02	Checks CanPlace() when moving
 *	1.03	Added CanGoto()
 *	1.04	Added DoorExit() and WhereTo()
 *	1.05	Added RM_DROPEMSG
 *      1.06    Doesnt tell you about invisible/dark doors
 *	1.07	Added complex exit dump for archwizards
 */

extern ITEM *Item1,*Item2;

char *ExitName(x)
int x;
{
	switch(x)
	{
		case 0:return("North");
		case 1:return("East");
		case 2:return("South");
		case 3:return("West");
		case 4:return("Up");
		case 5:return("Down");
		case 6:return("NorthEast");
		case 8:return("SouthEast");
		case 7:return("NorthWest");
		case 9:return("SouthWest");
		case 10:return("In");
		case 11:return("Out");
	}
	return("<ERROR>");
}


void Cmd_NewExit(i)
ITEM *i;
{
	short v;	/* Short gives better array access code */
	ITEM *a;
	ITEM *to;
	GENEXIT *g;
	if(!ArchWizard(i))
	{
		SendItem(i,"Sorry no walking through walls.\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	g=(GENEXIT *)FindSub(a,KEY_GENEXIT);
	if(!g)
	{
		if(MakeGenExit(a))
		{
			SendItem(i,"Couldn't create GENEXIT subinfo.\n");
			return;
		}
		g=(GENEXIT *)FindSub(a,KEY_GENEXIT);
	}
	v=(short)GetVerb();
	if((v<100)||(v>111))
	{
		SendItem(i,"You must specify a direction. \n");
		return;
	}
	to=FindSomething(i,O_PARENT(i));
	if(to==NULL)
	{
		SendItem(i,"I don't know where you want it to go to. \n");
		return;
	}
	if(g->ge_Dest[v-100])
		UnlockItem(g->ge_Dest[v-100]);
	LockItem(to);
	g->ge_Dest[v-100]=to;
	SendItem(i,"Exit Created.\n");
}

void Cmd_MsgExit(i)
ITEM *i;
{
	char *WStr;
	short v;
	ITEM *a;
	ITEM *to;
	if(!ArchWizard(i))
	{
		SendItem(i,"Sorry no walking through walls.\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	v=(short)GetVerb();
	if((v<100)||(v>111))
	{
		SendItem(i,"You must specify a direction. \n");
		return;
	}
	WStr=WordPtr;
	GetAll();
	if(!strlen(WordBuffer))
	{
		MSGEXIT *m;
		m=FindMsgExit(a,(short)(v-100));
		if(!m)
			return;
		SendEdit(i,"MessageExit %s %s %s %s",
			NameOf(a),ExitName(v-100),
			NameOf(m->me_Dest),
			TextOf(m->me_Text));
		return;
	}
	WordPtr=WStr;
	to=FindSomething(i,O_PARENT(i));
	if(to==NULL)
	{
		SendItem(i,"I don't know where you want it to go to. \n");
		return;
	}
	GetAll();
	MakeMsgExit(a,to,(short)(v-100),WordBuffer);
	SendItem(i,"Exit Created.\n");
}

void Cmd_CondExit(i)
ITEM *i;
{
	int table;
	short v;
	ITEM *a;
	ITEM *to;
	if(!ArchWizard(i))
	{
		SendItem(i,"Sorry no walking through walls.\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	v=(short)GetVerb();
	if((v<100)||(v>111))
	{
		SendItem(i,"You must specify a direction. \n");
		return;
	}
	to=FindSomething(i,O_PARENT(i));
	if(to==NULL)
	{
		SendItem(i,"I don't know where you want it to go to. \n");
		return;
	}
	GetAll();
	table=FindTableByName(WordBuffer);
	if(table==-1)
	{
		SendItem(i,"Unknown table.\n");
		return;
	}
	MakeCondExit(a,to,table,(short)(v-100));
	SendItem(i,"Exit Created.\n");
}

/*	BUG: Cmd_MoveDirn currently doesn't support MsgExits to doors.
 *	Note: There are NO CondExits to doors.
 */

void Cmd_MoveDirn(i,x)
ITEM *i;
int x;
{
	ITEM *d;
	MSGEXIT *m;
	CONDEXIT *c;
	if(O_PARENT(i)==NULL)
	{
		SendItem(i,"The void is infinite you can wander lost forever...\n");
		return;
	}
	c=FindCondExit(O_PARENT(i),(short)x);
	m=FindMsgExit(O_PARENT(i),(short)x);
	if((c)&&(TestCondExit(c)))
	{
		if(CanPlace(i,c->ce_Dest))
		{
			SendItem(i,"You can't get into %s.\n",NameOf(c->ce_Dest));
			return;
		}
		DoesAction(i,4,"%s %s.\n",GetOutMsg(i),ExitName(x));
		Place(i,c->ce_Dest);
		Cmd_Look(i);
		DoesAction(i,4,"%s.\n",GetInMsg(i));
		return;
	}
	if(c)
	{
		return;
	}
	if(m)
	{
		if(CanPlace(i,m->me_Dest))
		{
			SendItem(i,"You can't get into %s.\n",NameOf(m->me_Dest));
			return;
		}
		DoesAction(i,4,"%s %s.\n",GetOutMsg(i),ExitName(x));
		Place(i,m->me_Dest);
		if(!IsDarkFor(i)&&(IsPlayer(i)&&
			(PlayerOf(i)->pl_Flags&PL_BRIEF)==0))
			SendItem(i,"%s ",TextOf(m->me_Text));
		else
		{
			Cmd_Look(i);
			goto l1;
		}
		if((IsRoom(m->me_Dest))&&
			((RoomOf(m->me_Dest)->rm_Flags&RM_DROPEMSG)==0))
				Cmd_Look(i);
l1:		DoesAction(i,4,"%s.\n",GetInMsg(i));
		return;
	}		
	d=ExitOf(O_PARENT(i),x);
	if(d)
	{
		if(CanPlace(i,d))
		{
			SendItem(i,"You can't get into %s.\n",NameOf(d));
			return;
		}
		DoesAction(i,4,"%s %s.\n",GetOutMsg(i),ExitName(x));
		Place(i,d);
		Cmd_Look(i);
		DoesAction(i,4,"%s.\n",GetInMsg(i));
		return;
	}
	if((d=DoorOf(O_PARENT(i),x))!=NULL)
	{
	/* SIMPLE FORM FOR NOW! */
		if(IsDarkFor(i)||!CanSee(LevelOf(i),d))
		{
			SendItem(i,"You can't go that way.\n");
		}
		else
		{
			if(O_STATE(d)==1)
				SendItem(i,"%s is closed.\n",CNameOf(d));
			else
				SendItem(i,"%s is locked.\n",CNameOf(d));
		}
		return;
	}
	SendItem(i,"You can't go that way.\n");
}


void Cmd_DelExit(i)
ITEM *i;
{
	int v;
	ITEM *a;
	GENEXIT *g;
	MSGEXIT *m;
	CONDEXIT *c;
	if(!ArchWizard(i))
	{
		SendItem(i,"Sorry no instant walls.\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	v=GetVerb();
	if((v<100)||(v>111))
	{
		SendItem(i,"You must specify a direction. \n");
		return;
	}
	c=FindCondExit(a,(short)(v-100));
	m=FindMsgExit(a,(short)(v-100));
	if(c)
		UnCondExit(a,c);
	if(m)
		UnMsgExit(a,m);
	g=(GENEXIT *)FindSub(a,KEY_GENEXIT);
	if(!g)
	{
		return;
	}
	if(g->ge_Dest[v-100])
	{
		UnlockItem(g->ge_Dest[v-100]);
		g->ge_Dest[v-100]=NULL;
	}
	else
	{
		return;
	}
	SendItem(i,"Exit Deleted.\n");
}


void Cmd_Exits(i,j)
ITEM *i;
ITEM *j;
{
	MSGEXIT *m;
	CONDEXIT *ce;
	ITEM *k;
	int n=0;
	int c=0;
	if(j==NULL)
	{
		SendItem(i,"In the void nothing bars your way.\n");
		return;
	}
	while(n<12)
	{
		if((k=ExitOf(j,n))!=NULL)
		{
			c=1;
			SendItem(i,"%-20s:%s\n",ExitName(n),CNameOf(k));
			if(Arch(i))
				SendItem(i,"StdExit to #%d %s\n",
					ItemNumber(LevelOf(i),k),CNameOf(k));
		}
		m=FindMsgExit(j,(short)n);
		if(m)
		{
			c=1;
			SendItem(i,"%-20s:%s\n",ExitName(n),
				CNameOf(m->me_Dest));
			if(Arch(i))
				SendItem(i,"MsgExit to #%d %s '%s'\n",
					ItemNumber(LevelOf(i),m->me_Dest),
					CNameOf(m->me_Dest),
					TextOf(m->me_Text));
		}
		ce=FindCondExit(j,(short)n);
		if(ce&&TestCondExit(ce)==1)
		{
			c=1;
			SendItem(i,"%-20s:%s\n",ExitName(n),
				CNameOf(ce->ce_Dest));
			if(Arch(i))
				SendItem(i,"CondExit to #%d %s (%s)\n",
					ItemNumber(LevelOf(i),ce->ce_Dest),
					CNameOf(ce->ce_Dest),
					TabName(ce->ce_Table));
		}
		n++;
	}
	if(c==0)
		SendItem(i,"None....\n");
}

int CanGoto(i,t)
ITEM *i,*t;
{
	short x=0;
	ITEM *d;
	MSGEXIT *m;
	CONDEXIT *c;
	if(O_PARENT(i)==NULL)
	{
		return(255);
	}
	while(x<12)
	{
		c=FindCondExit(O_PARENT(i),(short)x);
		m=FindMsgExit(O_PARENT(i),(short)x);
		if((c)/*&&(TestCondExit(c))*/)
		{
			if(CanPlace(i,c->ce_Dest))
				continue;
			return(x);
		}
		if(m)
		{
			if(CanPlace(i,m->me_Dest))
			{
				continue;
			}
			return(x);
		}		
		d=ExitOf(O_PARENT(i),x);
		if(d)
		{
			if(CanPlace(i,d))
			{
				continue;
			}
			return(x);
		}
		x++;
	}
	return(255);
}


void Act_WhereTo()
{
	ITEM *i=ArgItem();
	short d=ArgNum();
	short f=ArgNum();
	MSGEXIT *m;
	CONDEXIT *c;
	c=FindCondExit(i,d);
	m=FindMsgExit(i,d);
	if((c)/*&&(TestCondExit(c))*/)
	{
		if(f==1)
			Item1=c->ce_Dest;
		else
			Item2=c->ce_Dest;
		return;
	}
	if(m)
	{
		if(f==1)
			Item1=m->me_Dest;
		else
			Item2=m->me_Dest;
		return;
	}
	if(f==1)
		Item1=ExitOf(i,d);
	else
		Item2=ExitOf(i,d);
}

void Act_DoorExit()
{
	ITEM *i=ArgItem();
	ITEM *d=ArgItem();
	short f=ArgNum();
	short ct=0;
	while(ct<12)
	{
		if(DoorOf(i,ct)==d)
		{
			SetFlag(f,ct);
			return;
		}
		ct++;
	}
	SetFlag(f,255);
	return;
}

int BackExit(n)
int n;
{
	switch(n)
	{
		case 0:return(2);
		case 1:return(3);
		case 2:return(0);
		case 3:return(1);
		case 4:return(5);
		case 5:return(4);
		case 6:return(8);
		case 7:return(9);
		case 8:return(6);
		case 9:return(7);
		case 10:return(11);
		case 11:return(10);
	}
	return(-1);
}
