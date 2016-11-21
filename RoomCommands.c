
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

Module "Room Commands";
Version "1.06";
Author "----*(A)";

/*
 *	1.00	AGC	Created This File
 *	1.01	AGC	Stopped SetLong free()ing extra memory by mistake
 *	1.02	AGC	Made Cmd_Look understand SetHere
 *	1.03	AGC	Made * strip out embedded controls
 *	1.04	AGC	5.07 version - with bitflag names
 *	1.05	AGC	Fixed so it uses right flag names
 *	1.06	AGC	Cmd_Look now does brief mode correctly for no title
 */


void Cmd_SetShort(i)
ITEM *i;
{
	ITEM *a;
	ROOM *r;
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
	r=RoomOf(a);
	if(r==NULL)
	{
		SendItem(i,"%s is not a room.\n",CNameOf(a));
		return;
	}
	SkipPrep();
	GetAll();
	if(!strlen(WordBuffer))
	{
		SendEdit(i,"SetShort #%d %s %s",ItemNumber(LevelOf(i),a),
			CNameOf(a),TextOf(r->rm_Short));
		return;
	}
	FreeText(r->rm_Short);
	r->rm_Short=AllocText(WordBuffer);
}

void Cmd_ShowRoom(i)
ITEM *i;
{
	int ct=0;
	ITEM *a;
	ROOM *r;
	if(!ArchWizard(i))
	{
		SendItem(i,"Try LOOK.\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	r=RoomOf(a);
	if(r==NULL)
	{
		SendItem(i,"%s is not a room.\n",CNameOf(a));
		return;
	}
	SendItem(i,"Room Name : %s\n",CNameOf(a));
	SendItem(i,"Picture   : %d\n",(int)GETPICTURE(r->rm_Flags));
	SendItem(i,"Short Text: %s\n",TextOf(r->rm_Short));
	SendItem(i,"Long Text :\n%s\n",TextOf(r->rm_Long));
	while(ct<16)
	{
		if(r->rm_Flags&(1<<ct))
			SendItem(i,"%s ",RBitName(ct));
		else
			if(strcmp(RBitName(ct),"{unset}"))
				SendItem(i,"-%s ",RBitName(ct));
		ct++;
	}
	SendItem(i,"\n");
}

void Cmd_SetRFlag(i)
ITEM *i;
{
	int c;
	ITEM *a;
	ROOM *r;
	if(!ArchWizard(i))
	{
		SendItem(i,"No can do....\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	r=RoomOf(a);
	if(r==NULL)
	{
		SendItem(i,"%s is not a room.\n",CNameOf(a));
		return;
	}
	SkipPrep();
	while(GetParsedWord())
	{
		if(!strlen(WordBuffer))
		{
			SendItem(i,"What flag though ?\n");
			return;
		}
		if(*WordBuffer=='-')
		{
			c=FindRBit(WordBuffer+1);
			if(c==-1)
			{
				SendItem(i,"Unknown flag %s.\n",WordBuffer+1);
				return;
			}
			r->rm_Flags&=~(1<<c);
		}
		else
		{
			c=FindRBit(WordBuffer);
			if(c==-1)
			{
				SendItem(i,"Unknown flag %s.\n",WordBuffer);
				return;
			}
			r->rm_Flags|=(1<<c);
		}

	}
}

void Cmd_SetLong(i)
ITEM *i;
{
	int siz;
	int bbp;
	int c;
	ITEM *a;
	ROOM *r;
	char *BigBuffer;
	FILE *f;
	if(!ArchWizard(i))
	{
		SendItem(i,"No!\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	r=RoomOf(a);
	if(r==NULL)
	{
		SendItem(i,"%s is not a room.\n",CNameOf(a));
		return;
	}
	SkipPrep();
	GetAll();
	if(!strlen(WordBuffer))
	{
		SendEdit(i,"SetLong #%d %s %s",ItemNumber(LevelOf(i),a),
			CNameOf(a),TextOf(r->rm_Long));
		return;
	}
	if(*WordBuffer!='*')
	{
		BigBuffer=WordBuffer;
		goto l1;
	}
	f=fopen(WordBuffer+1,"r");
	if(f==NULL)
	{
		SendItem(i,"Sorry - can't open the file.\n");
		return;
	}
	BigBuffer=malloc(2048);
	if(BigBuffer==NULL)
	{
		SendItem(i,"Not enough memory.\n");
		return;
	}
	bbp=0;
	while((c=fgetc(f))!=EOF)
	{
		if(c=='\n') c=' ';
		if(c>31)	/* Strip controls */
			BigBuffer[bbp++]=c;
		if(bbp>2047)
		{
			fclose(f);
			free(BigBuffer);
			SendItem(i,"Entry too large.\n");
			return;
		}
	}
	BigBuffer[bbp]=0;
	siz=bbp;
	fclose(f);
	if(BigBuffer[siz-1]=='\n')
		BigBuffer[siz-1]=0;
	BigBuffer[siz]=0;	/* Convert data to C string */
l1:	FreeText(r->rm_Long);
	r->rm_Long=AllocText(BigBuffer);
	if(BigBuffer!=WordBuffer)
		free(BigBuffer);
}


void Cmd_Look(i)
ITEM *i;
{
	ROOM *r;
	OBJECT *o;
	ITEM *l;
	PLAYER *p;
	TABLE *t;
	t=FindTable(103);
	if(t)
	{
		if(ExecBackground(t,i)==-1)
			return;		/* NOTDONE stops describe */
	}
	if(O_PARENT(i)==NULL)
	{
		SetUserTitle(i,"The Void");
		SendItem(i,"You are floating in the void.\n");
		return;
	}
	if(PlayerOf(i)==NULL)
		return;	/* Only Players Can LOOK! */
	r=RoomOf(O_PARENT(i));
	o=ObjectOf(O_PARENT(i));
	p=PlayerOf(O_PARENT(i));
	if(r)
	{
		if(r->rm_Flags&RM_DEATH)
			goto l2;
	}
	if(CanSee(LevelOf(i),O_PARENT(i))==0)
	{
		SendItem(i,"\
You are lost in an incomprehensible haze of unidentifiable images.\n");
		SetUserTitle(i,"Lost");
		return;
	}
	if(IsBlind(i))
	{
		SendItem(i,"You can't see, you are blind.\n");
		SetUserTitle(i,"Blind");
		return;
	}
	if(IsDarkFor(i))
	{
		SetUserTitle(i,"Dark");
		SendItem(i,"It is dark.\n");
		return;
	}

	if((r)&&(PlayerOf(i)->pl_Flags&PL_BRIEF))
	{
		SendItem(i,"%s\n",TextOf(r->rm_Short));
/*		SetUserTitle(i,"%s",TextOf(r->rm_Short))*/;
	}
	else if((o)&&(!r))
		{
			SendItem(i,"You are in %s\n",NameOf(O_PARENT(i)));
			SetUserTitle(i,"In the %s",NameOf(O_PARENT(i)));
		}
		else if(p)
			{
				SendItem(i,"You are being carried by %s\n",
					NameOf(O_PARENT(i)));
				SetUserTitle(i,"Carried by %s",
					NameOf(O_PARENT(i)));
			}

	if(PlayerOf(i)->pl_Flags&PL_BRIEF)
		goto l1;
l2:	if(r)
	{
		SetUserTitle(i,"%s",TextOf(r->rm_Short));
		SendItem(i,"%s",TextOf(r->rm_Long));
	}
	l=O_CHILDREN(O_PARENT(i));
	while(l)
	{
		if(CanSee(LevelOf(i),l))
		{
			if(ObjectOf(l))
			{
				if(ObjectOf(l)->ob_Flags&OB_FLANNEL)
					SendItem(i,"%s",
						TextOf(ObjectOf(l)->ob_Text[O_STATE(l)]));
			}
		}
		l=O_NEXT(l);
	}
	if((r)&&(r->rm_Flags&RM_MERGE)&&(O_PARENT(O_PARENT(i)))&&
		(RoomOf(O_PARENT(O_PARENT(i))))&&
		(RoomOf(O_PARENT(O_PARENT(i)))->rm_Flags&RM_JOIN))
	{
		SendItem(i,"%s",TextOf(RoomOf(O_PARENT(O_PARENT(i)))->rm_Long));
	}
	SendItem(i,"\n");
	t=FindTable(104);
	if(t)
	{
		ExecBackground(t,i);
	}
	if(r&&r->rm_Flags&RM_DEATH)
		goto ldie;
l1:	l=O_CHILDREN(O_PARENT(i));
	while(l)
	{
		if(l!=i)
			DescribeItem(i,l);
		l=O_NEXT(l);
	}
	SendItem(i,"\n");
	t=FindTable(105);
	if(t)
	{
		ExecBackground(t,i);
	}
ldie:	if(r)
	{
		if((r->rm_Flags&RM_DEATH)&&(LevelOf(i)<10000))
		{
			if(UserOf(i)!=-1)
			{
				SendTPacket(UserList[UserOf(i)].us_Port,
						PACKET_CLEAR,"");
				RemoveUser(UserOf(i));
			}
			else
				SendItem(i,"[You would die here if mortal]\n");
		}
	}	
}

void Cmd_Goto(i)
ITEM *i;
{
	ITEM *a;
	if(!ArchWizard(i))
	{
		SendItem(i,"Yes, so give me the route.\n");
		return;
	}
	a=FindSomething(i,O_PARENT(i));
	if(a==NULL)
	{
		SendItem(i,"I don't know what that is.\n");
		return;
	}
	DoesAction(i,4,"vanishes in a haze of multihued light.\n");
	Place(i,a);
	DoesAction(i,4,"appears amidst a puff of greenish smoke.\n");
	Cmd_Look(i);
}

void Cmd_Brief(i)
ITEM *i;
{
	PlayerOf(i)->pl_Flags|=PL_BRIEF;
}

void Cmd_Verbose(i)
ITEM *i;
{
	PlayerOf(i)->pl_Flags&=~PL_BRIEF;
}

void Cmd_SetPicture(i)
ITEM *i;
{
	if(!ArchWizard(i))
	{
		SendItem(i,"What ?\n");
		return;
	}
	SendItem(i,"Obsolete command.\n");
}
