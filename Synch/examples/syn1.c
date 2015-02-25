/* syn1.c - careless concurrent threads.
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define prt1(p) fprintf(stderr,p)
int cnt, n;

void *worker(void *arg){
int i, v;
	for(i=0; i<n; i++){
		v=cnt;
		printf("thread %d: %d\n",pthread_self(), v+((int *)arg)[0]);
		cnt=v+((int *)arg)[0];
	}
	return NULL;
}
int main(int argc, char *argv[]){
int inc=1, dec=-1;
pthread_t tid1, tid2;

	if(argc<2) n=100;
	else n=atoi(argv[1]);
	if(n<1) n=1;
	cnt=0;
	fprintf(stderr,"Start %s with n=%d, cnt=%d\n",argv[0],n,cnt);
	if(pthread_create(&tid1,NULL,worker,&inc)){ prt1("inc"); exit(1);};
	if(pthread_create(&tid2,NULL,worker,&dec)){ prt1("dec"); exit(2);};
	if(pthread_join(tid1,NULL)) prt1("join inc");
	if(pthread_join(tid2,NULL)) prt1("join dec");
	fprintf(stderr,"\nKoniec %s z n=%d, cnt=%d\n",argv[0],n,cnt);
	return 0;
}
