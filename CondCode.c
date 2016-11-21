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

Module	"Conditions";
Version "1.20";
Author	"Alan Cox";

/*
 *	Condition Code Logic For Table Drivers
 *
 *	1.15	Up to date for 5.06 release
 *	1.16	Fixed nasty order dependancies in condition logic
 *		Added specific bit flag entries
 *	1.17	5.07 version with the extra types (table etc)
 *	1.18	5.12 Stuff added to tables
 *	1.19	5.20 RWHO added
 *	1.20	5.20 Added FORKDUMP
 *	1.21	5.21 Added PROCOBJECT PROCSUBJECT PROCDAEMON
 */

char *Cnd_Table[]=
{
	"I       AT",
	"I       NOTAT",
	"I       PRESENT",
	"I       ABSENT",
	"I       WORN",
	"I       NOTWORN",
	"I       CARRIED",
	"I       NOTCARR",
	"II      ISAT",
	"II      ISNOTAT",
	"II      ISBY",
	"II      ISNOTBY",
	"F       ZERO",
	"F       NOTZERO",
	"FN      EQ",
	"FN      NOTEQ",
	"FN      GT",
	"FN      LT",
	"FF      EQF",
	"FF      NOTEQF",
	"FF      LTF",
	"FF      GTF",
	"II      ISIN",
	"II      ISNOTIN",
	"a       ADJ1",
	"a       ADJ2",
	"n       NOUN1",
	"n       NOUN2",
	"p       PREP",
	"N       CHANCE",
	"I       ISPLAYER",
	"I       ISUSER",
	"I       ISROOM",
	"I       ISOBJECT",
	"IN      STATE",
	"IP      PFLAG",
	"IO      OFLAG",
	"II      CANPUT",
	"IR      RFLAG",
	"N       LEVEL",
	"        IFDEAF",
	"        IFBLIND",
	"        ARCH",
	"I       GET",
	"I       DROP",
	"I       REMOVE",
	"I       WEAR",
	"I       CREATE",
	"I       DESTROY",
	"I       PUTO",
	"II      SWAP",
	"II      PLACE",
	"II      PUTIN",
	"II      TAKEOUT",
	"IBF     COPYOF",
	"FIB     COPYFO",
	"FF      COPYFF",
	"N       WHATO",
	"NI      GETO",
	"IF      WEIGH",
	"F       SET",
	"F       CLEAR",
	"IP      PSET",
	"IP      PCLEAR",
	"FN      LET",
	"FN      ADD",
	"FN      SUB",
	"FF      ADDF",
	"FF      SUBF",
	"FN      MUL",
	"FN      DIV",
	"FF      MULF",
	"FF      DIVF",
	"FN      MOD",
	"FF      MODF",
	"FN      RANDOM",
	"F       MOVE",
	"I       GOTO",
	"IN      WEIGHT",
	"IN      SIZE",
	"IO      OSET",
	"IO      OCLEAR",
	"IR      RSET",
	"IR      RCLEAR",
	"II      PUTBY",
	"I       INC",
	"I       DEC",
	"IN      SETSTATE",
	"T       PROMPT",
	"F       PRINT",
	"        SCORE",
	"T       MESSAGE",
	"T       MSG",
	"I       LISTOBJ",
	"I       LISTAT",
	"        INVEN",
	"        DESC",
	"T       END",
	"        DONE",
	"        NOTDONE",
	"        OK",
	"        ABORT",
	"        SAVE",
	"T       PARSE",
	"        NEWTEXT",
	"t       PROCESS",
	"ICN     DOCLASS",
	"II      GIVE",
	"INT     SETUT",
	"ITN     DOESACTION",
	"ITIN    DOESTO",
	"ITIN    DOESTOPLAYER",
	"IN      POBJ",
	"IN      PLOC",
	"I       PNAME",
	"I       PCNAME",
	"Ivnn    DAEMON",
	"vnn     ALLDAEMON",
	"Ivnn    HDAEMON",
	"Nt      WHEN",
	"IT      SETNAME",
	"INN     DUP",
	"        FRIG",
	"N       POINTS",
	"N       HURT",
	"N       CURED",
	"T       KILLOFF",
	"v       AUTOVERB",
	"        IF1",
	"        IF2",
	"T       BUG",
	"T       TYPO",
	"FN      NARG",
	"I       ISME",
	"TN      BROADCAST",
	"IT      ISCALLED",
	"II      IS",
	"I       SETME",
	"        PRONOUNS",
	"N       CHANCELEV",
	"I       EXITS",
	"        PWCHANGE",
	"I       SNOOP",
	"NI      UNSNOOP",
	"IN      GETUT",
	"TTT     CAT",
	"T       BECOME",
	"I       ALIAS",
	"        UNALIAS",
	"N       FIELD",
	"N       NEEDFIELD",
	"N       UNVEIL",
	"N       DEBUG",
	"IF      GETSCORE",
	"IF      GETSTR",
	"IF      GETLEV",
	"IF      SETSCORE",
	"IF      SETLEV",
	"IF      SETSTR",
	"T       SHELL",
	"Ic      CSET",
	"Ic      CCLEAR",
	"Ic      CFLAG",
	"I       CANSEE",
	"        RESCAN",
	"vnnN    MEANS",
	"Ivnn    TREEDAEMON",
	"T       SETIN",
	"T       SETOUT",
	"T       SETHERE",
	"IF      CANGOTO",
	"        MOBILES",
	"        DIR",
	"        ROOMS",
	"Ivnn    CHAINDAEMON",
	"IF      CANGOBY",
	"INI     SETIFLAG",
	"INN     GETIFLAG",
	"IN      CLEARIFLAG",
	"II      [FIGHT]",	/* Unused */
	"INN     WHERETO",
	"IIF     DOOREXIT",
	"I       CANMOVEROPE",
	"II      PLACEROPE",
	"I       ISROPE",
	"I       ISTIED",
	"IN      ROPEPREV",
	"IN      ROPENEXT",
	"II      TIE",
	"II      UNTIE",
	"II      JOIN",
	"II      CUTROPE",
	"IIN     DISTANCE",
	"IIN     WHICHWAY",
	"IC      CLASSAT",
	"II      DUPOF",
	"IN      MASTEROF",
	"IN      TIEDTO", 
	"$       COMMENT",
	"vanpan  COMVOCAB",
	"vIpI    COMMAND",
	"T       BSXSCENE",
	"NNT     BSXOBJECT",
	"        NOT",
	"        IFDARK",
	"IN      VISIBILITY",
	"IN      GETPARENT",
	"IN      GETNEXT",
	"IN      GETCHILDREN",
	"N       PEXIT",
	"INTTT   SETDESC",
	"ITTT    SETLONG",
	"ITTT    SETSHORT",
	"I       GETLONG",
	"I       GETSHORT",
	"IN      GETDESC",
	"I       GETNAME",
	"        SWAT",
	"F       FLAT",
	"NN      FINDMASTER",
	"INN     NEXTMASTER",
	"INN     FINDIN",
	"INNN    NEXTIN",
	"TN      LENTEXT",
	"I       PROCSUBJECT",
	"I       PROCOBJECT",
	"I       PROCDAEMON",
	"IN      GETSUPER",
	"II      SETSUPER",
	"II      MEMBER",
	"        ",
	"        CLS",
	"        ",
	"        ",
	"        ",
	"        ",
	"        ",
	"        ",
	"        ",
	"        ",
	"        ",
	"        ",
	"        ",
	"IC      ISCLASS",
	"TT      SUBSTR",
	"I       GETIN",
	"I       GETOUT",
	"I       GETHERE",
	"TTT     LOG",
	"IC      SETCLASS",
	"IC      UNSETCLASS",
	"FN      BITCLEAR",
	"FN      BITSET",
	"FN      BITTEST",
	"F       SPRINT",
	"NNNIIT  USER",
	"NI      SETI",
	"Ivnn    CDAEMON",
	"T       DELETE",
	"TI      ULOAD",
	"TI      USAVE",
	"TNN     FLOAD",
	"TNN     FSAVE",
	"IF      GETVIS",
	"IT      MESSAGETO",
	"IT      MSGTO",
	"TTTT    RWHOINIT",
	"        RWHODOWN",
 	"TT      RWHOLOGIN",
	"T       RWHOLOGOUT",
	"        RWHOPING",
	"INI     SETEXIT",
        "IN      DELEXIT",
	"INN     GETEXIT",
	"T       FORKDUMP",
	NULL
};

