#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

/*
 *	AberMUD boot up program. 
 *
 *	1.00	AGC	Prehistory	Original boot up program
 *	1.01	AGC	27/02/93	Passes arguments on
 *
 */

int main(int argc,char *argv[])
{
	long t,t2;
	int pid;
	printf("Aberystwyth Multi-User Dungeon (Revision 5.21 BETA 5)\n\
(c) Copyright 1987-1993, Alan Cox\n\
\n");
	close(0);
	close(1);
	ioctl(2,TIOCNOTTY,0);
	close(2);
	setpgrp();
	if(fork()!=0)
		exit(1);
	open("server_log",O_WRONLY|O_CREAT|O_APPEND,0600);
	dup(0);
	dup(0);
	while(1)
	{
		time(&t);
		argv[0]="AberMUD 5.21 Beta1";
		pid=fork();
		if(pid==0)
		{
			execvp("./server",&argv[0]);
			perror("./server");
			exit(1);
		}
		else
			if(pid!= -1)
				wait(NULL);
		time(&t2);
		if(t2-t<10)
		{
			printf("Spawning too fast - error ??\n");
			exit(1);
		}
	}
}
