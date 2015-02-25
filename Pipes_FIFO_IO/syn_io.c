#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void set_sig_handler(int signum,void(*handler)(int))
{
	struct sigaction sa;
	memset(&sa,0x00,sizeof(struct sigaction));
	
	sa.sa_handler = handler;
	if(-1==sigaction(signum,&sa,NULL))
		printf("Error with signal : %d",signum);		
}

void signal_handler(int sig)
{
	fprintf(stderr,"GoodBye...!\n");
}

int main(int argc, char* argv[])
{
	char buf[20];
	printf("%d\n", sizeof(buf));
	int n;
	set_sig_handler(SIGALRM, signal_handler);
	
	alarm(5);		// Have five seconds to input something
	printf("The process will end after 5 secs of being idle bro\n");
	while( n=read(0,buf,sizeof(buf)) > 0 )	// 0 for stdin, 1 stdout
	{
		write(1,buf,n);
		alarm(5);
	} 

}
