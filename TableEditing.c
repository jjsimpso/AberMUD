 /****************************************************************************\
 *									      *
 *			C R E A T O R    O F   L E G E N D S		      *
 *									      *
 *				(AberMud Version 5)			      *
 *									      *
 *  The Creator Of Legends System is (C) Copyright 1989 Alan Cox, All Rights  *
 *  Reserved.		  						      *
 *									      *
 \****************************************************************************/

#include "System.h"
#include "User.h"

extern USER UserList[];

Module  "Table Editing";
Version "1.03";
Author  "Alan Cox";

/*
 *	1.01	AGC	Initial Code.
 *	1.02	AGC	Tweaks for 5.08 to support item bound tables.
 *	1.03	AGC	Daemon table support.
 */


extern void Tbl_Edit();

char *EditingBuffers[MAXUSER];

void Cmd_EditTable(i)
ITEM *i;
{
	int u,n;
	short ct=0;
	TABLE *t;
	if(!ArchWizard(i))
	{
		SendItem(i,"Carpentry ?\n");
		return;
	}
	u=UserOf(i);
	if(u==-1)
		return;
	if(GetParsedWord()==NULL)
	{
		SendItem(i,"Yes but which table.\n");
		return;
	}
	if(isdigit(*WordBuffer))
		sscanf(WordBuffer,"%d",&n);
	else
	{
		n=FindTableByName(WordBuffer);
		if(n==-1)
		{
			SendItem(i,"Unknown table.\n");
			return;
		}
	}
	t=FindTable(n);
	if(t==NULL)
	{
		SendItem(i,"No such table.\n");
		return;
	}
	ct=0;
	while(ct<MAXUSER)
	{
		if(((UserList[ct].us_State==AWAIT_TEDIT)||
		   (UserList[ct].us_State==AWAIT_EDLIN))&&
			UserList[ct].us_UserPtr==(char *)t)
		{
			SendItem(i,"Sorry - This table is being edited.\n");
			SendUser(ct,"%s tried edit this table.\n",
				CNameOf(i));
			return;
		}
		ct++;
	}
	if(EditingBuffers[u]==NULL)
		EditingBuffers[u]=malloc(512);
	if(EditingBuffers[u]==NULL)
	{
		SendItem(i,"Sorry - no memory.\n");
		return;
	}
	UserList[u].us_State=AWAIT_TEDIT;
	UserList[u].us_UserInfo=0;	/* Start Line */
	UserList[u].us_UserPtr=(char *)t;
	SetPrompt(UserList[u].us_Item,":");
}

void Cmd_EditOTable(i)
ITEM *i;
{
	int u;
	TABLE *t;
	ITEM *j;
	if(!ArchWizard(i))
	{
		SendItem(i,"Carpentry ?\n");
		return;
	}
	u=UserOf(i);
	if(u==-1)
		return;
	j=FindSomething(i,O_PARENT(i));
	if(j==NULL)
	{
		SendItem(i,"What is that ?\n");
		return;
	}
	t=j->it_ObjectTable;
	if(t==NULL)
	{
		t=Allocate(TABLE);
		j->it_ObjectTable=t;
		t->tb_Number= -1;
		t->tb_Next=NULL;
		t->tb_First=NULL;
		t->tb_Name=AllocText("");
	}
	if(EditingBuffers[u]==NULL)
		EditingBuffers[u]=malloc(512);
	if(EditingBuffers[u]==NULL)
	{
		SendItem(i,"Sorry - no memory.\n");
		return;
	}
	UserList[u].us_State=AWAIT_TEDIT;
	UserList[u].us_UserInfo=0;	/* Start Line */
	UserList[u].us_UserPtr=(char *)t;
	SetPrompt(UserList[u].us_Item,":");
}

void Cmd_EditSTable(i)
ITEM *i;
{
	int u;
	TABLE *t;
	ITEM *j;
	if(!ArchWizard(i))
	{
		SendItem(i,"Carpentry ?\n");
		return;
	}
	u=UserOf(i);
	if(u==-1)
		return;
	j=FindSomething(i,O_PARENT(i));
	if(j==NULL)
	{
		SendItem(i,"What is that ?\n");
		return;
	}
	t=j->it_SubjectTable;
	if(t==NULL)
	{
		t=Allocate(TABLE);
		j->it_SubjectTable=t;
		t->tb_Number= -1;
		t->tb_Next=NULL;
		t->tb_First=NULL;
		t->tb_Name=AllocText("");
	}
	if(EditingBuffers[u]==NULL)
		EditingBuffers[u]=malloc(512);
	if(EditingBuffers[u]==NULL)
	{
		SendItem(i,"Sorry - no memory.\n");
		return;
	}	
	UserList[u].us_State=AWAIT_TEDIT;
	UserList[u].us_UserInfo=0;	/* Start Line */
	UserList[u].us_UserPtr=(char *)t;
	SetPrompt(UserList[u].us_Item,":");
}

