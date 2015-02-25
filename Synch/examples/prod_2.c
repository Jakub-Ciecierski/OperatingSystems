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

int main(int argc, char** argv) {
    
    int queue,sem;
    FILE * fp;
    key_t keyqueue,keysem;
    packet data;
    int i,j;
    
    if (argc != 2) {
        fprintf(stderr,"need 1 input argument (file name)\n");
        exit(EXIT_FAILURE);
    }

#if USE_FTOK
    keyqueue = ftok(FTOK_PATH,FTOK_CHAR);
    keysem = ftok(FTOK_PATH,FTOK_CHAR_S);
#else
    keyqueue = 0x00FF00;
    keysem= 0x00FF01;
#endif

    if((queue = msgget(keyqueue,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP))<0){
         perror("Can't get the msg queue:");
         exit(EXIT_FAILURE);
    }
    
    if((sem = semget(keysem,1,S_IRUSR|S_IWUSR))<0){
		perror("Create semaphores:");
		exit(EXIT_FAILURE);
    }
    
    fp = fopen(argv[1],"r");
    
    data.mtype = 1;
    while (fp != NULL && !feof(fp)) {
		data.offset=ftell(fp);
        i=0;
        while(i<PACKLEN-2&&(j=fgetc(fp))!=EOF)data.buf[i++]=(char)j;
        data.buf[i++]='\n';
        data.buf[i]='\0';
        data.size=i-1;
        printf("*** offset=%ld, size=%ld, buf:\n%s***\n",
			data.offset,data.size,data.buf);
        semOp(sem,0,-1);
        if(TEMP_FAILURE_RETRY(msgsnd(queue,&data,MSGSIZE,0))<0){
            perror("Write to msg queue:");
            exit(EXIT_FAILURE);
        }
        printf("message sent\n");
        sleep(2);
    }
    if (fp != NULL) fclose(fp);
    exit(EXIT_SUCCESS);
}
