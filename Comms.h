#define MSGPORT
struct Com_Pack1
{
	short pa_Type;
	short pa_Sender;
	short pa_Data[4];
};

struct Com_Pack2
{
	short pa_Type;
	short pa_Sender;
	char pa_Data[512];
};

typedef struct Com_Pack1 COMDATA;
typedef struct Com_Pack2 COMTEXT;

typedef struct SysPacket
{
#ifdef MSGPORT
	long	pa_Type;
#endif
	short pa_Size;
	char pa_Data[516];
} PACKET;

#define PACKET_CLEAR		0	/*Num. parser clear and shutdown */
#define PACKET_LOOPECHO		1	/*Num. cause either end to echo 2*/
#define PACKET_ECHOBACK		2	/*Num. echo back		 */
#define PACKET_LOGINACCEPT	3	/*Num. p1 holds systems ctrl sig */
					/*     p2 holds your 'user ref'  */
#define PACKET_CLEARED		4	/*User has cleared		 */
#define PACKET_ECHO		5	/* Set echo on p1=1 off on p1=0  */
#define PACKET_INPUT		6	/* p1=input on p2=input off      */
#define PACKET_LOGINREQUEST	128	/*Text. text holds:-
						SigCode:UserID:FilePath	 */

#define PACKET_OUTPUT		129	/*Text for printing		 */
#define PACKET_COMMAND		130	/*Incoming command line		 */
#define PACKET_SETPROMPT	131	/*Set prompt line		 */
#define PACKET_COMMFORCE	132	/*Command but always from base   */
					/*eg menu selections etc         */
#define PACKET_EDIT		133	/*Edit Request			 */
#define PACKET_SNOOPTEXT	134	/*Snooped text			 */
#define PACKET_SETFIELD		135	/*Field shift			 */
#define PACKET_SETTITLE		240	/*Title bar for clients		 */
#define PACKET_BSXSCENE		200	/*Draw a picture request	 */
#define PACKET_BSXOBJECT	201	/*Draw an object */

#define PACKET_ABORT		140	/* REMOTE DIE REQUEST */
#define PACKET_SUPERCEDE	141	/* I'VE PINCHED YOUR PORT..... */

#define PACKET_BONG		150	/* Once per second packet shover */

