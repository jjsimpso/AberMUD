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
 *	Conditional Exit Logic Evaluator
 *
 *	1.00	AGC	Created this file, to replace stub version
 */

#include "System.h"

Module  "Exit Logic";
Version "1.00";
Author  "Alan Cox";


int TestCondExit(e)
CONDEXIT *e;
{
	TABLE *t=FindTable(e->ce_Table);
	if(t!=NULL)
		if(ExecBackground(t,Me())==-1)
			return(0);
	return(1);
}
