#include "DM_Lib.h"
/*global variables*/
extern int relCatId;
extern int attrCatId;

int DM_select(int argc,char* argv[])
{
	int fileid,num_of_attrs,recid,mikos_eggr = 0,relwidth,f_rec_id,offset,operator,indexed,indexno,indexid,length,i,l;
	char *ptr,record1[RELCATSIZE],*record_read,*value,*attrname,type,*value_read;
	char relation[MAXNAME],**argument,attrrecord[ATTRCATSIZE],record[ATTRCATSIZE];
	int begin,arg_num,help,readid,scanid,record_id;	
	float helpf;

	if(argc % 2 == 0) //Periptwsh pou den exoume sxesh apotelesmatos
	{
		begin = 2;
		num_of_attrs = atoi(argv[1]);
		strcpy(relation,argv[2]);
		//eleghos an ta pedia pros probolh proerxontai apo thn idia sxesh
		for(i = 2,l = 0 ;l < num_of_attrs; i+=2,l++) 
		{
			if(strcmp(argv[i],argv[2]) != 0)
			{
				printf("DM_select ::The attributes asked do not derive from the same relation\n");
				return -1;
			}
		}
		//dhmiourgia enos proswrinou arxeiou opou tha apothhkeysoume proswrina ta apotelesmata ths Select 
		if(HF_CreateFile("temprelation") != HFE_OK )
		{
			HF_PrintError("Error in HF_CreateFile called on DM_select\n");
			return -1;
		}
		if((fileid = HF_OpenFile("temprelation")) < 0)
		{
			HF_PrintError("Error in HF_OpenFile called on DM_select\n");
			return -1;
		} 
	}
	else //periptwsh pou exoume sxesh apotelesmatos
	{
		begin = 3;
		num_of_attrs = atoi(argv[2]);
		strcpy(relation,argv[3]);
		//eleghos an ta pedia pros probolh proerxontai apo thn idia sxesh
		for(i = 3,l = 0;l < num_of_attrs; i+=2,l++)  
		{
			if(strcmp(argv[i],argv[3]) != 0)
			{
				printf("DM_select ::The attributes asked do not derive from the same relation\n");
				return -1;
			}
		}
		//kataskeyh orismatwn ths UT_Create gia th sxesh apotelesmatos
		if(construct_argument(argc,argv,&argument,&arg_num) != 1)
		{
			printf("Error in construct_argument called on DM_select\n");
			return -1;
		} 
		//Kataskeyh ths neas sxeshs
		if(UT_create(arg_num,argument) != UTE_OK)
		{
			printf("Error in UT_Create called on DM_select\n");
			return -1;
		}
		free_argument(&argument,num_of_attrs);
		//anoigma arxeiou neas sxeshs
		if((fileid = HF_OpenFile(argv[1])) < 0)
		{
			HF_PrintError("Error in HF_OpenFile called on DM_select\n");
			return -1;
		} 
	}

	//fernoume thn eggrafh apo to relCat pou antistoixei sth sxesh apo thn opoia proerxontai ta pedia pros probolh an yparxei h sxesh
	if((recid = giverelcatrecord(record1,relation)) < 0)
	{
		if(recid == -1) printf("Error in giverelcatrecord called on DM_select");
		else printf("Relation %s does not exist\n",relation); 
		if(HF_CloseFile(fileid) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_select\n");
			return -1;
		}
		return -1;
	}
	//anoigoume to arxeio ths sxeshs apo thn opoia proerxontai ta pedia pros probolh
	if((readid = HF_OpenFile(argv[begin])) < 0 )
	{
		HF_PrintError("error in HF_Openfile called on DM_select\n");
		if(HF_CloseFile(fileid) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_select\n");
			return -1;
		}
		return -1;
	}
	memcpy(&relwidth,record1 + MAXNAME,sizeof(int));	
	if(argv[i] != NULL)//exoume synthhkh where
	{
		if((operator = find_operator(argv[argc - 2])) == -1)
		{
			printf("The operator given is not valid\n");
			return -1;
		}
		attrname = argv[argc - 3];
		value = argv[argc - 1];
	
		if(strcmp(relation,argv[argc - 4]))//Eleghos an h sxesh ths synthkhs einai idia me 
                                          //aythn apo thn opoia ta pedia  
		{
			printf("The relation in where condition is different from the others given\n ");
			if(HF_CloseFile(fileid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_Select\n");
				return -1;
			}
			if(HF_CloseFile(readid) != HFE_OK)
			{	
				HF_PrintError("error in HF_Closefile called on DM_Select\n");
				return -1;
			}
			return -1;
		}
              //fernoume thn eggrafh apo to attrCat pou antisoixei sto pedio ths synthhkhs
		if((recid = giveattrcatrecord(attrrecord,relation,attrname)) < 0 )
		{
			if(recid == -1)printf("Error in giveattrcatrecord called on DM_Select\n");
			else if(recid == -2)printf("Attribute %s does not exist\n",attrname);
			if(HF_CloseFile(fileid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_Select\n");
				return -1;
			}
			if(HF_CloseFile(readid) != HFE_OK)
			{	
				HF_PrintError("error in HF_Closefile called on DM_Select\n");
				return -1;
			}
			return -1;
		}
	       //pairnoume plhrofories gia th sxesh
		memcpy(&indexed,attrrecord + 2*MAXNAME + 2*sizeof(int) + sizeof(char),sizeof(int));
		memcpy(&type,attrrecord + 2*MAXNAME + 2*sizeof(int),sizeof(char));
		memcpy(&length,attrrecord + 2*MAXNAME + sizeof(int),sizeof(int));
              //Eleghos an to value einai tou idiou typou me to pedio
		if(type == 'i')
		{
			if(isNumber(value) != 1)
			{
				printf("The value given is not an integer as it should be according to its type\n");
				return -2;
			}	
			help = (int)atoi(argv[argc - 1]);
			value = (char*)&help;
		}
		if(type == 'f')
		{
			if(isFloat(value) != 1)
			{
				printf("The value given is not a float as it should be according to its type\n");
				return -2;
			}
			helpf = (float)atof(argv[argc - 1]);
			value = (char*)&helpf;
		}
		
		if(indexed == TRUE)//to pedio einai eyrethriasmeno
		{	
			record_read = malloc(relwidth);
			if(record_read == NULL)
			{
				printf("Error in malloc called on DM_select\n");
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_Select\n");
					return -1;
				}
				if(HF_CloseFile(readid) != HFE_OK)
				{	
					HF_PrintError("error in HF_Closefile called on DM_Select\n");
					return -1;
				}
				return -1;
			}
			
			memcpy(&indexno,attrrecord + 2*MAXNAME + 3*sizeof(int) + sizeof(char),sizeof(int));
			//anoigoume to eyrethrio pou antistoixei sto pedio ths synthhhs
			if((indexid = AM_OpenIndex(relation,indexno)) < 0)
			{
				AM_PrintError("Error in AM_OpenIndex\n");
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_Select\n");
					return -1;
				}
				if(HF_CloseFile(readid) != HFE_OK)
				{	
					HF_PrintError("error in HF_Closefile called on DM_Select\n");
					return -1;
				}
				return -1;
			}
                    //anoigoume sarwsh me bash to value ths synthhkhs
			if((scanid = AM_OpenIndexScan(indexid,type,length,operator,value)) < 0)
			{
				AM_PrintError("Error in AM_OpenIndexScan\n");
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_Select\n");
					return -1;
				}
				if(HF_CloseFile(readid) != HFE_OK)
				{	
					HF_PrintError("error in HF_Closefile called on DM_Select\n");
					return -1;
				}
				if(AM_CloseIndex(indexid) != AME_OK)
				{
					AM_PrintError("Error in AM_CloseIndexScan\n");
					return -1;
				}
				return -1;
			}
                     //pairnoume ta id twn eggrafwn pou ikanopoioun th synthkh where
			while((record_id = AM_FindNextEntry(scanid)) != AME_EOF)
			{
                     //fernoume thn eggrafh pou antistoixei sto record_id
				if(HF_GetThisRec(readid,record_id,record_read,relwidth) != HFE_OK)
				{
					HF_PrintError("Error in HF_GetThisRec\n");
					if(HF_CloseFile(fileid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_Select\n");
						return -1;
					}
					if(HF_CloseFile(readid) != HFE_OK)
					{	
						HF_PrintError("error in HF_Closefile called on DM_Select\n");
						return -1;
					}
					if(AM_CloseIndexScan(scanid) != AME_OK)
					{
						AM_PrintError("Error in AM_CloseIndexScan\n");
						return -1;
					}
					if(AM_CloseIndex(indexid) != AME_OK)
					{
						AM_PrintError("Error in AM_CloseIndexScan\n");
						return -1;
					}
					return -1;
				}
				mikos_eggr = 0;	
				ptr = NULL;
				for(i = begin,l = 0; l < num_of_attrs ; i+= 2, l++)
				{
				//Gia kathe ena apo ta pedia pros probolh fernoume thn eggrafh apo to attrCat
					if((recid = giveattrcatrecord(record,argv[i],argv[i+1])) < 0)
					{
						if(recid == -1) printf("Error in giveattrcatrecord called on DM_Select\n");
						else if(recid == -2) printf("Attribute %s does not exist\n",argv[i+1]);
						if(HF_CloseFile(fileid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_Select\n");
							return -1;
						}
						if(HF_CloseFile(readid) != HFE_OK)
						{	
							HF_PrintError("error in HF_Closefile called on DM_Select\n");
							return -1;
						}
						if(AM_CloseIndexScan(scanid) != AME_OK)
						{
							AM_PrintError("Error in AM_CloseIndexScan\n");
							return -1;
						}
						if(AM_CloseIndex(indexid) != AME_OK)
						{
							AM_PrintError("Error in AM_CloseIndexScan\n");
							return -1;
						}
						return -1;
					}
					//pairnoume plhrofories gia to pedio
					memcpy(&type,record + 2*MAXNAME + 2*sizeof(int),sizeof(char));
					memcpy(&length,record + 2*MAXNAME + sizeof(int),sizeof(int));
					memcpy(&offset,record + 2*MAXNAME,sizeof(int));
					value_read = (char*)malloc(length);
					if(value_read == NULL)
					{
						printf("Error in malloc called on DM_select\n");
						if(HF_CloseFile(fileid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_Select\n");
							return -1;
						}
						if(HF_CloseFile(readid) != HFE_OK)
						{	
							HF_PrintError("error in HF_Closefile called on DM_Select\n");
							return -1;
						}
						if(AM_CloseIndexScan(scanid) != AME_OK)
						{
							AM_PrintError("Error in AM_CloseIndexScan\n");
							return -1;
						}
						if(AM_CloseIndex(indexid) != AME_OK)
						{
							AM_PrintError("Error in AM_CloseIndexScan\n");
							return -1;
						}
						return -1;
					}
					//eisagoume to value tou pediou sth nea eggrafh
					memcpy(value_read,record_read + offset,length);
					
					mikos_eggr += length;
					ptr = realloc(ptr,mikos_eggr*sizeof(char));
					if(ptr == NULL)
					{
						printf("Error in realloc called on DM_Select\n");
						if(HF_CloseFile(fileid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_Select\n");
							return -1;
						}
						if(HF_CloseFile(readid) != HFE_OK)
						{	
							HF_PrintError("error in HF_Closefile called on DM_Select\n");
							return -1;
						}
						if(AM_CloseIndexScan(scanid) != AME_OK)
						{
							AM_PrintError("Error in AM_CloseIndexScan\n");
							return -1;
						}
						if(AM_CloseIndex(indexid) != AME_OK)
						{
							AM_PrintError("Error in AM_CloseIndexScan\n");
							return -1;
						}
						return -1;
					}
					memcpy(ptr + mikos_eggr - length,value_read,length);
					free(value_read);
				}
				//eisagoume thn eggrafh pou kataskeyasame apo ta proballomena 
				//pedia sto arxeio
				if(HF_InsertRec(fileid,ptr,mikos_eggr) < 0)
				{
					HF_PrintError("Error in HF_InsertRec called on DM_Select");
					if(HF_CloseFile(fileid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_Select\n");
						return -1;
					}
					if(HF_CloseFile(readid) != HFE_OK)
					{	
						HF_PrintError("error in HF_Closefile called on DM_Select\n");
						return -1;
					}
					if(AM_CloseIndexScan(scanid) != AME_OK)
					{
						AM_PrintError("Error in AM_CloseIndexScan\n");
						return -1;
					}
					if(AM_CloseIndex(indexid) != AME_OK)
					{
						AM_PrintError("Error in AM_CloseIndexScan\n");
						return -1;
					}
					return -1;
				}
				free(ptr);
			}
			//kleinoume th sarwsh
			if(AM_CloseIndexScan(scanid) != AME_OK)
			{
				AM_PrintError("Error in AM_CloseIndexScan\n");
				return -1;
			}
			//Kleinoume to eyrethrio
			if(AM_CloseIndex(indexid) != AME_OK)
			{
				AM_PrintError("Error in AM_CloseIndexScan\n");
				return -1;
			}
			free(record_read);
		}
		else //to pedio den exei eyrethrio
		{
			record_read = malloc(relwidth);
			if(record_read == NULL)
			{
				printf("Error in malloc called on DM_select\n");
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_Select\n");
					return -1;
				}
				if(HF_CloseFile(readid) != HFE_OK)
				{	
					HF_PrintError("error in HF_Closefile called on DM_Select\n");
					return -1;
				}
				return -1;
			}
			memcpy(&offset,attrrecord + 2*MAXNAME,sizeof(int));
			//anoigoume HF sarwsh me bash to value
			if((scanid = HF_OpenFileScan(readid,relwidth,type,length,offset,operator,value)) <0)
			{
				HF_PrintError("Error in HF_OpenFileScan called on DM_select\n");
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_Select\n");
					return -1;
				}
				if(HF_CloseFile(readid) != HFE_OK)
				{	
					HF_PrintError("error in HF_Closefile called on DM_Select\n");
					return -1;
				}
				if(HF_CloseFileScan(scanid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFileScan called on DM_Select");
					return -1;
				}
				return -1;
			}
			//fernoume mia-mia th eggrafes pou ikanopoioun th synthhkh
			while((record_id = HF_FindNextRec(scanid,record_read)) !=HFE_EOF)
			{
				
				if(record_id < 0)
				{
					HF_PrintError("Error in HF_FindNextRec called on DM_select\n");
					if(HF_CloseFile(fileid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_Select\n");
						return -1;
					}
					if(HF_CloseFile(readid) != HFE_OK)
					{	
						HF_PrintError("error in HF_Closefile called on DM_Select\n");
						return -1;
					}
					if(HF_CloseFileScan(scanid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFileScan called on DM_Select");
						return -1;
					}
					return -1;
				}
				mikos_eggr = 0;	
				ptr = NULL;
				for(i = begin,l = 0; l < num_of_attrs ; i+= 2, l++)
				{
				//gia kathe pedio pros probolh fernoume apo to attrCat thn eggrafh 
                           //pou tou antistoixei
					if((recid = giveattrcatrecord(record,argv[i],argv[i+1])) < 0)
					{
						if(recid == -1) printf("Error in giveattrcatrecord called on DM_Select\n");
						else if(recid == -2) printf("Attribute %s does not exist\n",argv[i+1]);
						if(HF_CloseFile(fileid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_Select\n");
							return -1;
						}
						if(HF_CloseFile(readid) != HFE_OK)
						{	
							HF_PrintError("error in HF_Closefile called on DM_Select\n");
							return -1;
						}
						if(HF_CloseFileScan(scanid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFileScan called on DM_Select");
							return -1;
						}
						return -1;
					}
					//pairnoume plhrofories gia to pedio
					memcpy(&type,record + 2*MAXNAME + 2*sizeof(int),sizeof(char));
					memcpy(&length,record + 2*MAXNAME + sizeof(int),sizeof(int));
					memcpy(&offset,record + 2*MAXNAME,sizeof(int));
					value_read = (char*)malloc(length);
					if(value_read == NULL)
					{
						printf("Error in malloc called on DM_select\n");
						if(HF_CloseFile(fileid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_Select\n");
							return -1;
						}
						if(HF_CloseFile(readid) != HFE_OK)
						{	
							HF_PrintError("error in HF_Closefile called on DM_Select\n");
							return -1;
						}
						if(HF_CloseFileScan(scanid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFileScan called on DM_Select");
							return -1;
						}
						return -1;
					}
					//prosthetoume to value tou pediou sth nea eggrafh
					memcpy(value_read,record_read + offset,length);
					
					mikos_eggr += length;
					ptr = realloc(ptr,mikos_eggr*sizeof(char));
					if(ptr == NULL)
					{
						printf("Error in realloc called on DM_Select\n");
						if(HF_CloseFile(fileid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_Select\n");
							return -1;
						}
						if(HF_CloseFile(readid) != HFE_OK)
						{	
							HF_PrintError("error in HF_Closefile called on DM_Select\n");
							return -1;
						}
						if(HF_CloseFileScan(scanid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFileScan called on DM_Select");
							return -1;
						}
						return -1;
					}
					memcpy(ptr + mikos_eggr - length,value_read,length);
					free(value_read);
				}
				//eisagoume thn eggrafh sto arxeio
				if(HF_InsertRec(fileid,ptr,mikos_eggr) < 0)
				{
					HF_PrintError("Error in HF_InsertRec called on DM_Select");
					if(HF_CloseFile(fileid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_Select\n");
						return -1;
					}
					if(HF_CloseFile(readid) != HFE_OK)
					{	
						HF_PrintError("error in HF_Closefile called on DM_Select\n");
						return -1;
					}
					if(HF_CloseFileScan(scanid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFileScan called on DM_Select");
						return -1;
					}
					return -1;
				}
				free(ptr);
			}
			free(record_read);
			//kleinoume th sarwsh
			if(HF_CloseFileScan(scanid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFileScan called on DM_Select");
				return -1;
			}

		}
	}
	else//den exoume synthhkh where
	{
		record_read = malloc(relwidth);
		if(record_read == NULL)
		{
			printf("Error in malloc called on DM_select\n");
			return -1;
		}
		//fernoume mia mia tis eggrafes apo to arxeio ths sxeshs apo thn opoia 
		//proerxontai ta pedia pros probolh
		if((f_rec_id = HF_GetFirstRec(readid,record_read,relwidth)) < 0 && f_rec_id != HFE_EOF)
		{
			HF_PrintError("error in HF_GetFirstRec called on DM_insert\n");
			if(HF_CloseFile(fileid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_Select\n");
				return -1;
			}
			if(HF_CloseFile(readid) != HFE_OK)
			{	
				HF_PrintError("error in HF_Closefile called on DM_Select\n");
				return -1;
			}
			return -1;
		}		

		while(f_rec_id != HFE_EOF)
		{
			mikos_eggr = 0;
			ptr = NULL;
			for(i = begin, l = 0; l < num_of_attrs ; i+= 2, l++)
			{
                    //gia kathe pedio pros probolh fernoume apo to attrCat thn eggrafh 
                    //pou tou antistoixei
		   		if((recid = giveattrcatrecord(record,argv[i],argv[i+1])) < 0)
				{
					printf("Error in giveattrcatrecord called on DM_Select\n");
					if(HF_CloseFile(fileid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_Select\n");
						return -1;
					}
					if(HF_CloseFile(readid) != HFE_OK)
					{	
						HF_PrintError("error in HF_Closefile called on DM_Select\n");
						return -1;
					}
					return -1;
				}
				//pairnoume plhrofories gia to pedio
				memcpy(&type,record + 2*MAXNAME + 2*sizeof(int),sizeof(char));
				memcpy(&length,record + 2*MAXNAME + sizeof(int),sizeof(int));
				memcpy(&offset,record + 2*MAXNAME,sizeof(int));
				value_read = malloc(length);
				if(value_read == NULL)
				{
					printf("Error in malloc called on DM_select\n");
					if(HF_CloseFile(fileid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_Select\n");
						return -1;
					}
					if(HF_CloseFile(readid) != HFE_OK)
					{	
						HF_PrintError("error in HF_Closefile called on DM_Select\n");
						return -1;
					}
					return -1;
				}
                           //eisagoume to value tou pediou sth nea eggrafh	
				memcpy(value_read,record_read + offset,length);
				mikos_eggr += length;
				ptr = realloc(ptr,mikos_eggr*sizeof(char));
				if(ptr == NULL)
				{
					printf("Error in realloc called on DM_Select\n");
					if(HF_CloseFile(fileid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_Select\n");
						return -1;
					}
					if(HF_CloseFile(readid) != HFE_OK)
					{	
						HF_PrintError("error in HF_Closefile called on DM_Select\n");
						return -1;
					}
					return -1;
				}
				memcpy(ptr + mikos_eggr - length,value_read,length);
				free(value_read);
			}
			//eisagoume th nea eggrafh sto arxeio
			if(HF_InsertRec(fileid,ptr,mikos_eggr) < 0)
			{
				HF_PrintError("Error in HF_InsertRec called on DM_Select");
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_Select\n");
					return -1;
				}
				if(HF_CloseFile(readid) != HFE_OK)
				{	
					HF_PrintError("error in HF_Closefile called on DM_Select\n");
					return -1;
				}
				return -1;
			}
			f_rec_id = HF_GetNextRec(readid,f_rec_id,record_read,relwidth);
			free(ptr);
		}
		free(record_read);
	}
       //kleinoume to arxeio apo to opou proerxontai ta pedia
	if(HF_CloseFile(readid) != HFE_OK)
	{	
		HF_PrintError("error in HF_Closefile called on DM_Select\n");
		if(HF_CloseFile(fileid) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_Select\n");
			return -1;
		}
		return -1;
	}
	if(argc % 2 == 0)//periptwsh pou den exoume sxesh apotelesmatos
	{
		if(HF_CloseFile(fileid) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_Select\n");
			return -1;
		}
             //typwnoume ta apotelesmata ths select pou eixame apothkeysei sto prowrino arxeio
		if(print_results("temprelation",mikos_eggr,argc,argv)==-1)
		{
			printf("Error in print_results called on DM_select\n");
			return -1;
		}
		//katastrefoume to proswrino arxeio
		if(HF_DestroyFile("temprelation") != HFE_OK)
		{
			HF_PrintError("Error in HF_DestroyFile called on DM_select");
			return -1;
		}
	}
	else
	{
             //kleinoume to arxeio ths sxeshs apotelesmatos
		if(HF_CloseFile(fileid) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_Select\n");
			return -1;
		}
		
	}
	return DME_OK;
}

int DM_join(int argc,char* argv[])
{
	int attret1,attret2,indexed1,indexed2,indexno1,indexno2,recnum1,recnum2,length1,length;
	int externalid,internalid,relwidth1,relwidth2,relwidth,inrelwidth,recid1,recid2,op,offset1,offset2,offset,off,num_of_attrs;
	int begin,mikos_eggr,arg_num,fileid,filedesc,scanid,i,l,attrid;	
	char relation1[RELCATSIZE],relation2[RELCATSIZE],attribute1[ATTRCATSIZE],attribute2[ATTRCATSIZE],attribute[ATTRCATSIZE];
	char *record1,*record2,*value,*ptr,**argument,*val,*intrel;
	char type1,type2,type;
	
	//Eleghos an oi sxeseis pou emplekontai sth join einai diaforetikes
	if(strcmp(argv[argc - 5],argv[argc -2]) == 0)
	{
		printf("SelfJoin is forbidden\n");
		return -1;
	}

	//Eleghoume an oi sxeseis pou emplekontai sth synthkh ths join yparxoun
	//An nai tote fernoume kai thn eggrafh apo to relCat pou tous antistoixei
	if((recid1 = giverelcatrecord(relation1,argv[argc - 5])) < 0)
	{
		printf("recid1=%d\n",recid1);
		if(recid1 == -2)printf("Relation with name %s does not exist\n",argv[argc -5]);
		else printf("Error in giverelcatrecord\n");
		return -1;
	}
	
	if((recid2 = giverelcatrecord(relation2,argv[argc -2])) < 0)
	{
		if(recid2 == -2)printf("Relation with name %s does not exist\n",argv[argc -2]);
		else printf("Error in giverelcatrecord\n");
		return -1;
	}
       //Eleghoume an ta pedia pou emplekontai sth synthkh ths join yparxoun
	//An nai tote fernoume kai thn eggrafh apo to attrCat pou tous antistoixei
	if((attret1 = giveattrcatrecord(attribute1,argv[argc -5],argv[argc - 4])) < 0)
	{
		if(attret1 == -2) printf("Attribute with name %s does not exist\n",argv[argc - 4]);
		else printf("Error in giveattrcaterecord\n");
	}

	if((attret2 = giveattrcatrecord(attribute2,argv[argc -2],argv[argc -1])) < 0)
	{
		if(attret2 == -2) printf("Attribute with name %s does not exist\n",argv[argc - 1]);
		else printf("Error in giveattrcaterecord\n");
	}

	memcpy(&type1,attribute1 + 2*MAXNAME + 2*sizeof(int),sizeof(char));
	memcpy(&type2,attribute2 + 2*MAXNAME + 2*sizeof(int),sizeof(char));
	if(type1 != type2)
	{
		printf("The fields given cannot be joined because their type differs\n");
		return -1;
	}
	
	if(argc % 2 != 0)//Periptwsh pou den exoume sxesh apotelesmatos
	{
		num_of_attrs=(int) atoi(argv[1]);
		begin = 2;
		//kataskeyazoume ena proswrino arxeio pou ta apothhkeytoun ta apotelesmata ths Select
		if(HF_CreateFile("temprelation") != HFE_OK )
		{
			HF_PrintError("Error in HF_CreateFile called on DM_join\n");
			return -1;
		}
		//anoigoume to proswrino arxeio
		if((fileid = HF_OpenFile("temprelation")) < 0)
		{
			HF_PrintError("Error in HF_OpenFile called on DM_join\n");
			return -1;
		} 
	}
	else //periptwsh pou exoume sxesh apotelesmatos
	{
		begin = 3;
		num_of_attrs=(int) atoi(argv[2]);
		//kataskeyazoume ta orismata ths UT_Create
		if(construct_argument(argc,argv,&argument,&arg_num) != 1)
		{
			printf("Error in construct_argument called on DM_join\n");
		} 
		//kataskeyh ths neas sxeshs
		if(UT_create(arg_num,argument) != UTE_OK)
		{
			printf("Error in UT_Create called on DM_join\n");
			return -1;
		}
		free_argument(&argument,num_of_attrs);
		//anoigma arxeiou neas sxeshs
		if((fileid = HF_OpenFile(argv[1])) < 0)
		{
			HF_PrintError("Error in HF_OpenFile called on DM_join\n");
			return -1;
		} 
	}
	
	memcpy(&indexed1,attribute1 + 2*MAXNAME + 2*sizeof(int) + sizeof(char),sizeof(int));
	memcpy(&indexed2,attribute2 + 2*MAXNAME + 2*sizeof(int) + sizeof(char),sizeof(int));
	if((op = find_operator(argv[argc -3])) == -1)
	{
		printf("The operator given is not valid\n");
		return -1;
	}
	if(indexed1 == TRUE && indexed2 == TRUE)//periptwsh pou kai ta 2 padia exoun eyrethrio
	{
              //pairnoume plhrofories gia ta pedia kai tis sxeseis
		memcpy(&recnum1,relation1 + MAXNAME + 3*sizeof(int),sizeof(int));
		memcpy(&recnum2,relation2 + MAXNAME + 3*sizeof(int),sizeof(int));
		memcpy(&relwidth1,relation1 + MAXNAME,sizeof(int));
		memcpy(&relwidth2,relation2 + MAXNAME,sizeof(int));
		memcpy(&offset1,attribute1 + 2*MAXNAME,sizeof(int));
		memcpy(&offset2,attribute2 + 2*MAXNAME,sizeof(int));
		memcpy(&length1,attribute1 + 2*MAXNAME + sizeof(int),sizeof(int));
		memcpy(&indexno1,attribute1 + 2*MAXNAME + 3*sizeof(int) + sizeof(char),sizeof(int));
		memcpy(&indexno2,attribute2 + 2*MAXNAME + 3*sizeof(int) + sizeof(char),sizeof(int));
		//eleghos gia to poio apo ta 2 arxeia einai megalytero wste na doume poio tha anoixoume me HF kai poio me eyrethrio
		if(recnum1 > recnum2)
		{
			offset = offset1;
			relwidth = relwidth2;
			inrelwidth = relwidth1;
			intrel=argv[argc-5];
			//anoigma arxeiwn sxesewn kai eyrethriou
			if((externalid = HF_OpenFile(argv[argc - 2])) < 0 )
			{
				HF_PrintError("Error in HF_OpenFile\n");
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				return -1;
			}
			if((filedesc = HF_OpenFile(argv[argc - 5])) < 0 )
			{
				HF_PrintError("Error in HF_OpenFile\n");
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(externalid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				return -1;
			}
			if((internalid = AM_OpenIndex (argv[argc - 5],indexno1)) < 0)
			{
				AM_PrintError("Error in AM_OpenIndex\n");
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(externalid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(filedesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				return -1;
			}
			
			
		}
		else
		{
			offset = offset2;
			relwidth = relwidth1;
			inrelwidth = relwidth2;
			intrel=argv[argc-2];
			//anoigma arxeiwn sxesewn kai eyrethriou
			if((externalid = HF_OpenFile(argv[argc - 5])) < 0 )
			{
				HF_PrintError("Error in HF_OpenFile\n");
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				return -1;
			}
			if((filedesc = HF_OpenFile(argv[argc - 2])) < 0 )
			{
				HF_PrintError("Error in HF_OpenFile\n");
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(externalid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				return -1;
			}
			if((internalid = AM_OpenIndex (argv[argc - 2],indexno2)) < 0)
			{
				AM_PrintError("Error in AM_OpenIndex\n");
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(externalid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(filedesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				return -1;
			}
		}

		record1 = (char*)malloc(relwidth);
		if(record1 == NULL)
		{
			printf("Error in malloc called on DM_join\n");
			if(HF_CloseFile(fileid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(HF_CloseFile(externalid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(HF_CloseFile(filedesc) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(AM_CloseIndex(internalid) != AME_OK)
			{
				AM_PrintError("Error in AM_CloseIndex called on DM_join\n");
				return -1;
			}
			return -1;
		}
		record2 = (char*)malloc(inrelwidth);
		if(record2 == NULL)
		{
			printf("Error in malloc called on DM_join\n");
			if(HF_CloseFile(fileid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(HF_CloseFile(externalid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(HF_CloseFile(filedesc) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(AM_CloseIndex(internalid) != AME_OK)
			{
				AM_PrintError("Error in AM_CloseIndex called on DM_join\n");
				return -1;
			}
			return -1;
		}
		//fermoume mia mia tis eggrafes tou exwtwrikou arxeiou
		if((recid1 = HF_GetFirstRec(externalid,record1,relwidth)) < 0)
		{
			HF_PrintError("Error in HF_GetFirstRec\n");
			if(HF_CloseFile(fileid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(HF_CloseFile(externalid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(HF_CloseFile(filedesc) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(AM_CloseIndex(internalid) != AME_OK)
			{
				AM_PrintError("Error in AM_CloseIndex called on DM_join\n");
				return -1;
			}
			return -1;
		}
		while(recid1 != HFE_EOF)
		{
			value = (char *)malloc(length1);
			if(value == NULL)
			{
				printf("error in malloc called on DM_join\n");
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(externalid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(filedesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(AM_CloseIndex(internalid) != AME_OK)
				{
					AM_PrintError("Error in AM_CloseIndex called on DM_join\n");
					return -1;
				}
				return -1;
			}
			memcpy(value,record1 + offset,length1);
			//anoigoume sarwsh me bash thn timh tou pediou ths eggrafhs pou ferame apo to exwteriko arxeio
			if((scanid = AM_OpenIndexScan(internalid,type1,length1,op,value)) <0)
			{
				AM_PrintError("Error in AM_OpenIndexScan\n");
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(externalid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(filedesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(AM_CloseIndex(internalid) != AME_OK)
				{
					AM_PrintError("Error in AM_CloseIndex called on DM_join\n");
					return -1;
				}
				return -1;
			}
			//pairnoume ta id twn eggrafwn tou eswterikou arxeiou pou ikanopoioun th syn8hkh
			while((recid2 = AM_FindNextEntry(scanid)) != AME_EOF)
			{
				if(recid2 <0)
				{
					AM_PrintError("Error in AM_FindNextEntry called on DM_join\n");
					if(HF_CloseFile(fileid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_join\n");
						return -1;
					}
					if(HF_CloseFile(externalid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_join\n");
						return -1;
					}
					if(HF_CloseFile(filedesc) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_join\n");
						return -1;
					}
					if(AM_CloseIndexScan(scanid) != AME_OK)
					{
						AM_PrintError("Error in AM_CloseIndexScan called on DM_join\n");
						return -1;
					}
					if(AM_CloseIndex(internalid) != AME_OK)
					{
						AM_PrintError("Error in AM_CloseIndex called on DM_join\n");
						return -1;
					}
					return -1;
				}
				//fernoume thn eggrafh apo to eswteriko arxeio
				if(HF_GetThisRec(filedesc,recid2,record2,inrelwidth) != HFE_OK)
				{
					HF_PrintError("Error in HF_GetThisRec called on DM_join\n");
					if(HF_CloseFile(fileid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_join\n");
						return -1;
					}
					if(HF_CloseFile(externalid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_join\n");
						return -1;
					}
					if(HF_CloseFile(filedesc) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_join\n");
						return -1;
					}
					if(AM_CloseIndexScan(scanid) != AME_OK)
					{
						AM_PrintError("Error in AM_CloseIndexScan called on DM_join\n");
						return -1;
					}
					if(AM_CloseIndex(internalid) != AME_OK)
					{
						AM_PrintError("Error in AM_CloseIndex called on DM_join\n");
						return -1;
					}
					return -1;
				}
				ptr = NULL;
				mikos_eggr = 0;
				for(i = begin, l = 0; l < num_of_attrs ; i+= 2, l++)
				{
                            //gia kathe ena apo ta pedia pros probolh fernoume apo to attrCat
                            //thn eggrafh pou tous antistoxei
					if((attrid = giveattrcatrecord(attribute,argv[i],argv[i+1])) < 0)
					{
						printf("Error in giveattrcatrecord called on DM_join\n");
						if(HF_CloseFile(fileid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_join\n");
							return -1;
						}
						if(HF_CloseFile(externalid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_join\n");
							return -1;
						}
						if(HF_CloseFile(filedesc) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_join\n");
							return -1;
						}
						if(AM_CloseIndexScan(scanid) != AME_OK)
						{
							AM_PrintError("Error in AM_CloseIndexScan called on DM_join\n");
							return -1;
						}
						if(AM_CloseIndex(internalid) != AME_OK)
						{
							AM_PrintError("Error in AM_CloseIndex called on DM_join\n");
							return -1;
						}
						return -1;
					}
					//pairnoume plhrofories gai to pedio
					memcpy(&type,attribute + 2*MAXNAME + 2*sizeof(int),sizeof(char));
					memcpy(&length,attribute + 2*MAXNAME + sizeof(int),sizeof(int));
					memcpy(&off,attribute + 2*MAXNAME,sizeof(int));
					val = (char *)malloc(length);	
					if(val == NULL)
					{
						printf("Error in malloc called on DM_join\n");
						if(HF_CloseFile(fileid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_join\n");
							return -1;
						}
						if(HF_CloseFile(externalid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_join\n");
							return -1;
						}
						if(HF_CloseFile(filedesc) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_join\n");
							return -1;
						}
						if(AM_CloseIndexScan(scanid) != AME_OK)
						{
							AM_PrintError("Error in AM_CloseIndexScan called on DM_join\n");
							return -1;
						}
						if(AM_CloseIndex(internalid) != AME_OK)
						{
							AM_PrintError("Error in AM_CloseIndex called on DM_join\n");
							return -1;
						}
						return -1;
					}
					//analoga me to poia sxesh (eswterikh h exwterikh) anhkei to pedio 
					//pairnoume thn timh tou
					if(strcmp(argv[i],intrel) == 0)
						memcpy(val,record2 + off,length);
					else
						memcpy(val,record1 + off,length);
					mikos_eggr += length;
					//eisagoume to pedio sth nea eggrafh
					ptr = realloc(ptr,mikos_eggr*sizeof(char));
					if(ptr == NULL)
					{
						printf("Error in realloc called on DM_join\n");
						if(HF_CloseFile(fileid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_join\n");
							return -1;
						}
						if(HF_CloseFile(externalid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_join\n");
							return -1;
						}
						if(HF_CloseFile(filedesc) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_join\n");
							return -1;
						}
						if(AM_CloseIndexScan(scanid) != AME_OK)
						{
							AM_PrintError("Error in AM_CloseIndexScan called on DM_join\n");
							return -1;
						}
						if(AM_CloseIndex(internalid) != AME_OK)
						{
							AM_PrintError("Error in AM_CloseIndex called on DM_join\n");
							return -1;
						}
						return -1;
					}
					memcpy(ptr + mikos_eggr - length,val,length);
					free(val);
				}
				//eisagoume th nea eggrafh sto arxeio
				if(HF_InsertRec(fileid,ptr,mikos_eggr) < 0)
				{
					HF_PrintError("Error in HF_InsertRec called on DM_join");
					if(HF_CloseFile(fileid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_join\n");
						return -1;
					}
					if(HF_CloseFile(externalid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_join\n");
						return -1;
					}
					if(HF_CloseFile(filedesc) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_join\n");
						return -1;
					}
					if(AM_CloseIndexScan(scanid) != AME_OK)
					{
						AM_PrintError("Error in AM_CloseIndexScan called on DM_join\n");
						return -1;
					}
					if(AM_CloseIndex(internalid) != AME_OK)
					{
						AM_PrintError("Error in AM_CloseIndex called on DM_join\n");
						return -1;
					}
					return -1;
				}
				free(ptr);
			}
			//kleisimo sarwshs
			if(AM_CloseIndexScan(scanid) != AME_OK)
			{
				AM_PrintError("Error in AM_CloseIndexScan called on DM_join\n");
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(externalid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(filedesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(AM_CloseIndex(internalid) != AME_OK)
				{
					AM_PrintError("Error in AM_CloseIndex called on DM_join\n");
					return -1;
				}
				return -1;
			}
			recid1 = HF_GetNextRec(externalid,recid1,record1,relwidth);
			if(recid1 < 0 &&  recid1 != HFE_EOF)
			{
				HF_PrintError("Error in HF_GetNextRec called on DM_join\n");
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(externalid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(filedesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(AM_CloseIndex(internalid) != AME_OK)
				{
					AM_PrintError("Error in AM_CloseIndex called on DM_join\n");
					return -1;
				}
				return -1;
			}
			free(value);
		}
		//kleisimo arxeiwn
		if(HF_CloseFile(externalid) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_join\n");
			if(HF_CloseFile(fileid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(HF_CloseFile(filedesc) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(AM_CloseIndex(internalid) != AME_OK)
			{
				AM_PrintError("Error in AM_CloseIndex called on DM_join\n");
				return -1;
			}
			return -1;
		}
		if(HF_CloseFile(filedesc) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_join\n");
			if(HF_CloseFile(fileid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(AM_CloseIndex(internalid) != AME_OK)
			{
				AM_PrintError("Error in AM_CloseIndex called on DM_join\n");
				return -1;
			}
			return -1;
		}
		//kleisimo eyrethriou
		if(AM_CloseIndex(internalid) != AME_OK)
		{
			AM_PrintError("Error in AM_CloseIndex called on DM_join\n");
			if(HF_CloseFile(fileid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			return -1;
		}
		free(record1);
		free(record2);

	}
	if(indexed1==FALSE && indexed2==FALSE)//periptwsh pou kanena pedio den exei eyrethrio
	{
              //pairnoume plhrofories gia ta pedia kai tis sxeseis
		memcpy(&recnum1,relation1 + MAXNAME + 3*sizeof(int),sizeof(int));
		memcpy(&recnum2,relation2 + MAXNAME + 3*sizeof(int),sizeof(int));
		memcpy(&relwidth1,relation1 + MAXNAME,sizeof(int));
		memcpy(&relwidth2,relation2 + MAXNAME,sizeof(int));
		memcpy(&offset1,attribute1 + 2*MAXNAME,sizeof(int));
		memcpy(&offset2,attribute2 + 2*MAXNAME,sizeof(int));
		memcpy(&length1,attribute1 + 2*MAXNAME + sizeof(int),sizeof(int));
		//analoga me to poio arxeio einai megalytero apofasizoume poio tha einai 
		//sto exwteriko kai poio sto eswteriko loop
		if(recnum1 > recnum2)
		{
			offset = offset1;
			relwidth = relwidth2;
			inrelwidth = relwidth1;
			intrel = argv[argc-5];
			//anoigma arxeiwn sxesewn
			if((externalid = HF_OpenFile(argv[argc - 2])) < 0 )
			{
				HF_PrintError("Error in HF_OpenFile\n");
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				return -1;
			}
			if((internalid = HF_OpenFile(argv[argc - 5])) < 0 )
			{
				HF_PrintError("Error in HF_OpenFile\n");
				if(HF_CloseFile(externalid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				return -1;
			}
			
		}
		else
		{
			offset = offset2;
			relwidth = relwidth1;
			inrelwidth = relwidth2;
			intrel = argv[argc-2];
			//anoigma arxeiwn sxesewn
			if((externalid = HF_OpenFile(argv[argc - 5])) < 0 )
			{
				HF_PrintError("Error in HF_OpenFile\n");
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				return -1;
			}
			if((internalid = HF_OpenFile(argv[argc - 2])) < 0 )
			{
				HF_PrintError("Error in HF_OpenFile\n");
				if(HF_CloseFile(externalid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				return -1;
			}
			
		}
		record1 = (char*)malloc(relwidth);
		if(record1 == NULL)
		{
			printf("Error in malloc called on DM_join\n");
			if(HF_CloseFile(externalid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(HF_CloseFile(fileid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(HF_CloseFile(internalid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			return -1;
		}
		record2 = (char*)malloc(inrelwidth);
		if(record2 == NULL)
		{
			printf("Error in malloc called on DM_join\n");
			if(HF_CloseFile(externalid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(HF_CloseFile(fileid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(HF_CloseFile(internalid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			return -1;
		}
		//fernoume mia mia tis eggrafes tou exwtrikou arxeiou
		if((recid1 = HF_GetFirstRec(externalid,record1,relwidth)) < 0 && recid1!=HFE_EOF)
		{
			HF_PrintError("Error in HF_GetFirstRec\n");
			if(HF_CloseFile(externalid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(HF_CloseFile(fileid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(HF_CloseFile(internalid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			return -1;
		}
		while(recid1 != HFE_EOF)
		{
			value = (char *)malloc(length1);
			if(value == NULL)
			{
				printf("error in malloc called on DM_join\n");
				return -1;
			}
			memcpy(value,record1 + offset,length1);
			//anoigoume sarwsh HF me to value tou pediou ths eggrafhs tou exwterikou arxeiou
			if((scanid = HF_OpenFileScan(internalid,inrelwidth,type1,length1,offset,op,value)) <0)
			{
				HF_PrintError("Error in HF_OpenFileScan called on DM_join\n");
				if(HF_CloseFile(externalid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(internalid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				return -1;
			}
			//fernoume tis eggrafes tou eswterikou arxeiou pou ikanopoioun th syn8hkh ths join
			while((recid2 = HF_FindNextRec(scanid,record2)) != HFE_EOF)
			{
				if(recid2<0)
				{
					HF_PrintError("Error in HF_FindNextRec called on DM_join\n");
					if(HF_CloseFile(externalid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_join\n");
						return -1;
					}
					if(HF_CloseFile(fileid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_join\n");
						return -1;
					}
					if(HF_CloseFileScan(scanid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFileScan called on DM_join\n");
						return -1;
					}
					if(HF_CloseFile(internalid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_join\n");
						return -1;
					}
					return -1;
				}
				ptr = NULL;
				mikos_eggr = 0;
				for(i = begin, l = 0; l < num_of_attrs ; i+= 2, l++)
				{
                                   //gia kathe pedio pros probolh fernoume thn eggrafh tou attrCat pou tou antistoixei
					if((attrid = giveattrcatrecord(attribute,argv[i],argv[i+1])) < 0)
					{
						printf("Error in giveattrcatrecord called on DM_join\n");
						return -1;
					}
					memcpy(&type,attribute + 2*MAXNAME + 2*sizeof(int),sizeof(char));
					memcpy(&length,attribute + 2*MAXNAME + sizeof(int),sizeof(int));
					memcpy(&off,attribute + 2*MAXNAME,sizeof(int));
					val = (char *)malloc(length);
					if(val == NULL)
					{
						printf("Error in malloc called on DM_join\n");
						if(HF_CloseFile(externalid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_join\n");
							return -1;
						}
						if(HF_CloseFile(fileid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_join\n");
							return -1;
						}
						if(HF_CloseFileScan(scanid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFileScan called on DM_join\n");
							return -1;
						}
						if(HF_CloseFile(internalid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_join\n");
							return -1;
						}
						return -1;
					}
					//analoga me to apo poia sxesh proerxetai to pedio diavazoume to value apo 
					//thn eggrafh tou eswterikou h tou exwterikou arxeiou
					if(strcmp(argv[i],intrel) == 0)
						memcpy(val,record2 + off,length);
					else 
						memcpy(val,record1 + off,length);
					mikos_eggr += length;
					//bazoume to value tou arxeiou sthn nea eggrafh
					ptr = realloc(ptr,mikos_eggr*sizeof(char));
					if(ptr == NULL)
					{
						printf("Error in realloc called on DM_join\n");
						if(HF_CloseFile(externalid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_join\n");
							return -1;
						}
						if(HF_CloseFile(fileid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_join\n");
							return -1;
						}
						if(HF_CloseFileScan(scanid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFileScan called on DM_join\n");
							return -1;
						}
						if(HF_CloseFile(internalid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_join\n");
							return -1;
						}
						return -1;
					}
					memcpy(ptr + mikos_eggr - length,val,length);
					free(val);
				}
				//eisagoume th nea eggrafh sto arxeio
				if(HF_InsertRec(fileid,ptr,mikos_eggr) < 0)
				{
					HF_PrintError("Error in HF_InsertRec called on DM_join");
					if(HF_CloseFile(externalid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_join\n");
						return -1;
					}
					if(HF_CloseFile(fileid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_join\n");
						return -1;
					}
					if(HF_CloseFileScan(scanid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFileScan called on DM_join\n");
						return -1;
					}
					if(HF_CloseFile(internalid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_join\n");
						return -1;
					}
					return -1;
				}
				free(ptr);
			}
			//kleinoume th sarwsh
			if(HF_CloseFileScan(scanid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFileScan called on DM_join\n");
				if(HF_CloseFile(externalid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(internalid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				return -1;
			}
			recid1 = HF_GetNextRec(externalid,recid1,record1,relwidth);
			if(recid1 < 0 && recid1 != HFE_EOF)
			{
				HF_PrintError("Error in HF_GetNextRec called on DM_join\n");
				return -1;
			}
			free(value);
			
		}
		//kleinoume ta arxeia
		if(HF_CloseFile(externalid) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_join\n");
			if(HF_CloseFile(fileid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(HF_CloseFile(internalid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			return -1;
		}
		if(HF_CloseFile(internalid) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_join\n");
			if(HF_CloseFile(fileid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			return -1;
		}
		free(record1);
		free(record2);
	}
	if((indexed1 == TRUE && indexed2 == FALSE) ||(indexed1 == FALSE && indexed2 == TRUE))//periptwsh pou ena pedio exei eyrethrio
	{
              //pairnoume plhrofories gia ta arxeia kai tis sxeseis
		memcpy(&recnum1,relation1 + MAXNAME + 3*sizeof(int),sizeof(int));
		memcpy(&recnum2,relation2 + MAXNAME + 3*sizeof(int),sizeof(int));
		memcpy(&relwidth1,relation1 + MAXNAME,sizeof(int));
		memcpy(&relwidth2,relation2 + MAXNAME,sizeof(int));
		memcpy(&offset1,attribute1 + 2*MAXNAME,sizeof(int));
		memcpy(&offset2,attribute2 + 2*MAXNAME,sizeof(int));
		memcpy(&length1,attribute1 + 2*MAXNAME + sizeof(int),sizeof(int));
		memcpy(&indexno1,attribute1 + 2*MAXNAME + 3*sizeof(int) + sizeof(char),sizeof(int));
		memcpy(&indexno2,attribute2 + 2*MAXNAME + 3*sizeof(int) + sizeof(char),sizeof(int));
		//analoga me to pio apo ta pedia exei eyrethrio apofasizoume poio arxeio tha anoixei me HF 
		//kai poio me eyrethrio
		if(indexed1 == TRUE )
		{
			offset = offset1;
			relwidth = relwidth2;
			inrelwidth = relwidth1;
			intrel = argv[argc - 5];
			//anoigma arxeiwn kai eyrethrio
			if((externalid = HF_OpenFile(argv[argc - 2])) < 0 )
			{
				HF_PrintError("Error in HF_OpenFile\n");
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				return -1;
			}
			if((filedesc = HF_OpenFile(argv[argc - 5])) < 0 )
			{
				HF_PrintError("Error in HF_OpenFile\n");
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(externalid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				return -1;
			}
			if((internalid = AM_OpenIndex (argv[argc - 5],indexno1)) < 0)
			{
				AM_PrintError("Error in AM_OpenIndex\n");
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(externalid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(filedesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				return -1;
			}
		}
		else if(indexed2 == TRUE)
		{
			offset = offset2;
			relwidth = relwidth1;
			inrelwidth = relwidth2;
			intrel = argv[argc - 2];
			//anoigma arxeiwn kai eyrethrio
			if((externalid = HF_OpenFile(argv[argc - 5])) < 0 )
			{
				HF_PrintError("Error in HF_OpenFile\n");
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				return -1;
			}
			if((filedesc = HF_OpenFile(argv[argc - 2])) < 0 )
			{
				HF_PrintError("Error in HF_OpenFile\n");
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(externalid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				return -1;
			}
			if((internalid = AM_OpenIndex (argv[argc - 2],indexno2)) < 0)
			{
				AM_PrintError("Error in AM_OpenIndex\n");
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(externalid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(filedesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				return -1;
			}
		}
		
		record1 = (char*)malloc(relwidth);
		if(record1 == NULL)
		{
			printf("Error in malloc called on DM_join\n");
			if(HF_CloseFile(fileid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(HF_CloseFile(externalid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(HF_CloseFile(filedesc) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(AM_CloseIndex(internalid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			return -1;
		}
		record2 = (char*)malloc(inrelwidth);
		if(record2 == NULL)
		{
			printf("Error in malloc called on DM_join\n");
			if(HF_CloseFile(fileid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(HF_CloseFile(externalid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(HF_CloseFile(filedesc) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(AM_CloseIndex(internalid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			return -1;
		}	
              //fernoume mia mia tis eggrafes tou ekswterikou arxeiou
		if((recid1 = HF_GetFirstRec(externalid,record1,relwidth)) < 0)
		{
			HF_PrintError("Error in HF_GetFirstRec\n");
			if(HF_CloseFile(fileid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(HF_CloseFile(externalid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(HF_CloseFile(filedesc) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(AM_CloseIndex(internalid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			return -1;
		}
		while(recid1 != HFE_EOF)
		{
			value = (char *)malloc(length1);
			if(value == NULL)
			{
				printf("error in malloc called on DM_join\n");
				return -1;
			}
			memcpy(value,record1 + offset,length1);
			//anoigoume sarwsh AM me bash to value tou pediou ths eggrafhs tou exwterikou arxeiou
			if((scanid = AM_OpenIndexScan(internalid,type1,length1,op,value)) <0)
			{
				AM_PrintError("Error in AM_OpenIndexScan\n");
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(externalid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(filedesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(AM_CloseIndex(internalid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				return -1;
			}
			//pairnoume ta id twn eggrafwn tou eswterikou arxeiou pou ikanopoioun th syn8hkh
			while((recid2 = AM_FindNextEntry(scanid)) != AME_EOF)
			{
				ptr = NULL;
				mikos_eggr = 0;
				if(recid2 <0)
				{
					AM_PrintError("Error in AM_FindNextEntry called on DM_join\n");
					if(HF_CloseFile(fileid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_join\n");
						return -1;
					}
					if(HF_CloseFile(externalid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_join\n");
						return -1;
					}
					if(HF_CloseFile(filedesc) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_join\n");
						return -1;
					}
					if(AM_CloseIndexScan(scanid) != AME_OK)
					{
						AM_PrintError("Error in AM_CloseIndexScan called on DM_join\n");
						return -1;
					}
					if(AM_CloseIndex(internalid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_join\n");
						return -1;
					}
					return -1;
				}
				//fernoume thn eggrafh tou eswterikou arxeiou 
				if(HF_GetThisRec(filedesc,recid2,record2,inrelwidth) != HFE_OK)
				{
					HF_PrintError("Error in HF_GetThisRec called on DM_join\n");
					if(HF_CloseFile(fileid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_join\n");
						return -1;
					}
					if(HF_CloseFile(externalid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_join\n");
						return -1;
					}
					if(HF_CloseFile(filedesc) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_join\n");
						return -1;
					}
					if(AM_CloseIndexScan(scanid) != AME_OK)
					{
						AM_PrintError("Error in AM_CloseIndexScan called on DM_join\n");
						return -1;
					}
					if(AM_CloseIndex(internalid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_join\n");
						return -1;
					}
					return -1;
				}
				for(i = begin, l = 0; l < num_of_attrs ; i+= 2, l++)
				{
                                   //gia kathe pedio pros probaolh fernoume apo to attrCat thn eggrafh pou tou antistoixei
					if((attrid = giveattrcatrecord(attribute,argv[i],argv[i+1])) < 0)
					{
						printf("Error in giveattrcatrecord called on DM_join\n");
						if(HF_CloseFile(fileid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_join\n");
							return -1;
						}
						if(HF_CloseFile(externalid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_join\n");
							return -1;
						}
						if(HF_CloseFile(filedesc) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_join\n");
							return -1;
						}
						if(AM_CloseIndexScan(scanid) != AME_OK)
						{
							AM_PrintError("Error in AM_CloseIndexScan called on DM_join\n");
							return -1;
						}
						if(AM_CloseIndex(internalid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_join\n");
							return -1;
						}
						return -1;
					}
					//pairnoume plhrofories gai to pedio
					memcpy(&type,attribute + 2*MAXNAME + 2*sizeof(int),sizeof(char));
					memcpy(&length,attribute + 2*MAXNAME + sizeof(int),sizeof(int));
					memcpy(&off,attribute + 2*MAXNAME,sizeof(int));
					val = (char *)malloc(length);
					if(val == NULL)
					{
						printf("Error in malloc called on DM_join\n");
						if(HF_CloseFile(fileid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_join\n");
							return -1;
						}
						if(HF_CloseFile(externalid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_join\n");
							return -1;
						}
						if(HF_CloseFile(filedesc) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_join\n");
							return -1;
						}
						if(AM_CloseIndexScan(scanid) != AME_OK)
						{
							AM_PrintError("Error in AM_CloseIndexScan called on DM_join\n");
							return -1;
						}
						if(AM_CloseIndex(internalid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_join\n");
							return -1;
						}
						return -1;
					}
					//anolaga me to se poia sxesh anhkei to pedio pairnoume thn 
					//timh tou apo thn eggrafh toy eswterikou h tou exwterikou arxeiou
					if(strcmp(argv[i],intrel) == 0)
						memcpy(val,record2 + off,length);
					else 
						memcpy(val,record1 + off,length);
					mikos_eggr += length;
					//eisagoume thn timh tou sth nea eggrafh
					ptr = realloc(ptr,mikos_eggr*sizeof(char));
					if(ptr == NULL)
					{
						printf("Error in realloc called on DM_join\n");
						if(HF_CloseFile(fileid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_join\n");
							return -1;
						}
						if(HF_CloseFile(externalid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_join\n");
							return -1;
						}
						if(HF_CloseFile(filedesc) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_join\n");
							return -1;
						}
						if(AM_CloseIndexScan(scanid) != AME_OK)
						{
							AM_PrintError("Error in AM_CloseIndexScan called on DM_join\n");
							return -1;
						}
						if(AM_CloseIndex(internalid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_join\n");
							return -1;
						}
						return -1;
					}
					memcpy(ptr + mikos_eggr - length,val,length);
					free(val);
				}
				//eisagoume th nea eggrafh sto arxeio
				if(HF_InsertRec(fileid,ptr,mikos_eggr) < 0)
				{
					HF_PrintError("Error in HF_InsertRec called on DM_join");
					if(HF_CloseFile(fileid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_join\n");
						return -1;
					}
					if(HF_CloseFile(externalid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_join\n");
						return -1;
					}
					if(HF_CloseFile(filedesc) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_join\n");
						return -1;
					}
					if(AM_CloseIndexScan(scanid) != AME_OK)
					{
						AM_PrintError("Error in AM_CloseIndexScan called on DM_join\n");
						return -1;
					}
					if(AM_CloseIndex(internalid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_join\n");
						return -1;
					}
					return -1;
				}
				free(ptr);
			}
			//kleinoume sarwsh
			if(AM_CloseIndexScan(scanid) != AME_OK)
			{
				AM_PrintError("Error in AM_CloseIndexScan called on DM_join\n");
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(externalid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(filedesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(AM_CloseIndex(internalid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_joine\n");
					return -1;
				}
				return -1;
			}
			recid1 = HF_GetNextRec(externalid,recid1,record1,relwidth);
			if(recid1 < 0 && recid1 != HFE_EOF)
			{
				HF_PrintError("Error in HF_GetNextRec called on DM_join\n");
				if(HF_CloseFile(fileid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(externalid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(HF_CloseFile(filedesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				if(AM_CloseIndex(internalid) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_join\n");
					return -1;
				}
				return -1;
			}
			free(value);
		}
		//kleisimo arxeiwn
		if(HF_CloseFile(externalid) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_join\n");
			if(HF_CloseFile(fileid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(HF_CloseFile(filedesc) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(AM_CloseIndex(internalid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			return -1;
		}
		if(HF_CloseFile(filedesc) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_join\n");
			if(HF_CloseFile(fileid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			if(AM_CloseIndex(internalid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			return -1;
		}
		//kleisimo eyrethriou
		if(AM_CloseIndex(internalid) != AME_OK)
		{
			AM_PrintError("Error in AM_CloseIndex called on DM_join\n");
			if(HF_CloseFile(fileid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_join\n");
				return -1;
			}
			return -1;
		}

	}

	if(argc % 2 != 0)//periptwsh pou den exoume sxesh apotelesmatos
	{
		//kleisimo proswrinou arxeiou
		if(HF_CloseFile(fileid) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_join\n");
			return -1;
		}
		//ektypwsh apotelesmatwn join pou eixame apothhkeysei sto proswrino arxeio
		if(print_results("temprelation",mikos_eggr,argc,argv)==-1)
		{
			printf("Error in print_results called on DM_join\n");
			return -1;
		}
		//katastrofh proswrinou arxeiou
		if(HF_DestroyFile("temprelation") != HFE_OK)
		{
			HF_PrintError("Error in HF_DestroyFile called on DM_join");
		}
	}
	else
	{
              //kleisimo arxeiou apotelesmatos
		if(HF_CloseFile(fileid) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_join\n");
			return -1;
		}
	}
	
	return DME_OK;	
}

int DM_delete(int argc, char* argv[])
{	int rel_id,filedesc,relwidth,record_id,next_record_id,op,attr_id,indexno,index,offset,length,rec_num,count = 0;
	int indexcnt,scandesc,indexed,help;
	char relation[RELCATSIZE],*record,type,field[ATTRCATSIZE],*value;
	float helpf;
	
	if(argc !=2 && argc != 5)
	{
		printf("DM_delete Wrong number of arguments given\n");
		return -1;
	}
	//fernoume thn eggrafh tou relCat pou antistoixei sth sxesh me onoma argv[1]
	if((rel_id = giverelcatrecord(relation,argv[1])) <0)
	{	
		if(rel_id == -1) 	printf("Error in giverelcatrecord called on DM_Delete \n");
		else if(rel_id == -2)	printf("Relation %s does not exist\n",argv[1]);
		return -1;			
	}
	//pairnoume plhrofories gia th sxesh
	memcpy(&indexcnt,relation + MAXNAME + 2*sizeof(int),sizeof(int));
	memcpy(&relwidth,relation + MAXNAME,sizeof(int));
	memcpy(&rec_num,relation + MAXNAME + 3*sizeof(int),sizeof(int));

	if(argv[2] == NULL) //Periptwsh pou den exoume synthhkh
	{
              //anoigoume to arxeio ths sxeshs ths opoias tha diagrapsoume tis eggrafes
		if((filedesc = HF_OpenFile(argv[1])) < 0)
		{
			HF_PrintError("Error in HF_OpenFile called on DM_Delete\n");
			return -1;
		}
		record = (char *)malloc(relwidth);
		if(record == NULL)
		{
			printf("Error in malloc called on DM_delete\n");
			if(HF_CloseFile(filedesc) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_delete\n");
				return -1;
			}
			return -1;	
		}
		//pairnoume mia mia tis eggrafes ths sxeshs
		if((record_id = HF_GetFirstRec(filedesc,record,relwidth)) < 0 && record_id != HFE_EOF)
		{
			HF_PrintError("Error in HF_GetFirstRec called on DM_delete\n");
			if(HF_CloseFile(filedesc) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_delete\n");
				return -1;
			}
			return -1;
		}
		while(record_id != HFE_EOF)
		{
			//diagrafoume thn eggrafh apo ta eyrethria an yparxoun
			if(indexcnt != -1){
				if(delete_record_from_indices(record_id,record,argv[1]) != DME_OK)
				{
					printf("Error in delete_record_from_indices called on DM_Delete\n");
					if(HF_CloseFile(filedesc) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_delete\n");
						return -1;
					}
					return -1;
				}
			}
			//fernoume thn epomenh eggrafh me bash to id ths progoumenhs
			next_record_id = HF_GetNextRec(filedesc,record_id,record,relwidth); 
			if(next_record_id < 0 && next_record_id !=HFE_EOF)
			{
				HF_PrintError("Error in HF_GetNextRec called on DM_delete\n");
				if(HF_CloseFile(filedesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_delete\n");
					return -1;
				}
				return -1;
			}
			//diagrafh eggrafhs apo to arxeio 
			if(HF_DeleteRec(filedesc,record_id,relwidth) != HFE_OK)
			{
				HF_PrintError("Error in HF_DeleteRec called on DM_delete\n");
				if(HF_CloseFile(filedesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_delete\n");
					return -1;
				}
				return -1;
			}
			rec_num--;
			record_id = next_record_id;
			count++;
		}
		free(record);
		
		//enhmerwsh tou relation  me to neo plithos twn eggrafwn   
		memcpy(relation + MAXNAME + 3*sizeof(int),&rec_num,sizeof(int));
		if(update_relations(rel_id,relation) != UTE_OK)
		{
			printf("Error in update_relations called on DM_delete\n");
			if(HF_CloseFile(filedesc) != HFE_OK)
			{		
				HF_PrintError("Error in HF_CloseFile called on DM_delete\n");
				return -1;
			}
			return -1;
		}
		//kleisimo arxeiou
		if(HF_CloseFile(filedesc) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_delete\n");
			return -1;
		}
	}
	else  //Periptwsh pou exoume synthhkh
	{
		//fernoume thn egrafh tou attribute apo ton pinaka attrCat
		if((attr_id=giveattrcatrecord(field,argv[1],argv[2])) <0)
		{
			if(attr_id == -1) 	printf("Error in giveattrrecord called on DM_delete\n");
			else if(attr_id == -2) 	printf("Attribute %s does not exist\n",argv[2]);
			return -1;
		}
		if((op = find_operator(argv[3])) == -1)
		{
			printf("The operator given is not valid\n");
			return -1;
		}
		//pairnoume plhrofories gai to pedio
		memcpy(&offset,field + 2*MAXNAME,sizeof(int));
		memcpy(&length,field + 2*MAXNAME +sizeof(int),sizeof(int));
		memcpy(&type,field + 2*MAXNAME + 2*sizeof(int),sizeof(char));
		if(type == 'i')
		{
			if(isNumber(argv[4]) != 1)
			{
				printf("The value given is not an integer as it should be according to its type\n");
				return -1;
			}
			help=atoi(argv[argc-1]);
			value=(char *)&help;
		}
		if(type == 'f')
		{
			if(isNumber(argv[4]) != 1)
			{
				printf("The value given is not a float as it should be according to its type\n");
				return -1;
			}
			helpf=atof(argv[argc-1]);
			value=(char *)&helpf;
		}
		if(type == 'c') value=argv[argc-1];

		memcpy(&indexed,field + 2*MAXNAME + 2*sizeof(int) + sizeof(char),sizeof(int));
		if(indexed == TRUE)//periptwsh pou exoume eyrethrio sto pedio
		{
			memcpy(&indexno,field + 2*MAXNAME + 3*sizeof(int) + sizeof(char),sizeof(int));
			//anoigma arxeiou
			if((filedesc = HF_OpenFile(argv[1])) < 0)
			{
				HF_PrintError("Error in HF_OpenFile called on DM_Delete\n");
				return -1;
			}
			//anoigma arxeiou eyrethriou
			if((index = AM_OpenIndex(argv[1],indexno)) <0)
			{
				AM_PrintError("Error in AM_OpenIndex called on DM_delete\n");
				if(HF_CloseFile(filedesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_delete\n");
					return -1;
				}
				return -1;
			}
			//anoigma sarwshs
			if((scandesc = AM_OpenIndexScan(index,type,length,op,value)) <0)
			{
				AM_PrintError("Error in AM_OpenIndexScan called on DM_delete\n");
				if(HF_CloseFile(filedesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_delete\n");
					return -1;
				}
				if(AM_CloseIndex(index) != AME_OK)
				{
					AM_PrintError("Error in AM_CloseAndexScan called on DM_delete\n");
					return -1;
				}
				return -1;
			}
			record = (char *)malloc(relwidth);
			if(record == NULL)
			{
				printf("Error in malloc called on DM_delete\n");
				if(HF_CloseFile(filedesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_delete\n");
					return -1;
				}
				if(AM_CloseIndexScan(scandesc) != AME_OK)
				{
					AM_PrintError("Error in AM_CloseAndexScan called on DM_delete\n");
					return -1;
				}
				if(AM_CloseIndex(index) != AME_OK)
				{
					AM_PrintError("Error in AM_CloseAndexScan called on DM_delete\n");
					return -1;
				}
				return -1;	
			}
			//pairnoume ta id twn eggrafwn pou ikanopoioun  th synthkh
			while((record_id = AM_FindNextEntry(scandesc)) != AME_EOF)
			{	
				if(record_id < 0 )
				{
					AM_PrintError("Error in AM_FindNextEntry called on DM_delete\n");
					if(HF_CloseFile(filedesc) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_delete\n");
						return -1;
					}
					if(AM_CloseIndexScan(scandesc) != AME_OK)
					{
						AM_PrintError("Error in AM_CloseAndexScan called on DM_delete\n");
						return -1;
					}
					if(AM_CloseIndex(index) != AME_OK)
					{
						AM_PrintError("Error in AM_CloseAndexScan called on DM_delete\n");
						return -1;
					}
					return -1;
				}
				//fernoume thn eggrafh
				if(HF_GetThisRec(filedesc,record_id,record,relwidth) != HFE_OK)
				{
					HF_PrintError("Error in HF_GetThisRec called on DM_delete\n");
					if(HF_CloseFile(filedesc) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_delete\n");
						return -1;
					}
					if(AM_CloseIndexScan(scandesc) != AME_OK)
					{
						AM_PrintError("Error in AM_CloseAndexScan called on DM_delete\n");
						return -1;
					}
					if(AM_CloseIndex(index) != AME_OK)
					{
						AM_PrintError("Error in AM_CloseAndexScan called on DM_delete\n");
						return -1;
					}
					return -1;
				}
				//diagrafh eggrafhs apo ta eyrethria an yparxoun
				if(indexcnt != -1){
					if(delete_record_from_indices(record_id,record,argv[1]) != DME_OK)
					{
						printf("Error in delete_record_from_indices called on DM_Delete\n");
						if(HF_CloseFile(filedesc) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_delete\n");
							return -1;
						}
						if(AM_CloseIndexScan(scandesc) != AME_OK)
						{
							AM_PrintError("Error in AM_CloseAndexScan called on DM_delete\n");
							return -1;
						}
						if(AM_CloseIndex(index) != AME_OK)
						{
							AM_PrintError("Error in AM_CloseAndexScan called on DM_delete\n");
							return -1;
						}
						return -1;
					}
				}
				//diagrafh eggrafhs apo to arxeio 
				if(HF_DeleteRec(filedesc,record_id,relwidth) != HFE_OK)
				{
					HF_PrintError("Error in HF_DeleteRec called on DM_delete\n");
					if(HF_CloseFile(filedesc) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_delete\n");
						return -1;
					}
					if(AM_CloseIndexScan(scandesc) != AME_OK)
					{
						AM_PrintError("Error in AM_CloseAndexScan called on DM_delete\n");
						return -1;
					}
					if(AM_CloseIndex(index) != AME_OK)
					{
						AM_PrintError("Error in AM_CloseAndexScan called on DM_delete\n");
						return -1;
					}
					return -1;
				}
				rec_num--;
				count++;
			}
			free(record);
			//enhmerwsh ths eggrafhs ths sxeshs , me to neo plithis eggrafwn,sto relCat
			memcpy(relation + MAXNAME +3*sizeof(int),&rec_num,sizeof(int));
			if(update_relations(rel_id,relation) != UTE_OK)
			{
				printf("Error in update_relations called on DM_delete\n");
				if(HF_CloseFile(filedesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_delete\n");
					return -1;
				}
				if(AM_CloseIndexScan(scandesc) != AME_OK)
				{
					AM_PrintError("Error in AM_CloseAndexScan called on DM_delete\n");
					return -1;
				}
				if(AM_CloseIndex(index) != AME_OK)
				{
					AM_PrintError("Error in AM_CloseAndexScan called on DM_delete\n");
					return -1;
				}
				return -1;
			}
			//kleisimo sarwshs
			if(AM_CloseIndexScan(scandesc) != AME_OK)
			{
				AM_PrintError("Error in AM_CloseAndexScan called on DM_delete\n");
				if(HF_CloseFile(filedesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_delete\n");
					return -1;
				}
				if(AM_CloseIndex(index) != AME_OK)
				{
					AM_PrintError("Error in AM_CloseAndexScan called on DM_delete\n");
					return -1;
				}
				return -1;
			}
			//kleisimo eyrehriou
			if(AM_CloseIndex(index) != AME_OK)
			{
				AM_PrintError("Error in AM_CloseIndex called on DM_delete\n");
				if(HF_CloseFile(filedesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_delete\n");
					return -1;
				}
				return -1;
			}
			//Kleisimo arxeiou
			if(HF_CloseFile(filedesc) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_delete\n");
				return -1;
			}
		}
		else //periptwsh pou to pedio den exei eyrethrio
		{
			//anoigma arxeiou
			if((filedesc = HF_OpenFile(argv[1])) < 0)
			{
				HF_PrintError("Error in HF_OpenFile called on DM_Delete\n");
				return -1;
			}
			//anoigoume sarwsh sto arxeio 
			if(type == 'i') 
			{
				help=atoi(argv[argc-1]);
				value=(char *)&help;
			}
			if(type == 'f') 
			{
				helpf=atof(argv[argc-1]);
				value=(char *)&helpf;
			}
			if(type == 'c') value=argv[argc-1];

			if((scandesc = HF_OpenFileScan(filedesc,relwidth,type,length,offset,op,value)) <0)
			{	
				HF_PrintError("Error in HF_OpenFileScan called on DM_delete\n");
				if(HF_CloseFile(filedesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_delete\n");
					return -1;
				}
				return -1;
			}
			record = (char *)malloc(relwidth);
			if(record == NULL)
			{
				printf("Error in malloc called on DM_delete\n");
				if(HF_CloseFileScan(scandesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFileScan called on DM_delete\n");
					return -1; 
				}
				if(HF_CloseFile(filedesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_delete\n");
					return -1;
				}
				return -1;	
			}
			while((record_id = HF_FindNextRec(scandesc,record)) != HFE_EOF)
			{
				if(record_id < 0)
				{
					HF_PrintError("Error in HF_FindNExtRec called on DM_delete\n");
					if(HF_CloseFileScan(scandesc) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFileScan called on DM_delete\n");
						return -1; 
					}
					if(HF_CloseFile(filedesc) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_delete\n");
						return -1;
					}
					return -1;
				}
				//diagrafh eggrafhs apo ta eyrethria an yparxoun
				if(indexcnt != -1){
					if(delete_record_from_indices(record_id,record,argv[1]) != DME_OK)
					{
						printf("Error in delete_record_from_indices called on DM_Delete\n");
						if(HF_CloseFileScan(scandesc) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFileScan called on DM_delete\n");
							return -1; 
						}
						if(HF_CloseFile(filedesc) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_delete\n");
							return -1;
						}
						return -1;
					}
				}
				//diagrafh eggrafhs apo to arxeio 
				if(HF_DeleteRec(filedesc,record_id,relwidth) != HFE_OK)
				{
					HF_PrintError("Error in HF_DeleteRec called on DM_delete\n");
					if(HF_CloseFileScan(scandesc) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFileScan called on DM_delete\n");
						return -1; 
					}
					if(HF_CloseFile(filedesc) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_delete\n");
						return -1;
					}
					return -1;
				}
				rec_num--;
				count++;
			}
			free(record);
			//enhmerwsh ths eggrafhs ths sxeshs , me to neo plithis eggrafwn,sto relCat
			memcpy(relation + MAXNAME +3*sizeof(int),&rec_num,sizeof(int));
			if(update_relations(rel_id,relation) != UTE_OK)
			{
				printf("Error in update_relations called on DM_delete\n");
				if(HF_CloseFileScan(scandesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFileScan called on DM_delete\n");
					return -1; 
				}
				if(HF_CloseFile(filedesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_delete\n");
					return -1;
				}
				return -1;
			}
			//kleinoume th sarwsh
			if(HF_CloseFileScan(scandesc) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFileScan called on DM_delete\n");
				if(HF_CloseFile(filedesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_delete\n");
					return -1;
				}
				return -1; 
			}
			//Kleinoume to arxeio
			if(HF_CloseFile(filedesc) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_delete\n");
				return -1;
			}
		}
	}
	return DME_OK; 	
}

int DM_subtract(int argc,char *argv[])
{
	int filedesc1,id,filedesc2,rel_id1,rel_id2,scanid,pr_recid,scandesc,length,offset,indexcnt,scan,nextrec,indexno;
	int attrcnt1,attrcnt2,relwidth1,relwidth2,rec_num,indexed,index,comp;
	char *record1,*record,record2[ATTRCATSIZE],type,*value,relation1[RELCATSIZE],relation2[RELCATSIZE];

	if(argc != 3)
	{
		printf("Wrong number of arguments given\n");
		return -1;
	}
       //eleghos an oi dyo sxeseis einai idies
	if(!strcmp(argv[1],argv[2]))
	{
		printf("Relation 1 and relation 2 should not be the same\n");
		return -1;
	}
       //eleghos an oi sxeseis yparxoun kai an nai fernoume tis eggrfes pou tous antistoixoun apo to relCat
	if((rel_id1 = giverelcatrecord(relation1,argv[1])) < 0)
	{
		if(rel_id1 == -1)printf("Error in giverelcatrecord called on DM_subtract\n");
		else if(rel_id1 == -2) printf("Relation %s does not exist\n",argv[1]);
		return -1;			
	}

	if((rel_id2 = giverelcatrecord(relation2,argv[2])) <0)
	{
		if(rel_id2 == -1)printf("Error in giverelcatrecord called on DM_subtract \n");
		else if(rel_id2 == -2) printf("Relation %s does not exist\n",argv[2]);
		return -1;			
	}

	memcpy(&attrcnt1,relation1 + MAXNAME + sizeof(int),sizeof(int));
	memcpy(&attrcnt2,relation2 + MAXNAME + sizeof(int),sizeof(int));
	//eleghos an exoun to idio plithos pediwn
	if( attrcnt1 != attrcnt2)
	{
		printf("The relations given have different number of attributes\n");
		return -1;
	} 

	memcpy(&relwidth1,relation1 + MAXNAME,sizeof(int));
	memcpy(&relwidth2,relation2 + MAXNAME,sizeof(int));
	memcpy(&indexcnt,relation1 + MAXNAME + 2* sizeof(int),sizeof(int));
	memcpy(&rec_num,relation1 + MAXNAME + 3* sizeof(int),sizeof(int));
	//eleghos an exoun idio mikos
	if(relwidth1 != relwidth2)
	{
		printf("The relations given have different width for their records\n");
		return -1;
	}
 
	if(check_attributes(argv[1],argv[2],attrcnt1) != DME_OK)
	{
		return -1;
	}

	record = (char*)malloc(relwidth1);
	if(record == NULL)
	{
		printf("Error in malloc called on DM_subtract\n");
		return -1;
	}

	record1 = (char*)malloc(relwidth2);
	if(record1 == NULL)
	{
		printf("Error in malloc called on DM_subtract\n");
		return -1;
	}
       //anoigma arxeiwn 2 sxesewn
	if((filedesc1 = HF_OpenFile(argv[1])) < 0)
	{
		HF_PrintError("Error in HF_OpenFile called on DM_subtract\n");
		return -1;
	}

	if((filedesc2 = HF_OpenFile(argv[2])) < 0)
	{
		HF_PrintError("Error in HF_OpenFile called on DM_subtract\n");
		if(HF_CloseFile(filedesc1) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
			return -1;
		}
		return -1;
	}
       //fernoume mia mia tis eggrafes tou 2 arxeiou
	if((pr_recid = HF_GetFirstRec(filedesc2,record,relwidth1)) < 0 && pr_recid!=HFE_EOF)
	{
		HF_PrintError("Error in HF_GetFirstRec called on DM_Subtract\n");
		if(HF_CloseFile(filedesc1) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
			return -1;
		}
		if(HF_CloseFile(filedesc2) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
			return -1;
		}		
		return -1;
	}
	while(pr_recid != HFE_EOF)
	{
		if(indexcnt ==-1)//periptwsh pou to arxeiou apo opou diagrafoume den exei eyrethria
		{
                     //fernoume thn eggrafh apo to attrCat enos apo ta pedia tou arxeiou apo opou diagrafoume
			if((scandesc = HF_OpenFileScan(attrCatId,ATTRCATSIZE,'c',MAXNAME,0,EQUAL,argv[1])) < 0)
			{
				HF_PrintError("Error in HF_OpenIndexScan called on DM_subtract\n");
				if(HF_CloseFile(filedesc1) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
					return -1;
				}
				if(HF_CloseFile(filedesc2) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
					return -1;
				}
				return -1;	
			}
			if((nextrec = HF_FindNextRec(scandesc,record2)) != HFE_EOF && nextrec<0)
			{
				HF_PrintError("Error in HF_FindNextRec called on DM_Subtract\n");
				if(HF_CloseFile(filedesc1) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
					return -1;
				}
				if(HF_CloseFile(filedesc2) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
					return -1;
				}
				if(HF_CloseFileScan(scandesc) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseIndexScan called on DM_subtract\n");
					return -1;	
				}
				return -1;
			}
			if(HF_CloseFileScan(scandesc) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseIndexScan called on DM_subtract\n");
				if(HF_CloseFile(filedesc1) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_subtract\n");
					return -1;
				}
				if(HF_CloseFile(filedesc2) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
					return -1;
				}
				return -1;	
			}
			//pairnoume plhrofories gia ayto to pedio
			memcpy(&offset,record2 + 2*MAXNAME,sizeof(int));
			memcpy(&length,record2 + 2*MAXNAME + sizeof(int),sizeof(int));
			memcpy(&type,record2 + 2*MAXNAME + 2*sizeof(int),sizeof(char));
			value = malloc(length);
			if(value == NULL)
			{
				printf("Error in malloc called on DM_subtract\n");
				if(HF_CloseFile(filedesc1) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_subtract\n");
					return -1;
				}
				if(HF_CloseFile(filedesc2) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
					return -1;
				}
				return -1;
			}		
			memcpy(value,record + offset,length);
			//anoigoume sarwsh HF sto arxeio apo opou diagrafoume me bash to value tou parapanw pediou
			//gia thn eggrafh tou arxeiou 2
			if((scanid = HF_OpenFileScan(filedesc1,relwidth1,type,length,offset,EQUAL,value)) < 0 )
			{
				HF_PrintError("Error in HF_OpenIndexScan\n");
				if(HF_CloseFile(filedesc1) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
					return -1;
				}
				if(HF_CloseFile(filedesc2) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
					return -1;
				}
				return -1;
			}
		
			while((id = HF_FindNextRec(scanid,record1)) != HFE_EOF)
			{
				if(id <0)
				{
					if(HF_CloseFile(filedesc2) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
						return -1;
					}
					if(HF_CloseFileScan(scanid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseIndexScan called on DM_subtract\n");
						return -1;	
					}
					if(HF_CloseFile(filedesc1) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
						return -1;
					}
					HF_PrintError("Error in HF_FindNextRec called on DM_Subtract\n");
					return -1;
				}
				//sygkrinoume tis eggrafes
				comp = compare_records(record1,record,argv[1]);
				if(comp == OK)//an einai idies
				{
                                   //diagrafoume thn eggrafh apo to arxeio                        
					if(HF_DeleteRec(filedesc1,id,relwidth1) != HFE_OK)
					{
						HF_PrintError("Error in HF_DeleteRec called in DM_subtract\n");
						if(HF_CloseFile(filedesc2) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_subtract\n");
							return -1;
						}
						if(HF_CloseFileScan(scanid) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseIndexScan called on DM_subtract\n");
							return -1;	
						}
						if(HF_CloseFile(filedesc1) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
							return -1;
						}
						return -1;
					}
					//meiwnoume to plithos twn eggrafwn
					rec_num--;
				}
				else if(comp == -1)
				{
					printf("Error in HF_DeletRec called on DM_subtract\n");
					if(HF_CloseFile(filedesc2) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
						return -1;
					}
					if(HF_CloseFileScan(scanid) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseIndexScan called on DM_subtract\n");
						return -1;	
					}
					if(HF_CloseFile(filedesc1) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
						return -1;
					}
					return -1;
				}
			}
			free(value);
			//kleinoume sarwsh
			if(HF_CloseFileScan(scanid) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseIndexScan called on DM_subtract\n");
				if(HF_CloseFile(filedesc1) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
					return -1;
				}
				if(HF_CloseFile(filedesc2) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
					return -1;
				}
				return -1;	
			}
		}
		else//periptwsh pou to arxeio apo opou diagrafoume exei ayrethrio
		{
                     //anoigoume sarwsh sto attrCat arxeio gia na feroume tis eggrafes twn pediwn ths sxeshs
			if((scandesc = HF_OpenFileScan(attrCatId,ATTRCATSIZE,'c',MAXNAME,0,EQUAL,argv[1])) < 0)
			{
				HF_PrintError("Error in HF_OpenIndexScan called on DM_subtract\n");
				if(HF_CloseFile(filedesc1) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
					return -1;
				}
				if(HF_CloseFile(filedesc2) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
					return -1;
				}
				return -1;	
			}
			//gia kathe eggrafh pediou pou fernoume koitame an ayto einai eyrethriasmeno
			while((nextrec = HF_FindNextRec(scandesc,record2)) != HFE_EOF)
			{
				if(nextrec<0)
				{
					HF_PrintError("Error in HF_FindNextRec called on DM_Subtract\n");
					if(HF_CloseFile(filedesc1) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
						return -1;
					}
					if(HF_CloseFile(filedesc2) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
						return -1;
					}
					if(HF_CloseFileScan(scandesc) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseIndexScan called on DM_subtract");
						return -1;
					}
					return -1;
				}
				memcpy(&indexed,record2 + 2*MAXNAME + 2*sizeof(int) + sizeof(char),sizeof(int));
				if(indexed == TRUE)//an exei eyrethrio
				{
					memcpy(&indexno,record2 + 2*MAXNAME + 3*sizeof(int) + sizeof(char),sizeof(int));
					//anoigoume to eyrethrio
					if((index = AM_OpenIndex (argv[1], indexno)) < 0)
					{
						AM_PrintError("Error in AM_OpenIndex\n");
						if(HF_CloseFile(filedesc1) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
							return -1;
						}
						if(HF_CloseFile(filedesc2) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
							return -1;
						}
						if(HF_CloseFileScan(scandesc) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseIndexScan called on DM_subtract");
							return -1;
						}
						return -1;
					}
					//pairnoume plhrofories gai to pedio
					memcpy(&type,record2 + 2*MAXNAME + 2*sizeof(int),sizeof(char));
					memcpy(&length,record2 + 2*MAXNAME + sizeof(int),sizeof(int));
					memcpy(&offset,record2 + 2*MAXNAME,sizeof(int));
					value = malloc(length);
					if(value == NULL)
					{
						printf("Error in malloc called on DM_subtract\n");
						if(HF_CloseFile(filedesc1) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
							return -1;
						}
						if(HF_CloseFile(filedesc2) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
							return -1;
						}
						if(HF_CloseFileScan(scandesc) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseIndexScan called on DM_subtract\n");
							return -1;
						}
						if(AM_CloseIndex(index) != AME_OK)
						{
							AM_PrintError("Error in AM_CloseIndex\n");
							return -1;
						}
						return -1;
					}
					//pairnoume thn timh ths eggrafhs tou 2ou arxeiou gia ayto to pedio
					memcpy(value,record + offset,length);
                                  //anoigoume sarwsh sto eyrethrio gia ayto to pedio
					if((scan = AM_OpenIndexScan(index,type,length,EQUAL,value)) <0)
					{
						AM_PrintError("Error in AM_OpenIndexScan called on DM_subtract\n");
						if(HF_CloseFile(filedesc1) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
							return -1;
						}
						if(HF_CloseFile(filedesc2) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
							return -1;
						}
						if(HF_CloseFileScan(scandesc) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseIndexScan called on DM_subtract");
							return -1;
						}
						if(AM_CloseIndex(index) != AME_OK)
						{
							AM_PrintError("Error in AM_CloseIndex\n");
							return -1;
						}
						return -1;
					}
					free(value);
					//afou brhkame ena apo ta eyrethria kanoume break
					break;
				}
			}
			//kleinoume th sarwsh sto attrCat
			if(HF_CloseFileScan(scandesc) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseIndexScan called on DM_subtract\n");
				if(HF_CloseFile(filedesc1) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
					return -1;
				}
				if(HF_CloseFile(filedesc2) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
					return -1;
				}
				if(AM_CloseIndexScan(scan) != AME_OK)
				{
					AM_PrintError("Error in AM_OpenIndexScan called on DM_subtract\n");
					return -1;
				}
				if(AM_CloseIndex(index) != AME_OK)
				{
					AM_PrintError("Error in AM_CloseIndex\n");
					return -1;
				}
				return -1;
			}
			while((id = AM_FindNextEntry(scan)) != AME_EOF)
			{
				if(id < 0)
				{
					AM_PrintError("Error in AM_FindNextEntry called on DM_subtract\n");
					if(HF_CloseFile(filedesc1) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_subtract\n");
						return -1;
					}
					if(HF_CloseFile(filedesc2) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
						return -1;
					}
					if(AM_CloseIndexScan(scan) != AME_OK)
					{
						AM_PrintError("Error in AM_OpenIndexScan called on DM_subtract\n");
						return -1;
					}
					if(AM_CloseIndex(index) != AME_OK)
					{
						AM_PrintError("Error in AM_CloseIndex\n");
						return -1;
					}
					return -1;
				}
				//fernoume thn eggrafh tou arxeiou apo opou diagrafoume
				if(HF_GetThisRec(filedesc1,id,record1,relwidth1) != HFE_OK)
				{
					HF_PrintError("Error in HF_GetThisRec called on DM_subtract\n");
					return -1;
				}
				//sygkrinoume tis eggrafes
				comp = compare_records(record1,record,argv[1]);
				if(comp == OK)//an einai idies 
				{
                                   //diagrafoume thn eggrafh apo to arxeio
					if(HF_DeleteRec(filedesc1,id,relwidth1) != HFE_OK)
					{
						HF_PrintError("Error in HF_DeleteRec called in DM_subtract\n");
						if(HF_CloseFile(filedesc1) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
							return -1;
						}
						if(HF_CloseFile(filedesc2) != HFE_OK)
						{
							HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
							return -1;
						}
						if(AM_CloseIndexScan(scan) != AME_OK)
						{
							AM_PrintError("Error in AM_OpenIndexScan called on DM_subtract\n");
							return -1;
						}
						if(AM_CloseIndex(index) != AME_OK)
						{
							AM_PrintError("Error in AM_CloseIndex\n");
							return -1;
						}
						return -1;
					}
					//meiwnoume plithos eggrafwn tou arxeiou
					rec_num--;
				}
				else if(comp == -1)
				{
					printf("Error in HF_DeletRec called on Dm_subtract\n");
					if(HF_CloseFile(filedesc1) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
						return -1;
					}
					if(HF_CloseFile(filedesc2) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
						return -1;
					}
					if(AM_CloseIndexScan(scan) != AME_OK)
					{
						AM_PrintError("Error in AM_OpenIndexScan called on DM_subtract\n");
						return -1;
					}
					if(AM_CloseIndex(index) != AME_OK)
					{
						AM_PrintError("Error in AM_CloseIndex\n");
						return -1;
					}
					return -1;
				}
				//diagrafoume thn eggrafh apo ta eyrthria
				if(delete_record_from_indices(id,record1,argv[1]) != DME_OK)
				{
					printf("Error in delete_record_from_indices\n");
					if(HF_CloseFile(filedesc1) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
						return -1;
					}
					if(HF_CloseFile(filedesc2) != HFE_OK)
					{
						HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
						return -1;
					}
					if(AM_CloseIndexScan(scan) != AME_OK)
					{
						AM_PrintError("Error in AM_OpenIndexScan called on DM_subtract\n");
						return -1;
					}
					if(AM_CloseIndex(index) != AME_OK)
					{
						AM_PrintError("Error in AM_CloseIndex\n");
						return -1;
					}
					return -1;
				}
			}
			//kleinoume th sarwshs
			if(AM_CloseIndexScan(scan) != AME_OK)
			{
				AM_PrintError("Error in AM_OpenIndexScan called on DM_subtract\n");
				if(HF_CloseFile(filedesc1) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
					return -1;
				}
				if(HF_CloseFile(filedesc2) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
					return -1;
				}
				if(AM_CloseIndex(index) != AME_OK)
				{
					AM_PrintError("Error in AM_CloseIndex\n");
					return -1;
				}
				return -1;
			}
			//kleisimo eyrethriou
			if(AM_CloseIndex(index) != AME_OK)
			{
			      AM_PrintError("Error in AM_CloseIndex\n");
			      if(HF_CloseFile(filedesc1) != HFE_OK)
				  {
					HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
					return -1;
				  }
				  if(HF_CloseFile(filedesc2) != HFE_OK)
				  {
					HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
					return -1;
				  }
			      return -1;
			}
			
		}
		pr_recid = HF_GetNextRec(filedesc2,pr_recid,record,relwidth1);
		
	}
	free(record1);
	free(record);
	//enhmerwsh eggrafhs sxeshs sto relCat me to neo plithos eggrafwn ths sxeshs
	memcpy(relation1 + MAXNAME + 3*sizeof(int),&rec_num,sizeof(int));
	if(update_relations(rel_id1,relation1) != UTE_OK)
	{
		printf("Error in update_relations called on DM_subtract\n");
		if(HF_CloseFile(filedesc1) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
			return -1;
		}
		if(HF_CloseFile(filedesc2) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_subtract\n");
			return -1;
		}
		return -1;
	}
	//kleisimo arxeiwn
	if(HF_CloseFile(filedesc1) != HFE_OK)
	{
		HF_PrintError("Error in HF_CloseFile called on DM_subtract\n");
		if(HF_CloseFile(filedesc2) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_subtract \n");
			return -1;
		}
		return -1;
	}
	if(HF_CloseFile(filedesc2) != HFE_OK)
	{
		HF_PrintError("Error in HF_CloseFile called on DM_subtract\n");
		return -1;
	}

	return DME_OK;
}

int DM_insert(int argc,char* argv[])
{
	int rel_id,relwidth,attrcnt,indexcnt,attr_id,offset,length,val,rec_id,rec_num,i,filedesc;
	float value;
	char relation[RELCATSIZE],*new_record,attribute[ATTRCATSIZE],type;

	if(argc%2 !=0)
	{
		printf("DM_insert: Wrong number of attributes given\n");
		return -1;
	}
	//fernoume thn eggrafh ths sxeshs apo to arxeio relCat	
	if((rel_id=giverelcatrecord(relation,argv[1]))<0)
	{
		if(rel_id == -1)	printf("Error in giverelcatrecord called on DM_insert\n");
		else if(rel_id == -2)	printf("Relation %s does not exist\n",argv[1]);
		return -1;
	}
	memcpy(&relwidth,relation + MAXNAME,sizeof(int));
	memcpy(&attrcnt,relation + MAXNAME + sizeof(int),sizeof(int));
	memcpy(&indexcnt,relation + MAXNAME + 2*sizeof(int),sizeof(int));
	memcpy(&rec_num,relation + MAXNAME + 3*sizeof(int),sizeof(int));
	if(attrcnt != ((argc-2)/2))     //attributes pou dothhkan ligotera apo ayta ths sxeshs
	{
		printf("DM_insert: Wrong number of attributes given\n");
		return -1;
	}
	new_record=(char *)malloc(relwidth);
	if(new_record == NULL)
	{
		printf("Error in malloc called on DM_insert\n");
		return -1;
	}
	//dhmiourgia eggrafhs
	for(i = 2;i < argc; i += 2)
	{
		//fernoume eggrafh tou pediou apo to arxeio attrCat
		if((attr_id = giveattrcatrecord(attribute,argv[1],argv[i])) < 0)
		{
			if(attr_id == -1)	printf("Error in giveattrcatrecord called on DM_insert\n");
			else if(attr_id == -2)	printf("Attribute %s does not exist\n",argv[i]);
			return -1;
		}
		//pairnoume plhrofories gia to pedio
		memcpy(&offset,attribute + 2*MAXNAME,sizeof(int));
		memcpy(&length,attribute + 2*MAXNAME + sizeof(int),sizeof(int));
		memcpy(&type,attribute + 2*MAXNAME + 2*sizeof(int),sizeof(char));
		switch (type)
		{
			case 'i' :
				if(isNumber(argv[i+1]) != 1)
				{
					printf("The argument given is not an integer\n");	
					return -1;
				}
				val = atoi(argv[i+1]);
				memcpy(new_record + offset,&val,sizeof(int));
				break;
			case 'f' :
				if(isFloat(argv[i+1]) != 1)
				{
					printf("The argument given is not a float\n");
					return -1;
				}
				value = (float)atof(argv[i+1]);
				memcpy(new_record + offset,&value,sizeof(float));
				break;
			case 'c' :
				memcpy(new_record + offset,argv[i+1],length);
				break;					
		}		
	}
	//anoigma arxeiou
	
	if((filedesc = HF_OpenFile(argv[1])) < 0)
	{
		HF_PrintError("Error in HF_OpenFile called on DM_insert\n");
		return -1;
	}
	//eisagwgh eggrafhs sto arxeio
	if((rec_id = HF_InsertRec(filedesc,new_record,relwidth)) < 0)
	{
		HF_PrintError("Error in HF_InsertRec called on DM_insert\n");
		if(HF_CloseFile(filedesc) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_insert\n");
			return -1;
		} 
		return -1;
	}
	rec_num++;
	//eisagwgh eggrafhs sta eyrethria an yparxoun
	
	if(indexcnt != -1){
		if(insert_record_into_indices(rec_id,new_record,argv[1]) != DME_OK)
		{
			printf("Error in insert_record_into_indices called on DM_insert\n");
			if(HF_CloseFile(filedesc) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_insert\n");
				return -1;
			} 
			return -1;
		}	
	}
	//enhmerwsh ths eggrafhs ths sxeshs sto relCat me neo plithos eggrafwn
	memcpy(relation + MAXNAME + 3*sizeof(int),&rec_num,sizeof(int));
	if(update_relations(rel_id,relation) != UTE_OK)
	{
		printf("Error in update_relations called on DM_insert\n");
		if(HF_CloseFile(filedesc) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_insert\n");
			return -1;
		} 
		return -1;
	}

	//kleisimo arxeiou
	if(HF_CloseFile(filedesc) != HFE_OK)
	{
		HF_PrintError("Error in HF_CloseFile called on DM_insert\n");
		return -1;
	} 
	
	free(new_record);
	return DME_OK;
}

int DM_add(int argc , char * argv[])
{
	int filedesc1,filedesc2,recid,indexcnt,rec_num,rel_id1,rel_id2;
	int attrcnt1,attrcnt2,relwidth1,relwidth2;
	char *record1,relation1[RELCATSIZE],relation2[RELCATSIZE];	

	if(argc != 3)
	{
		printf("Wrong number of arguments given\n");
		return -1;
	}

	if(!strcmp(argv[1],argv[2]))//eleghos an oi dyo sxeseis einai idies
	{
		printf("Relation 1 and relation 2 should not be the same\n");
		return -1;
	}
       //eleghos an yparxoun oi dyo sxeseis kai an nai fernoume apo to relCat tis eggrafes tous
	if((rel_id1 = giverelcatrecord(relation1,argv[1])) < 0)
	{
		if(rel_id1 == -1)printf("Error in giverelcatrecord called on DM_add\n");
		else if(rel_id1 == -2) printf("Relation %s does not exist\n",argv[1]);
		return -1;			
	}

	if((rel_id2 = giverelcatrecord(relation2,argv[2])) <0)
	{
		if(rel_id2 == -1)printf("Error in giverelcatrecord called on DM_add \n");
		else if(rel_id2 == -2) printf("Relation %s does not exist\n",argv[1]);
		return -1;			
	}
	
	memcpy(&attrcnt1,relation1 + MAXNAME + sizeof(int),sizeof(int));
	memcpy(&attrcnt2,relation2 + MAXNAME + sizeof(int),sizeof(int));
	if( attrcnt1 != attrcnt2)//eleghos an exoun idio plithos pediwn
	{
		printf("The relations given have different number of attributes\n");
		return -1;
	} 

	memcpy(&relwidth1,relation1 + MAXNAME,sizeof(int));
	memcpy(&relwidth2,relation2 + MAXNAME,sizeof(int));
	memcpy(&indexcnt,relation1 + MAXNAME + 2* sizeof(int),sizeof(int));
	memcpy(&rec_num,relation1 + MAXNAME + 3*sizeof(int),sizeof(int));
	

	if(relwidth1 < relwidth2)//an to mikos eggrafis out 1ou einai mikrotero apo ayths tou 2ou tote de xwraei h eggrafi sto arxeio
	{
		printf("The relations given have different width for their records\n");
		return -1;
	}
	
	if(check_attributes(argv[1],argv[2],attrcnt1) != DME_OK)
	{
		return -1;
	}
       //anoigma arxeiwn sxesewn
	if((filedesc1 = HF_OpenFile(argv[1])) < 0)
	{
		HF_PrintError("Error in HF_OpenFile called on DM_add\n");
		return -1;
	}	
	if((filedesc2 = HF_OpenFile(argv[2])) < 0)
	{
		HF_PrintError("Error in HF_OpenFile called on DM_add\n");
		return -1;
	}
	record1=(char *)malloc(relwidth2);
	if(record1 == NULL)
	{
		printf("Error in malloc called on DM_insert\n");
		if(HF_CloseFile(filedesc1) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_add\n");
			return -1;
		}
		if(HF_CloseFile(filedesc2) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_add\n");
			return -1;
		}
		return -1;
	}
	//fernoume mia mia tis eggrafes tou deyterou arxeiou
	if((recid=HF_GetFirstRec(filedesc2,record1,relwidth2)) <0 && recid != HFE_EOF )
	{
		HF_PrintError("Error in HF_GetFirstRec called on DM_add\n");
		if(HF_CloseFile(filedesc1) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_add\n");
			return -1;
		}
		if(HF_CloseFile(filedesc2) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_add\n");
			return -1;
		}
		return -1;
	}
	while(recid != HFE_EOF)
	{
		//eisagwgh sto arxeio 
		if(HF_InsertRec(filedesc1,record1,relwidth1) <0)
		{
			HF_PrintError("Error in HF_InsertRec called on DM_add\n");
			if(HF_CloseFile(filedesc1) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_add\n");
				return -1;
			}
			if(HF_CloseFile(filedesc2) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_add\n");
				return -1;
			}
			return -1;	
		}
		rec_num++;
		//eisagwgh sta eyrethria
		if(indexcnt != -1)
		{
			if(insert_record_into_indices(recid,record1,argv[1]) != DME_OK)
			{
				printf("Error in insert_record_into_indices called on DM_add\n");
				if(HF_CloseFile(filedesc1) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_add\n");
					return -1;
				}
				if(HF_CloseFile(filedesc2) != HFE_OK)
				{
					HF_PrintError("Error in HF_CloseFile called on DM_add\n");
					return -1;
				}
				return -1;
			}
				
			
		}
		recid=HF_GetNextRec(filedesc2,recid,record1,relwidth2);
		if(recid < 0 && recid!=HFE_EOF)
		{
			HF_PrintError("Error in HF_GetNextRec \n");
			if(HF_CloseFile(filedesc1) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_add\n");
				return -1;
			}
			if(HF_CloseFile(filedesc2) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFile called on DM_add\n");
				return -1;
			}
			return -1;	
		}
	}
	//enhmerwsh eggrafhs sxeshs sto relCat me neo plithos eggrafwn ths sxeshs
	memcpy(relation1 + MAXNAME + 3*sizeof(int),&rec_num,sizeof(int));
	if(update_relations(rel_id1,relation1) != UTE_OK)
	{
		printf("Error in update_relations called on DM_add\n");
		if(HF_CloseFile(filedesc1) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_add\n");
			return -1;
		}
		if(HF_CloseFile(filedesc2) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_add\n");
			return -1;
		}
		return -1;
	}
	free(record1);
	//kleisimo arxeiwn
	if(HF_CloseFile(filedesc1) != HFE_OK)
	{
		HF_PrintError("Error in HF_CloseFile called on DM_add\n");
		if(HF_CloseFile(filedesc2) != HFE_OK)
		{
			HF_PrintError("Error in HF_CloseFile called on DM_add\n");
			return -1;
		}
		return -1;
	}
	if(HF_CloseFile(filedesc2) != HFE_OK)
	{
		HF_PrintError("Error in HF_CloseFile called on DM_add\n");
		return -1;
	}

	return DME_OK;
}
