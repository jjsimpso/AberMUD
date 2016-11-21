#include "System.h"
#include "User.h"

extern USER UserList[];

/******************************************************************************


	AberMUD 5.20

	This module provides the interface between the database language
	and the LibRwho.c file provided by the author of the RWHO 

	1.00		AGC		Created
	1.01		AGC		ANSIfication

******************************************************************************/

Module 	"Rwho Interface Library";
Author 	"Alan Cox";
Version "1.00";

void Act_RwhoDeclareUp(void)
{
	char *mudname=TextOf(ArgText());
	char *mudident=TextOf(ArgText());
	char *server=TextOf(ArgText());
	char *serverpw=TextOf(ArgText());
	rwhocli_setup(server,serverpw,mudname,mudident);
}

void Act_RwhoDeclareDown(void)
{
	rwhocli_shutdown();
}

void Act_RwhoDeclareAlive(void)
{
	rwhocli_pingalive();
}

void Act_RwhoLogin(void)
{
	char *id=TextOf(ArgText());
	char *name=TextOf(ArgText());
	int u=UserOf(Me());
	if(u!=-1)
		rwhocli_userlogin(id,name,UserList[u].us_Login);
}

void Act_RwhoLogout(void)
{
	char *name=TextOf(ArgText());
	rwhocli_userlogout(name);
}
