 /****************************************************************************\
 *									      *
 *			C R E A T O R    O F   L E G E N D S		      *
 *				(AberMud Version 5)			      *
 *									      *
 *  The Creator Of Legends System is (C) Copyright 1989 Alan Cox, All Rights  *
 *  Reserved.		  						      *
 *									      *
 \****************************************************************************/

/*
 *	System Functions
 *
 *	1.00	Original Version
 *	1.04	Added Pointer Validation Facilities
 *	1.05	Made Alloc clear the structure allocated
 *	1.10	Added Memory Statistics On Boot
 *	1.11	Fixed Link to null bug
 *	1.12	Added INHERIT to FindSub
 *	1.13	Added Comments
 *	1.14	Fixed Texts >32766 times
 *	1.15	Made it FreeText item names and also free text block when does
 *		a FreeText.
 *	1.16	Added Register Optimisations
 *	1.17	75 way hashed text list
 *	1.18	Added Save/Restore on NextInByClass for DOCLASS nesting
 *	1.19	Made FindInByClass etc do mask 0 correctly.
 *	1.20	Added QuickAllocText feature
 *	1.21	Tidied up Error crash recovery
 *	1.22	Bug fixes for 5.06
 *	1.23	Extended Error Logging
 *	1.24	Fixed SHARE and Save/Load
 *	1.25	Changed to reduce string load, due to unix malloc overhead
 *	1.26	Flushes log writes
 *	1.27	Deleting items now checks for tables and deletes them.
 *	1.28	Removed pointless uid logging
 *	1.29	Cleaned up for strict ANSI C
 */

#include "System.h"
#include "User.h"

extern USER UserList[];

Module  "System";
Version "1.29";
Author  "----*(A)";


/*
 *	Display An Error Message, called via the Error() macro
 */

void ErrFunc(er,mod,ver,line,file)
char *er,*mod,*ver,*file;
int line;
{
	short ct=0;
	static int err_yes=0;
	printf("MODULE: %s  VERSION: %s\n",mod,ver);
	printf("FILE: %s  LINE: %d\n",file,line);
	printf("ERROR LOGGED WAS\n");
	printf("%s.\n",er);
	Log("MODULE: %s  VERSION: %s",mod,ver);
	Log("FILE: %s  LINE: %d",file,line);
	Log("ERROR LOGGED WAS");
	Log("%s",er);
	if(err_yes==0&&post_boot)
	{
		err_yes++;
		printf("\n\nAttempting to rescue universe\n");
		while(ct<MAXUSER)
		{
			if(UserList[ct].us_Item)
				ExitUser(ct);
			ct++;
		}
		DisintegrateAll();
		DisintegrateAll();
		if(SaveSystem("gonebang.uni")==0)
		{
printf("\n\n-----------------------------------------------\n\n");
printf("System successfully saved as 'gonebang.uni'\n");
printf("CAUTION: Ensure this rescued game works correctly before replacing the \
older\nversions with it.\n");
		}
		else
			printf("Attempt to save failed.\n");
	}
	else
		printf("Cannot rescue game.\n");
	exit(1);
}

/*
 *	Log an error to the log file 
 */

void Log(char *fmt, ...)
{
	char *x;
	long v;
	va_list va;
	static FILE *file=NULL;
	if(file==NULL) file=fopen(LOG_FILE,"a");
	if(file==NULL)
		return;	/* Try to open log file */
		
	va_start(va, fmt);
	time(&v);
	x=ctime(&v);
	*strchr(x,'\n')=0;
	fprintf(file,"%s:",x);
	vfprintf(file, fmt, va);		/* Save error message */
	fprintf(file,"\n");			/* Return to tidy it */
	fflush(file);
	va_end(va);
}
	
/*
 *	Allocate a block of memory via the Allocate macro. The memory is
 *	cleared to 0 and if close to no free memory a warning is displayed
 */
	
char *AllocFunc(int x, char *mod, char *ver, int line, char *file)
{
	register char *a;
	register int n=0;
	a=malloc(x);		/* Request Memory */
	if(a==NULL)
	{
			ErrFunc("Out Of Memory",mod,ver,line,file);	/* Failed */
	}
	while(n<x)
		a[n++]=0;	/* Clear Memory */
	return(a);		/* Return block */
}


TPTR AllocText(char *x)
{
	return((TPTR)strdup(x));
}

TPTR AllocComment(char *x)
{
	return((TPTR)strdup(x));
}

TPTR QuickAllocText(char *x)
{
	return((TPTR)strdup(x));
}	


