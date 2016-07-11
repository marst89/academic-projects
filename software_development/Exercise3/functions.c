#include "functions.h"
/*global variables*/
int relCatId;
int attrCatId;
int viewCatId;
int viewattrCatId;
/*-----------------------H synarthsh ayth sygkrinei dyo eggrafes sxesewn-------------------------*/
int compare_records(char *record1,char *record2,char *relation)
{
	int scandesc,attr_id,offset,length,temp_int1,temp_int2;
	float temp_float1,temp_float2;
	char attribute[ATTRCATSIZE],*temp1,*temp2,type,attrname[MAXNAME];

	if((scandesc = HF_OpenFileScan(attrCatId,ATTRCATSIZE,'c',MAXNAME,0,EQUAL,relation)) < 0)
	{
		HF_PrintError("Error in HF_OpenIndexScan called on DM_add\n");
		return -1;	
	}
	while((attr_id = HF_FindNextRec(scandesc,attribute)) != HFE_EOF)
	{
		if(attr_id < 0)
		{
			HF_PrintError("Error in HF_FindNextEntry called on DM_subtract\n");
			if(HF_CloseFileScan(scandesc) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFileScan called on compare_records\n");
				return -1; 
			}
			return -1;
		}
		memcpy(&offset,attribute + 2*MAXNAME,sizeof(int));
		memcpy(&length,attribute + 2*MAXNAME + sizeof(int),sizeof(int));
		memcpy(&type,attribute + 2*MAXNAME + 2*sizeof(int),sizeof(char));
		memcpy(attrname,attribute + MAXNAME,MAXNAME);

		switch(type)
		{
		 case 'c':
			temp1 = (char*)malloc(length);
			if(temp1 == NULL)
			{
				printf("Error in malloc called on compare_records\n");
				if(HF_CloseFileScan(scandesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFileScan called on compare_records\n");
					return -1; 
				}
				return -1;
			}
			temp2 = (char*)malloc(length);
			if(temp2 == NULL)
			{
				printf("Error in malloc called on compare_records\n");
				if(HF_CloseFileScan(scandesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFileScan called on compare_records\n");
					return -1; 
				}
				return -1;
			}
			memcpy(temp1,record1 + offset,length);
			memcpy(temp2,record2 + offset,length);
			if(strcmp(temp1,temp2) !=0)
			{
				free(temp1);
				free(temp2); 
				if(HF_CloseFileScan(scandesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFileScan called on compare_records\n");
					return -1; 
				}
				return -2;
			}
			free(temp1);
			free(temp2);
			break;
		 case 'i':
			memcpy(&temp_int1,record1 + offset,sizeof(int));
			memcpy(&temp_int2,record2 + offset,sizeof(int));
			if(temp_int1 != temp_int2)
			{
				if(HF_CloseFileScan(scandesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFileScan called on compare_records\n");
					return -1; 
				}
				return -2;
			}
			break;
		case'f':
			memcpy(&temp_float1,record1 + offset,sizeof(float));
			memcpy(&temp_float2,record2 + offset,sizeof(float));
			if(temp_float1 != temp_float2)
			{
				if(HF_CloseFileScan(scandesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFileScan called on compare_records\n");
					return -1; 
				}
				return -2;

			}
			break;
		}
	}
	if(HF_CloseFileScan(scandesc) != HFE_OK)
	{
		HF_PrintError("Error in HF_CloseFileScan called on compare_records\n");
		return -1; 
	}
	return OK;
}
/*----------------------H synarthsh ayth elegxei an ena dothen orisma einai akeraios arithmos------------------------*/
int isNumber(char *str)
{
	int size,i;
	size = strlen(str);
   
 	for(i = 0; i < size; i++)
	{
		if(str[i] < '0' || str[i] > '9')
			return 0;
   	}
	return 1;
}

/*-----------------h synarthsh ayth elegxei an ena dothen orisma einai pragmatikos arithmos------------------------*/
int isFloat(char *str)
{

    	int i = 0;

    	while(str[i] != '.')
    	{
		if(str[i] < '0' || str[i] > '9')
			return 0;
		i++;
	}
	i += 1;
   	while(str[i] != '\0')
    	{
		 if(str[i] < '0' || str[i] > '9')
                        return 0;
                i++;
    	}
     	return 1;  
}
/*----------------------H synarthsh ayth eisagei mia eggrafh sto attrCat----------------------------------------*/
int insertInattrCat(char *relname,char *attrname,int offset,int attrlength,char attrtype,int indexed,int indexno)
{
	int recId;	
	char* record;

	record = (char *)malloc(ATTRCATSIZE);
	if(record == NULL)
	{
		printf("Error in malloc called on insertInattrCat\n");
		return -1;
	}
	memset(record,'\0',ATTRCATSIZE);
	memcpy((void *)record,(void *)relname,MAXNAME);
	record += MAXNAME;	
	memcpy((void *)record,(void *)attrname,MAXNAME);
	record += MAXNAME;	
	memcpy((void *)record,(void *)&offset,sizeof(int));	
	record += sizeof(int);
	memcpy((void *)record,(void *)&attrlength,sizeof(int));
	record += sizeof(int);		
	memcpy((void *)record,(void *)&attrtype,sizeof(char));		
	record += sizeof(char);
	memcpy((void *)record,(void *)&indexed,sizeof(int));
	record += sizeof(int);
	memcpy((void *)record,(void *)&indexno,sizeof(int));
	record += sizeof(int);
	record -= ATTRCATSIZE;
	recId = HF_InsertRec(attrCatId, record,ATTRCATSIZE);
	if (recId < 0)
	{
		printf("ERROR in insertInatrrCat\n");
		HF_PrintError("Record not inserted.\n");
	}
	free(record);
	return recId;
}


/*-------------------------h synarthsh ayth eisagei mia eggrafh sto relCat---------------------------*/
int insertInrelCat(char *relname,int relwidth,int attrcnt,int indexcnt,int rec_num)
{
	int recId;	
	char* record;

	record=(char*)malloc(RELCATSIZE);
	if(record == NULL)
	{
		printf("error in malloc\n");
		return -1;
	}
	memset(record,'\0',RELCATSIZE);
	memcpy((void *)record,(void *)relname,MAXNAME);
	record += MAXNAME;
	memcpy((void *)record,(void *)&relwidth,sizeof(int));
	record += sizeof(int);
	memcpy((void *)record,(void *)&attrcnt,sizeof(int));
	record += sizeof(int);
	memcpy((void *)record,(void *)&indexcnt,sizeof(int));
	record += sizeof(int);
	memcpy((void *)record,(void *)&rec_num,sizeof(int));
	record += sizeof(int);
	record -= RELCATSIZE;
	recId = HF_InsertRec(relCatId,record,RELCATSIZE);
	if (recId < 0)
	{
		printf("ERROR in insertInrelCat\n");
		HF_PrintError("Record not inserted.\n");
	}
	free(record);
	return recId;
}
/*----Apodesmeysi tou dynamika desmeymenou pinaka apo strings pou dimiourgi8ike ap tin construct_argument--------*/
void free_argument(char ***argument,int attrno)
{
	int i;	

	for(i = 0; i < 2 + 2*attrno ; i++)free((*argument)[i]);			//apodesmeysi strings
	free(*argument);
	*argument = NULL;
	return;
}
/*----H synarthsh ayth kataskeyazei ta aparaithta orismata gia thn klhsh ths UT_create apo th select kai th join---*/
int construct_argument(int argc,char *argv[],char *** argument,int *arg_num)
{
	int i,attrsno,attret,length;
	char **buf,attribute[ATTRCATSIZE],type,format[MAXNAME];
	
	attrsno = (int)atoi(argv[2]);
	buf = NULL;


	if ((buf = (char**)malloc((2 + 2*attrsno)*sizeof(char *))) == NULL)//desmeysi deiktwn
	{
		printf("Error in malloc called on construct argument\n");
		return -1;
	}

	for(i = 0; i < 2 + 2*attrsno ; i++)					//desmeysi strings
	{
		if((buf[i] = malloc(MAXNAME)) == NULL)
		{
			printf("Error in malloc called on construct argument\n");
			return -1;
		}
	} 
	strcpy(buf[0],"create");
	strcpy(buf[1],argv[1]);

	for(i = 2; (i + 2) < (3 + 2*attrsno); i+=2)	
	{
		if((attret = giveattrcatrecord(attribute,argv[i+1],argv[i+2])) < 0)
		{
			if(attret == -1) printf("Error in giveattrcatrecord called on DM_Select\n");
			else if(attret == -2) printf("Attribute %s does not exist\n",argv[i+1]);
			return -1;
		}
 		memcpy(&type,attribute + 2*MAXNAME + 2*sizeof(int),sizeof(char));
		if(type == 'c')
		{
			memcpy(&length,attribute + 2*MAXNAME + sizeof(int),sizeof(int));
			sprintf(format,"'c%d'",length);
		}
		else if(type == 'i') strcpy(format,"'i'");
		else if(type == 'f') strcpy(format,"'f'");
		strcpy(buf[i],argv[i + 2]);
		strcpy(buf[i + 1],format);
	}
	*argument = buf;
	*arg_num = 2 + 2*attrsno;
	return 1;
}

/*---------------------Synarthsh pou epistrefei enan akeraio analoga me ton telesth pou dothike--------------------*/
int find_operator(char *oper )
{
	if(!strcmp(oper,"="))
		return 1;
	if(!strcmp(oper,"<"))
		return 2;
	if(!strcmp(oper,">"))
		return 3;
	if(!strcmp(oper,"<="))
		return 4;
	if(!strcmp(oper,">="))
		return 5;
	if(!strcmp(oper,"!="))
		return 6;

	return -1;
}

/*--------------synarthsh poy typwnei ta apotelesmata toy proswrinou arxeiou apo th select kai th join---------------*/
int print_results(char *filename,int recsize,int argc, char* argv[])
{
	int fileid,i,attrnum,length,offset,recid,temp_int;
	char record[recsize],attr_rec[ATTRCATSIZE],type,*value;
	float temp_float;

	/*plithos proballomenwn attributes*/
	attrnum=atoi(argv[1]);
	/*anoigma arxeiou*/
	if((fileid = HF_OpenFile(filename)) < 0)
	{
		HF_PrintError("Error in HF_OpenFile called on DM_select\n");
		return -1;
	} 
	if((recid = HF_GetFirstRec(fileid,record,recsize)) < 0 && recid != HFE_EOF)
	{
		HF_PrintError("Error in HF_GetFirstRec called on DM_select\n");
		if (HF_CloseFile(fileid) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_select\n");
			return -1;
		} 
		return -1;
	}
	while(recid != HFE_EOF )
	{
		offset = 0;
		for(i=2; i<= 2*attrnum; i+=2)
		{
			if(giveattrcatrecord(attr_rec,argv[i],argv[i+1]) < 0)
			{
				printf("Error in giveattrcatrecord called on DM_Select\n");
				if (HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_select\n");
					return -1;
				} 
				return -1;
			}
			memcpy(&length,attr_rec +2*MAXNAME + sizeof(int),sizeof(int));
			memcpy(&type,attr_rec +2*MAXNAME + 2*sizeof(int),sizeof(char));		
			
			switch (type)
			{
				case 'i':memcpy(&temp_int,record + offset,sizeof(int));
					 printf("%d\t",temp_int);
					 break;
				case 'f':memcpy(&temp_float,record + offset,sizeof(float));
  					printf("%.2f\t",temp_float);
					 break;
				case 'c':value = malloc(length*sizeof(char));
					if(value == NULL)
					{
						printf("Error in malloc called in print_results\n");
						if (HF_CloseFile(fileid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_select\n");
							return -1;
						} 
						return -1;
					}
					memcpy(value,record + offset +1,length); //<--- +1 gia ta aytakia
					value[strlen(value)-1] = '\0';  
					printf("%-20s\t",value);
					free(value);
					break;
			}
			offset += length;			
		}
		printf("\n");
		recid = HF_GetNextRec(fileid,recid,record,recsize);
		if(recid <0 && recid!=HFE_EOF )
		{
			HF_PrintError("Error called on HF_GetNextRec called on print_results\n");
			if (HF_CloseFile(fileid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_select\n");
				return -1;
			} 
			return -1;
		}
	}
	if (HF_CloseFile(fileid) != HFE_OK)
	{
		HF_PrintError("Error in HF_CloseFile called on DM_select\n");
		return -1;
	} 

	return OK;
}
/*-----------------------Elegxei an dyo sxeseis exoun ta idia pedia---------------------------------------------*/
int check_attributes(char *relation1,char *relation2,int attrcnt)
{
	int scanid1,scanid2,recid1,recid2,i = 0,j,flag;
	char relattr1[ATTRCATSIZE],relattr2[ATTRCATSIZE];
	attrDesc attributes1[attrcnt];
	attrDesc attributes2[attrcnt];

	if((scanid1 = HF_OpenFileScan(attrCatId,ATTRCATSIZE,'c',MAXNAME,0,EQUAL,relation1)) < 0)
	{
		HF_PrintError("Error in HF_OpenFileScan called on check_attributes\n");
		return -1;
	}

	if((scanid2 = HF_OpenFileScan(attrCatId,ATTRCATSIZE,'c',MAXNAME,0,EQUAL,relation2)) < 0)
	{
		HF_PrintError("Error in HF_OpenFileScan called on check_attributes\n");
		return -1;
	}

	while((recid1 = HF_FindNextRec(scanid1,relattr1)) != HFE_EOF)
	{
		if(recid1 < 0)
		{
			HF_PrintError("Error in HF_FindNextRec called on check_attributes\n");
			if(HF_CloseFileScan(scanid1) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFileScan called on check_attributes\n");
				return -1;
			}
			if(HF_CloseFileScan(scanid2) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFileScan called on check_attributes\n");
				return -1;
			}
			return -1;
		}
		memcpy(attributes1[i].relname,relattr1,MAXNAME);
		memcpy(attributes1[i].attrname,relattr1 + MAXNAME,MAXNAME);
		memcpy(&attributes1[i].offset,relattr1 + 2*MAXNAME,sizeof(int));
		memcpy(&attributes1[i].attrlength,relattr1 + 2*MAXNAME + sizeof(int),sizeof(int));
		memcpy(&attributes1[i].attrtype,relattr1 + 2*MAXNAME + 2*sizeof(int),sizeof(char));
		memcpy(&attributes1[i].indexed,relattr1 + 2*MAXNAME + 2*sizeof(int) + sizeof(char),sizeof(int));
		memcpy(&attributes1[i].indexno,relattr1 + 2*MAXNAME + 3*sizeof(int) + sizeof(char),sizeof(int));
		i++;
		
	}
	i = 0;
	while((recid2 = HF_FindNextRec(scanid2,relattr2)) != HFE_EOF)
	{
		if(recid2 < 0)
		{
			HF_PrintError("Error in HF_FindNextRec called on check_attributes\n");
			if(HF_CloseFileScan(scanid1) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFileScan called on check_attributes\n");
				return -1;
			}

			if(HF_CloseFileScan(scanid2) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFileScan called on check_attributes\n");
				return -1;
			}
			return -1;
		}
		memcpy(attributes2[i].relname,relattr2,MAXNAME);
		memcpy(attributes2[i].attrname,relattr2 + MAXNAME,MAXNAME);
		memcpy(&attributes2[i].offset,relattr2 + 2*MAXNAME,sizeof(int));
		memcpy(&attributes2[i].attrlength,relattr2 + 2*MAXNAME + sizeof(int),sizeof(int));
		memcpy(&attributes2[i].attrtype,relattr2 + 2*MAXNAME + 2*sizeof(int),sizeof(char));
		memcpy(&attributes2[i].indexed,relattr2 + 2*MAXNAME + 2*sizeof(int) + sizeof(char),sizeof(int));
		memcpy(&attributes2[i].indexno,relattr2 + 2*MAXNAME + 3*sizeof(int) + sizeof(char),sizeof(int));
		i++;
		
	}

	if(HF_CloseFileScan(scanid1) != HFE_OK)
	{
		HF_PrintError("Error in HF_CloseFileScan called on check_attributes\n");
		return -1;
	}

	if(HF_CloseFileScan(scanid2) != HFE_OK)
	{
		HF_PrintError("Error in HF_CloseFileScan called on check_attributes\n");
		return -1;
	}
	

	for(i = 0; i < attrcnt; i++)
	{
		flag = 0;
		for(j = 0; j < attrcnt; j++)
		{
			if(attributes2[j].offset == attributes1[i].offset)
			{
				if(attributes1[i].attrtype == attributes2[j].attrtype && attributes1[i].attrlength == attributes2[j].attrlength)
				{
					flag = 1;
					break;
				}

			}
		}
		if(flag == 0)
		{
			printf("Relations %s and %s have different offset or type or length of one or more of their attributes\n",relation1,relation2);
			return -1;
		}	
	}

	return DME_OK;
}

/*----------------------------------Synarthsh pou diagrafei tin eggrafh apo ola ta eurethria---------------------------------*/
int delete_record_from_indices(int record_id,char *record,char* relation)
{
	int scandesc,attr_id,indexed,indexno,index,offset,length;
	char attribute[ATTRCATSIZE],*value,type;

	//Anoigoume sarwsh sto arxeio attrCat
	if((scandesc = HF_OpenFileScan(attrCatId,ATTRCATSIZE,'c',MAXNAME,0,EQUAL,relation)) < 0)
	{
		HF_PrintError("Error in HF_OpenFileScan called on delete_record_from_indices\n");
		return -1;
	}
	while((attr_id = HF_FindNextRec(scandesc,attribute)) != HFE_EOF)
	{
		if(attr_id < 0)
		{
			HF_PrintError("Error in HF_FindNextEntry called on delete_record_from_indices\n");
			if(HF_CloseFileScan(scandesc) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFileScan called on delete_record_from_indices\n");
				return -1; 
			}
			return -1;
		}
		memcpy(&indexed,attribute + 2*MAXNAME + 2*sizeof(int) + sizeof(char),sizeof(int));
		if(indexed == TRUE)
		{
			memcpy(&offset,attribute + 2*MAXNAME,sizeof(int));
			memcpy(&length,attribute + 2*MAXNAME + sizeof(int),sizeof(int));
			memcpy(&type,attribute + 2*MAXNAME + 2*sizeof(int),sizeof(char));
			memcpy(&indexno,attribute + 2*MAXNAME + 3* sizeof(int) + sizeof(char),sizeof(int));
			value=(char *)malloc(length);
			if(value == NULL)
			{
				printf("Error in malloc called on delete_record_from_indices\n");
				if(HF_CloseFileScan(scandesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFileScan called on delete_record_from_indices\n");
					return -1; 
				}
				return -1;
			}
			memcpy(value,record + offset,length);
			if((index = AM_OpenIndex(relation,indexno)) < 0)
			{
				AM_PrintError("Error in AM_OpenIndex called on delete_record_from_indices\n");
				if(HF_CloseFileScan(scandesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFileScan called on delete_record_from_indices\n");
					return -1; 
				}
				return -1;
			}
			if(AM_DeleteEntry(index,type,length,value,record_id) != AME_OK)
			{
				AM_PrintError("Error in AM_DeleteEntry called on delete_record_from_indices\n");
				if(AM_CloseIndex(index) != AME_OK)
				{
					AM_PrintError("Error in AM_CloseIndex called on delete_record_from_indices\n");
					return -1;
				}
				if(HF_CloseFileScan(scandesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFileScan called on delete_record_from_indices\n");
					return -1; 
				}
				return -1;
			}
			free(value);
			if(AM_CloseIndex(index) != AME_OK)
			{
				AM_PrintError("Error in AM_CloseIndex called on delete_record_from_indices\n");
				if(HF_CloseFileScan(scandesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFileScan called on delete_record_from_indices\n");
					return -1; 
				}
				return -1;
			}
		}
	}
	if(HF_CloseFileScan(scandesc) != HFE_OK)
	{
		HF_PrintError("Error in HF_CloseFileScan called on delete_record_from_indices\n");
		return -1; 
	}	
	return DME_OK;
}

/*-------------------------------Synarthsh pou eisagei thn eggrafh se ola ta eurethria------------------------------*/
int insert_record_into_indices(int record_id,char *record,char *relation)
{
	int scandesc,attr_id,indexed,indexno,index,offset,length;
	char attribute[ATTRCATSIZE],*value,type;

	//Anoigoume sarwsh sto arxeio attrCat
	if((scandesc = HF_OpenFileScan(attrCatId,ATTRCATSIZE,'c',MAXNAME,0,EQUAL,relation)) < 0)
	{
		HF_PrintError("Error in HF_OpenFileScan called on delete_record_from_indices\n");
		return -1;
	}
	while((attr_id = HF_FindNextRec(scandesc,attribute)) != HFE_EOF)
	{
		if(attr_id < 0)
		{
			HF_PrintError("Error in HF_FindNextEntry called on delete_record_from_indices\n");
			if(HF_CloseFileScan(scandesc) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFileScan called on delete_record_from_indices\n");
				return -1; 
			}
			return -1;
		}
		memcpy(&indexed,attribute + 2*MAXNAME + 2*sizeof(int) + sizeof(char),sizeof(int));
		if(indexed == TRUE)
		{
			memcpy(&offset,attribute + 2*MAXNAME,sizeof(int));
			memcpy(&length,attribute + 2*MAXNAME + sizeof(int),sizeof(int));
			memcpy(&type,attribute + 2*MAXNAME + 2*sizeof(int),sizeof(char));
			memcpy(&indexno,attribute + 2*MAXNAME + 3* sizeof(int) + sizeof(char),sizeof(int));
			value=(char *)malloc(length);
			if(value == NULL)
			{
				printf("Error in malloc called on delete_record_from_indices\n");
				if(HF_CloseFileScan(scandesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFileScan called on delete_record_from_indices\n");
					return -1; 
				}
				return -1;
			}
			memcpy(value,record + offset,length);
			if((index = AM_OpenIndex(relation,indexno)) < 0)
			{
				AM_PrintError("Error in AM_OpenIndex called on delete_record_from_indices\n");
				if(HF_CloseFileScan(scandesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFileScan called on delete_record_from_indices\n");
					return -1; 
				}
				return -1;
			}
			if(AM_InsertEntry(index,type,length,value,record_id) != AME_OK)
			{
				AM_PrintError("Error in AM_InsertEntry called on delete_record_from_indices\n");
				if(AM_CloseIndex(index) != AME_OK)
				{
					AM_PrintError("Error in AM_CloseIndex called on delete_record_from_indices\n");
					return -1;
				}
				if(HF_CloseFileScan(scandesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFileScan called on delete_record_from_indices\n");
					return -1; 
				}
				return -1;
			}
			free(value);
			if(AM_CloseIndex(index) != AME_OK)
			{
				AM_PrintError("Error in AM_CloseIndex called on delete_record_from_indices\n");
				return -1;
			}
		}
	}
	if(HF_CloseFileScan(scandesc) != HFE_OK)
	{
		HF_PrintError("Error in HF_CloseFileScan called on delete_record_from_indices\n");
		return -1; 
	}	
	return DME_OK;

}

/*---------------------------Synarthsh pou diagrafei ta pedia ths sxeshs relation---------------------------*/
int delete_attributes(char *relation)
{
	int scandesc,recid;
	char record[ATTRCATSIZE];

	if((scandesc = HF_OpenFileScan(attrCatId,ATTRCATSIZE,'c',MAXNAME,0,EQUAL,relation)) < 0)
	{
		HF_PrintError("Error in HF_OpenIndexScan\n");
		return -1;	
	}

	while((recid = HF_FindNextRec(scandesc,record)) != HFE_EOF)
	{
		if(HF_DeleteRec(attrCatId,recid,ATTRCATSIZE) != HFE_OK)
		{
			HF_PrintError("Error in HF_DeleteRec\n");
			if(HF_CloseFileScan(scandesc) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFileScan\n");
				return -1;
			}
			return -1;	
		}

	}

	
	if(HF_CloseFileScan(scandesc) != HFE_OK)
	{
		HF_PrintError("Error in HF_CloseFileScan\n");
		return -1;
	}
	return UTE_OK;

}

/*--------------------------------------Ananewnei to relCat-----------------------------------------*/
int update_relations(int recid,char *record)
{
	if(HF_DeleteRec(relCatId,recid,RELCATSIZE) != HFE_OK)
	{
		HF_PrintError("Error in HF_DeleteRec\n");
		return -1;	
	}

	if(HF_InsertRec(relCatId,record,RELCATSIZE) < 0)
	{
		HF_PrintError("Error in  HF_InsertRec\n");
		return -1;	
	}

	return UTE_OK;
}

/*---------------------------------------Ananewnei to attrCat---------------------------------------*/
int update_attributes(int recid,char *record)
{
	
	if(HF_DeleteRec(attrCatId,recid,ATTRCATSIZE) != HFE_OK)
	{
		HF_PrintError("Error in HF_DeleteRec\n");
		return -1;	
	}

	if(HF_InsertRec(attrCatId,record,ATTRCATSIZE) < 0)
	{
		HF_PrintError("Error in  HF_InsertRec\n");
		return -1;	
	}
	return UTE_OK;
			
}
/*-------------------------Fernei mia eggrafi apo to relCat---------------------------------------*/
int giverelcatrecord(char* record,char *relation)
{
	int scandesc,recid;	

	if((scandesc = HF_OpenFileScan(relCatId,RELCATSIZE,'c',MAXNAME,0,EQUAL,relation)) < 0)
	{
		HF_PrintError("Error in HF_OpenFileScan\n");
		return -1;
	}
	
	recid = HF_FindNextRec(scandesc,record); 
	if(recid<0 && recid!=HFE_EOF)
	{
		if(HF_CloseFileScan(scandesc) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFileScan\n");
			return -1;
		}
		HF_PrintError("Error in HF_FindNextRec\n");
		return -1;
	}
	else if(recid == HFE_EOF)
	{
		if(HF_CloseFileScan(scandesc) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFileScan\n");
			return -1;
		}		
		return -2;
	}
	if(HF_CloseFileScan(scandesc) != HFE_OK) 
	{
		HF_PrintError("Error in HF_CloseFileScan\n");
		return -1;
	}

	return recid;	
}
/*-----------------------------------Fernei mia eggrafi apo to attrCat-------------------------------------------*/
int giveattrcatrecord(char *record,char *relation,char *attribute)
{
	int scandesc,recid;
	char temp[MAXNAME];

	if((scandesc = HF_OpenFileScan(attrCatId,ATTRCATSIZE,'c',MAXNAME,0,EQUAL,relation)) < 0)
	{
		HF_PrintError("Error in HF_OpenIndexScan\n");
		return -1;	
	}
	
	while((recid = HF_FindNextRec(scandesc,record)) >0) 
	{
		memcpy(temp,record + MAXNAME,MAXNAME);
		if(!strcmp(temp, attribute))
		{
			if(HF_CloseFileScan(scandesc) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFileScan\n");
				return -1;
			}
			return recid;
			
		}
	}
	if(recid < 0 && recid !=HFE_EOF)
	{
		if(HF_CloseFileScan(scandesc) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFileScan\n");
		}
		HF_PrintError("Error in HF_FindNextRec\n");
		return -1;
	}
	else
	{
		if(HF_CloseFileScan(scandesc) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFileScan\n");
			return -1;
		}
		return -2; 
	}

}

