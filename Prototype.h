/*
 *	If you have full ANSI prototyping this file will help keep everything
 *	as the right form of arguments. 
 */

/*	SYSTEM.C	*/

extern void noreturn ErrFunc(char *,char *,char *,int,char *);
extern void Log(char *fmt, ...);
extern char *AllocFunc(int,char *,char *,int,char *);
extern TPTR AllocText(char *);
extern TPTR AllocComment(char *);
extern TPTR QuickAllocText(char *);
extern long CountTexts(void);
extern long TextSize(void);
extern long TextNumber(TPTR);	/* Wants speeding up */
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
extern ITEM *NextIn(short,ITEM *,short,short);	/* Items must not have moved */
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
extern void LineDump(void);

#ifdef CHECK_ITEM
extern int ICheck(ITEM *,int,char *);
#endif

#ifdef CHECK_TXT
extern int TCheck(TPTR,int,char *);
#endif

/* SYSSUPPORT.C */

extern PLAYER *PlayerOf(ITEM *);
extern OBJECT *ObjectOf(ITEM *);
extern ROOM *RoomOf(ITEM *);
extern USERFLAG *UserFlagOf(ITEM *);
extern USERFLAG *UserFlag2Of(ITEM *);
extern CONTAINER *ContainerOf(ITEM *);
extern int UserOf(ITEM *);
extern int IsUser(ITEM *);
extern int IsRoom(ITEM *);
extern int IsPlayer(ITEM *);
extern int IsObject(ITEM *);
extern void SendItemDirect(ITEM *,char *);
extern void SendItem(); /* VARARGS */
extern int IsCalled(ITEM *,char *);
extern void SetName(ITEM *,char *);
extern int ArchWizard(ITEM *);	/* OPTION TO READ FROM CONFIG FILE DESIRABLE */
extern char *NameOf(ITEM *);
extern char *CNameOf(ITEM *);	/* 128 BYTE LIMIT */
extern short LevelOf(ITEM *);
extern void Place(ITEM *,ITEM *);
extern void XPlace(ITEM *,ITEM *);
extern void RemoveUser(unsigned int);
extern void ExitUser(unsigned int);
extern int CountUsers(void);
extern int IsBlind(ITEM *);
extern int IsDeaf(ITEM *);
extern void ByeBye(ITEM *,char *);
extern void DescribeItem(ITEM *,ITEM *);
extern void DoesAction();	/* VARARGS */
extern void DoesTo();		/* VARARGS */
extern void DoesToPlayer(); 	/* VARARGS */
/*
 *	CMD_LOOK() and these three functions + Broadcast ought to work
 *	on mobiles which are being snooped.
 */
extern ITEM *FindSomething(ITEM *,ITEM *);
extern void SetPrompt(ITEM *,char *);
extern ITEM *ExitOf(ITEM *,unsigned int);
extern ITEM *DoorOf(ITEM *,unsigned int);
extern void Broadcast(char *,int);
extern void SendEdit();			/* VARARGS */
extern void TimeOut(int);		/* UNUSED */
extern void SetUserTitle();		/* VARARG */
extern int IsLit(ITEM *);		/* Belongs in DarkLight ? */
extern int IsUnique(short,short,short);
extern int ItemNumber(short,ITEM *);

/* SUBHANDLER.C */

extern int MakeRoom(ITEM *);
extern int UnRoom(ITEM *);
extern int MakeObject(ITEM *);
extern int UnObject(ITEM *);
extern int MakePlayer(ITEM *);
extern int UnPlayer(ITEM *);
extern int MakeGenExit(ITEM *);
extern int UnGenExit(ITEM *);
extern CONDEXIT *MakeCondExit(ITEM *,ITEM *,short,short);
extern CONDEXIT *MakeNLCondExit(ITEM *,ITEM *,short,short);
extern int UnCondExit(ITEM *,CONDEXIT *);
extern CONDEXIT *FindCondExit(ITEM *,short);
extern MSGEXIT *MakeMsgExit(ITEM *,ITEM *,short,char *);
extern MSGEXIT *MakeNLMsgExit(ITEM *,ITEM *,short,char *);
extern int UnMsgExit(ITEM *,MSGEXIT *);
extern MSGEXIT *FindMsgExit(ITEM *,short);
extern int AddChain(ITEM *,ITEM *);
extern int AddNLChain(ITEM *,ITEM *);
extern CHAIN *FindChain(ITEM *,ITEM *);
extern int RemoveChain(ITEM *,ITEM *);
extern void SynchChain(ITEM *);
extern CONTAINER *BeContainer(ITEM *);
extern int UnContainer(ITEM *);
extern int GetUserFlag(ITEM *,int);
extern ITEM *GetUserItem(ITEM *,int);
extern void SetUserFlag(ITEM *,int,int);
extern void SetUserItem(ITEM *,int,ITEM *);
extern int UnUserFlag(ITEM *);
extern int UnUserBlock(USERFLAG *,ITEM *);
extern void InitUserFlag(USERFLAG *);
extern int MakeInherit(ITEM *,ITEM *);
extern int UnInherit(ITEM *);
extern ITEM *Inheritor(ITEM *);
extern void UnUserText(ITEM *);
extern void SetUText(ITEM *,int,TPTR);
extern TPTR GetUText(ITEM *,int);
extern INOUTHERE *FindIOH(ITEM *);
extern void KillIOH(ITEM *);
extern INOUTHERE *GetIOH(ITEM *);
extern void SetInMsg(ITEM *,char *);
extern void SetOutMsg(ITEM *,char *);
extern void SetHereMsg(ITEM *,char *);
extern char *GetInMsg(ITEM *);
extern char *GetOutMsg(ITEM *);
extern char *GetHereMsg(ITEM *);

