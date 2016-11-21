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
 *	Command Parsing Logic
 *
 *	1.00	AGC	Created File
 *	1.01	AGC	Added ordinates
 *	1.02	AGC	Minor Fixes
 *	1.03	AGC	Fixes (incomplete) for noun,prep,verb of same text
 *	1.04	AGC	Sorted word list
 *	1.05	AGC	Knows / " ' : are one character tokens
 *	1.06	AGC	Buffers fixed to right size, dunno how it ever used to work....
 *	1.07	AGC	Strict ANSIfication
 */

#include "System.h"

Module "Parser";
Version "1.07";
Author "----*(A)";

/*
 *	The parser breaks the user input into words in the form requested
 *	by the user. It can
 *		Break a new word {skipping noise words}
 *		Decode a word
 *		Decode an {adjective}/noun pair {skipping unknowns}
 *		Get the rest of the input (through phrases)
 *		Skip to next phrase [,;. AND THEN define phrases]
 *
 */

WLIST *WordList=NULL;
PCONTEXT ParserData[MAXUSER];
short ParsingPersona;

/*
 *	Context Managers
 */

void SetPersona(int x)
{
	ParsingPersona=x;
}

PCONTEXT *GetContext(short u)
{
	if(u<0)
		return(NULL);
	return(&ParserData[u]);
}

void ProLoad(short prep, register int *ad, register int *no)
{
	switch(prep)
	{
		case 1: *ad=Me()->it_Adjective;
		        *no=Me()->it_Noun;
			break;

		case 2: *ad=ParserData[ParsingPersona].pa_It[0];
			*no=ParserData[ParsingPersona].pa_It[1];
			break;

		case 3: *ad=ParserData[ParsingPersona].pa_Them[0];
			*no=ParserData[ParsingPersona].pa_Them[1];
			break;

		case 4: *ad=ParserData[ParsingPersona].pa_Him[0];
			*no=ParserData[ParsingPersona].pa_Him[1];
			break;

		case 5: *ad=ParserData[ParsingPersona].pa_Her[0];
			*no=ParserData[ParsingPersona].pa_Her[1];
			break;

		case 6: *ad=ParserData[ParsingPersona].pa_There[0];
			*no=ParserData[ParsingPersona].pa_There[1];
			break;
		default:
			*ad=-1;
			*no=-1;
			break;
	}
}

void SetItData(short u, ITEM *x, short a, short n)
{
	if(u==-1)
		u=ParsingPersona;
	if((IsPlayer(x))&&(x!=Me()))
	{
		ParserData[u].pa_Them[0]=a;
		ParserData[u].pa_Them[1]=n;
		if(PlayerOf(x)->pl_Flags&PL_MALE)
		{
			ParserData[u].pa_Him[0]=a;
			ParserData[u].pa_Him[1]=n;
			return;
		}
		if(PlayerOf(x)->pl_Flags&PL_FEMALE)
		{
			ParserData[u].pa_Her[0]=a;
			ParserData[u].pa_Her[1]=n;
			return;
		}
		ParserData[u].pa_It[0]=a;
		ParserData[u].pa_It[1]=n;
		return;
	}
	if(IsObject(x))
	{
		if((ObjectOf(x)->ob_Flags&OB_NOIT)==0)
		{
			ParserData[u].pa_It[0]=a;
			ParserData[u].pa_It[1]=n;
		}
	}
	if(IsRoom(x))
	{
		ParserData[u].pa_There[0]=a;
		ParserData[u].pa_There[1]=n;
	}
}

/*
 *	Word Managers
 */


void AddWord(char *name, short code, short type)
{
	register WLIST *a=Allocate(WLIST);
	register WLIST *w,*p;
	a->wd_Text=malloc(strlen(name)+1);
	if(a->wd_Text==NULL)
		Error("Out Of Memory");
	strcpy(a->wd_Text,name);
	a->wd_Code=code;
	a->wd_Type=type;
/*
 *	Sort word list.. mostly for speed of AddVerb etc
 */
 	p=NULL;
 	w=WordList;
 	while(w!=NULL)
 	{
 		if(w->wd_Code>code)
 			break;
		p=w;
 		w=w->wd_Next;
 	}
	if(p==NULL)
	{
		a->wd_Next=WordList;
		WordList=a;
	}
	else
	{
		p->wd_Next=a;
		a->wd_Next=w;
	}
}

