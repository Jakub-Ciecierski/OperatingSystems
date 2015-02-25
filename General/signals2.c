#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>

sigset_t mask1,mask2;
int signr;

int main ()
{
	// synchroneous handling of signals
	printf("%d\n",getpid());
	
	// blocks all the signals
	sigfillset(&mask2);
	sigprocmask(SIG_BLOCK,&mask2,NULL);
	
	// handles the SIGINT signal
	sigemptyset(&mask1);
	sigaddset(&mask1,SIGINT);
	
	if(sigwait(&mask1,&signr)==0)
	{
		fprintf(stdout,"signal delivered %d",signr);
	}

	return 0;
}
