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
 *	User Control Files
 */

#include "System.h"
#include <netinet/in.h>

Module "User File";
Version "1.03";
Author "Alan Cox";

/*
 *	1.00	Original very standard version
 *	1.01	Saves userflags too!
 *	1.02	Uses htons() htonl(), and being word aligned is more portable
 *	1.03	Added netinet/in.h prototypes
 */

/*#define DONT_NETWORK_SWAP*/	/* Turn off use of network form */


void SwapUFFToHost(UFF *r)
{
#ifndef DONT_NETWORK_SWAP
	r->uff_Perception=ntohs(r->uff_Perception);
	r->uff_ActorTable=ntohs(r->uff_ActorTable);
	r->uff_ActionTable=ntohs(r->uff_ActionTable);
	r->uff_Size=ntohs(r->uff_Size);
	r->uff_Weight=ntohs(r->uff_Weight);
	r->uff_Strength=ntohs(r->uff_Strength);
	r->uff_Flags=ntohs(r->uff_Flags);
	r->uff_Level=ntohs(r->uff_Level);
	r->uff_Score=ntohl(r->uff_Score);
	r->uff_Flag[0]=ntohl(r->uff_Flag[0]);
	r->uff_Flag[1]=ntohl(r->uff_Flag[1]);
	r->uff_Flag[2]=ntohl(r->uff_Flag[2]);
	r->uff_Flag[3]=ntohl(r->uff_Flag[3]);
	r->uff_Flag[4]=ntohl(r->uff_Flag[4]);
	r->uff_Flag[5]=ntohl(r->uff_Flag[5]);
	r->uff_Flag[6]=ntohl(r->uff_Flag[6]);
	r->uff_Flag[7]=ntohl(r->uff_Flag[7]);
	r->uff_Flag[8]=ntohl(r->uff_Flag[8]);
	r->uff_Flag[9]=ntohl(r->uff_Flag[9]);
#endif
}

void SwapUFFToNeutral(UFF *r)
{
#ifndef DONT_NETWORK_SWAP
	r->uff_Perception=htons(r->uff_Perception);
	r->uff_ActorTable=htons(r->uff_ActorTable);
	r->uff_ActionTable=htons(r->uff_ActionTable);
	r->uff_Size=htons(r->uff_Size);
	r->uff_Weight=htons(r->uff_Weight);
	r->uff_Strength=htons(r->uff_Strength);
	r->uff_Flags=htons(r->uff_Flags);
	r->uff_Level=htons(r->uff_Level);
	r->uff_Score=htonl(r->uff_Score);
	r->uff_Flag[0]=htonl(r->uff_Flag[0]);
	r->uff_Flag[1]=htonl(r->uff_Flag[1]);
	r->uff_Flag[2]=htonl(r->uff_Flag[2]);
	r->uff_Flag[3]=htonl(r->uff_Flag[3]);
	r->uff_Flag[4]=htonl(r->uff_Flag[4]);
	r->uff_Flag[5]=htonl(r->uff_Flag[5]);
	r->uff_Flag[6]=htonl(r->uff_Flag[6]);
	r->uff_Flag[7]=htonl(r->uff_Flag[7]);
	r->uff_Flag[8]=htonl(r->uff_Flag[8]);
	r->uff_Flag[9]=htonl(r->uff_Flag[9]);
#endif
}

int WriteRecord(FILE *f, UFF *r)
{
	SwapUFFToNeutral(r);
	if(fwrite((char *)r,sizeof(UFF),1,f)!=1)
	{
		Log("UAF Write Failed: File May Be Corrupt!!!");
		return(-1);
	}
	fflush(f);
	return(0);
}

int ReadRecord(FILE *f, UFF *r)
{
	int err=fread((char *)r,sizeof(UFF),1,f);
	if(err>0)
		SwapUFFToHost(r);
	return(err);
}

int FindRecord(FILE *f, int n)
{
	if(n==-1)
	{
		fseek(f,0,2);	/* EOF - New Record */
	}
	else
	{
		if(fseek(f,n*sizeof(UFF),0)==-1)
		{
			fprintf(stderr,"FSEEK: UAF SEEK RANGE ERROR\n");
			return(-1);
		}
	}
	return(0);
}

FILE *OpenUAF(void)
{
	extern int errno;
	FILE *f;
	if((f=fopen(USERFILE,"r+"))==NULL)
	{
		Log("UAF Access Failed :Error %d",errno);
		Error("UAF access failed: Cannot continue");
	}
	return(f);
}

void CloseUAF(FILE *f)
{
	fclose(f);
}

int LoadPersona(char *name, UFF *r)
{
	FILE *a=OpenUAF();
	int ct=0;
	while(ReadRecord(a,r)==1)
	{
		if(stricmp(name,r->uff_Name)==0)
		{
			CloseUAF(a);
			return(ct);
		}
		ct++;	/* Count records */
	}
	CloseUAF(a);
	return(-1);
}

int SavePersona(UFF *r, int n)
{
	FILE *a=OpenUAF();
	if(FindRecord(a,n)==-1)
	{
		CloseUAF(a);
		Log("SavePersona: Record %d Does Not Exist",n);
		Error("SavePersona: Could not find record");
	}	
	if(WriteRecord(a,r)==-1)
	{
		CloseUAF(a);
		Log("User Write Failed");
		return(-1);
	}
	CloseUAF(a);
	return(0);
}

int FindFreeRecord(void)
{
	UFF dummy;
	return(LoadPersona(" ",&dummy));
}
	
int SaveNewPersona(UFF *r)
{
	return(SavePersona(r,FindFreeRecord()));
}
