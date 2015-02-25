#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

#define WR_FIFO "WRFIFO"
#define RD_FIFO "RDFIFO"

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
{
	int stat_ret;
	struct stat stat_buffer;
	
	
	
	char buffer[80];
	int fd_wr, fd_rd, n, m;
	set_sig_handler(SIGALRM, signal_handler);
	set_sig_handler(SIGINT, signal_handler);
	
	printf("unlinking the fifo...\n");
	(void)unlink(WR_FIFO);
	(void)unlink(RD_FIFO);
	
	stat_ret = stat(RD_FIFO, &stat_buffer);
	printf("The stats before creating fifo: %d\n",stat_ret);
	
	printf("setting the umask...\n");
	umask(0);
	
	printf("mkfifo...\n");
	if( mkfifo(WR_FIFO, 0666))
		return 1;
	if( mkfifo(RD_FIFO, 0666))
		return 1;
	
	stat_ret = stat(RD_FIFO, &stat_buffer);
	printf("The stats after creating fifo: %d\n",stat_ret);
	
	printf("opening fifo file \n");
	if ( (fd_wr = open(WR_FIFO, O_RDONLY | O_NONBLOCK)) < 0)
	{
		fprintf(stderr,"error with opening WR_FIFO\n");
		return 1;
	}
	if ( (fd_rd = open(RD_FIFO, O_RDWR )) < 0)
	{
		fprintf(stderr,"error with opening RD_FIFO\n");
		return 1;
	}
	
	
	printf("FIFOs opened\n");
	
	alarm(15);		// Have 15 seconds to input something
	printf("The process will end after 15 secs of being idle bro\n");
	
	while(_continue == 1)
	{
		while( (n=read(fd_wr,buffer,sizeof(buffer)))> 0 )	// 0 for stdin, 1 stdout
		{
			
			if( (m=write(fd_rd,buffer,n)) != n)
			{
				fprintf(stderr,"sv error with writing info RD_FIFO");
				return 1;	
			}
			else
				printf("read from WR_FIFO into RD_FIFO\n");
			alarm(15);
		}
	} 
	printf("Prepering shut-down\nunlinking the fifo...\n");
	(void)unlink(WR_FIFO);
	(void)unlink(RD_FIFO);
	printf("Shutting down..\n");
	return 0;
}

