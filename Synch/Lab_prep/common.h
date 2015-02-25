#ifndef __COMMON__
#define __COMMON__

#define PACKLEN 108

// costum semun union for semctl function
union semun {                    
        int              val;    /* Value for SETVAL */
        struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
        unsigned short  *array;  /* Array for GETALL, SETALL */
        struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                    (Linux specific) */
};

typedef struct {
        long mtype;
        long offset;
        long size;
        char buf[PACKLEN];
} packet;
#define   MSGSIZE (sizeof(packet)-sizeof(long))		// We dont include long mtype (4 bytes)
													// This is why we subtract sizeof(long) from total MSGSIZE				

// Initialize the semaphore of index semNum with initValue
void semInit(int pid,int semNum, int initValue);

// Adds value to the current semaphore value of index semNum
void semOp(int pid, int semNum, int value);

// Returns the current value of semaphore of index semNum
// Not used by me in this project but still might be usefull
int getSemVal( int sid, int semNum );

// Good old sethandler
void sethandler(int sig, void (*f) (int));


#endif
