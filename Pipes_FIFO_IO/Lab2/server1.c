/* ---server1.c---*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

void usage(char *prg){
	fprintf(stderr,"USAGE: %s fifo_file\n",prg);
}

void read_from_fifo(int fifo){
	ssize_t count;
	char c;
	
	struct timespec tim,tim2;
	tim.tv_sec = 0;
	tim.tv_nsec = 5000000L;
	
	do{
		count=TEMP_FAILURE_RETRY(read(fifo,&c,1));
		
		nanosleep(&tim,&tim2);	
		
		if(count<0){
			perror("Read:");
			exit(EXIT_FAILURE);
		}
		if(count>0){
			if(TEMP_FAILURE_RETRY(write(STDOUT_FILENO,&c,1))<0){
			perror("Write:");
			exit(EXIT_FAILURE);
			}
		}
	}while(count>0);
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
	fprintf(stderr,"Fifo %s opened\n",argv[1]);
	read_from_fifo(fifo);	
	if(TEMP_FAILURE_RETRY(close(fifo))<0){
			perror("Close fifo:");
			exit(EXIT_FAILURE);
	}
	return EXIT_SUCCESS;
}
