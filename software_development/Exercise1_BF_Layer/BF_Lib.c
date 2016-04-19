//--------------------------------Anaptiksi Logismikou-----------------------------------//
//-------------------------------------1h askhsh-----------------------------------------//

/*****************************************************************************************
Gia thn ergasia ayth ergasthkan oi foithtes :
	1.Karaiskos Xrhstos  me A.M 1115200500066
	2.Poutachidou Maria  me A.M 1115200600063
	3.Tsirikou Dhmhtra   me A.M 1115200600153
*****************************************************************************************/

#include "BF_Lib.h"

int lru(void)			//epistrefei ti 8esi tou block sto buffer me to mikrotero time
{
	int i,min = timestamp,place;
	
	for(i = 0; i < BF_BUFFER_SIZE; i++)
	{
		if(Buffer[i].is_used == 1 && Buffer[i].pin_counter == 0)
		{
			if(Buffer[i].time <= min)
			{
				place = i;
				min = Buffer[i].time;
			}
		}
	}

	return place;
}

int get_bit_value2(char Byte,int Bit)	//pairnei ena byte kai tin 8esi enos bit kai epistrefei tin timi tou (0 h' 1)	
{
	int m;
	m=Bit%8;
	if((Byte & (char)pow(2,m))==0)
		return 0;
	else
		return 1;
}

int find_in_bitmap(char* bitmap,int position,int bitmap_size,int one_bit)//apo ti 8esi position tou bitmap k meta fernei to prwto mideniko (an one_bit==0)
{									 //i ton prwto asso (an one_bit==1)
	int i,loops,m,q,arxi_i=0,arxi_m=0;
	loops = ceil((float)bitmap_size/8);
	arxi_i=position/8;
	arxi_m=position%8; 
	for(i=arxi_i;i<loops;i++)
	{
		if(i == (loops-1)) q=bitmap_size%8; 	
	 	else q=8;
		for(m=arxi_m; m<q; m++)
		{
			if(get_bit_value2(bitmap[i],m) == one_bit)
			return i*8+m;
		}
		arxi_m=0;
	}
	return -1;
}


void pinbit(char* bitmap,int position)//kanei asso to bit sti 8esi position tou bitmap
{
     int Byte,m;
     Byte=position/8;
     m=position%8;
     bitmap[Byte]=bitmap[Byte] | (char)pow(2,m);
     return;
}

void unpinbit(char* bitmap,int position)//kanei miden to bit sti 8esi position tou bitmap
{
     int Byte,m;
     Byte=position/8;
     m=position%8;
     bitmap[Byte]=bitmap[Byte] & (~(char)pow(2,m));
     return;
}

int get_bit_value(char* bitmap,int position)//pairnei olokliro to bitmap kai tin 8esi enos bit kai epistrefei tin timi tou (0 h' 1)	
{
    int Byte,m;
    Byte=position/8;
    m=position%8;
    if((bitmap[Byte] & (char)pow(2,m))==0)
        return 0;
    else
        return 1;
}

