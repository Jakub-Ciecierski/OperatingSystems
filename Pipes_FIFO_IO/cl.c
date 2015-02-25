#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

#define WR_FIFO "WRFIFO"
#define RD_FIFO "RDFIFO"

void* writeThread(void* param);

volatile sig_atomic_t do_sleep = 0;

int stat_ret;
pthread_t tid1; // read thread
pthread_t tid2; // write thread

void signal_handler(int sig)
{
	//printf("writeThread recieved signal, shutting down...\n");
	return;
}

void set_sig_handler(int signum,void(*handler)(int))
{
	struct sigaction sa;
	memset(&sa,0x00,sizeof(struct sigaction));
	
	sa.sa_handler = handler;
	if(-1==sigaction(signum,&sa,NULL))
		printf("Error with signal : %d",signum);		
}

int main (int argc, char* argv[])
{
	// save main thread tid (readThread)
	tid1 = pthread_self();
	
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	
	// create writeThread with tid = tid2
	pthread_create(&tid2, &attr, writeThread, NULL);
	readThread();

	pthread_join(tid2, NULL);
	
	printf("process cl: %d shutting down...\n",getpid());
	return 0;
}

/* Reads bytes from RD_FIFO to stdout */
void readThread()
{
	char buffer[80];
	int fd, n, m;
	if((fd = open(RD_FIFO, O_RDONLY)) < 0)
	{
		fprintf(stderr,"cl with pid: %d failed to open Read FIFO\n",getpid());
		return 1;
	}
	
	/* The loop ends when FIFO closes */
	struct stat stat_buffer;
	stat_ret = stat(RD_FIFO, &stat_buffer);
	
	fcntl(fd, F_SETFL, O_NONBLOCK);
	while(stat_ret == 0)
	{
		if( do_sleep == 1)
		{
			printf("Just sent a msg going to sleep for synchronization...\n");
			sleep(1);
			do_sleep = 0;
		}
		stat_ret = stat(RD_FIFO, &stat_buffer);
		while( (n = read(fd, buffer, sizeof(buffer))) > 0  )
		{
			if( (m = write(1, buffer, n)) != n )
			{
				fprintf(stderr,"cl with pid: %d failed to write into RD_FIFO\n",getpid());
				return 1;
			} 
			else
				printf("read %d bytes from RD_FIFO\n",m);
		}
	}
	close(fd);
	// notify write thread to shutdown ( reading from stdin hangs the thread so it needs a signal)
	pthread_kill(tid2,SIGINT);
	printf("cl with pid: %d RD_FIFO closed, its thread returning...\n",getpid());
}

/* Writes bytes from stdin into WR_FIFO */
void* writeThread(void* param) 
{
	// set sigint to notify the shut down
	set_sig_handler(SIGINT, signal_handler);
	
	char buffer[80];
	int fd, n, m;
	if((fd = open(WR_FIFO, O_WRONLY)) < 0)
	{
		fprintf(stderr,"cl with pid: %d failed to open Write FIFO\n",getpid());
		return 1;
	}
	
	fcntl(fd, F_SETFL, O_NONBLOCK);		// use fcntl => read doesn't block when it doesn't have anything to read
	/* The loop ends when FIFO closes */
	while(stat_ret == 0)
	{
		while( (n = read(0, buffer, sizeof(buffer))) > 0 ) // reads from stdin
		{
			if( (m = write(fd, buffer, n)) != n ) // write to WR_FIFO
			{
				fprintf(stderr,"cl with pid: %d failed to write into WR_FIFO\n",getpid());
				return 1;
			} 
			else
			{
				do_sleep = 1;
				printf("written %d bytes into WR_FIFO\n",m);
			}
		}
	}
	close(fd);
	printf("cl with pid: %d WR_FIFO closed, its thread returning...\n",getpid());
}

