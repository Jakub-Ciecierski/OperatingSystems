#include <stdio.h>
#include <stdlib.h>
#include <linux/ipc.h>
#include <linux/msg.h>
#include <sys/stat.h>
#include "common.h"

int main()
{
	int qid;
	key_t msgkey;
	
	msgkey = ftok("/home/",'c');
	
	printf("before msgget\n");
	if( (qid = msgget(msgkey,IPC_CREAT|S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP)) < 0 )
	{
		perror("msgget");
		exit(EXIT_FAILURE);
	}
	printf("after msgget\n");
	
	msg mymsg;
	
	int length = sizeof(msg) - sizeof(long);
	
	printf("before msgsnd\n");
	if(msgrcv ( qid, &mymsg, length, 1, 0 ) < 0 )
	{
		perror("msgsnd");
		exit(EXIT_FAILURE);
	}
	printf("received msg queue: salary: %lf\n",mymsg.salary);
}

