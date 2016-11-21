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
#include "User.h"

Module "Command Driver";
Version "1.16";
Author "----*(A)";

extern USER UserList[];
extern int Current_User;
extern FILE *Log_File;
extern char *RBitNames[],*OBitNames[],*PBitNames[];

/*
 *	1.00	AGC	Created this file
 *	1.03	AGC	Added miscellaneous command defns.
 *	1.04	AGC	Added horrible nasty AGOSII no password hack
 *	1.05	AGC	Added save/load userflag feature
 *	1.06    AGC     Fixed ! for no A/Wiz
 *	1.07	AGC	ANSI prototyping changes for 5.06
 *	1.08	AGC	Added command changes for 5.07
 *	1.09	AGC	Added command changes for 5.08
 *	1.10	AGC	SendUser queues text output for efficiency of comms
 *	1.11	AGC	No SendUser buffering now.. better IPC instead
 *	1.12	AGC	Backouts added to state machine for logins
 *	1.13	AGC	Added missing argument to fix registration bug
 *	1.14	AGC	'!' ovverride is now ':' override as '!' these days is repeat
 *	1.15	AGC	Whoops.. stop deleting nouns on failed logins!!!
 *	1.16	AGC	Strict ANSIfication
 */

/*
 *	Handle a line of input sent by a PLAY program, and decide what to
 *	do with it, by checking the status of the player in the User list
 *
 */

static UFF LoginUFF;	/* Login Routines Use This Buffer */

extern short LineFault[];

void Command_Driver(int us, short state, char *command)
{
	switch(state)
	{
		case AWAIT_NAME:	/* Reply to Name: */

				Name_Got(us,command);
				break;

		case AWAIT_PASSWORD:	/* Reply to Password: */

				Check_Password(us,command,1);
				break;

		case AWAIT_PASSRETRY:	/* Reply to 2nd Password: */

				Check_Password(us,command,0);
				break;

		case AWAIT_COMMAND:	/* Normal command */

				Run_Command(us,command);
				break;

		case AWAIT_EMAIL:
					/* Set email for */
				SetPlayerEmail(us,command);
				break;
				
		case AWAIT_PWSET:	/* Set a password for.... */

				LoginUFF.uff_Flag[9]=0;
				CreatePersona(us,command);
				break;

		case AWAIT_SETSEX:	/* What sex shall I make you */

				SetPlayerSex(us,command);
				break;
		case AWAIT_TEDIT:	/* Editing for tables */

				Tbl_Driver(us,command);
				break;

		case AWAIT_EDLIN:	/* Line editing in table */

				Tbl_EditLine(us,command);
				break;
		case AWAIT_PWVERIFY:	/* Password: on change pw */

				PWVerify((short)us,command);
				break;

		case AWAIT_PWNEW:	/* New password : */

				PWNew((short)us,command);
				break;

		case AWAIT_PWVERNEW:	/* And again to verify */

				PWNewVerify((short)us,command);
				break;
		
		case AWAIT_OE1:		/* Using EditObject/BeObject */

				Objedit_1((short)us,command);
				break;

		case AWAIT_OE2:

				Objedit_2((short)us,command);
				break;

		case AWAIT_OE3:

				Objedit_3((short)us,command);
				break;

		case AWAIT_OE4:

				Objedit_4((short)us,command);
				break;

		case AWAIT_OE5:

				Objedit_5((short)us,command);
				break;

		case AWAIT_OE6:

				Objedit_6((short)us,command);
				break;

		case AWAIT_OE7:

				Objedit_7((short)us,command);
				break;

		case AWAIT_OE8:
				Objedit_8((short)us,command);
				break;
		default:

				Log("User in invalid state(%d)",us);
				break;

	}
}


/*
 *	Send a message to a PLAY program permitting it to resume input
 */

void PermitInput(int u)
{
	if(UserList[u].us_Port==NULL)
		return;	/* User gone */
	SendUser(-2, "");	/* Flush outputs */
	SendNPacket(UserList[u].us_Port,PACKET_INPUT,1,0,0,0);
}

char *UserLastLine;	/* Pointer supplied for snoop */


/*
 *	Send a message to a user, allowing full printf formatting
 */

void BufOut(int u, char *y)
{
	char v;
	if(UserList[u].us_Port==NULL)
		return;
	while(strlen(y)>510)	/* Send in 512 byte blocks */
	{
		v=y[510];
		y[510]=0;
		SendTPacket(UserList[u].us_Port,PACKET_OUTPUT,y);
		y[510]=v;
		y+=510;
	}
	if(SendTPacket(UserList[u].us_Port,PACKET_OUTPUT,y)==-20)
		LineFault[u]=1;
}