/* ACTIONCODE.C */

extern short ClassMask;
extern LINE *ClassLine;
extern short ClassMode1,ClassMode2;

extern void Act_Get(void);
extern void Act_Drop(void);
extern void Act_Wear(void);
extern void Act_Remove(void);
extern void Act_Create(void);
extern void Act_Destroy(void);
extern void Act_Swap(void);
extern void Act_Place(void);
extern void Act_PutIn(void);
extern void Act_TakeOut(void);
extern void Act_CopyOF(void);
extern void Act_CopyFO(void);
extern void Act_CopyFF(void);
extern void Act_WhatO(void);
extern void Act_GetO(void);
extern void Act_Weigh(void);
extern void Act_Set(void);
extern void Act_Clear(void);
extern void Act_PSet(void);
extern void Act_PClear(void);
extern void Act_Let(void);
extern void Act_Add(void);
extern void Act_Sub(void);
extern void Act_Mul(void);
extern void Act_Div(void);
extern void Act_Mod(void);
extern void Act_AddF(void);
extern void Act_SubF(void);
extern void Act_MulF(void);
extern void Act_DivF(void);
extern void Act_ModF(void);
extern void Act_Random(void);
extern void Act_Move(void);
extern void Act_Goto(void);
extern void Act_Weight(void);
extern void Act_Size(void);
extern void Act_OSet(void);
extern void Act_OClear(void);
extern void Act_RSet(void);
extern void Act_RClear(void);
extern void Act_CSet(void);
extern void Act_CClear(void);
extern void Act_PutBy(void);
extern void Act_Inc(void);
extern void Act_Dec(void);
extern void Act_SetState(void);
extern void Act_Prompt(void);
extern void Act_Print(void);
extern void Act_Score(void);
extern void Act_Message(void);
extern void Act_Msg(void);
extern void Act_ListObj(void);
extern void Act_ListAt(void);
extern void Act_Inven(void);
extern void Act_Desc(void);
extern void Act_End(void);
extern void Act_Done(void);
extern void Act_NotDone(void);
extern void Act_Ok(void);
extern void Act_Abort(void);
extern void Act_Save(void);
extern void Act_NewText(void);
extern void Act_Process(void);
extern void Act_DoClass(void);
extern void Act_Give(void);
extern void Act_DoesAction(void);
extern void Act_DoesTo(void);
extern void Act_DoesToPlayer(void);
extern void Act_PObj(void);
extern void Act_PLoc(void);
extern void Act_PName(void);
extern void Act_PCName(void);
extern void Act_Daemon(void);
extern void Act_AllDaemon(void);
extern void Act_HDaemon(void);
extern void Act_When(void);
extern void Act_SetName(void);
extern void Act_Dup(void);
extern void Act_Points(void);
extern void Act_Hurt(void);
extern void Act_Cured(void);
extern void Act_KillOff(void);
extern int  Act_If1(void);	/* Should be conditions !!! */
extern int  Act_If2(void);
extern void Act_Bug(void);
extern void Act_Typo(void);
extern int  Act_IsMe(void);	/* Should be condition */
extern void Act_Broadcast(void);
extern int Cnd_IsCalled(void);	/* Wrong file! */
extern void Act_SetMe(void);
extern void Act_Pronouns(void);
extern void Act_Exits(void);
extern void Act_PWChange(void);
extern void PWVerify(short,char *);
extern void PWNew(short,char *);
extern void PWNewVerify(short,char *);
extern void Act_Snoop(void);
extern void Act_UnSnoop(void);
extern void Act_Debug(void);
extern void Act_GetScore(void);
extern void Act_GetStr(void);
extern void Act_GetLev(void);
extern void Act_SetScore(void);
extern void Act_SetStr(void);
extern void Act_SetLev(void);
extern void Act_Shell(void);	/* MACHINE DEPENDANT THROUGHOUT */
extern void Act_TreeDaemon(void);
extern void Act_ChainDaemon(void);
extern void Act_Means(void);
extern void Act_CanGoto(void);
extern void Act_CanGoBy(void);
extern void Act_GetIFlag(void);
extern void Act_SetIFlag(void);
extern void Act_ClearIFlag(void);
extern void Act_Parse(void);
extern void Act_Comment(void);
extern void Act_ComVocab(void);
extern void Act_Command(void);
extern void Act_AutoVerb(void);
extern int  Cnd_ClassAt(void);	/* Some migrants who should be in condition */
extern int  Cnd_DupOf(void);
extern void Act_MasterOf(void);
extern int Cnd_IfDark(void);	/* More migrants */
extern void Act_Visibility(void);
extern void Act_GetParent(void);
extern void Act_GetNext(void);
extern void Act_GetChild(void);
extern void Act_PExit(void);
extern void Act_SetDesc(void);
extern void Act_SetLong(void);
extern void Act_SetShort(void);
extern void Act_GetLong(void);
extern void Act_GetShort(void);
extern void Act_GetDesc(void);
extern void Act_GetName(void);
extern void Act_Swat(void);
extern void Act_Flat(void);
extern void Act_SetIn(void);
extern void Act_SetOut(void);
extern void Act_SetHere(void);
extern void Act_FindMaster(void);
extern void Act_NextMaster(void);
extern void Act_FindIn(void);
extern void Act_NextIn(void);	/* Same dangers of moving items as with system.c */
extern void Act_LenText(void);
extern void Act_Field(void);
extern void Act_GetUText(void);
extern void Act_SetUText(void);
extern void Act_Cat(void);
extern void Act_Cls(void);
extern void Act_Become(void);
extern void Act_Alias(void);
extern void Act_UnAlias(void);
extern void Act_Unveil(void);
extern int  Cnd_SubStr(void);
extern void Act_GetIn(void);
extern void Act_GetOut(void);
extern void Act_GetHere(void);
extern void Act_Log(void);
extern void Act_SetClass(void);
extern void Act_UnSetClass(void);
extern void Act_BitClear(void);
extern void Act_BitSet(void);
extern int  Cnd_BitTest(void);
extern void Act_SPrint(void);
extern void Act_User(void);
extern void Act_Cls(void);
extern void Act_GetVis(void);

