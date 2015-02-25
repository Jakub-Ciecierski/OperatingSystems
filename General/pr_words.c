#include <stdio.h>
#include <string.h>

int main(int argc,char* argv[])
{
	//char* line;
	while(1)
	{
		char line[255];
		fgets(line,255,stdin);
		
		fprintf(stdout,"%s",line);
		//int i;
		int words=0;
		char* p;
		p=strtok(line," ");
		while(p!=NULL)		
		{
			words++;
			p=strtok(NULL," ");
		}
		fprintf(stdout,"\nnumber of words: %d",words);
	}
	return 0;
}