void SendUser(int u, char *fmt, ...)
{
	static char x[4096];
	/* char v; */
	va_list va;
	
	UserLastLine=x;
	if(u==-1)
		return;
	if(u==-2)
		return;
		
	va_start(va, fmt);
	vsnprintf(x, 4096, fmt, va);
	va_end(va);
	BufOut(u,x);
}



/*
 *	The player has given a name - decide if it is new or old, and then
 *	set the player up so his/her next input goes to the right place
 *	The AGOSII version of this is hacked about to avoid asking for all
 *	of this un-needed Multi-User type stuff
 */

int Name_Got(int u, char *name)
{
	int ct=0;
	if(strlen(name)>MAXNAME)	/* Name too long */
	{
		SendUser(u,"Sorry that name is too long, try another.\n");
		PermitInput(u);
		return(0);
	}
	if(!strlen(name))		/* No Name - kick off */
	{
		SendTPacket(UserList[u].us_Port,PACKET_CLEAR,
				"");
		*UserList[u].us_Name=0;
		RemoveUser(u);
		return(0);
	}
	while(name[ct])			/* Check for letters only */
	{
		if(isupper(name[ct]))
			name[ct]=tolower(name[ct]);
		if((name[ct]<'a')||(name[ct]>'z'))
		{
			SendUser(u,
		"Sorry your choice of name must consist of letters only.\n");
			PermitInput(u);
			return(0);
		}
		ct++;
	}
/*
 *	We put the name to lowercase with a capital first letter.
 */
	name[0]=toupper(name[0]);
	strcpy(UserList[u].us_Name,name);
	UserList[u].us_State=AWAIT_PASSWORD;
	if(LoadPersona(name,&LoginUFF)==-1)	/* New player ? */
	{
#ifdef AGOS
/*
 *	Nasty hack job number 1
 */
		UserList[u].us_Record=PL_MALE;
		CreatePersona(u,"");
		PermitInput(u);
		return(0);
#else
/*
 *	Wait for new player to decide a sex
 */
#define REGISTER
#ifdef REGISTER
		SendUser(u,"Not registered.\n");
		SendUser(u,"By registering for this game you become a guest member of the computer\n");
		SendUser(u,"society, and the data given will be held for the administration of the\n");
		SendUser(u,"the society, and its security.\n\n");
#endif
		SendTPacket(UserList[u].us_Port,PACKET_SETPROMPT,
			"What sex (M/F) shall I make you : ");
		UserList[u].us_State=AWAIT_SETSEX;
		PermitInput(u);
		return(0);
#endif
	}
#ifdef AGOS
/*
 *	The management wish to apologise in advance for this piece of code
 *	however it is short, and it seems to work...
 */
	Check_Password(u,LoginUFF.uff_Password,0);
	PermitInput(u);
	return(0);	/* Shudder */
#else
	SendTPacket(UserList[u].us_Port,PACKET_SETPROMPT,"Password: ");
	SendNPacket(UserList[u].us_Port,PACKET_ECHO,1,0,0,0); /* NOECHO */
	PermitInput(u);
	return(0);
#endif
}

/*
 *	The player has typed a password to an old persona. In the case of AGOSII
 *	it only seems that way - the password was typed by the game too
 *	see the routine above.
 */

