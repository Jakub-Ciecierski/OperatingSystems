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
	mymsg.mtype = 1;
	mymsg.request = 1;
	mymsg.salary = 1000.00;
	printf("after mymsg\n");
	int length = sizeof(msg) - sizeof(long);
	
	printf("before msgsnd\n");
	if( msgsnd (qid,&mymsg, length, 0) < 0 )
	{
		perror("msgsnd");
		exit(EXIT_FAILURE);
	}
}
