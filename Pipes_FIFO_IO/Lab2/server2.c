/*---server2.c---*/
#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#include "io.h"
#include "packet.h"

void usage(char *prg){
	fprintf(stderr,"USAGE: %s fpifo_file\n",prg);
}

void read_from_fifo(int fifo){
	ssize_t count, i;
	PACKET buffer;
	char *msg;
	msg=buffer.msg;
	do{
		count=bulk_read(fifo,(void *)&buffer,PIPE_BUF);
		if(count<0){
			perror("Read:");
			exit(EXIT_FAILURE);
		}
		if(count>0){
			printf("\nPID:%d-------------------------------------\n",
				buffer.header.pid);
			for(i=0;i<MSG_SIZE;i++)
				if(isalnum(msg[i]))
						if(TEMP_FAILURE_RETRY(write(STDOUT_FILENO,msg+i,1))<0){
						perror("Write:");
						exit(EXIT_FAILURE);
						}
		}
	}while(count==PIPE_BUF);
}

int main(int argc, char** argv) {
	int fifo;
	if(argc!=2) {
		usage(argv[0]);
		return EXIT_FAILURE;
	}

	if(mkfifo(argv[1], S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP)<0)
		if(errno!=EEXIST){
			perror("Create fifo:");
			exit(EXIT_FAILURE);
		}
	if((fifo=TEMP_FAILURE_RETRY(open(argv[1],O_RDONLY)))<0){
			perror("Open fifo:");
			exit(EXIT_FAILURE);
	}
	read_from_fifo(fifo);	
	if(TEMP_FAILURE_RETRY(close(fifo))<0){
			perror("Close fifo:");
			exit(EXIT_FAILURE);
	}
	return EXIT_SUCCESS;
}
