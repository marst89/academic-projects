#include "UT_Lib.h"
/*global variables*/
extern int relCatId;
extern int attrCatId;
extern int viewCatId;
extern int viewattrCatId;

int UT_create(int argc,char *argv[])
{
	int i,relwidth = 0,attrcnt=0,scandesc,number;
	int offset = 0;
	char temp[MAXNAME],record[RELCATSIZE];	
	
	
	//anoigoume sarwsh sto arxeio relCat gia na elexoume an h sxesh me onoma argv[1] yparxei hdh
	if((scandesc = HF_OpenFileScan(relCatId,RELCATSIZE,'c',MAXNAME,0,EQUAL,argv[1])) <0)
	{
		HF_PrintError("Error in HF_OpenFileScan called on UT_create\n ");
		return -1;	
	}
	if(HF_FindNextRec(scandesc,record) > 0)		
	{
		printf("The relation %s already exists\n",argv[1]);
		if(HF_CloseFileScan(scandesc) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFileScan called on UT_create\n ");
			return -1;
		}
		return -1;	
	}
	if(HF_CloseFileScan(scandesc) != HFE_OK)
	{
		HF_PrintError("Error in HF_CloseFileScan called on UT_create\n ");
		return -1;
	}
	//dhmiourgia arxeiou 
	if((HF_CreateFile(argv[1])) != HFE_OK)
	{
		printf("Error in UT_create\n");
		HF_PrintError("Error in HF_CreateFile\n");
	}

	/*---------------Metrame to mikos tis eggrafis kai ton arithmo twn pediwn--------------------------*/
	for(i = 3; i < argc;i += 2)
	{
		switch(argv[i][1])
		{	
			case 'i':
			case 'f':
				relwidth += 4;
				break;
			case 'c':
				strcpy(temp,argv[i] + 2);				
				temp[strlen(temp)] = '\0';
				number = (int)atoi(temp);
				if(number > 255)
				{
					printf("Wrong argument given.Correct type cXXX\n");

				}
				relwidth += number;
				break;
		}
	attrcnt++;
	}
	/*-------------Enhmerwsh tou relation catalogue me th nea sxeh pou dhmiourghthhke-------------------*/
	if(insertInrelCat(argv[1],relwidth,attrcnt,-1,0) == -1 )
	{
		printf("Sfalma stin insertInrelcat\n");
		return -1;
	}
	relwidth = 0;
	/*----------------Enhmerwsh tou attribute catalogue me ta pedia ths pros dhmiourgia sxeshs---------*/
	for(i = 2; i < argc;i += 2)
	{
		switch(argv[i + 1][1])
		{	
			case 'i':
				if(insertInattrCat(argv[1],argv[i],relwidth,sizeof(int),'i',0,-1) == -1 )
				{
					printf("Sfalma stin insertInattrcat\n");
					return -1;
				}
				relwidth += 4;
				break;
			case 'f':
				if(insertInattrCat(argv[1],argv[i],relwidth,sizeof(float),'f',0,-1) == -1 )
				{
					printf("Sfalma stin insertInattrcat\n");
					return -1;
				}
				relwidth += 4;
				break;
			case 'c':
				strcpy(temp,argv[i + 1] + 2);				
				temp[strlen(temp)] = '\0';
				offset = (int)atoi(temp);
				if(offset > 255)
				{
					printf("Wrong argument given.Correct type cXXX\n");

				}
				if(insertInattrCat(argv[1],argv[i],relwidth,offset,'c',0,-1) == -1 )
				{
					printf("Sfalma stin insertInattrcat\n");
					return -1; 
				}
				relwidth += offset;
				break;	
		}	
	}
	return UTE_OK;	
}

