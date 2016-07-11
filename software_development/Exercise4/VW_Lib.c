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

#include "VW_Lib.h"


/*---global variables---*/
extern int relcatFileDesc;
extern int attrcatFileDesc;
extern int viewcatFileDesc;
extern int viewattrcatFileDesc;


void printArgs(int argc, char **argv)
{
	int i;
	for(i=0;i<argc;++i)
		printf("argv[%d]=%s\n",i,argv[i]);
	printf("argv[%d]=(null)\n\n", i);
	fflush(stdout);
}

int VW_createSelect(int argc,char *argv[])
{
	int i,viewret,attrno,l,op,flag,k,attrcnt;
	char viewrecord[VIEWCATSIZE],type;
	char **argument;
	relDesc relrecord;
	attrDesc* attributes;


	/*-----------Elegxos an h opsh pros dhmiourgia yparxei hdh---------------*/
	viewret = giveviewcatrecord(viewrecord,argv[1]);
	if( viewret >= 0 )	
	{
		printf("The view given already exists\n");
		return -1;
	}
	
	if(viewret == -1)
	{
		printf("Error in giveviewcatrecord called on VW_CreateSelect\n");
		return -1;
	}
	/*---Eleghos an oi sxeseis apo thn opoia proerxontai ta pedia pros probolh diaferoun--*/ 
	attrno = (int)atoi(argv[2]);
	for(l = 0,i = 3; l < attrno ; l++,i+=2)
	{
		if(strcmp(argv[3],argv[i]))
		{
			printf("The relations given differ\n");
			return -1;
		}
	}
	/*---Eleghos an h sxesh apo thn opoia proerxontai ta pedia pros probolh yparxei--*/
	if(UT_relInfo(argv[3],&relrecord, &attributes) < 0) {
		printf("Relation with name %s does not exist\n",argv[3]);
		return -1;
	}
	/*---Eleghos an ta pedia pros probolh einai ontws pedia ths sxeshw probolhs---*/
	attrcnt = relrecord.attrcnt; 
	for(l = 0, i = 4; l < attrno; l++, i += 2)
	{
		flag = 0;
		for(k = 0; k < attrcnt; k++)
		{
			if(strcmp(argv[i],attributes[k].attrname) == 0)
			{
				
				flag = 1;
				break;
			}
		}
		if(flag == 0)
		{
			printf("Attribute with name %s does not exist\n",argv[i]);
			return -1;
		}
	}
	/*-----------Kataskeyh orismatwn ths select-----------*/
	if(construct_argument(&argument,argv,argc,0) != VWE_OK) 
	{
		printf("Error in construct_argument called on VW_CreateSelect\n");
		return -1;
	}
	if(DM_select(argc,argument) != DME_OK)
	{
		printf("Error in DM_select called on VW_CreateSelect\n");
		return -1;
	}
	free_argument(&argument);
	/*-------------------------------Enhmerwsh tou viwCat--------------------------*/
	if(argv[3 + 2*attrno] ==NULL)
	{
		if(insertInViewCat(argv[1],0,argv[3],"NULL",-1,"NULL","NULL","NULL",attrno) < 0) 
		{
			printf("Error in insertInViewCat called on VW_insertInViewCat\n");
			return -1;
		}	
	}
	else
	{
		for(l = 0, i = 4; l < attrno; l++, i += 2) //<------ALLAGH!!!!!!!
		{
			for(k = 0; k <attrcnt ; k++)
			{
				if(strcmp(argv[i],attributes[k].attrname) == 0) break;
			}
		}
		type = attributes[k].attrtype;
		if(type == 'i')
		{
			if(isNumber(argv[argc -1]) != 1)
			{
				printf("The value given is not an integer as it should be according to its type\n");
				return -1;
			}
		}
		if(type == 'f')
		{
			if(isFloat(argv[argc -1]) != 1)
			{
				printf("The value given is not a float as it should be according to its type\n");
				return -1;			
			}
		}
		if((op = find_operator(argv[argc - 2])) == -1)
		{
			printf("Invalid operator\n");
			return -1;
		}
		if(insertInViewCat(argv[1],1,argv[argc - 4],argv[argc - 3],op,"NULL","NULL",argv[argc -1],attrno) < 0)
		{
			printf("Error in insertInViewCat called on VW_CreateSelect\n");
			return -1;
		}
	}		
	/*----------------------Enhmerwsh tou viewattrCat----------------------*/
	for(l = 0,i = 4; l < attrno ; l++,i+=2)
	{
		if(insertInViewAttrCat(argv[1],argv[i],argv[3],argv[i]) <0)
		{
			printf("Error in insertInViewAttrCat\n");
			return -1;
		}
	}
	return VWE_OK;

}


