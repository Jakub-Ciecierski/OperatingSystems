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

void semInit (int sid,int semNum, int initVal)
{
	// set initial value to the semaphore
	union semun arg;
	arg.val = initVal;
	if(semctl (sid, semNum, SETVAL, arg) == - 1)
		return -1;
}

void semOp (int sid,int semNum, int value)
{
	struct sembuf sem_buf;
	sem_buf.sem_num = semNum;
	sem_buf.sem_op = value;
	sem_buf.sem_flg = SEM_UNDO;
	if(semop(sid, &sem_buf, 1) == -1){
		perror("semop()");
		exit(EXIT_FAILURE);
	}
}

int getSemVal( int sid, int semnum )
{
	return( semctl(sid, semnum, GETVAL, 0));
}

void sethandler(int sig, void (*f) (int))
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = f;
    if (sigaction(sig, &sa, NULL) == -1){
        perror("sigaction:");
        exit(EXIT_FAILURE);
	}
}
