/*-----------------------------------ANAPTYKSH LOGISMIKOY---------------------------------------------------------*/
/*------------------------------------4o MEROS- VW_Lib.c----------------------------------------------------------*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Gia thn ergasia ayth ergasthkan oi foithtes :~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*1)Karaiskos Xrhstos me A.M :1115200500063
2)Poutachidou Maria me A.M :1115200600063
3)Tsirikou Dimitra  me A.M :1115200600153
###################################################################################################################
H askhsh ayth dokimasthke epityxws sta mhxanhmata linux ths sxolhs!!!!!
Gia th metaglwttish kai ektelesh ayths ths askhshs ayths plhktrologoume akolouthes tis entoles:*/
//dbcreate <onoma_bashs>
//gcc -o aminirel *.c *.o -lm
//aminirel <onoma_bashs>
//dbdestroy <onoma_bashs>

#include "functions.h"

/*---global variables---*/
extern int relcatFileDesc;
extern int attrcatFileDesc;
extern int viewcatFileDesc;
extern int viewattrcatFileDesc;


/*-----------------Pairnei san orisma enan int kai mas epistrefei ton telesth ston opoio antistoixei-----------------*/
char* reverse_op(int op)
{
	if(op == 1)
		return "=";
	if(op == 2)
		return "<";
	if(op == 3)
		return ">";
	if(op == 4)
		return "<=";
	if(op == 5)
		return ">=";
	if(op == 6)
		return "!=";
}


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

/*-----------------------------Synarthseis gia thn epexergasia twn viewCat kai viewattrCat------------------------*/
/*-------------------------------------Eisagei mia eggrafh sto arxeio viewCat-------------------------------------*/
int insertInViewCat(char *viewname,int type,char *relname1,char *attrname1,int op,char* relname2,char *attrname2,char *value,int attrcnt)
{
	int recId;	
	char* record,name[MAXNAME];

	record=(char*)malloc(VIEWCATSIZE);
	if(record == NULL)
	{
		printf("error in malloc\n");
		return -1;
	}
	memset(record,'\0',VIEWCATSIZE);
	memcpy((void *)record,(void *)viewname,MAXNAME);
	record += MAXNAME;
	memcpy((void *)record,(void *)&type,sizeof(int));
	record += sizeof(int);
	memcpy((void *)record,(void *)relname1,MAXNAME);
	record += MAXNAME;
	memcpy((void *)record,(void *)attrname1,MAXNAME);
	record += MAXNAME;
	memcpy((void *)record,(void *)&op,sizeof(int));
	record += sizeof(int);
	memcpy((void *)record,(void *)relname2,MAXNAME);
	memcpy(name,record,MAXNAME);	
	record += MAXNAME;
	memcpy((void *)record,(void *)attrname2,MAXNAME);
	record += MAXNAME;
	memcpy((void *)record,(void *)value,MAXNAME);
	record += MAXSTRINGLENGTH;
	memcpy((void *)record,(void *)&attrcnt,sizeof(int));
	record += sizeof(int);
	record -= VIEWCATSIZE;
	recId = HF_InsertRec(viewcatFileDesc,record,VIEWCATSIZE);
	if (recId < 0)
	{
		printf("ERROR in insertInviewCat\n");
		HF_PrintError("Record not inserted.\n");
	}
	free(record);
	return recId;
}

/*------------------------------------Eisagei mia eggrafh sto arxeio viewAttrCat------------------------*/
int insertInViewAttrCat(char *viewname,char *viewattrname,char *relname,char *relattrname)
{
	int recId;	
	char* record;

	record=(char*)malloc(VIEWATTRCATSIZE);
	if(record == NULL)
	{
		printf("error in malloc\n");
		return -1;
	}
	memset(record,'\0',VIEWATTRCATSIZE);
	memcpy((void *)record,(void *)viewname,MAXNAME);
	record += MAXNAME;
	memcpy((void *)record,(void *)viewattrname,MAXNAME);
	record += MAXNAME;
	memcpy((void *)record,(void *)relname,MAXNAME);
	record += MAXNAME;
	memcpy((void *)record,(void *)relattrname,MAXNAME);
	record += MAXNAME;
	record -= VIEWATTRCATSIZE;
	recId = HF_InsertRec(viewattrcatFileDesc,record,VIEWATTRCATSIZE);
	if (recId < 0)
	{
		printf("ERROR in insertInviewAttrCat\n");
		HF_PrintError("Record not inserted.\n");
	}
	free(record);
	return recId;
}


