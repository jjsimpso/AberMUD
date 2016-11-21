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
 *	Daemon Initialise
 */

#include <signal.h>
#include "System.h"

Module  "Bootstrap And Main Loop";
Version "1.03";
Author  "Alan Cox";


jmp_buf Oops;			/* Used when we stack overflow */

extern PORT *Master_Port;	/* Our connection to user processes */

extern int WrapUp();


short SupercedeFlag=0;		/* Set if we recieve a supercede message */

static COMDATA Supercede=	/* Message to send if we find another */
{				/* running when we start ourselves up */
	PACKET_SUPERCEDE,
	-1,
	{ 0,0,0,0 }
};

/*
 *	Set up the IPC channels and being to get things going
 */

void IPCMain(void)
{
	int er;
	Master_Port=CreateMPort(FL_TEMPORARY);	/* The virtual telnet layer or the IPC layer uses this port */
	if(Master_Port==NULL)
	{
		printf("***Abort: Failed to create MASTER_PORT\n");
		exit(10);
	}
	if(FindService("MYTHOS"))	/* Previous one running still */
	{
		PORT *OutPort=FindService("MYTHOS");
		if(OpenMPort(OutPort)) /* Tell it it has been booted off */
		{
			WriteMPort(OutPort,(COMTEXT *)&Supercede,sizeof(COMDATA));
			CloseMPort(OutPort);
		}
		DeAssignService("MYTHOS");
	}
/*
 *	Now tell everyone where to find the Master_Port entry.
 */
	if((er=AssignService("MYTHOS",Master_Port))<0)
	{
		if(er==-10)
		{
			printf("***Abort: MASTER_PORT in use\n");
			exit(0);
		}
		printf("***Abort: AssignService failure.\n");
		exit(1);
	}
	printf("Creator Of Legends: Initialised\n\n");


	BlockOff(Master_Port);
/*
 *	This next piece forms the main loop. We sit around until we get
 *	a line of input, or a second passes.
 */
	while(1)
	{	
		setjmp(Oops);		/* In case of error return here */
		Scheduler();
		SendUser(-2, "");
		ProcessPackets();	/* Alarm interrupted blocked */
		FixLineFaults();	/* Kick oiff everyone timer faulted */
/*
 *	Finally if we have been replaced by newer versions, and no other
 *	person is on the game, then die peacefully
 */
		if(SupercedeFlag&&(CountUsers()==0))
			exit(0);	/* Die peacefully */
	}
}
