#define _GNU_SOURCE 
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

void semInit(int sem, int startValue, int semNum){
        union semun initUnion;
        initUnion.val = startValue;
        if (semctl(sem,semNum,SETVAL,initUnion) == -1){
                perror("Semafore initialization:");
                exit(EXIT_FAILURE);
        }
}

void semOp (int sem, int semNum,int value){
        struct sembuf buffer;
        buffer.sem_num = semNum;
        buffer.sem_op = value;
        buffer.sem_flg = 0;
        if (TEMP_FAILURE_RETRY(semop(sem,&buffer,1)) == -1){
                perror("Semaphore operation:");
                exit(EXIT_FAILURE);
        }
}

//returns -1 if error is EAGAIN
int semOpNoWait (int sem, int semNum,int value){
        struct sembuf buffer;
        buffer.sem_num = semNum;
        buffer.sem_op = value;
        buffer.sem_flg = IPC_NOWAIT;
        if (TEMP_FAILURE_RETRY(semop(sem,&buffer,1)) == -1){
            if (errno == EAGAIN) {
                return -1;
            } else {   
                perror("Semaphore operation:");
                exit(EXIT_FAILURE);
            }
        }
        return 0;
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
