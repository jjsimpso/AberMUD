struct IPC_Message
{
	long ms_Size;
	struct IPC_Message *ms_Next;
};

typedef struct IPC_Message MESSAGE;
#ifdef AMIGA_OLD
struct IPC_Port
{
	long po_SystemKey;
	MESSAGE *po_MessageList;
	long po_Flags;
	struct Task *po_Task;
	short po_Signal;
	short po_Open;
};

#else
struct IPC_Port
{
	int po_pid;
	int po_fd;
	int po_Open;
	int po_Flags;
	int po_SiloPtr;
	char po_Silo[8192];
};
#endif
typedef struct IPC_Port PORT;

#define PORT_SYSKEY	37612
#define FL_DELETE	(1<<4)
#define FL_TEMPORARY	(1<<4)
#define FL_FAULT	(1<<5)
#define NOWAIT		(1<<6)
#define FL_BOUND	(1<<7)
extern PORT *CreateMPort();
extern PORT *OpenMPort();
extern PORT *FindService();
