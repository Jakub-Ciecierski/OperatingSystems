/*---client1.c---*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include "io.h"

void usage(char *prg){
	fprintf(stderr,"USAGE: %s fifo_file file \n",prg);
}

void write_to_fifo(int fifo, int file){
	ssize_t count;
	char buf[PIPE_BUF];
	
	do{
		count=bulk_read(file,buf,PIPE_BUF);
		if(count<0){
			perror("Read:");
			exit(EXIT_FAILURE);
		}
		if(count < PIPE_BUF) memset(buf+count,0,PIPE_BUF-count);
		if(count>0){
			if(bulk_write(fifo,buf,PIPE_BUF)<0){
				exit(EXIT_FAILURE);
				perror("Write:");
			}
		}
	}while(count==PIPE_BUF);
}

int main(int argc, char** argv) {
	int fifo,file;
	if(argc!=3) {
		usage(argv[0]);
		return EXIT_FAILURE;
	}

	/*
	if(mkfifo(argv[1], S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP)<0)
		if(errno!=EEXIST){
			perror("Create fifo:");
			exit(EXIT_FAILURE);
		}
	*/
	
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
