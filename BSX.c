#include "System.h"
#include "User.h"

/*
 *	BSX.c:		Manager for BSX graphics objects. At the moment this is simply a case of tagging the items in memory
 *			and keeping a linked list of them.
 *
 *
 *	1.00	AGC	Added BSX support to AberMUD 5.21
 *	1.01	AGC	-1 can be used to specify remove this object
 *	1.02	AGC 	Extra newlines between commands removed. Purge done on
 *			object loads.
 *	1.03	AGC	ANSIfication
 *
 */

Module  "BSX";
Author  "Alan Cox";
Version "1.03";

extern USER UserList[];
extern char CmdBuffer[];

static BSXImage *BSXImageList=NULL;

BSXImage *BSXAllocate(char *name, int size)
{
	BSXImage *image=Allocate(BSXImage);
	strncpy(image->bsx_Identifier,name,8);
	image->bsx_Identifier[8]=0;
	image->bsx_Data=(unsigned char *)malloc(size);
	if(image->bsx_Data==NULL)
		Error("Out of memory");
	image->bsx_DataSize=size;
	image->bsx_Next=BSXImageList;
	BSXImageList=image;
	return(image);
}

BSXImage *BSXFindFirst(void)
{
	return(BSXImageList);
}

BSXImage *BSXFindNext(BSXImage *image)
{
	return(image->bsx_Next);
}

BSXImage *BSXFind(char *name)
{
	BSXImage *image=BSXFindFirst();
	while(image!=NULL)
	{
		if(strcmp(name,image->bsx_Identifier)==0)
			return(image);
		image=BSXFindNext(image);
	}
	return(NULL);
}


void BSXDelete(BSXImage *i)
{
	if(i==BSXImageList)
	{
		BSXImageList=i->bsx_Next;
	}
	else
	{
		BSXImage *walk=BSXFindFirst();
		while(walk->bsx_Next!=NULL)
		{
			if(walk->bsx_Next==i)
			{
				walk->bsx_Next=i->bsx_Next;
				break;
			}
		}
		if(walk->bsx_Next==NULL)
			Error("Invalid BSXImage pointer");
		walk=walk->bsx_Next;
	}
	free(i->bsx_Data);
	free(i);
}

static int HexDecode(char x)
{
	if(x>='0'&&x<='9')
		return(x-'0');
	if(x>='A'&&x<='F')
		return(x-'A'+10);
	if(x>='a'&&x<='f')
		return(x-'a'+10);
	return(-1);
}

int BSXEncodePair(char *buf)
{
	int result;
	int v;
	v=HexDecode(*buf++);
	if(v==-1)
		return(-1);
	result=v*16;
	v=HexDecode(*buf);
	if(v==-1)
		return(-1);
	result=result+v;
	return(result);
}

void BSXDecodePair(unsigned char in, char *out)
{
	static char hexify[]="0123456789ABCDEF";
	*out++=hexify[in/16];
	*out=hexify[in&0x0F];
}

/*
 *	Load and encode a BSX image from a path
 */

BSXImage *BSXLoadImage(char *name, char *path)
{
	FILE *f=fopen(path,"r");
	BSXImage *image;
	unsigned char *data;
	int size;
	int code;
	char buf[2];
	if(f==NULL)
		return(NULL);
	if(fseek(f,0L,2)==-1)
	{
		fclose(f);
		return(NULL);
	}
	size=(int)ftell(f);
	rewind(f);
	size/=2;	/* Hex cost */
	image=BSXAllocate(name,size);
	if(image==NULL)
	{
		fclose(f);
		return(NULL);
	}	
	data=image->bsx_Data;
	while(fread(buf,2,1,f)==1)
	{
		code=BSXEncodePair(buf);
		if(code==-1)
		{
			BSXDelete(image);
			fclose(f);
			return(NULL);
		}
		*data++=code;
	}
	fclose(f);
	return(image);
}

void Cmd_DeleteBSX(ITEM *i)
{
	BSXImage *image;
	if(!ArchWizard(i))
	{
		SendItem(i,"Pardon ?\n");
		return;
	}
	if(GetWord()==(WLIST *)(-1))
	{
		SendItem(i,"Yes, but which BSX object ?\n");
		return;
	}
	image=BSXFind(WordBuffer);
	if(image==NULL)
	{
		SendItem(i,"Unknown BSX object.\n");
		return;
	}
	BSXDelete(image);
	SendItem(i,"Ok.\n");
}

void Cmd_LoadBSX(ITEM *i)
{
	BSXImage *image;
	int u=0;
	if(!ArchWizard(i))
	{
		SendItem(i,"Pardon ?\n");
		return;
	}
	if(GetWord()==(WLIST *)(-1))
	{
		SendItem(i,"Yes, but what shall I call it.\n");
		return;
	}
	if(BSXFind(WordBuffer)!=NULL)
	{
		SendItem(i,"BSX item already exists.\n");
		return;
	}
	strcpy(CmdBuffer,WordBuffer);
	GetAll();
	if(*WordBuffer==0)
	{
		SendItem(i,"Load which file.\n");
		return;
	}
	image=BSXLoadImage(CmdBuffer,WordBuffer);
	if(image==NULL)
	{
		SendItem(i,"Load failed.\n");
		return;
	}
	SendItem(i,"Ok.\n");
	/* Invalidate user caches */
	while(u<MAXUSER)
	{
		if(UserList[u].us_Port!=NULL)
		{
			SendTPacket(UserList[u].us_Port,PACKET_BSXSCENE,"@PUR");
			SendTPacket(UserList[u].us_Port,PACKET_BSXSCENE,CmdBuffer);
			SendTPacket(UserList[u].us_Port,PACKET_BSXSCENE,".");
		}
		u++;
	}
}

