 /****************************************************************************\
 *									      *
 *			C R E A T O R    O F   L E G E N D S		      *
 *				(AberMud Version 5)			      *
 *									      *
 *  The Creator Of Legends System is (C) Copyright 1989 Alan Cox, All Rights  *
 *  Reserved.		  						      *
 *									      *
 \****************************************************************************/

/******************************************************************************
 *									      *
 *  			System Include File: Version 5.30		      *
 *									      *
 *****************************************************************************/ 
/* -------------------------- System Includes ------------------------ */

#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <stdarg.h>
#include <unistd.h>

#include "IPC.h"
#include "Comms.h"

/*
 *	WARNING: The system is set up to run on a compiler which promotes
 *	all arguments: eg lattice C. If you need to change this, you will
 *	have to implement proper varargs and also tidy up some of the 
 *	function calls, where the typing is not precise. A simpler approach
 *	is to find a compiler with sizeof(int)==sizeof(long) and then 
 *	#define short int, since I don't think any library calls expect shorts.
 */

#define CPRT	"Alan Cox"

/*
 *	These seem dodgy on Lattice 4.01 so we don't use the builtins
 *	The other good bugs to watch include being allowed to return(n)
 *	from a void, which simply loses n
 */

#ifdef LETTUCE_4
#ifdef stricmp
#undef stricmp
#endif
#ifdef strcmp
#undef strcmp
#endif
#ifdef memcmp
#undef memcmp
#endif
#endif

/*
 *	GNU C macro bits to keep warnings down
 */
 
#ifdef __GNUC__
#define unused __attribute__((unused))
#define noreturn __attribute__((noreturn))
#else
#define unused
#define noreturn
#endif
 
/*
 *	---------- Configuration Options ---------
 */

#define ANSI_C					/* Has prototypes */
/*#define NO_VOID*/				/* Doesn't know 'void' */
#define LOG_FILE	"Creator.log"		/* Where to dump logging crud */
/*#define SECURE*/					/* Disable shell/file stuff */
#define	UNIX
/*#define AMIGA */
#define MAXUSER		128			/* Max users at once	      */
#define MAXNAME		14			/* Maximum length of players  */
						/* Don't set this >32         */
#define MAXUSERID	32	/* 8 for U*IX *//* Max space to hold a userid */

/*
 *	If you keep MAXNAME/MAXUSERID 32 even if too large, it will ensure the
 *	user file is compatible with any other machines which do the same
 */

/*#define PANSY_MODE*/			/* Americans don't like dying.. */
#define REGISTER			/* For irritating users */

#define TCP_PORT	5000	

#define USERFILE	"UAF"		/* Where to find characters   */

#define BOSS1   "Arashi"		/* Name of priviledged chars */
#define BOSS2	"Hobbit"			/*  ""         ""        ""   */
#define BOSS3	"Debugger"		/*  ""         ""	 ""   */
#define BOSS4	"Bonzo"		/*  ""         ""	 ""   */
#define BOSS5	":Boss"		/*  ""         ""	 ""   */

/*
 *	If you change BOSS1/BOSS2 recompile SysSupport. If you change USERFILE
 *	recompile UserFile.c. If you change any of the MAXxxxx entries recompile
 *	everything which mentions User.h (most of it)
 *	If you change LOG_FILE recompile system.c. To disable Logging on the 
 *	Amiga set LOG_FILE to "NIL:" or hack it out of the code. If you want
 *	you can set LOG_FILE to a console window and monitor the log output on
 *	it instead of into a file, or you can set it to go through a PIPE: 
 *	device to another program.
 */
 
/*
 *	Other Options
 */

#define CHECK_TXT		/* Check all text pointers at low level */
/* CHECK_TXT is not supported on the unix implementation */
#define CHECK_ITEM		/* Check all item pointers at low level */

/* The two above lower system performance especially on large games -
   they are meant for system debugging (or paranoid authors). Its a good
   idea *NOT* to use these on a virtual memory system
*/
#undef BOOT_DEBUG		/* Startup with Debug set		*/
/* For errors prior to character logins - unless is really screws up you
 * can stick a DEBUG command in the startup and save the db again then 
 * restart - this option is meant for emergencies - I use it always tho
 * and have a debug 0 at the end of startup - of course if it crashes at the
 * moment between debug 0 and login I got problems....
 */