/*
 *	Unimplemented or Unused
 */

extern void Act_Distance(void);
extern void Act_WhichWay(void);
extern void Act_PutO(void);
extern void Act_Frig(void);
extern void Act_NArg(void);
extern void Act_NeedField(void);
extern void Act_Mobiles(void);
extern void Act_Dir(void);
extern void Act_Rooms(void);

/*
 *	Proposed Rope Logic
 */

extern void Act_TiedTo(void);
extern void Act_PlaceRope(void);
extern void Act_RopePrev(void);
extern int Cnd_IsRope(void);
extern int Cnd_IsTied(void);
extern void Act_RopeNext(void);
extern void Act_Tie(void);
extern void Act_Untie(void);
extern void Act_Join(void);
extern void Act_CutRope(void);
extern int Cnd_CanMoveRope(void);

extern int Cnd_IsNotBy(void);
extern void Act_MessageTo(void);
extern void Act_MsgTo(void);

/* BOOTDAEMON.C */

extern jmp_buf Oops;
extern short SupercedeFlag;
extern short SupercedeFlag;
extern void IPCMain(void);

/* BSX.c */

BSXImage *BSXAllocate(char *,int);
BSXImage *BSXFindFirst(void);
BSXImage *BSXFindNext(BSXImage *);
BSXImage *BSXFind(char *);
void BSXDelete(BSXImage *);
int BSXEncodePair(char *);
void BSXDecodePair(unsigned char,char *);
BSXImage *BSXLoadImage(char *,char *);
void Cmd_DeleteBSX(ITEM *);
void Cmd_LoadBSX(ITEM *);
void Cmd_ListBSX(ITEM *);
void Cmd_ShowBSX(ITEM *);
void BSXDecompSend(int, BSXImage *);
void Handle_BSXPacket(int, char *);
void Act_BSXScene(void);
void Act_BSXObject(void);

