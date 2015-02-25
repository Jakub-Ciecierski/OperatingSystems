#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "packet.h"
#include <signal.h>


void usage (char* info)
{
	printf("%s usage:\n",info);
	printf("%s fifo file \n",info);
}

void run_read(int fifo, int fd)
{
	int n;
	
	PACKET buffer;
	buffer.header.pid = getpid();
	
	do
	{
		n = read(fd,buffer.msg,PIPE_BUF);
		if ( n < 0)
		{
			perror("read()");
			exit(EXIT_FAILURE);
		}
		if(n < MSG_SIZE)
			memset(buffer.msg+n,0,MSG_SIZE-n);
		if(n > 0)
		{
			if(write(fifo, (void*)&buffer, sizeof(buffer) ) < 0 )
			{
				perror("write()");
				exit(EXIT_FAILURE);
			}
		}
			
	}while ( n == PIPE_BUF );
}

int main(int argc, char* argv[])
{
	if(argc !=3)
	{
		usage(argv[0]);
		return 0;
	}
	
	const char* MY_FIFO = argv[1];
	const char* MY_FILE = argv[2];
	int fifo,fd;
	
	/*
	printf("unlinking the fifo...\n");
	(void)unlink(MY_FIFO);
	
	printf("mkfifo...\n");
	if( mkfifo(MY_FIFO, 0666))
		return 1;
	*/
	printf("opening fifo file \n");
	if ( (fifo = open(MY_FIFO, O_WRONLY) ) < 0)
	{
		fprintf(stderr,"error with opening FIFO\n");
		return 1;
	}
	
	printf("opening output file \n");
	if ( (fd = open(MY_FILE, O_RDONLY) ) < 0)
	{
		fprintf(stderr,"error with opening FILE\n");
		return 1;
	}
	printf("FIFO opened\n");
	run_read(fifo, fd);
	
	
	close(fd);
	return 0;
	
	return 0;
}
