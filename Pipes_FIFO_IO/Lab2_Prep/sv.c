#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include "packet.h"

volatile sig_atomic_t _continue = 1;

void set_sig_handler(int signum,void(*handler)(int))
{
	struct sigaction sa;
	memset(&sa,0x00,sizeof(struct sigaction));
	
	sa.sa_handler = handler;
	if(-1==sigaction(signum,&sa,NULL))
		printf("Error with signal : %d",signum);		
}

void sigint_handler(int sigint)
{
	printf("SIGINT received\n");
	_continue = 0;
}

void usage (char* info)
{
	printf("%s usage:\n",info);
	printf("%s fifo filtering_cmd\n",info);
}

void filter_process(char* FILT_CMD, int fdpipe[2])
{
	close(0);			// close stdin
	dup(fdpipe[0]);		// dup read
	close(fdpipe[0]);	
	close(fdpipe[1]);
	system(FILT_CMD);
}

void run_read (int fifo, char* FILT_CMD)
{
	int n;
	PACKET buffer;
	char output_file[256];
	char* txt = ".txt";
	
	while(_continue == 1)
	{
		do
		{
			FILE* file;
			if( (n = read(fifo, (void*)&buffer, PIPE_BUF)) > 0)
			{
				sprintf(output_file, "%d", buffer.header.pid);
				strcat(output_file,txt);
				
				strcat(FILT_CMD," > ");
				strcat(FILT_CMD,output_file);
				//"FILT_CMD > PID.TXT"
				
				if ((file = popen(FILT_CMD, "w")) == NULL) 
				{
					perror("popen");
					exit(3);
				}
				
				int i;
				for(i=0;i<MSG_SIZE;i++)
					if(*(buffer.msg+i) != '\0')
						fwrite(buffer.msg+i, sizeof(char), 1, file);
				
				/*
				int off;
				
				if((off = fseek(file,i,SEEK_CUR) < 0 ) )
				{
					perror("fseek()");
					exit(EXIT_FAILURE);
				}
				*/
				
				/*
				int fdpipe[2];
				pipe(fdpipe);
				
				switch(fork())
				{	
					case -1: perror("fork"); exit(4);
					case 0: filter_process(FILT_CMD, fdpipe);
						exit(0);
					default:break;
				}
				
				int i;
				for(i=0;i<MSG_SIZE;i++)
					if(*(buffer.msg+i) != '\0')
						write(fdpipe[1],buffer.msg+i,1);
				*/
				
				printf("PACKET PID:--------------------%d\n",buffer.header.pid);
				
				sleep(1);
				fclose(file);
				//close(fdpipe[1]);
				//close(fdpipe[0]);
			}
			if(n<0)
			{
				perror("read()");
				exit(EXIT_FAILURE);
			}
		}while(n == PIPE_BUF);
	}
	
}

int main(int argc, char* argv[])
{
	if(argc !=3)
	{
		usage(argv[0]);
		return 0;
	}
	
	// set sigint handler for shutdown
	set_sig_handler(SIGINT, sigint_handler);
	
	// fifo name string
	const char* MY_FIFO = argv[1];
	// command name string
	
	char* FILT_CMD = argv[2];
	
	// descriptor
	int fifo;
	
	printf("unlinking the fifo...\n");
	// destroys the FIFO
	(void)unlink(MY_FIFO);
	
	// Make FIFO
	printf("mkfifo...\n");
	if( mkfifo(MY_FIFO, 0666))
		return 1;
	
	printf("opening fifo file \n");
	if ( (fifo = open(MY_FIFO, O_RDONLY) ) < 0)
	{
		fprintf(stderr,"error with opening WR_FIFO\n");
		return 1;
	}
	printf("FIFO opened\n");
	run_read(fifo, FILT_CMD);
	
	close(fifo);
	return 0;
}
