#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

#define MY_FIFO "MYFIFO"

volatile sig_atomic_t _continue = 1;

void set_sig_handler(int signum,void(*handler)(int))
{
	struct sigaction sa;
	memset(&sa,0x00,sizeof(struct sigaction));
	
	sa.sa_handler = handler;
	if(-1==sigaction(signum,&sa,NULL))
		printf("Error with signal : %d",signum);		
}

void signal_handler(int sig)
{
	_continue = 0;
}

int main(int argc, char* argv[])

	char buffer[80];
	int fd, n, m;
	
	set_sig_handler(SIGINT, signal_handler);
	
	printf("unlinking the fifo...\n");
	(void)unlink(MY_FIFO);
	
	printf("setting the umask...\n");
	umask(0);
	
	printf("mkfifo...\n");
	if( mkfifo(MY_FIFO, 0666))
		return 1;

	printf("opening fifo file \n");
	if ( (fd = open(MY_FIFO, O_RDONLY | O_NONBLOCK)) < 0)
	{
		fprintf(stderr,"error with opening WR_FIFO\n");
		return 1;
	}
	
	printf("FIFO opened\n");
	
	while(_continue == 1)
	{
		while( (n=read(fd,buffer,sizeof(buffer)))> 0 )	// 0 for stdin, 1 stdout
		{
			if( (m=write(1,buffer,n)) != n)
			{
				fprintf(stderr,"sv error with writing info RD_FIFO");
				return 1;	
			}
			else
				printf("read from WR_FIFO into RD_FIFO\n");
		}
	} 
	printf("Prepering shut-down\nunlinking the fifo...\n");
	(void)unlink(MY_FIFO);
	printf("Shutting down..\n");
	return 0;
}

