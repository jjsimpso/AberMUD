/*
 *	If you have full ANSI prototyping this file will help keep everything
 *	as the right form of arguments. All the VARARG cases are identified here
 *	to aid porting, as are any known bugs
 */

/*	SYSTEM.C	*/

extern long TextMemory;
extern long ItemMemory;
extern long SubMemory;
extern void ErrFunc(char *,char *,char *,int,char *);
extern void Log();
extern char *AllocFunc(int,char *,char *,int,char *);
extern TPTR TextList[2][75];
extern TPTR AllocText(char *);
extern TPTR AllocComment(char *);
extern TPTR QuickAllocText(char *);
extern long CountTexts(void);
extern long TextSize(void);
extern long TextNumber(TPTR);
extern char *TextOf(TPTR);
extern void FreeText(TPTR);
extern void FreeComment(TPTR);
extern int UnlinkItem(ITEM *);
extern int LinkItem(ITEM *,ITEM *);
extern ITEM *ItemList;
extern ITEM *CreateItem(char *,int,int);
extern int FreeItem(ITEM *);
extern void LockItem(ITEM *);
extern void UnlockItem(ITEM *);
extern void SetState(ITEM *,short);
extern void SetVocab(ITEM *,short,short);
extern int WordMatch(ITEM *,short,short);
extern int CanSee(short,ITEM *);
extern ITEM *FindMaster(short,short,short);
extern ITEM *NextMaster(short,ITEM *,short,short);
extern ITEM *FindIn(short,ITEM *,short,short);
extern ITEM *NextIn(short,ITEM *,short,short);	
extern SUB *FindSub(ITEM *,short);
extern SUB *NextSub(SUB *,short);
extern SUB *AllocSub(ITEM *,short,short);
extern void FreeSub(ITEM *,SUB *);
extern int Contains(ITEM *,ITEM *);
extern ITEM *FindContains(short,ITEM *,short,short);
extern ITEM *NextContains(short,ITEM *,ITEM *,short,short);
extern long MasterNumber(ITEM *);
extern int ValidItem(ITEM *);		/* Used for a nasty hack in daemons
					   (temporary function) */
extern long CountItems(void);
extern ITEM *GetNextPointer(void);	/* Hacks for DOCLASS */
extern void SetNextPointer(ITEM *);
extern ITEM *FindInByClass(short,ITEM *,short);
extern ITEM *NextInByClass(short,ITEM *,short);	/* Hacked about for DOCLASS */

#ifdef CHECK_ITEM
extern int ICheck(ITEM *,int,char *);
#endif

#ifdef CHECK_TXT
extern int TCheck(TPTR,int,char *);
#endif

/* SYSSUPPORT.C */

extern PLAYER *PlayerOf();
extern OBJECT *ObjectOf();
extern ROOM *RoomOf();
extern USERFLAG *UserFlagOf();
extern CONTAINER *ContainerOf();
extern int UserOf();
extern int IsUser();
extern int IsRoom();
extern int IsPlayer();
extern int IsObject();
extern void SendItemDirect();
extern void SendItem(); /* VARARGS */
extern int IsCalled();
extern void SetName();
extern int ArchWizard();	/* OPTION TO READ FROM CONFIG FILE DESIRABLE */
extern char *NameOf();
extern char *CNameOf();	/* 128 BYTE LIMIT */
extern short LevelOf();
extern void Place();
extern void XPlace();
extern void RemoveUser();
extern void ExitUser();
extern int CountUsers();
extern int IsBlind();
extern int IsDeaf();
extern void ByeBye();
extern void DescribeItem();
extern void DoesAction();	/* VARARGS */
extern void DoesTo();		/* VARARGS */
extern void DoesToPlayer(); 	/* VARARGS */
/*
 *	CMD_LOOK() and these three functions + Broadcast ought to work
 *	on mobiles which are being snooped.
 */
extern ITEM *FindSomething();
extern void SetPrompt();
extern ITEM *ExitOf();
extern ITEM *DoorOf();
extern void Broadcast();
extern void SendEdit();			/* VARARGS */
extern void TimeOut();			/* UNUSED */
extern void SetUserTitle();		/* VARARG */
extern void SetUserPicture();		/* VARARG */
extern int IsLit();			/* Belongs in DarkLight ? */
extern void MemoryAlert();		/* Not dramatic enough */
extern int IsUnique();
extern int ItemNumber();

