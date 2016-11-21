#ifdef AMIGA
#include <exec/types.h>
#endif
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#ifndef AMIGA
#include <time.h>
#endif
#include <fcntl.h>
#include <errno.h>
#ifndef AMIGA
#include <arpa/telnet.h>
#else
#define IAC	255
#define DONT	254
#define DO	253
#define WILL	252
#define WONT	251
#define TELOPT_ECHO	1
#endif
#include "System.h"
#include "User.h"
#ifdef AMIGA
#include <pragmas/socket_pragmas.h>
extern long SocketBase;
#define errno Errno()
#endif

#define MK_NET(a,b,c,d)	((a<<24)|(b<<16)|(c<<8)|d)

/*
 *	Implement embedded comms client, and telnet modes direct from
 *	the server. This replaces the front end stuff to drive tcp/ip
 *	on 5.14. We now do it all BSD style with sockets and select,
 *	so the game shouldn't need a sys5 style interface anywhere.
 *
 *	1.00		AGC		Created Initial Version For 5.16
 *	1.01		AGC		Fixed Minor Bugs/Upgrades
 *	1.02		AGC		Added Client Mode Switch
 *	1.03		AGC		Added '!' facility for nonclienters
 *	1.04		AGC		Tidying Up Of Code
 *	1.05		AGC		Added stuff for -p <port>
 *	1.06		AGC		Spots dropped idle lines
 *      1.07            AGC             HARDBANNER optional
 *	1.08		CCS		Added TCP_WRAPPER and echo control
 *	1.09		AGC		Removed wrapper, moved motd code.
 *	1.10		AGC		Fix endian problems, also fixes for some dodgier tcp stacks
 *	1.11		AGC		Client mode fixed (silly mistake), round robin scheduler.
 *	1.12		AGC		BSX graphics support
 *	1.13		AGC		Minor client mode bug, and @TMS bug fixed
 *	1.14		AGC		'Smart' prompt code. Only alpha prompts are sent to BSX users
 *	1.15		AGC		Server supports AmiTCP. Small changes only. I am seriously impressed
 *					with the AmiTCP package.
 */

extern USER UserList[];

Module "IPCDirect";
Version "1.15";
Author "Alan Cox";

/*
 *	This is definitely a case for a big structure for each user, but
 *	I do a lot of scanning of this data, even on fairly idle channels
 *	so stuffing it into arrays, in this chosen order lowers paging
 *	Its probably academic considering the amount of background stuff
 *	people put in a typical abermud5
 */

static char UserLine[MAXUSER][81];
static short UserPos[MAXUSER];
static char SnoopLine[MAXUSER][81];
static short SnoopPos[MAXUSER];
static char Prompts[MAXUSER][64];
/* Stuff below this comment gets scanned every second */
static char UserInput[MAXUSER][514];
static short InputPos[MAXUSER];
static char UserState[MAXUSER];
static char UserEcho[MAXUSER];

static int MainFD,AltFD,BsxFD;
static fd_set ReadMask,WriteMask;
int SysPort=TCP_PORT;

/*
 *	This info is needed elsewhere (BSX.c)
 */

int IsBSX(int u)
{
	if(UserState[u]==2)
		return(1);
	return(0);
}

/*
 *	Given a file descriptor, find its owner process. Assumes that all
 *	channels have an owner. See changes to ComServer Handle_Login() to
 *	make this true
 */

int FindUserFD(int fd)
{
	int ct=0;
	while(ct<MAXUSER)
	{
		if(UserList[ct].us_Port&&(UserList[ct].us_Port->po_fd&255)==(fd&255))
		{
			return(ct);
		}
		ct++;
	}
	Error("No User associated with fd");	/* Boom! */
}

int IsUserFD(int fd)
{
	int ct=0;
	while(ct<MAXUSER)
	{
		if(UserList[ct].us_Port&&(UserList[ct].us_Port->po_fd&255)==(fd&255))
			return(ct);
		ct++;
	}
	return(-1);
}

/*
 *	Grab incoming data from a channel. If we finish reading a line, make
 *	a packet of it and stuff it into the main game.
 */

