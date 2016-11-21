#include "System.h"
#include "User.h"
#ifdef UNIX
#include <unistd.h>
#endif

extern USER UserList[];

Module	"New Commands";
Author	"Alan Cox";
Version "1.01";

/*
 *	Implement the extra commands added to the dbase language for
 *	5.12.
 *
 *	1.00		AGC		Created
 *	1.01		AGC		FORKDUMP
 *	1.02		AGC		PROCDAEMON PROCSUBJECT PROCOBJECT
 */

void Act_SetI()
{
	int n=ArgNum();
	ITEM *i=ArgItem();
	if(n==1)
		Item1=i;
	else
		Item2=i;
}

void Act_CDaemon()
{
	ITEM *t=ArgItem();
	int v=ArgWord();
	int n=ArgWord();
	CDaemon(t,v,n,ArgWord());
}

int Cnd_Delete()
{
	char *x=TextOf(ArgText());
#ifndef SECURE
	if(unlink(x)==-1)
		return(0);
	return(1);
#else
	return(0);
#endif
}

static char *String_Fetch(f)
FILE *f;
{
	int len;
	char *t;
	if(fscanf(f,"%d:",&len)!=1)
		return(NULL);
	if(len<0)
		return(NULL);
	t=malloc(len+1);
	if(t==NULL)
		return(NULL);
	if(fread(t,len+1,1,f)!=1)
	{
		free(t);
		return(NULL);
	}
	return(t);
}

int Cnd_ULoad()
{
#ifndef SECURE
	int ct=0;
	char *t=TextOf(ArgText());
	char *s[4];
	ITEM *i=ArgItem();
	FILE *f;
	USERFLAG a;
	f=fopen(t,"r");
	if(f==NULL)
		return(0);
	if(fread((char *)&a,sizeof(USERFLAG),1,f)!=1)
	{
		fclose(f);
		return(0);
	}
	while(ct<4)
	{
		s[ct]=String_Fetch(f);
		if(s[ct]==NULL)
		{
			fclose(f);
			while(--ct>0)
				free(s[ct]);
			return(0);
		}
		ct++;
	}
	fclose(f);
/*
 *	Now set the values up witgh the real UserFlag2.
 */
	ct=0;
	while(ct<8)
	{
		SetUserFlag(i,ct+8,a.uf_Flags[ct]);
		ct++;
	}
/*
 *	Now set up the strings
 */
	ct=0;
	while(ct<4)
	{
		TPTR t;
		t=AllocText(s[ct]);
		SetUText(i,ct+4,t);
		FreeText(t);
		free(s[ct]);
		ct++;
	}
	return(1);
#else
	return(0);
#endif
}

int Cnd_USave()
{
#ifndef SECURE
	char *t=TextOf(ArgText());
	int ct=0;
	ITEM *i=ArgItem();
	USERFLAG *u;
	FILE *f=fopen(t,"w");
	if(f==NULL)
		return(0);
	u=UserFlag2Of(i);
	if(u==NULL)
	{
		fclose(f);
		return(0);
	}
	if(fwrite((char *)u,sizeof(USERFLAG),1,f)!=1)
	{
		fclose(f);
		return(0);
	}
	while(ct<4)
	{
		char *tmp=TextOf(GetUText(i,ct+4));
		fprintf(f,"%d:",strlen(tmp));
		if(fwrite((char *)tmp,strlen(tmp)+1,1,f)!=1)
		{
			fclose(f);
			return(0);
		}
		ct++;
	}
	fclose(f);
	return(1);
#else
	return(0);
#endif
}

int Cnd_FLoad()
{
#ifndef SECURE
	char *t=TextOf(ArgText());
	int top=ArgNum(),bottom=ArgNum();
	int *buffer;
	int ct=0;
	FILE *f=fopen(t,"r");
	if(top<0||top>bottom||bottom>511||f==NULL)
	{
		if(f!=NULL)
			 fclose(f);
		return(0);
	}
	buffer=(int *)malloc(sizeof(int)*(bottom-top+1));
	if(buffer==NULL)
	{
		fclose(f);
		return(0);
	}
	if(fread((char *)buffer,sizeof(int)*(bottom-top+1),1,f)!=1)
	{
		fclose(f);
		free((char *)buffer);
		return(0);
	}
	while(top<=bottom)
	{
		SetFlag(top,buffer[ct]);
		ct++;
		top++;
	}
	free((char *)buffer);
	fclose(f);
	return(1);
#else
	return(0);
#endif
}

