#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <wait.h>
#include <string.h>
#include "common.h"

volatile sig_atomic_t do_continue = 1;

void sig_handler(int sig)
{
	printf("Signal received: %d \n",sig);
	do_continue = 0;
}

void usage(char* name)
{
	printf("usage of: %s\n",name);
	printf("./%s child_count\n",name);
}

// sid - semaphore id
// psid - private semaphore id
// qid - msg queue id 
// nr_of_child - an index of child
// child_cnt - number of children
void child_work(int sid,int psid, int qid, int nr_of_child, int child_cnt)
{
	while(do_continue)
	{
		// critical point
		// simply receives the msg from the msg queue
		// and prints it to stdout
		packet pack;
		if(msgrcv ( qid, &pack, MSGSIZE, 1, 0 ) < 0 )
		{
			perror("msgsnd");
			exit(EXIT_FAILURE);
		}
		printf("Child: #%d, PID: %d, offset: %d, size: %d\n",nr_of_child,getpid(), pack.offset, pack.size);
		printf("%s\n",pack.buf);
		
		sleep(1);
		semOp(psid,0,1);
		// end critical point
	}
}

void parent_work(int sid, int psid, int qid, int child_cnt)
{
	printf("PARENT STARTING\n");
	while(do_continue)
	{
		printf("Setting private semaphore to -%d\n",child_cnt);
		semOp(psid,0,-child_cnt);	// Check when the round ends.
									//
									// Line 52 - will add 1 to semaphore value
									// each time child receives a msg.
									//
									// When psid semaphore hits 0 this function will un-hang
									// and will proceed to go to sleep - representing the end of the round
		
		printf("round ended, going to sleep\n");
		sleep(5);
		semOp(sid,0,child_cnt);		// line 74 in prod_1.c decreaments semaphore value
									// by 1 each time it sends a msg,
									// when the round ends we add another child_cnt value
									// to our semaphore so that prod can keep sending the msgs
	}
}

int main(int argc, char* argv[])
{
	if(argc !=2)
	{
		usage(argv[0]);
		exit(0);
	}
	int child_cnt = atoi(argv[1]);
	
	key_t semkey, msgkey;
	int sid, qid, psid;
	
	msgkey = 0x00FF00;
	semkey = 0x00FF01;
	
	// Creating msgqueue and semaphores
	if( (sid = semget(semkey, 1, IPC_CREAT | 0660 )) == -1)
	{
		perror("semget");
		exit(EXIT_FAILURE);
	}
	
	if( (psid = semget(IPC_PRIVATE, 1, IPC_CREAT | 0660 )) == -1)	//IPC_PRIVATE becouse we only use it in this program
	{																
		perror("semget");
		exit(EXIT_FAILURE);
	}
	
	if( (qid = msgget(msgkey,IPC_CREAT|S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP)) < 0 )
	{
		perror("msgget");
		exit(EXIT_FAILURE);
	}
	// End of creating msgqueue and semaphores
	
	sethandler(SIGINT,sig_handler);
	semInit(sid, 0, child_cnt); //(id of sem, index of sem, init value);
								// Initialize the semaphore with value equal to
								// the number of children receiving msgs
	
	semInit(psid,0,0);			// Initialize the private semaphore with value of 0
	
	int i;
	for(i = 0; i < child_cnt; i++){		// Create child_cnt children to do recieving
		if(fork()==0)
			child_work(sid,psid,qid,i+1,child_cnt);	// i+1 indicated the index of child - for printing
	}
	
	parent_work(sid,psid,qid,child_cnt);	// parent is resposible for restarting the round
	
	
	int stat;
	wait(&stat);
	
	// Remove msg queue
	if(msgctl(qid,IPC_RMID,NULL)<0){	
        perror("Remove queue:");
        exit(EXIT_FAILURE);
    }
    
    // Remove semaphore/s
    struct sembuf s;
    s.sem_flg = IPC_RMID;
    if(semctl(sid,&s,NULL)<0){
        perror("Remove queue:");
        exit(EXIT_FAILURE);
    }
    if(semctl(psid,&s,NULL)<0){
        perror("Remove queue:");
        exit(EXIT_FAILURE);
    }
    
    printf("queue and semaphore removed\n");
	return 0;
}