int VW_createJoin(int argc,char* argv[])
{
	int i,viewret,attrno,l,op,flag,k,attrcnt1,attrcnt2,id1,id2;
	char viewrecord[VIEWCATSIZE];
	char **argument;
	relDesc relrecord1,relrecord2;
	attrDesc *attributes1,*attributes2;

	/*-----------Elegxos an h opsh pros dhmiourgia yparxei hdh---------------*/
	viewret = giveviewcatrecord(viewrecord,argv[1]);
	if( viewret >= 0 )  
	{
		printf("The view given already exists\n");
		return -1;
	}
	if(viewret == -1)
	{
		printf("Error in giveviewcatrecord called on VW_CreateSelect\n");
		return -1;
	}
	/*-----------Eleghos an oi sxeseis tou join einai idies-----------*/
	if(!strcmp(argv[argc - 5],argv[argc - 2]))
	{
		printf("Selfjoin is forbidden\n");
		return -1;
	}
	/*---Eleghos an oi sxeseis panw stis opoies ginetai to join yparxoun---*/
	if(UT_relInfo(argv[argc - 5],&relrecord1, &attributes1) < 0) 
	{
		printf("Relation with name %s does not exist\n",argv[argc - 5]);
		return -1;
	}

	if(UT_relInfo(argv[argc - 2],&relrecord2, &attributes2) < 0)
	{
		printf("Relation with name %s does not exist\n",argv[argc - 2]);
		return -1;
	}
	//Gia na ginei join prepei toulaxiston mia apo tis sxeseis na einai prwtarxikh
	id1 = giveviewcatrecord(viewrecord,argv[argc-5]);
	id2 = giveviewcatrecord(viewrecord,argv[argc-2]);
	if(id1 >= 0 && id2 >= 0)
	{
		printf("Join between 2 views is forbidden\n");
		return -1;
	}
	attrcnt1 = relrecord1.attrcnt;			
	attrcnt2 = relrecord2.attrcnt;
	attrno = (int)atoi(argv[2]);
	/*Eleghos oi probalomenes sxeseis einai mia apo aytes pou symmetexoun sto join kai an ta pronballomena pedia ontws anhkoun se aytes*/ 
	for(l = 0, i = 4; l < attrno; l++, i += 2)
	{
		flag = 0;
		if(strcmp(argv[i-1],relrecord1.relname) == 0)
		{
			
			for(k = 0; k < attrcnt1; k++)
			{
				
				if(strcmp(argv[i],attributes1[k].attrname) == 0)
				{
					
					flag = 1;
					break;
				}
			}
			if(flag == 0)
			{
				printf("Attribute with name %s does not belong to one of the join relations\n",argv[i]);
				return -1;
			}
			
		}
		else if(strcmp(argv[i-1],relrecord2.relname) == 0)
		{
			for(k = 0; k < attrcnt2; k++)
			{
				if(strcmp(argv[i],attributes2[k].attrname) == 0)
				{
					flag = 1;
					break;
				}
			}
			if(flag == 0)
			{
				printf("Attribute with name %s does not belong to one of the join relations\n",argv[i]);
				return -1;
			}
			
		}
		else
		{
			printf("The projected relation/relations differ from the joined relations\n");
			return -1;
		}
	}
	/*-------Kataskeyh orismatwn ths join--------*/
	if(construct_argument(&argument,argv,argc,1) != VWE_OK) 
	{
		printf("Error in construct_argument called on VW_CreateSelect\n");
		return -1;
	}

	if(DM_join(argc,argument) != DME_OK)
	{
		printf("Error in DM_select called on VW_CreateSelect\n");
		return -1;
	}
	free_argument(&argument);
	
	if((op = find_operator(argv[argc - 3])) == -1)
	{
		printf("Invalid operator\n");
		return -1;
	}
	/*-----------------Enhmerwsh tou viewCat kai viewattrCat-------------------------*/
	if(insertInViewCat(argv[1],2,argv[argc - 5],argv[argc - 4],op,argv[argc - 2],argv[argc - 1],"NULL",attrno) < 0) 
	{
		printf("Error in insertInviewCat called on VW_CreateSelect\n");
		return -1;
	}

	for(l = 0,i = 4; l < attrno ; l++,i+=2)
	{
		if(insertInViewAttrCat(argv[1],argv[i],argv[i-1],argv[i]) <0)
		{
			printf("Error in insertInViewAttrCat\n");
			return -1;
		}
	}

	return VWE_OK;
	
}

