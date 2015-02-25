/* packet.h */
#ifndef __DEF_PACKET_H__
#define __DEF_PACKET_H__
#include <limits.h>
#include <sys/types.h>
typedef struct{
	pid_t pid;
	} PACKET_HEADER;
#define MSG_SIZE (PIPE_BUF - sizeof(PACKET_HEADER))
typedef struct{
	PACKET_HEADER header;
	char msg[MSG_SIZE];
} PACKET;
#endif
