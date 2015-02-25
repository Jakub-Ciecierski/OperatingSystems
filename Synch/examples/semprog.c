#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/sem.h>
union semun{ int val; struct semid_ds *buf; ushort_t *array;};
int s_init(char *name, int cnt) {/* utworzenie/do³¹czenie i inicjacja pojedynczego semafora o nazwie name i wartoœci licznika cnt */
	int semid;
	union semun arg;
	semid = semget(ftok(name, 1), 1, IPC_CREAT | 0600);
	arg.val=cnt;
	if(semctl(semid,0,SETVAL,arg)<0){   perror("sem_init");   exit(1); }
	return semid;
}
void s_wait(int semid) {/* operacja czekaj na semaforze */
	struct sembuf s;
	s.sem_num = 0;
	s.sem_op = -1;
	s.sem_flg = SEM_UNDO;
	if(semop(semid, &s, 1) < 0) { perror("sem_wait"); exit(1);}
}
void s_post(int semid) {
	struct sembuf s;
	s.sem_num = 0;
	s.sem_op = 1;
	s.sem_flg = SEM_UNDO;
	if(semop(semid, &s, 1) < 0) { perror("sem_post"); exit(1);}
}
static int  flag;
void pisz(int semid, char c) {
	int i;
	if(flag) s_wait(semid);
	for (i = 0; i < 5; i++) {/* sekcja krytyczna programu */
	printf("%c", c); fflush(stdout); sleep(1);
	}/* koniec sekcji krytycznej */
	if(flag) s_post(semid);
}
int main(int argc, char *argv[]){
	int semid;
	if(argc>1) flag=1;
	semid = s_init("semprog",1);
	if (fork() == 0) { // Proces potomny
		for (;;) pisz(semid, '1');
		exit(0);
	}
	for (;;) pisz(semid, '2');
	exit(0);
}
