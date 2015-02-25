/* Problems with Non-Atomic Access - an example from
 * "The GNU C Library" documentation
 * The program fills mem struct alternatively with 0,0 and 1,1.
 * However, on most machines it takes several instructions to store
 * a new pair of integers in mem. If the signal is delivered in between these
 * instruction the handler might find that mem.a is 0 and mem.b is 1
 * (or vice versa).
 */
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

struct two_words { int a, b; } mem;

void handler(int signum){
/* uncomment the line below for systems with traditional signal semantics */
/*	signal(SIGALRM, handler); */
	printf("%d,%d\n",mem.a, mem.b);
	alarm(1);
}
int main(void){
	static struct two_words zeros={0,0}, ones={1,1};
	signal(SIGALRM, handler);
	mem=zeros;
	alarm(1);
	while(1){
		mem=zeros;
		mem=ones;
	}
	return 0;
}
