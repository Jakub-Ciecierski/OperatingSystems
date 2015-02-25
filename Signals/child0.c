#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>

void usage(char* name)
{
	fprintf(stderr,"usage of %s",name);
	fprintf(stderr,"\nInputs integer number from the interval [-128 , 127]\n");
	raise(SIGTERM);
	pause();
}

int main(int argc,char* argv[])
{
	pid_t pid = getpid();
	int arg_val;
	
	if(sscanf(argv[1],"%d",&arg_val) !=1)
	{
		usage(argv[0]);
	}
	if(!(arg_val >= -128 && arg_val <= 127))
	{
		usage(argv[0]);
	}
	fprintf(stdout,"%s with PID %d, passed argument: %d\n",argv[0],pid,arg_val);
	exit(arg_val + 128);
}
