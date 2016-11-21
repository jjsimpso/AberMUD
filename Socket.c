/*
 *	Set up a socket
 *			
 *	Alan Cox 1990
 *
 *	This software is placed in the public domain
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>


/*
 *	Create the client socket
 */
 
int Make_Connection(int port,const char *buf)
{
	struct sockaddr_in myaddress;
	struct hostent *host;
	int v;
	host=gethostbyname(buf);
	if(host==NULL)
	{
	        return(-1);
	}
	myaddress.sin_family=host->h_addrtype;
	myaddress.sin_addr.s_addr=*((long *)host->h_addr);
	myaddress.sin_port=htons(port);
	v=socket(AF_INET,SOCK_STREAM,0);
	if(v==-1)
	{
		return(-1);
	}
	if(connect(v,(struct sockaddr *)&myaddress,sizeof(myaddress))<0)
	{
		close(v);
		return(-1);
	}
	return(v);
}