/* COMSERVER.C */

/*
 *	This module is the machine specific IPC binding
 */

extern int Current_UserList;
extern short LineFaults(void);
extern void FixLineFaults(void);

extern int WrapUp(int);
extern int SendBlock(PORT *, COMTEXT *, int);
/*
 *	Sendblock is the machine specific IPC send function. It is a non-blocking
 *	send passed the arguments defining the user by message PORT *, Where
 *	a PORT * can be defined to suit the ipc. A char pointer to a block, and
 *	its length to send. Errors when writing should cause a system failure
 *	if serious. If a users buffer space is full the function TimeOut should
 *	be called for the user in question.
 */
extern int SendTPacket(PORT *, short, char *);
extern int SendNPacket(PORT *, short, short, short, short, short);
extern int GetPacket(PORT *, COMTEXT *);
/*
 *	GetPacket is supplied with a PORT * and a packet to read the data into.
 *	It should copy a complete packet into the buffer supplied, remembering
 *	that packets are variable length (length is supplied in send call). 
 *	Serious errors should cause system failure.
 */
extern void Handle_Login(char *);
extern void InterpretPacket();
extern void ProcessPackets(void);
extern int Handle_Output(char *);
extern int Handle_Command(int,char *);
extern int Handle_CommForce(int,char *);

/*
 *	The module uses the remote functions OpenMPort() which should open 
 *	the specified port number/address for read/write, and CloseMPort to
 *	close it. Note that the front end program needs to be able to sense
 *	a CloseMPort.
 */
extern int CloseMPort(PORT *);

/* COMMAND_DRIVER.C */

extern void Command_Driver(int,short,char *);
extern void PermitInput(int);
extern char *UserLastLine;
extern void BufOut(int, char *);
extern void SendUser(int, char *, ...);	
extern int Name_Got(int,char *);
extern int Check_Password(int,char *,int);
extern int SetPlayerSex(int,char *);
extern int SetPlayerEmail(int, char *);
extern int CreatePersona(int,char *);
extern int Run_Command(int,char *);

/* COMPILETABLE */

extern int RememberToLockItem(ITEM *);
extern void LockLockList(void);
extern void LoadLineBuffer(char *);
extern int EncodeEntry(ITEM *,LINE *);
extern int EncodeTable(ITEM *,TABLE *,FILE *);
extern void WipeLine(LINE *);
extern void DeleteTable(TABLE *);
extern void FreeTableHeader(TABLE *);
extern unsigned long PairArg(unsigned short *);
extern char *NumText(int);
extern void Decompress(LINE *,char *);

/* CONDITIONCODE.C */

extern char *Cnd_Table[];
extern int FindCnd(char *);
extern int Cnd_At(void);
extern int Cnd_NotAt(void);
extern int Cnd_Present(void);
extern int Cnd_Absent(void);
extern int Cnd_Worn(void);
extern int Cnd_NotWorn(void);
extern int Cnd_Carried(void);
extern int Cnd_NotCarr(void);
extern int Cnd_IsAt(void);
extern int Cnd_IsNotAt(void);
extern int Cnd_IsBy(void);
extern int Cnd_Zero(void);
extern int Cnd_NotZero(void);
extern int Cnd_Eq(void);
extern int Cnd_NotEq(void);
extern int Cnd_Gt(void);
extern int Cnd_Lt(void);
extern int Cnd_EqF(void);
extern int Cnd_NeF(void);
extern int Cnd_LtF(void);
extern int Cnd_GtF(void);
extern int Cnd_IsIn(void);
extern int Cnd_IsNotIn(void);
extern int Cnd_Adj1(void);
extern int Cnd_Adj2(void);
extern int Cnd_Noun1(void);
extern int Cnd_Noun2(void);
extern int Cnd_Prep(void);
extern int Cnd_Chance(void);
extern int Cnd_IsPlayer(void);
extern int Cnd_IsUser(void);
extern int Cnd_IsRoom(void);
extern int Cnd_IsObject(void);
extern int Cnd_State(void);
extern int Cnd_PFlag(void);
extern int Cnd_OFlag(void);
extern int Cnd_RFlag(void);
extern int Cnd_CFlag(void);
extern int Cnd_CanPut(void);
extern int Cnd_Level(void);
extern int Cnd_IfDeaf(void);
extern int Cnd_IfBlind(void);
extern int Cnd_Arch(void);
extern int Cnd_Is(void);
extern int Cnd_ChanceLev(void);
extern int Cnd_CanSee(void);
extern int Cnd_IsClass(void);

