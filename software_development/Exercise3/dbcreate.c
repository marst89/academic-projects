#include "functions.h"
/*global variables*/
extern int relCatId;
extern int attrCatId;

int main(int argc,char *argv[])
{
	char command[180];
	char *dbname;
		
	if(argc != 2)
	{
		printf("Usage: %s dbname \n",argv[0]);
		return 1;
	}

	HF_Init();
	AM_Init();

	dbname = argv[1];
	sprintf(command,"mkdir %s ",dbname);
	system(command);
	
	if(chdir(dbname))
	{
		printf("The datebase could not be created\n");
		return 2;
	}

	if((HF_CreateFile("relCat")) != HFE_OK)
	{
		printf("dbcreate:  HF_Create failed in creating relCat file \n");
		return -1;
	}

	if((HF_CreateFile("attrCat")) != HFE_OK)
	{
		printf("dbcreate:  HF_Create failed in creating attrCat file \n");
		return -1;
	}

	if((attrCatId = HF_OpenFile("attrCat")) < 0)
	{
		printf("dbcreate:HF_OpenFile An error occured while opening attrCat file\n");
		return -1;
	}

	if((relCatId = HF_OpenFile("relCat")) < 0)
	{
		printf("dbcreate:HF_OpenFile An error occured while opening relCat file\n");
		return -1;
	}

	if((HF_CreateFile("viewCat")) != HFE_OK)
	{
		printf("dbcreate:  HF_Create failed in creating viewCat file \n");
		return -1;
	}
	
	if((HF_CreateFile("viewAttrCat")) != HFE_OK)
	{
		printf("dbcreate:  HF_Create failed in creating viewAttrCat file \n");
		return -1;
	}	

	
	if(insertInattrCat("relCat","relname",0,MAXNAME,'c',FALSE,-1)<0)
	{
		printf("dbcreate : Error in insertInattrCat");
		return -1;
	}

	if(insertInattrCat("relCat","relwidth",MAXNAME,sizeof(int),'i',FALSE,-1)<0)
	{
		printf("dbcreate : Error in insertInattrCat");
		return -1;
	}

	if(insertInattrCat("relCat","attrcnt",MAXNAME + sizeof(int),sizeof(int),'i',FALSE,-1)<0)
	{
		printf("dbcreate : Error in insertInattrCat");
		return -1;
	}
	
	if(insertInattrCat("relCat","indexcnt",MAXNAME + 2*sizeof(int),sizeof(int),'i',FALSE,-1)<0)
	{
		printf("dbcreate : Error in insertInattrCat");
		return -1;
	}

	if(insertInattrCat("attrCat","relname",0,MAXNAME,'c',FALSE,-1)<0)
	{
		printf("dbcreate : Error in insertInattrCat");
		return -1;
	}

	if(insertInattrCat("attrCat","attrname",MAXNAME,MAXNAME,'c',FALSE,-1)<0)
	{
		printf("dbcreate : Error in insertInattrCat");
		return -1;
	}

	if(insertInattrCat("attrCat","offset",2*MAXNAME,sizeof(int),'i',FALSE,-1)<0)
	{
		printf("dbcreate : Error in insertInattrCat");
		return -1;
	}

	if(insertInattrCat("attrCat","attrlength",2*MAXNAME + sizeof(int),sizeof(int),'i',FALSE,-1)<0)
	{
		printf("dbcreate : Error in insertInattrCat");
		return -1;
	}

	if(insertInattrCat("attrCat","attrtype",2*MAXNAME + 2*sizeof(int),sizeof(char),'c',FALSE,-1)<0)
	{
		printf("dbcreate : Error in insertInattrCat");
		return -1;
	}

	if(insertInattrCat("attrCat","indexed",2*MAXNAME + 2*sizeof(int) + sizeof(char),sizeof(int),'i',FALSE,-1)<0)
	{
		printf("dbcreate : Error in insertInattrCat");
		return -1;
	}	

	if(insertInattrCat("attrCat","indexno",2*MAXNAME + 3*sizeof(int) + sizeof(char),sizeof(int),'i',FALSE,-1)<0)
	{
		printf("dbcreate : Error in insertInattrCat");
		return -1;
	}	
	

	if(insertInattrCat("viewCat","viewname",0,MAXNAME,'c',FALSE,-1)<0)
	{
		printf("dbcreate : Error in insertInattrCat");
		return -1;
	}	

	if(insertInattrCat("viewCat","type",MAXNAME,sizeof(short int),'i',FALSE,-1)<0)
	{
		printf("dbcreate : Error in insertInattrCat");
		return -1;
	}

	if(insertInattrCat("viewCat","relname1",MAXNAME + sizeof(short int),MAXNAME,'c',FALSE,-1)<0)
	{
		printf("dbcreate : Error in insertInattrCat");
		return -1;
	}

	if(insertInattrCat("viewCat","attrname1",2*MAXNAME + sizeof(short int),MAXNAME,'c',FALSE,-1)<0)
	{
		printf("dbcreate : Error in insertInattrCat");
		return -1;
	}

	if(insertInattrCat("viewCat","op",3*MAXNAME + sizeof(short int),sizeof(int),'i',FALSE,-1)<0)
	{
		printf("dbcreate : Error in insertInattrCat");
		return -1;
	}

	if(insertInattrCat("viewCat","relname2",3*MAXNAME + sizeof(short int) + sizeof(int),MAXNAME,'c',FALSE,-1)<0)
	{
		printf("dbcreate : Error in insertInattrCat");
		return -1;
	}

	if(insertInattrCat("viewCat","attrname2",4*MAXNAME + sizeof(short int) + sizeof(int),MAXNAME,'c',FALSE,-1)<0)
	{
		printf("dbcreate : Error in insertInattrCat");
		return -1;
	}

	if(insertInattrCat("viewCat","value",5*MAXNAME + sizeof(short int) + sizeof(int),MAXSTRINGLENGTH,'c',FALSE,-1)<0)
	{
		printf("dbcreate : Error in insertInattrCat");
		return -1;
	}

	if(insertInattrCat("viewCat","attrcnt",5*MAXNAME + sizeof(short int) + sizeof(int) + MAXSTRINGLENGTH,sizeof(int),'i',FALSE,-1)<0)
	{
		printf("dbcreate : Error in insertInattrCat");
		return -1;
	}

	if(insertInattrCat("viewattrCat","viewname",0,MAXNAME,'c',FALSE,-1)<0)
	{
		printf("dbcreate : Error in insertInattrCat");
		return -1;
	}

	if(insertInattrCat("viewattrCat","viewattrname",MAXNAME,MAXNAME,'c',FALSE,-1)<0)
	{
		printf("dbcreate : Error in insertInattrCat");
		return -1;
	}

	if(insertInattrCat("viewattrCat","relname",2*MAXNAME,MAXNAME,'c',FALSE,-1)<0)
	{
		printf("dbcreate : Error in insertInattrCat");
		return -1;
	}

	if(insertInattrCat("viewattrCat","relattrname",3*MAXNAME,MAXNAME,'c',FALSE,-1)<0)
	{
		printf("dbcreate : Error in insertInattrCat");
		return -1;
	}

	if(insertInrelCat("relCat",RELCATSIZE,4,-1,0) < 0)
	{
		printf("dbcreate : Error in insertInrelCat");
		return -1;
	}

	if(insertInrelCat("attrCat",ATTRCATSIZE,7,-1,0) < 0)
	{
		printf("dbcreate : Error in insertInrelCat");
		return -1;
	}

	if(insertInrelCat("viewCat",VIEWCATSIZE,9,-1,0) < 0)
	{
		printf("dbcreate : Error in insertInrelCat");
		return -1;
	}	

	if(insertInrelCat("viewattrCat",VIEWATTRCATSIZE,4,-1,0) < 0)
	{
		printf("dbcreate : Error in insertInrelCat");
		return -1;
	}	

	if(HF_CloseFile(relCatId) != HFE_OK)
	{
		HF_PrintError("Error in HF_CloseFile");
	}

	if(HF_CloseFile(attrCatId) != HFE_OK)
	{
		HF_PrintError("Error in HF_CloseFile");
	}
	return 0;
	
}