void Cmd_EditDTable(i)
ITEM *i;
{
	int u;
	TABLE *t;
	ITEM *j;
	if(!ArchWizard(i))
	{
		SendItem(i,"Carpentry ?\n");
		return;
	}
	u=UserOf(i);
	if(u==-1)
		return;
	j=FindSomething(i,O_PARENT(i));
	if(j==NULL)
	{
		SendItem(i,"What is that ?\n");
		return;
	}
	t=j->it_DaemonTable;
	if(t==NULL)
	{
		t=Allocate(TABLE);
		j->it_DaemonTable=t;
		t->tb_Number= -1;
		t->tb_Next=NULL;
		t->tb_First=NULL;
		t->tb_Name=AllocText("");
	}
	if(EditingBuffers[u]==NULL)
		EditingBuffers[u]=malloc(512);
	if(EditingBuffers[u]==NULL)
	{
		SendItem(i,"Sorry - no memory.\n");
		return;
	}	
	UserList[u].us_State=AWAIT_TEDIT;
	UserList[u].us_UserInfo=0;	/* Start Line */
	UserList[u].us_UserPtr=(char *)t;
	SetPrompt(UserList[u].us_Item,":");
}

/*
 *	Table State Drivers
 *
 *
 * Editing Commands
 *
 * G<n> goto line n
 * Q    quit
 * L<n>{,<m>} List range
 * E<n>{,<m>} edit line
 * D<n>{,<m>} delete lines
 * I	      insert a line
 * N	      next line
 * P	      previous line
 * B	      bottom
 * T	      top
 * F <v> {n} {n} [l] [l]
 * 
 *
 * RETURN does N
 *
 */


void Tbl_Quit(u,x)
int u;
char *x;
{
	UserList[u].us_State=AWAIT_COMMAND;
	free(EditingBuffers[u]);
	EditingBuffers[u]=NULL;
	SetPrompt(UserList[u].us_Item,"-}--- ");
}

void Tbl_Goto(u,x)
int u;
char *x;	/* Arg String */
{
	int line=UserList[u].us_UserInfo;
	LINE *l;
	if(*x)
	{
		if(sscanf(x,"%d",&line)==0)
		{
			SendUser(u,"ERR> Invalid Line Number\n");
			return;
		}
	}
	if(line>=0)
		l=FindLine((TABLE *)UserList[u].us_UserPtr,line);
	else
		l=NULL;
	if(l==NULL)
	{
		SendUser(u,"ERR> No Such Line\n");
		return;
	}
	Decompress(l,EditingBuffers[u]);
/* Show New Line - Note G alone shows line */
	SendUser(u,"%d %s\n",line,EditingBuffers[u]);
	UserList[u].us_UserInfo=line;
}

void Tbl_Find(u,arg)
int u;
char *arg;
{
	short v,n1,n2;
	short nps;
	short ct=0;
	char vs[32],n1s[32],n2s[32];
	int tl= -1,bl= 99999;
	int cl=0;
	WLIST *w;
	LINE *l;
	strcpy(vs,"ANY");
	if((nps=sscanf(arg,"%29s %29s %29s %d %d",vs,n1s,n2s,&tl,&bl))<1)
	{
		SendUser(u,"Unknown wordmatch.\n");
		return;
	}
	if(nps<2) strcpy(n1s,"ANY");
	if(nps<3)strcpy(n2s,"ANY");
/*	printf("%s:%s:%s\n",vs,n1s,n2s);	*/
	if(stricmp(vs,"ANY")==0)
		v=-1;
	else
	{
		w=FindInList(WordList,vs,WD_VERB);
		if(w)
			v=w->wd_Code;
		else
		{
			SendUser(u,"Unknown verb: %s.\n",vs);
			return;
		}
	}
	if(stricmp(n1s,"ANY")==0)
		n1=-1;
	else
	{
		w=FindInList(WordList,n1s,WD_NOUN);
		if(w)
			n1=w->wd_Code;
		else
		{
			SendUser(u,"Unknown noun: %s.\n",n1s);
			return;
		}
	}
	if(stricmp(n2s,"ANY")==0)
		n2=-1;
	else
	{
		w=FindInList(WordList,n2s,WD_NOUN);
		if(w)
			n2=w->wd_Code;
		else
		{
			SendUser(u,"Unknown noun: %s.\n",n2s);
			return;
		}
	}
	Verb=v;
	Noun1=n1;
	Noun2=n2;
	l=((TABLE *)UserList[u].us_UserPtr)->tb_First;
	while(l)
	{
		if(ArgMatch(l)&&cl<=bl&&cl>=tl)
		{
			Decompress(l,EditingBuffers[u]);
			SendUser(u,"%d %s\n",ct,EditingBuffers[u]);
		}
		l=l->li_Next;
		cl++;
		ct++;
	}
}

