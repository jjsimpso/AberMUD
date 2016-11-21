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
 *	Daemon Communications Server Routines
 *
 *	Strange this but the theory behind the user being in a state
 *	and of having a set of state changing vectors actually came
 *	from the original pre-abermud basic experiments on a sirius!
 */

#include "System.h"		/* System includes and options */
#include "User.h"

Module "Communication Server";
Version "1.28";
Author "----*(A)";

/*
 *	1.21	AGC	General Version With SUPERCEDE
 *	1.22	AGC	Added acknowledged clear to avoid crashes
 *	1.23	AGC	Fixed TPacket sent as NPacket bug in Handle_Login
 *	1.24	AGC	Changes to fit in nicely with new IPC
 *	1.25	AGC	Tweaks to FixLineFaults()
 *	1.26	AGC	Moved motd code
 *	1.27	AGC	BSX Aware
 *	1.28	AGC	ANSIfication cleanup
 */

PORT *Master_Port=NULL;		/* Our Port  	    		*/
USER UserList[MAXUSER];		/* Information about each user  */
short LineFault[MAXUSER];	/* 1 = user has timeout problem */

void FixLineFaults(void)
{
	int ct=0;
	while(ct<MAXUSER)
	{
		if(UserList[ct].us_Port!=NULL)
		{
			SiloFlush(UserList[ct].us_Port);
			if(LineFault[ct]||UserList[ct].us_Port->po_Flags&FL_FAULT)
			{
				/* Fighting.. no getouts allowed */
				if(UserList[ct].us_Item && GetUserFlag(UserList[ct].us_Item,2)!=0)
				{
					ct++;
					continue;
				}
				RemoveUser(ct);
				UserList[ct].us_State=AWAIT_LOGIN;
			}
		}
		LineFault[ct]=0;
		ct++;
	}
}


int Current_UserList;		/* Number of the current user   */


/*
 *	Post a block of data to a PLAY process.
 */

int SendBlock(PORT *us, COMTEXT *block, int size)
{
	int er;
	if((er=WriteMPort(us,block,size))<0)
	{
		if(er!=-20)
		{
			Log("Error: Port write fail - %d\n",er);
			exit(10);
		}
		return(-20);	/* Queue Filled */
	}
	return(1);
}

/*
 *	Post a text block to a PLAY process
 */

int SendTPacket(PORT *us, short class, char *data)
{
	COMTEXT a;
	a.pa_Type=class;
	a.pa_Sender=-1;
	strcpy(a.pa_Data,data);
	return(SendBlock(us,&a,strlen(data)+2*(sizeof(short))+1));
}

/*
 *	Post a numeric block to a PLAY process
 */
	
int SendNPacket(PORT *us, short class, short p1, short p2, short p3, short p4)
{
	COMDATA a;
	a.pa_Type=class;
	a.pa_Sender=-1;
	a.pa_Data[0]=p1;
	a.pa_Data[1]=p2;
	a.pa_Data[2]=p3;
	a.pa_Data[3]=p4;
	return(SendBlock(us,(COMTEXT *)&a,sizeof(COMDATA)));
}
	

/*
 *	Get a message sent to us from the Master_Port
 */

int GetPacket(PORT *port, COMTEXT *packet)
{
	int er;
	BlockOff(port);
	if((er=ReadMPort(port, packet))<0)
	{
		if(er==-3||er==-2)	/* Interruted call */
			return(-2);	/* Port empty */
		exit(11);
	}
	if(packet->pa_Type==PACKET_BONG)
		return(-2);	/* Fake a timer event */
	return(0);
}

/*
 *	This function starts the process of logging a new user into the 
 *	game environment.
 */

