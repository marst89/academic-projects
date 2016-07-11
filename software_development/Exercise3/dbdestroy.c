#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
/*global variables*/
extern int relCatId;
extern int attrCatId;

int main(int argc,char *argv[])
{
	char command[80];
	char *dbname;

	if(argc != 2)
	{
		printf("Usage; %s dbdestroy \n",argv[0]);
		return 1;
	}

	dbname = argv[1];
	sprintf(command,"rm -r %s",dbname);
	system(command);

	return 0; 
}
