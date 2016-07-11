#include "functions.h"
#include "parser3.h"
/*global variables*/
extern int relCatId;
extern int attrCatId;
extern int viewCatId;
extern int viewattrCatId;

int main(int argc,char *argv[])
{
	char command[80];
	char *dbname;
	
	if(argc != 2)
	{
		printf("Usage: %s aminirel \n", argv[0]);
		return 1;
	}
	dbname = argv[1];

	HF_Init();
	AM_Init();

	if(chdir(dbname))
	{
		printf("The datebase could not be created\n");
		return 2;
	}

	if((relCatId = HF_OpenFile("relCat")) < 0)
	{
		printf("aminirel:HF_OpenFile An error occured while opening relCat file\n");
		return -1;
	}

	if((attrCatId = HF_OpenFile("attrCat")) < 0)
	{
		printf("aminirel:HF_OpenFile An error occured while opening attrCat file\n");
		return -1;
	}

	if(( viewCatId = HF_OpenFile("viewCat")) < 0)
	{
		printf("aminirel:HF_OpenFile An error occured while opening viewCat file\n");
		return -1;
	}


	if(( viewattrCatId = HF_OpenFile("viewAttrCat")) < 0)
	{
		printf("aminirel:HF_OpenFile An error occured while opening viewCat file\n");
		return -1;
	}


	while(yyparse() == RESTART);

	return 0;

}