long TextNumber(register TPTR t)
{
	Error("TextNumber: Text not listed");
	return(0L);	/* Compiler happiness */
}

/*
 *	Return a string pointer given a TXT pointer
 */

char *TextOf(TPTR x)
{
	return((char *)x);
}

void FreeText(TPTR x)
{
	free((char *)x);
}


void FreeComment(TPTR x)
{
	free((char *)x);
}

/*	
 *	Fundamental Item Controllers
 *
 *	The item list works on a similar structure to the texts, a one way linked
 *	list. However to handle containment the system is far more complex in its
 *	potential twinings. Each item has a pointer to its parent (container),
 *	a pointer to the first thing in a linked list it contains, and a pointer
 *	to the next item in the linked list of items contained by its parent.
 */	

/*
 *	Disconnect An Item From Any Parent Objects
 */

int UnlinkItem(register ITEM *x)
{
	register ITEM *a;
#ifdef CHECK_ITEM
	CheckItem(x);
#endif
	if(O_FREE(x))		/* Already disconnected */
		return(0);
	if(O_CHILDREN(O_PARENT(x))==x)	/* First item special case */
	{
		O_CHILDREN(O_PARENT(x))=O_NEXT(x);	/* Move pointer on */
		O_PARENT(x)=NULL;		/* Clear our pointers */
		O_NEXT(x)=NULL;
		return(0);
	}
	a=O_CHILDREN(O_PARENT(x));
	if(a==NULL)				/* More cockup checks */
		Error("UnlinkItem: Parent Empty");
	while(O_NEXT(a))
	{
		if(O_NEXT(a)==x)		/* Unlink from list */
		{
			O_NEXT(a)=O_NEXT(x);
			O_PARENT(x)=NULL;
			O_NEXT(x)=NULL;
			return(0);
		}
		a=O_NEXT(a);
	}
	Error("UnlinkItem: Parent Does Not Contain Child");
}

/*
 *	Place an UnLinked item into a container. Note an UNLINKED item.
 */

int LinkItem(ITEM *a, ITEM *b)
{
#ifdef CHECK_ITEM
	CheckItem(a);
	if(b)
		CheckItem(b);
#endif
	if(!O_FREE(a))
		return(-1);	/* item not free yet */
	O_PARENT(a)=b;
	if(b)
	{
		O_NEXT(a)=O_CHILDREN(b);	/* Hook into chain */
		O_CHILDREN(b)=a;
	}
	else
		O_NEXT(a)=NULL;		/* Moving into nothingness */
	return(0);
}


/*
 *		ITEM CONTROLLERS
 */

ITEM *ItemList=NULL;		/* Global list of items, see above */

/*
 *	Create an item, setting up its name adj and noun fields. We have to set
 *	adj and noun so you can refer to it. If you set up an item with no 
 *	adjective and noun, you have a problem!
 */

ITEM *CreateItem(char *name, int ad, int no)
{
	register ITEM *a=Allocate(ITEM);/* Memory for the item  */
	O_PARENT(a)=NULL;		/* Starts in the void   */
	O_NEXT(a)=NULL;			/* Connected to nothing */
	O_CHILDREN(a)=NULL;		/* No children either   */
	O_PROPERTIES(a)=NULL;		/* No properties        */
	O_ADJECTIVE(a)=ad;		/* Adjective as asked   */
	O_NOUN(a)=no;			/* Noun as was asked    */
	a->it_Users=0;			/* Not yet locked into  */
	O_STATE(a)=0;			/* Set its state to zero*/
	a->it_MasterNext=ItemList;	/* Link into lists */
	ItemList=a;
	a->it_ActorTable=0;		/* Start with tables 0  */
	a->it_ActionTable=0;
	a->it_Class=0;			/* Clear class maskings */
	a->it_Perception=0;		/* Generally visiblish  */
	a->it_Name=AllocText(name);	/* allocate the text string */
	return(a);
}

/*
 *	Delete an object, very similar in many ways to freeing a text but with
 *	more requirements: Item must be EMPTY, IN VOID, UNLOCKED, and with NO
 *	properties
 */