int FreeWord(char *name, register short type)
{
	register WLIST *a,*b;
	a=WordList;
	b=NULL;
	while(a)
	{
		if((a->wd_Type==type)&&(stricmp(a->wd_Text,name)==0))
		{
			if(b==NULL)
			{
				WordList=a->wd_Next;
				free((char *)a);
			}
			else
			{
				b->wd_Next=a->wd_Next;
				free((char *)a);
			}
			return(0);
		}
		b=a;
		a=a->wd_Next;
	}
	return(-1);
}

char *FindWText(register int num, register short type)
{
	register WLIST *a;
	a=WordList;
	if(num==-1)
		return("ANY");
	if(num==-2)
		return("NONE");
	while(a)
	{
		if((a->wd_Type==type)&&(a->wd_Code==num))
		{
			return(a->wd_Text);
		}
		a=a->wd_Next;
	}
	return("<UNSET>");
}

/*
 *	We work on two word lists Words holds all the words
 *	Noise holds specific words to skip
 */

WLIST *FindInList(register WLIST *list, register char *word, register short type)
{
	if(list==NULL)
		return(NULL);	/* NULL is handle for null list */
	while(list)
	{
		if(((type==0)||(list->wd_Type==type))&&(stricmp(list->wd_Text,word)==0))
		{
			return(list);
		}
		list=list->wd_Next;;
	}
	return(NULL);
}

char *BreakWord(register char *iptr, register char *fbuf, 
	WLIST *skiplist, short skiptype)	
/* returns next iptr  wbuf should be 128 bytes */
{
	if((*iptr=='.')||(*iptr==';')||(*iptr==','))
		return(NULL);		/* Phrase end */
	if((*iptr==':')||(*iptr=='"')||(*iptr=='/')||(*iptr=='\''))
	{
		*fbuf++=*iptr++;
		*fbuf=0;
		return(iptr);
	}
l1:	while(*iptr)
		if(isspace(*iptr))
			iptr++;
		else
			break;
	if(!*iptr)
		return(NULL);		/* List expired */
	if((*iptr=='.')||(*iptr==';')||(*iptr==','))
		return(NULL);		/* Phrase end */
	while(*iptr)
		if((!isspace(*iptr))&&(*iptr!=',')&&(*iptr!=';')&&(*iptr!='.'))
			*fbuf++=*iptr++;
		else
			break;
	*fbuf=0;
	if(stricmp(fbuf,"AND")==0)
		return(NULL);
	if(stricmp(fbuf,"THEN")==0)
		return(NULL);
	if(FindInList(skiplist,fbuf,skiptype))
		goto l1;
	return(iptr);
}

char *GetRestOfInput(register char *iptr, char *bfr)
{
	while(*iptr)
		if(isspace(*iptr))
			iptr++;
		else
			break;
	strcpy(bfr,iptr);
	iptr+=strlen(iptr);
	return(iptr);
}

char *FNxPhrs(register char *iptr)
{
	static char fb[512];
	register char *fbuf=fb;
	if(iptr==NULL)
		return(NULL);
l1:	while(*iptr)
		if(isspace(*iptr))
			iptr++;
		else
			break;
	if(!*iptr)
		return(NULL);		/* List expired */
	if((*iptr=='.')||(*iptr==';')||(*iptr==','))
	{
		iptr++;
		return(iptr);		/* Phrase end */
	}
	while(*iptr)
		if(!isspace(*iptr))
			*fbuf++=*iptr++;
		else
			break;
	*fbuf=0;
	if(stricmp(fb,"AND")==0)
		return(iptr);
	if(stricmp(fb,"THEN")==0)
		return(iptr);
	goto l1;
}

/*
 *	Globals For Normal Parsing
 */

char *WordPtr;
char WordBuffer[514];	

WLIST *GetWord(void)
{
	WLIST *a;
	WordPtr=BreakWord(WordPtr,WordBuffer,WordList,WD_NOISE);
	if(!WordPtr)
		return((WLIST *)-1);
	a=FindInList(WordList,WordBuffer,0);
	return(a);
}

int GetOrd(void)
{
	char *WRem=WordPtr;
	WLIST *a=GetWord();
	if(a==NULL)
	{
		WordPtr=WRem;
		return(1);
	}
	if(a==(WLIST *)-1)
	{
		WordPtr=WRem;
		return(1);
	}
	if(a->wd_Type!=WD_ORDIN)
	{
		WordPtr=WRem;
		return(1);
	}
	return(a->wd_Code);
}
	
