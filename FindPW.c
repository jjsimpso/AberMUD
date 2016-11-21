
 /****************************************************************************\
 *									      *
 *			C R E A T O R    O F   L E G E N D S		      *
 *				(AberMud Version 5)			      *
 *									      *
 *  The Creator Of Legends System is (C) Copyright 1989 Alan Cox, All Rights  *
 *  Reserved.		  						      *
 *									      *
 \****************************************************************************/

#include "System.h"

int main(int argc, char *argv[])
{
	short ct=1;
	UFF u;
	while(argv[ct])
	{
		if(LoadPersona(argv[ct],&u)==-1)
			printf("%s: Not Registered\n",argv[ct]);
		else
			printf("%d:%s: Password %s\n",ct,argv[ct],
				u.uff_Password);
		ct++;
	}
	return 0;
}

void Log(char *fmt, ...){;}

void ErrFunc(char *x, char *y, char *z, int a, char *b)
{
	fprintf(stderr,"%s %s %s %d %s\n",x,y,z,a,b);
	exit(1);
}

