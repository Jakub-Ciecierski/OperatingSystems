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


void usage(char* name)
{
	printf("usage of: %s\n",name);
	printf("./%s filename\n",name);
}

int main(int argc, char* argv[])
{
	if(argc !=2)
	{
		usage(argv[0]);
		exit(0);
	}

	key_t semkey, msgkey;
	int sid, qid;
	packet data;
	FILE* fp;
	
	msgkey = 0x00FF00;
	semkey = 0x00FF01;
	
	// Open msg queue and semaphore 
	if( (sid = semget(semkey, 1, IPC_CREAT | 0660 )) == -1)
	{
		perror("semget");
		exit(EXIT_FAILURE);
	}
	if( (qid = msgget(msgkey,IPC_CREAT|S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP)) < 0 )
	{
		perror("msgget");
		exit(EXIT_FAILURE);
	}
	
	printf("opening file...\n");
	fp = fopen(argv[1],"r");
	printf("file opened\n");
	if(fp == NULL)
		printf("file is null\n");
		
	int i,j;
	data.mtype = 1;
    while (fp != NULL && !feof(fp)) {
		data.offset=ftell(fp);		// save the current position of the file
        i=0;
        
        while(i<PACKLEN-2&&(j=fgetc(fp))!=EOF)		// fgetc returns next character from the stream
			data.buf[i++]=(char)j;					// as unsigned char cast to int.
													// Thus the need of explicit cast.
        data.buf[i++]='\n';	
        data.buf[i]='\0';		// string must be null terminated !
        data.size=i-1;
        printf("*** offset=%ld, size=%ld, buf=\n%s***\n",
			data.offset,data.size,data.buf);
        if((msgsnd(qid,&data,MSGSIZE,0))<0){	// Send the msg to msg queue.
            perror("Write to msg queue:");		// The MSGSIZE defined in common.h
            exit(EXIT_FAILURE);
        }
        semOp(sid,0,-1);	// Notify the consumer of sent msg.
							// When this semaphore's value hits 0
							// this function will hang.
							// Parent in consumer will reset the value
							// when new round starts.
        printf("message sent\n");
        sleep(3);
    }
	if (fp != NULL)
		fclose(fp);
	return 0;
}
