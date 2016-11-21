#define AWAIT_LOGIN	0	/* Free slot */
#define AWAIT_NAME	1
#define AWAIT_PASSWORD	2
#define AWAIT_PASSRETRY	3
#define AWAIT_PWSET	4
#define AWAIT_SETSEX	5
#define AWAIT_COMMAND	0
#define AWAIT_TEDIT	6
#define AWAIT_EDLIN	7
#define AWAIT_PWVERIFY	8	/* Three for password command */
#define AWAIT_PWNEW	9
#define AWAIT_PWVERNEW	10
#define AWAIT_ACK	0/*11*/	/* Waiting for death event */
#define AWAIT_OEDIT	20
#define AWAIT_OE1	20
#define AWAIT_OE2	21
#define AWAIT_OE3	22
#define AWAIT_OE4	23
#define AWAIT_OE5	24
#define AWAIT_OE6	25
#define AWAIT_OE7	26
#define AWAIT_OE8	27
#define AWAIT_OEND	27
#define AWAIT_EMAIL	28


struct User_Entry
{
 	char  us_Name[MAXNAME+1];	/* Note the +1 !, finding this has caused much grief ! */
	char  us_UserName[MAXUSERID+1];
	short us_State;
	short us_Flags;
	PORT *us_Port;
	ITEM *us_Item;
	short us_SysFlags[32];
	short us_UserInfo;	/* Two vars for state handler use */
	char *us_UserPtr;
	long us_Record;
	char us_Password[8];
#ifdef ATTACH
	ITEM *us_RealPerson;
#endif
	long us_Login;
};

typedef struct User_Entry USER;

#define PENT(x) (UserList[x].us_Password)