int Cnd_FSave()
{
#ifndef SECURE
	char *t=TextOf(ArgText());
	int *bp;
	int top=ArgNum(),bottom=ArgNum();
	FILE *f=fopen(t,"w");
	int *buffer;
	if(f==NULL)
	{
		return(0);
	}
	if(top<bottom||top<0||bottom>511)
	{
		fclose(f);
		return(0);
	}
	buffer=(int *)malloc(sizeof(int)*(bottom-top+1));
	if(buffer==NULL)
	{
		fclose(f);
		return(0);
	}
	bp=buffer;
	while(top<=bottom)
	{
		*bp=GetFlag(top);
		top++;
	}
	if(fwrite((char *)buffer,sizeof(int)*(bottom-top+1),1,f)!=1)
	{
		free(buffer);
		fclose(f);
		return(0);
	}
	fclose(f);
	free(buffer);
	return(1);
#else
	return(0);
#endif
}

void Act_Getvis()
{
	ITEM *i=ArgItem();
	int n=ArgNum();
	SetFlag(n,i->it_Perception);
}

void Act_ForkDump()
{
	char *x=TextOf(ArgText());
	int ct;
	int pid=fork();
	if(pid==-1)
	{
		Log("Dump failed - bad fork");
		return;
	}
	if(pid!=0)
		return;
/*
 *	Child now does some work
 */
	ct=0;
	while(ct<MAXUSER)
	{
		if(UserList[ct].us_Item!=NULL)
			RemoveUser(ct);
		ct++;
	}
	DisintegrateAll();
	DisintegrateAll();
	SaveSystem(x);
	exit(0);
}

void Act_SetExit()
{
	ITEM *from=ArgItem();
	int dn=ArgNum();
	ITEM *to=ArgItem();
	GENEXIT *g=(GENEXIT *)FindSub(from,KEY_GENEXIT);
	if(g==NULL)
	{
		MakeGenExit(from);
		g=(GENEXIT *)FindSub(from,KEY_GENEXIT);
	}
	if(dn<0||dn>11)
	{
		Log("Bad Setexit");
	}
	LockItem(to);
	if(g->ge_Dest[dn]!=NULL)
		UnlockItem(g->ge_Dest[dn]);
	g->ge_Dest[dn]=to;
}

void Act_DelExit()
{
	ITEM *from=ArgItem();
	int dn=ArgNum();
	GENEXIT *g=(GENEXIT *)FindSub(from,KEY_GENEXIT);
	if(dn<0||dn>11)
	{
		Log("Invalid DelExit");
		return;
	}
	if(g==NULL)
		return;
	if(g->ge_Dest[dn]!=NULL)
	{
		UnlockItem(g->ge_Dest[dn]);
		g->ge_Dest[dn]=NULL;
	}
}


int Cnd_ProcDaemon()
{
	ITEM *i=ArgItem();
	int ct=0;
	while(i!=NULL && ct<20)
	{
		if(i->it_DaemonTable)
		{
			if(ExecTable(i->it_DaemonTable)==1)
				return(1);
		}
		i=i->it_Superclass;
		ct++;
	}
	return 0;
}
	
int Cnd_ProcSubject()
{
	ITEM *i=ArgItem();
	int ct=0;
	while(i!=NULL && ct<20)
	{
		if(i->it_SubjectTable)
		{
			if(ExecTable(i->it_SubjectTable)==1)
				return(1);
		}
		i=i->it_Superclass;
		ct++;
	}
	return 0;
}
	
int Cnd_ProcObject()
{
	ITEM *i=ArgItem();
	int ct=0;
	while(i!=NULL && ct<20)
	{
		if(i->it_ObjectTable)
		{
			if(ExecTable(i->it_ObjectTable)==1)
				return(1);
		}
		i=i->it_Superclass;
		ct++;
	}
	return 0;
}
	

int Cnd_GetSuper()
{
	ITEM *i=ArgItem();
	int icode=ArgNum();
	if(i->it_Superclass==NULL)
		return 0;
	if(icode==1)
		Item1=i->it_Superclass;
	if(icode==2)
		Item2=i->it_Superclass;
	return 1;
}

void Act_SetSuper()
{
	ITEM *it=ArgItem();
	ITEM *i2=ArgItem();

	if(it->it_Superclass!=NULL)
		UnlockItem(it->it_Superclass);
	if(it==i2)
		it->it_Superclass=NULL;
	else
	{
		LockItem(i2);
		it->it_Superclass=i2;
	}
}

int Cnd_Member()
{
	ITEM *it=ArgItem();
	ITEM *cl=ArgItem();
	int ct=0;
	while(it->it_Superclass!=NULL && ct<20)
	{
		if(it->it_Superclass==cl)
			return(1);
		it=it->it_Superclass;
		ct++;
	}
	return(0);
}

