#include <pthread.h>
#include <stdio.h>

int sum;
void* runner(void* param);

int main (int argc, char* argv[])
{
	sum = 0;
	printf("main thread id: %lu \n",(unsigned long) pthread_self());
	
	pthread_t tid;
	pthread_attr_t attr;
	
	pthread_attr_init(&attr);
	
	pthread_create(&tid, &attr, runner, argv[1]);
	
	printf("created thread with id: %lu \n",(unsigned long)tid);
	
	int i = 0;
	for (i=0;i<10;i++)
	{
		printf("Main thread: iterating sum variable...\n");
		sum++;
		printf("Main thread: sum equals %d\n",sum);
		sleep(5);
	}
	
	pthread_join(tid,NULL);
		
	return 0;
}

void* runner(void* param)
{
	char* input =(char*) param;
	printf("input: %s \nnew thread with id: %lu \n ",input, (unsigned long)pthread_self());
	
	int i = 0;
	for (i=0;i<10;i++)
	{
		printf("Helper thread: iterating sum variable...\n");
		sum++;
		printf("Helper thread: sum equals %d\n",sum);
		sleep(3);
	}
	pthread_exit(0);
}