int ReadBlock(int u, COMTEXT *pkt)
{
/* We read only when we get something pending off select - so if we
   read nothing first try they have buggered off */
	int gotany=0;
	char c;
	int l;
unsigned	char	t_iac=IAC,
			t_will=WILL,
			t_wont=WONT,
			t_do  =DO,
			t_dont=DONT;

	while((l=recv(UserList[u].us_Port->po_fd,&c,1,0))>0)
	{
		gotany=1;

/*
 * Telnet traps - get dump any IAC stuff
 */

		if( ((unsigned char) c) == t_iac)
		{
			read(UserList[u].us_Port->po_fd,&c,1);

			if( ((unsigned char)c== t_will)
			||  ((unsigned char)c== t_wont)
			||  ((unsigned char)c== t_do  )
			||  ((unsigned char)c== t_dont) )
			{
				read(UserList[u].us_Port->po_fd,&c,1);
			}
			continue;
		}

	        if(c=='!'&&InputPos[u]==0&&UserState[u]==0)
		{
			c='\n';
			InputPos[u]=strlen(UserInput[u]);
		}
		if(c==8||c==127)
		{
			if(InputPos[u]>0&&UserState[u]==0)
				InputPos[u]--;
			return(0);
		}
		if(c!='\r')
		{
			UserInput[u][InputPos[u]++]=c;
			if(InputPos[u]==513||c=='\n')
			{
				if(UserState[u]==0 && UserEcho[u]==1)
				{
					CharPut(u,'\r');	/* Fix echo in nonecho mode */
					CharPut(u,'\n');
				}
				UserInput[u][InputPos[u]-1]=0;
				if(UserState[u]>0 && UserEcho[u]==0)	/* BSX expects us to neatly echo command */
				{
					if(*UserInput[u]!='#')		/* But not # commands */
					{
						char *p=UserInput[u];
						CharPut(u,'>');
						CharPut(u,' ');
						while(*p)
							CharPut(u,*p++);
						CharPut(u,'\r');
						CharPut(u,'\n');
					}
				}
				strcpy(pkt->pa_Data,UserInput[u]);
				pkt->pa_Sender=u;
				if(UserState[u]==2 && UserInput[u][0]=='#')
					pkt->pa_Type=PACKET_BSXSCENE;	/* BSX process the line */
				else
					pkt->pa_Type=PACKET_COMMAND;
				InputPos[u]=0;
				return(1);
			}
		}
	}
	/* EAGAIN added for SYS5 machines */
	if((l==-1 && errno!=EWOULDBLOCK && errno != EAGAIN) || gotany==0)
	{
		/* Network fault.. shove it */
		UserList[u].us_Port->po_Flags|=FL_FAULT;
		FD_CLR(UserList[u].us_Port->po_fd,&ReadMask);
		FD_CLR(UserList[u].us_Port->po_fd,&WriteMask);
	}
	return(0);
}

/* 
 *	Client used to do formatting, so we have to do wrapping here now
 */

void SnoopPut(int u, char c)
{
	SnoopLine[u][SnoopPos[u]++]=c;
}

void SnoopFlush(int u, int n)
{
	if(UserState[u]==1)
		WriteSocket(u,"\002S",2);
	else
	        WriteSocket(u,"|",1);
	WriteSocketText(u,SnoopLine[u],n);
	WriteSocket(u,"\r\n",2);
	if(UserState[u]==1)
		WriteSocket(u,"\003",1);
	SnoopPos[u]=0;
}


void SnoopCharPut(int u, char c)
{
	if(SnoopPos[u]==79)
	{
		if(c==' '||c=='\n'||c=='\t')
			SnoopFlush(u,79);
		else
		{
			int ct=78;
			while(ct--)
			{
				if(SnoopLine[u][ct]==' '||SnoopLine[u][ct]=='\t')
				{
					SnoopFlush(u,ct);
					while(++ct<=78)
						SnoopPut(u,SnoopLine[u][ct]);
					SnoopPut(u,c);
					return;
				}
			}
			SnoopFlush(u,78);
			SnoopPut(u,c);
		}
	}
	else
	{
		if(c=='\n')
		{
			SnoopFlush(u,SnoopPos[u]);
			return;
		}
		SnoopPut(u,c);
	}
	return;
}		

/* Formatting for normal text output */

void CharPut(int u, char c)
{
	if(UserPos[u]==79)
	{
		if(c==' '||c=='\n'||c=='\t')
			LineFlush(u,79);
		else
		{
			int ct=78;
			while(ct--)
			{
				if(UserLine[u][ct]==' '||UserLine[u][ct]=='\t')
				{
					LineFlush(u,ct);
					while(++ct<=78)
						LinePut(u,UserLine[u][ct]);
					LinePut(u,c);
					return;
				}
			}
			LineFlush(u,78);
			LinePut(u,c);
		}
	}
	else
	{
		if(c=='\n')
		{
			LineFlush(u,UserPos[u]);
			return;
		}
		LinePut(u,c);
	}
	return;
}		


void LinePut(int u, char c)
{
	UserLine[u][UserPos[u]++]=c;
}

