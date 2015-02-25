/* semprint.c - an example of IPC System V semaphore use. */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <errno.h>
#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
 /* union semun is defined in <sys/sem.h> */
#else
union semun{ int val; struct semid_ds *buf; unsigned short *array;};
#endif
int s_creat(key_t key, int val) {
/* creation/attachment o a single semaphore with given key
 * and its initalization to val - only if created. 
 */
	int semid;
	union semun arg;
	semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0600);
	if(semid == -1){
		if(errno!=EEXIST){
			perror("semgat, EXCL"); return -1;
		}
		/* try to attach existing semaphore set */
		semid = semget(key, 1, 0);
		if(semid == -1){
			perror("semget"); exit(1);
		}
		fprintf(stderr,"Sem. attached: key=%lx\n",
			(unsigned long)key);
		return semid;
	}
	arg.val=val;
	if(semctl(semid,0,SETVAL,arg)<0){
		perror("sem_init");   exit(2);
	}
	fprintf(stderr,"New sem. created: key=%lx, val=%d\n",
			(unsigned long)key, val);
	return semid;
}
void s_wait(int semid) {/* wait operation */
	struct sembuf s;
	s.sem_num = 0;
	s.sem_op = -1;
	s.sem_flg = SEM_UNDO;
	if(semop(semid, &s, 1) < 0) { perror("sem_wait"); exit(1);}
}
void s_post(int semid) {/* post operation */
	struct sembuf s;
	s.sem_num = 0;
	s.sem_op = 1;
	s.sem_flg = SEM_UNDO;
	if(semop(semid, &s, 1) < 0) { perror("sem_post"); exit(1);}
}
int s_getval(int semid){/* get semaphore value */
	int semval;
	semval = semctl(semid, 0, GETVAL, 0);
	return semval;
}
static int  flag;
void do_printing(int semid, char c) {
	int i;
	if(flag) s_wait(semid);
	for (i = 0; i < 5; i++) {/* critical section: beginning */
	   printf("%c", c); fflush(stdout); sleep(1);
	}/* critical section: end */
	if(flag) s_post(semid);
	sleep(1);
}
int main(int argc, char *argv[]){
	int semid1, semid2, v;
	key_t key1, key2;
	if(argc>1) flag=1;
	key1=ftok(".",1);
	if(key1 == -1) { perror("key1"); exit(11); }
	key2=ftok(".",2);
	if(key2 == -1) { perror("key2"); exit(12); }
	semid1 = s_creat(key1,1);
	if(semid1 == -1) exit(13);
	semid2 = s_creat(key2,0);
	if(semid2 == -1) exit(14);
	s_post(semid2);
	v=s_getval(semid2);
	
	if (fork() == 0) { // child process
		for (;;) do_printing(semid1, '1'+2*v);
		exit(0);
	}
	for (;;) do_printing(semid1, '2'+2*v);
/* s_wait(semid2); */
	exit(0);
}
