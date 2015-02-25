 
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

void reader_rcv_msg(int queue)
{
    packet data;
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
            printf("\n*** offset=%ld, size=%ld, buf:\n%s\n***",
				data.offset, data.size, data.buf);
			fflush(stdout);
            sleep(2);
        }
    }
}

int main(int argc, char** argv) {
    int queue;
    key_t keyqueue;
#if USE_FTOK
    keyqueue = ftok(FTOK_PATH,FTOK_CHAR);
#else
    keyqueue = 0x00FF00;
#endif
    
    sethandler(SIGINT,int_handler);
    
    if((queue = msgget(keyqueue,IPC_CREAT|IPC_EXCL|S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP))<0){
        perror("Create queue:");
        exit(EXIT_FAILURE);
    }
    
    reader_rcv_msg(queue);
    
    if(msgctl(queue,IPC_RMID,NULL)<0){
        perror("Remove queue:");
        exit(EXIT_FAILURE);
    }
    printf("queue removed\n");
    exit(EXIT_SUCCESS);
}
