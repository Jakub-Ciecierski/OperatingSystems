#include <stdio.h>
#include <string.h>

int main(int argc,char* argv[])
{
	char numbers[255];
	fgets(numbers,255,stdin);
	char* p;
	p=strtok(numbers," ");
	int sum=0;
	while(p!=NULL)
	{
		int x;
		x=atoi(p);
		sum+=x;
		p=strtok(NULL," ");
	}
	fprintf(stdout,"the sum: %d",sum);
}