/* CONTAINER.C */

extern int WeighUp(ITEM *);
extern int WeightOf(ITEM *);
extern int SizeContents(ITEM *);
extern int SizeOfRec(ITEM *,int);
extern int CanPlace(ITEM *,ITEM *);	/* SEE COMMENTS FOR LIMITS */

/* CONTAINERCOMMANDS.C */

extern void Cmd_ContainerShow(ITEM *);
extern void Cmd_SetCFlag(ITEM *);
extern void Cmd_SetVolume(ITEM *);

/* DAEMONS.C */

extern void RunDaemon(ITEM *,int,int,int);
extern void AllDaemon(int,int,int);
extern void HDaemon(ITEM *,int,int,int);
extern void TreeDaemon(ITEM *,int,int,int);
extern void ChainDaemon(ITEM *,int,int,int);
extern void CDaemon(ITEM *,int,int,int);

/* DARKLIGHT.C */

extern int IsDarkFor(ITEM *);
extern int RecCheckDark(ITEM *,short);

/* EDITING.C */

extern char CmdBuffer[512];
extern void Cmd_Exorcise(ITEM *);
extern void Cmd_Abort(ITEM *);
extern void Cmd_AddWord(ITEM *,short);
extern int  FindFreeWord(short);	/* MUCH TOO SLOW */
extern void Cmd_AddVerb(ITEM *);
extern void Cmd_AddNoun(ITEM *);
extern void Cmd_AddAdj(ITEM *);
extern void Cmd_AddPrep(ITEM *);
extern void Cmd_AddPronoun(ITEM *);
extern void Cmd_AddOrdinate(ITEM *);
extern char *TabName(int);
extern void Cmd_ItemInfo(ITEM *);
extern void Cmd_ListItems(ITEM *);
extern void Cmd_SetState(ITEM *);
extern void Cmd_SetPerception(ITEM *);
extern void Cmd_SetName(ITEM *);
extern void Cmd_NewItem(ITEM *);
extern void Cmd_DelItem(ITEM *);
extern void Cmd_BeRoom(ITEM *);
extern void Cmd_BeObject(ITEM *);
extern void Cmd_BePlayer(ITEM *);
extern void Cmd_BeContainer(ITEM *);
extern void Cmd_UnRoom(ITEM *);
extern void Cmd_UnObject(ITEM *);
extern void Cmd_UnPlayer(ITEM *);
extern void Cmd_UnContainer(ITEM *);
extern void Cmd_SaveUniverse(ITEM *);
extern void Cmd_StatMe(ITEM *);
extern void Cmd_ListWord(ITEM *);
extern void Cmd_DelWord(ITEM *,short);
extern void Cmd_DelVerb(ITEM *);
extern void Cmd_DelNoun(ITEM *);
extern void Cmd_DelPronoun(ITEM *);
extern void Cmd_DelAdj(ITEM *);
extern void Cmd_DelPrep(ITEM *);
extern void Cmd_DelOrdinate(ITEM *);
extern void Cmd_Rename(ITEM *);
extern void Cmd_Chain(ITEM *);
extern void Cmd_UnChain(ITEM *);
extern void Cmd_UFlagShow(ITEM *);
extern void Cmd_SetUFlag(ITEM *);
extern void Cmd_SetUItem(ITEM *);
extern void Cmd_ShowFlag(ITEM *);
extern void Cmd_SetFlag(ITEM *);
extern void Cmd_Share(ITEM *);
extern void Cmd_UnShare(ITEM *);
extern void Cmd_TrackFlag(ITEM *);
extern void Cmd_UnTrackFlag(ITEM *);
extern void Cmd_ListTrack(ITEM *);
extern void Cmd_Status(ITEM *);
extern void Cmd_Which(ITEM *);
extern void Cmd_Debugger(ITEM *);
extern void Cmd_FindItem(ITEM *);
extern void Cmd_FindFlag(ITEM *);
extern void Cmd_Which(ITEM *);
extern void Cmd_ShowSuperClass(ITEM *);
extern void Cmd_SetSuperClass(ITEM *);
 
/* EXITLOGIC.C */

extern int TestCondExit(CONDEXIT *);

/* FLAGCONTROLLER.C */

extern char *FlagName[];
extern int GetFlagByName(char *);
extern void SetFlagName(short,char *);
extern char *GetFlagName(short);
extern void Cmd_NameFlag(ITEM *);
extern void Cmd_UnNameFlag(ITEM *);
extern void Cmd_ListFlags(ITEM *);

/* GENERALCOMMANDS.C */