int Check_Password(int u, char *pwentry, int retflg)
{
	TABLE *t;
	ITEM *place;
	PLAYER *p;
	int r;
/*
 *	Load The Persona Entry For Real
 */
	if((r=LoadPersona(UserList[u].us_Name,&LoginUFF))==-1)
	{
/*
 *	Something odd has happened, or the PURGE utility has been used
 */
		SendUser(u,"Sorry, your persona has just disappeared...\n");
		SendNPacket(UserList[u].us_Port,PACKET_ECHO,0,0,0,0);
		SendTPacket(UserList[u].us_Port,PACKET_SETPROMPT,"Name :");
		UserList[u].us_State=AWAIT_NAME;
		PermitInput(u);
		return(0);
	}
/*
 *	Check the password is correct (on AGOSII the fake password always is)
 */
	if(strncmp(pwentry,LoginUFF.uff_Password,7))
	{
		if(UserList[u].us_State==AWAIT_PASSWORD)
			UserList[u].us_State=AWAIT_PASSRETRY;
		else
		{
/*
 *	Too many tries - go back to asking for name.
 */
			UserList[u].us_State=AWAIT_NAME;
			SendTPacket(UserList[u].us_Port,PACKET_SETPROMPT,
				"By what name shall I call you: ");
			SendNPacket(UserList[u].us_Port,PACKET_ECHO,0,0,0,0); 
				/* ECHO */
		}
		PermitInput(u);
		return(0);
	}					
/*
 *	Having got the correct password we check if it is an allowable name,
 *	and not already playing etc. This is done AFTER so that you cannot
 *	log in as someone to see if they are playing but invisible like you
 *	can in MUD/MIST etc.
 */
	if(FindInList(WordList,UserList[u].us_Name,WD_NOUN))
	{	
		SendUser(u,
		"Sorry that name could be confused with other things.\n");
		SendTPacket(UserList[u].us_Port,PACKET_SETPROMPT,
				"By what name shall I call you: ");
		UserList[u].us_State=AWAIT_NAME;
		SendNPacket(UserList[u].us_Port,PACKET_ECHO,0,0,0,0); /* ECHO */
		PermitInput(u);
		return(0);
	}
	if(FindInList(WordList,UserList[u].us_Name,WD_NOISE)||
		FindInList(WordList,UserList[u].us_Name,WD_PREP)||
		FindInList(WordList,UserList[u].us_Name,WD_ORDIN))
	{	
		SendUser(u,
		"Sorry that name could be confused with other things.\n");
		SendTPacket(UserList[u].us_Port,PACKET_SETPROMPT,
				"By what name shall I call you: ");
		UserList[u].us_State=AWAIT_NAME;
		SendNPacket(UserList[u].us_Port,PACKET_ECHO,0,0,0,0); /* ECHO */
		PermitInput(u);
		return(0);
	}
	if(LoginUFF.uff_Score==-1)
	{
		UserList[u].us_Record=LoginUFF.uff_Flags;
		return CreatePersona(u,pwentry);
	}
/*
 *	Create the player
 */
	UserList[u].us_Record=r;
	UserList[u].us_State=AWAIT_COMMAND;	/* Command mode */
	SendTPacket(UserList[u].us_Port,PACKET_SETPROMPT,"-}--- ");
/*
 *	Quick ego trip....
 */
	if(stricmp(UserList[u].us_Name,"Anarchy")==0)
		SendTPacket(UserList[u].us_Port,PACKET_SETPROMPT,"----*");
/*	SendUser(u,"Welcome to AberMUD V %s\n",UserList[u].us_Name);	*/
	AddWord(UserList[u].us_Name,(short)(10000+u),WD_NOUN);	/* Add name word */
	UserList[u].us_Item=CreateItem(UserList[u].us_Name,-1,10000+u);
	LockItem(UserList[u].us_Item);	/* Lock for userlist entry */
	MakePlayer(UserList[u].us_Item);
	p=(PLAYER *)FindSub(UserList[u].us_Item,KEY_PLAYER);
	if(!p)
		Error("Login: Player Create Failure");
	p->pl_UserKey=u;
	place=FindMaster(10000,1,1);	/* Look for Adj#1 Noun#1 autostart */
	if(place)
	{
		LinkItem(UserList[u].us_Item,place);
	}
	strncpy(LoginUFF.uff_Password,pwentry,8);
	strncpy(UserList[u].us_Password,pwentry,8);
	p->pl_Size=LoginUFF.uff_Size;
	p->pl_Weight=LoginUFF.uff_Weight;
	p->pl_Strength=LoginUFF.uff_Strength;
	p->pl_Flags=LoginUFF.uff_Flags;
	p->pl_Level=LoginUFF.uff_Level;
	p->pl_Score=LoginUFF.uff_Score;
	UserList[u].us_Item->it_ActorTable=LoginUFF.uff_ActorTable;
	UserList[u].us_Item->it_ActionTable=LoginUFF.uff_ActionTable;
	UserList[u].us_Item->it_Perception=LoginUFF.uff_Perception;
	SetUserFlag(UserList[u].us_Item,0,LoginUFF.uff_Flag[0]);
	SetUserFlag(UserList[u].us_Item,1,LoginUFF.uff_Flag[1]);
	SetUserFlag(UserList[u].us_Item,2,LoginUFF.uff_Flag[2]);
	SetUserFlag(UserList[u].us_Item,3,LoginUFF.uff_Flag[3]);
	SetUserFlag(UserList[u].us_Item,4,LoginUFF.uff_Flag[4]);
	SetUserFlag(UserList[u].us_Item,5,LoginUFF.uff_Flag[5]);
	SetUserFlag(UserList[u].us_Item,6,LoginUFF.uff_Flag[6]);
	SetUserFlag(UserList[u].us_Item,7,LoginUFF.uff_Flag[7]);
	SetUserFlag(UserList[u].us_Item,14,LoginUFF.uff_Flag[8]);
	SetUserFlag(UserList[u].us_Item,15,LoginUFF.uff_Flag[9]);
	DoesAction(UserList[u].us_Item,4,"arrives.\n");
	SendNPacket(UserList[u].us_Port,PACKET_ECHO,0,0,0,0); /* ECHO */
	t=FindTable(102);
	if(t)
		ExecBackground(t,UserList[u].us_Item);
	PermitInput(u);
	return(0);
}

/*
 *	New player specifies a sex
 */