int VW_destroy(int argc,char *argv[])
{
	int recid1;
	char viewrecord[VIEWCATSIZE];
	char **argument;
	relDesc relrecord;
	attrDesc* attributes;

	/*----Eleghos an h sxesh h h opsh pros katastrofh yparxei----*/
	if(!strcmp(argv[0],"destroy")) 
	{
		if(UT_relInfo(argv[1],&relrecord, &attributes) < 0)
		{
			printf("Relation with name %s does not exist\n",argv[3]);
			return -1;
		}
		if((recid1 = giveviewcatrecord(viewrecord,argv[1])) >= 0)
		{
			printf("The name you gave belongs to a view.To destroy it give the appropiate order\n");
			return -1;
		}
		if(recid1 == -1)
		{
			printf("Error in giveviewcatrecord\n");
			return -1;
		}
	
	}
	else
	{
		if((recid1 = giveviewcatrecord(viewrecord,argv[1])) < 0)
		{
			if(recid1 == -2)printf("View with name %s does not exist\n",argv[1]);
			else printf("Error in giveviewcatrecord\n");
			return -1;
		}	
	}
	/*-------------Eleghos an yparxoun opseis pou na basizontai panw se ayth th sxesh h opsh-----------*/
	if(existing_views(argv[1]) == 1)
	{
		printf("Cannot destroy the relation given since there is a view based on it\n");
		return -1;
	}
	if(construct_argument(&argument,argv,argc,2) != VWE_OK) 
	{
		printf("Error in construct_argument called on VW_Destroy\n");
		return -1;
	}

	if(UT_destroy(argc,argument) != UTE_OK)
	{
		printf("Error in UT_destroy called on VW_Destroy\n");
		return -1;
	}
		
	free_argument(&argument);
	/*----An exoume katastrofh opshs enhmerwsh viewCat kai viewattrCat-----*/
	if(!strcmp(argv[0],"destroyMView"))
	{
		if(HF_DeleteRec(viewcatFileDesc,recid1,VIEWCATSIZE) != HFE_OK)
		{
			HF_PrintError("Error in UT_destroy called on VW_Destroy\n");
			return -1;
		}
		
		if(delete_viewattrs(argv[1]) != OK)
		{
			printf("Error in delete_viewattrs called on VW_Destroy\n");
			return -1;
		}

	}

	return VWE_OK;	
	
}

