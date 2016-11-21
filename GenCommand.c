 /****************************************************************************\
 *									      *
 *			C R E A T O R    O F   L E G E N D S		      *
 *				(AberMud Version 5)			      *
 *									      *
 *  The Creator Of Legends System is (C) Copyright 1989 Alan Cox, All Rights  *
 *  Reserved.		  						      *
 *									      *
 \****************************************************************************/

/*
 *	Some General Command Functions
 */

#include "System.h"

Module "General Commands";
Version "1.01";
Author "----*(A)";

void Cmd_Invisible(i)
ITEM *i;
{
	if(LevelOf(i)<16)
	{
		SendItem(i,"You can't do that.\n");
		return;
	}
	if(i->it_Perception!=0)
	{
		SendItem(i,"You are already invisible.\n");
		return;
	}
	DoesAction(i,4,"vanishes in a haze of multihued light.\n");
	i->it_Perception=LevelOf(i);
	DoesAction(i,4,"is now invisible.\n");
}

void Cmd_Visible(i)
ITEM *i;
{
	if(i->it_Perception==0)
	{
		SendItem(i,"You are already visible.\n");
		return;
	}
	i->it_Perception=0;
	DoesAction(i,4,"appears amidst a puff of greenish smoke.\n");
}

void Cmd_Say(i)
ITEM *i;
{
	GetAll();
	if(!strlen(WordBuffer))
		strcpy(WordBuffer,"something");
	DoesAction(i,7,"says '%s'.\n",WordBuffer);
}

void Cmd_Place(i)
ITEM *i;
{
	ITEM *src;
	ITEM *dest;
	if(!ArchWizard(i))
	{
		SendItem(i,"Sorry, I don't understand that.\n");
		return;
	}
	src=FindSomething(i,O_PARENT(i));
	SkipPrep();
	dest=FindSomething(i,O_PARENT(i));
	if(!src)
	{
		SendItem(i,"I don't know what you are trying to place.\n");
		return;
	}
	if(!dest)
	{
		SendItem(i,"I don't know where you are trying to place that.\n");
		return;
	}
	if(O_PARENT(src))
	{
		if(CanSee(LevelOf(O_PARENT(src)),src))
			SendItem(O_PARENT(src),
			"%s has just vanished from your belongings.\n",CNameOf(src));
	}
	DoesAction(src,4,"vanishes.\n");
	Place(src,dest);
	DoesAction(src,4,"appears with a bang.\n");
	if(CanSee(LevelOf(dest),src))
		SendItem(dest,"%s suddenly appears in your belongings!\n",
			CNameOf(src));
}