int SetPlayerSex(int u, char *s)
{
	if(*s==0)
	{
		SendTPacket(UserList[u].us_Port,PACKET_SETPROMPT,"By what name shall I call you: ");
		UserList[u].us_State=AWAIT_NAME;
		PermitInput(u);
		return(0);
	}
	if(isupper(*s)) *s=tolower(*s);
	if((*s!='m')&&(*s!='f'))	/* Nothing else ... */
	{
		SendUser(u,"Male or Female (Return to abandon).\n");
		PermitInput(u);
		return(0);
	}
	if(*s=='m')
		UserList[u].us_Record=PL_MALE;
	else
		UserList[u].us_Record=PL_FEMALE;
	UserList[u].us_State=AWAIT_EMAIL;
	SendTPacket(UserList[u].us_Port,PACKET_SETPROMPT,
		"Email address (Required): ");
	PermitInput(u);
	return(0);
}


int SetPlayerEmail(int u, char *mail)
{
	FILE *f=fopen("register.log","a");
	if(f==NULL)
		Error("Register.log open failed.\n");
	fprintf(f,"Register of %s: EMail is %s\n",UserList[u].us_Name,mail);
	fclose(f);
	SendUser(u,"Thank you. You should be registered shortly.\n");
/*
 *	Prepare to wait for a password
 */
	UserList[u].us_State=AWAIT_PWSET;
	SendTPacket(UserList[u].us_Port,PACKET_SETPROMPT,
			"Give me a password for this character : ");
	SendNPacket(UserList[u].us_Port,PACKET_ECHO,1,0,0,0); /* NOECHO */
	PermitInput(u);
	return(0);
}

/*
 *	Create a new player
 */

int CreatePersona(int u, char *pw)
{
	ITEM *place;
	PLAYER *p;
	TABLE *t;
	SendNPacket(UserList[u].us_Port,PACKET_ECHO,0,0,0,0);
/*
 *	Verify the legality of the players name
 */
	if(FindInList(WordList,UserList[u].us_Name,WD_NOUN))
	{	
		SendUser(u,
		"Sorry that name could be confused with other things.\n");
		UserList[u].us_State=AWAIT_NAME;
		SendTPacket(UserList[u].us_Port,PACKET_SETPROMPT,
				"By what name shall I call you: ");
		PermitInput(u);
		return(0);
	}
	if(FindInList(WordList,UserList[u].us_Name,WD_NOISE)||
		FindInList(WordList,UserList[u].us_Name,WD_PREP)||
	        FindInList(WordList,UserList[u].us_Name,WD_ORDIN))
	{	
		SendUser(u,
		"Sorry that name could be confused with other things.\n");
		SendTPacket(UserList[u].us_Port,PACKET_SETPROMPT,
				"By what name shall I call you: ");
		UserList[u].us_State=AWAIT_NAME;
		PermitInput(u);
		return(0);
	}
/*
 *	Now create the player
 */
	UserList[u].us_State=AWAIT_COMMAND;	/* Command mode */
	strncpy(UserList[u].us_Password,pw,8);
	SendTPacket(UserList[u].us_Port,PACKET_SETPROMPT,"-}--- ");
	if(stricmp(UserList[u].us_Name,"Anarchy")==0)
		SendTPacket(UserList[u].us_Port,PACKET_SETPROMPT,"----*");
/*	SendUser(u,"Welcome to AberMUD V %s\n",UserList[u].us_Name);	*/
	AddWord(UserList[u].us_Name,(short)(10000+u),WD_NOUN);	/* Add name word */
	UserList[u].us_Item=CreateItem(UserList[u].us_Name,-1,10000+u);
	LockItem(UserList[u].us_Item);	/* Lock for userlist entry */
	MakePlayer(UserList[u].us_Item);
	p=(PLAYER *)FindSub(UserList[u].us_Item,KEY_PLAYER);
	if(!p)
		Error("Login: Player Create Failure");
	p->pl_UserKey=u;
	place=FindMaster(10000,1,1);
	if(place)
	{
		LinkItem(UserList[u].us_Item,place);
/*		Cmd_Look(UserList[u].us_Item);*/
	}
	p->pl_Size=120;			/* Change these defaults if you want to */
	p->pl_Weight=120;
	p->pl_Strength=60;
	p->pl_Flags=UserList[u].us_Record;	/* We 'Borrow' this field */
	p->pl_Level=1;
	p->pl_Score=0;
	UserList[u].us_Item->it_ActorTable=0;
	UserList[u].us_Item->it_ActionTable=2;
	UserList[u].us_Item->it_Perception=0;
	UserList[u].us_Record=-1;
	SetUserFlag(UserList[u].us_Item,0,0);
	SetUserFlag(UserList[u].us_Item,1,0);
	SetUserFlag(UserList[u].us_Item,2,0);
	SetUserFlag(UserList[u].us_Item,3,0);
	SetUserFlag(UserList[u].us_Item,4,0);
	SetUserFlag(UserList[u].us_Item,5,0);
	SetUserFlag(UserList[u].us_Item,6,0);
	SetUserFlag(UserList[u].us_Item,7,0);
	SetUserFlag(UserList[u].us_Item,15,LoginUFF.uff_Flag[9]);
	DoesAction(UserList[u].us_Item,4,"arrives.\n");
	t=FindTable(101);
	if(t)
		ExecBackground(t,UserList[u].us_Item);
	PermitInput(u);
	return(0);
}

