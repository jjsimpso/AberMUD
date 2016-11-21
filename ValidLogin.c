/*
 *	This function should be customised according to the actual
 *	load parameters you wish to enforce	
 */

#include <time.h>
#include <stdio.h>

int MaxSlot(void)
{
	long t;
	struct tm *tv;
	time(&t);
	tv=localtime(&t);
	if(tv==NULL)
		return(0);
	if(tv->tm_hour<7)
		return(20);
	if(tv->tm_hour<21)
		return(12);
	if(tv->tm_hour<23)
		return(16);
	if(tv->tm_hour==23)
		return(20);
	return(8);
}
