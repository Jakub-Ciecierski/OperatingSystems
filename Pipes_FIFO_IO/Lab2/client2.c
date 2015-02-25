/*---client2.c---*/
#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "io.h"
#include "packet.h"


void usage(char *prg){
	fprintf(stderr,"USAGE: %s fifo_file file \n",prg);
}

void write_to_fifo(int fifo, int file){
	ssize_t count;
	PACKET buffer;
	char *msg;
	buffer.header.pid=getpid();
	msg=buffer.msg;
	
	do{
		count=bulk_read(file,msg,MSG_SIZE);
		if(count<0){
			perror("Read:");
			exit(EXIT_FAILURE);
		}
		if(count < MSG_SIZE) memset(msg+count,0,MSG_SIZE-count);
		if(count>0){
			if(bulk_write(fifo,(void *)&buffer,sizeof(buffer))<0){
			perror("Write:");
			exit(EXIT_FAILURE);
			}
		}
	}while(count==MSG_SIZE);
}

int main(int argc, char** argv) {
	int fifo,file;
	if(argc!=3) {
		usage(argv[0]);
		return EXIT_FAILURE;
	}

	if(mkfifo(argv[1], S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP)<0)
		if(errno!=EEXIST){
			perror("Create fifo:");
			exit(EXIT_FAILURE);
		}
	if((fifo=TEMP_FAILURE_RETRY(open(argv[1],O_WRONLY)))<0){
			perror("Open fifo:");
			exit(EXIT_FAILURE);
	}
	if((file=TEMP_FAILURE_RETRY(open(argv[2],O_RDONLY)))<0){
			perror("Open file:");
			exit(EXIT_FAILURE);
	}
	write_to_fifo(fifo,file);	
	if(TEMP_FAILURE_RETRY(close(file))<0){
			perror("Close file:");
			exit(EXIT_FAILURE);
	}
	if(TEMP_FAILURE_RETRY(close(fifo))<0){
			perror("Close fifo:");
			exit(EXIT_FAILURE);
	}
	return EXIT_SUCCESS;
}
