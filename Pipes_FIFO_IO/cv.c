#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#define FIFO_FILE "MYFIFO"


int main (int argc, char* argv[])
{
	char buf[80];
	int fd, m, n;
	
	printf("unlinking the fifo...\n");
	(void)unlink(FIFO_FILE);
	
	printf("setting the umask...\n");
	umask(0);
	
	printf("mkfifo...\n");
	if( mkfifo(FIFO_FILE, 0666))
		return 1;
	
	printf("opening fifo file \n");
	if ( (fd = open(FIFO_FILE, O_RDONLY | O_NONBLOCK)) < 0)
		return 1;
	
	printf("FIFO opened\n");
	
	while(1)
	{
		while( (n = read(fd, buf, sizeof(buf))) > 0 )
		{
			if( (m = write(1, buf, n) != n) )
				return 1;
			else
				printf("%d bytes read from FIFO\n",n);
				
		} 
	}
	return 0;
	
}
