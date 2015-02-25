#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

pid_t pid;
volatile sig_atomic_t _continue = 1;

void sendSig(pid_t id,int sig)
{
	if(sig==1)
	{	
		kill(id,SIGUSR1);
		printf("signal: SIGUSR1 sent to: %d \n",id);
	}		
	if(sig==2)
	{
		kill(id,SIGUSR2);
		printf("signal: SIGUSR2 sent to: %d \n",id);
	}
	if(sig==3)
	{
		kill(id,SIGINT);
		printf("signal: SIGINT sent to: %d \n",id);
	}
}

void runTerminal()
{
	printf("Insert the process id\n");

	printf("PID: ");	
	scanf("%d",&pid);
	
	int sig = 0;
	while(1)
	{
		printf("SIGUSR1: 1, SIGUSR2: 2, SIGINT: 3, new Process: 4, exit: 5");
		scanf("%d",&sig);
		if (sig == 4)
		{
			printf("proceeding to input new process id...\n");
			break;
		}
		else if(sig < 1 || sig > 5)
			printf("incorrect input, please try again.\n");
		else if(sig == 5)
		{
			_continue = 0;
			break;
		}
		else
			sendSig(pid,sig);
	}
		
}

int main()
{
	pid = -1;
	while(_continue == 1)
		runTerminal();
	printf("goodbye...\n");
	return 0;
}
