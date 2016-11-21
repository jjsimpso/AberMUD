#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define LINUX

#ifndef LINUX

char *strdup(x)
char *x;
{
	extern char *malloc();
	char *t=malloc(strlen(x)+1);
	if(t==NULL)
		return(NULL);
	strcpy(t,x);
	return(t);
}

#endif

int stricmp(a,b)
char *a,*b;
{
	while(*a)
	{
		char x,y;
		x=*a;
		y=*b;
		if(isupper(x)) x=tolower(x);
		if(isupper(y)) y=tolower(y);
		if(x<y)
			return(-1);
		if(x>y)
			return(1);
		a++;
		b++;
	}
	if(*b!=0)
		return(1);
	return(0);
}

#ifndef LINUX

rename(a,b)
char *a,*b;
{
	if(link(a,b)==-1)
		return(-1);
	if(unlink(a)==-1)
		return(-1);
	return(0);
}


#endif

char *strtok2(a,b,c)
char *a,*b,*c;
{
	static char *d,*e;
	if(a!=NULL)
		d=a;
	if(d==NULL)
		return(NULL);
	while(*d&&strchr(b,*d))
	{
		if(*d=='{')	/* FUDGE FUDGE... */
		{
			d++;
			break;
		}
		d++;
	}
	if(*d==0)
	{
		d=NULL;
		return(NULL);
	}
	e=d;
	while(*d&&strchr(c,*d)==NULL)
		d++;
	if(*d)
		*d++=0;
	return(e);
}