int FreeItem(register ITEM *x)
{
	register ITEM *a;
#ifdef CHECK_ITEM
	CheckItem(x);
#endif
	/* Free tables first of all */
	if(x->it_ObjectTable!=NULL)
	{
		DeleteTable(x->it_ObjectTable);
		FreeTableHeader(x->it_ObjectTable);
		x->it_ObjectTable=NULL;
	}
	if(x->it_SubjectTable!=NULL)
	{
		DeleteTable(x->it_SubjectTable);
		FreeTableHeader(x->it_SubjectTable);
		x->it_SubjectTable=NULL;
	}
	if(x->it_DaemonTable!=NULL)
	{
		DeleteTable(x->it_DaemonTable);
		FreeTableHeader(x->it_DaemonTable);
		x->it_DaemonTable=NULL;
	}
	/* We must delete tables first - we may have a lock into ourself */
	if(x->it_Users)		/* Still in use */
		return(-1);
	if(!O_EMPTY(x))		/* Not empty */
		return(-2);
	if(O_PROPERTIES(x))
		return(-3);	/* Delete ALL Props First */
	if(!O_FREE(x))
		return(-4);	/* Still linked */
	if(x->it_Superclass)	/* Dump superclasses */
	{
		UnlockItem(x->it_Superclass);
	}
	if(ItemList==x)
	{
		ItemList=x->it_MasterNext;	/* Now delete the entry */
		FreeText(x->it_Name);
		free((char *)x);
		return(0);
	}
	a=ItemList;
	if(!a)
		Error("FreeItem: Empty Item List");
	while(a->it_MasterNext)
	{
		if(a->it_MasterNext==x)
		{
			a->it_MasterNext=x->it_MasterNext;
			FreeText(x->it_Name);
			free((char *)x);
			return(0);
		}
		a=a->it_MasterNext;
	}
	Error("FreeItem: Invalid Item Handle");
}

void LockItem(ITEM *x)	/* Mark an item 'in use' */
{
#ifdef CHECK_ITEM
	CheckItem(x);
#endif
	x->it_Users++;
}

void UnlockItem(ITEM *x)	/* Mark an item 'out of use' */
{
#ifdef CHECK_ITEM
	CheckItem(x);
#endif
	x->it_Users--;
	if((x->it_Users==0)&&(x->it_Perception==-1))	/* Pending deletion */
	{
		FreeItem(x);
		return;
	}
	if(x->it_Users<0)
	{
		Log("Can't unlock %s",NameOf(x));
		Error("Unlock: Item already free");
	}
}

void SetState(ITEM *x, short v)
{
#ifdef CHECK_ITEM
	CheckItem(x);
#endif
	x->it_State=v;
}

/*
 *	Change the vocabulary on an item NEVER set to -1,-1
 */

void SetVocab(ITEM *item, short ad, short no)
{
#ifdef CHECK_ITEM
	CheckItem(item);
#endif
	O_NOUN(item)=no;
	O_ADJECTIVE(item)=ad;
}

/*
 *	Test for a word matching
 */

int WordMatch(ITEM *i, short a, short n)
{
#ifdef CHECK_ITEM
/*	CheckItem(i);	*/	/* Removed because it made testing TOO slow */
#endif
	if((a==-1)&&(n==O_NOUN(i)))
		return(1);
	if((a==O_ADJECTIVE(i))&&(n==O_NOUN(i)))
		return(1);
	return(0);
}

/*
 *	Test if item is visible
 */

int CanSee(short pe, ITEM *it)
{
#ifdef CHECK_ITEM
/*	CheckItem(it);	*/	/* Removed because it made testing too slow */
#endif
	if(it->it_Perception>pe)
		return(0);
	return(1);
}

/*
 *	Find a matching item, anywhere in the game
 */

ITEM *FindMaster(short pe, short a, short n)
{
	register ITEM *i=ItemList;	/* Walk the entire list of items */
	while(i)
	{
		if((WordMatch(i,a,n))&&(CanSee(pe,i)))
			return(i);
		i=i->it_MasterNext;
	}
	return(NULL);
}

/*
 *	Find the next item in the game matching the words
 */

ITEM *NextMaster(short pe,register ITEM *i, short a, short n)
{
#ifdef CHECK_ITEM
	CheckItem(i);
#endif
	i=i->it_MasterNext;
	while(i)
	{
		if((WordMatch(i,a,n))&&(CanSee(pe,i)))
			return(i);
		i=i->it_MasterNext;
	}
	return(NULL);
}

/*
 *	Find the first item in container matching the words
 */

ITEM *FindIn(short pe, ITEM *i, short a, short n)
{
	if(i==NULL)
		return(NULL);
#ifdef CHECK_ITEM
	CheckItem(i);
#endif
	i=O_CHILDREN(i);
	while(i)
	{
		if((WordMatch(i,a,n))&&(CanSee(pe,i)))
			return(i);
		i=O_NEXT(i);
	}
	return(NULL);
}

