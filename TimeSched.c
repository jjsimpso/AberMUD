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
 *	Time Scheduling Control
 *
 *
 */

#include "System.h"

Module	"Time Scheduler";
Version "1.05";
Author  "Alan Cox";

/*
 *	1.00	Basic Scheduling
 *	1.01	Added Facilities To Retain Defined $ME For WHEN
 *	1.02	Fixed crash when $ME died during a WHEN
 *	1.03	Major changes to allow for removal of event after current event
 *		during the current events execution (phew!) - took a while to
 *		find him!
 *	1.04	Added a CountSchedules() function for profiling
 *	1.05	Strict ansification, cleaned up time_t
 */

struct TimeEvent	/* This is a private internal structure */
{
	unsigned long ti_Time;
	short ti_Table;
	ITEM *ti_Runner;
	struct TimeEvent *ti_Next;
};

typedef struct TimeEvent EVENT;

static EVENT *EventList=NULL;
static EVENT *CurrentEvent;

long CountSchedules()
{
	EVENT *c=EventList;
	long ct=0;
	while(c)
	{
		ct++;
		c=c->ti_Next;
	}
	return(ct);
}

void AddEvent(unsigned long tm, short table)
{
	time_t t;	/* How much software will crash when the sign 
				   bit flips ? */
	EVENT *New=Allocate(EVENT);
	EVENT *Walk=EventList;
	EVENT *Prev=NULL;
	time(&t);
	New->ti_Time=t+tm;
	New->ti_Table=table;
	New->ti_Runner=Me();
	LockItem(Me());
/*
 *	Decide Where To Add Entry
 */
	while(Walk)
	{
		if(Walk->ti_Time>=New->ti_Time)
		{
			if(Prev)
			{
				Prev->ti_Next=New;
				New->ti_Next=Walk;
				return;
			}
			else
			{
				New->ti_Next=EventList;
				EventList=New;
				return;
			}
		}
		Prev=Walk;
		Walk=Walk->ti_Next;
	}
	if(Prev)
	{
		Prev->ti_Next=New;
		New->ti_Next=NULL;
	}
	else
	{
		EventList=New;
		New->ti_Next=NULL;
	}
}

static void DeleteEvent(EVENT *Ev)
{
/*
 * The system takes care to avoid deleting the current event twice when the
 * runner of that event occurs. We delete it the FIRST time only, so that 
 * we don't leave the dying person locked.
 */
	if(Ev==CurrentEvent)
		CurrentEvent=NULL;	/* The current event has been eaten */
	UnlockItem(Ev->ti_Runner);
	if(EventList==Ev)
	{
		EventList=Ev->ti_Next;
		free((char *)Ev);
	}
	else
	{
		EVENT *Step=EventList;
		if(Step==NULL)
			Error("DeleteEvent: No Events!");
		while(Step->ti_Next)
		{
			if(Step->ti_Next==Ev)
			{
				Step->ti_Next=Ev->ti_Next;
				free((char *)Ev);
				return;
			}
			Step=Step->ti_Next;
		}
		Error("DeleteEvent: Bad Event Pointer");
	}
}

static void RunEvent(EVENT *event)
{
/*	We keep this little routine seperate in case we need to add multiple
 *	event classes.	
 */

	TABLE *tab;
	Verb=0;		/* 0 is timeout verb code in system */

	tab=FindTable(event->ti_Table);
	if(tab)
		ExecBackground(tab,event->ti_Runner);
}

void Scheduler(void)
{
/*	We have to be careful here, since the event queue may get changed
 *	AS we wander down it. Due to the way this routine currently works
 *	if you keep requeueing items too fast the queue may keep the 
 *	scheduler running forever without returning. For the MUD system 
 *	this is unlikely to cause problems.
 */
	time_t t;
	EVENT *Walk=EventList;
	time(&t);
	while(EventList)
	{
		if(EventList->ti_Time>t)
			break;	/* Finished -> Since queue is time ordered */
		Walk=EventList;		/* Next event is always top */
		CurrentEvent=Walk;	/* CurrentEvent manages dying in WHEN */
		RunEvent(Walk);		/* Do the event */
		if(CurrentEvent)
			DeleteEvent(Walk);
	}
}

void KillEventQueue(ITEM *i)
{
	EVENT *e=EventList,*ne;
	while(e)
	{
		ne=e->ti_Next;
		if(e->ti_Runner==i)
			DeleteEvent(e);
		e=ne;
	}
}

void WipeEventQueue(void)
{
	EVENT *e=EventList,*ne;
	while(e)
	{
		ne=e->ti_Next;
		DeleteEvent(e);
		e=ne;
	}
}
