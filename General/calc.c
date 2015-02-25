#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char *argv[])
{
	int i=0;
	/*		MALLOC			*/
	/*	void* malloc (size_t size);	*/
	/*	Allocates a block of		*/
	/*	'size' bytes of memory		*/
	/*					*/
	/*	Returns the pointer to the 	*/
	/*	beginning of the block		*/
	/*	of type void* which can be 	*/
	/*	cast to a desired type		*/

	char* buffer=NULL;
	buffer = (char*) malloc (5);
	buffer[0]='a';
	buffer[1]='b';
	buffer[2]='c';
	buffer[3]='e';
	buffer[4]='f';

	puts(buffer);
	memset(buffer,'g',2);
	puts(buffer);
	
	free(buffer);

	/*			CALLOC				*/
	/*	void* calloc (size_t num, size_t size);		*/
	/*	num elements of size bytes			*/

	int* table=NULL;
	int table_size = 5;
	table = (int*) calloc(table_size,sizeof(int));

	printf("size_of int :%zu\n",sizeof(int));

	for(i=0;i<table_size;i++)
		table[i]=i;

	/*		MEMSET		*/
	printf("before memset\n");		
	for(i=0;i<table_size;i++)
		printf("%d\n",table[i]);
	
	memset(table,0x00,4*table_size);	
	
	printf("after memset\n");
	for(i=0;i<table_size;i++)
		printf("%d\n",table[i]);
		
	/*		MEMCPY		*/
	int* table_cpy;
	memcpy(table_cpy,table,4*table_size);
	puts("memcpy table_cpy:");
	for(i=0;i<table_size;i++)
		printf("%d\n",table_cpy[i]);
	free(table);

	/*		STRTOK		*/
	char text[] ="Remove, . use.less . , chars-";
	char* stk = strtok(text,",.-");
	while(stk!=NULL)
	{
		puts(stk);	
		stk=strtok(NULL,", .-");
	}
	return 0;
}
