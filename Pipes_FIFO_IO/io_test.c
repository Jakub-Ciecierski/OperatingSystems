#include <unistd.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	char* buffer;
	buffer = (char*) malloc (sizeof(char)*124);
	getwd(buffer);
	
	puts(buffer);
	
	return 0;
}
