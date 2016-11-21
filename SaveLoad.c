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

/*
 *	Disk Save/Load functions
 *
 *	1.00		AGC	First Version
 *	1.01		AGC	Saves substructures
 *	1.02		AGC	Table saving code
 *	1.03-1.06	AGC	Various tweaks, and substructure adds
 *	1.07		AGC	Saves flag names
 *	1.08		AGC	Runtime file format stuff
 *	1.09		AGC	Hashed text lists
 *	1.10		AGC	Register Optimisations
 *	1.11		AGC	Setin Setout Sethere
 *	1.12		AGC	UserTexts and frees texts right on Setin etc
 *	1.13		AGC	Bug Fixes For 5.06 Release
 *	1.14		AGC	Added Named table support
 *	1.15		AGC	Added 5.08 Item table support
 *	1.16		AGC	Fixed bug if you have single occurance of ""
 *	1.17		AGC	Fixed Unix errno problem.
 *	1.18		AGC	Fixed Unix errno problem properly
 *	1.19		AGC	Taught it about USERFLAG2 and CONDEXIT
 *	1.20		AGC	Tidied warnings out for 5.20
 *	1.21		AGC	Loads superclass and daemontable fields.
 *	1.22		AGC	Fixed SaveUniverse error that isnt bug
 *	1.23		AGC	Saves BSX pictures
 *	1.25		AGC	BSX save/load name bugfix
 */

Module	"DiskIO";
Version	"1.24";
Author  "----*(A)";

extern int errno;
extern ITEM *ItemList;
/*
 *	Items are saved by direct ordered dump, all text dumped is done
 *	literally at present, not through the text list.
 *
 *	Item links are converted to itemnumber in the master list
 *	save for ->masternext which is unsaved as it is implied
 *
 *	For loading an initial value gives the number of items to load
 *	the items are allocated (but not substructs) into a linked list
 *	and an array of their pointers kept for conversions. Each item
 *	is loaded, then the whole array is freed
 */


/* Global Declarations */

static void SaveShort();
static unsigned short LoadShort();
static void SaveLong();
static unsigned long LoadLong();
static void SaveItem();
static ITEM *LoadItem();
static void SaveString();
static TPTR LoadString();
static void SaveBSX();
static BSXImage *LoadBSX();
static void SaveWord();
static int LoadWord();
static void SaveVocab();
static void LoadVocab();
static void WriteHeader();
static long ReadHeader();
static void SaveSub();
static void LoadSub();
static void LoadObject();
static void SaveObject();
static void SetTwo();
static long GetTwo();
static unsigned short *SaveAction();
static short *LoadAction();
static void LoadLine();
static void SaveLine();
static void LoadTable();
static void SaveTable();
static void LoadAllTables();
static void SaveAllTables();
static TABLE *LoadItemTable();
static void SaveItemTable();

static ITEM **ItemArray;	/* Used for item loaders */
static long Load_Format=-1;
static int Load_Error=0;

static void SetTwo(x,v)
unsigned short *x;
register char *v;
{
	x[1]=((unsigned long)v)%65536;	/* Safe -we never extract this pointer */
	x[0]=((unsigned long)v)/65536;	/* directly so little endians should */
					/* be just fine.... CHANGE TO HTONS?? */
}

static long GetTwo(x)
register unsigned short *x;
{
	return(x[1]+65536L*x[0]);
}

static void SaveShort(file,v)
FILE *file;
register unsigned short v;
{
	unsigned char x[3];
/* Assumes 16 bit short - they are used as such even if bigger so is ok */
	x[0]=v/256;
	x[1]=v%256;
	if(fwrite(x,2,1,file)!=1)
		Load_Error=errno;
}

static unsigned short LoadShort(x)
FILE *x;
{
	unsigned char a[3];
	if(fread(a,2,1,x)!=1)
		Load_Error=errno;
	return((unsigned short)(256L*a[0]+a[1]));
}

static void SaveLong(file,x)
FILE *file;
register unsigned long x;
{
/*
 *	Assumes 32 bits - see notes above
 */
	SaveShort(file,(unsigned short)(x/65536));
	SaveShort(file,(unsigned short)(x%65536));
}

static unsigned long LoadLong(f)
FILE *f;
{
	unsigned short a=LoadShort(f);
	return((unsigned long)(a*65536L+LoadShort(f)));
}

static void SaveItem(file,i)
FILE *file;
ITEM *i;
{
	if(i==NULL)
		SaveLong(file,-1L);
	else
	{
		SaveLong(file,MasterNumber(i));
	}
}

