#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>

sigset_t mask;

volatile sig_atomic_t _continue = 1;
void* signalThread (void* param);

int main()
{
	printf("Proccess id %d \n",getpid());
	
	mask;
	sigfillset(&mask);
	pthread_sigmask(SIG_BLOCK, &mask, NULL);
	
	pthread_t tid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_create(&tid, &attr, signalThread, NULL);
	
	/*
	* creating second signal thread will do no good 
	* since one signal can be handled only once - 
	* therefor the other thread will be idle with nothing
	* to handle.
	*/ 
	//pthread_create(&tid, &attr, signalThread, NULL);
	
	mainThread();
	
	pthread_join(tid,NULL);
	
	printf("Sigint recieved, shutting down the process...\n");
	return 0;
}

void mainThread()
{
		
	printf("Main thread with tip: %lu, starting...\n",(unsigned long)pthread_self());
	while(_continue == 1)
	{
		printf(".\n");
		sleep(1);
	}
}

void* signalThread (void* param)
{
	printf("Secondary thread starting with tid: %lu\n",(unsigned long)pthread_self());
	int sig;
	
	while(_continue == 1)
	{
		sigwait(&mask,&sig);
		if(sig == SIGINT )
			_continue = 0;
		else
			printf("secondary thread: %lu recieved a signal %d \n",(unsigned long)pthread_self(),sig);
			
	}
	//pthread_sigmask(SIG_BLOCK, &main, NULL);
	
}