void LineFlush(int u, char n)
{
	WriteSocketText(u,UserLine[u],n);
	WriteSocket(u,"\r\n",2);
	UserPos[u]=0;
}

void FieldShift(int u, int f)
{
	if(f==0||f>77)
		return;
	do
	{
		CharPut(u,' ');
	}
	while(UserPos[u]%f!=0);
}

/* Fake the port binding operating - we use the fd that is involved as the
   key to the port */

PORT *Bind_Port(int u, int p)
{
	PORT *a=Allocate(PORT);

	a->po_fd=(p&255);
	UserState[u]=0;
	UserEcho[u]=0;
	UserState[u]=p/256;
	UserPos[u]=0;
	UserInput[u][0]=0;
	Prompts[u][0]=0;
	InputPos[u]=0;
	SnoopPos[u]=0;
	a->po_Flags=0;
	a->po_SiloPtr=0;
	return(a);
}

/* Set up the master socket */

PORT *CreateMPort(int f)
{
	PORT *p=Allocate(PORT);
	p->po_fd=Make_Socket(SysPort);
	MainFD=p->po_fd;
	FD_ZERO(&ReadMask);
	FD_SET(p->po_fd,&ReadMask);
	AltFD=Make_Socket(SysPort+1);
	FD_SET(AltFD,&ReadMask);
	BsxFD=Make_Socket(SysPort+2);
	FD_SET(BsxFD,&ReadMask);
	return(p);
}

/* Open a port, meaningless really */

PORT *OpenMPort(PORT *port)
{
	port->po_Open++;
	return(port);
}

/* Close down a port */

int CloseMPort(PORT *port)
{
	SiloFlush(port);
	port->po_Open--;
#ifdef AMIGA
	CloseSocket(port->po_fd);
#else
	close(port->po_fd);
#endif
	FD_CLR(port->po_fd,&ReadMask);
	FD_CLR(port->po_fd,&WriteMask);
	free((char *)port);
	return 0;
}

/* We don't use the blocking switches at all in this set of IPC code */

void BlockOn(PORT *p){;}
void BlockOff(PORT *p){;}

/*
 *	Take an outgoing packet service and turn it into action on the
 *	sockets.
 */
 
int WriteMPort(PORT *a, COMTEXT *block, int len)
{
	COMDATA *d=(COMDATA *)block;
	char *fp;
	int u=FindUserFD(a->po_fd);
	switch(block->pa_Type)
	{
		case PACKET_OUTPUT:;
			fp=block->pa_Data;
			while(*fp)
				CharPut(u,*fp++);
			break;
		case PACKET_CLEAR:
			fp=block->pa_Data;
			while(*fp)
				CharPut(u,*fp++);
			if(UserState[u]==2)
				WriteSocket(u,"@TMS",4);
			break;
		case PACKET_SETPROMPT:
			if(UserState[u]!=1)
				strcpy(Prompts[u],block->pa_Data);
			else
			{
				WriteSocket(u,"\002P",2);
				WriteSocket(u,block->pa_Data,strlen(block->pa_Data));
				WriteSocket(u,"\003",1);
			}
			break;
		case PACKET_LOGINACCEPT:
			InputPos[u]=0;
			break;
		case PACKET_EDIT:
			if(UserState[u]==1)
			{
				WriteSocket(u,"\002T",2);
				WriteSocket(u,block->pa_Data,strlen(block->pa_Data));
				WriteSocket(u,"\003",1);
			}
			break;
		case PACKET_SNOOPTEXT:
			fp=block->pa_Data;
			while(*fp)
				SnoopCharPut(u,*fp++);
			break;
		case PACKET_SETTITLE:break;
		case PACKET_LOOPECHO:
			break;	/* Cant be bothered.. */
		case PACKET_ECHOBACK:break;
		case PACKET_BSXSCENE:	/* Graphics stuff - only goes to BSX users */
			if(UserState[u]==2)
				WriteSocket(u,block->pa_Data,strlen(block->pa_Data));
			break;
		case PACKET_ECHO:
			UserEcho[u]=d->pa_Data[0];
			switch(UserState[u])
			{
				case 1:
					WriteSocket(u,"\002E",2);
					WriteSocket(u,d->pa_Data[0]?"Y\003":"N\003",2);
					break;
				case 2:
					break;
				case 0:
					{
						unsigned	char	t_iac=IAC,
									t_will=WILL,
									t_wont=WONT,
									t_echo=TELOPT_ECHO;

						WriteSocket(u,(char *)&t_iac,1);
						WriteSocket(u,(d->pa_Data[0])?((char *)&t_will):((char *)&t_wont),1);
						WriteSocket(u,(char *)&t_echo,1);
					}
					break;	/* Dont bother with echoing now */
			}
			break;
		case PACKET_INPUT:
			switch(UserState[u])
			{
				case 2:
					if(!isalpha(*Prompts[u]))
						break;
					WriteSocketText(u,Prompts[u],strlen(Prompts[u]));
					WriteSocketText(u,"\r\n",2);
					break;
				case 0:
					WriteSocketText(u,Prompts[u],strlen(Prompts[u]));
					break;
				case 1:
					if(isalpha(*Prompts[u]))
					{
						WriteSocketText(u,Prompts[u],strlen(Prompts[u]));
						WriteSocketText(u,"\r\n",2);
					}
					WriteSocket(u,"\002G\003",3);
					break;
			}
			break;
		case PACKET_SETFIELD:
			FieldShift(u,d->pa_Data[0]);
			break;
	}
	return(0);
}

