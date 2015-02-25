/* io.h */
#ifndef __DEF_IO_H__
#define __DEF_IO_H__
#include <sys/types.h>
size_t bulk_read(int fd, char *buf, size_t count);
size_t bulk_write(int fd, char *buf, size_t count);
#endif
