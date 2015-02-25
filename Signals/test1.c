#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

volatile sig_atomic_t _continue = 1;

int maxcnt, maxtime;

pid_t child1_pid;
pid_t child2_pid;

// 1 for positive phase, -1 for negative
int phase_c1;
int phase_c2;

int counter_c1;
int counter_c2;

//sigset_t mask;

void set_sig_handler(int signum,void(*handler)(int))
{
	struct sigaction sa;
	memset(&sa,0x00,sizeof(struct sigaction));
	
	sa.sa_handler = handler;
	if(-1==sigaction(signum,&sa,NULL))
		printf("Error with signal : %d",signum);		
}

void sigusr1_child1_handler(int sig)
{
	//fprintf(stdout,"CHILD1 recieved signal\n");
	if(phase_c1 == 1)
	{
		fprintf(stdout,"CHILD1 positive phase\n");
		counter_c1++;
	}
	else
	{
		fprintf(stdout,"CHILD1 negative phase\n");
		counter_c1--;
	}
}

void sigusr1_child2_handler(int sig)
{
	//fprintf(stdout,"CHILD2 recieved signal\n");
	if(phase_c2 == 1)
	{
		fprintf(stdout,"CHILD2 positive phase\n");
		counter_c2++;
	}
	else
	{
		fprintf(stdout,"CHILD2 negative phase\n");
		counter_c2--;
	}
}

void do_child1()
{
	sigset_t mask
	set_sig_handler(SIGUSR1,sigusr1_child1_handler);
	
	child1_pid = getpid();
	fprintf(stdout,"CHILD1 process pid: %d\n",child1_pid);
	
	sigemptyset(&mask);
	sigaddset(&mask,SIGUSR1);
	sigprocmask(SIG_BLOCK,&mask,NULL);
	
	phase_c1 = 1;
	counter_c1 = 0;
	
	while(_continue == 1)
	{
		srand(time(NULL)*child1_pid);
		int ph_time = rand() % 6 + 1 ;
		int t;
		fprintf(stdout,"CHILD1 :going to sleep for %d seconds\n",ph_time);
		t=sleep(ph_time);
		while(t!=0)
		{
			fprintf(stdout,"CHILD1: was awaken, going back to sleep for %d seconds\n",t);
			t=sleep(t);
		}
		
		fprintf(stdout,"CHILD1: counter : %d\n",counter_c1);
		kill(child2_pid,SIGUSR1);
		phase_c1 = -phase_c1;
		
		if(maxtime == ph_time ||  abs(counter_c1) > maxcnt)
			_continue=0;
	}
	fprintf(stdout,"CHILD1: shutting down...\n");
	exit(counter_c1);
}

void do_child2()
{
	set_sig_handler(SIGUSR1,sigusr1_child2_handler);
	child2_pid = getpid();
	fprintf(stdout,"CHILD2 process pid: %d\n",child2_pid);
	
	phase_c2 = 1;
	counter_c2 = 0;
	
	while(_continue == 1)
	{
		srand(time(NULL)*child2_pid);
		int ph_time = rand() % 6 + 1 ;
		int t;
		fprintf(stdout,"CHILD2 :going to sleep for %d seconds\n",ph_time);
		t=sleep(ph_time);
		while(t!=0)
		{
			fprintf(stdout,"CHILD2: was awaken, going back to sleep for %d seconds\n",t);
			t=sleep(t);
		}
		
		fprintf(stdout,"CHILD2: counter : %d\n",counter_c2);
		kill(child1_pid,SIGUSR1);
		phase_c2 = -phase_c2;
		if(maxtime == ph_time ||  abs(counter_c1) > maxcnt)
			_continue=0;
	}
	fprintf(stdout,"CHILD2: shutting down...\n");
	exit(counter_c2);
	
}

void usage(char* name)
{
	fprintf(stderr,"usage of %s\n",name);
	fprintf(stderr,"%s maxcnt maxtime\n",name);
	exit(EXIT_FAILURE);
}

int main (int argc,char* argv[])
{
	if(argc!=3)
		usage(argv[0]);
	if(sscanf(argv[1],"%d",&maxcnt) !=1)
		exit(EXIT_FAILURE);
	if(sscanf(argv[2],"%d",&maxtime) !=1)
		exit(EXIT_FAILURE);	
	fprintf(stdout,"user input maxcnt: %d and maxtime: %d\n",maxcnt,maxtime);
	
	int status_c1;
	int status_c2;
	
	int pid1 = fork();
	
	if(pid1 <0 )
		exit(EXIT_FAILURE);
	
	if(pid1==0)
		do_child1();
	
	else
	{
		int pid2 = fork();
		if(pid2 <0 )
			exit(EXIT_FAILURE);
	
		if(pid2==0)
			do_child2();
		else
		{
			waitpid(child1_pid,&status_c1,0);
			waitpid(child2_pid,&status_c2,0);
			fprintf(stdout,"CHILD1: %d exited with value %d\n",child1_pid,WEXITSTATUS(status_c1));
			fprintf(stdout,"CHILD2: %d exited with value %d\n",child2_pid,WEXITSTATUS(status_c2));
			exit(0);
		}
	}
	return 0;
}


		/* some usefull stuff
		int sig;
		if(sigpending(&pending_mask) == 0 )
			while(sigismember(&pending_mask,SIGUSR1)==1)
			{	
				sigwait(&pending_mask,&sig);
				sigdelset(&pending_mask,SIGUSR1);
				fprintf(stdout,"something pending\n");
			}*/