//--------------------------------------------------------------------------------------------------------
void BF_Init(void)
{
	int i;
	
	timestamp = 0;

	for (i=0;i<MAXOPENFILES;i++)				//Arxikopoiisi pinakwn OpenFiles kai Headers
	{
		OpenFiles[i].fp=NULL;				
		OpenFiles[i].header_offset=-1;
		memset(OpenFiles[i].blockmap,0,3);    		
  		memset(Headers[i].bitmap,0,BF_BLOCK_SIZE);
		strcpy(Headers[i].fname,"\0");		
		Headers[i].used = 0;
	}

	for (i=0;i<BF_BUFFER_SIZE;i++)				//Arxikopoiisi Buffer pou periexei ta blocks stin mnimi
	{
		Buffer[i].is_used=0;
		strcpy(Buffer[i].filename,"\0");	
		Buffer[i].block_num=-1;
		Buffer[i].pin_counter=0;
		Buffer[i].dirty=0;
		memset(Buffer[i].data,0,BF_BLOCK_SIZE);
		Buffer[i].time = 0;
	}
	BF_errno=0;
}
//--------------------------------------------------------------------------------------------------------
int BF_CreateFile(char *filename)
{
	char header[BF_BLOCK_SIZE];
	FILE *fp;	

	if ( (fp=fopen(filename,"rb")) != NULL )//an yparxei to arxeio i fopen (read-only) 8a einai epityxis
	{					
		fclose(fp);	
		BF_errno=BFE_FILEEXISTS;
		return BF_errno;		//epistrefoume kwdiko sfalmatos gt 8eloume na min yparxei idi
	}
	//efoson to arxeio den yparxei idi 
	memset(header,0,BF_BLOCK_SIZE);		
	if ( (fp=fopen(filename,"wb")) == NULL)	//dimiourgoume/anoigoume to arxeio 
	{
		BF_errno=BFE_FILEOPENPROB;
		return BF_errno;
	}
	if(fwrite(header,BF_BLOCK_SIZE,1,fp) != 1)//kai grafoume to header
	{	
		BF_errno = BFE_INCOMPLETEHDRWRITE;  
		return BF_errno;
	}	
	fclose(fp);	
	return BFE_OK;
}
//--------------------------------------------------------------------------------------------------------
int BF_DestroyFile(char *filename)
{
	FILE *fp;
	int i,header;	

	if ( (fp=fopen(filename,"rb")) == NULL )		
	{						
		BF_errno=BFE_FILENOTEXISTS;		//epistrefoume kwdiko sfalmatos gt 8eloume na yparxei idi
		return BF_errno;		
	}

	for(i = 0; i< MAXOPENFILES; i++)		//gia ka8e arxeio tou pinaka OpenFiles 
	{
		if( OpenFiles[i].fp == NULL)continue;	//an einai keni i 8esi tou pinaka , proxwrame stin epomeni
		header = OpenFiles[i].header_offset;	//alliws elegxoume an to arxeio einai anoixto--->opote de mporei na katastrafei
		if(!strcmp(Headers[header].fname,filename))
		{
			fclose(fp);
			BF_errno = BFE_FILEOPENINTABLE;
			return BF_errno;
		}
	}

	//an ftasei mexri edw kai yparxei kai einai kleisto , to katastrefoume
	fclose(fp);
	if(remove(filename) != 0)
	{
		BF_errno = BFE_OS; 
		return BF_errno;
	}
	return BFE_OK;	
}
//--------------------------------------------------------------------------------------------------------
int BF_OpenFile(char *filename)
{	
	FILE *fptr;
	int i,header,j,k;


	if ( (fptr=fopen(filename,"rb+")) == NULL )		
	{						
		BF_errno=BFE_FILENOTEXISTS;
		return BF_errno;		
	}
	for(i = 0; i < MAXOPENFILES; i++)//psaxnoume na vroume an to arxeio exei idi anoixtei (exoume ferei tin epikefalida tou ston pinaka twn Headers)
	{
		if (OpenFiles[i].fp == NULL )continue;
		header = OpenFiles[i].header_offset;
		if(header!=-1)
		{
			if(!strcmp(Headers[header].fname,filename)) break;
		}
	}

	for(j = 0; j < MAXOPENFILES; j++)	//gia ka8e 8esi tou pinaka OpenFiles
	{
		if(OpenFiles[j].fp == NULL)	//an i 8esi einai keni
		{
			OpenFiles[j].fp = fptr;	//topo8etw ekei to deikti sto arxeio
			if(i != MAXOPENFILES) OpenFiles[j].header_offset = header;//an exoume idi fortwsei to header tou arxeiou ston pinaka Headers	
			else							  //alliws 
			{
				for(k = 0; k < MAXOPENFILES; k++ )//psaxnoume adeia 8esi ston pinaka twn Headers 
				{
					if(Headers[k].used == 0)
					{
						if(fread(Headers[k].bitmap,BF_BLOCK_SIZE,1,fptr) != 1)
						{
							BF_errno = BFE_INCOMPLETEREAD;
							return BF_errno;		
						}
						strcpy(Headers[k].fname,filename);
						Headers[k].used = 1;
						break;
					}
				}
				OpenFiles[j].header_offset = k;
			}
			return j;	
		}
	}	
	//den vrhkame kenh thesh ston pinaka twn anoixtwn arxeiwn	
	BF_errno = BFE_FTABFULL;
	return BF_errno;	
}
//--------------------------------------------------------------------------------------------------------
int BF_CloseFile(int fileDesc)
{
	int j,k,flag = 0;
	char filename[MAX_FILENAME];

	if( fileDesc < 0 || fileDesc > MAXOPENFILES || OpenFiles[fileDesc].fp == NULL)//elegxoume an einai egkyro to anagnwristiko 
	{
		BF_errno =  BFE_FD;
		return BF_errno;
	}
	strcpy(filename,Headers[OpenFiles[fileDesc].header_offset].fname);
	if(find_in_bitmap(OpenFiles[fileDesc].blockmap,0,BLOCKMAP_SIZE,1) != -1)	//an ayto to anoigma exei ena toulaxiston karfwmeno mplok de 
	{										//mporei na kleisei
		BF_errno = BFE_FILEBLOCKPINNED;	
		return BF_errno;
	}
	for(j = 0; j < MAXOPENFILES; j++)					//psaxnoume na vroume an to arxeio einai anoixto apo alla anoigmata
	{
		if(j != fileDesc){
			if(!strcmp(filename,Headers[OpenFiles[j].header_offset].fname))
			 {
				flag = 1;
				break;
			 }
		}
	}
	if(flag == 0)			//an einai to flag mhden einai to last anoigma ara prepei na graftoun ta dirty blocks pisw sto arxeio
	{
		if((fseek(OpenFiles[fileDesc].fp,0,SEEK_SET)) != 0)
		{
				BF_errno = BFE_OS;
				return BF_errno;
		}
		if(fwrite(Headers[OpenFiles[fileDesc].header_offset].bitmap,BF_BLOCK_SIZE,1,OpenFiles[fileDesc].fp) != 1)
		{	
				BF_errno = BFE_INCOMPLETEHDRWRITE; 
				return BF_errno;
		}
		Headers[OpenFiles[fileDesc].header_offset].used = 0;	// i 8esi tou pinaka Headers de xrisimopoieitai pia
		for(k = 0; k < BF_BUFFER_SIZE; k++)			//psaxnoume to Buffer gia dirty blocks tou arxeiou pou kleinoume kai ta grafoume 
		{							//pisw sto arxeio
			if(!strcmp(Buffer[k].filename,filename))
			{
				if(Buffer[k].dirty == 1)
				{
					if((fseek(OpenFiles[fileDesc].fp,(Buffer[k].block_num + 1)* BF_BLOCK_SIZE,SEEK_SET)) != 0)
					{
						BF_errno = BFE_OS;
						return BF_errno;
					}
					if(fwrite(Buffer[k].data,BF_BLOCK_SIZE,1,OpenFiles[fileDesc].fp) != 1)
					{	
						BF_errno = BFE_INCOMPLETEWRITE; 
						return BF_errno;
					}	
				}
				Buffer[k].is_used=0;
			}
		}
	}
	fclose(OpenFiles[fileDesc].fp);
	OpenFiles[fileDesc].fp=NULL;			
	OpenFiles[fileDesc].header_offset=-1;
	memset(OpenFiles[fileDesc].blockmap,0,3);	
 	return BFE_OK;
}
//--------------------------------------------------------------------------------------------------------
int BF_GetFirstBlock(int fileDesc,int *blockNum,char **blockBuf)
{
	FILE *fptr;
	int valid_block,header,l,place,a,h_of;	

	if( fileDesc < 0 || fileDesc > MAXOPENFILES || OpenFiles[fileDesc].fp == NULL)//elegxoume an einai egkyro to anagnwristiko
	{
		BF_errno =  BFE_FD;
		return BF_errno;
	}
	
	header = OpenFiles[fileDesc].header_offset; 
	valid_block = find_in_bitmap(Headers[header].bitmap,0,BITMAP_SIZE,1);//vriskoume to prwto egkyro block sto bitmap
	if(valid_block == -1)
	{
		BF_errno = BFE_EOF;
		return BF_errno; 
	}
	//psaxnoume na vroume an yparxei idi stin endiamesi mnimi
	for(l = 0; l < BF_BUFFER_SIZE; l++)
	{
		if(Buffer[l].is_used == 1 && !strcmp(Buffer[l].filename,Headers[header].fname) && Buffer[l].block_num == valid_block)
		{
			if(get_bit_value(OpenFiles[fileDesc].blockmap,l) == 1)//an to block einai hdh karfwmeno apo ayto to anoigma
			{
				*blockBuf = Buffer[l].data;
                          	*blockNum = valid_block;
				BF_errno = BFE_BLOCKFIXED;
				return BFE_OK;
			}
			else{							//alliws karfwnoume to block
				Buffer[l].pin_counter++;
				pinbit(OpenFiles[fileDesc].blockmap,l);
				Buffer[l].time = ++timestamp;
				*blockBuf = Buffer[l].data;
				*blockNum = valid_block;
				return BFE_OK;
			}
		} 
	}
	//an den yparxei sthn endiamesi mnimi psaxnoume na vroume an yparxei kenh thesi
	for(l = 0; l< BF_BUFFER_SIZE; l++)
	{
		if(Buffer[l].is_used == 0)
		{
			if((fseek(OpenFiles[fileDesc].fp,(valid_block + 1 ) * BF_BLOCK_SIZE,SEEK_SET)) != 0)//+1 gia na afhsoume pisw to header
			{
				BF_errno = BFE_OS;
				return BF_errno;
			}
			if(fread(Buffer[l].data,BF_BLOCK_SIZE,1,OpenFiles[fileDesc].fp) != 1)//fortwnoume ta dedomena tou block stin endiamesi mnimi
			{
				BF_errno = BFE_INCOMPLETEREAD;
				return BF_errno;		
			}
			Buffer[l].is_used = 1;			
			strcpy(Buffer[l].filename,Headers[header].fname);	
			Buffer[l].block_num = valid_block;		 	
			Buffer[l].pin_counter = 1;  
			pinbit(OpenFiles[fileDesc].blockmap,l);					
			Buffer[l].time = ++timestamp;
			Buffer[l].dirty=0;
			*blockBuf = Buffer[l].data;
			*blockNum = valid_block;
			return BFE_OK;			
		}	
	}

	for(l = 0; l < BF_BUFFER_SIZE; l++)//elegxoume an yparxei toulaxiston ena unpinned block sto buffer
	{
		if(Buffer[l].is_used == 1 && Buffer[l].pin_counter == 0) break;
	}
	if(l == BF_BUFFER_SIZE)		//an einai ola pinned epistrefoume sfalma
	{
		BF_errno = BFE_NOBUF;
		return BF_errno;
	}
	else//yparxei estw kai ena ksekarfwto block sthn endiamesi mnimi, efarmozoume algorithmo LRU
	{
		place = lru();
                //an to block pros antikatastash exei tropopoihthei to grafoume sto arxeio
                if(Buffer[place].dirty == 1)
		{
			for(a = 0; a<MAXOPENFILES; a++)
			{
				h_of = OpenFiles[a].header_offset;
				if(!strcmp(Headers[h_of].fname,Buffer[place].filename))
				{
					fptr = OpenFiles[a].fp;
					break;
				}
			}
			if((fseek(fptr,(Buffer[place].block_num + 1)* BF_BLOCK_SIZE,SEEK_SET)) != 0)
			{
				BF_errno = BFE_OS;
				return BF_errno;
			}
			if(fwrite(Buffer[place].data,BF_BLOCK_SIZE,1,fptr) != 1)
			{	
				BF_errno = BFE_INCOMPLETEWRITE;
				return BF_errno;
			}
		}
		if((fseek(OpenFiles[fileDesc].fp,(valid_block + 1 ) * BF_BLOCK_SIZE,SEEK_SET)) != 0)
		{
			BF_errno = BFE_OS;
			return BF_errno;
		}
		if(fread(Buffer[place].data,BF_BLOCK_SIZE,1,OpenFiles[fileDesc].fp) != 1)
		{
			BF_errno = BFE_INCOMPLETEREAD;
			return BF_errno;
		}
		Buffer[place].is_used = 1;			
		strcpy(Buffer[place].filename,Headers[header].fname);	
		Buffer[place].block_num = valid_block;		 	
		Buffer[place].pin_counter=1;
		pinbit(OpenFiles[fileDesc].blockmap,place);					
		Buffer[place].time = ++timestamp;
		Buffer[place].dirty=0; 
		*blockBuf = Buffer[place].data;
		*blockNum = valid_block;
		return BFE_OK;
	}
}
//--------------------------------------------------------------------------------------------------------
int BF_GetNextBlock(int fileDesc,int *blockNum,char **blockBuf)
{	
	FILE *fptr;
	int valid_block,header,place,l,a,h_of;	
	
	if( fileDesc < 0 || fileDesc > MAXOPENFILES || OpenFiles[fileDesc].fp == NULL)//elegxoume an einai egkyro to anagnwristiko
	{
		BF_errno =  BFE_FD;
		return BF_errno;
	}
	header = OpenFiles[fileDesc].header_offset; 
	if((get_bit_value(Headers[header].bitmap,*blockNum)) == 0)// elegxoume an einai egkyro to block pou dinetai
	{
		BF_errno = BFE_INVALIDBLOCK;
		return BF_errno;
	}
	
	valid_block = find_in_bitmap(Headers[header].bitmap,*blockNum + 1,BITMAP_SIZE,1);//vriskoume to prwto egkyro block sto bitmap meta to *blockNum
	if(valid_block == -1)
	{
		BF_errno = BFE_EOF;
		return BF_errno; 
	}
	//psaxnoume na vroume an yparxei idi stin endiamesi mnimi
	for(l = 0; l < BF_BUFFER_SIZE; l++)
	{
		if(Buffer[l].is_used == 1 && !strcmp(Buffer[l].filename,Headers[header].fname) && Buffer[l].block_num == valid_block)
		{
			if(get_bit_value(OpenFiles[fileDesc].blockmap,l) == 1)//to block einai hdh karfwmeno apo ayto to anoigma
			{
				BF_errno = BFE_BLOCKFIXED;
				return BFE_OK;
			}
			else{
				Buffer[l].pin_counter++;
				pinbit(OpenFiles[fileDesc].blockmap,l);
				Buffer[l].time = ++timestamp;
				*blockBuf = Buffer[l].data;
				*blockNum = valid_block;
				return BFE_OK;
			}
		} 
	}
	//an den yparxei sthn endiamesi mnimi psaxnw na vrw an yparxei kenh thesi
	for(l = 0; l< BF_BUFFER_SIZE; l++)
	{
		if(Buffer[l].is_used == 0)
		{
			if((fseek(OpenFiles[fileDesc].fp,(valid_block + 1) * BF_BLOCK_SIZE,SEEK_SET)) != 0)//+1 gia na afhsoume pisw to header
			{
				BF_errno = BFE_OS;
				*blockBuf = Buffer[l].data;
				*blockNum = valid_block;
				return BF_errno;
			}
			if(fread(Buffer[l].data,BF_BLOCK_SIZE,1,OpenFiles[fileDesc].fp) != 1)
			{
				BF_errno = BFE_INCOMPLETEREAD;
				return BF_errno;		
			}
			Buffer[l].is_used = 1;			
			strcpy(Buffer[l].filename,Headers[header].fname);	
			Buffer[l].block_num = valid_block;		 	
			Buffer[l].pin_counter = 1; 
			Buffer[l].dirty=0;
			pinbit(OpenFiles[fileDesc].blockmap,l);					
			Buffer[l].time = ++timestamp;
			*blockBuf = Buffer[l].data;
			*blockNum = valid_block;
			return BFE_OK;			
		}	
	}

	for(l = 0; l < BF_BUFFER_SIZE; l++)
	{
		if(Buffer[l].is_used == 1 && Buffer[l].pin_counter == 0) break;
	}
	if(l == BF_BUFFER_SIZE)
	{
		BF_errno = BFE_NOBUF;
		return BF_errno;
	}
	else//yparxei estw kai ena ksekarfwto block sthn endiamesi mnimi
	{
		place = lru();
		if(Buffer[place].dirty == 1)
                {	
			for(a = 0; a<MAXOPENFILES; a++)
			{
				h_of = OpenFiles[a].header_offset;
				if(!strcmp(Headers[h_of].fname,Buffer[place].filename))
				{
					fptr = OpenFiles[a].fp;
					break;
				}
			}
                        if((fseek(fptr,(Buffer[place].block_num + 1)* BF_BLOCK_SIZE,SEEK_SET)) != 0)
                        {
                                BF_errno = BFE_OS;
                                return BF_errno;
                        }
                        if(fwrite(Buffer[place].data,BF_BLOCK_SIZE,1,fptr) != 1)
                        {
                                BF_errno = BFE_INCOMPLETEWRITE;
                                return BF_errno;
                        }
                }
                if((fseek(OpenFiles[fileDesc].fp,(valid_block + 1 ) * BF_BLOCK_SIZE,SEEK_SET)) != 0)//+1 gia na afhsoume pisw to header
                {
                        BF_errno = BFE_OS;
                        return BF_errno;
                }
                if(fread(Buffer[place].data,BF_BLOCK_SIZE,1,OpenFiles[fileDesc].fp) != 1)
                {
                        BF_errno = BFE_INCOMPLETEREAD;
                        return BF_errno;
                }
		Buffer[place].is_used = 1;			
		strcpy(Buffer[place].filename,Headers[header].fname);	
		Buffer[place].block_num = valid_block;		 	
		Buffer[place].pin_counter=1;  
		pinbit(OpenFiles[fileDesc].blockmap,place);					
		Buffer[place].time = ++timestamp;
		Buffer[place].dirty=0;
		*blockBuf = Buffer[place].data;
		*blockNum = valid_block;
		return BFE_OK;
	}
	
}
//--------------------------------------------------------------------------------------------------------
int BF_GetThisBlock(int fileDesc,int blockNum,char **blockBuf)
{
	FILE *fptr;
	int place,header,l,a,h_of;

	if( fileDesc < 0 || fileDesc > MAXOPENFILES || OpenFiles[fileDesc].fp == NULL)//elegxoume an einai egkyro to anagnwristiko
	{
		BF_errno =  BFE_FD;
		return BF_errno;
	}
	header = OpenFiles[fileDesc].header_offset; 
	if((get_bit_value(Headers[header].bitmap,blockNum)) == 0)//elegxoume an einai egkyro to blockNum
	{
		BF_errno = BFE_INVALIDBLOCK;
		return BF_errno;
	}
	//psaxnoume na vroume an yparxei idi stin endiamesi mnimi
	for(l = 0; l < BF_BUFFER_SIZE; l++)
	{
		if(Buffer[l].is_used == 1 && !strcmp(Buffer[l].filename,Headers[header].fname) && Buffer[l].block_num == blockNum)
		{
			if(get_bit_value(OpenFiles[fileDesc].blockmap,l) == 1)//to block einai hdh karfwmeno apo ayto to anoigma
			{
				BF_errno = BFE_BLOCKFIXED;
				*blockBuf = Buffer[l].data;
				return BFE_OK;
			}
			else{
				Buffer[l].pin_counter++;
				pinbit(OpenFiles[fileDesc].blockmap,l);
				Buffer[l].time = ++timestamp;
				*blockBuf = Buffer[l].data;
				return BFE_OK;
			}
		} 
	}
	//an den yparxei sthn endiamesi mnimi psaxnw na vrw an yparxei kenh thesi
	for(l = 0; l< BF_BUFFER_SIZE; l++)
	{
		if(Buffer[l].is_used == 0)
		{
			if((fseek(OpenFiles[fileDesc].fp,(blockNum + 1) * BF_BLOCK_SIZE,SEEK_SET)) != 0)//+1 gia na afhsoume pisw to header
			{
				BF_errno = BFE_OS;
				return BF_errno;
			}
			if(fread(Buffer[l].data,BF_BLOCK_SIZE,1,OpenFiles[fileDesc].fp) != 1)
			{
				BF_errno = BFE_INCOMPLETEREAD;
				return BF_errno;		
			}
			Buffer[l].is_used = 1;			
			strcpy(Buffer[l].filename,Headers[header].fname);	
			Buffer[l].block_num =blockNum ;		 	
			Buffer[l].pin_counter=1;
			Buffer[l].dirty=0;
			pinbit(OpenFiles[fileDesc].blockmap,l);					
			Buffer[l].time = ++timestamp;
			*blockBuf = Buffer[l].data;
			return BFE_OK;			
		}	
	}
	for(l = 0; l < BF_BUFFER_SIZE; l++)
	{
		if(Buffer[l].is_used == 1 && Buffer[l].pin_counter == 0) break;
	}
	if(l == BF_BUFFER_SIZE)
	{
		BF_errno = BFE_NOBUF;
		return BF_errno;
	}
	else//yparxei estw kai ena ksekarfwto block sthn endiamesi mnimi
	{
		place = lru();
		if(Buffer[place].dirty == 1)
                {
			for(a = 0; a<MAXOPENFILES; a++)
			{
				h_of = OpenFiles[a].header_offset;
				if(!strcmp(Headers[h_of].fname,Buffer[place].filename))
				{
					fptr = OpenFiles[a].fp;
					break;
				}
			}
                        if((fseek(fptr,(Buffer[place].block_num + 1)* BF_BLOCK_SIZE,SEEK_SET)) != 0)
                        {
                                BF_errno = BFE_OS;
                                return BF_errno;
                        }
                        if(fwrite(Buffer[place].data,BF_BLOCK_SIZE,1,fptr) != 1)
                        {
                                BF_errno = BFE_INCOMPLETEWRITE;
                                return BF_errno;
                        }
                }
                if((fseek(OpenFiles[fileDesc].fp,(blockNum + 1 ) * BF_BLOCK_SIZE,SEEK_SET)) != 0)//+1 gia na afhsoume pisw to header
                {
                        BF_errno = BFE_OS;
                        return BF_errno;
                }
                if(fread(Buffer[place].data,BF_BLOCK_SIZE,1,OpenFiles[fileDesc].fp) != 1)
                {
                        BF_errno = BFE_INCOMPLETEREAD;
                        return BF_errno;
                }		
		Buffer[place].is_used = 1;			
		strcpy(Buffer[place].filename,Headers[header].fname);	
		Buffer[place].block_num = blockNum;		 	
		Buffer[place].pin_counter=1;
		pinbit(OpenFiles[fileDesc].blockmap,place);					
		Buffer[place].dirty=0;
		Buffer[place].time = ++timestamp;
		*blockBuf = Buffer[place].data;
		return BFE_OK;
	}
}
//--------------------------------------------------------------------------------------------------------
int BF_AllocBlock(int fileDesc,int *blockNum,char **blockBuf)
{
	FILE* fptr;
	int place,l,header,lruresult,a,h_of;

	if( fileDesc < 0 || fileDesc > MAXOPENFILES || OpenFiles[fileDesc].fp == NULL)//elegxoume an einai egkyro to anagnwristiko
	{
		BF_errno =  BFE_FD;
		return BF_errno;
	}
	header = OpenFiles[fileDesc].header_offset;
	place = find_in_bitmap(Headers[header].bitmap,0,BITMAP_SIZE,0);//vriskoume tin prwti adeia 8esi sto bitmap tou header
	if(place == -1)
	{
		BF_errno = BFE_EOF;
		return BF_errno;
	}
	for(l = 0; l< BF_BUFFER_SIZE; l++)
	{
		if(Buffer[l].is_used == 0)
		{
			Buffer[l].is_used = 1;			
			strcpy(Buffer[l].filename,Headers[header].fname);	
			Buffer[l].block_num = place;		 	
			Buffer[l].pin_counter=1;
			pinbit(OpenFiles[fileDesc].blockmap,l);
			Buffer[l].time = ++timestamp;
			Buffer[l].dirty=0;
			memset(Buffer[l].data,0,BF_BLOCK_SIZE);
                	if((fseek(OpenFiles[fileDesc].fp,(place + 1) * BF_BLOCK_SIZE,SEEK_SET)) != 0)//+1 gia na afhsoume pisw to header
                	{
			     	BF_errno = BFE_OS;
                        	return BF_errno;
                	}
                	if(fwrite(Buffer[l].data,BF_BLOCK_SIZE,1,OpenFiles[fileDesc].fp) != 1)
                	{
                        	BF_errno = BFE_INCOMPLETEWRITE;
                        	return BF_errno;
                	}
			*blockBuf = Buffer[l].data;  
			*blockNum = Buffer[l].block_num;
			pinbit(Headers[header].bitmap,place);
			return BFE_OK;			
		}	
	}
	for(l = 0; l < BF_BUFFER_SIZE; l++)
	{
		if(Buffer[l].is_used == 1 && Buffer[l].pin_counter == 0) break;
	}
	if(l == BF_BUFFER_SIZE)
	{
		BF_errno = BFE_NOBUF;
		return BF_errno;
	}
        else//yparxei estw kai ena ksekarfwto block sthn endiamesi mnimi
	{
		lruresult = lru();
		if(Buffer[lruresult].dirty == 1)
		{
			for(a = 0; a<MAXOPENFILES; a++)
			{
				h_of = OpenFiles[a].header_offset;
				if(!strcmp(Headers[h_of].fname,Buffer[lruresult].filename))
				{
					fptr = OpenFiles[a].fp;
					break;
				}
			}
			if((fseek(fptr,(Buffer[lruresult].block_num + 1 ) * BF_BLOCK_SIZE,SEEK_SET)) != 0)
		        {
		                BF_errno = BFE_OS;
		                return BF_errno;
		        }
		        if(fwrite(Buffer[lruresult].data,BF_BLOCK_SIZE,1,fptr) != 1)
		        {
		                BF_errno = BFE_INCOMPLETEWRITE;
		                return BF_errno;
		        }
		}
		memset(Buffer[lruresult].data,0,BF_BLOCK_SIZE);
		if((fseek(OpenFiles[fileDesc].fp,(place + 1) * BF_BLOCK_SIZE,SEEK_SET)) != 0)//+1 gia na afhsoume pisw to header
		{
			BF_errno = BFE_OS;
			return BF_errno;
		}
		if(fwrite(Buffer[lruresult].data,BF_BLOCK_SIZE,1,OpenFiles[fileDesc].fp) != 1)
		{
			BF_errno = BFE_INCOMPLETEWRITE;
			return BF_errno;		
		}
		Buffer[lruresult].is_used = 1;			
		strcpy(Buffer[lruresult].filename,Headers[header].fname);	
		Buffer[lruresult].block_num = place;		 	
		Buffer[lruresult].pin_counter=1;
		Buffer[lruresult].dirty=0;
		pinbit(OpenFiles[fileDesc].blockmap,lruresult);					
		Buffer[lruresult].time = ++timestamp;
		*blockBuf = Buffer[lruresult].data;
		*blockNum = Buffer[lruresult].block_num;
		pinbit(Headers[header].bitmap,place);
		return BFE_OK;
	}
}
//--------------------------------------------------------------------------------------------------------
int BF_DisposeBlock(int fileDesc,int *blockNum)
{
	int place,header,l;

        if( fileDesc < 0 || fileDesc > MAXOPENFILES || OpenFiles[fileDesc].fp == NULL)//elegxoume an einai egkyro to anagnwristiko
	{
		BF_errno =  BFE_FD;
		return BF_errno;
	}
	header = OpenFiles[fileDesc].header_offset; 
	place = get_bit_value(Headers[header].bitmap,*blockNum);//elegxoume an to block einai egkyro

	if(place == 0)
	{
		BF_errno = BFE_INVALIDBLOCK;
		return BF_errno;
	}
	
	for(l = 0; l < BF_BUFFER_SIZE; l++)
	{
		if(Buffer[l].is_used == 1 && !strcmp(Headers[header].fname,Buffer[l].filename) && Buffer[l].block_num == *blockNum)
		{
			if(Buffer[l].pin_counter != 0)
			{
				BF_errno = BFE_BLOCKUNFIXED;
				return BF_errno;
			}
			else 
			{
				unpinbit(Headers[header].bitmap,*blockNum);
				Buffer[l].is_used = 0;
				return BFE_OK;	
			}	
		} 
	}
        unpinbit(Headers[header].bitmap,*blockNum);//sto bitmap tou header kanoume to bit -> 0
	return BFE_OK;	
}
//--------------------------------------------------------------------------------------------------------
int BF_UnpinBlock(int fileDesc,int blockNum,int dirty)
{
	int i,header;
	if( fileDesc < 0 || fileDesc > MAXOPENFILES || OpenFiles[fileDesc].fp == NULL) //elegxoume an einai egkyro to anagnwristiko
	{
		BF_errno =  BFE_FD;
		return BF_errno;
	}
	header = OpenFiles[fileDesc].header_offset;
	for(i = 0; i < BF_BUFFER_SIZE; i++)
	{
		if(Buffer[i].is_used == 1 && !strcmp(Headers[header].fname,Buffer[i].filename)&& Buffer[i].block_num == blockNum) 
			break;
	}
	if(i == BF_BUFFER_SIZE)
	{
		BF_errno =  BFE_INVALIDBLOCK;
                return BF_errno;
        }
	//brikame th thesh tou block sthn endiamesh mnhmh
	if(dirty == 1) //an einai mhden den kanoume allagh giati kapoio allo anoigma mporei na to exei kanei 1
		Buffer[i].dirty=1;
	if(Buffer[i].pin_counter == 0)
	{
		BF_errno = BFE_BLOCKUNFIXED;
		return BF_errno;		
	}
	Buffer[i].pin_counter--;  //meiwnoume ta kerfwmata tou block
	unpinbit(OpenFiles[fileDesc].blockmap,i); //enhmerwsh blockmap tou anoigmatos sth thesh fileDesc
	return BFE_OK;

}
//--------------------------------------------------------------------------------------------------------
void BF_PrintError(char *errString)
{
	fprintf(stderr,"\t%s\n",errString);
	switch (BF_errno)
	{
		case BFE_NOMEM:
			fprintf(stderr,"\tSfalma: Elleipsi Mnimis\n");
			break;
		case BFE_NOBUF:
			fprintf(stderr,"\tSfalma: Elleipsi Xwrou Endiamesis Mnimis\n");
			break;
		case BFE_BLOCKFIXED:
			fprintf(stderr,"\tSfalma: Mplok idi 'karfwmeno' sti Mnimi\n");
			break;
		case BFE_BLOCKNOTINBUF:
			fprintf(stderr,"\tSfalma: Mplok gia 'ksekarfwma' den einai sti Mnimi\n");
			break;
		case BFE_BLOCKINBUF:
			fprintf(stderr,"\tSfalma: Mplok idi sti Mnimi\n");
			break;
		case BFE_OS:
			fprintf(stderr,"\tSfalma: Geniko Sfalma Leitourgikou Systimatos\n");
			break;
		case BFE_INCOMPLETEREAD:
			fprintf(stderr,"\tSfalma: Atelis Anagnwsi Mplok\n");
			break;
		case BFE_INCOMPLETEWRITE:
			fprintf(stderr,"\tSfalma: Ateles Grapsimo se Mplok\n");
			break;
		case BFE_INCOMPLETEHDRREAD:
			fprintf(stderr,"\tSfalma: Atelis Anagnwsi Mplok-Kefalidas\n");
			break;
		case BFE_INCOMPLETEHDRWRITE:
			fprintf(stderr,"\tSfalma: Ateles Grapsimo se Mplok-Kefalida\n");
			break;
		case BFE_INVALIDBLOCK:
			fprintf(stderr,"\tSfalma: Mi Egkyros Anagnwristikos Ari8mos Mplok\n");
			break;
		case BFE_FILEOPEN:
			fprintf(stderr,"\tSfalma: Arxeio idi Anoixto\n");
			break;
		case BFE_FTABFULL:
			fprintf(stderr,"\tSfalma: Lista Anoixtwn Arxeiwn Pliris\n");
			break;
		case BFE_FD:
			fprintf(stderr,"\tSfalma: Mi Egkyros Anagnwristikos Ari8mos Arxeiou\n");
			break;
		case BFE_EOF:
			fprintf(stderr,"\tSfalma: Telos Arxeiou\n");
			break;
		case BFE_BLOCKFREE:
			fprintf(stderr,"\tSfalma: Mplok idi Dia8esimo\n");
			break;
		case BFE_BLOCKUNFIXED:
			fprintf(stderr,"\tSfalma: Mplok idi 'Ksekarfwmeno'\n");
			break;
		case BFE_FILEEXISTS:
			fprintf(stderr,"\tSfalma:  To Arxeio Pros Dimiourgia Yparxei idi\n");
			break;
		case BFE_FILEOPENPROB:
			fprintf(stderr,"\tSfalma:  Provlima sto Anoigma tou Arxeiou\n");
			break;
		case BFE_FILENOTEXISTS:
                        fprintf(stderr,"\tSfalma:  To Arxeio Pros Anoigma Den Yparxei\n");
                        break;
		case BFE_FILEOPENINTABLE:
                        fprintf(stderr,"\tSfalma:  To Arxeio Pros Katastrofi anoixto ston pinaka twn openfiles\n");
                        break;
		case BFE_FILEBLOCKPINNED:
                        fprintf(stderr,"\tSfalma:  Apotiximeno kleisimo tou arxeiou gt exei pinned blocks\n");
                        break;
		default:
			fprintf(stderr,"\t Agnwsto Sfalma\n"); 
			break;
	}
}
//--------------------------------------------------------------------------------------------------------


