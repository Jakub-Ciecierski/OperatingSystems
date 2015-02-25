 
#define _GNU_SOURCE 
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include "common.h"

volatile sig_atomic_t do_continue = 1;

void int_handler(int signo)
{
    printf("Signal %d received, ending\n",signo);
    do_continue = 0;
}

void reader_rcv_msg(int queue,int sem, int n_value)
{
	long PID;
    packet data;
    int toread = n_value;
    int type = 0;
    while(do_continue) {
        if((msgrcv(queue,&data,MSGSIZE,type,0))<0){
			if(errno==EINTR) continue;
            perror("msgrcv:");
            if(kill(0,SIGINT)<0) {
                perror("Kill:");
                exit(EXIT_FAILURE);
            }
        } else {
            printf(ANSI_COLOR_GREEN "*** offset=%ld, size=%ld:\n" ANSI_COLOR_RESET "%s\n***\n",
				data.offset, data.size, data.buf);
			fflush(stdout);
            toread--;
            sleep(2);
        }
        if(toread == 0) {
            printf(ANSI_COLOR_RED "I received %d messages, renewing phase\n" ANSI_COLOR_RESET,n_value);
			fflush(stdout);
            toread = n_value;
            sleep(10);
            semOp(sem,0,n_value);
        }
    }
}   

int main(int argc, char** argv) {
    int queue,sem;
    key_t keyqueue,keysem;
    int n_value;
#if USE_FTOK
    keyqueue = ftok(FTOK_PATH,FTOK_CHAR);
    keysem = ftok(FTOK_PATH,FTOK_CHAR_S);
#else
    keyqueue = 0x00FF00;
    keysem= 0x00FF01;
#endif
    
    if(argc != 2) {
        fprintf(stderr,"must use 1 parameter, length of phase\n");
        exit(EXIT_FAILURE);
    }
    n_value = atoi(argv[1]);
    if(n_value <1) {
		fprintf(stderr,"n<1\n");
		exit(EXIT_FAILURE);
	}
    
    sethandler(SIGINT,int_handler);
    
    if((sem = semget(keysem,1,IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR))<0){
                perror("Create semaphores:");
                exit(EXIT_FAILURE);
    }
    semInit(sem,n_value,0);
    
    if((queue = msgget(keyqueue,IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP))<0){
        perror("Create queue:");
        exit(EXIT_FAILURE);
    }
    
    reader_rcv_msg(queue,sem,n_value);
    
    //Removing queues and semaphores
    if(semctl(sem,0,IPC_RMID)<0){
        perror("Remove semaphores:");
        exit(EXIT_FAILURE);
    }
    sleep(1);
    if(msgctl(queue,IPC_RMID,NULL)<0){
        perror("Remove queue:");
        exit(EXIT_FAILURE);
    }
    printf("msg queue and semaphore set removed\n");
    exit(EXIT_SUCCESS);
}