/* SUBHANDLER.C */

extern int MakeRoom();
extern int UnRoom();
extern int MakeObject();
extern int UnObject();
extern int MakePlayer();
extern int UnPlayer();
extern int MakeGenExit();
extern int UnGenExit();
extern CONDEXIT *MakeCondExit();
extern CONDEXIT *MakeNLCondExit();
extern int UnCondExit();
extern CONDEXIT *FindCondExit();
extern MSGEXIT *MakeMsgExit();
extern MSGEXIT *MakeNLMsgExit();
extern int UnMsgExit();
extern MSGEXIT *FindMsgExit();
extern int AddChain();
extern int AddNLChain();
extern CHAIN *FindChain();
extern int RemoveChain();
extern void SynchChain();
extern CONTAINER *BeContainer();
extern int UnContainer();
extern int GetUserFlag();
extern ITEM *GetUserItem();
extern void SetUserFlag();
extern void SetUserItem();
extern int UnUserFlag();
extern void InitUserFlag();
extern int MakeInherit();
extern int UnInherit();
extern ITEM *Inheritor();
extern INOUTHERE *FindIOH();
extern void KillIOH();
extern INOUTHERE *GetIOH();
extern void SetInMsg();
extern void SetOutMsg();
extern void SetHereMsg();
extern char *GetInMsg();
extern char *GetOutMsg();
extern char *GetHereMsg();
extern void SetUText();
extern TPTR GetUText();

/* ACTIONCODE.C */

extern short ClassMask;
extern LINE *ClassLine;
extern short ClassMode1,ClassMode2;

extern void Act_Get();
extern void Act_Drop();
extern void Act_Wear();
extern void Act_Remove();
extern void Act_Create();
extern void Act_Destroy();
extern void Act_Swap();
extern void Act_Place();
extern void Act_PutIn();
extern void Act_TakeOut();
extern void Act_CopyOF();
extern void Act_CopyFO();
extern void Act_CopyFF();
extern void Act_WhatO();
extern void Act_GetO();
extern void Act_Weigh();
extern void Act_Set();
extern void Act_Clear();
extern void Act_PSet();
extern void Act_PClear();
extern void Act_Let();
extern void Act_Add();
extern void Act_Sub();
extern void Act_Mul();
extern void Act_Div();
extern void Act_Mod();
extern void Act_AddF();
extern void Act_SubF();
extern void Act_MulF();
extern void Act_DivF();
extern void Act_ModF();
extern void Act_Random();
extern void Act_Move();
extern void Act_Goto();
extern void Act_Weight();
extern void Act_Size();
extern void Act_OSet();
extern void Act_OClear();
extern void Act_RSet();
extern void Act_RClear();
extern void Act_CSet();
extern void Act_CClear();
extern void Act_PutBy();
extern void Act_Inc();
extern void Act_Dec();
extern void Act_SetState();
extern void Act_Prompt();
extern void Act_Print();
extern void Act_Score();
extern void Act_Message();
extern void Act_Msg();
extern void Act_ListObj();
extern void Act_ListAt();
extern void Act_Inven();
extern void Act_Desc();
extern void Act_End();
extern void Act_Done();
extern void Act_NotDone();
extern void Act_Ok();
extern void Act_Abort();
extern void Act_Save();
extern void Act_NewText();
extern void Act_Process();
extern void Act_DoClass();
extern void Act_Give();
extern void Act_DoesAction();
extern void Act_DoesTo();
extern void Act_DoesToPlayer();
extern void Act_PObj();
extern void Act_PLoc();
extern void Act_PName();
extern void Act_PCName();
extern void Act_Daemon();
extern void Act_AllDaemon();
extern void Act_HDaemon();
extern void Act_When();
extern void Act_SetName();
extern void Act_Dup();
extern void Act_Points();
extern void Act_Hurt();
extern void Act_Cured();
extern void Act_KillOff();
extern int  Act_If1();	/* Should be conditions !!! */
extern int  Act_If2();
extern void Act_Bug();
extern void Act_Typo();
extern int  Act_IsMe();	/* Should be condition */
extern void Act_Broadcast();
extern int Cnd_IsCalled();	/* Wrong file! */
extern void Act_SetMe();
extern void Act_Pronouns();
extern void Act_Exits();
extern void Act_PWChange();
extern void PWVerify();
extern void PWNew();
extern void PWNewVerify();
extern void Act_Snoop();
extern void Act_UnSnoop();
extern void Act_Debug();
extern void Act_GetScore();
extern void Act_GetStr();
extern void Act_GetLev();
extern void Act_SetScore();
extern void Act_SetStr();
extern void Act_SetLev();
extern void Act_Shell();	/* MACHINE DEPENDANT THROUGHOUT */
extern void Act_TreeDaemon();
extern void Act_ChainDaemon();
extern void Act_Means();
extern void Act_CanGoto();
extern void Act_CanGoBy();
extern void Act_GetIFlag();
extern void Act_SetIFlag();
extern void Act_ClearIFlag();
extern void Act_Parse();
extern void Act_Comment();
extern void Act_ComVocab();
extern void Act_Command();
extern void Act_AutoVerb();
extern int  Cnd_ClassAt();	/* Some migrants who should be in condition */
extern int  Cnd_DupOf();
extern void Act_MasterOf();
extern int Cnd_IfDark();	/* More migrants */
extern void Act_Visibility();
extern void Act_GetParent();
extern void Act_GetNext();
extern void Act_GetChild();
extern void Act_PExit();
extern void Act_SetDesc();
extern void Act_SetLong();
extern void Act_SetShort();
extern void Act_GetLong();
extern void Act_GetShort();
extern void Act_GetDesc();
extern void Act_GetName();
extern void Act_Swat();
extern void Act_Flat();
extern void Act_SetIn();
extern void Act_SetOut();
extern void Act_SetHere();
extern void Act_FindMaster();
extern void Act_NextMaster();
extern void Act_FindIn();
extern void Act_NextIn();	/* Same dangers of moving items as with system.c */
extern void Act_LenText();
extern void Act_Field();
extern void Act_GetUText();
extern void Act_SetUText();
extern void Act_Cat();
extern void Act_Cls();
extern void Act_Become();
extern void Act_Alias();
extern void Act_UnAlias();
extern void Act_Unveil();
extern int  Cnd_SubStr();
extern void Act_GetIn();
extern void Act_GetOut();
extern void Act_GetHere();
extern void Act_Log();
extern void Act_SetClass();
extern void Act_UnSetClass();
extern void Act_BitClear();
extern void Act_BitSet();
extern int  Cnd_BitTest();