/* Delete a closed port */

int DeleteMPort(PORT *a)
{
	if(a->po_Open)
	{
		a->po_Flags|=FL_DELETE;
		return(0);
	}
#ifdef AMIGA
	CloseSocket(a->po_fd);
#else
	close(a->po_fd);
#endif
	free((char *)a);
	return(0);
}

/* We don't use the service finding stuff any more */

int AssignService(char *s, PORT *p){return(0);}
int DeAssignService(char *s){return(0);}
PORT *FindService(char *s) {return NULL;}

/*
 * 	Wait up to a second for a message to read.
 *
 *	This code has changed only slightly for Beta3, but the implications are significant. Previously
 *	lower numbered connections got a distinct advantage. The new code does a round robin scan of
 *	file decriptors. 
 */

int ReadMPort(PORT *a, COMTEXT *b)
{
	struct timeval tvl;
	static long magic_time_cookie=0;
	long t;
	static int ct=0;	/* Now static */
	fd_set rfm;/*WriteMask;*/
	int oct=ct;

	/* For non ANSI compilers... */
	memcpy(&rfm,&ReadMask,sizeof(rfm));

	/* Initialise tvl seperately to keep non ansi compilers happy */
	tvl.tv_sec=1;
	tvl.tv_usec=0;

	FD_ZERO(&WriteMask);

	/*
	 *	Slow machine fix: Stops busy machine never running background jobs
	 */

	time(&t);
	if(t-magic_time_cookie>2)
	{
		magic_time_cookie=t;
		return(-2);
	}
	if(select(NFDBITS,&rfm,NULL,NULL,&tvl)==-1)
	{
		perror("select");
		exit(1);
	}
	/* Now do reading */
	do
	{
		if(ct==MainFD||ct==AltFD||ct==BsxFD)
		{
			if(FD_ISSET(ct,&rfm))
			{
				return(MakeConnection(ct,b));
/*				return(0);*/
			}
		} 
		else if(FD_ISSET(ct,&rfm))
		{
			if(IsUserFD(ct)!=-1 && ReadBlock(FindUserFD(ct),b)==1)
			{
				return(0);
			}
		}
		ct++;
		if(ct==NFDBITS)
			ct=0;
	}
	while(ct!=oct);
	return(-2);
}

/* Handle non-blocking socket queues and the incomplete writes stuff */

int Silo(PORT *port, char *block, int len)
{
	if(port->po_SiloPtr+len>8191-sizeof(int))
	{
		port->po_Flags|=FL_FAULT;
		FD_CLR(port->po_fd,&ReadMask);
		FD_CLR(port->po_fd,&WriteMask);
		return(-20);
	}
	memcpy(port->po_Silo+port->po_SiloPtr,block,len);
	port->po_SiloPtr+=len;
	return(0);
}

int SiloFlush(PORT *port)
{
	int sz;
	if(port==NULL)
		return(0);
	if(port->po_Flags&FL_FAULT)
		return(-1);
	sz=send(port->po_fd,port->po_Silo,port->po_SiloPtr,0);
	if(sz==-1&&errno!=EWOULDBLOCK&&errno!=EAGAIN)
	{
		port->po_Flags|=FL_FAULT;
		FD_CLR(port->po_fd,&ReadMask);
		FD_CLR(port->po_fd,&WriteMask);
		return(-1);
	}
	port->po_SiloPtr-=sz;
/* Have to bcopy this one : must NOT memcpy it as we need to ensure
   it handles overlaps right. If you dont have such a beast write one
   If you have bcopy but not memcpy,then replace the memcpy's - thats ok 
*/
	if(port->po_SiloPtr==0)
		return(0);
	bcopy(port->po_Silo+sz,port->po_Silo,port->po_SiloPtr);
	return(-1);
}