/*	ARGUMENT CODING:
 *	N=NUMBER I=ITEM p=PREPWORD n=NOUNWORD a=ADJWORD
 *	F=FLAG(0-511) B=BIT(0-15) $=COMMENT TEXT R=RBIT
 *	P=PBIT O=OBIT C=CLASS c=CBIT t=table
 *
 *	Thus an action needing a text an item and a noun would be
 *	TIN<5 spaces>ACTIONNAME
 *
 *	And would call ArgText(),ArgItem(), and ArgNum() to get its args
 *	Note: ArgText returns a TXT pointer - to get the string use 
 *	TextOf(ArgText()).
 */
	
/*
 *	Find an action/condition by name
 */

int FindCnd(x)
char *x;
{
	int i=0;
	while(Cnd_Table[i])
	{
		if(stricmp(x,Cnd_Table[i]+8)==0)
			return(i);
		i++;
	}
	return(-1);
}

/*
 *	Condition Evaluators
 */

int Cnd_At(void)
{
	return(O_PARENT(Me())==ArgItem());
}

int Cnd_NotAt(void)
{
	return(O_PARENT(Me())!=ArgItem());
}

int Cnd_Present(void)
{
	ITEM *i=ArgItem();
	if(O_PARENT(i)==Me())
		return(1);
	if(O_PARENT(i)==O_PARENT(Me()))
		return(1);
	return(0);
}