/*
 *	Unimplemented or Unused
 */

extern void Act_Distance();
extern void Act_WhichWay();
extern void Act_PutO();
extern void Act_Frig();
extern void Act_NArg();
extern void Act_NeedField();
extern void Act_Mobiles();
extern void Act_Dir();
extern void Act_Rooms();

/*
 *	Proposed Rope Logic
 */

extern void Act_TiedTo();
extern void Act_PlaceRope();
extern void Act_RopePrev();
extern int Cnd_IsRope();
extern int Cnd_IsTied();
extern void Act_RopeNext();
extern void Act_Tie();
extern void Act_Untie();
extern void Act_Join();
extern void Act_CutRope();
extern int Cnd_CanMoveRope();


/* BOOTDAEMON.C */

extern jmp_buf Oops;
extern short SupercedeFlag;
extern void IPCMain();

/* COMSERVER.C */

/*
 *	This module is the machine specific IPC binding
 */

extern int Current_UserList;

extern int WrapUp();
extern int SendBlock();		/* Needs IPC defs to specify args */
/*
 *	Sendblock is the machine specific IPC send function. It is a non-blocking
 *	send passed the arguments defining the user by message PORT *, Where
 *	a PORT * can be defined to suit the ipc. A char pointer to a block, and
 *	its length to send. Errors when writing should cause a system failure
 *	if serious. If a users buffer space is full the function TimeOut should
 *	be called for the user in question.
 */
extern int SendTPacket();
extern int SendNPacket();
extern int GetPacket();
/*
 *	GetPacket is supplied with a PORT * and a packet to read the data into.
 *	It should copy a complete packet into the buffer supplied, remembering
 *	that packets are variable length (length is supplied in send call). 
 *	Serious errors should cause system failure.
 */
extern void Handle_Login();
extern void InterpretPacket();
extern void ProcessPackets();
extern int Handle_Output();
extern int Handle_Command();
extern int Handle_CommForce();