#define ATTACH			/* Include attach function */



#ifdef NO_VOID
#define void
#endif

/* ------------------------ Control Macros -------------------------- */


#define Module 	static char *Mod_Name unused =
#define Version static char *Ver_Name unused =
#define Author  static char *Aut_Name unused =

#define Error(x)	ErrFunc((x),Mod_Name,Ver_Name,__LINE__,__FILE__)
#define Allocate(x) \
	(x *)AllocFunc(sizeof(x),Mod_Name,Ver_Name,__LINE__,__FILE__)
#define CheckItem(x)	ICheck((x),__LINE__,__FILE__)
#define CheckTxt(x)	TCheck((x),__LINE__,__FILE__)

/* ----------------- Creator Of Legends Structures ----------------- */

struct Txt
{
	char *te_Text;		/* The string of text */
	short te_Users;		/* Number of uses - if this goes >32767
				   trouble, so every 32768th is new one! */
	struct Txt *te_Next;	/* Next text */
};

typedef struct Txt TXT;	/* No longer TEXT - clashed with exec/types.h */
typedef struct Txt * TPTR;	/* Text Pointer */

struct WordList
{
	char *wd_Text;			/* The word text */
	short wd_Type;			/* Its type - see WD_ defs */
	short wd_Code;			/* Code for this word */
	struct WordList *wd_Next;	/* Next Word */
};


typedef struct WordList WLIST;


struct Sub		/* Substruct Node */
{
	struct Sub *pr_Next;	/* Next Substructure */
	short pr_Key;		/* Substructure identity key */
};

typedef struct Sub SUB;

typedef struct Table TABLE;	/* Forward reference for items */

struct Item
{
	struct Item *it_MasterNext;	/* Next Master Entry 	  */
	struct Item *it_Parent;		/* Parent Entry		  */
	struct Item *it_Children;	/* First item it contains */
	struct Item *it_Next;		/* Next item in this chain*/
	short  it_Noun;			/* Noun for the item	  */
	short  it_Adjective;		/* Adjective for it	  */
	short  it_ActorTable;		/* Table for actions by it*/
	short  it_ActionTable;		/* Daemon handler for it  */
	short  it_Users;		/* Number of locks on it  */
	short  it_State;		/* Item actual state      */
	short  it_Class;		/* Class bitmask          */
	short  it_Perception;		/* Level needed to see it */
	TPTR   it_Name;			/* Items name string      */
	SUB   *it_Properties;		/* Substructure chain	  */
	struct Item *it_Superclass;	/* Superclass for tables  */
/*
 *	New for 5.08 - item bound tables
 */
	TABLE *it_ObjectTable;
	TABLE *it_SubjectTable;
	TABLE *it_DaemonTable;		/* Personal Daemon Table */
/*
 *	New for 5.30 - item zone
 */
	short    it_Zone;		/* Item 'zone' */
};

typedef struct Item ITEM;

/*
 *	Useful Item Macros
 */

#define O_NEXT(o)	((o)->it_Next)		/* The next item in room   */
#define O_PARENT(o)	((o)->it_Parent)	/* The holder of this item */
#define O_FREE(o)	(O_PARENT(o)==NULL)	/* True if item is in void */
#define O_CHILDREN(o)	((o)->it_Children)	/* The first item within   */
#define O_EMPTY(o)	(O_CHILDREN(o)==NULL)	/* True if item empty      */
#define O_PROPERTIES(o)	((o)->it_Properties)	/* First property of item  */
#define O_ADJECTIVE(o)	((o)->it_Adjective)	/* Adjective of item       */
#define O_NOUN(o)	((o)->it_Noun)		/* Noun of item            */
#define O_STATE(o)	((o)->it_State)		/* The state of the item   */
						/* DO NOT USE TO SET       */