int UT_buildindex(int argc,char *argv[])
{
	int recid1,recid2,recid,indexcnt,indexno,length,indexed,filedesc,scandesc,relwidth,offset;
	char record1[ATTRCATSIZE],record2[RELCATSIZE],type,*record_read,*value;

	
	if(argc != 3)     
	{
		printf("UT_buildindex ::Wrong number of arguments given\n");
		return -1;
	}

	/*-----------Elegxos an yparxei h sxesh gia ths opoias kapoio pedio theloume na dhmiourghsoume eurethrio-----*/
	if((recid2 = giverelcatrecord(record2,argv[1])) < 0)  
	{
		if(recid2 == -1)	printf("Error in giverelcatrecord\n");
		else if(recid2 == -2)	printf("Relation %s does not exist\n",argv[1]);
		return -1;
	}
	
	/*-----------Elegxos an yparxei to pedio gia to opoio theloume na dhmiourghsoume eurethrio------------------*/
	if((recid1 = giveattrcatrecord(record1,argv[1],argv[2])) < 0)  
	{
		if(recid1 == -1)	printf("Error in giveattrcatrecord\n");
		else if(recid1 == -2)	printf("Attribute %s does not exist\n",argv[2]);
		return -1;
	}
	
	
	/*-------------------------------Enhmerwsh twn attrCat kai relCat-----------------------------------------*/  
	memcpy(&relwidth,record2 + MAXNAME,sizeof(int));
	memcpy(&indexed,record1 + 2*MAXNAME + 2*sizeof(int) + sizeof(char),sizeof(int));
	/*------------------------------Elegxos an to pedio exei hdh eurethrio-----------------------------------*/
	if(indexed == TRUE)
	{
		printf("Attribute %s  has already an index\n",argv[2]);
		return UTE_OK;
	}
	memcpy(&indexcnt,record2 + MAXNAME + 2*sizeof(int),sizeof(int));  /*to eyrethrio pou tha dhmiourghsoume tha exei arithmo indexcnt+1*/
	indexcnt++;
	memcpy(&offset,record1 + 2*MAXNAME,sizeof(int));
	memcpy(&length,record1 + 2*MAXNAME + sizeof(int),sizeof(int));
	memcpy(&type,record1 + 2*MAXNAME + 2*sizeof(int),sizeof(char));
	memcpy(record2 + MAXNAME + 2*sizeof(int),&indexcnt,sizeof(int));
	indexed = TRUE;
	indexno = indexcnt;
	memcpy(record1 + 2*MAXNAME + 2*sizeof(int) + sizeof(char),&indexed,sizeof(int));
	memcpy(record1 + 2*MAXNAME + 3*sizeof(int) + sizeof(char),&indexno,sizeof(int));
		

	if(update_attributes(recid1,record1) < 0)
	{
		printf("Error in update_attributes called on UT_buildindex\n");
		return -1;
	}
	
	if(update_relations(recid2,record2) < 0)
	{
		printf("Error in update_relations called on UT_buildindex\n");
		return -1;
	}	
	/*-------------------------------------Dhmiourgia eyrethriou-----------------------------------------*/
	if(AM_CreateIndex(argv[1],indexcnt,type,length) != AME_OK)  
	{
		AM_PrintError("Error in AM_CreateIndex called on UT_buildindex\n");
		return -1;
	}
	/*-----------------------Eisagwgh sto eurethrio twn hdh yparxouswn eggrafwn apo th sxesh------------*/
	/*----------------------------------Anoigma arxeiou-------------------------------------------------*/
	if((filedesc=HF_OpenFile(argv[1])) <0)
	{
		HF_PrintError("Error in HF_OpenFile called on UT_buildindex\n");
		return -1;
	}
	/*----------------------------------Anoigma arxeiou eyrethriou-------------------------------------*/
	if((scandesc=AM_OpenIndex(argv[1],indexcnt)) < 0)
	{
		AM_PrintError("Error in AM_OpenIndex called on UT_buildindex\n");
		if(HF_CloseFile(filedesc) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on UT_buildindex\n");
			return -1;
		}
		return -1;
	}
	record_read=(char *)malloc(relwidth);
	if(record_read == NULL)
	{
		printf("Error in malloc called on UT_buildindex\n");
		if(HF_CloseFile(filedesc) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on UT_buildindex\n");
			return -1;
		}
		if(AM_CloseIndex(scandesc) !=AME_OK)
		{
			AM_PrintError("Error in AM_CloseIndex called on UT_buildindex\n");
			return -1;
		}	
		return -1;
	}
	value=(char *)malloc(length);
	if(value == NULL)
	{
		printf("Error in malloc called on UT_buildindex\n");
		if(HF_CloseFile(filedesc) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on UT_buildindex\n");
			return -1;
		}
		if(AM_CloseIndex(scandesc) !=AME_OK)
		{
			AM_PrintError("Error in AM_CloseIndex called on UT_buildindex\n");
			return -1;
		}	
		return -1;
	}
	recid=HF_GetFirstRec(filedesc,record_read,relwidth);
	if(recid < 0 && recid !=HFE_EOF)
	{
		HF_PrintError("Error in HF_GetFirstRec called on UT_buildindex\n");
		if(HF_CloseFile(filedesc) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on UT_buildindex\n");
			return -1;
		}
		if(AM_CloseIndex(scandesc) !=AME_OK)
		{
			AM_PrintError("Error in AM_CloseIndex called on UT_buildindex\n");
			return -1;
		}	
		return -1;
	}
	while(recid != HFE_EOF)
	{
		memcpy(value,record_read + offset,length);
		if(AM_InsertEntry(scandesc,type,length,value,recid) != AME_OK)
		{
			AM_PrintError("Error in AM_InsertEntry called on UT_buildindex\n");
			if(HF_CloseFile(filedesc) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on UT_buildindex\n");
				return -1;
			}
			if(AM_CloseIndex(scandesc) !=AME_OK)
			{
				AM_PrintError("Error in AM_CloseIndex called on UT_buildindex\n");
				return -1;
			}	
			return -1;
		}
		recid=HF_GetNextRec(filedesc,recid,record_read,relwidth);
		if(recid < 0 && recid !=HFE_EOF )
		{
			HF_PrintError("Error in HF_GetNextRec called on UT_buildIndex\n");
			if(HF_CloseFile(filedesc) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on UT_buildindex\n");
				return -1;
			}
			if(AM_CloseIndex(scandesc) !=AME_OK)
			{
				AM_PrintError("Error in AM_CloseIndex called on UT_buildindex\n");
				return -1;
			}	
			return -1;
		}
	} 
	free(record_read);
	free(value);
	//Kleisimo arxeiou
	if(HF_CloseFile(filedesc) != HFE_OK)
	{
		HF_PrintError("Error in HF_CloseFile called on UT_buildindex\n");
		return -1;
	}
	//Kleisimo arxeiou eyrethriou
	if(AM_CloseIndex(scandesc) !=AME_OK)
	{
		AM_PrintError("Error in AM_CloseIndex called on UT_buildindex\n");
		return -1;
	}
	return DME_OK;
}