/*
 *	The module uses the remote functions OpenMPort() which should open 
 *	the specified port number/address for read/write, and CloseMPort to
 *	close it. Note that the front end program needs to be able to sense
 *	a CloseMPort.
 */

/* COMMAND_DRIVER.C */

extern void Command_Driver();
extern void PermitInput();
extern char *UserLastLine;
extern void SendUser();			/* VARARGS */
extern int NameGot();
extern int Check_Password();
extern int SetPlayerSex();
extern int CreatePersona();
extern int Run_Command();

/* COMPILETABLE */

extern int RememberToLockItem();
extern void LockLockList();
extern void LoadLineBuffer();
extern int EncodeEntry();
extern int EncodeTable();
extern void WipeLine();
extern void DeleteTable();
extern long PairArg();
extern char *NumText();
extern void Decompress();

/* CONDITIONCODE.C */

extern char *Cnd_Table[];
extern int FindCnd();
extern int Cnd_At();
extern int Cnd_NotAt();
extern int Cnd_Present();
extern int Cnd_Absent();
extern int Cnd_Worn();
extern int Cnd_NotWorn();
extern int Cnd_Carried();
extern int Cnd_NotCarr();
extern int Cnd_IsAt();
extern int Cnd_IsNotAt();
extern int Cnd_IsBy();
extern int Cnd_Zero();
extern int Cnd_NotZero();
extern int Cnd_Eq();
extern int Cnd_NotEq();
extern int Cnd_Gt();
extern int Cnd_Lt();
extern int Cnd_EqF();
extern int Cnd_NeF();
extern int Cnd_LtF();
extern int Cnd_GtF();
extern int Cnd_IsIn();
extern int Cnd_IsNotIn();
extern int Cnd_Adj1();
extern int Cnd_Adj2();
extern int Cnd_Noun1();
extern int Cnd_Noun2();
extern int Cnd_Prep();
extern int Cnd_Chance();
extern int Cnd_IsPlayer();
extern int Cnd_IsUser();
extern int Cnd_IsRoom();
extern int Cnd_IsObject();
extern int Cnd_State();
extern int Cnd_PFlag();
extern int Cnd_OFlag();
extern int Cnd_RFlag();
extern int Cnd_CFlag();
extern int Cnd_CanPut();
extern int Cnd_Level();
extern int Cnd_IfDeaf();
extern int Cnd_IfBlind();
extern int Cnd_Arch();
extern int Cnd_Is();
extern int Cnd_ChanceLev();
extern int Cnd_CanSee();
extern int Cnd_IsClass();

/* CONTAINER.C */

extern int WeighUp();
extern int WeightOf();
extern int SizeContents();
extern int SizeOfRec();
extern int CanPlace();	/* SEE COMMENTS FOR LIMITS */

/* CONTAINERCOMMANDS.C */

extern void Cmd_ContainerShow();
extern void Cmd_SetCFlag();
extern void Cmd_SetVolume();

/* DAEMONS.C */

extern void RunDaemon();
extern void AllDaemon();
extern void HDaemon();
extern void TreeDaemon();
extern void ChainDaemon();

/* DARKLIGHT.C */

extern int IsDarkFor();
extern int RecCheckDark();

/* EDITING.C */

extern void Cmd_Exorcise();
extern void Cmd_Abort();
extern void Cmd_AddWord();
extern int  FindFreeWord();
extern void Cmd_AddVerb();
extern void Cmd_AddNoun();
extern void Cmd_AddAdj();
extern void Cmd_AddPrep();
extern void Cmd_AddPronoun();
extern void Cmd_AddOrdinate();
extern void Cmd_ItemInfo();
extern void Cmd_ListItems();
extern void Cmd_SetState();
extern void Cmd_SetPerception();
extern void Cmd_SetName();
extern void Cmd_NewItem();
extern void Cmd_DelItem();
extern void Cmd_BeRoom();
extern void Cmd_BeObject();
extern void Cmd_BePlayer();
extern void Cmd_BeContainer();
extern void Cmd_UnRoom();
extern void Cmd_UnObject();
extern void Cmd_UnPlayer();
extern void Cmd_UnContainer();
extern void Cmd_SaveUniverse();
extern void Cmd_StatMe();
extern void Cmd_ListWord();
extern void Cmd_DelWord();
extern void Cmd_DelVerb();
extern void Cmd_DelNoun();
extern void Cmd_DelPronoun();
extern void Cmd_DelAdj();
extern void Cmd_DelPrep();
extern void Cmd_DelOrdinate();
extern void Cmd_Rename();
extern void Cmd_Chain();
extern void Cmd_UnChain();
extern void Cmd_UFlagShow();
extern void Cmd_SetUFlag();
extern void Cmd_SetUItem();
extern void Cmd_TestMode();
extern void Cmd_EditMode();
extern void Cmd_FreeMode();
extern void Cmd_ShowFlag();
extern void Cmd_SetFlag();
extern void Cmd_SaveGame();
extern void Cmd_LoadGame();
extern void Cmd_Share();
extern void Cmd_UnShare();
extern void Cmd_Status();
extern void Cmd_Which();

