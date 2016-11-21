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
 *	Socket Creation Functions
 *
 *	1.00	Original Version
 *	1.01	License change
 */
 
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#ifdef AMIGA
#include <pragmas/socket_pragmas.h>
#endif

extern int errno;

static struct sockaddr_in myaddress;
static int master_socket;

#ifdef AMIGA
long SocketBase;

int exit_cleanup(void)
{
	if(SocketBase!=NULL)
	{
		CloseLibrary(SocketBase);
		SocketBase=NULL;
	}
}
#endif

/*
 *	Create the server socket
 */
 
int Make_Socket(int port)
{
#ifdef PARANOIA
	char buf[64];
	struct hostent *host;
#endif	
	int v;
	int tmp=1;
#ifdef AMIGA
	if(SocketBase==NULL)
	{
		SocketBase=OpenLibrary("bsdsocket.library",0);
		if(SocketBase==NULL)
		{
			fprintf(stderr,"Fatal error: AmiTCP is not running.\n");
			exit(1);
		}
		SetErrnoPtr(&errno,sizeof(errno));
	}
#endif
#ifdef PARANOIA
	gethostname(buf,63);
	host=gethostbyname(buf);
	if(host==NULL)
	{
		fprintf(stderr,"INET: Error can't find '%s'\n",buf);
		exit(1);
	}
	myaddress.sin_family=host->h_addrtype;
#endif
	myaddress.sin_port=htons(port);
	v=socket(AF_INET,SOCK_STREAM,0);
	if(v==-1)
	{
		fprintf(stderr,"(%d)",errno);
		perror("INET: socket, ");
		exit(1);
	}
	setsockopt(v,SOL_SOCKET,SO_REUSEADDR,&tmp,sizeof(tmp));
	while(bind(v,(struct sockaddr *)&myaddress,sizeof(myaddress))<0)
	{
		if(errno != EADDRINUSE)
		{
			close(v);
			fprintf(stderr,"(%d)",errno);
			perror("INET: bind, ");
			exit(1);
		}
		printf("Address in use: Retrying...\n");
		sleep(5);
	}
	if(listen(v,5)==-1)
	{
		fprintf(stderr,"(%d)",errno);
		perror("INET: listen,");
		exit(1);
	}
	master_socket=v;
	return(v);
}