void Tbl_Top(u,arg)
int u;
char *arg;
{
	UserList[u].us_UserInfo=0;
	Tbl_Goto(u,"");
}

long CountEntries(x)
TABLE *x;
{
	long ct=0;
	LINE *l=x->tb_First;
	while(l)
	{
		ct++;
		l=l->li_Next;
	}
	return(ct);
}

void Tbl_Bottom(u,arg)
int u;
char *arg;
{
	UserList[u].us_UserInfo=CountEntries((TABLE *)UserList[u].us_UserPtr)-1;
	Tbl_Goto(u,"");
}

void Tbl_Next(u,arg)
int u;
char *arg;
{
	if(UserList[u].us_UserInfo==
			CountEntries((TABLE *)UserList[u].us_UserPtr)-1)
		SendUser(u,"BOTTOM\n");
	else
		UserList[u].us_UserInfo++;
	Tbl_Goto(u,"");
}

void Tbl_Previous(u,arg)
int u;
char *arg;
{
	if(UserList[u].us_UserInfo)
		UserList[u].us_UserInfo--;
	else
		SendUser(u,"TOP\n");
	Tbl_Goto(u,"");
}

void Tbl_List(u,arg)
int u;
char *arg;
{
	int line,lend,n;
	LINE *l;
	n=sscanf(arg,"%d,%d",&line,&lend);
	if(n==0)
		line=UserList[u].us_UserInfo;
	if(n==1)
		lend=line;
	if(lend<line)
		return;
	if(lend-line>39)
	{
		SendUser(u,"ERR> Too may lines\n");
		return;
	}
	while(line<=lend)
	{
		if(line>=0)
			l=FindLine((TABLE *)UserList[u].us_UserPtr,line);
		else
			l=NULL;
		if(l==NULL)
		{
			SendUser(u,"ERR> No Such Line\n");
			return;
		}
		Decompress(l,EditingBuffers[u]);
/* Show New Line - Note G alone shows line */
		SendUser(u,"%d %s\n",line,EditingBuffers[u]);
		line++;
	}
}

/* Insert Edit Delete */

void Tbl_Insert(u,arg)
int u;
char *arg;
{
	LINE *n;
	int line=UserList[u].us_UserInfo;
	short ia=0;
	if((*arg=='B')||(*arg=='b'))
	{
		ia=1;
		arg++;
	}
	if(sscanf(arg,"%d",&line)!=1)
		line=UserList[u].us_UserInfo;
	n=NewLine((TABLE *)UserList[u].us_UserPtr,line+ia);
	n->li_Verb=-1;
	n->li_Noun1=-1;
	n->li_Noun2=-1;
	UserList[u].us_UserInfo=line+ia;
	Tbl_Edit(u,"");
}

void Tbl_Edit(u,arg)
int u;
char *arg;
{
	int line;
	LINE *l;
	line=UserList[u].us_UserInfo;
	if(sscanf(arg,"%d",&line)!=1)
		line=UserList[u].us_UserInfo;
	l=FindLine((TABLE *)UserList[u].us_UserPtr,line);
	if(l==NULL)
	{
		SendUser(u,"No Such Line\n");
		return;
	}
	UserList[u].us_UserInfo=line;
	UserList[u].us_State=AWAIT_EDLIN;
	Decompress(l,EditingBuffers[u]);
	SendEdit(UserList[u].us_Item,"%s",EditingBuffers[u]);
	SetPrompt(UserList[u].us_Item,">");
}	