/* EXITLOGIC.C */

extern int TestCondExit();

/* FLAGCONTROLLER.C */

extern char *FlagName[];
extern int GetFlagByName();
extern void SetFlagName();
extern char *GetFlagName();
extern void Cmd_NameFlag();
extern void Cmd_UnNameFlag();
extern void Cmd_ListFlag();

/* GENERALCOMMANDS.C */

extern void Cmd_Invisible();
extern void Cmd_Visible();
extern void Cmd_Say();
extern void Cmd_Place();

/* INSANDOUTS.C */

extern char *ExitName();
extern void Cmd_NewExit();
extern void Cmd_DelExit();
extern void Cmd_MsgExit();
extern void Cmd_MoveDirn();
extern void Cmd_Exits();
extern int  CanGoto();
extern void Act_WhereTo();
extern void Act_DoorExit();
extern int BackExit();

/* OBJECTCOMMANDS.C */

extern void Cmd_ObjectShow();
extern void Cmd_SetOFlag();
extern void Cmd_SetDesc();
extern void Cmd_SetOSize();
extern void Cmd_SetOWeight();
extern void Cmd_SetActor();
extern void Cmd_SetAction();

/* OBJECTEDIT.C */

extern void Cmd_ObjEdit();
extern void Objedit_1();
extern void Objedit_2();
extern void Objedit_3();
extern void Objedit_4();
extern void Objedit_5();
extern void Objedit_6();
extern void Objedit_7();

/* PLAYERCOMMANDS.C */

extern void Cmd_PlayerShow();
extern void Cmd_SetPFlag();
extern void Cmd_SetPSize();
extern void Cmd_SetPWeight();
extern void Cmd_SetPStrength();
extern void Cmd_SetPLevel();
extern void Cmd_SetPScore();
extern void Cmd_Users();

/* ROOMCOMMANDS.C */

extern void Cmd_SetShort();
extern void Cmd_ShowRoom();
extern void Cmd_SetRFlag();
extern void Cmd_SetLong();
extern void Cmd_Look();
extern void Cmd_Goto();
extern void Cmd_Brief();
extern void Cmd_Verbose();
extern void Cmd_SetPicture();

/* SAVELOAD.C */

extern short *SaveAction();
extern short *LoadAction();
extern void LoadLine();
extern void SaveLine();
extern void LoadTable();
extern void SaveTable();
extern void LoadAllTables();
extern void SaveAllTable();
extern int SaveSystem();
extern int LoadSystem();
extern ITEM *ItemFind();

/* SNOOP.C */

extern int StartSnoop();
extern void StopSnoop();
extern void StopSnoopOn();
extern void StopAllSnoops();
extern void StopSnoopsOn();
extern void SnoopCheckString();
extern int SnoopCheckRec();

/* TABLECOMMANDS.C */

extern void Cmd_ListTables();
extern void Cmd_AddTable();
extern void Cmd_DeleteTable();
extern void Cmd_NewTable();
extern void Cmd_ListTables();

/* TABLEDRIVER.C */

extern TABLE *TableList;
extern TPTR TxtArg;
extern TPTR TxtArg2;
extern char TxBuf[];
extern int System_Debug;

extern void WipeFlags();
extern int GetFlag();
extern void SetFlag();
extern TXT DummyTxt;
extern ITEM DummyItem;
extern LINE *CurrentLine;
extern TABLE *CurrentTable;
extern ITEM *Item1,*Item2,*Debugger;
extern int Noun1,Adj1,Noun2,Adj2,Verb,Prep,Ord1,Ord2;
extern short Traf[4];