/*-------------------------Fernei mia eggrafi apo to viewCat---------------------------------------*/
int giveviewcatrecord(char* record,char *relation)
{
	int scandesc,recid;	

	if((scandesc = HF_OpenFileScan(viewcatFileDesc,VIEWCATSIZE,'c',MAXNAME,0,EQUAL,relation)) < 0)
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
/*-----------------------------------Fernei mia eggrafi apo to viewattrCat-------------------------------------------*/
int giveViewattrcatrecord(char *record,char *relation,char *attribute)
{
	int scandesc,recid;
	char temp[MAXNAME];

	if((scandesc = HF_OpenFileScan(viewattrcatFileDesc,VIEWATTRCATSIZE,'c',MAXNAME,0,EQUAL,relation)) < 0)
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

/*-----------------diagrafei apo to viewattrCat tis eggrafes poy antistoixoun sta pedia mias opshs----------------*/
int delete_viewattrs(char *view)
{
	int scandesc,recid;
	char record[VIEWATTRCATSIZE];

	if((scandesc = HF_OpenFileScan(viewattrcatFileDesc,VIEWATTRCATSIZE,'c',MAXNAME,0,EQUAL,view)) < 0)
	{
		HF_PrintError("Error in HF_OpenIndexScan\n");
		return -1;	
	}

	while((recid = HF_FindNextRec(scandesc,record)) != HFE_EOF)
	{
		if(HF_DeleteRec(viewattrcatFileDesc,recid,VIEWATTRCATSIZE) != HFE_OK)
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
	return OK;

}
/*-----------------------------------------------------------------------------------------------------------------*/

/*----------------Synarthseis pou apodesmeyoun th mnhmh pou desmeysame otan kataskeyazame ta orismata--------------*/
void free_argument(char ***argument)
{
	free((*argument)[0]);
	*argument = NULL;
	return ;
}

void free_insert_argument(char ***argument)
{
	free((*argument)[1]);
	*argument = NULL;
	return ;
}


void free_args(char ***argument,int argc)		//opou exoume kanei malloc ola ta strings
{
	int i;	

	for(i = 0; i < argc ; i++)free((*argument)[i]);			//apodesmeysi strings
	free(*argument);
	*argument = NULL;
	return;
}
/*-----------------------------------------------------------------------------------------------------------------*/


/*-----------------------------------Synarthseis gia thn kataskeyh orismatwn--------------------------------------*/
int construct_select_arg(char ***argument,int argc,char* viewname,char* relation)
{
	int i,recid,scandesc;
	char **buf,temp[MAXNAME],field[VIEWATTRCATSIZE],attrname[MAXNAME];
	
	buf = NULL;


	if ((buf = (char**)malloc((argc+1)*sizeof(char *))) == NULL)//desmeysi deiktwn
	{
		printf("Error in malloc called on construct argument\n");
		return -1;
	}

	for(i = 0; i < argc ; i++)					//desmeysi strings
	{
		if((buf[i] = malloc(MAXNAME)) == NULL)
		{
			printf("Error in malloc called on construct argument\n");
			return -1;
		}
	} 
	strcpy(buf[0],"select");
	sprintf(temp,"%s.%s",viewname,"temp");
	strcpy(buf[1],temp);
	sprintf(buf[2],"%d",(argc - 3)/2);
	
	if((scandesc = HF_OpenFileScan(viewattrcatFileDesc,VIEWATTRCATSIZE,'c',MAXNAME,0,EQUAL,viewname)) < 0)
	{
		HF_PrintError("Error in HF_OpenIndexScan\n");
		return -1;	
	}
	i = 3;
	while((recid = HF_FindNextRec(scandesc,field)) != HFE_EOF) 
	{
		if(recid < 0)
		{
			HF_PrintError("Error in HF_FindNextRec called on construct_select_arg\n");
			if(HF_CloseFileScan(scandesc) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFileScan\n");
			}			
			return -1;
		}
		strcpy(buf[i],relation);
		memcpy(attrname,field + MAXNAME,MAXNAME);
		strcpy(buf[i+1],attrname);
		i+=2;
	}
	if(HF_CloseFileScan(scandesc) != HFE_OK)
	{
		HF_PrintError("Error in HF_CloseFileScan\n");
		return -1;
	}
	buf[argc] = NULL;
	*argument = buf;
	return OK;
}

int select_arg(char ***argument,int argc,char* relation,char* destination)
{
	int i,k;
	char **buf;
	relDesc rel;
	attrDesc* attributes; 
	buf = NULL;


	if ((buf = (char**)malloc((argc+1)*sizeof(char *))) == NULL)//desmeysi deiktwn
	{
		printf("Error in malloc called on construct argument\n");
		return -1;
	}

	for(i = 0; i < argc ; i++)					//desmeysi strings
	{
		if((buf[i] = malloc(MAXNAME)) == NULL)
		{
			printf("Error in malloc called on construct argument\n");
			return -1;
		}
	} 
	strcpy(buf[0],"select");
	strcpy(buf[1],destination);
	sprintf(buf[2],"%d",(argc - 3)/2);
	if(UT_relInfo(relation,&rel, &attributes) < 0)
	{
		printf("Relation with name %s does not exist\n",relation);		
		return -1;
	}
	for(k = 0,i = 3;k < rel.attrcnt; k++, i+=2)
	{
		strcpy(buf[i],relation);
		strcpy(buf[i+1],attributes[k].attrname);
	}
	buf[argc] = NULL;
	*argument = buf;
	return OK;
}


int select_cond_arg(char *** argument,int argc,char* relation,char* destination,char*field,char* op,char* value)
{
	int i,k;
	char **buf;
	relDesc rel;
	attrDesc* attributes;
	buf = NULL;


	if ((buf = (char**)malloc((argc+1)*sizeof(char *))) == NULL)//desmeysi deiktwn
	{
		printf("Error in malloc called on construct argument\n");
		return -1;
	}

	for(i = 0; i < argc ; i++)					//desmeysi strings
	{
		if((buf[i] = malloc(MAXNAME)) == NULL)
		{
			printf("Error in malloc called on construct argument\n");
			return -1;
		}
	} 
	strcpy(buf[0],"select");
	strcpy(buf[1],destination);
	sprintf(buf[2],"%d",(argc - 7)/2);
	if(UT_relInfo(relation,&rel, &attributes) < 0)
	{
		printf("Relation with name %s does not exist\n",relation);		
		return -1;
	}
	for(k = 0,i = 3;k < rel.attrcnt; k++, i+=2)
	{
		strcpy(buf[i],relation);
		strcpy(buf[i+1],attributes[k].attrname);
	}
	strcpy(buf[argc-4],relation);
	strcpy(buf[argc-3],field);
	strcpy(buf[argc-2],op);
	strcpy(buf[argc-1],value);
	buf[argc] = NULL;
	*argument = buf;
	return OK;
}

int construct_select_cond_arg(char ***argument,int argc,char* viewname,char* relation,char* attrname,int op,char* value)
{
	int i,recid,scandesc;
	char **buf,temp[MAXNAME],field[VIEWATTRCATSIZE],name[MAXNAME];
	
	buf = NULL;


	if ((buf = (char**)malloc((argc+1)*sizeof(char *))) == NULL)//desmeysi deiktwn
	{
		printf("Error in malloc called on construct argument\n");
		return -1;
	}

	for(i = 0; i < argc ; i++)					//desmeysi strings
	{
		if((buf[i] = malloc(MAXNAME)) == NULL)
		{
			printf("Error in malloc called on construct argument\n");
			return -1;
		}
	} 
	strcpy(buf[0],"select");
	sprintf(temp,"%s.%s",viewname,"temp");
	strcpy(buf[1],temp);
	sprintf(buf[2],"%d",(argc-7)/2);
	
	if((scandesc = HF_OpenFileScan(viewattrcatFileDesc,VIEWATTRCATSIZE,'c',MAXNAME,0,EQUAL,viewname)) < 0)
	{
		HF_PrintError("Error in HF_OpenIndexScan\n");
		return -1;	
	}
	i = 3;
	while((recid = HF_FindNextRec(scandesc,field)) != HFE_EOF) 
	{
		if(recid < 0)
		{
			HF_PrintError("Error in HF_FindNextRec called on construct_select_arg\n");
			if(HF_CloseFileScan(scandesc) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFileScan\n");
			}			
			return -1;
		}
		strcpy(buf[i],relation);
		memcpy(name,field + MAXNAME,MAXNAME);
		strcpy(buf[i+1],name);
		i+=2;
	}
	strcpy(buf[argc-4],relation);
	strcpy(buf[argc-3],attrname);
	strcpy(buf[argc-2],reverse_op(op));
	strcpy(buf[argc-1],value);
	if(HF_CloseFileScan(scandesc) != HFE_OK)
	{
		HF_PrintError("Error in HF_CloseFileScan\n");
		return -1;
	}
	buf[argc] = NULL;
	*argument = buf;
	return OK;
}

int construct_join_arg(char ***argument,int argc,char* viewname,char* relation,char* relname1,char* attrname1,int op,char* relname2,char*attrname2,char*inrel)
{
	int i,recid,scandesc;
	char **buf,temp[MAXNAME],field[VIEWATTRCATSIZE],attrname[MAXNAME],rel[MAXNAME];
	
	buf = NULL;
	if ((buf = (char**)malloc((argc+1)*sizeof(char *))) == NULL)//desmeysi deiktwn
	{
		printf("Error in malloc called on construct argument\n");
		return -1;
	}

	for(i = 0; i < argc ; i++)					//desmeysi strings
	{
		if((buf[i] = malloc(MAXNAME)) == NULL)
		{
			printf("Error in malloc called on construct argument\n");
			return -1;
		}
	} 
	strcpy(buf[0],"join");
	sprintf(temp,"%s.%s",viewname,"temp");
	strcpy(buf[1],temp);
	sprintf(buf[2],"%d",(argc-8)/2);
	
	if((scandesc = HF_OpenFileScan(viewattrcatFileDesc,VIEWATTRCATSIZE,'c',MAXNAME,0,EQUAL,viewname)) < 0)
	{
		HF_PrintError("Error in HF_OpenIndexScan\n");
		return -1;	
	}
	i = 3;
	while((recid = HF_FindNextRec(scandesc,field)) != HFE_EOF) 
	{
		if(recid < 0)
		{
			HF_PrintError("Error in HF_FindNextRec called on construct_select_arg\n");
			if(HF_CloseFileScan(scandesc) != HFE_OK)
			{
				HF_PrintError("Error in HF_CloseFileScan\n");
			}			
			return -1;
		}
		memcpy(rel,field + 2*MAXNAME,MAXNAME);
		if(strcmp(inrel,rel) == 0)
			strcpy(buf[i],relation);
		else 
			strcpy(buf[i],rel);
		memcpy(attrname,field + MAXNAME,MAXNAME);
		strcpy(buf[i+1],attrname);
		i+=2;
	}
	if(strcmp(inrel,relname1) == 0)
		strcpy(buf[argc-5],relation);
	else
		strcpy(buf[argc-5],relname1);
	strcpy(buf[argc-4],attrname1);
	strcpy(buf[argc-3],reverse_op(op));
	if(strcmp(inrel,relname2) == 0)
		strcpy(buf[argc-2],relation);
	else
		strcpy(buf[argc-2],relname2);
	strcpy(buf[argc-1],attrname2);
	if(HF_CloseFileScan(scandesc) != HFE_OK)
	{
		HF_PrintError("Error in HF_CloseFileScan\n");
		return -1;
	}
	buf[argc] = NULL;
	*argument = buf;
	return OK;
}

int construct_add_arg(char*** argument,int argc,char* relation1,char* relation2)
{
	int i;
	char **buf;

	buf = NULL;


	if ((buf = (char**)malloc((argc+1)*sizeof(char *))) == NULL)//desmeysi deiktwn
	{
		printf("Error in malloc called on construct argument\n");
		return -1;
	}

	for(i = 0; i < argc ; i++)					//desmeysi strings
	{
		if((buf[i] = malloc(MAXNAME)) == NULL)
		{
			printf("Error in malloc called on construct argument\n");
			return -1;
		}
	}
	strcpy(buf[0],"add");
	strcpy(buf[1],relation1);
	strcpy(buf[2],relation2);
	buf[argc] = NULL;
	*argument = buf;
	return OK;
}

int construct_sub_arg(char*** argument,int argc,char* relation1,char* relation2)
{
	int i;
	char **buf;

	buf = NULL;


	if ((buf = (char**)malloc((argc+1)*sizeof(char *))) == NULL)//desmeysi deiktwn
	{
		printf("Error in malloc called on construct argument\n");
		return -1;
	}

	for(i = 0; i < argc ; i++)					//desmeysi strings
	{
		if((buf[i] = malloc(MAXNAME)) == NULL)
		{
			printf("Error in malloc called on construct argument\n");
			return -1;
		}
	}
	strcpy(buf[0],"subtract");
	strcpy(buf[1],relation1);
	strcpy(buf[2],relation2);
	buf[argc] = NULL;
	*argument = buf;
	return OK;
}

int construct_destroy_arg(char*** argument,int argc,char* relation)
{
	int i;
	char **buf;

	buf = NULL;


	if ((buf = (char**)malloc(argc*sizeof(char *))) == NULL)//desmeysi deiktwn
	{
		printf("Error in malloc called on construct argument\n");
		return -1;
	}

	for(i = 0; i < argc ; i++)					//desmeysi strings
	{
		if((buf[i] = malloc(MAXNAME)) == NULL)
		{
			printf("Error in malloc called on construct argument\n");
			return -1;
		}
	}
	strcpy(buf[0],"destroy");
	strcpy(buf[1],relation);
	*argument = buf;
	return OK;
}


int construct_argument(char ***argument,char *argv[],int count,int mode)
{
	int i;
	char ** arg;
	arg = malloc((count+1)*sizeof(char*));
	for (i = 1 ; i<count ; i++) arg[i] = argv[i];
	if (mode == 0)
	{
		arg[0] = malloc((strlen("select")+1)*sizeof(char));
		strcpy(arg[0],"select");
		arg[count] = NULL;
		*argument=arg;
		return VWE_OK;
	}
	else if (mode == 1)
	{
		arg[0] = malloc((strlen("join")+1)*sizeof(char));
		strcpy(arg[0],"join");
		arg[count] = NULL;
		*argument=arg;
		return VWE_OK;
	}
	else if (mode ==2)
	{
		arg[0] = malloc((strlen("destroy")+1)*sizeof(char));
		strcpy(arg[0],"destroy");
		arg[count] = NULL;
		*argument=arg;
		return VWE_OK;
	}
	
	return -1;
}

int construct_insert_arg(char ***argument,char *argv[],int count,char *relname)
{
	int i;
	char ** arg;
	arg = malloc((count+1)*sizeof(char*));
	for(i = 0; i < count ;i++) arg[i] = argv[i];
	arg[1] = malloc((strlen(relname) + 1)*sizeof(char));
	strcpy(arg[1],relname);
	arg[count] = NULL;
	*argument=arg;
	return VWE_OK;
}

int construct_create_arg(char ***argument,char *argv[],int argc,char* viewname)
{
	int i,length,attrcnt,k,l;
	char **buf,type,format[MAXNAME];
	relDesc relation;
	attrDesc* attributes;	


	buf = NULL;


	if ((buf = (char**)malloc((argc+1)*sizeof(char *))) == NULL)//desmeysi deiktwn
	{
		printf("Error in malloc called on construct argument\n");
		return -1;
	}

	for(i = 0; i < argc ; i++)					//desmeysi strings
	{
		if((buf[i] = malloc(MAXNAME)) == NULL)
		{
			printf("Error in malloc called on construct argument\n");
			return -1;
		}
	} 
	strcpy(buf[0],"create");
	strcpy(buf[1],viewname);
	if(UT_relInfo(argv[1],&relation, &attributes) < 0)           
	{
		printf("Relation with name %s does not exist\n",argv[1]);
		return -1;		
	}
	attrcnt = relation.attrcnt;
	for(k = 0,l = 2; k < attrcnt;k++,l+=2)	
	{
		for(i = 2; i < argc; i+=2)
		{
			if(strcmp(argv[i],attributes[k].attrname) == 0) break;
		}		
 		type = attributes[k].attrtype;
		if(type == 'c')
		{
			length = attributes[k].attrlength;
			sprintf(format,"'c%d'",length);
		}
		else if(type == 'i') strcpy(format,"'i'");
		else if(type == 'f') strcpy(format,"'f'");
		strcpy(buf[l],argv[i]);
		strcpy(buf[l + 1],format);
	}
	buf[argc] = NULL;
	*argument = buf;
	return 1;
}


/*-----------------------------------------------------------------------------------------------------------------*/

/*----------Bohthtikh synarthsh pou analoga me to mode kanei eisagwgh h diaggrarh stis opseis mias sxeshs----------*/
int update_views(char *view,char* initial_relation,char *temp,int mode)
{
	int arg_num,attrcnt,flag,f_recId,op;
	char view_record[VIEWCATSIZE],**argument,record[VIEWCATSIZE],help[MAXNAME],relname1[MAXNAME],relname2[MAXNAME],**add_argv,**destroy_argv;
	char value[MAXSTRINGLENGTH],attrname1[MAXNAME],attrname2[MAXNAME],**sub_argv,viewname[MAXNAME];
	int type,typos;
	
	//fernoume apo to viwCat thn eggrafh pou antistoixei sthn opsh view
	if(giveviewcatrecord(view_record,view) <0 )
	{
		printf("Error in giveviewcatrecord called on update\n");
		return -1;
	}
	//pernoume ton typo ths opshs
	memcpy(&type,view_record + MAXNAME,sizeof(int));
	//analoga me to ti typou einai h opsh kataskeyazoume ta orismata pou mas xreiazontai kai kaloume thn antistoixh
	//DM epipedoy synarthsh h opoia kanei select (aplh h me synthkh) h join into se ena proswrino arxeio me onoma
	//<viewnameT> to opoio tha periexei tis eggrafes pou analoga me to mode tha prepei na eisagoume h na 
	//diagrapsoume apo thn opsh
	if(type == 0)
	{
		memcpy(&attrcnt,view_record + 5*MAXNAME + sizeof(int) + sizeof(int) + MAXSTRINGLENGTH,sizeof(int));
		arg_num=3 +2* attrcnt;
		
		if(construct_select_arg(&argument,arg_num,view,temp) != OK)
		{
			printf("Error in construct_select_arg called on update_views\n");
			return -1;
		}
		if(DM_select(arg_num,argument) != DME_OK)
		{
			printf("Error in DM_select called on update_views\n");
			return -1;
		}
	}
	if(type ==1)
	{
		memcpy(&attrcnt,view_record + 5*MAXNAME + sizeof(int) + sizeof(int) + MAXSTRINGLENGTH,sizeof(int));
		arg_num=3 +2* attrcnt+4;
		memcpy(&op,view_record + 3*MAXNAME +sizeof(int),sizeof(int));
		memcpy(attrname1,view_record + 2*MAXNAME +sizeof(int),MAXNAME);
		memcpy(value,view_record + 5*MAXNAME + sizeof(int) + sizeof(int),MAXSTRINGLENGTH);
		if(construct_select_cond_arg(&argument,arg_num,view,temp,attrname1,op,value) != OK)
		{
			printf("Error in construct_select_cond_arg called on update_views\n");
			return -1;
		}
		if(DM_select(arg_num,argument) != DME_OK)
		{
			printf("Error in DM_select called on update_views\n");
			return -1;
		}
	}
	if(type ==2)
	{
		memcpy(&attrcnt,view_record + 5*MAXNAME + 2*sizeof(int) + MAXSTRINGLENGTH,sizeof(int));
		arg_num=3 +2* attrcnt+5;
		memcpy(&op,view_record + 3*MAXNAME +sizeof(int),sizeof(int));
		memcpy(relname1,view_record + MAXNAME +sizeof(int),MAXNAME);
		memcpy(attrname1,view_record + 2*MAXNAME +sizeof(int),MAXNAME);
		memcpy(relname2,view_record + 3*MAXNAME +2*sizeof(int),MAXNAME);
		memcpy(attrname2,view_record + 4*MAXNAME + 2*sizeof(int),MAXNAME);
		if(construct_join_arg(&argument,arg_num,view,temp,relname1,attrname1,op,relname2,attrname2,initial_relation) != OK)	
		{
			printf("Error in construct_select_join_arg called on update_views\n");
			return -1;
		}
		if(DM_join(arg_num,argument) != DME_OK)
		{
			printf("Error in DM_select called on update_views\n");
			return -1;
		}
	}
	free_args(&argument,arg_num);
	//fernoume apo to arxeio viewCat mia mia tis eggrafes twn opsewn.Gia kathemia apo aytes pou basizetai sthn opsh kaloume thn update_views
	if((f_recId = HF_GetFirstRec(viewcatFileDesc,record,VIEWCATSIZE)) < 0 && f_recId != HFE_EOF)
	{
		HF_PrintError("Error in HF_GetFirstRec called on VW_Insert\n");
		return -1;
	}
	while(f_recId != HFE_EOF)
	{
		flag = 0;
		memcpy(&typos,record + MAXNAME,sizeof(int));
		if(typos == 0 || typos == 1)
		{
			memcpy(relname1,record + MAXNAME + sizeof(int),MAXNAME);
			if(!strcmp(relname1,view)) flag=1;
		}
		else if(typos == 2)
		{
			memcpy(relname1,record + MAXNAME + sizeof(int),MAXNAME);
			memcpy(relname2,record + 3*MAXNAME + sizeof(int) + sizeof(int),MAXNAME);
			if(!strcmp(relname1,view) || !strcmp(relname2,view)) flag=1;

		}
		if(flag == 1)
		{			
			sprintf(help,"%s.%s",view,"temp");
			memcpy(viewname,record,MAXNAME);
			update_views(viewname,view,help,mode);
		}
		f_recId = HF_GetNextRec(viewcatFileDesc,f_recId,record,VIEWCATSIZE);
		if(f_recId <0 && f_recId != HFE_EOF)
		{
			HF_PrintError("Error in HF_GetNextRec called on existing_views\n");
			return -1;
		}
	
	}
	sprintf(help,"%s.%s",view,"temp");
	if(mode == 0)
	{
		if(construct_add_arg(&add_argv,3,view,help) != OK)
		{
			printf("Error in construct_add_arg called on update_views\n");
			return -1;
		}
		
		if(DM_add(3,add_argv) != DME_OK)
		{
			printf("Error in DM_add called on update_views\n");
			return -1;
		}
		free_args(&add_argv,3);
	}
	else if(mode == 1)
	{
		if(construct_sub_arg(&sub_argv,3,view,help) != OK)
		{
			printf("Error in construct_add_arg called on update_views\n");
			return -1;
		}
		if(DM_subtract(3,sub_argv) != DME_OK)
		{
			printf("Error in DM_subtract called on update_views\n");
			return -1;
		}
		free_args(&sub_argv,3);
	}
	
	if(construct_destroy_arg(&destroy_argv,2,help) != OK)
	{
		printf("Error in construct_destroy_arg called on update_views\n");
		return -1;
	}
	if(UT_destroy(2,destroy_argv) != UTE_OK)
	{
		printf("Error in UT_destroy called on update_views\n");
		return -1;
	}
	free_args(&destroy_argv,2);
	
	return OK;
	
}

/*----Epistrefei 0 an to name den einai arxeio panw sto opoio vasizetai mia opsh k 1 an to name einai arxeio panw sto opoio vasizetai mia opsh----*/
int existing_views(char *name)
{
	int f_recId;
	int type;
	char record[VIEWCATSIZE],relname1[MAXNAME],relname2[MAXNAME];	

	//fernoume apo to viewCat mia mia tis eggrafes twn opsewn kai eleghoume an basizontai panw sthn opsh me onoma name 
	if((f_recId = HF_GetFirstRec(viewcatFileDesc,record,VIEWCATSIZE)) < 0 && f_recId != HFE_EOF)
	{
		HF_PrintError("Error in HF_GetFirstRec called on existing_views\n");
		return -1;
	}
	while(f_recId != HFE_EOF)
	{
		memcpy(&type,record + MAXNAME,sizeof(int));
		if(type == 0 || type == 1)
		{
			memcpy(relname1,record + MAXNAME + sizeof(int),MAXNAME);
			if(!strcmp(relname1,name))
				return 1;
		}
		else if(type == 2)
		{
			memcpy(relname1,record + MAXNAME + sizeof(int),MAXNAME);
			memcpy(relname2,record + 3*MAXNAME + sizeof(int) + sizeof(int),MAXNAME);
			if(!strcmp(relname1,name))
				return 1;
			if(!strcmp(relname2,name))
				return 1;
		}
		f_recId = HF_GetNextRec(viewcatFileDesc,f_recId,record,VIEWCATSIZE);
		if(f_recId <0 && f_recId != HFE_EOF)
		{
			HF_PrintError("Error in HF_GetNextRec called on existing_views\n");
			return -1;
		}
		
	}
	return 0;
}
