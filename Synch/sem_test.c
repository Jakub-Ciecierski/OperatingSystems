#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/sem.h>
union semun{ int val; struct semid_ds *buf; unsigned short *array;};

int get_sem_val( int sid, int semnum )
{
	return( semctl(sid, semnum, GETVAL, 0));
}

int sem_init (key_t key, int initval)
{
	int sid;
	// create semaphore
	if( (sid = semget(key, 1, IPC_CREAT | 0660 )) == -1)
		return -1;
	
	// set initial value to the semaphore
	union semun arg;
	arg.val = initval;
	if(semctl (sid, 0, SETVAL, arg) == - 1)
		return -1;
	
	return sid;
}

void sem_wait (int sid)
{
	struct sembuf sem_buf;
	sem_buf.sem_num = 0;
	sem_buf.sem_op = -1;
	sem_buf.sem_flg = SEM_UNDO;
	if(semop(sid, &sem_buf, 1) == -1){
		perror("semop()");
		exit(EXIT_FAILURE);
	}
}

void sem_post (int sid)
{
	struct sembuf sem_buf;
	sem_buf.sem_num = 0;
	sem_buf.sem_op = 1;
	sem_buf.sem_flg = SEM_UNDO;
	if(semop(sid, &sem_buf, 1) == -1){
		perror("semop()");
		exit(EXIT_FAILURE);
	}
}

void do_printing(int sid, char c,int is_parent)
{
	sem_wait(sid);
	// critical point
	int i;
	for(i=0;i < 5; i++){
		printf("%c",c); 
		fflush(stdout);
		sleep(1);
	}
	printf("\n");
	sem_post(sid);
}

int main (int argc, char* argv[])
{
	if(argc != 2){
		printf("usage: ./sem_test sem_init_val\n");
		return 0;
	}
	int init_val = atoi(argv[1]);
	
	key_t sem_key = 0x00FF01;
	int sid;
	if ( (sid=sem_init(sem_key, init_val)) == -1 )
	{
		perror("semOp()");
		exit(EXIT_FAILURE);
	}
	if(fork() == 0){
		while(1)
			do_printing(sid,'1',0);
		exit(0);
	}
	while(1)
		do_printing(sid,'2',1);
	return 0;
}