extern ITEM *Me();
extern void SetMe();
extern ITEM *Actor();
extern int ArgNum();
extern int ArgWord();
extern ITEM *ArgItem();
extern TPTR ArgText();
extern void ParseArgs();
extern int ArgMatch();
extern int ExecBackground();
extern int ExecTable();
extern int RunLine();
extern ITEM *FindAnItem();
extern TABLE *FindTable();
extern TABLE *NewTable();
extern LINE *NewLine();
extern LINE *FindLine();
extern int DeleteLine();
extern int UserAction();
extern int UserDaemon();
extern int RandPerc();

/* TABLEEDITING.C */

extern void Cmd_EditTable();
extern void Tbl_Quit();
extern void Tbl_Goto();
extern void Tbl_Find();
extern void Tbl_Top();
extern long CountEntries();
extern void Tbl_Bottom();
extern void Tbl_Next();
extern void Tbl_Previous();
extern void Tbl_List();
extern void Tbl_Insert();
extern void Tbl_Edit();
extern void Tbl_EditLine();
extern void Tbl_DeleteLine();
extern void Tbl_Driver();
extern void OutLineBlock();
extern void Cmd_SaveTable();

/* TIMESCHEDULER.C */

extern void AddEvent();
extern void Scheduler();
extern void KillEventQueue();
extern void WipeEventQueue();

/* USERFILE.C */

extern int WriteRecord();
extern int ReadRecord();
extern int FindRecord();
extern FILE *OpenUAF();
extern void CloseUAF();
extern int LoadPersona();
extern int SavePersona();
extern int FindFreeRecord();
extern int SaveNewPersona();

/* UTILCOMMAND.C */

extern void Cmd_DoorPair();
extern void Cmd_ShowAllObjects();
extern void Cmd_ShowAllRooms();
extern void Cmd_ShowAllPlayers();
extern int ShowItemData();

/* CLASS.C */

extern void SetClassTxt();
extern void SetClassName();
extern TXT *GetClassTxt();
extern char *GetClassName();
extern short WhichClass();
extern void ClassDescStr();
extern void Cmd_ListClass();
extern void Cmd_NameClass();
extern void Cmd_SetClass();
extern void Cmd_UnsetClass();

/* DUPLICATOR.C */

extern int Clone_Room();
extern int Clone_Object();
extern int Clone_Player();
extern int Clone_GenExit();
extern int Clone_MsgExit();
extern int Clone_Chain();
extern int Clone_Container();
extern int Clone_UserFlag();
extern int Clone_Inherit();
extern ITEM *Clone_Item();
extern int Duped();
extern int Disintegrate();
extern void DisintegrateAll();

/* MAIN.C */

extern void main();

extern short post_boot;

/* PARSER.C */

extern WLIST *WordList;
extern PCONTEXT ParserData[];
extern short ParsingPersona;

extern void SetPersona();
extern PCONTEXT *GetContext();
extern void ProLoad();
extern void SetItData();
extern long WordMemory;
extern void AddWord();
extern int FreeWord();
extern char *FindWText();
extern WLIST *FindInList();
extern char *BreakWord();
extern char *GetRestOfInput();
extern char *FNxPhrs();
extern char *WordPtr;
extern char WordBuffer[];
extern WLIST *GetWord();
extern int GetOrd();
extern char *GetParsedWord();
extern void GetAll();
extern int GetNumber();
extern int GetThing();
extern void SkipPrep();
extern int GetPrep();
extern int GetVerb();
extern char *NextPhrase();

/* FLAGNAME.C */

extern char *PBitNames[];
extern char *OBitNames[];
extern char *RBitNames[];
extern char *CBitNames[];
extern int FindRBit();
extern int FindPBit();
extern int FindOBit();
extern int FindCBit();
extern char *RBitName();
extern char *OBitName();
extern char *PBitName();
extern char *CBitName();
extern void Cmd_RBitName();
extern void Cmd_OBitName();
extern void Cmd_PBitName();
extern void Cmd_CBitName();
extern void Cmd_ListRBits();
extern void Cmd_ListOBits();
extern void Cmd_ListPBits();
extern void Cmd_ListCBits();

/* _THE END_ */
