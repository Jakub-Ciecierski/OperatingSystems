#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>

char c = '0';
pid_t pid;

/*Takes a signal and pointer to a handler function for that signal*/
void set_sig_handler(int signum,void(*handler)(int))
{
	struct sigaction sa;

	sa.sa_handler = handler;
	if(-1==sigaction(signum,&sa,NULL))
		printf("Error with signal : %d",signum);
		
}

void sigint_handler(int signum)
{
	
		printf("\nSIGINT detected, proceeding the shutdown...\n");
		int sec=5;
		while(sec>0)
		{
			printf("%d seconds until shutdown...\n",sec);
			sec--;
			sleep(1);
		}
		printf("goodbye...\n");
		exit(0);
	

}

void sigusr1_handler(int signum)
{
	printf("\nSIGUSR1 detected, character incremented\n");
	c++;
	if(c>'z')
		c='0';
}

void sigusr2_handler(int signum)
{
	printf("\nSIGUSR2 detected, character decremented\n");
	c--;
	if(c<'0')
		c='z';
}

void runThread()
{
	while(true)
	{
		/*print the char and procces id*/
		printf("%c\t getId: %d \n",c,getpid()); 

		/*flush the buffer to stdout */
		/*NOT NEEDED WITH NEW LINE CHAR, SINCE NEW LINE FLUSHES THE BUFFER*/
		fflush(stdout);

		/*sleep for 2 seconds*/
		sleep(2);	
	}	
}


int main()
{	
	/*Set the handlers !*/
	set_sig_handler(SIGINT,sigint_handler);
	set_sig_handler(SIGUSR1,sigusr1_handler);
	set_sig_handler(SIGUSR2,sigusr2_handler);

	
	pid=fork();
	if(pid>0)
		runThread();
	else			
		return 0;
	return 0;
}
