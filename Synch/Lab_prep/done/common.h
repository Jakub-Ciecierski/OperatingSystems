#ifndef __COMMON_LAB3__
#define __COMMON_LAB3__

#define USE_FTOK 1
#define USE_COLORS 1

#define PACKLEN		108
#define FTOK_PATH "/home/"
#define FTOK_CHAR 'c'
#define FTOK_CHAR_S 'm'

#if USE_COLORS
# define ANSI_COLOR_RED     "\x1b[31m"
# define ANSI_COLOR_GREEN   "\x1b[32m"
# define ANSI_COLOR_YELLOW  "\x1b[33m"
# define ANSI_COLOR_BLUE    "\x1b[34m"
# define ANSI_COLOR_MAGENTA "\x1b[35m"
# define ANSI_COLOR_CYAN    "\x1b[36m"
# define ANSI_COLOR_RESET   "\x1b[0m"
#else
# define ANSI_COLOR_RED     ""
# define ANSI_COLOR_GREEN   ""
# define ANSI_COLOR_YELLOW  ""
# define ANSI_COLOR_BLUE    ""
# define ANSI_COLOR_MAGENTA ""
# define ANSI_COLOR_CYAN    ""
# define ANSI_COLOR_RESET   ""
#endif


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
#define   MSGSIZE (sizeof(packet)-sizeof(long))

void semInit(int sem, int startValue, int semNum);
void semOp (int sem, int semNum,int value);
int  semOpNoWait (int sem, int semNum,int value);
void sethandler(int sig, void (*f) (int));

#endif
