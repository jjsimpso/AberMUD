/*
 *	Extra functions to get the Amiga version working nicely
 */

#include <stdio.h>
#include <pragmas/socket_pragmas.h>
extern long SocketBase;


long htonl(long x){return x;}
long ntohl(long x){return x;}
short htons(short x){return x;}
short ntohs(short x){return x;}

void bcopy(void *a,void *b,int c)
{
	memcpy(b,a,c);
}

void bzero(void *p,int n)
{
	memset(p,'\0',n);
}

void sleep(int n)
{
	Delay(50*n);
}

#ifndef MUPX
int gethostname(char *name,int namelen)
{
	char *cp=getenv("HOSTNAME");
	if(cp==NULL)
		return(-1);
	stccpy(name,cp,namelen);
	free(cp);
	return 0;
}
#endif

int select(int nfds,void *readmask,void *writemask, void *exceptmask, void *timeout)
{
	return WaitSelect(nfds,readmask,writemask,exceptmask,timeout,NULL);
}