/* Send data to someone */

int WriteSocket(int user, char *data, int bytes)
{
	int l;
	if(SiloFlush(UserList[user].us_Port))
	{
		return(Silo(UserList[user].us_Port,data,bytes));
	}
	l=send(UserList[user].us_Port->po_fd,data,bytes,0);
	if(l<bytes)
		if(Silo(UserList[user].us_Port,data+l,bytes-l)!=0)
			return(-1);
	return(0);
}

/* BSX MUD uses @xyz as command strings. This means we have to quote our @
   characters in BSX mode as @TXT@$ */

int WriteSocketText(int user, char *data, int bytes)
{	
	/* Do all we can to avoid lots of messy little writes */
	/* If @TXT actually worked on the BSX client I might even bother
	   checking this code works! */
#ifdef DONTDEF
	char *t;
	if(UserState[user]!=2)
#endif
		return WriteSocket(user,data,bytes);
#ifdef DONTDEF
	t=memchr(data,'@',bytes);
	if(t==NULL)
		return WriteSocket(user,data,bytes);
	/* We've tried all we can but we will have to do some thinking */
	do
	{
		if(t!=data)
		{
			if(WriteSocket(user,data,t-bytes)==-1)
				return(-1);
		}
		if(WriteSocket(user,"@TXT@$",6)==-1)
			return(-1);
		bytes-=(t-data);
		bytes--;	/* For the '@' too */
		data=t+1;
		t=memchr(data,'@',bytes);
	}
	while(t!=NULL);
	/* All dealt with at last */
	return(0);
#endif
}

void WriteFlush(int u){;}

/* Convert an IP address to a text string. We can't go further than ip
   names as the lookup can take up to 30 seconds. This is not acceptable
   in a server environment  - note the address has already been fed
   through ntohl() */

char *NetName(unsigned long l)
{
	int code[4];
	static char name[64];
	code[0]=l%65536;
	code[2]=l/65536;
	code[3]=code[2]/256;
	code[2]%=256;
	code[1]=code[0]/256;
	code[0]%=256;
	sprintf(name,"%d.%d.%d.%d",code[3],code[2],code[1],code[0]);
	return(name);
}

#ifdef	TIME_LIMITS

#define	CLOSEHOUR	9
#define	CHECK		18

int can_play_now(void)
{
	time_t          x;
	struct tm      *a;
        
	time(&x);
	a = localtime(&x);
	return (a->tm_hour >= CHECK || a->tm_hour < CLOSEHOUR ||
		a->tm_wday == 6 || a->tm_wday == 0);
}

#endif /* TIME_LIMITS */                                       


/*
 *	Fake up a login request packet
 */

int MakeConnection(int fd, COMTEXT *p)
{
	struct sockaddr_in netaddr;
	socklen_t lv=sizeof(netaddr);
	int ofd=fd;
#ifdef AMIGA
	long yes=1;
#endif

#ifndef AMIGA
	fcntl(fd,F_SETFL,O_NDELAY);
#endif
	fd=accept(fd,(struct sockaddr *)&netaddr,&lv);
#ifndef AMIGA
	fcntl(fd,F_SETFL,0);
#endif

	p->pa_Type=PACKET_LOGINREQUEST;
	p->pa_Sender=fd;

	/* Reverse the address, that makes it easier to work with everywhere else */
	netaddr.sin_addr.s_addr=ntohl(netaddr.sin_addr.s_addr);

	sprintf(p->pa_Data,"%s%s$%d$*","Internet:",NetName(netaddr.sin_addr.s_addr),
			fd+(ofd==AltFD?256:0)+(ofd==BsxFD?512:0));

/* SWANSEA STUFF - PLEASE ENFORCE ELSEWHERE TOO... */
	if(netaddr.sin_addr.s_addr==MK_NET(137,44,1,1)
		||netaddr.sin_addr.s_addr==MK_NET(143,52,2,10))
	{
		sprintf(p->pa_Data,"BAD$SITE");
		p->pa_Sender= -1;
		send(fd,"Not permitted from this site\n\r",30,0);
#ifdef AMIGA
		CloseSocket(fd);
#else
		close(fd);
#endif
		return(-1);
	}

/*	printf("***GOT CONNECTION\n");*/
#ifndef AMIGA
	fcntl(fd,F_SETFL,O_NDELAY);
#else
	/* AmiTCP has the BSD blocking ioctl() but lacks the use of fcntl() */
	ioctl(fd,FIONBIO,&yes);
#endif
	FD_SET(fd,&ReadMask);
	return(0);
}


