#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>

void usage(char* name)
{
	fprintf(stderr,"%s usage:/n",name);
}

int main(int argc,char* argv[])
{
	int sum=0;
	int* status;
	char* proccess_name = "./child0";
	char* p_table[3];
	p_table[0] = proccess_name;
	p_table[2] = NULL;
	
	if(argc==1)
	{
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	
	int i=0;
	for(i=1;i<argc;i++)
	{
		p_table[1] = argv[i];
		
		int pid = fork();
		if(pid<0)
			exit(EXIT_FAILURE);
		if(pid==0) 	// child proccess
		{
				execvp(proccess_name,p_table);
		}
		
	}
	pid_t pid;
	while((pid=waitpid(-1,&status,0))>0)
	{
		if(WIFEXITED(status))
			sum+=WEXITSTATUS(status)-128;
		else
			printf("abnormal child exited: %d\n",pid);
	}
	fprintf(stdout,"the sum of returned values from child0: %d \n",sum);
	exit(0);
}
