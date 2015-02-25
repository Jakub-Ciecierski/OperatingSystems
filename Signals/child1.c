#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>

volatile sig_atomic_t gameover = 1;
int ret_val = 128;
int p;

void set_sig_handler(int signum,void(*handler)(int))
{
	struct sigaction sa;
	memset(&sa,0x00,sizeof(struct sigaction));
	
	sa.sa_handler = handler;
	if(-1==sigaction(signum,&sa,NULL))
		printf("Error with signal : %d",signum);
		
}

void handler_sigint(int sig)
{
	gameover = 0;
	fprintf(stdout,"about to close process...\n");
}

void handler_sigusr1(int sig)
{
	ret_val+=p;
}

void handler_sigusr2(int sig)
{
	ret_val-=p;
}

void usage(char* name)
{
	fprintf(stderr,"usage of %s\n",name);
	fprintf(stderr,"\nInputs integer number from the interval [-128 , 127]\n");
	raise(SIGTERM);
	pause();
}

int main(int argc,char* argv[])
{
	set_sig_handler(SIGINT,handler_sigint);
	set_sig_handler(SIGUSR1,handler_sigusr1);
	set_sig_handler(SIGUSR2,handler_sigusr2);
	
	if(argc !=2)
		usage(argv[0]);
		
	if(sscanf(argv[1],"%d",&p) !=1)
		usage(argv[0]);
	
	if(!(p >= -128 && p <= 127))
		usage(argv[0]);
	
	while(gameover)
	{
			fprintf(stderr,"PID: %d, p: %d, ret_val: %d\n",getpid(),p,ret_val);
			sleep(p);
	}
	
	exit(ret_val+128);
}
