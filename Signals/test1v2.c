/*
 * Code made my me - JC.
 * The code is long and overly complicated - deal with.
 * It shows a simply synchronised signal sending/recieving.
 * Altho it has its flaws : it cannot store more than more
 * one pending signal of the same type.
 * Might contain bugs. HF
 * */

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

sigset_t mask;

int maxcnt, maxtime;

pid_t child1_pid;
pid_t child2_pid;

/*
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
*/

void do_child1()
{
	int _continue = 1;
	sigset_t pending_mask;
	
	int phase_c1;
	int counter_c1;
	
	child1_pid = getpid();
	
	fprintf(stdout,"starting CHILD1 process pid: %d, and its brothers :%d\n",child1_pid,child2_pid);
	
	/*
	sigemptyset(&mask);
	sigaddset(&mask,SIGUSR1);
	sigaddset(&mask,SIGUSR2);
	sigprocmask(SIG_BLOCK,&mask,NULL);
	*/
	
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
		
		int sig;
		if(sigpending(&pending_mask) == 0 )
		{
			if(sigismember(&pending_mask,SIGUSR1)==1)
			{	
				sigwait(&pending_mask,&sig);
				if(sig==SIGUSR1)
				{
					if(phase_c1==1)
						counter_c1++;
					else
						counter_c1--;
				}
			}
			if(sigismember(&pending_mask,SIGUSR2)==1)
			{
				sigwait(&pending_mask,&sig);
				if(sig==SIGUSR2)
					_continue = 0;
			}
		}
		fprintf(stdout,"CHILD1: counter : %d\n",counter_c1);
		kill(child2_pid,SIGUSR1);
		phase_c1 = -phase_c1;
		
		if(maxtime < ph_time ||  abs(counter_c1) > maxcnt)
		{
			_continue=0;
			kill(child2_pid,SIGUSR2);
		}
	}
	fprintf(stdout,"CHILD1: shutting down...\n");
	exit(counter_c1);
}

void do_child2()
{
	int _continue = 1;
	sigset_t pending_mask;
	
	int phase_c2;
	int counter_c2;
	
	child2_pid = getpid();
	
	fprintf(stdout,"starting CHILD2 process pid: %d and its brothers: %d\n",child2_pid,child1_pid);
	
	/*
	sigemptyset(&mask);
	sigaddset(&mask,SIGUSR1);
	sigaddset(&mask,SIGUSR2);
	sigprocmask(SIG_BLOCK,&mask,NULL);
	*/
	
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
		
		int sig;
		if(sigpending(&pending_mask) == 0 )
		{
			if(sigismember(&pending_mask,SIGUSR1)==1)
			{	
				sigwait(&pending_mask,&sig);
				if(sig==SIGUSR1)
				{
					if(phase_c2==1)
						counter_c2++;
					else
						counter_c2--;
				}
			}
			if(sigismember(&pending_mask,SIGUSR2)==1)
			{
				sigwait(&pending_mask,&sig);
				if(sig==SIGUSR2)
					_continue = 0;
			}
		}
		fprintf(stdout,"CHILD2: counter : %d\n",counter_c2);
		kill(child1_pid,SIGUSR1);
		phase_c2 = -phase_c2;
		
		if(maxtime < ph_time ||  abs(counter_c2) > maxcnt)
		{
			_continue=0;
			kill(child1_pid,SIGUSR2);
		}
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
	
	int child1_pipe[2];
	int child2_pipe[2];
	
	fprintf(stdout,"piping once...\n");
	pipe(child1_pipe);
	fprintf(stdout,"piping twice...\n");
	pipe(child2_pipe);
	
	sigemptyset(&mask);
	sigaddset(&mask,SIGUSR1);
	sigaddset(&mask,SIGUSR2);
	sigprocmask(SIG_BLOCK,&mask,NULL);
	
	int status_c1;
	int status_c2;
	
	int pid1 = fork();
	
	if(pid1 <0 )
		exit(EXIT_FAILURE);
	
	if(pid1==0)
	{
		int my_pid = getpid();
		fprintf(stdout,"CHILD1: %d writing\n",my_pid);
		
		write(child1_pipe[1],&my_pid,sizeof(my_pid));
		read(child2_pipe[0],&child2_pid,sizeof(child2_pid));
		
		fprintf(stdout,"CHILD1 with pid: %d has brother with pid: %d\n",my_pid,child2_pid);
		close(child1_pipe[0]);
		close(child1_pipe[1]);
		do_child1();
	}
	
	else
	{
		int pid2 = fork();
		if(pid2 <0 )
			exit(EXIT_FAILURE);
	
		if(pid2==0)
		{
			int my_pid = getpid();
			fprintf(stdout,"CHILD2: %d writing\n",my_pid);
			
			write(child2_pipe[1],&my_pid,sizeof(my_pid));
			read(child1_pipe[0],&child1_pid,sizeof(child1_pid));
			
			fprintf(stdout,"CHILD2 with pid: %d has brother with pid: %d\n",my_pid,child1_pid);
			close(child2_pipe[0]);
			close(child2_pipe[1]);
			do_child2();
		}
		else
		{
			waitpid(child1_pid,&status_c1,0);
			waitpid(child2_pid,&status_c2,0);
			fprintf(stdout,"CHILD1: %d exited with value %d\n",pid1,WEXITSTATUS(status_c1));
			fprintf(stdout,"CHILD2: %d exited with value %d\n",pid2,WEXITSTATUS(status_c2));
			exit(0);
		}
	}
	return 0;
}