extern void Cmd_Invisible(ITEM *);
extern void Cmd_Visible(ITEM *);
extern void Cmd_Say(ITEM *);
extern void Cmd_Place(ITEM *);

/* INSANDOUTS.C */

extern char *ExitName(int);
extern void Cmd_NewExit(ITEM *);
extern void Cmd_DelExit(ITEM *);
extern void Cmd_MsgExit(ITEM *);
extern void Cmd_CondExit(ITEM *);
extern void Cmd_MoveDirn(ITEM *,int);
extern void Cmd_Exits(ITEM *,ITEM *);
extern int  CanGoto(ITEM *,ITEM *);
extern void Act_WhereTo(void);
extern void Act_DoorExit(void);
extern int BackExit(int);

/* OBJECTCOMMANDS.C */

extern void Cmd_ObjectShow(ITEM *);
extern void Cmd_SetOFlag(ITEM *);
extern void Cmd_SetDesc(ITEM *);
extern void Cmd_SetOSize(ITEM *);
extern void Cmd_SetOWeight(ITEM *);
extern void Cmd_SetActor(ITEM *);
extern void Cmd_SetAction(ITEM *);

/* OBJECTEDIT.C */

extern void Cmd_ObjEdit(ITEM *);
extern void Objedit_1(short,char *);
extern void Objedit_2(short,char *);
extern void Objedit_3(short,char *);
extern void Objedit_4(short,char *);
extern void Objedit_5(short,char *);
extern void Objedit_6(short,char *);
extern void Objedit_7(short,char *);
extern void Objedit_8(short,char *);

/* PLAYERCOMMANDS.C */

extern void Cmd_PlayerShow(ITEM *);
extern void Cmd_SetPFlag(ITEM *);
extern void Cmd_SetPSize(ITEM *);
extern void Cmd_SetPWeight(ITEM *);
extern void Cmd_SetPStrength(ITEM *);
extern void Cmd_SetPLevel(ITEM *);
extern void Cmd_SetPScore(ITEM *);
extern void Cmd_Users(ITEM *);

/* ROOMCOMMANDS.C */

extern void Cmd_SetShort(ITEM *);
extern void Cmd_ShowRoom(ITEM *);
extern void Cmd_SetRFlag(ITEM *);
extern void Cmd_SetLong(ITEM *);
extern void Cmd_Look(ITEM *);
extern void Cmd_Goto(ITEM *);
extern void Cmd_Brief(ITEM *);
extern void Cmd_Verbose(ITEM *);
extern void Cmd_SetPicture(ITEM *);

/* SAVELOAD.C */

extern void SaveAllTable(FILE *);
extern int SaveSystem(char *);
extern int LoadSystem(char *);

/* SNOOP.C */

extern int StartSnoop(ITEM *,ITEM *,short);
extern void StopSnoop(ITEM *,SNOOP *);
extern void StopSnoopOn(ITEM *,ITEM *);
extern void StopAllSnoops(ITEM *);
extern void StopSnoopsOn(ITEM *);
extern void SnoopCheckString(ITEM *,char *);
extern int SnoopCheckRec(ITEM *,char *,short);

/* TABLECOMMANDS.C */

extern void Cmd_ListTables(ITEM *);
extern void Cmd_AddTable(ITEM *);
extern void Cmd_DeleteTable(ITEM *);
extern void Cmd_NewTable(ITEM *);
extern void Cmd_ListTables(ITEM *);
extern void Cmd_ListTables(ITEM *);
extern void Cmd_NameTable(ITEM *);

/* TABLEDRIVER.C */

extern TABLE *TableList;
extern TPTR TxtArg;
extern TPTR TxtArg2;
extern char TxBuf[];
extern int System_Debug;
extern short Traf[4];
extern LINE *CurrentLine;

extern TXT DummyTxt;
extern ITEM DummyItem;
extern TABLE *CurrentTable;
extern ITEM *Item1,*Item2,*Debugger;
extern int Noun1,Adj1,Noun2,Adj2,Verb,Prep,Ord1,Ord2;
extern short Traf[4];