int VW_insert(int argc,char *argv[])
{
	int recid,attrcnt,relwidth,f_recId;
	char viewrecord[VIEWCATSIZE],record[VIEWCATSIZE];
	char **argument,**insargument,relname1[MAXNAME],relname2[MAXNAME],viewname[MAXNAME],**add_argv,**destroy_argv;
	int typos;
	relDesc relrecord;
	attrDesc* attributes;

	/*---------Eleghos an h sxesh yparxei----------*/
	if(UT_relInfo(argv[1],&relrecord, &attributes) < 0)  
	{
		printf("Relation with name %s does not exist\n",argv[1]);
		return -1;
	}
	/*--------Eleghos an h sxesh einai prwtarxikh--------*/
	if((recid = giveviewcatrecord(viewrecord,argv[1])) >= 0) 
	{
		printf("The relation given is not primal\n");
		return -1;
	}	
	if(recid == -1 )
	{
		printf("Error in giveviewcatrecord called on VW_Insert\n");
		return -1;
	}
	
	attrcnt = relrecord.attrcnt;
	relwidth = relrecord.relwidth;
	
	if(attrcnt != ((argc-2)/2))     //attributes pou dothhkan ligotera apo ayta ths sxeshs
	{
		printf("VW_insert: Wrong number of attributes given\n");
		return -1;
	}
	/*--------kataskeyh enos proswrinou arxeiou opou tha baloume thn eggrafh pros eisagwgh------*/
	if(construct_create_arg(&argument,argv,argc,"temp") < 0)
	{
		printf("Error in construct_create_arg called on VW_Insert\n");
		return -1;
	}
	if(UT_create(argc,argument) != UTE_OK)
	{
		printf("Error in UT_create called on VW_Insert\n");
		return -1;
	}
	free_args(&argument,argc);
	if(construct_insert_arg(&insargument,argv,argc,"temp") != VWE_OK)
	{
		printf("Error in construct_insert_arg called on VW_Insert\n");
		return -1;
	}
	if(DM_insert(argc,insargument) != DME_OK)
	{
		printf("Error in DM_insert called on VW_Insert\n");
		return -1;
	}
	free_insert_argument(&insargument);
	/*Gia kathe opsh pou yparxei sto viewCat koitame an basizetai sth sxesh pou kanoume thn eisagwgh kai an nai kaloume thn update_views*/
	if((f_recId = HF_GetFirstRec(viewcatFileDesc,record,VIEWCATSIZE)) < 0 && f_recId != HFE_EOF)
	{
		HF_PrintError("Error in HF_GetFirstRec called on VW_Insert\n");
		return -1;
	}

	while(f_recId != HFE_EOF)
	{
		memcpy(&typos,record + MAXNAME,sizeof(int));
		if(typos == 0 || typos == 1)
		{
			memcpy(relname1,record + MAXNAME + sizeof(int),MAXNAME);
			if(!strcmp(relname1,argv[1]))
			{
				memcpy(viewname,record,MAXNAME);
				if(update_views(viewname,argv[1],"temp",0) != OK)
				{
					printf("Error in update_views called on VW_Insert\n");
					return -1;
				}

			}
		}
		else if(typos == 2)
		{
			memcpy(relname1,record + MAXNAME + sizeof(int),MAXNAME);
			memcpy(relname2,record + 3*MAXNAME + sizeof(int) + sizeof(int),MAXNAME);
			if(!strcmp(relname1,argv[1]) || !strcmp(relname2,argv[1]))
			{
				memcpy(viewname,record,MAXNAME);
				if(update_views(viewname,argv[1],"temp",0) != OK)
				{
					printf("Error in update_views called on VW_Insert\n");
					return -1;
				}
			}

		}
		f_recId = HF_GetNextRec(viewcatFileDesc,f_recId,record,VIEWCATSIZE);
		if(f_recId <0 && f_recId != HFE_EOF)
		{
			HF_PrintError("Error in HF_GetNextRec called on existing_views\n");
			return -1;
		}
		
	}
	/*--------Kanoume add tis eggrafes tou temp arxeiou sth sxesh mas-------*/
	if(construct_add_arg(&add_argv,3,argv[1],"temp") != OK)
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
	/*--------Katastrefoume to temp arxeio-------*/
	if(construct_destroy_arg(&destroy_argv,2,"temp") != OK)
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
	return VWE_OK;
}

