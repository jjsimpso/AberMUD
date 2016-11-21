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
 *	Main Driver For System
 */

#include "System.h"
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#define SMALL_MALLOC

#undef	TCP_WRAP		/* Use access controls */

#ifdef	TCP_WRAP
#include	<syslog.h>
#endif

/*
 *	1.00	AGC	Basic Startup
 *	1.01	AGC	Added crash handler
 *	1.02	AGC	MONITOR option
 *	1.03	AGC	Added __interrupt for 5.06 and Lat5
 *	1.04	AGC	Changed to 5.07
 *	1.05	AGC 	Tweaked for 5.08
 *      1.06    AGC     5.10 Unix version uses SIGPIPE
 *	1.07	AGC	5.16 with -p <port> specifier
 *	1.08	AGC	signal function type specifier
 *	1.09	AGC	Signals checked for definition: Amiga merge
 *	1.10	AGC	Main returns int not void
 */

Module "Main Program";
Version "1.10";
Author "Alan Cox";

short post_boot;	/* Set to 1 if game booted */

#ifdef UNIX
#define signal_function	void
#else
#define signal_function int
#endif

signal_function SegV()
{
	Log("Segmentation Fault caused abort");
	abort();
}

signal_function Bus()
{
	Log("Bus error caused abort");
	abort();
}

signal_function Div0()
{
	Log("Divison by zero");
	abort();
}



int main(argc,argv)
int argc;
char *argv[];
{
	extern int EditMode();
	extern int WrapUp();
	extern void SoftwareFailure();
	extern int SysPort;
	if(argc>=3&&strcmp(argv[1],"-p")==0)
	{
		SysPort=atoi(argv[2]);
		if(SysPort<5000)	/* Check RFC compliance */
		{
			fprintf(stderr,"Port number should be 5000 or higher for a service.\n");
			exit(1);
		}
		argv+=2;
		argc-=2;
	}
	printf("\
Aberystwyth Multi-User Dungeon Server\n\
(c) Copyright 1987-2002, Alan Cox.\n\
Release 5.30.0, April 2002\n\n\
Booting...\r");
#ifdef SIGBUS
	signal(SIGBUS,Bus);
#endif
#ifdef SIGSEGV
	signal(SIGSEGV,SegV);
#endif
#ifdef SIGFPE
	signal(SIGFPE,Div0);
#endif
#ifdef SIGPIPE
	signal(SIGPIPE,SIG_IGN);
#endif
/* If we are run from init, escape from the console after reporting the
   boot */
#ifdef UNIX
	if(getppid()==1)
	{
		close(0);
		close(1);
		close(2);
		open("/dev/null",O_RDONLY);
		open("syslog",O_WRONLY|O_APPEND);
		open("syslog",O_WRONLY|O_APPEND);
	}
#endif
	if(argc>2)
	{
		fprintf(stderr,"Arguments! - %s <universe>\n",argv[0]);
		exit(1);
	}
	Log("Startup Commenced");
	if(argv[1])
	{
                printf("Loading '%s'\r",argv[1]);
		Log("Loading Universe '%s'",argv[1]);
		LoadSystem(argv[1]);
		Log("Universe Booted");
	}
        printf("%75s\rBoot Completed\n","");
	Log("Startup Completed");
	Log("IPC Server Starting");

#ifdef	TCP_WRAP
		openlog("abermud",LOG_PID,LOG_MAIL);
#endif

	post_boot=1;
	AddEvent(0,1);	/* Queue Autoboot table */
	IPCMain();
	exit(0);
}