static ITEM *LoadItem(file)
FILE *file;
{
	register long x=LoadLong(file);
	if(x==-1)
		return(NULL);
	return(ItemArray[x]);
}

static void SaveString(file,s)
FILE *file;
TPTR s;
{
	register char *str=TextOf(s);
	register long v=strlen(str);
	if(s->te_Users==1)
		v|=65536;
	SaveLong(file,v);
	if(fwrite(str,strlen(str),1,file)!=1)
		Load_Error=errno;
}

static TPTR LoadString(file)
FILE *file;
{
	register char *a;
	register long v=LoadLong(file);
	register TPTR t;
	register short f=0;
	if(v>=65536)
	{
		f=1;
		v-=65536;
	}
	a=malloc(v+1);
	if(!a)
	{
		Error("Out Of Memory");
	}
	if(fread(a,v,1,file)!=1)
	{
		Load_Error=errno;
	}
	a[v]=0;
	if(f)
		t=QuickAllocText(a);
	else
		t=AllocText(a);
	free(a);
	return(t);
}

static TPTR LoadComment(file)
FILE *file;
{
	register char *a;
	register long v=LoadLong(file);
	register TPTR t;
	if(v>65536)
	{
		v-=65536;
	}
	a=malloc(v+1);
	if(!a)
		Error("Out Of Memory");
	if(fread(a,v,1,file)!=1)
	{
		Load_Error=errno;
	}
	a[v]=0;
	t=AllocComment(a);
	free(a);
	return(t);
}

static void SaveBSX(file,i)
FILE *file;
BSXImage *i;
{
	int v=strlen(i->bsx_Identifier);
	SaveLong(file,v);
	SaveLong(file,i->bsx_DataSize);
	if(fwrite(i->bsx_Identifier,v,1,file)!=1)
		Load_Error=errno;
	if(fwrite(i->bsx_Data,i->bsx_DataSize,1,file)!=1)
		Load_Error=errno;
}

static BSXImage *LoadBSX(file)
FILE *file;
{
	extern BSXImage *BSXAllocate();
	BSXImage *i;
	char buf[128];
	long v=LoadLong(file);
	long d=LoadLong(file);
	if(fread(buf,v,1,file)!=1)
	{
		Load_Error=errno;
		return(NULL);
	}
	buf[v]=0;
	i=BSXAllocate(buf,d);
	if(fread(i->bsx_Data,d,1,file)!=1)
	{
		Load_Error=errno;
		BSXDelete(i);
		return(NULL);
	}
	return(i);
}

static void SaveWord(file,x)
FILE *file;
register WLIST *x;
{
/* NOTE: Words aren't allowed to contain spaces - THIS RELIES ON IT !!! */
	fprintf(file,"%s %d %d.",x->wd_Text,x->wd_Code,x->wd_Type);
}

static int LoadWord(file)
FILE *file;
{
	static char x[128];
	int a,b;
	if(fscanf(file,"%s %d %d.",x,&a,&b)!=3)
		Error("LoadWord: Corruption In Database File");
	if(strcmp(x,";END")==0)	/*; not legal in word either! */
		return(0);
	AddWord(x,(short)a,(short)b);
	return(1);
}

static void SaveVocab(file)
FILE *file;
{
	register WLIST *a=WordList;
	while(a)
	{
		SaveWord(file,a);
		a=a->wd_Next;
	}
	fprintf(file,";END 0 0.");
}

static void LoadVocab(file)
FILE *file;
{
	while(LoadWord(file));
}

static void WriteHeader(file)
FILE *file;
{
	long header[4];
	header[0]=CountItems();
	header[1]=10;		/* Format Identifier */
/*
 *	FORMATS:	0:	Original (no longer supported)
 *			1:	5.04 no flag names
 *			2:	5.04 flag names
 *			3:	5.05 vocab seek & classes
 *			4:	5.06 with text speed flags
 *			5:	5.07 with named tables
 *			6:	5.08 with item bound tables
 *		        7:      5.11 USERFLAG2 and CONDEXITs
 *			8:	5.14
 *			9:	5.20 superclass and daemon table
 *		       10:	5.21 BSX picture data
 */
	time(&(header[2]));	/* Save time */
	header[3]=0;		/* Reserved for vocab seek*/
	SaveLong(file,header[0]);
	SaveLong(file,header[1]);
	SaveLong(file,header[2]);
	SaveLong(file,header[3]);
}


