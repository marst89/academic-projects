
#include "AM_Lib.h"

int AM_get_bit_value2(char Byte,int Bit)	//pairnei ena byte kai tin 8esi enos bit kai epistrefei tin timi tou (0 h' 1)	
{
	int m;
	m=Bit%8;
	if((Byte & (char)pow(2,m))==0)
		return 0;
	else
		return 1;
}

int AM_find_in_bitmap(char* bitmap,int position,int bitmap_size,int one_bit)//apo ti 8esi position tou bitmap k meta fernei to prwto midenik(one_bit==0)
{									 //i ton prwto asso (an one_bit==1)
	int i,loops,m,q,arxi_i=0,arxi_m=0;
	loops = (int)ceil((float)bitmap_size/8);
	if(position!=0) arxi_i=(loops-1)-position/8;
	else arxi_i=loops-1;
	arxi_m=position%8; 
	for(i=arxi_i;i>=0;i--)
	{
		if(i == 0)
		{ 
			q=bitmap_size%8;
			if(q == 0) q=8;
		} 	
	 	else q=8;
		for(m=arxi_m; m<q; m++)
		{
			if(AM_get_bit_value2(bitmap[i],m) == one_bit)
				return ((loops-1)-i)*8+m;
		}
		arxi_m=0;
	}
	return -1;
}


void AM_pinbit(char* bitmap,int position,int bitmap_size)//kanei asso to bit sti 8esi position tou bitmap
{
	int Byte,m,bitmap_bytes;
	bitmap_bytes = (int)ceil((float)bitmap_size/8.0);         //bitmap_size=megisto dynato plithos eggrafwn
	Byte=position/8;
	m=position%8;
	bitmap[(bitmap_bytes-1)-Byte]=bitmap[(bitmap_bytes-1)-Byte] | (char)pow(2,m);
	return;
}

void AM_unpinbit(char* bitmap,int position,int bitmap_size)//kanei miden to bit sti 8esi position tou bitmap
{
     int Byte,m,bitmap_bytes;

     bitmap_bytes = (int)ceil((float)bitmap_size/8);
     Byte=position/8;
     m=position%8;
     bitmap[(bitmap_bytes-1)-Byte]=bitmap[(bitmap_bytes-1)-Byte] & (~(char)pow(2,m));
     return;
}

int AM_get_bit_value(char* bitmap,int position,int bitmap_size)
//pairnei olokliro to bitmap kai tin 8esi enos bit kai epistrefei tin timi tou (0 h' 1)	
{
	int Byte,m,bitmap_bytes;
	bitmap_bytes = ceil((float)bitmap_size/8);
	Byte=position/8;
	m=position%8;
	if((bitmap[(bitmap_bytes-1)-Byte] & (char)pow(2,m))==0)
		return 0;
	else
		return 1;
}

unsigned int JSHash(char* str, unsigned int len) 
{
    unsigned int hash = 542391111;
    unsigned int i = 0;

    for(i = 0; i < len-1, (*str != '\0'); str++, i++) {
       hash *= ((hash << 5) + (*str) + (hash >> 2));
    }
    return hash;
}

unsigned short int find_bucket(unsigned int glob_depth,unsigned int retval)//vriskei se poia thesi toy pinaka katakermatismou antistoixei h timh pou
//epistrefei h hash function
{
	unsigned int shift;

	shift = 32 - glob_depth;
	retval = retval >> shift;
	return 	(unsigned short int)retval;
}

int type_check(char attrType,int attrLength)//elegxei an o typos poy dothike ws orisma symvadizei me ton arithmo twn bytes pou dinetai epishs ws orisma
{
	if(attrType ==  'i' || attrType == 'f')
	{
		if(attrLength != 4)
		{
			AM_errno = WRONG_LENGTH;
			return AM_errno;
		}
	}
	if(attrType == 'c')
	{
		if(attrLength > 255 || attrLength < 1)
		{
			AM_errno = WRONG_LENGTH;
			return AM_errno;
		}
	}

	return AME_OK;
}
