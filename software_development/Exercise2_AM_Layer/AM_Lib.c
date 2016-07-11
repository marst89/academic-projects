/*-----------------------------------ANAPTYKSH LOGISMIKOY---------------------------------------------------------*/
/*------------------------------------2o MEROS- AM_Lib.c----------------------------------------------------------*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Gia thn ergasia ayth ergasthkan oi foithtes :~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
1)Karaiskos Xrhstos me A.M :1115200500063
2)Poutachidou Maria me A.M :1115200600063
3)Tsirikou Dimitra  me A.M :1115200600153
###################################################################################################################
H askhsh ayth dokimasthke epityxws sta mhxanhmata linux ths sxolhs!!!!!
Gia th metaglwttish kai ektelesh ayths ths askhshs ayths plhktrologoume tis entoles:
gcc -c main1.c
gcc -c AM_Lib.c
gcc -c helpful.c
gcc -o ask main1.o AM_Lib.o BF_Lib.o HF_Lib.o helpful.o -lm
./ask
gcc -c main2.c
gcc -o ask main2.o AM_Lib.o BF_Lib.o HF_Lib.o helpful.o -lm
./ask
gcc -c main3.c
gcc -o ask main3.o AM_Lib.o BF_Lib.o HF_Lib.o helpful.o -lm
./ask
to helpful.c einai to phgaio arxeio mas me vohthitikes synarthseis gia to xeirismo twn bits kai epishs synarthseis opws h hash function ktl...
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "AM_Lib.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void AM_Init( void )
{
	int i;
	
	for(i = 0; i < MAXSCANS; i++)
	{
		Scans[i].scan_open = 0;
		Scans[i].last_rec = -1;
		Scans[i].cur_file = -1;
		Scans[i].operation  = 0;
		Scans[i].value = NULL;
		Scans[i].type = ' ';
		Scans[i].length = -1;
		Scans[i].pointer = -1;
	}
	
	BF_Init();
}

//--------------------------------------------------------------------------------------------
int AM_CreateIndex( char *fileName, int indexNo, char attrType, int attrLength)
{
	int id,blockNum,D = 1,ret;
	char newname[AM_MAX_FILE_NAME],*blockBuf;
	
	ret = type_check(attrType,attrLength);

	if(ret != AME_OK)
	{
		return ret;
	}

	sprintf(newname,"%s.%d",fileName,indexNo);	//dhmioyrgia tou onomatos tou deutereuontos eurethriou		
	if((BF_CreateFile(newname)) != BFE_OK)		//dhmiourgia tou deutereuontos eurethriou
	{
		AM_errno = ERRORINBFLEVEL;
		BF_PrintError("Sfalma sthn BF_CreateFile");
		return AM_errno;
	}
	if((id = BF_OpenFile(newname)) < 0)		//anoigma tou deutereuontos eurethriou
	{
		AM_errno = ERRORINBFLEVEL;
		BF_PrintError("Sfalma sthn BF_OpenFile");
		return AM_errno;
	}
	if((BF_AllocBlock(id,&blockNum,&blockBuf)) < 0)	//dhmiourgia tou pinaka epektatou katakermatismou tou deutereuontos eurethriou(header AM epipedou)
	{
		AM_errno = ERRORINBFLEVEL;
		BF_PrintError("Sfalma sthn BF_AllocBlock");
		return AM_errno;
	}
	if((BF_UnpinBlock(id,blockNum,TRUE)) != BFE_OK)	
	{
		AM_errno = ERRORINBFLEVEL;
		BF_PrintError("Sfalma sthn BF_UnpinBlock");
		return AM_errno;
	}
	if((BF_CloseFile(id)) != BFE_OK)
	{
		AM_errno = ERRORINBFLEVEL;
		BF_PrintError("Sfalma sthn BF_CloseFile");
		return AM_errno;
	}

	return AME_OK;
}

//-------------------------------------------------------------------------------------------
int AM_DestroyIndex( char *fileName, int indexNo )
{
	char newname[AM_MAX_FILE_NAME];

	sprintf(newname,"%s.%d",fileName,indexNo);
	if((BF_DestroyFile(newname)) != BFE_OK)
	{
		AM_errno = ERRORINBFLEVEL;
		BF_PrintError("Sfalma sthn BF_DestroyFile");
		return AM_errno;
	}

	return AME_OK;
}

//--------------------------------------------------------------------------------------------
int AM_OpenIndex( char *fileName, int indexNo )
{
	char newname[AM_MAX_FILE_NAME];
	int id;
	
	sprintf(newname,"%s.%d",fileName,indexNo);
	if((id = BF_OpenFile(newname) )< 0)
	{
		AM_errno = ERRORINBFLEVEL;
		BF_PrintError("Sfalma sthn BF_OpenFile");
		return AM_errno;
	}
	
	return id;
}
//--------------------------------------------------------------------------------------------
int AM_CloseIndex( int fileDesc )
{
	if((BF_CloseFile(fileDesc)) != BFE_OK)
	{
		AM_errno = ERRORINBFLEVEL;
		BF_PrintError("Sfalma sthn BF_CloseFile");
		return AM_errno;
	}
	
	return AME_OK;
}
//--------------------------------------------------------------------------------------------
int AM_InsertEntry( int fileDesc, char attrType, int attrLength, char *value, int recId )
{
	int D,HeaderNum,recSize,maxRecs,place,d,newNum,k,j,array_size,m,newid,l,flag = 0,temp_int,ret,ins;
	unsigned int hash,newhash;	
	short int pointer,short_no,newshort,temp_short,no,number;
	char *HeaderBuf,*blockBuf,*newblock,*bitmap,*newbitmap,nvalue[attrLength],*i;
 	char pointers[BF_BLOCK_SIZE];
	int eeee;

	ret = type_check(attrType,attrLength);

	if(ret != AME_OK)
	{
		return ret;
	}

	recSize = attrLength + sizeof(int);	//ypologismos megethous eggrafhs
	maxRecs = (int)((float)(8*(BF_BLOCK_SIZE - sizeof(int)))/ (float)(8*recSize + 1));	//megistos aritmos eggrafwn pou xwraei sto arxeio
	
	if((BF_GetThisBlock(fileDesc,0,&HeaderBuf)) != BFE_OK)	//fernoume to header tou AM epipedou-pinaka epektatou katakermatismou
	{
		BF_PrintError("Sfalma stin GetThisBlock");
		AM_errno = ERRORINBFLEVEL;
		return AM_errno;
	}

	memcpy(&D,HeaderBuf,sizeof(int));	//apothikeuoume to oliko vathos sto D
	if(D == 0)//to oliko vathos einai 0(mhden)
	{
		memcpy(&pointer,HeaderBuf + sizeof(int),sizeof(short int));
		if(pointer == 0)//den exei dhmiourgithei akoma kados 
		{
			if((BF_AllocBlock(fileDesc,&temp_int,&blockBuf)) != BFE_OK)//dhmiourgoume ton 1o kado gia apothikeysh twn eggrafwn
			{
				BF_PrintError("Sfalma stin BF_Alloc");
				AM_errno = ERRORINBFLEVEL;
				return AM_errno;
			}
			pointer = (short int) temp_int;
			memcpy(HeaderBuf + sizeof(int),&pointer,sizeof(short int));	//enhmerwnoume ton pinaka tou epektatou me ton arithmo pou 
			//antistoixei ston kouva pou ferame
		}
		else//exei hdh dhmiourghthei o prwtos kados
		{

			temp_int = (int)pointer; 
			if((BF_GetThisBlock(fileDesc,temp_int,&blockBuf)) != BFE_OK)//ton fernoume sth mnhmh
			{
				BF_PrintError("Sfalma stin BF_GetThisBlock");
				AM_errno = ERRORINBFLEVEL;
				return AM_errno;
			}
		}
	}
	else//to oliko vathos einai diaforo tou mhdenos
	{
		hash = JSHash(value,attrLength);	
		short_no = find_bucket(D,hash);		//vriskoume th thesh tou pinaka epektatou katakermatismou apo ton opoio diavazoume ton arithmo tou
							// kadou pou tha feroume sth mnhmh
		memcpy(&pointer,HeaderBuf + sizeof(int) + short_no * sizeof(short int),sizeof(short int));
		temp_int = (int)pointer; 
		if((BF_GetThisBlock(fileDesc,temp_int,&blockBuf)) != BFE_OK)
		{
			BF_PrintError("Sfalma stin BF_GetThisBlock");
			AM_errno = ERRORINBFLEVEL;
			return AM_errno;
		}
	}
	
	bitmap = blockBuf + BF_BLOCK_SIZE - (int) ceil((float)maxRecs / (float)8);
	if((place = AM_find_in_bitmap(bitmap,0,maxRecs,0))== -1) //an den vrethike kenh thesh gia thn eisagwgh ths eggrafhs
	{
		memcpy(&d,blockBuf,sizeof(int));	//apothikeuoume sto d to topiko vathos
		if(D == 8)				//o pinakas tou epektatou katakermatismou exei ftasei sto megisto megethos tou
		{
			if((BF_UnpinBlock(fileDesc,0,TRUE)) != BFE_OK)
			{
				BF_PrintError("Sfalma stin BF_UnpinBlock");
				AM_errno = ERRORINBFLEVEL;
				return AM_errno;
			}
			if((BF_UnpinBlock(fileDesc,temp_int,TRUE)) != BFE_OK)
			{
				BF_PrintError("Sfalma stin BF_UnpinBlock");
				AM_errno = ERRORINBFLEVEL;
				return AM_errno;
			}
			AM_errno = MAXDEPTH; 
			return AM_errno;
		}
		if(d == D)	//to topiko vathos iso me to oliko ----->diplasiasmos tou pinaka epektatou katakermatismou
		{
			D++;	//auksanoume to oliko vathos 
			memset(pointers,0,BF_BLOCK_SIZE);	//arxikopoioume sto mhden to vohthiko pinaka pou tha xrhsimopoihsoume 
								//gia to diplasiasmo twn deiktwn
			memcpy(pointers,&D,sizeof(int));
			for(j = 0,l=0; j < (int)pow(2,D - 1); j++,l+=2)//diplasiazoume tous deiktes kai tous apothikeuoume ston vohthitiko pinaka
			{
				memcpy(pointers + sizeof(int) + l * sizeof(short int),HeaderBuf + sizeof(int) + j*sizeof(short int),sizeof(short int));
				memcpy(pointers + sizeof(int) +(l+1)*sizeof(short int),HeaderBuf + sizeof(int) + j*sizeof(short int),sizeof(short int));
			}
			memcpy(HeaderBuf,pointers,BF_BLOCK_SIZE);	//antigrafoume to vohthitiko pinaka ston pinaka epektatou
		}
			//d<D
		k = D - d;
		d++;	//auksanoume to topiko vathos
		if((BF_AllocBlock(fileDesc,&newNum,&newblock)) != BFE_OK)	//dhmiourgoume neo block 
		{
			BF_PrintError("Sfalma stin BF_Alloc");
			AM_errno = ERRORINBFLEVEL;
			return AM_errno;
		}
		memcpy(blockBuf,&d,sizeof(int));
		memcpy(newblock,&d,sizeof(int));
		array_size = (int)pow(2,D);
		for(j = 0;j < array_size; j++)	//anamoirasmos twn deiktwn metaksy tou paliou kai tou neou block
		{
			memcpy(&no,HeaderBuf + sizeof(int) + j*sizeof(short int),sizeof(short int));
			if(no == pointer)
			{
				for(m = 0; m < (int)pow(2,k-1); m++)
				{
					temp_short = (short int)newNum;
					memcpy(HeaderBuf + sizeof(int) + (j + (int)pow(2,k-1) + m)*sizeof(short int),&temp_short,sizeof(short int));	
				}
				break;	
			}	
		}
		for(j = 0; j < maxRecs; j++)	//anamoirasmos twn eggrafwn pou yphrxan ston palio kado
		{
			memcpy(nvalue,blockBuf + sizeof(int) + j*recSize,attrLength);
			newhash = JSHash(nvalue,attrLength);
			newshort = find_bucket(D,newhash);
			memcpy(&number,HeaderBuf + sizeof(int) + newshort*sizeof(short int),sizeof(short int));
			if(number != pointer)
			{	
				memcpy(&newid,blockBuf + sizeof(int) + j*recSize + attrLength,sizeof(int));
				newbitmap = newblock + BF_BLOCK_SIZE -(int) ceil((float)maxRecs / (float)8);
				place = AM_find_in_bitmap(newbitmap,0,maxRecs,0);					
				i = newblock + sizeof(int) + place * recSize;
				//eisagwgi tis eggrafis
				memcpy(i,nvalue,attrLength);
				memcpy(attrLength + i,&newid,sizeof(int));
				AM_unpinbit(bitmap,j,maxRecs);
				AM_pinbit(newbitmap,place,maxRecs);
			}
		}
		if((BF_UnpinBlock(fileDesc,0,TRUE)) != BFE_OK)
		{
			BF_PrintError("Sfalma stin BF_UnpinBlock");
			AM_errno = ERRORINBFLEVEL;
			return AM_errno;
		}
		if((BF_UnpinBlock(fileDesc,temp_int,TRUE)) != BFE_OK)
		{
			BF_PrintError("Sfalma stin BF_UnpinBlock");
			AM_errno = ERRORINBFLEVEL;
			return AM_errno;
		}
		if((BF_UnpinBlock(fileDesc,newNum,TRUE)) != BFE_OK)
		{
			BF_PrintError("Sfalma stin BF_UnpinBlock");
			AM_errno = ERRORINBFLEVEL;
			return AM_errno;
		}
		//kaloume anadromika thn insert gia na eisagoume th nea eggrafh
		ins = AM_InsertEntry(fileDesc,attrType,attrLength,value,recId );
		return ins;			
	}	
	//eisagwgi neas eggrafis
	memcpy(blockBuf + sizeof(int) + place * recSize,value,attrLength);
	memcpy(attrLength + blockBuf + sizeof(int) + place * recSize,&recId,sizeof(int));
	AM_pinbit(bitmap,place,maxRecs);
	if((BF_UnpinBlock(fileDesc,0,TRUE)) != BFE_OK)
	{
		BF_PrintError("Sfalma stin BF_UnpinBlock");
		AM_errno = ERRORINBFLEVEL;
		return AM_errno;
	}
	if((BF_UnpinBlock(fileDesc,temp_int,TRUE)) != BFE_OK)
	{
		BF_PrintError("Sfalma stin BF_UnpinBlock");
		AM_errno = ERRORINBFLEVEL;
		return AM_errno;
	}

	return 	AME_OK;
}

//--------------------------------------------------------------------------------------------
int AM_DeleteEntry( int fileDesc, char attrType, int attrLength, char *value, int recId )
{
	int i,D,place,k = 0,recSize,maxRecs,id_read,flag = 0,HeaderNum,ret,temp_int;
	short int short_no,pointer;
	unsigned int hash;
	char *blockBuf,*bitmap,value_read[attrLength],*HeaderBuf;

	ret = type_check(attrType,attrLength);

	if(ret != AME_OK)
	{
		return ret;
	}

	recSize = attrLength + sizeof(int);
	maxRecs = 8*(BF_BLOCK_SIZE - sizeof(int)) / (8*recSize + 1);

	if((BF_GetFirstBlock(fileDesc,&HeaderNum,&HeaderBuf)) != BFE_OK)
	{
		BF_PrintError("Sfalma stin GetFirstBlock");
		AM_errno = ERRORINBFLEVEL;
		return AM_errno;
	}	

	memcpy(&D,HeaderBuf,sizeof(int));
	//vriskoume poio kado prepei na feroume sth mnhmh analoga me to topiko vathos
	if (D == 0)		
	{
		temp_int=1;
	}
	else
	{
		hash = JSHash(value,attrLength);
		short_no = find_bucket(D,hash);
		memcpy(&pointer,HeaderBuf + sizeof(int) + short_no*sizeof(short int),sizeof(short int));
		temp_int = (int)pointer; 
	}	
	if((BF_GetThisBlock(fileDesc,temp_int,&blockBuf)) != BFE_OK)
	{
		BF_PrintError("Sfalma stin BF_GetThisBlock");
		AM_errno = ERRORINBFLEVEL;
		return AM_errno;
	}

	bitmap = blockBuf + BF_BLOCK_SIZE -(int) ceil((float)maxRecs / (float)8);

	while((place = AM_find_in_bitmap(bitmap,k,maxRecs,1)) != -1)//oso yparxoun eggrafes ston kado
	{
		memcpy(value_read,blockBuf + sizeof(int) + place * recSize,attrLength);
		memcpy(&id_read,blockBuf + sizeof(int) + place * recSize + attrLength,sizeof(int));
		if(!memcmp(value_read,value,attrLength))//sygkrinoume thn trexousa timh me th dotheisa
		{
			if(id_read == recId)
			{
				AM_unpinbit(bitmap,place,maxRecs);
				flag = 1;	
			}
		}
		k = place + 1;	
	}

	if((BF_UnpinBlock(fileDesc,HeaderNum,TRUE)) != BFE_OK)
	{
		AM_errno = ERRORINBFLEVEL;
		return AM_errno;
	}
	if((BF_UnpinBlock(fileDesc,pointer,TRUE)) != BFE_OK)
	{
		AM_errno = ERRORINBFLEVEL;
		return AM_errno;
	}

	if(flag == 0)//den vrethike kamia eggrafi
	{
		AM_errno = REC_NOT_FOUND;
		return AM_errno;
	}

	return AME_OK;
	
}
//-------------------------------------------------------------------------------------------
int AM_OpenIndexScan( int fileDesc, char attrType, int attrLength, int op, char *value )
{
	int i = 0,ret;
	
	ret = type_check(attrType,attrLength);

	if(ret != AME_OK)
	{
		return ret;
	}

	while(i < MAXSCANS)
	{
		if(Scans[i].scan_open == 0)
		{
			Scans[i].scan_open = 1;
                	Scans[i].last_rec = -1;
                	Scans[i].cur_file = fileDesc;
                	Scans[i].operation  = op;
			if( op != EQUAL && op != NOT_EQUAL )
			{
				AM_errno = UNAVAILABLEOP;
				return AM_errno;
			}
			if(value != NULL)
			{
				if ( (Scans[i].value = malloc(attrLength * sizeof(char)) ) == NULL)
				{
					AM_errno = OS_FAILURE;
					return AM_errno;
				}
                		memcpy(Scans[i].value,value,attrLength);
			}
                	Scans[i].type = attrType;
                	Scans[i].length = attrLength;
			Scans[i].pointer = 0;
			break;
		}
		i++;
	}

	if(i == MAXSCANS)
	{
		AM_errno = SCANSTABLEFULL;
		return AM_errno;
	}

	return i;
}

//-------------------------------------------------------------------------------------------
int AM_FindNextEntry( int scanDesc )
{
	int D,place,HeaderNum,curNum,maxRecs,recSize,recid,k,d,currec,temp_int,eeee;
	short int pointer,short_no,blockNum,newNum,j,no,hash_bucket;
	unsigned int hash;
	char *blockBuf,*bitmap,*newbitmap,value_read[Scans[scanDesc].length],*HeaderBuf,*newBuf;

	if(scanDesc < 0 || scanDesc > 19)
	{
		AM_errno = INVALIDSCANDESC;
		return AM_errno;
	}

	if((BF_GetFirstBlock(Scans[scanDesc].cur_file,&HeaderNum,&HeaderBuf)) != BFE_OK)
	{
		BF_PrintError("Sfalma stin GetFirstBlock");
		AM_errno = ERRORINBFLEVEL;
		return AM_errno;
	}

	recSize = Scans[scanDesc].length + sizeof(int);
	maxRecs = 8*(BF_BLOCK_SIZE - sizeof(int)) / (8*recSize + 1);
	//--------------------------------------------------NULL-------------------------------------------------------
	if(Scans[scanDesc].value == NULL)
	{
		no=Scans[scanDesc].pointer;	//kratame th thesh ston pinaka katakermatismou pou eixe meinei h prohgoumenh sarwsh 
		currec=Scans[scanDesc].last_rec;//kratame ton arithmo ths teleytaias eggrafhs pou eixe meinei h prohgoumenh sarwsh
		memcpy(&D,HeaderBuf,sizeof(int));
		while(no < (short int)pow(2,D))//oso den exoun teleiwsei oi kouvades
		{
			memcpy(&blockNum,HeaderBuf + sizeof(int) + no * sizeof(short int),sizeof(short int));
			temp_int = (int)blockNum;
			if((BF_GetThisBlock(Scans[scanDesc].cur_file,temp_int,&blockBuf)) != BFE_OK)//fernoume ton kado sth mnhmh
			{
				BF_PrintError("Sfalma stin BF_GetThisBlock");
				AM_errno = ERRORINBFLEVEL;
				return AM_errno;
			}
			bitmap = blockBuf + BF_BLOCK_SIZE -(int) ceil((float)maxRecs / (float)8);
			if((place = AM_find_in_bitmap(bitmap,currec+1,maxRecs,1)) != -1)//an yparxei eggrafh sto bitmap meta thn trexousa
			{
				memcpy(&recid,blockBuf + sizeof(int) + place * recSize + Scans[scanDesc].length,sizeof(int));
				Scans[scanDesc].last_rec = place;	//kratame ton arithmo ths trexousas eggrafhs
				Scans[scanDesc].pointer = no;
				if((BF_UnpinBlock(Scans[scanDesc].cur_file,temp_int,FALSE)) != BFE_OK)
				{
					BF_PrintError("Sfalma sthn BF_UnpinBlock");
					AM_errno = ERRORINBFLEVEL;
					return AM_errno;
				}
				if((BF_UnpinBlock(Scans[scanDesc].cur_file,HeaderNum,FALSE)) != BFE_OK)
				{
						BF_PrintError("Sfalma sthn BF_UnpinBlock");
						AM_errno = ERRORINBFLEVEL;
						return AM_errno;
				}
				return recid;
			}
			if((BF_UnpinBlock(Scans[scanDesc].cur_file,temp_int,FALSE)) != BFE_OK)
			{
				BF_PrintError("Sfalma sthn BF_UnpinBlock");
				AM_errno = ERRORINBFLEVEL;
				return AM_errno;
			}
			memcpy(&d,blockBuf,sizeof(int));
			k = D - d;
			no = no + (short int)pow(2,k); //pame sto deikth tou pinaka pou deixnei ston epomeno kado
			currec=-1;
		}
		if((BF_UnpinBlock(Scans[scanDesc].cur_file,HeaderNum,FALSE)) != BFE_OK)
		{
			BF_PrintError("Sfalma sthn BF_UnpinBlock");
			AM_errno = ERRORINBFLEVEL;
			return AM_errno;
		}
	
		return AME_EOF;	//exoun teleiwsei oles oi eggrafes tou deutereuontos eurethriou
			
	}
	//--------------------------------------------------EQUAL-------------------------------------------------------
	if(Scans[scanDesc].operation == EQUAL)
	{
		memcpy(&D,HeaderBuf,sizeof(int));
		if (D == 0)
		{
			temp_int=1;
		}
		else
		{
			hash = JSHash(Scans[scanDesc].value,Scans[scanDesc].length);
			short_no = find_bucket(D,hash);
			memcpy(&blockNum,HeaderBuf + sizeof(int) + short_no * sizeof(short int),sizeof(short int));
			temp_int = (int)blockNum;
		}
		if((BF_GetThisBlock(Scans[scanDesc].cur_file,temp_int,&blockBuf)) != BFE_OK)
		{
			BF_PrintError("Sfalma stin BF_GetThisBlock");
			AM_errno = ERRORINBFLEVEL;
			return AM_errno;
		}
		bitmap = blockBuf + BF_BLOCK_SIZE -(int) ceil((float)maxRecs / (float)8);
		currec = Scans[scanDesc].last_rec;
		while((place = AM_find_in_bitmap(bitmap,currec + 1,maxRecs,1)) != -1)
		{
			memcpy(value_read,blockBuf + sizeof(int) + place * recSize,Scans[scanDesc].length);
			if(!strcmp(value_read,Scans[scanDesc].value)) //sygkrinoume thn trexousa timh me th dotheisas
			{
				Scans[scanDesc].last_rec = place;
				Scans[scanDesc].pointer = short_no;
				memcpy(&recid,blockBuf + sizeof(int) + place * recSize + Scans[scanDesc].length,sizeof(int));
				if((BF_UnpinBlock(Scans[scanDesc].cur_file,temp_int,FALSE)) != BFE_OK)
				{
					BF_PrintError("Sfalma sthn BF_UnpinBlock");
					AM_errno = ERRORINBFLEVEL;
					return AM_errno;
				}
				if((BF_UnpinBlock(Scans[scanDesc].cur_file,HeaderNum,FALSE)) != BFE_OK)
				{
					BF_PrintError("Sfalma sthn BF_UnpinBlock");
					AM_errno = ERRORINBFLEVEL;
					return AM_errno;
				}
				return recid;
			}
			currec=place;	
		}
		if((BF_UnpinBlock(Scans[scanDesc].cur_file,temp_int,FALSE)) != BFE_OK)
		{
			BF_PrintError("Sfalma sthn BF_UnpinBlock");
			AM_errno = ERRORINBFLEVEL;
			return AM_errno;
		}
		if((BF_UnpinBlock(Scans[scanDesc].cur_file,HeaderNum,FALSE)) != BFE_OK)
		{
			BF_PrintError("Sfalma sthn BF_UnpinBlock");
			AM_errno = ERRORINBFLEVEL;
			return AM_errno;
		}
		AM_errno = AME_EOF;
		return AM_errno;
	}
	//--------------------------------------------------NOT EQUAL------------------------------------------------------
	if(Scans[scanDesc].operation == NOT_EQUAL)
	{
		memcpy(&D,HeaderBuf,sizeof(int));
		if (D == 0)
		{
			temp_int=1;
			no=0;
			short_no=0;
		}
		else
		{
			hash = JSHash(Scans[scanDesc].value,Scans[scanDesc].length);
			short_no = find_bucket(D,hash);
			no=Scans[scanDesc].pointer;
		}
		while(no < (short int)pow(2,D))
		{
			
			memcpy(&blockNum,HeaderBuf + sizeof(int) + no * sizeof(short int),sizeof(short int));
			temp_int = (int)blockNum;
			if((BF_GetThisBlock(Scans[scanDesc].cur_file,temp_int,&blockBuf)) != BFE_OK)
			{
				BF_PrintError("Sfalma stin BF_GetThisBlock");
				AM_errno = ERRORINBFLEVEL;
				return AM_errno;
			}
			bitmap = blockBuf + BF_BLOCK_SIZE -(int) ceil((float)maxRecs / (float)8);
			memcpy(&hash_bucket,HeaderBuf + sizeof(int) + short_no * sizeof(short int),sizeof(short int));
			if (blockNum != hash_bucket)//psaxnoume se olous tous kouvades ektos autou ston opoio anhkei h dotheisa value
			{
				if((place = AM_find_in_bitmap(bitmap,Scans[scanDesc].last_rec + 1,maxRecs,1)) != -1)
				{
					Scans[scanDesc].last_rec = place;
					Scans[scanDesc].pointer = no;
					memcpy(&recid,blockBuf + sizeof(int) + place * recSize + Scans[scanDesc].length,sizeof(int));
					if((BF_UnpinBlock(Scans[scanDesc].cur_file,temp_int,FALSE)) != BFE_OK)
					{
						BF_PrintError("Sfalma sthn BF_UnpinBlock");
						AM_errno = ERRORINBFLEVEL;
						return AM_errno;
					}
					if((BF_UnpinBlock(Scans[scanDesc].cur_file,HeaderNum,FALSE)) != BFE_OK)
					{
						BF_PrintError("Sfalma sthn BF_UnpinBlock");
						AM_errno = ERRORINBFLEVEL;
						return AM_errno;
					}
					return recid;
				}
			}
			else//psaxnoume ston kado pou anhkei h eggrafi mexri na vroume kapoia diaforh
			{
				currec=Scans[scanDesc].last_rec;
				while((place = AM_find_in_bitmap(bitmap,currec + 1,maxRecs,1)) != -1)
				{
					memcpy(value_read,blockBuf + sizeof(int) + place * recSize,Scans[scanDesc].length);
					if(strcmp(value_read,Scans[scanDesc].value))
					{
						Scans[scanDesc].last_rec = place;
						Scans[scanDesc].pointer = no;
						memcpy(&recid,blockBuf + sizeof(int) + place * recSize + Scans[scanDesc].length,sizeof(int));
						if((BF_UnpinBlock(Scans[scanDesc].cur_file,temp_int,FALSE)) != BFE_OK)
						{
							BF_PrintError("Sfalma sthn BF_UnpinBlock");
							AM_errno = ERRORINBFLEVEL;
							return AM_errno;
						}
						if((BF_UnpinBlock(Scans[scanDesc].cur_file,HeaderNum,FALSE)) != BFE_OK)
						{
							BF_PrintError("NOT4 Sfalma sthn BF_UnpinBlock");
							AM_errno = ERRORINBFLEVEL;
							return AM_errno;
						}
						return recid;
					}
					currec++;
				}
			}
			if((BF_UnpinBlock(Scans[scanDesc].cur_file,temp_int,FALSE)) != BFE_OK)
			{
				BF_PrintError("NOT5 Sfalma sthn BF_UnpinBlock");
				AM_errno = ERRORINBFLEVEL;
				return AM_errno;
			}
			memcpy(&d,blockBuf,sizeof(int));
			Scans[scanDesc].last_rec=-1;
			k = D - d;
			no = no + (short int)pow(2,k);	
		}
		if((BF_UnpinBlock(Scans[scanDesc].cur_file,HeaderNum,FALSE)) != BFE_OK)
		{
			BF_PrintError("Sfalma sthn BF_UnpinBlock");
			AM_errno = ERRORINBFLEVEL;
			return AM_errno;
		}
		return AME_EOF;		
	}	
}

//-------------------------------------------------------------------------------------------
int AM_CloseIndexScan( int scanDesc )
{
	if(scanDesc < 0 || scanDesc > 19){	//elegxos an einai egkyro to anagnwristiko tou arxeiou
		AM_errno = INVALIDSCANDESC;
		return AM_errno;
	}
	
	if(Scans[scanDesc].scan_open == 1)
	{
		Scans[scanDesc].scan_open = 0;
                Scans[scanDesc].last_rec = -1;
                Scans[scanDesc].cur_file = -1;
                Scans[scanDesc].operation  = 0;
		if(Scans[scanDesc].value != NULL)
		{
			free(Scans[scanDesc].value);
	                Scans[scanDesc].value = NULL;
		}
                Scans[scanDesc].type = ' ';
                Scans[scanDesc].length = -1;
	}
	else
	{
		AM_errno = ALREADYCLOSEDSCAN;
		return AM_errno;
	}

	return AME_OK;
}
//------------------------------------------------------------------------------------------
void AM_PrintError( char * errString )
{
	fprintf(stderr,"\t%s\n",errString);

	switch(AM_errno)
	{
		case WRONG_LENGTH :
			fprintf(stderr,"\t Sfalma sto Mikos twn Typwn Dedomenwn\n");
			break;	
		case UNAVAILABLEOP :
			fprintf(stderr,"\t Mi Dia8esimos Telestis Sygkrisis\n");		
		case SCANSTABLEFULL :
			fprintf(stderr,"\t O Pinakas Anoixtwn Sarwsewn einai Gematos\n");
			break;
		case INVALIDSCANDESC : 	
			fprintf(stderr,"\t Lan8asmeni 8esi ston Pinaka Anoixtwn Sarwsewn\n");
			break;	
		case ALREADYCLOSEDSCAN : 
			fprintf(stderr,"\t I Sarwsi Einai idi Kleisti\n");
			break;
		case ERRORINBFLEVEL :
			fprintf(stderr,"\t Sfalma sto Epipedo BF\n");
			break;
		case MAXDEPTH :
			fprintf(stderr,"\t O Pinakas Katakermatismou eftase sto Megisto Mege8os tou\n");
			break;						
		case REC_NOT_FOUND :			
			fprintf(stderr,"\t I Eggrafi de Vre8ike\n");
			break;
		case NOMORERECS	:
			fprintf(stderr,"\t Den Yparxoun Alles Eggrafes\n");
			break;
		case OS_FAILURE	:
			fprintf(stderr,"\t Sfalma Leitourgikou Systimatos\n");
			break;
		default :
			fprintf(stderr,"\t Agnwsto Sfalma\n");
			break; 
	}
}

//-----------------------------------------------------------------------------------------