static long ReadHeader(file)
FILE *file;
{
	long v[4];
	register long ct;
	v[0]=LoadLong(file);
	v[1]=LoadLong(file);
	v[2]=LoadLong(file);
	v[3]=LoadLong(file);
	Load_Format=v[1];
	if(Load_Format<1)
	{
		fprintf(stderr,
"This database format is too old.\n");
		exit(0);
	}
	if(Load_Format>10)
	{
		fprintf(stderr,
"This database format is beyond my knowledge, you need a newer SERVER.\n");
		exit(0);
	}
	Log("Header Read: %ld Items",v[0]);
	ct=0;
/*
 *	The +1 here stops a 0 sized malloc, which upsets some machines
 */
	ItemArray=(ITEM **)malloc(v[0]*sizeof(ITEM *)+1);
	if(ItemArray==NULL)
		Error("Out Of Memory");
	while(ct<v[0])
	{
		ItemArray[ct]=Allocate(ITEM);
		ItemArray[ct]->it_MasterNext=NULL;
		if(ct)
			ItemArray[ct-1]->it_MasterNext=ItemArray[ct];
		else
			ItemList=ItemArray[ct];
		ct++;
	}
	ItemArray[ct-1]->it_MasterNext=NULL;
/*
 *	The above creates a correctly ordered linked set of trash items
 */
	return(v[0]);
}

static void SaveSub(file,s)
FILE *file;
register SUB *s;
{
	SaveShort(file,s->pr_Key);
	switch(s->pr_Key)
	{
		case KEY_INOUTHERE:
			SaveString(file,((INOUTHERE *)s)->io_InMsg);
			SaveString(file,((INOUTHERE *)s)->io_OutMsg);
			SaveString(file,((INOUTHERE *)s)->io_HereMsg);
			break;
		case KEY_USERFLAG2:
		case KEY_USERFLAG:
			SaveShort(file,((USERFLAG *)s)->uf_Flags[0]);
			SaveShort(file,((USERFLAG *)s)->uf_Flags[1]);
			SaveShort(file,((USERFLAG *)s)->uf_Flags[2]);
			SaveShort(file,((USERFLAG *)s)->uf_Flags[3]);
			SaveShort(file,((USERFLAG *)s)->uf_Flags[4]);
			SaveShort(file,((USERFLAG *)s)->uf_Flags[5]);
			SaveShort(file,((USERFLAG *)s)->uf_Flags[6]);
			SaveShort(file,((USERFLAG *)s)->uf_Flags[7]);
			SaveItem(file,((USERFLAG *)s)->uf_Items[0]);
			SaveItem(file,((USERFLAG *)s)->uf_Items[1]);
			SaveItem(file,((USERFLAG *)s)->uf_Items[2]);
			SaveItem(file,((USERFLAG *)s)->uf_Items[3]);
			SaveItem(file,((USERFLAG *)s)->uf_Items[4]);
			SaveItem(file,((USERFLAG *)s)->uf_Items[5]);
			SaveItem(file,((USERFLAG *)s)->uf_Items[6]);
			SaveItem(file,((USERFLAG *)s)->uf_Items[7]);
			break;
		case KEY_CONTAINER:
			SaveShort(file,((CONTAINER *)s)->co_Volume);
			SaveShort(file,((CONTAINER *)s)->co_Flags);
			break;
		case KEY_CHAIN:
			SaveItem(file,((CHAIN *)s)->ch_Chained);
			break;
		case KEY_ROOM:
			SaveString(file,((ROOM *)s)->rm_Short);
			SaveString(file,((ROOM *)s)->rm_Long);
			SaveShort(file,((ROOM *)s)->rm_Flags);
			break;
		case KEY_OBJECT:
			SaveString(file,((OBJECT *)s)->ob_Text[0]);
			SaveString(file,((OBJECT *)s)->ob_Text[1]);
			SaveString(file,((OBJECT *)s)->ob_Text[2]);
			SaveString(file,((OBJECT *)s)->ob_Text[3]);
			SaveShort(file,((OBJECT *)s)->ob_Size);
			SaveShort(file,((OBJECT *)s)->ob_Weight);
			SaveShort(file,((OBJECT *)s)->ob_Flags);
			break;
		case KEY_PLAYER:
			SaveShort(file,((PLAYER *)s)->pl_UserKey);
			SaveShort(file,((PLAYER *)s)->pl_Size);
			SaveShort(file,((PLAYER *)s)->pl_Weight);
			SaveShort(file,((PLAYER *)s)->pl_Strength);
			SaveShort(file,((PLAYER *)s)->pl_Flags);
			SaveShort(file,((PLAYER *)s)->pl_Level);
			SaveLong(file,((PLAYER *)s)->pl_Score);
			break;
		case KEY_GENEXIT:
			SaveItem(file,((GENEXIT *)s)->ge_Dest[0]);
			SaveItem(file,((GENEXIT *)s)->ge_Dest[1]);
			SaveItem(file,((GENEXIT *)s)->ge_Dest[2]);
			SaveItem(file,((GENEXIT *)s)->ge_Dest[3]);
			SaveItem(file,((GENEXIT *)s)->ge_Dest[4]);
			SaveItem(file,((GENEXIT *)s)->ge_Dest[5]);
			SaveItem(file,((GENEXIT *)s)->ge_Dest[6]);
			SaveItem(file,((GENEXIT *)s)->ge_Dest[7]);
			SaveItem(file,((GENEXIT *)s)->ge_Dest[8]);
			SaveItem(file,((GENEXIT *)s)->ge_Dest[9]);
			SaveItem(file,((GENEXIT *)s)->ge_Dest[10]);
			SaveItem(file,((GENEXIT *)s)->ge_Dest[11]);
			break;
		case KEY_CONDEXIT:
			SaveItem(file,((CONDEXIT *)s)->ce_Dest);
			SaveShort(file,(short)((CONDEXIT *)s)->ce_Table);
			SaveShort(file,(short)((CONDEXIT *)s)->ce_ExitNumber);
			break;
		case KEY_MSGEXIT:
			SaveItem(file,((MSGEXIT *)s)->me_Dest);
			SaveString(file,((MSGEXIT *)s)->me_Text);
			SaveShort(file,((MSGEXIT *)s)->me_ExitNumber);
			break;
		case KEY_INHERIT:
			SaveItem(file,((INHERIT *)s)->in_Master);
			break;
		case KEY_DUPED:
			SaveItem(file,((DUP *)s)->du_Master);
			break;
		case KEY_USERTEXT:
			SaveString(file,((USERTEXT *)s)->ut_Text[0]);
			SaveString(file,((USERTEXT *)s)->ut_Text[1]);
			SaveString(file,((USERTEXT *)s)->ut_Text[2]);
			SaveString(file,((USERTEXT *)s)->ut_Text[3]);
			SaveString(file,((USERTEXT *)s)->ut_Text[4]);
			SaveString(file,((USERTEXT *)s)->ut_Text[5]);
			SaveString(file,((USERTEXT *)s)->ut_Text[6]);
			SaveString(file,((USERTEXT *)s)->ut_Text[7]);
			break;
		default:Error("Unknown Subtype");
	}
}