extern void WipeFlags(void);
extern int GetFlag(int);
extern void SetFlag(int,int);
extern void PCurrentLine(void);
extern void FPCurrentLine(void);
extern ITEM *Me(void);
extern void SetMe(ITEM *);
extern ITEM *Actor(void);
extern int ArgNum(void);
extern unsigned int UArgNum(void);
extern int ArgWord(void);
extern ITEM *ArgItem(void);
extern TPTR ArgText(void);
extern void ParseArgs(int);
extern int ArgMatch(LINE *);
extern int ExecBackground(TABLE *,ITEM *);
extern int ExecTable(TABLE *);
extern int RunLine(LINE *);
extern ITEM *FindAnItem(int,int,int);
extern int FindTableByName(char *);
extern TABLE *FindTable(int);
extern TABLE *NewTable(int,char *);
extern LINE *NewLine(TABLE *,int);
extern LINE *FindLine(TABLE *,int);
extern int DeleteLine(TABLE *,int);
extern int WipeDeleteLine(TABLE *,int);
extern int UserAction(ITEM *,int);
extern int UserDaemon(ITEM *);
extern int RandPerc(void);

/* TABLEEDITING.C */

extern char *EditingBuffers[];

extern void Cmd_EditTable(ITEM *);
extern void Cmd_EditOTable(ITEM *);
extern void Cmd_EditDTable(ITEM *);
extern void Cmd_EditSTable(ITEM *);
extern void Tbl_Quit(int,char *);
extern void Tbl_Goto(int,char *);
extern void Tbl_Find(int,char *);
extern void Tbl_Top(int,char *);
extern long CountEntries(TABLE *);
extern void Tbl_Bottom(int,char *);
extern void Tbl_Next(int,char *);
extern void Tbl_Previous(int,char *);
extern void Tbl_List(int,char *);
extern void Tbl_Insert(int,char *);
extern void Tbl_Edit(int,char *);
extern void Tbl_EditLine(int,char *);
extern void Tbl_DeleteLine(int,char *);
extern void Tbl_Driver(int,char *);
extern void OutLineBlock(FILE *,char *);
extern void Cmd_SaveTable(ITEM *);

/* TIMESCHEDULER.C */

extern short Sched_Lock;

extern long CountSchedules(void);
extern void AddEvent(unsigned long,short);
extern void Scheduler(void);
extern void KillEventQueue(ITEM *);
extern void WipeEventQueue(void);

/* USERFILE.C */

extern void SwapUFFToHost(UFF *);
extern void SwapHostToNeutral(UFF *);
extern int WriteRecord(FILE *,UFF *);
extern int ReadRecord(FILE *,UFF *);
extern int FindRecord(FILE *,int);
extern FILE *OpenUAF(void);
extern void CloseUAF(FILE *);
extern int LoadPersona(char *,UFF *);
extern int SavePersona(UFF *,int);
extern int FindFreeRecord(void);
extern int SaveNewPersona(UFF *);

/* UTILCOMMAND.C */

extern void Cmd_DoorPair(ITEM *);
extern void Cmd_ShowAllObjects(ITEM *);
extern void Cmd_ShowAllRooms(ITEM *);
extern void Cmd_ShowAllPlayers(ITEM *);
extern int ShowItemData(ITEM *,ITEM *);

/* CLASS.C */

extern void SetClassTxt(short,TXT *);
extern void SetClassName(short,char *);
extern TXT *GetClassTxt(short);
extern char *GetClassName(short);
extern short WhichClass(char *);
extern void ClassDescStr(ITEM *,short);
extern void Cmd_ListClass(ITEM *);
extern void Cmd_NameClass(ITEM *);
extern void Cmd_SetClass(ITEM *);
extern void Cmd_UnsetClass(ITEM *);

/* DUPLICATOR.C */

extern int Clone_Room(ITEM *,ITEM *);
extern int Clone_Object(ITEM *,ITEM *);
extern int Clone_Player(ITEM *,ITEM *);
extern int Clone_GenExit(ITEM *,ITEM *);
extern int Clone_MsgExit(ITEM *,MSGEXIT *);
extern int Clone_Chain(ITEM *,CHAIN *);
extern int Clone_Container(ITEM *,CONTAINER *);
extern int Clone_UserFlag(ITEM *,USERFLAG *);
extern int Clone_UserText(ITEM *,ITEM *);
extern int Clone_Inherit(ITEM *,INHERIT *);
extern ITEM *Clone_Item(ITEM *,short);
extern int Duped(ITEM *);
extern int Disintegrate(ITEM *);
extern void DisintegrateAll(void);

/* MAIN.C */

extern void SegV();
extern void Bus();
extern void Div0();

extern int main(int,char *[]);

extern short post_boot;

/* PARSER.C */

extern WLIST *WordList;
extern PCONTEXT ParserData[];
extern short ParsingPersona;