void Handle_Login(char *msg)
{
	PORT *port;
	long v;
	char *a;
	int ct=0;
	char ubf[MAXUSERID+64];
	char mbuf[256];
	FILE *motd;
	extern PORT *Bind_Port();
/*
 *	Look for a free slot
 */
	while(ct<MAXUSER)	
	{
		if(UserList[ct].us_Name[0]==0)
			break;
		ct++;
	}
/*
 *	See if there was room to login
 */
	if(ct==MAXUSER-1||ct>=MaxSlot())
	{
		while(*msg!='$') msg++;
		*msg++=0;
		if(sscanf(msg,"%ld$%s",&v,ubf)==0)
		{
			Log("Error: Corrupt Login Attempt");
			return;
		}
		port=(PORT *)Bind_Port(ct,v);
		if(port==NULL)
		{
			UserList[ct].us_Port=NULL;
			return;
		}
		if(port->po_fd&256)
		{
			if(ct<MAXUSER-1)
				goto oops;
		}
		/* New IPC needs a user associating with the channel
		   before we do work on it */
		UserList[ct].us_Port=port;
		SendTPacket(port,PACKET_CLEAR,
			"Sorry..... the game is currently full.\n");
		CloseMPort(port);
		UserList[ct].us_Port=NULL;
	}
	else
/*
 *	Accept the new login and enter it into the userlist
 */
	{
oops:
		a=msg;
		while(*msg!='$')
			msg++;
		*msg++=0;
		if(!sscanf(msg,"%ld$%s",&v,ubf))
		{
			Log("Error: Corrupt Login");
			return;
		}
		port=Bind_Port(ct,v);
		if(port==NULL)
		{
			Log("Error: Port Open Failed");
			return;
		}
		strcpy(UserList[ct].us_UserName,a);
		strcpy(UserList[ct].us_Name,"<login>");
		UserList[ct].us_State=AWAIT_NAME;
		UserList[ct].us_Flags=0;
		UserList[ct].us_Port=port;
		time(&(UserList[ct].us_Login));
		if(*ubf!='*')
			sprintf(UserList[ct].us_UserName,"Internet:%s",ubf);
/*
 *	Game Login Code Here
 */
		if(!IsBSX(ct) || (motd=fopen("motd.bsx","r"))==NULL)
			motd=fopen("motd","r");
		if(motd)
		{
			while(fgets(mbuf,255,motd)!=NULL)
			{
				SendUser(ct,mbuf);
			}
			fclose(motd);
		}
		SendNPacket(port,PACKET_LOGINACCEPT,0,ct,0,0); /* login ok */
		SendNPacket(port,PACKET_ECHO,0,0,0,0);	/* Echo On */
		SendTPacket(port,PACKET_SETPROMPT,"What be thy name ? ");
		SendNPacket(port,PACKET_INPUT,1,0,0,0); /* Go ahead and type */
	}
	return;
}

/*
 *	Decide what to do with a packet recieved from a PLAY
 */

void InterpretPacket(COMTEXT *x)
{
	Current_UserList=x->pa_Sender;

	switch(x->pa_Type)
	{
		case PACKET_SUPERCEDE:	/* Sent when new SERVER takes over */

			SupercedeFlag=1;
			break;

		case PACKET_ABORT:	/* Sent to cause termination */

			Broadcast("Remote Abort Requested: Terminating Abruptly.\n",0);
			exit(0);

		case PACKET_CLEAR:		/* Client gone pop! */

			if(Current_UserList!=-1)
			{
				printf("Time Out From %d\n",Current_UserList);
				TimeOut(Current_UserList);
			}
			break;
			
		case PACKET_CLEARED:		/* Sent to confirm a CLEAR request */

			if(Current_UserList!=-1)
				UserList[Current_UserList].us_State=AWAIT_LOGIN;
			break;

		case PACKET_LOOPECHO:		/* Loopback for testing */
		
			if(Current_UserList!=-1)
				SendNPacket(UserList[Current_UserList].us_Port,
					PACKET_ECHOBACK,0,0,0,0);
			break;	/* Honour echoback protocols */

		case PACKET_COMMAND:    	/* Input from a player */
	
			Handle_Command(Current_UserList,x->pa_Data);
			break;

		case PACKET_LOGINREQUEST:	/* A new login */
		
			Handle_Login(x->pa_Data);
			break;

		case PACKET_OUTPUT:

			Handle_Output(x->pa_Data);	/* Console Out */
			break;

		case PACKET_COMMFORCE:		/* Command off menu bar */

			Handle_CommForce(Current_UserList,x->pa_Data);
			break;	/* State AWAIT_COMMAND command */
		
		case PACKET_BSXSCENE:		/* Incoming BSX Request */
			
			Handle_BSXPacket(Current_UserList,x->pa_Data);
			break;

		default: Log("Invalid Packet: Type %d.\n",
				x->pa_Type);
	}
}

/*
 *	Handle all of the pending input from the Master_Port
 */

void ProcessPackets(void)
{
	int er;
	COMTEXT p;
	while(1)
	{
		er=GetPacket(Master_Port,&p);
		if(er==-2)
			break;
		InterpretPacket(&p);
	}
}

int Handle_Output(char *x)
{
	printf("%s",x);	/* For Now */
	return(0);
}

/*
 *	Sort out what must be done when a user does a command
 */

int Handle_Command(int ch, char *x)
{
	if(ch==-1)
		return(-1);
	Command_Driver(ch,UserList[ch].us_State,x);
	return(0);
}


int Handle_CommForce(int ch, char *x)
{
	if(ch==-1)
		return(-1);
	if(UserList[ch].us_State==AWAIT_COMMAND)
		Command_Driver(ch,AWAIT_COMMAND,x);
	return(0);
}