#define O_USERS(o)	((o)->it_Users)		/* Number of locks on item */
						/* DO NOT USE TO SET 	   */

/* --------------------------- SubStructs -------------------------- */

struct Sub_Room
{
	SUB rm_Sub;
	TPTR rm_Short;		/* Short Text */
	TPTR rm_Long;		/* Long Text  */
	unsigned short rm_Flags;
};

typedef struct Sub_Room ROOM;

#define RM_DARK		1	/* Room has no light source 		*/
#define RM_OUTSIDE	2	/* Room is outside	(OBSOLETE)	*/
#define RM_DEATH	4	/* Room description kills players 	*/
#define RM_MERGE	8	/* Room should be merged with parent    */
#define RM_JOIN		16	/* Room may be used in a merge		*/
#define RM_DROPEMSG	32	/* Drop description if doing a msgexit  */

/* PICMASK is obsolete */
#define RM_PICMASK	0xFFC0	/* Max of 1024 pictures 		*/
#define GETPICTURE(x)	(((x)&RM_PICMASK)>>6)

struct Sub_Object
{
	SUB ob_Sub;		/* Substructure node 		*/
	TPTR ob_Text[4];	/* State 0-3 texts   		*/
/*	
 *	Under AGOSII only Text[3] is used. This is compatible databasewise with
 *	a Creator Of Legends database and thus makes testing easier. Of course
 *	run time trashes all the junk anyway 
 */
	short ob_Size;		/* Size (overrides Player size) */
	short ob_Weight;	/* Weight (overrides Player)    */
	short ob_Flags;		/* Misc Info			*/
};

typedef struct Sub_Object OBJECT;

#define OB_FLANNEL	1	/* Merge text into main description 	*/
#define OB_NOIT		2	/* Do not affect pronouns 		*/
#define OB_WORN		4	/* Item is 'worn' 			*/
#define OB_DESTROYED	8	/* Item is destroyed (OBSOLETE ) 	*/
#define OB_CANGET	16	/* Player can get the item (used by GET)*/
#define OB_CANWEAR	32	/* Player can wear item (used by WEAR)	*/
#define OB_LIGHTSOURCE	64	/* Always lit */
#define OB_LIGHT0	128	/* Lit in state 0 */
#define OB_NOSEECARRY	256	/* Don't see item when carried */

struct Sub_Player
{
	SUB pl_Sub;
	short pl_UserKey;	/* -1 for mobiles 	*/
	short pl_Size;		/* Overriden by object  */
	short pl_Weight;	/*    ""     ""   ""    */
	short pl_Strength;	/* 1/10 Carrying Limit  */
	short pl_Flags;		/* Misc Info		*/
	short pl_Level;		/* User Utilitised	*/
	long  pl_Score;		/* User Utilitised	*/
};

#define PL_MALE		1	/* Player is male	*/
#define PL_FEMALE	2	/* Player is female	*/
#define PL_NEUTER	0	/* Player is sexless    */
#define PL_CONFUSED	3	/* Person with a problem! */
#define PL_SEXBITS	3	/* Sex mask		*/
#define PL_BRIEF	4	/* Player in BRIEF mode	*/
#define PL_BLIND	8	/* Player cannot see	*/
#define PL_DEAF		16	/* Player cannot hear	*/

typedef struct Sub_Player PLAYER;

struct Sub_GenExit
{
	SUB ge_Sub;
	ITEM *ge_Dest[12];	/* Array of exits NULL - none */
};

typedef struct Sub_GenExit GENEXIT;

struct Sub_CondExit
{
	SUB ce_Sub;
	ITEM *ce_Dest;
	short ce_Table;
	short ce_ExitNumber;
};

typedef struct Sub_CondExit CONDEXIT;

struct Sub_MsgExit
{
	SUB me_Sub;
	ITEM *me_Dest;		/* Where it goes    */
	TPTR me_Text;		/* What text to use */
	short me_ExitNumber;	/* Which exit it is */
};

typedef struct Sub_MsgExit MSGEXIT;

struct Sub_Chain
{
	SUB ch_Sub;
	ITEM *ch_Chained;	/* The linked item */
};