/*
 *	Find the next item matching the words
 */

ITEM *NextIn(short pe, register ITEM *i, short a, short n)
{
#ifdef CHECK_ITEM
	CheckItem(i);
#endif
	i=O_NEXT(i);
	while(i)
	{
		if((WordMatch(i,a,n))&&(CanSee(pe,i)))
			return(i);
		i=O_NEXT(i);
	}
	return(NULL);
}

/*
 *	Properties: The substructure handlers.
 *
 *	Every substructure has a SUB entry at the start of the data (see 
 *	system.h), this is all that is used by these routines
 */

/*
 *	Find the first substructure of type 'key' in item.
 */

SUB *FindSub(ITEM *item, register short key)
{
	ITEM *b=NULL;
	register SUB *a=item->it_Properties;	/* Start of property list */
#ifdef CHECK_ITEM
	CheckItem(item);
#endif
	while(a)			/* Walk list */
	{
		if(a->pr_Key==key)
			return(a);	/* Found it */
		if(a->pr_Key==KEY_INHERIT)
			b=((INHERIT *)(a))->in_Master;	/* Note inheritance */
		a=a->pr_Next;
	}
	if(!post_boot)
		return(NULL);	/* Don't share while booting */
	if(b)
	{
		a=b->it_Properties;	/* Do properties if any	inherited */
		while(a)
		{
			if(a->pr_Key==key)
				return(a);
			a=a->pr_Next;
		}
	}
	return(NULL);
}

/*
 *	Find the next substructure of type 'key' in item.
 *	NOTE: This does not walk across two items. Thus an inherited chain
 *	and a non-inherited chain will not both be found. This is a 'feature'
 */

SUB *NextSub(SUB *sub, register short key)
{
	register SUB *a=sub->pr_Next;
	while(a)
	{
		if(a->pr_Key==key)
			return(a);
		a=a->pr_Next;
	}
	return(NULL);
}	

/*
 *	Allocate a substructure of type 'key' and size 'size', then add it to
 *	ITEM. Note the entry is added first and thus will be found first.
 *
 *	Both of these routines are almost always used indirectly by other setup
 *	code. (See SubHandler.c mainly)
 */

SUB *AllocSub(ITEM *item, short key, short size)
{
	SUB *a=(SUB *)malloc(size);
#ifdef CHECK_ITEM
	CheckItem(item);
#endif
	if(a==NULL)
		Error("Out Of Memory");
	a->pr_Next=item->it_Properties;		/* Link into list */
	item->it_Properties=a;
	a->pr_Key=key;
	return(a);
}

/*
 *	Free a substructure from an item, pretty much like freeing text
 */

void FreeSub(ITEM *item, register SUB *sub)
{
	register SUB *a=item->it_Properties;
#ifdef CHECK_ITEM
	CheckItem(item);
#endif
	if(a==NULL)
		Error("FreeSub: Item has no properties");
	if(a==sub)
	{
		item->it_Properties=sub->pr_Next;
		free((char *)sub);
		return;
	}
	while(a->pr_Next)
	{
		if(a->pr_Next==sub)
		{
			a->pr_Next=sub->pr_Next;
			free((char *)sub);
			return;
		}
		a=a->pr_Next;
	}
	Error("FreeProp: Property not in item given");
}

/*
 *	Test if item a contains item b to any depth (non recursive)
 */

int Contains(register ITEM *a, register ITEM *b)
/* true if a cont b */
{
	int ct=32;
#ifdef CHECK_ITEM
	CheckItem(a);
	CheckItem(b);
#endif
	while(O_PARENT(b)&& ct-- )
	{
		if(O_PARENT(b)==a)
			return(1);
		b=O_PARENT(b);		/* Go up a level */
	}
	return(0);
}

/*
 *	Find the first item contained in i matching words
 */

ITEM *FindContains(short pe, ITEM *i, short a, short n)
{
	register ITEM *b=FindMaster(pe,a,n);
#ifdef CHECK_ITEM
	CheckItem(i);
#endif
	while(b)
	{
		if(Contains(i,b))
			return(b);
		b=NextMaster(pe,b,a,n);
	}
	return(NULL);
}

/*
 *	Find the next item contained in i, after j
 */

ITEM *NextContains(short pe, ITEM *i, ITEM *j, short a, short n)
{
	register ITEM *b=NextMaster(pe,j,a,n);
#ifdef CHECK_ITEM
	CheckItem(i);
	CheckItem(j);
#endif
	while(b)
	{
		if(Contains(i,b))
			return(b);
		b=NextMaster(pe,b,a,n);
	}
	return(NULL);
}	