/*
 *	The user has typed in a command to be done
 */
	
int Run_Command(int u, char *cmd)
{
	ITEM *i=UserList[u].us_Item;
	int v;
	char *x;
	TABLE *t;
	WordPtr=cmd;
	if(i==NULL)
		return(0);		/* Do nothing.. person is dead */
	if(setjmp(Oops))		/* Set the error trap */
		goto panic;
	if(strlen(WordPtr)==0)		/* Nothing said */
		goto l2;
/*	printf("User %d Cmd %s\n",u,WordPtr);	*/
	/* Fake seeing the input string for snoopers */
	/* Ought to show prompts I guess but I'm lazy */
	SnoopCheckString(i,cmd);
	SnoopCheckString(i,"\n");
	if(stricmp(cmd,"INIT")==0)
	{
/*
 *	Yes you can fool this check for INIT being done, but if you do
 *	it's on your own head.
 */
		if(FindInList(WordList,"Abort",WD_VERB)==0)
		{
			AddWord("Abort",1,WD_VERB);
			AddWord("AddVerb",2,WD_VERB);
			AddWord("AddNoun",3,WD_VERB);
			AddWord("AddAdj",4,WD_VERB);
			AddWord("AddPrep",5,WD_VERB);
			AddWord("AddPronoun",6,WD_VERB);
			AddWord("AddOrdinate",7,WD_VERB);
			AddWord("II",8,WD_VERB);
			AddWord("ListItems",9,WD_VERB);
			AddWord("SetName",10,WD_VERB);
			AddWord("Set",11,WD_VERB);
			AddWord("Create",12,WD_VERB);
			AddWord("Delete",13,WD_VERB);
			AddWord("BeRoom",14,WD_VERB);
			AddWord("BeObject",15,WD_VERB);
			AddWord("BePlayer",16,WD_VERB);
			AddWord("UnRoom",17,WD_VERB);
			AddWord("UnObject",18,WD_VERB);
			AddWord("UnPlayer",19,WD_VERB);
			AddWord("SaveUniverse",20,WD_VERB);
			AddWord("StatMe",21,WD_VERB);
			AddWord("SetShort",22,WD_VERB);
			AddWord("SetLong",25,WD_VERB);
			AddWord("ShowRoom",23,WD_VERB);
			AddWord("SetRFlag",24,WD_VERB);
			AddWord("Look",26,WD_VERB);
			AddWord("Goto",27,WD_VERB);
			AddWord("Brief",28,WD_VERB);
			AddWord("Verbose",29,WD_VERB);
			AddWord("ShowObject",30,WD_VERB);
			AddWord("SetOFlag",31,WD_VERB);
			AddWord("SetDesc",32,WD_VERB);
			AddWord("Invisible",33,WD_VERB);
			AddWord("Visible",34,WD_VERB);
			AddWord("Say",35,WD_VERB);
			AddWord("Place",36,WD_VERB);
			AddWord("ListWord",37,WD_VERB);
			AddWord("DelVerb",38,WD_VERB);
			AddWord("DelNoun",39,WD_VERB);
			AddWord("DelAdj",40,WD_VERB);
			AddWord("DelPrep",41,WD_VERB);
			AddWord("DelPronoun",42,WD_VERB);
			AddWord("DelOrdinate",43,WD_VERB);
			AddWord("NewExit",44,WD_VERB);
			AddWord("DelExit",45,WD_VERB);
			AddWord("OSize",46,WD_VERB);
			AddWord("OWeight",47,WD_VERB);
			AddWord("ShowPlayer",48,WD_VERB);
			AddWord("SetPFlag",49,WD_VERB);
			AddWord("SetPSize",50,WD_VERB);
			AddWord("SetPWeight",51,WD_VERB);
			AddWord("PSize",50,WD_VERB);
			AddWord("PWeight",51,WD_VERB);
			AddWord("PStrength",52,WD_VERB);
			AddWord("SetPStrength",52,WD_VERB);
			AddWord("SetPLevel",53,WD_VERB);
			AddWord("SetPScore",54,WD_VERB);
			AddWord("PLevel",53,WD_VERB);
			AddWord("PScore",54,WD_VERB);
			AddWord("Users",55,WD_VERB);
			AddWord("ListTables",56,WD_VERB);
			AddWord("DeleteTable",57,WD_VERB);
			AddWord("LoadTable",58,WD_VERB);
			AddWord("EditTable",59,WD_VERB);
			AddWord("NewTable",60,WD_VERB);
			AddWord("Quit",61,WD_VERB);
			AddWord("Chain",62,WD_VERB);
			AddWord("UnChain",63,WD_VERB);
			AddWord("Rename",64,WD_VERB);
			AddWord("SaveTable",65,WD_VERB);
			AddWord("SetActor",66,WD_VERB);
			AddWord("SetAction",67,WD_VERB);
			AddWord("BeContainer",68,WD_VERB);
			AddWord("UnContainer",69,WD_VERB);
			AddWord("SetVolume",70,WD_VERB);
			AddWord("ShowContainer",71,WD_VERB);
			AddWord("MessageExit",72,WD_VERB);
			AddWord("SetCFlag",73,WD_VERB);
			AddWord("SetUFlag",74,WD_VERB);
			AddWord("SetUItem",75,WD_VERB);
			AddWord("ShowUser",76,WD_VERB);
			AddWord("SetPicture",77,WD_VERB);
			AddWord("Share",78,WD_VERB);
			AddWord("UnShare",79,WD_VERB);
			AddWord("Status",80,WD_VERB);
			AddWord("NameFlag",81,WD_VERB);
			AddWord("UnNameFlag",82,WD_VERB);
			AddWord("ListFlag",83,WD_VERB);
			AddWord("DoorPair",84,WD_VERB);
			AddWord("ShowFlag",85,WD_VERB);
			AddWord("SetFlag",86,WD_VERB);
			AddWord("SetPerception",87,WD_VERB);
			AddWord("ShowAllRooms",88,WD_VERB);
			AddWord("ShowAllObjects",89,WD_VERB);
			AddWord("ShowAllPlayers",95,WD_VERB);
			AddWord("EditObject",96,WD_VERB);
/*
 *	Directions
 */
			AddWord("North",100,WD_VERB);
			AddWord("East",101,WD_VERB);
			AddWord("South",102,WD_VERB);
			AddWord("West",103,WD_VERB);
			AddWord("Up",104,WD_VERB);
			AddWord("Down",105,WD_VERB);
			AddWord("NorthEast",106,WD_VERB);
			AddWord("NorthWest",107,WD_VERB);
			AddWord("SouthEast",108,WD_VERB);
			AddWord("SouthWest",109,WD_VERB);
			AddWord("In",110,WD_VERB);
			AddWord("Out",111,WD_VERB);
/*
 *	Second Verb Set
 */
			AddWord("ListClass",150,WD_VERB);
			AddWord("NameClass",151,WD_VERB);
			AddWord("SetClass",152,WD_VERB);
			AddWord("UnsetClass",153,WD_VERB);
			AddWord("TrackFlag",154,WD_VERB);
			AddWord("UnTrackFlag",155,WD_VERB);
			AddWord("ListTrack",156,WD_VERB);
			AddWord("Debugger",157,WD_VERB);
			AddWord("FindItem",158,WD_VERB);
			AddWord("FindFlag",159,WD_VERB);
			AddWord("Exorcise",160,WD_VERB);
/*
 *		New for 5.07
 */
			AddWord("NameTable",161,WD_VERB);
			AddWord("NameRFlag",162,WD_VERB);
			AddWord("NameOFlag",163,WD_VERB);
			AddWord("NamePFlag",164,WD_VERB);
			AddWord("NameCFlag",165,WD_VERB);
			AddWord("ListRFlags",166,WD_VERB);
			AddWord("ListOFlags",167,WD_VERB);
			AddWord("ListPFlags",168,WD_VERB);
			AddWord("ListCFlags",169,WD_VERB);
			AddWord("Which",170,WD_VERB);
/*
 *	5.08 Item table editing features
 */
			AddWord("EditOTable",171,WD_VERB);
			AddWord("EditSTable",172,WD_VERB);
			AddWord("CondExit",173,WD_VERB);
/*
 *	5.21 extensions
 */
			AddWord("EditDTable",174,WD_VERB);
			AddWord("SetSuper",175,WD_VERB);
			AddWord("ShowSuper",176,WD_VERB);
			AddWord("DeleteBSX",177,WD_VERB);
			AddWord("LoadBSX",178,WD_VERB);
			AddWord("ListBSX",179,WD_VERB);
			AddWord("ShowBSX",180,WD_VERB);
/*
 *	Prepositions
 */
			AddWord("in",1,WD_PREP);
			AddWord("to",2,WD_PREP);
			AddWord("with",3,WD_PREP);
			AddWord("at",4,WD_PREP);
/*
 *	5.07 System bit flag default names
 */
			RBitNames[0]=strdup("dark");
			RBitNames[1]=strdup("outside");
			RBitNames[2]=strdup("death");
			RBitNames[3]=strdup("merge");
			RBitNames[4]=strdup("join");
			RBitNames[5]=strdup("dropemsg");
			OBitNames[0]=strdup("flannel");
			OBitNames[1]=strdup("noit");
			OBitNames[2]=strdup("worn");
			OBitNames[3]=strdup("reserved");
			OBitNames[4]=strdup("canget");
			OBitNames[5]=strdup("canwear");
			OBitNames[6]=strdup("light");
			OBitNames[7]=strdup("light0");
			OBitNames[8]=strdup("noseecarry");
			PBitNames[0]=strdup("male");
			PBitNames[1]=strdup("female");
			PBitNames[2]=strdup("brief");
			PBitNames[3]=strdup("blind");
			PBitNames[4]=strdup("deaf");
			SendUser(u,"Initialised...\n");
		}
		else
			SendUser(u,"It already is, bird brain.\n");
		goto l1;
	}
	SetPersona(u);
/*
 *	! can be used if you put in a table 0 entry like any any any done, as
 *	a mistake. !command does the command without checking tables
 */
	if((*WordPtr==':')&&(ArchWizard(i)))
	{
		WordPtr++;
		v=GetVerb();
		goto l4;
	}
/*
 *	'*' cancels the autoverb feature
 */
	if(*WordPtr=='*')
	{
		WordPtr++;
		goto l7;
	}
	if(GetContext((short)u)->pa_Verb>0)	/* AUTOVERB */
		v=GetContext((short)u)->pa_Verb;
	else
l7:		v=GetVerb();
	x=WordPtr;
/*
 *	See if there is a table entry for the player. If so then
 *	do that and be finished.
 */
	if(UserAction(i,v)==1)
		goto l1;
	WordPtr=x;
l4:	switch(v)
	{
/*
 *	Select the correct system command routine
 */
		case -1:SendItem(i,"Sorry, But I don't recognise that verb.\n");
			break;
		case 1: Cmd_Abort(i);break;
		case 2: Cmd_AddVerb(i);break;
		case 3: Cmd_AddNoun(i);break;
		case 4: Cmd_AddAdj(i);break;
		case 5: Cmd_AddPrep(i);break;
		case 6: Cmd_AddPronoun(i);break;
		case 7: Cmd_AddOrdinate(i);break;
		case 8: Cmd_ItemInfo(i);break;
		case 9: Cmd_ListItems(i);break;
		case 10:Cmd_SetName(i);break;
		case 11:Cmd_SetState(i);break;
		case 12:Cmd_NewItem(i);break;
		case 13:Cmd_DelItem(i);break;
		case 14:Cmd_BeRoom(i);break;
		case 15:Cmd_BeObject(i);break;
		case 16:Cmd_BePlayer(i);break;
		case 17:Cmd_UnRoom(i);break;
		case 18:Cmd_UnObject(i);break;
		case 19:Cmd_UnPlayer(i);break;
		case 20:Cmd_SaveUniverse(i);
			PermitInput(u);
			return(1);
		case 21:Cmd_StatMe(i);break;
		case 22:Cmd_SetShort(i);break;
		case 23:Cmd_ShowRoom(i);break;
		case 24:Cmd_SetRFlag(i);break;
		case 25:Cmd_SetLong(i);break;
		case 26:Cmd_Look(i);break;
		case 27:Cmd_Goto(i);break;
		case 28:Cmd_Brief(i);break;
		case 29:Cmd_Verbose(i);break;
		case 30:Cmd_ObjectShow(i);break;
		case 31:Cmd_SetOFlag(i);break;
		case 32:Cmd_SetDesc(i);break;
		case 33:Cmd_Invisible(i);break;
		case 34:Cmd_Visible(i);break;
		case 35:Cmd_Say(i);break;
		case 36:Cmd_Place(i);break;
		case 37:Cmd_ListWord(i);break;
		case 38:Cmd_DelVerb(i);break;
		case 39:Cmd_DelNoun(i);break;
		case 40:Cmd_DelAdj(i);break;
		case 41:Cmd_DelPrep(i);break;
		case 42:Cmd_DelPronoun(i);break;
		case 43:Cmd_DelOrdinate(i);break;
		case 44:Cmd_NewExit(i);break;
		case 45:Cmd_DelExit(i);break;
		case 46:Cmd_SetOSize(i);break;
		case 47:Cmd_SetOWeight(i);break;
		case 48:Cmd_PlayerShow(i);break;
		case 49:Cmd_SetPFlag(i);break;
		case 50:Cmd_SetPSize(i);break;
		case 51:Cmd_SetPWeight(i);break;
		case 52:Cmd_SetPStrength(i);break;
		case 53:Cmd_SetPLevel(i);break;
		case 54:Cmd_SetPScore(i);break;
		case 55:Cmd_Users(i);break;
		case 56:Cmd_ListTables(i);break;
		case 57:Cmd_DeleteTable(i);break;
		case 58:Cmd_AddTable(i);break;
		case 59:Cmd_EditTable(i);break;
		case 60:Cmd_NewTable(i);break;
		case 61:
#ifdef ATTACH		
			if(UserList[u].us_RealPerson)
			{
				SetMe(i);
				Act_UnAlias();
				break;
			}
#endif
			SendTPacket(UserList[u].us_Port,PACKET_CLEAR,
			"Goodbye......\n");
			RemoveUser(u);
			return(1);
		case 62:Cmd_Chain(i);break;
		case 63:Cmd_UnChain(i);break;
		case 64:Cmd_Rename(i);break;
		case 65:Cmd_SaveTable(i);break;
		case 66:Cmd_SetActor(i);break;
		case 67:Cmd_SetAction(i);break;
		case 68:Cmd_BeContainer(i);break;
		case 69:Cmd_UnContainer(i);break;
		case 70:Cmd_SetVolume(i);break;
		case 71:Cmd_ContainerShow(i);break;
		case 72:Cmd_MsgExit(i);break;
		case 73:Cmd_SetCFlag(i);break;
		case 74:Cmd_SetUFlag(i);break;
		case 75:Cmd_SetUItem(i);break;
		case 76:Cmd_UFlagShow(i);break;
		case 77:Cmd_SetPicture(i);break;
		case 78:Cmd_Share(i);break;
		case 79:Cmd_UnShare(i);break;
		case 80:Cmd_Status(i);break;
		case 81:Cmd_NameFlag(i);break;
		case 82:Cmd_UnNameFlag(i);break;
		case 83:Cmd_ListFlags(i);break;
		case 84:Cmd_DoorPair(i);break;
		case 85:Cmd_ShowFlag(i);break;
		case 86:Cmd_SetFlag(i);break;
		case 87:Cmd_SetPerception(i);break;
		case 88:Cmd_ShowAllRooms(i);break;
		case 89:Cmd_ShowAllObjects(i);break;
		case 95:Cmd_ShowAllPlayers(i);break;
		case 96:Cmd_ObjEdit(i);break;
		case 100:Cmd_MoveDirn(i,0);break;
		case 101:Cmd_MoveDirn(i,1);break;
		case 102:Cmd_MoveDirn(i,2);break;
		case 103:Cmd_MoveDirn(i,3);break;
		case 104:Cmd_MoveDirn(i,4);break;
		case 105:Cmd_MoveDirn(i,5);break;
		case 106:Cmd_MoveDirn(i,6);break;
		case 107:Cmd_MoveDirn(i,7);break;
		case 108:Cmd_MoveDirn(i,8);break;
		case 109:Cmd_MoveDirn(i,9);break;
		case 110:Cmd_MoveDirn(i,10);break;
		case 111:Cmd_MoveDirn(i,11);break;
		case 150:Cmd_ListClass(i);break;
		case 151:Cmd_NameClass(i);break;
		case 152:Cmd_SetClass(i);break;
		case 153:Cmd_UnsetClass(i);break;
		case 154:Cmd_TrackFlag(i);break;
		case 155:Cmd_UnTrackFlag(i);break;
		case 156:Cmd_ListTrack(i);break;
		case 157:Cmd_Debugger(i);break;
		case 158:Cmd_FindItem(i);break;
		case 159:Cmd_FindFlag(i);break;
		case 160:Cmd_Exorcise(i);break;
		case 161:Cmd_NameTable(i);break;
		case 162:Cmd_RBitName(i);break;
		case 163:Cmd_OBitName(i);break;
		case 164:Cmd_PBitName(i);break;
    		case 165:Cmd_CBitName(i);break;
		case 166:Cmd_ListRBits(i);break;
		case 167:Cmd_ListOBits(i);break;
		case 168:Cmd_ListPBits(i);break;
		case 169:Cmd_ListCBits(i);break;
		case 170:Cmd_Which(i);break;
		case 171:Cmd_EditOTable(i);break;
		case 172:Cmd_EditSTable(i);break;
		case 173:Cmd_CondExit(i);break;
		case 174:Cmd_EditDTable(i);break;
		case 175:Cmd_SetSuperClass(i);break;
		case 176:Cmd_ShowSuperClass(i);break; 
		case 177:Cmd_DeleteBSX(i);break;
		case 178:Cmd_LoadBSX(i);break;
		case 179:Cmd_ListBSX(i);break;
		case 180:Cmd_ShowBSX(i);break;
		default:SendItem(i,"I don't understand.\n");
			break;
	}
/*
 *	Run the status table
 */
l1:	t=FindTable(100);
	if(t&&UserList[u].us_State==AWAIT_COMMAND)
		ExecBackground(t,UserList[u].us_Item);
panic:;
l2:	PermitInput(u);
	return 0;
}