void Tbl_EditLine(u,line)
int u;
char *line;
{
	char x[24];
	LINE *l=FindLine((TABLE *)
			UserList[u].us_UserPtr,UserList[u].us_UserInfo);
	if(strlen(line)==0)
	{
		SendUser(u,"Edit Abandoned\n");
		goto ned;
	}
	if(l==NULL)
	{
		SendUser(u,"Umm, someone else has fiddled with the table!\n");
	}
	else
	{
		Decompress(l,EditingBuffers[u]);
		WipeLine(l);
		free((char *)l->li_Data);
		l->li_Data=Allocate(unsigned short);
		l->li_Data[0]=CMD_EOL;
		LoadLineBuffer(line);
		if(EncodeEntry(UserList[u].us_Item,l)==-1)
		{	/* Restore Old Entry */
			LoadLineBuffer(EditingBuffers[u]);
			EncodeEntry(UserList[u].us_Item,l);
			goto ked;
		}
	}
ned:	UserList[u].us_State=AWAIT_TEDIT;
	sprintf(x,"%d:",UserList[u].us_UserInfo);
	SetPrompt(UserList[u].us_Item,x);
ked:	PermitInput(u);
}
	
void Tbl_DeleteLine(u,arg)
int u;
char *arg;
{
	int line;
	int db=0;
	line=UserList[u].us_UserInfo;
	if(*arg=='B')
	{
		db=1;
		arg++;
	}
	if(sscanf(arg,"%d",&line)==0)
		line=UserList[u].us_UserInfo+db;
	if(WipeDeleteLine((TABLE *)UserList[u].us_UserPtr,line+db)==-1)
		SendUser(u,"No Such Line\n");
	else
	{
		UserList[u].us_UserInfo=line;
		Tbl_Goto(u,"");
	}
}

void Tbl_Driver(us,command)
int us;
char *command;
{
	char x[24];
	switch(toupper(*command))
	{
		case 'D':Tbl_DeleteLine(us,command+1);break;
		case 'E':Tbl_Edit(us,command+1);
			 sprintf(x,"%d>",UserList[us].us_UserInfo);
			 goto l2;
		case 'G':Tbl_Goto(us,command+1);break;
		case 'B':Tbl_Bottom(us,command+1);break;
		case 'T':Tbl_Top(us,command+1);break;
		case 'N':Tbl_Next(us,command+1);break;
		case 'P':Tbl_Previous(us,command+1);break;
		case 'I':Tbl_Insert(us,command+1);break;
		case 'L':Tbl_List(us,command+1);break;
		case 'Q':Tbl_Quit(us,command+1);goto l1;
		case 'F':Tbl_Find(us,command+1);break;
		default:SendUser(us,"?\n");
	}
	sprintf(x,"%d:",UserList[us].us_UserInfo);
l2:	SetPrompt(UserList[us].us_Item,x);
l1:	PermitInput(us);
}

void OutLineBlock(f,s)
FILE *f;
char *s;
{
	char c;
	while(strlen(s)>=70)
	{
		c=s[71];
		s[71]=0;
		fprintf(f,"%s+\n",s);
		s+=71;
		*s=c;
	}
	fprintf(f,"%s\n",s);
}
	
void Cmd_SaveTable(i)
ITEM *i;
{
	int n=GetNumber();
	TABLE *t;
	LINE *l;
	FILE *f;
	char TableBuff[512];
	if(!ArchWizard(i))
	{
		SendItem(i,"I don't understand, are tables an endangered species then ?\n");
		return;
	}
	if(n==-1)
	{
		SendItem(i,"Which table though ?\n");
		return;
	}
	t=FindTable(n);
	if(t==NULL)
	{
		SendItem(i,"That table doesn't exist");
		return;
	}
	GetAll();
	if(!strlen(WordBuffer))
	{
		SendItem(i,"Where shall I save to though ?\n");
		return;
	}
	f=fopen(WordBuffer,"w");
	if(f==NULL)
	{
		SendItem(i,"Can't create file '%s'.\n",WordBuffer);
		return;
	}
	l=t->tb_First;
	while(l)
	{
		Decompress(l,TableBuff);
		OutLineBlock(f,TableBuff);
		l=l->li_Next;
	}
	fclose(f);
	SendItem(i,"Table Copy Dumped To File\n");
}