char *GetParsedWord(void)
{
	return(WordPtr=BreakWord(WordPtr,WordBuffer,WordList,WD_NOISE));
}

void GetAll(void)
{
	WordPtr=GetRestOfInput(WordPtr,WordBuffer);
}

int GetNumber(void)
{
	int a;
	WordPtr=BreakWord(WordPtr,WordBuffer,WordList,WD_NOISE);
	if(!WordPtr)
		return(-1);
	if(sscanf(WordBuffer,"%d",&a)==0)
		return(-1);
	return(a);
}

int GetThing(int *ad, int *no)
{
	char *a=WordPtr;	/* Save of */
	WLIST *b;
	WLIST *t;
	int posnoun=-1;
	char *backptr;
	*ad=-1;
l1:	backptr=WordPtr;
	b=GetWord();
	if(b==(WLIST *)-1)
	{
		WordPtr=a;
		return(-1);
	}
	if(b==NULL)
		goto l1;	/* Dud word */
	if(FindInList(WordList,WordBuffer,WD_PREP)&&
		FindInList(WordList,WordBuffer,WD_NOUN))
	{
		if(GetWord()==(WLIST *)-1)
		{
			WordPtr=backptr;
			b=GetWord();	/* Use noun */
		}
		else
		{
			WordPtr=backptr;
			*no= -1;
			return(-1);
		}
	}
	t=FindInList(WordList,WordBuffer,WD_PRONOUN);
	if(t)
	{
		ProLoad(t->wd_Code,ad,no);
		return(0);
	}
	if(FindInList(WordList,WordBuffer,WD_ADJ))
	{
		t=FindInList(WordList,WordBuffer,WD_ADJ);
		b=t;
	}
	else
		goto l2;
	t=FindInList(WordList,WordBuffer,WD_NOUN);
	if(t)
		posnoun=t->wd_Code;	/* It might still be noun */
	if(b)
		*ad=b->wd_Code;
l3:	a=WordPtr;
	b=GetWord();
	if(b==(WLIST *)-1)
	{
		WordPtr=a;
		if(posnoun)
		{
			*ad=-1;
			*no=posnoun;
			return(0);
		}
		return(-1);
	}
	if(b==NULL)
		goto l3;	/* Dud word */
l2:	if(!(b=FindInList(WordList,WordBuffer,WD_NOUN)))
	{
		WordPtr=a;
		if(posnoun!=-1)
		{
			*no=posnoun;	/* Adj/Noun clasher - noun form */
			*ad=-1;
			return(0);
		}
		if(!b)
			return(-1);
	}
	*no=b->wd_Code;
	return(0);
}

void SkipPrep(void)		/* Skip any preposition which is next word */
{
	char *a=WordPtr;
	WLIST *b=GetWord();
	if(b==(WLIST *)-1)
	{
		WordPtr=a;
		return;
	}
	if(b==NULL)
	{
		WordPtr=a;
		return;
	}
	if(b->wd_Type!=WD_PREP)
	{
		WordPtr=a;
		return;
	}
}

int GetPrep(void)
{
	char *a=WordPtr;
	register WLIST *b;
l1:	b=GetWord();
	if(b==(WLIST *)-1)
	{
		WordPtr=a;
		return(-1);
	}
	if(b==NULL)
		goto l1;	/* Dud word */
	if(b->wd_Type!=WD_PREP)
	{
		b=FindInList(WordList,WordBuffer,WD_PREP);
		if(!b)
		{
			WordPtr=a;
			return(-1);
		}
	}
	return(b->wd_Code);
}

int GetVerb(void)
{
	char *a=WordPtr;
	register WLIST *b;
	b=GetWord();
	if(b==(WLIST *)-1)
	{
		WordPtr=a;
		return(-1);
	}
	if(b==NULL)
	{
		WordPtr=a;
		return(-1);
	}
	if(b->wd_Type!=WD_VERB)
	{
		b=FindInList(WordList,WordBuffer,WD_VERB);
		if(!b)
		{
			WordPtr=a;
			return(-1);
		}
	}
	return(b->wd_Code);
}

char *NextPhrase(void)
{
	WordPtr=FNxPhrs(WordPtr);
	return(WordPtr);
}

