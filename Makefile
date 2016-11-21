

CFILES  = NoMalloc.c Main.c NewCmd.c AnsiBits.c TableDriver.c \
	  TabCommand.c CompileTable.c InsAndOuts.c System.c PlyCommand.c \
	  ObjCommand.c GenCommand.c SysSupport.c RoomCommands.c Editing.c \
	  Parser.c SubHandler.c SaveLoad.c ComDriver.c BootDaemon.c \
	  ComServer.c CondCode.c ActionCode.c TimeSched.c UserFile.c Daemons.c \
	  TableEditing.c Container.c ExitLogic.c ContCommand.c Snoop.c \
	  DarkLight.c Duplicator.c FlagControl.c UtilCommand.c ObjectEdit.c \
	  Class.c LookFor.c UserVector.c FlagName.c FindPW.c runaber.c IPCClean.c \
	  LibRwho.c AberRwho.c IPCDirect.c LibSocket.c ValidLogin.c \
	  BSX.c

OFILES  = NoMalloc.o Main.o NewCmd.o AnsiBits.o TableDriver.o \
	  TabCommand.o CompileTable.o InsAndOuts.o System.o PlyCommand.o \
	  ObjCommand.o GenCommand.o SysSupport.o RoomCommands.o Editing.o \
	  Parser.o SubHandler.o SaveLoad.o ComDriver.o BootDaemon.o \
	  ComServer.o CondCode.o ActionCode.o TimeSched.o UserFile.o Daemons.o \
	  TableEditing.o Container.o ExitLogic.o ContCommand.o Snoop.o \
	  DarkLight.o Duplicator.o FlagControl.o UtilCommand.o ObjectEdit.o \
	  Class.o LookFor.o UserVector.o FlagName.o LibRwho.o AberRwho.o \
	  IPCDirect.o LibSocket.o ValidLogin.o BSX.o


HEADERS = System.h User.h Comms.h NoProto.h

LDFLAGS = 
ECHO    = /bin/echo
MV	= /bin/mv
TOUCH   = touch

CC	= gcc -Wall -pedantic

all     : server FindPW Run_Aber Reg docs
	@${ECHO}   AberMUD5 is up to date.

server  : ${OFILES}
	${CC} ${LDFLAGS} ${OFILES}
	@${TOUCH} server
	@${MV} server server.old
	@${MV} a.out server

Reg	: Reg.o UserFile.o AnsiBits.o
	${CC} ${LDFLAGS} Reg.o UserFile.o AnsiBits.o -o Reg

Reg.o	: Reg.c System.h

client  : Socket.o Client.o
	${CC} ${LDFLAGS} Client.o Socket.o -lcurses -o client

Socket.o : Socket.c

Client.o : Client.c

FindPW  : UserFile.o FindPW.o AnsiBits.o
	${CC} ${LDFLAGS} FindPW.o UserFile.o AnsiBits.o -o FindPW

FindPW.o : FindPW.c

runaber.o : runaber.c

Run_Aber : Run_Aber.o
	${CC} ${LDFLAGS} Run_Aber.o -o Run_Aber

docs:
	nroff -man DOC/Contents DOC/1-2 DOC/3 DOC/4 DOC/5 DOC/6 DOC/7 DOC/8 DOC/9 DOC/10 DOC/A >Manual.txt

clean:
	rm -f *.o *~ server FindPW Run_Aber Reg server.old Manual.txt


${OFILES} : ${HEADERS}
.PRECIOUS: ${CFILES} ${HEADERS}
.DEFAULT: ${CFILES} ${HEADERS} 