extern void SetPersona(int);
extern PCONTEXT *GetContext(short);
extern void ProLoad(short,int *,int *);
extern void SetItData(short,ITEM *,short,short);
extern void AddWord(char *,short,short);
extern int FreeWord(char *,short);
extern char *FindWText(int,short);
extern WLIST *FindInList(WLIST *,char *,short);
extern char *BreakWord(char *,char *,WLIST *,short);
extern char *GetRestOfInput(char *,char *);
extern char *FNxPhrs(char *);
extern char *WordPtr;
extern char WordBuffer[];
extern WLIST *GetWord(void);
extern int GetOrd(void);
extern char *GetParsedWord(void);
extern void GetAll(void);
extern int GetNumber(void);
extern int GetThing(int *,int *);
extern void SkipPrep(void);
extern int GetPrep(void);
extern int GetVerb(void);
extern char *NextPhrase(void);

/* FLAGNAME.C */

extern char *PBitNames[];
extern char *OBitNames[];
extern char *RBitNames[];
extern char *CBitNames[];
extern int FindRBit(char *);
extern int FindPBit(char *);
extern int FindOBit(char *);
extern int FindCBit(char *);
extern char *RBitName(int);
extern char *OBitName(int);
extern char *PBitName(int);
extern char *CBitName(int);
extern void Cmd_RBitName(ITEM *);
extern void Cmd_OBitName(ITEM *);
extern void Cmd_PBitName(ITEM *);
extern void Cmd_CBitName(ITEM *);
extern void Cmd_ListRBits(ITEM *);
extern void Cmd_ListOBits(ITEM *);
extern void Cmd_ListPBits(ITEM *);
extern void Cmd_ListCBits(ITEM *);

/* NewCmd.c */

extern void Act_SetI(void);
extern void Act_CDaemon(void);
extern int Cnd_Delete(void);
extern int Cnd_ULoad(void);
extern int Cnd_USave(void);
extern int Cnd_FLoad(void);
extern int Cnd_FSave(void);
extern void Act_Getvis(void);
extern void Act_ForkDump(void);
extern void Act_SetExit(void);
extern void Act_DelExit(void);
extern int Cnd_ProcDaemon(void);
extern int Cnd_ProcSubject(void);
extern int Cnd_ProcObject(void);
extern int Cnd_GetSuper(void);
extern void Act_SetSuper(void);
extern int Cnd_Member(void);

/* AberRWho.c */

extern void Act_RwhoDeclareUp(void);
extern void Act_RwhoDeclareDown(void);
extern void Act_RwhoLogin(void);
extern void Act_RwhoLogout(void);
extern void Act_RwhoDeclareAlive(void);

/* UserVector.c */

void UserVector(short,short,short,ITEM *,ITEM *,TPTR);

/* LibRWho.c */

int rwhocli_setup(char *,char *,char *,char *);
int rwhocli_shutdown(void);
int rwhocli_pingalive(void);
int rwhocli_userlogin(char *,char *,time_t);
int rwhocli_userlogout(char *);

/* AnsiBits.c */

extern int stricmp(const char *, const char *);
extern char *strtok2(char *,char *,char *);

/* LookFor.c */

extern void CheckLineFor(LINE *,ITEM *,int);
extern void CheckTableFor(TABLE *,ITEM *,int);
extern void CheckAllFor(ITEM *,ITEM *,int);

/* IPCDirect.c */

extern int IsBSX(int);
extern int FindUserFD(int);
extern int IsUserFD(int);
extern int ReadBlock(int, COMTEXT *);
extern void SnoopPut(int, char);
extern void SnoopFlush(int, int);
extern void SnoopCharPut(int, char);
extern void CharPut(int, char);
extern void LinePut(int, char);
extern void LineFlush(int, char);
extern void FieldShift(int, int);
extern PORT *Bind_Port(int, int);
extern PORT *CreateMPort(int);
extern PORT *OpenMPort(PORT *);
extern int CloseMPort(PORT *);
extern void BlockOn(PORT *);
extern void BlockOff(PORT *);
extern int WriteMPort(PORT *, COMTEXT *, int);
extern int DeleteMPort(PORT *);
extern int AssignService(char *, PORT *);
extern int DeAssignService(char *);
extern PORT *FindService(char *);
extern int ReadMPort(PORT *, COMTEXT *);
extern int Silo(PORT *, char *, int);
extern int SiloFlush(PORT *);
extern int WriteSocket(int, char *, int);
extern int WriteSocketText(int, char *, int);
extern void WriteFlush(int u);
extern char *NetName(unsigned long);
extern int can_play_now(void);
extern int MakeConnection(int, COMTEXT *);

/* LibSocket.c */

int Make_Socket(int);

/* ValidLogic.c */

int MaxSlot(void);

/* _THE END_ */