typedef struct Sub_Chain CHAIN;

struct Sub_UserFlag
{
	SUB uf_Sub;
	short uf_Flags[8];	/* 8 Number Flags (currently) */
	ITEM *uf_Items[8];	/* 8 Item Flags   (currently) */
};

typedef struct Sub_UserFlag USERFLAG;

struct Sub_Container
{
	SUB co_Sub;
	short co_Volume;	/* MAX Volume */
	short co_Flags;
	TPTR  co_ConText;	/* Used to prefix contents lists (eg 'On the shelf is') */
};

typedef struct Sub_Container CONTAINER;

#define CO_SOFT		1	/* Item has size increased by contents  */
#define CO_SEETHRU	2	/* You can see into the item		*/
#define CO_CANPUTIN	4	/* For PUTIN action			*/
#define CO_CANGETOUT	8	/* For GETOUT action			*/
#define CO_CLOSES	16	/* Not state 0 = closed			*/
#define CO_SEEIN	32	/* Container shows contents by		*/

struct Sub_Snoop
{
	SUB sn_Sub;
	ITEM *sn_Snooper;		/* Who is snooping   */
	char sn_String[12];		/* 12 reserved bytes */
	struct Sub_SnoopBack *sn_BackPtr; /* Other half of data */
	short sn_Ident;			/* Ident flags       */
};

#define SN_PLAYER	1	/* Watching someone/thing  */
#define SN_PLACE	2	/* Not implemented yet 	   */
#define SN_GLOBAL	3	/* Fun.... but not working */

typedef struct Sub_Snoop SNOOP;

struct Sub_SnoopBack
{
	SUB sb_Sub;
	ITEM *sb_Snooped;	/* What we snooping 		  */
	SNOOP *sb_SnoopKey;	/* Pointer to its snoop structure */
};

typedef struct Sub_SnoopBack SNOOPBACK;

struct Sub_Dup
{
	SUB du_Sub;
	ITEM *du_Master;		/* Master this is copied from */
};

typedef struct Sub_Dup DUP;

/*
 *	Ropes not currently supported
 */

struct Sub_Rope
{
	SUB ro_Sub;
	ITEM *ro_Next[2];		/* Next node in both dirns 	*/
	ITEM *ro_Tied;			/* First item this is tied to   */
	unsigned long ro_Event;		/* Event key - stops looping    */
	short ro_Position;		/* Node positioning	 	*/
};

typedef struct Sub_Rope ROPE;

struct Sub_TieChain
{
	SUB tc_Sub;
	ITEM *tc_Rope;			/* The rope we are tied to         */
	ITEM *tc_Next;			/* Next item tied in this list     */
	unsigned long tc_Event;		/* Event number to avoid accidents */
};

typedef struct Sub_TieChain TIECHAIN;

/*
 *	Inheritance of sub items
 */

struct Sub_Inherit
{
	SUB in_Sub;
	ITEM *in_Master;	/* Who to inherit from - NOT NESTED! */
};

typedef struct Sub_Inherit INHERIT;

struct Sub_InOutHere
{
	SUB io_Sub;
	TPTR io_InMsg;
	TPTR io_OutMsg;
	TPTR io_HereMsg;
};

typedef struct Sub_InOutHere INOUTHERE;

struct Sub_UserText
{
	SUB ut_Sub;
	TPTR ut_Text[8];
};

typedef struct Sub_UserText USERTEXT;

/* New in 5.30 */

struct Tag_Generic
{
	unsigned short ta_TagID;
	unsigned short ta_TagInfo;
	/* Tag data follows */
};

#define TAGID_SPACE		0		/* Tag ID for free space */
#define TAGID(x)		((x)&0x7FFF)	/* Tag field */
#define TAGID_SIMPLE		0x8000		/* Tag has simple (2 byte) data form */
	
struct Sub_Generic
{
	SUB ge_Sub;
	unsigned short ge_TagCount;
	struct Tag_Generic ge_Tags[1];		/* Array of 1 or more tags. */
};
	