/*
 *	Return the position of an item in the master lists
 */

long MasterNumber(ITEM *x)
{
	register ITEM *a=ItemList;
	register int b=0;
	while(a)
	{
		if(a==x)
			return(b);
		b++;
		a=a->it_MasterNext;
	}
	Error("Invalid Item Handle");
}

/*
 *	Test if an item pointer is valid
 */

int ValidItem(ITEM *x)
{
	register ITEM *a=ItemList;
	while(a)
	{
		if(a==x)
			return(1);
		a=a->it_MasterNext;
	}
	return(0);
}

/*
 *	Count the number of items in the master lists.
 */

long CountItems(void)
{
	register ITEM *a=ItemList;
	register long b=0;
	while(a)
	{
		b++;
		a=a->it_MasterNext;
	}
	return(b);
}

static ITEM *Lfnd_NextPtr=NULL;		/* We have to cache one here */

/*
 *	Remember the next pointer, used by tables for context changing
 */

ITEM *GetNextPointer(void)
{
	return(Lfnd_NextPtr);
}

/*
 *	Restore the pointer
 */

void SetNextPointer(ITEM *x)
{
	Lfnd_NextPtr=x;
}

/*
 *	Find an item in an item, by classmask not by words
 */

ITEM *FindInByClass(short per, register ITEM *i, register short m)
{
	i=O_CHILDREN(i);
	while(i)
	{
		if((i->it_Class&m)&&(CanSee(per,i)))
		{
			Lfnd_NextPtr=O_NEXT(i);
			return(i);
		}
		if((m==0)&&(CanSee(per,i)))
		{
			Lfnd_NextPtr=O_NEXT(i);
			return(i);
		}
		i=O_NEXT(i);
	}
	return(NULL);
}

/*
 *	Find the next item in a container by classmask
 */

ITEM *NextInByClass(short per, register ITEM *i, register short m)
{
/*
 *	We don't use item->next, since user may have moved item. We DO NOT define
 *	what happens if the user moves item->next itself. (There must be a better
 *	way to do this, but I can't think of one right now...)
 */
	i=Lfnd_NextPtr;
	while(i)
	{
		if((i->it_Class&m)&&(CanSee(per,i)))
		{
			Lfnd_NextPtr=O_NEXT(i);
			return(i);
		}
		if((m==0)&&(CanSee(per,i)))
		{
			Lfnd_NextPtr=O_NEXT(i);
			return(i);
		}
		i=O_NEXT(i);
	}
	return(NULL);
}

static char *Valt(ITEM *x)
{
	if(ValidItem(x))
		return("");
	else
		return("<**INVALID**>");
}

void LineDump(void)
{
	extern ITEM *Item1,*Item2;
	static short called=0;	/* If we fail we skip second time */
	if(called==1)
		return;
	called=1;
	fprintf(stderr,"Verb %d  Adj1 %d  Noun1 %d  Prep %d  Adj2 %d  Noun2 %d\
\n",Verb,Adj1,Noun1,Prep,Adj2,Noun2);
	fprintf(stderr,"$ME is %lx %s\n",(unsigned long)Me(),Valt(Me()));
	fprintf(stderr,"$1  is %lx %s\n",(unsigned long)Item1,Valt(Item1));
	fprintf(stderr,"$2  is %lx %s\n",(unsigned long)Item1,Valt(Item2));
	fprintf(stderr,"$AC is %lx %s\n",(unsigned long)Actor(),Valt(Actor()));
	if(CurrentLine)
	{
		FPCurrentLine();
	}
}

#ifdef CHECK_ITEM

/*
 *	Check an item pointer to trap system errors
 */

int ICheck(register ITEM *item, int line, char *file)
{
	register ITEM *x=ItemList;
	extern ITEM DummyItem;
	while(x)
	{
		if(x==item)
			return(1);
		x=x->it_MasterNext;
	}
	if(item==&DummyItem)
		return(1);
	fprintf(stderr,"Pointer=%ld!\n", (unsigned long)item);
	LineDump();
	ErrFunc("Invalid ITEM *","<VALIDATOR>","1.00",line,file);
}

#else
int ICheck(register ITEM *item, int line, char *file){return(1);}
#endif

#ifdef CHECK_TXT

/*
 *	Check a text pointer to trap system errors
 */

int TCheck(register TPTR txt, int line, char *file)
{
	return(1);
}

#endif