int Cnd_Absent(void)
{
	return(1-Cnd_Present());
}

int Cnd_Worn(void)
{
	ITEM *i=ArgItem();
	if(!IsObject(i))
		return(0);
	if(O_PARENT(i)!=Me())
		return(0);
	if(ObjectOf(i)->ob_Flags&OB_WORN)
		return(1);
	return(0);
}

int Cnd_NotWorn(void)
{
	return(1-Cnd_Worn());
}

int Cnd_Carried(void)
{
	return(O_PARENT(ArgItem())==Me());
}

int Cnd_NotCarr(void)
{
	return(O_PARENT(ArgItem())!=Me());
}

int Cnd_IsAt(void)
{
	ITEM *i=ArgItem();
	return(O_PARENT(i)==ArgItem());
}

int Cnd_IsNotAt(void)
{
	ITEM *i=ArgItem();
	return(O_PARENT(i)!=ArgItem());
}

int Cnd_IsBy(void)
{
	ITEM *i=ArgItem();
	return(O_PARENT(i)==O_PARENT(ArgItem()));
}

int Cnd_IsNotBy(void)
{
	ITEM *i=ArgItem();
	return(O_PARENT(i)!=O_PARENT(ArgItem()));
}

int Cnd_Zero(void)
{
	return(GetFlag(ArgNum())==0);
}

int Cnd_NotZero(void)
{
	return(GetFlag(ArgNum())!=0);
}

int Cnd_Eq(void)
{
	int v=ArgNum();
	return(GetFlag(v)==ArgNum());
}

int Cnd_NotEq(void)
{
	int v=ArgNum();
	return(GetFlag(v)!=ArgNum());
}

int Cnd_Gt(void)
{
	int v=ArgNum();
	return(GetFlag(v)>ArgNum());
}

int Cnd_Lt(void)
{
	int v=ArgNum();
	return(GetFlag(v)<ArgNum());
}

