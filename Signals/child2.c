#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>

sigset_t mask;
volatile sig_atomic_t gameover = 1;
int ret_val = 128;
int p;

void usage(char* name)
{
	fprintf(stderr,"usage of %s\n",name);
	fprintf(stderr,"\nInputs integer number from the interval [-128 , 127]\n");
	raise(SIGTERM);
	pause();
}

int main(int argc,char* argv[])
{
	int sigNo;
	
	if(argc !=2)
		usage(argv[0]);
		
	if(sscanf(argv[1],"%d",&p) !=1)
		usage(argv[0]);
	
	if(!(p >= -128 && p <= 127))
		usage(argv[0]);
	
	sigemptyset(&mask);
	sigaddset(&mask,SIGINT);
	sigaddset(&mask,SIGUSR1);
	sigaddset(&mask,SIGUSR2);
	sigprocmask(SIG_BLOCK, &mask, NULL);
	
	while(gameover)
	{
			fprintf(stderr,"PID: %d, p: %d, ret_val: %d\n",getpid(),p,ret_val);
			//sleep(p);
			sigwait(&mask,&sigNo);
			if(sigNo == SIGUSR1)
				ret_val+=p;
			if(sigNo == SIGUSR2)
				ret_val-=p;
			if(sigNo == SIGINT)
			{
				gameover=0;
				fprintf(stdout,"SIGINT recieved, about to close...\n");
			}
	}
	
	exit(ret_val+128);
}