/*	Substructure Identity Tags. Note: If a substructure is changed so that
 *	it is incompatible with its old saved form, a new tag id should be 
 *	defined for it, and a converting load routine added for the old one
 *	Such convertors will eventually be discarded when sufficiently old
 *	in the interests of code size. The other approach is to leave the tag
 *	ID the same, and to change system version number for the save format.
 *	The first method is much preferred
 */

#define KEY_ROOM	1	
#define KEY_OBJECT	2	
#define KEY_PLAYER	3
#define KEY_GENEXIT	4
#define KEY_MSGEXIT	5
#define KEY_CONDEXIT	6
#define KEY_CONTAINER	7	
#define KEY_CHAIN	8
#define KEY_USERFLAG	9	
#define KEY_BACKTRACK	10	/* OBSOLETE - UNUSED */
#define KEY_SNOOP	11
#define KEY_SNOOPBACK	12
#define KEY_ROPE	13	/* Reserved - unused */
#define KEY_DUPED	14
#define KEY_TIECHAIN	15	/* Reserved - unused */
#define KEY_INOUTHERE	16
#define KEY_USERTEXT	17
#define KEY_USERFLAG2   18
/*
 *	Entries 18-63 are reserved for system expansion - try to use 64-254 if
 *	you add your own user substructures, in case of system updates.
 */

#define KEY_INHERIT	255	/* Inherit */

#define WD_NOUN		1
#define WD_PREP 	2
#define WD_PRONOUN	3
#define WD_CLASS	4
#define WD_VERB		5
#define WD_ADJ		6
#define WD_NOISE	7	/* DO NOT USE! */
#define WD_ORDIN	8

struct ParserContext
{
	short pa_It[2];		/* Adj[0] Noun[1] of pronoun */
	short pa_Them[2];
	short pa_Him[2];
	short pa_Her[2];
	short pa_There[2];
	short pa_Verb;		/* <1 means none */
};

typedef struct ParserContext PCONTEXT;

typedef struct Line LINE;

struct Line
{
	short li_Verb;			/* -1 = any -2 = none */
	short li_Noun1;	
	short li_Noun2;
	unsigned short *li_Data;	/* The line command bytes */
	LINE *li_Next;			/* Next line to execute   */
};


struct Table
{
	short tb_Number;		/* Table number 	  */
	LINE *tb_First;			/* First line   	  */
	TABLE *tb_Next;			/* Next table in list     */
	TXT   *tb_Name;			/* Tables now have names! */
};


#define CMD_EOL	10000

/*
 *	WARNING: The UserFileFormat structure IS machine dependant and
 *	length dependant. If you need to port a user file from machine
 *	to machine, write a program to read each entry and dump it all
 *	in Ascii on one machine, and another to convert the ascii back
 *	to UFF structures and resave. So long as you stick to similar
 *	processor machines you should be ok - eg SUN to AMIGA.
 */

struct UserFileFormat
{
	char uff_Name[32/*MAXNAME*/];
	short uff_Perception;
	short uff_ActorTable;
	short uff_ActionTable;
	short uff_Size;
	short uff_Weight;
	short uff_Strength;
	short uff_Flags;
	short uff_Level;
	long  uff_Score;
	char uff_Password[8];
	long uff_Flag[10];
	char uff_Reserved[28];		/* For userflags etc */
};

typedef struct UserFileFormat UFF;

#define Arch(x)	((ArchWizard(x))||(stricmp(CNameOf(x),CPRT)==0))
#define ArchCheck(x) ArchWizard(x)

/* Now the stuff for BSX. I did consider binding images to rooms and stuff, but it raises a few 
   awkward issues when you wish to do special effects, and by keeping them together it speeds
   up BSX requests and lets me compress the hex format BSX data in memory */

typedef struct _bsximage
{
	struct _bsximage *bsx_Next;
	char bsx_Identifier[8];
	unsigned char *bsx_Data;
	short bsx_DataSize;
} BSXImage;

/* ----------------------- System.c Functions ---------------------- */

#ifdef ANSI_C
#include "Prototype.h"		/* NON ANSI IS UNTESTED */
#else
#include "NoPrototype.h"
#endif