int UT_destroy(int argc, char *argv[])
{
	int recid1,indexcnt,i;
	char record1[RELCATSIZE]; 

	if(argc != 2)   
	{
		printf("UT_destroy ::Wrong number of arguments given\n");
		return -1;
	}
	/*----------------elegxos an h pros katastrofi sxesh yparxei hdh------------------------------------*/
	if((recid1 = giverelcatrecord(record1,argv[1])) < 0) 
	{
		if(recid1 == -1)	printf("Error in giverelcatrecord\n");
		else if(recid1 == -2)	printf("Relation %s does not exist\n",argv[1]);
		return -1;
	}

	memcpy(&indexcnt,record1 + MAXNAME + 2*sizeof(int),sizeof(int));
	/*----------------------------------------Katastrofi twn eurethriwn-----------------------------------*/	
	for(i = 0;i <= indexcnt; i++)
	{
		if(AM_DestroyIndex(argv[1],i) != AME_OK)
		{
			AM_PrintError("Error in Am_DestroyIndex\n");
			return -1;
		} 
		
	}
	/*---------------------Enhmerwsh tou relCat mesw ths diagrafhs ths eggrafhs gia th sxesh ayth-------*/
	if(HF_DeleteRec(relCatId,recid1,RELCATSIZE) != HFE_OK)
	{
		HF_PrintError("Error in HF_DeleteRec\n");
		return -1;	
	}
	/*--Enhmerwsh tou attrCat mesw ths diagrafhs twn antistoixwn eggrafwn gia ta pedia ths sxeshs ayths--*/
	if(delete_attributes(argv[1]) != UTE_OK)
	{
		printf("Error in delete_attributes\n");
		return -1;
	}
	/*------------------------------katastrofi tou arxeiou-sxeshs----------------------------------------*/
	if(HF_DestroyFile(argv[1]) != HFE_OK)
	{
		HF_PrintError("Error in HF_DestroyFile\n");
		return -1;
	}

	return UTE_OK;
}

int UT_quit(void)
{	
	if(HF_CloseFile(attrCatId) != HFE_OK)
    	{
		HF_PrintError("Error in HF_CloseFile");
		return -1;
       }

	if(HF_CloseFile(relCatId) != HFE_OK)
       {
		HF_PrintError("Error in HF_CloseFile");
		return -1;
       }

	if(HF_CloseFile(viewCatId) != HFE_OK)
    	{
		HF_PrintError("Error in HF_CloseFile");
		return -1;
       }

	if(HF_CloseFile(viewattrCatId) != HFE_OK)
    	{
		HF_PrintError("Error in HF_CloseFile");
		return -1;
       }

	printf("You just chose to exit!Byee!\n");	

	exit(1);
}