static void LoadSub(file,item,type)
FILE *file;
ITEM *item;
short type;
{
	TPTR xt;
	switch(type)
	{
		case KEY_INOUTHERE:
		{
			SetInMsg(item,TextOf(xt=LoadString(file)));
			FreeText(xt);
			SetOutMsg(item,TextOf(xt=LoadString(file)));
			FreeText(xt);
			SetHereMsg(item,TextOf(xt=LoadString(file)));
			FreeText(xt);
			break;
		}
		case KEY_USERTEXT:
		{
			SetUText(item,0,(xt=LoadString(file)));
			FreeText(xt);
			SetUText(item,1,(xt=LoadString(file)));
			FreeText(xt);
			SetUText(item,2,(xt=LoadString(file)));
			FreeText(xt);
			SetUText(item,3,(xt=LoadString(file)));
			FreeText(xt);
			SetUText(item,4,(xt=LoadString(file)));
			FreeText(xt);
			SetUText(item,5,(xt=LoadString(file)));
			FreeText(xt);
			SetUText(item,6,(xt=LoadString(file)));
			FreeText(xt);
			SetUText(item,7,(xt=LoadString(file)));
			FreeText(xt);
			break;
		}
		case KEY_CONTAINER:
		{
			register CONTAINER *c=BeContainer(item);
			c->co_Volume=LoadShort(file);
			c->co_Flags=LoadShort(file);
			break;
		}
		case KEY_USERFLAG:
		case KEY_USERFLAG2:
		{
			register USERFLAG *u;
			int x=0;
			if(type==KEY_USERFLAG2)
				x=8;
			SetUserFlag(item,0+x,LoadShort(file));
			SetUserFlag(item,1+x,LoadShort(file));
			SetUserFlag(item,2+x,LoadShort(file));
			SetUserFlag(item,3+x,LoadShort(file));
			SetUserFlag(item,4+x,LoadShort(file));
			SetUserFlag(item,5+x,LoadShort(file));
			SetUserFlag(item,6+x,LoadShort(file));
			SetUserFlag(item,7+x,LoadShort(file));
			if(type==KEY_USERFLAG)
				u=UserFlagOf(item);
			else
				u=UserFlag2Of(item);
			u->uf_Items[0]=LoadItem(file);
			u->uf_Items[1]=LoadItem(file);
			u->uf_Items[2]=LoadItem(file);
			u->uf_Items[3]=LoadItem(file);
			if(Load_Format>7)
			{
				u->uf_Items[4]=LoadItem(file);
				u->uf_Items[5]=LoadItem(file);
				u->uf_Items[6]=LoadItem(file);
				u->uf_Items[7]=LoadItem(file);
			}
			break;
		}
		case KEY_CHAIN:
		{
			ITEM *x=LoadItem(file);
			AddNLChain(item,x);
			break;
		}
		case KEY_ROOM:
		{
			register ROOM *a;
			MakeRoom(item);
			a=(ROOM *)FindSub(item,KEY_ROOM);
			FreeText(a->rm_Short);
			FreeText(a->rm_Long);
			a->rm_Short=LoadString(file);
			a->rm_Long=LoadString(file);
			a->rm_Flags=LoadShort(file);
			break;
		}
		case KEY_OBJECT:
		{
			register OBJECT *o;
			MakeObject(item);
			o=(OBJECT *)FindSub(item,KEY_OBJECT);
			FreeText(o->ob_Text[0]);
			FreeText(o->ob_Text[1]);
			FreeText(o->ob_Text[2]);
			FreeText(o->ob_Text[3]);	
			o->ob_Text[0]=LoadString(file);
			o->ob_Text[1]=LoadString(file);
			o->ob_Text[2]=LoadString(file);
			o->ob_Text[3]=LoadString(file);
			o->ob_Size=LoadShort(file);
			o->ob_Weight=LoadShort(file);
			o->ob_Flags=LoadShort(file);
			break;
		}
		case KEY_PLAYER:
		{
			register PLAYER *p;
			MakePlayer(item);
			p=(PLAYER *)FindSub(item,KEY_PLAYER);
			p->pl_UserKey=LoadShort(file);
			p->pl_Size=LoadShort(file);
			p->pl_Weight=LoadShort(file);
			p->pl_Strength=LoadShort(file);
			p->pl_Flags=LoadShort(file);
			p->pl_Level=LoadShort(file);
			p->pl_Score=LoadLong(file);
			break;
		}
		case KEY_GENEXIT:
		{
			register GENEXIT *g;
			MakeGenExit(item);
			g=(GENEXIT *)FindSub(item,KEY_GENEXIT);
			g->ge_Dest[0]=LoadItem(file);
			g->ge_Dest[1]=LoadItem(file);
			g->ge_Dest[2]=LoadItem(file);
			g->ge_Dest[3]=LoadItem(file);
			g->ge_Dest[4]=LoadItem(file);
			g->ge_Dest[5]=LoadItem(file);
			g->ge_Dest[6]=LoadItem(file);
			g->ge_Dest[7]=LoadItem(file);
			g->ge_Dest[8]=LoadItem(file);
			g->ge_Dest[9]=LoadItem(file);
			g->ge_Dest[10]=LoadItem(file);
			g->ge_Dest[11]=LoadItem(file);
			break;
		}
		case KEY_CONDEXIT:
		{
			register ITEM *a;
			short c,d;
			a=LoadItem(file);
			c=LoadShort(file);
			d=LoadShort(file);
			/* MakeCond... sets its own locks */
			MakeNLCondExit(item,a,c,d);
			break;
		}
		case KEY_MSGEXIT:
		{
			ITEM *a;
			TPTR b;
			short c;
			a=LoadItem(file);
			b=LoadString(file);
			c=LoadShort(file);
			MakeNLMsgExit(item,a,c,TextOf(b));
			FreeText(b);
			break;
		}
		case KEY_INHERIT:
		{
			ITEM *a=LoadItem(file);
			register INHERIT *d=(INHERIT *)AllocSub(item,KEY_INHERIT,
				sizeof(INHERIT));
			d->in_Master=a;
			break;
		}
		case KEY_DUPED:
		{
			ITEM *a=LoadItem(file);
			register DUP *d=(DUP *)AllocSub(item,KEY_DUPED,sizeof(DUP));
			d->du_Master=a;
			break;
		}
		default:Log("Subtype %d found!",type);
			Error("Unknown Subtype");
	}
}