int VW_delete(int argc,char *argv[])
{
	int recid,attrcnt,relwidth,f_recId,k,arg_num;
	char viewrecord[VIEWCATSIZE],record[VIEWCATSIZE];
	char **selargument,relname1[MAXNAME],relname2[MAXNAME],viewname[MAXNAME],**sub_argv,**destroy_argv;
	int typos;
	relDesc relrecord;
	attrDesc* attributes;

	/*---------Eleghos an h sxesh yparxei----------*/
	if(UT_relInfo(argv[1],&relrecord, &attributes) < 0)  
	{
		printf("Relation with name %s does not exist\n",argv[1]);
		return -1;
	}
	/*---------Eleghos an h sxesh einai prwtarxikh----------*/
	if((recid = giveviewcatrecord(viewrecord,argv[1])) >= 0)
	{
		printf("The relation given is not primal\n");
		return -1;
	}
	if(recid == -1)
	{
		printf("Error in giveviewcatrecord called on VW_Delete\n");
		return -1;
	}

	attrcnt = relrecord.attrcnt;
	relwidth = relrecord.relwidth;
	/*---------Dhmiourgia enos proswrinou arxeiou pou periexei tis eggrafes pou prepei na diagrafoun apo th sxesh--------*/
	//To arxeio ayto dhmiourgeitai me xrhsh select into pou einai aplh probolh olwn twn pediwn ths sxeshs gia delete
	//xwris synthkh kai select me synthhkh gia delete me synthkh opou pali proballontai ola ta pedia tis sxeshs 
	if(argv[2] == NULL)
	{
		arg_num = 3 + 2*relrecord.attrcnt;
		if(select_arg(&selargument,arg_num,argv[1],"temp") != VWE_OK)
		{
			printf("Error in select_arg called on VW_Insert\n");
			return -1;
		}
	}
	else
	{
		for(k = 0;k < attrcnt; k++)
		{
			if(strcmp(attributes[k].attrname,argv[2]) != 0) break;
				
		}
		if(k == attrcnt)
		{
			printf("Attribute with name %s does not belong to the relation projected\n",argv[2]);
			return -1;
		}
		arg_num = 3 + 2*relrecord.attrcnt + 4;
		if(select_cond_arg(&selargument,arg_num,argv[1],"temp",argv[argc-3],argv[argc-2],argv[argc-1]) != VWE_OK)
		{
			printf("Error in select_arg called on VW_Insert\n");
			return -1;
		}
	}
	if(DM_select(arg_num,selargument) != DME_OK)
	{
		printf("Error in DM_select called on VW_delete\n");
		return -1;
	}
	free_args(&selargument,arg_num);
	/*---Gia kathe opsh tou viewCat koitame an basizetai sth sxesh apo opou diagrafoume kai an nai kaloume update_views---*/ 
	if((f_recId = HF_GetFirstRec(viewcatFileDesc,record,VIEWCATSIZE)) < 0 && f_recId != HFE_EOF)
	{
		HF_PrintError("Error in HF_GetFirstRec called on VW_Insert\n");
		return -1;
	}
	while(f_recId != HFE_EOF)
	{
		memcpy(&typos, record + MAXNAME, sizeof(int));
		if(typos == 0 || typos == 1)
		{
			memcpy(relname1,record + MAXNAME + sizeof(int),MAXNAME);
			if(!strcmp(relname1,argv[1]))
			{
				memcpy(viewname,record,MAXNAME);
				if(update_views(viewname,argv[1],"temp",1) != OK)
				{
					printf("Error in update_views called on VW_Delete\n");
					return -1;
				}
			}
		}
		else if(typos == 2)
		{
			memcpy(relname1,record + MAXNAME + sizeof(int),MAXNAME);
			memcpy(relname2,record + 3*MAXNAME + sizeof(int) + sizeof(int),MAXNAME);
			if(!strcmp(relname1,argv[1]) || !strcmp(relname2,argv[1]))
			{
				memcpy(viewname,record,MAXNAME);
				if(update_views(viewname,argv[1],"temp",1) != OK)
				{
					printf("Error in update_views called on VW_Delete\n");
					return -1;
				}
			}

		}
		f_recId = HF_GetNextRec(viewcatFileDesc,f_recId,record,VIEWCATSIZE);
		if(f_recId <0 && f_recId != HFE_EOF)
		{
			HF_PrintError("Error in HF_GetNextRec called on existing_views\n");
			return -1;
		}
		
	}
	/*------------Diagrafh eggrafwn proswrinou arxeiou apo th sxesh mas-------*/
	if(construct_sub_arg(&sub_argv,3,argv[1],"temp") != OK)
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
	/*-------------Katastrofh proswrinou arxeiou---------*/
	if(construct_destroy_arg(&destroy_argv,2,"temp") != OK)
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
	return VWE_OK;
	
}