int Cnd_EqF(void)
{
	int v=ArgNum();
	return(GetFlag(v)==GetFlag(ArgNum()));
}

int Cnd_NeF(void)
{
	int v=ArgNum();
	return(GetFlag(v)!=GetFlag(ArgNum()));
}

int Cnd_LtF(void)
{
	int v=ArgNum();
	return(GetFlag(v)<GetFlag(ArgNum()));
}

int Cnd_GtF(void)
{
	int v=ArgNum();
	return(GetFlag(v)>GetFlag(ArgNum()));
}

int Cnd_IsIn(void)
{
	ITEM *i=ArgItem();
	return(Contains(i,ArgItem()));
}

int Cnd_IsNotIn(void)
{
	return(1-Cnd_IsIn());
}

int Cnd_Adj1(void)
{
	return(ArgWord()==Adj1);
}

int Cnd_Adj2(void)
{
	return(ArgWord()==Adj2);
}

int Cnd_Noun1(void)
{
	return(ArgWord()==Noun1);
}

int Cnd_Noun2(void)
{
	return(ArgWord()==Noun2);
}

int Cnd_Prep(void)
{
	return(ArgWord()==Prep);
}

int Cnd_Chance(void)
{
	return(RandPerc()<ArgNum());
}

int Cnd_IsPlayer(void)
{
	return(IsPlayer(ArgItem()));
}

int Cnd_IsUser(void)
{
	return(UserOf(ArgItem())!=-1);
}

int Cnd_IsRoom(void)
{
	return(IsRoom(ArgItem()));
}

int Cnd_IsObject(void)
{
	return(IsObject(ArgItem()));
}

int Cnd_State(void)
{
	ITEM *i=ArgItem();
	return(O_STATE(i)==ArgNum());
}

int Cnd_PFlag(void)
{
	PLAYER *p=PlayerOf(ArgItem());
	if(p==NULL)
	{
		ArgNum();
		return(0);
	}
	return(((p->pl_Flags&(1<<ArgNum())))?1:0);
}

int Cnd_OFlag(void)
{
	OBJECT *o=ObjectOf(ArgItem());
	if(o==NULL)
	{
		ArgNum();
		return(0);
	}
	return(((o->ob_Flags&(1<<ArgNum())))?1:0);
}

int Cnd_RFlag(void)
{
	ROOM *r=RoomOf(ArgItem());
	if(r==NULL)
	{
		ArgNum();
		return(0);
	}
	return(((r->rm_Flags&(1<<ArgNum())))?1:0);
}

int Cnd_CFlag(void)
{
	CONTAINER *c=ContainerOf(ArgItem());
	if(c==NULL)
	{
		ArgNum();
		return(0);
	}
	return(((c->co_Flags&(1<<ArgNum())))?1:0);
}

int Cnd_CanPut(void)
{
	ITEM *a=ArgItem();
	return(((CanPlace(a,ArgItem()))==0)?1:0);
}

int Cnd_Level(void)
{
	return(LevelOf(Me())>=ArgNum());
}

int Cnd_IfDeaf(void)
{
	PLAYER *p=PlayerOf(Me());
	if(p==NULL)
		return(0);
	return(p->pl_Flags&PL_DEAF);
}

int Cnd_IfBlind(void)
{
	PLAYER *p=PlayerOf(Me());
	if(p==NULL)
		return(0);
	return(p->pl_Flags&PL_BLIND);
}

int Cnd_Arch(void)
{
	return(ArchWizard(Me()));
}

int Cnd_Is(void)
{
	return(ArgItem()==ArgItem());
}

int Cnd_ChanceLev(void)
{
	return(RandPerc()<(ArgNum()*LevelOf(Me())));
}

int Cnd_CanSee(void)
{
	return(CanSee(LevelOf(Me()),ArgItem()));
}

int Cnd_IsClass(void)
{
	ITEM *i=ArgItem();
	short cm=1<<ArgNum();
	if(i->it_Class&cm)
		return(1);
	else
		return(0);
}