static void LoadObject(file,i)
register FILE *file;
register ITEM *i;
{
	register long n;
	i->it_Name=LoadString(file);
/*	printf("%s\n",TextOf(i->it_Name));	*/
	i->it_Adjective=LoadShort(file);
	i->it_Noun=LoadShort(file);
	i->it_State=LoadShort(file);
	i->it_Perception=LoadShort(file);
	i->it_Next=LoadItem(file);
	i->it_Children=LoadItem(file);
	i->it_Parent=LoadItem(file);
	i->it_ActorTable=LoadShort(file);
	i->it_ActionTable=LoadShort(file);
	i->it_Users=LoadShort(file);
	i->it_Class=LoadShort(file);
	if(Load_Format>8)
	{
		if(LoadShort(file))
			i->it_Superclass=LoadItem(file);
		else
			i->it_Superclass=NULL;
	}
	i->it_Properties=NULL;
	n=LoadLong(file);	/* props */
	if(n==0)
		return;
	while(n)
	{
		n=LoadShort(file);
		if(n)
			LoadSub(file,i,(short)n);
	}
	if(Load_Format>5)
	{
		i->it_ObjectTable=LoadItemTable(file);
		i->it_SubjectTable=LoadItemTable(file);
		if(Load_Format>8)
			i->it_DaemonTable=LoadItemTable(file);
		else
			i->it_DaemonTable=NULL;
	}
	else
	{
		i->it_ObjectTable=NULL;
		i->it_SubjectTable=NULL;
	}
}
	