void Cmd_ListBSX(ITEM *i)
{
	BSXImage *image;
	if(!ArchWizard(i))
	{
		SendItem(i,"Pardon ?\n");
		return;
	}
	image=BSXFindFirst();
	while(image!=NULL)
	{
		SendItem(i,"%s\n",image->bsx_Identifier);
		image=BSXFindNext(image);
	}	
}

void Cmd_ShowBSX(ITEM *i)
{
	BSXImage *image;
	if(!ArchWizard(i))
	{
		SendItem(i,"Pardon ?\n");
		return;
	}
	if(GetWord()==(WLIST *)(-1))
	{
		SendItem(i,"Yes, but which BSX object ?\n");
		return;
	}
	image=BSXFind(WordBuffer);
	if(image==NULL)
	{
		SendItem(i,"Unknown BSX object.\n");
		return;
	}
	if(!IsUser(i))
		return;
	SendTPacket(UserList[UserOf(i)].us_Port,PACKET_BSXSCENE,"@SCE");
	SendTPacket(UserList[UserOf(i)].us_Port,PACKET_BSXSCENE,image->bsx_Identifier);
	SendTPacket(UserList[UserOf(i)].us_Port,PACKET_BSXSCENE,".@RFS");
	SendItem(i,"Ok.\n");
}


/*
 *	Client callback processor 
 */

void BSXDecompSend(int user, BSXImage *image)
{
	/* Use the CmdBuffer again for this - 512 bytes of work space */
	int ct=0;
	int cto=0;
	unsigned char *ptr=image->bsx_Data;
	while(ct<image->bsx_DataSize)
	{
		BSXDecodePair(*ptr++,&CmdBuffer[cto]);
		cto+=2;
		if(cto==510)
		{
			CmdBuffer[cto]=0;
			SendTPacket(UserList[user].us_Port,PACKET_BSXSCENE,CmdBuffer);
			cto=0;
		}
		ct++;
	}
	if(cto!=0)
	{
		CmdBuffer[cto]=0;
		SendTPacket(UserList[user].us_Port,PACKET_BSXSCENE,CmdBuffer);
	}
/*	SendTPacket(UserList[user].us_Port,PACKET_BSXSCENE,"\n");*/
}

void Handle_BSXPacket(int user, char *data)
{
	BSXImage *i;
	if(strlen(data)<4)
		return;
	if(strncmp(data,"#RQS",4)==0||strncmp(data,"#RQO",4)==0)
	{
		char *t=data+5;
		while(*t!='.'&&*t)
			t++;
		*t=0;
		i=BSXFind(data+5);
		if(i==NULL)
		{
			/* Whoops it doesn't exist.. that shouldn't happen. */
			return;
		}
		if(strncmp(data,"#RQS",4)==0)
			SendTPacket(UserList[user].us_Port,PACKET_BSXSCENE,"@DFS");
		else
			SendTPacket(UserList[user].us_Port,PACKET_BSXSCENE,"@DFO");
		SendTPacket(UserList[user].us_Port,PACKET_BSXSCENE,i->bsx_Identifier);
		SendTPacket(UserList[user].us_Port,PACKET_BSXSCENE,".");
		BSXDecompSend(user,i);
		SendTPacket(UserList[user].us_Port,PACKET_BSXSCENE,"@RFS");
		return;
	}
	/* Nothing else really matters */
}

/* 
 *	BSX Database actions
 */

void Act_BSXScene(void)
{
	ITEM *m=Me();
	int u;
	char *t;
	if(!IsUser(m))
	{
		ArgText();
		return;
	}
	u=UserOf(m);
	t=TextOf(ArgText());
	if(!IsBSX(u))
		return;
	if(BSXFind(t)==NULL)
		SendUser(u,"BSXError - unknown '%s'.\n",t);
	SendTPacket(UserList[u].us_Port,PACKET_BSXSCENE,"@SCE");
	SendTPacket(UserList[u].us_Port,PACKET_BSXSCENE,t);
	SendTPacket(UserList[u].us_Port,PACKET_BSXSCENE,".@RFS");
}
	
void Act_BSXObject(void)
{
	ITEM *m=Me();
	int u;
	char buf[128];
	int p,d;
	char *t;

	if(!IsUser(m))
	{
		ArgNum();
		ArgNum();
		ArgText();
		return;
	}
	u=UserOf(m);
	p=ArgNum();
	d=ArgNum();
	t=TextOf(ArgText());
	if(!IsBSX(u))
		return;
	if(BSXFind(t)==NULL)
		SendUser(u,"BSXError - unknown '%s'.\n",t);
	if(p==-1)	/* RMO */
	{
		sprintf(buf,"@RMO%s.@RFS",t);
	}
	else
	{
		/* Purge fixes bug in older BSX clients */
		sprintf(buf,"@PUR%s.@VIO%s.",t,t);
		t=buf+strlen(buf);
		BSXDecodePair((unsigned char)p,t);
		BSXDecodePair((unsigned char)p,t+2);
		strcat(buf,"@RFS");
		t[4]=0;
	}
	SendTPacket(UserList[u].us_Port,PACKET_BSXSCENE,buf);
}