static void SaveObject(file,i)
register FILE *file;
register ITEM *i;
{
	SUB *n;
	KillEventQueue(i);
	SaveString(file,i->it_Name);
	SaveShort(file,i->it_Adjective);
	SaveShort(file,i->it_Noun);
	SaveShort(file,i->it_State);
	SaveShort(file,i->it_Perception);
	SaveItem(file,i->it_Next);
	SaveItem(file,i->it_Children);
	SaveItem(file,i->it_Parent);
	SaveShort(file,i->it_ActorTable);
	SaveShort(file,i->it_ActionTable);
	SaveShort(file,i->it_Users);
	SaveShort(file,i->it_Class);
	if(i->it_Superclass)
	{
		SaveShort(file,1);
		SaveItem(file,i->it_Superclass);
	}
	else
		SaveShort(file,0);
	SaveLong(file,(unsigned long)i->it_Properties);
	n=i->it_Properties;
	if(n==0)
		return;
	while(n)
	{
		SaveSub(file,n);
		n=n->pr_Next;
	}
	SaveShort(file,0);	/* Prop end marker */
	SaveItemTable(file,i->it_ObjectTable);
	SaveItemTable(file,i->it_SubjectTable);
	SaveItemTable(file,i->it_DaemonTable);	
}

unsigned short *SaveAction(file,c)
FILE *file;
register unsigned short *c;
{
	int cc=*c;
	TPTR t;
	ITEM *i;
	register char *ptr;
	SaveShort(file,*c++);
	ptr=Cnd_Table[cc];
	while(*ptr!=' ')
	{
		switch(*ptr++)
		{
		case '$':;
		case 'T':t=(TPTR )GetTwo(c);
			 c+=2;
			 if(t==(TPTR )1)
				SaveShort(file,0);
			 else
			 {
				if(t==(TPTR)3)
					SaveShort(file,3);
				else
				{
					SaveShort(file,1);
					SaveString(file,t);
				}
			 }
			 break;
		case 'I':i=(ITEM *)GetTwo(c);
			 c+=2;
			 if(i==(ITEM *)1)
			 {
				SaveShort(file,1);
				break;
			 }
			 if(i==(ITEM *)3)
			 {
				SaveShort(file,3);
				break;
			 }
			 if(i==(ITEM *)5)
			 {
				SaveShort(file,5);
				break;
			 }
			 if(i==(ITEM *)7)
			 {
				SaveShort(file,7);
				break;
			 }
			 if(i==(ITEM *)9)
			 {
				SaveShort(file,9);
				break;
			 }
			 SaveShort(file,0);
			 SaveItem(file,i);
			 break;
		case 'C':;
		case 'R':;
		case 'O':;
		case 'P':;
		case 'c':;
		case 'B':;
		case 'F':;
		case 'v':;
		case 'p':;
		case 'n':;
		case 'a':;
		case 't':;
		case 'N':SaveShort(file,*c++);
			 break;
		default:Error("Save: Bad Cnd_Table Entry");
		}
	}
	return(c);
}

short *LoadAction(file,c)
FILE *file;
register short *c;
{
	short cc=*c++;
	ITEM *i;
	TPTR t;
	register char *ptr=Cnd_Table[cc];
	while(*ptr!=' ')
	{
		switch(*ptr++)
		{
		case '$':switch(LoadShort(file))
			 {
				case 0:t=(TPTR)1;break;
				case 3:t=(TPTR)3;break;
				default:t=LoadComment(file);
			 }
			 SetTwo(c,(char *)t);
			 c+=2;
			 break;
		case 'T':switch(LoadShort(file))
			 {
				case 0:t=(TPTR )1;break;
				case 3:t=(TPTR) 3;break;
				default:t=LoadString(file);
			 }
			 SetTwo(c,(char *)t);
			 c+=2;
			 break;
		case 'I':switch(LoadShort(file))
			 {
				case 1:i=(ITEM *)1;break;
				case 3:i=(ITEM *)3;break;
				case 5:i=(ITEM *)5;break;
				case 7:i=(ITEM *)7;break;
				case 9:i=(ITEM *)9;break;
				default:i=LoadItem(file);
			 }
			 SetTwo(c,(char *)i);
			 c+=2;
			 break;
		case 'B':;
		case 'C':;
		case 'c':;
		case 'R':;
		case 'O':;
		case 'P':;
		case 'F':;
		case 'v':;
		case 'p':;
		case 'n':;
		case 'a':;
		case 't':;
		case 'N':*c++=LoadShort(file);
			 break;
		default:Error("Load: Bad Cnd_Table Entry");
		}
	}
	return(c);
}

void LoadLine(file,l)
FILE *file;
register LINE *l;
{
	short *a=(short *)malloc(1024);
	register short *b=a;
	if(a==NULL)
		Error("Out Of Memory");
	l->li_Verb=LoadShort(file);
	l->li_Noun1=LoadShort(file);
	l->li_Noun2=LoadShort(file);
	while((*b=LoadShort(file))!=CMD_EOL)
	{
		b=LoadAction(file,b);
	}
	free(l->li_Data);
	l->li_Data=(unsigned short *)malloc(sizeof(short)*(b-a+2));
	memcpy((char *)l->li_Data,(char *)a,sizeof(short)*(b-a+2));
	free((char *)a);
}

void SaveLine(file,l)
FILE *file;
LINE *l;
{
	unsigned short *a=l->li_Data;
	SaveShort(file,l->li_Verb);
	SaveShort(file,l->li_Noun1);
	SaveShort(file,l->li_Noun2);
	while(*a!=CMD_EOL)
	{
		a=SaveAction(file,a);
	}
	SaveShort(file,CMD_EOL);
}

TABLE *LoadItemTable(file)
FILE *file;
{
	TABLE *t;
	int x=LoadShort(file);
	if(x==0)
		return(NULL);
	t=Allocate(TABLE);
	LoadShort(file);
	LoadTable(file,t);
	return(t);
}

void SaveItemTable(file,t)
FILE *file;
TABLE *t;
{
	if(t==NULL)
	{
		SaveShort(file,0);
	}
	else
	{
		SaveShort(file,1);
		SaveTable(file,t);
	}
}

void LoadTable(file,t)
FILE *file;
TABLE *t;
{	
	register LINE *l;
	if(Load_Format>4)
		t->tb_Name=LoadString(file);
	else
		t->tb_Name=AllocText("(unset)");
	while(LoadShort(file)==0)
	{
		l=NewLine(t,65535);
		LoadLine(file,l);
	}
}

void SaveTable(file,t)
FILE *file;
TABLE *t;
{
	register LINE *l=t->tb_First;
	SaveShort(file,t->tb_Number);
	SaveString(file,t->tb_Name);
	while(l)
	{
		SaveShort(file,0);
		SaveLine(file,l);
		l=l->li_Next;
	}
	SaveShort(file,1);
}

void LoadAllTables(file)
FILE *file;
{
	register TABLE *t;
	while(LoadShort(file)==0)
	{
		t=NewTable(LoadShort(file),NULL);
		LoadTable(file,t);
	}
}

void SaveAllTables(file)
FILE *file;
{
	register TABLE *t=TableList;
	while(t)
	{
		SaveShort(file,0);
		SaveTable(file,t);
		t=t->tb_Next;
	}
	SaveShort(file,1);
}
	
extern char *FlagName[];

static void SaveFlag(file,s)
FILE *file;
short s;
{
	register char *str=FlagName[s];
	short v;
	if(str==NULL)
		SaveShort(file,0);
	else
	{
		v=strlen(str);
/*		printf("%d %ld\n",(int)s,v);	*/
		SaveShort(file,v);
		if(fwrite(str,strlen(str),1,file)!=1)
			Load_Error=errno;
	}
}


void LoadFlag(file,n)
FILE *file;
register short n;
{
	register char *a;
	long v=LoadShort(file);
/*	printf("FLAG %d V=%ld",(int)n,v);	*/
	if(v==0)
	{
		SetFlagName(n,NULL);
	}
	else
	{
		a=malloc(v+1);
		if(!a)
			Error("Out Of Memory");
		if(fread(a,v,1,file)!=1)
			Load_Error=errno;
		a[v]=0;
		SetFlagName(n,a);
		free(a);
	}
}

static void LoadClass(a,c)
FILE *a;
short c;
{
	if(LoadShort(a))
		SetClassTxt(c,LoadString(a));
	else
		SetClassName(c,NULL);
}

static void SaveClass(a,c)
FILE *a;
short c;
{
	if(GetClassTxt(c))
	{
		SaveShort(a,1);
		SaveString(a,GetClassTxt(c));
	}
	else
		SaveShort(a,0);
}
		
void SaveBitFlags(f)
FILE *f;
{
	int ct=0;
	while(ct<16)
	{
		fprintf(f,"%s\001",RBitName(ct));
		fprintf(f,"%s\001",OBitName(ct));
		fprintf(f,"%s\001",PBitName(ct));
		fprintf(f,"%s\001",CBitName(ct));
		ct++;
	}
}

static char *LoadDupStr(x)
char *x;
{
	char *a=strdup(x);
	if(!a)
	{
		fprintf(stderr,"Out Of Memory");
		exit(0);
	}
	return(a);
}

void LodBuf(f,x)
FILE *f;
char *x;
{
	while((*x=fgetc(f))!='\001')
		x++;
	*x=0;
}

void LoadBitFlags(f)
FILE *f;
{
	int ct=0;
	char buf[256];
	while(ct<16)
	{
		LodBuf(f,buf);
		RBitNames[ct]=LoadDupStr(buf);
		LodBuf(f,buf);
		OBitNames[ct]=LoadDupStr(buf);
		LodBuf(f,buf);
		PBitNames[ct]=LoadDupStr(buf);
		LodBuf(f,buf);
		CBitNames[ct]=LoadDupStr(buf);
		ct++;
	}
}

void SaveBSXImages(file)
FILE *file;
{
	extern BSXImage *BSXFindFirst(),*BSXFindNext();
	BSXImage *i=BSXFindFirst();
	while(i!=NULL)
	{
		SaveShort(file,1);
		SaveBSX(file,i);
		i=BSXFindNext(i);
	}
	SaveShort(file,0);
}

void LoadBSXImages(file)
FILE *file;
{
	int i;
	i=LoadShort(file);
	while(i!=0)
	{
		LoadBSX(file);
		i=LoadShort(file);
	}
}

int SaveSystem(n)
char *n;
{
	register ITEM *i;
	static char b[140];	/* Max file name length =128 */
	FILE *a=fopen(n,"r");
	register short c=0;
	sprintf(b,"%s.bak",n);
	if(a)
	{
		fclose(a);
		unlink(b);	/* Can't rename over another file */
		if(rename(n,b)==-1)
		{
			return(-1);
		}	/* TRY LINK(a,b) ON UNIX - OR
			   SEE /usr/src/bin/passwd.c FOR YOUR CLONE. ANYWAY 
			   RENAME IS ANSI ...*/
	}
/* Now to save */
	a=fopen(n,"w");
	if(a==NULL)
		return(-2);
	Load_Error=0;
	WriteHeader(a);
	i=ItemList;
	while(i)
	{
		SaveObject(a,i);
		i=i->it_MasterNext;
	}
	SaveAllTables(a);
	SaveVocab(a);
	while(c<512)
		SaveFlag(a,c++);
	c=0;
	while(c<16)
		SaveClass(a,c++);
	SaveBitFlags(a);
	SaveBSXImages(a);
	if(ferror(a)&&Load_Error==0)
		Load_Error=errno;
	fclose(a);
	return(Load_Error);
}

		
int LoadSystem(n)
char *n;
{
	FILE *a=fopen(n,"r");
	register long ct=0;
	long v,c=0;
	if(a==NULL)
		return(-1);
	Load_Error=0;
	v=ReadHeader(a);	/* Set up items */
	while(ct<v)
	{
		LoadObject(a,ItemArray[ct]);
		ct++;
	}
	LoadAllTables(a);
	free((char *)ItemArray);	/* Free Reloc Info */
	LoadVocab(a);
	if(Load_Format>1)	/* If new format database.. */
		while(c<512)
			LoadFlag(a,c++);
	if(Load_Format>2)	/* If 5.05 dbm load classes */
	{
		c=0;
		while(c<16)
			LoadClass(a,c++);
	}
	if(Load_Format>4)	/* 5.07 load bit flags too */
		LoadBitFlags(a);
	if(Load_Format>9)
		LoadBSXImages(a);
	fclose(a);
	Log("Load Read Completed");
	return(Load_Error);
}



