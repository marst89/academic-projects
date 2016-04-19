#include <stdio.h>
#include <string.h>
#include <math.h>

#define 	BFE_OK			 0		// OK 
#define 	BFE_NOMEM		-1		// Elleipsi Mnimis 
#define 	BFE_NOBUF		-2		// Elleipsi Xwrou Endiamesis Mnimis 
#define 	BFE_BLOCKFIXED		-3		// Mplok idi 'karfwmeno' sti Mnimi
#define 	BFE_BLOCKNOTINBUF	-4		// Mplok gia 'ksekarfwma' den einai sti Mnimi
#define 	BFE_BLOCKINBUF		-5		// Mplok idi sti Mnimi
#define 	BFE_OS			-6		// Geniko Sfalma Leitourgikou Systimatos
#define 	BFE_INCOMPLETEREAD	-7		// Atelis Anagnwsi Mplok
#define 	BFE_INCOMPLETEWRITE	-8		// Ateles Grapsimo se Mplok
#define 	BFE_INCOMPLETEHDRREAD	-9		// Atelis Anagnwsi Mplok-Kefalidas
#define 	BFE_INCOMPLETEHDRWRITE	-10		// Ateles Grapsimo se Mplok-Kefalida
#define 	BFE_INVALIDBLOCK	-11		// Mi Egkyros Anagnwristikos Ari8mos Mplok
#define 	BFE_FILEOPEN		-12		// Arxeio idi Anoixto
#define 	BFE_FTABFULL		-13		// Lista Anoixtwn Arxeiwn Pliris
#define 	BFE_FD			-14		// Mi Egkyros Anagnwristikos Ari8mos Arxeiou
#define 	BFE_EOF			-15		// Telos Arxeiou
#define 	BFE_BLOCKFREE		-16		// Mplok idi Dia8esimo
#define 	BFE_BLOCKUNFIXED	-17		// Mplok idi 'Ksekarfwmeno' 
#define		BFE_FILEEXISTS		-18		// To Arxeio Pros Dimiourgia Yparxei idi
#define		BFE_FILEOPENPROB	-19		// Provlima sto Anoigma tou Arxeiou
#define		BFE_FILENOTEXISTS	-20		// To Arxeio Pros Anoigma Den Yparxei 
#define		BFE_FILEOPENINTABLE	-21		// To Arxeio Pros Katastrofi anoixto ston pinaka twn openfiles
#define		BFE_FILEBLOCKPINNED	-22		//Apotiximeno kleisimo tou arxeiou gt exei pinned blocks

#define 	BF_BLOCK_SIZE 		1024		//mege8os block(se bytes)
#define 	BF_BUFFER_SIZE 		20		//mege8os tou buffer (se blocks)
#define 	MAXOPENFILES 		25		//megistos ari8mos anoixtwn arxeiwn
#define 	MAX_FILENAME 		200		//megisto mikos onomatos arxeiou
#define		BITMAP_SIZE		8*1024		//megethos tou bitmap
#define 	BLOCKMAP_SIZE		20		//megethos tou xarth twn blocks(blockmap)
#define		TRUE			1 
#define 	FALSE			0
#define 	MAX_TIMESTAMP		21

typedef struct			//stoixeia apo ta opoia 8a apoteleitai o pinakas OpenFiles
{
FILE *fp;			//deiktis sto anoixto arxeio
int header_offset;		//akeraios gia ti 8esi opou vrisketai to header tou arxeiou ston pinaka twn Headers
char blockmap[3];		//gia na vroume poia blocks exei karfwsei o xrhsths apo ta yparxonta sto Buffer
}File_Entry;			

typedef struct
{
char bitmap[BF_BLOCK_SIZE];	//ka8e header periexei mono to bitmap kai fylassetai ston pinaka twn Headers
char fname[MAX_FILENAME];
int used;			
}Header_Entry;

typedef struct 			//plirofories gia ka8e stoixeio tou buffer 
{
int is_used;			//deixnei an xrisimopoieitai to sygkekrimeno block tou buffer(bool mono sti c++!!)
char filename[MAX_FILENAME];	//onoma arxeiou sto opoio anikei to block
int block_num;		 	//posa blocks mege8ous BLOCKSIZE apexei apo tin arxi tou arxeiou sto opoio anikei
int pin_counter;		//poses fores exei ginei pin to sygkekrimeno block
int dirty;			//an exei tropopoii8ei (dirty=1) prepei na ksanagraftei 
char data[BF_BLOCK_SIZE];	//actual data tou block
int time;			//o xronos pou xrhsimopihthhke gia teleutaia fora to block
}Buffer_Entry;

//---------------------Global variables-----------------
int BF_errno;
int timestamp;
File_Entry   OpenFiles[MAXOPENFILES];			//pinakas gia plirofories twn anoixtwn arxeiwn
Header_Entry Headers[MAXOPENFILES];			//pinakas twn headers twn anoixtwn arxeiwn
Buffer_Entry Buffer[BF_BUFFER_SIZE];			//buffer opou diavazontai ka8e fora 20 apo ta block twn arxeiwn
//------------------------------------------------------

//-----------------------Functions----------------------
void BF_Init(void);
int BF_CreateFile(char *filename);
int BF_DestroyFile(char *filename);
int BF_OpenFile(char *filename);
int BF_CloseFile(int fileDesc);
int BF_GetFirstBlock(int fileDesc,int *blockNum,char **blockBuf);
int BF_GetNextBlock(int fileDesc,int *blockNum,char **blockBuf);
int BF_GetThisBlock(int fileDesc,int blockNum,char **blockBuf);
int BF_AllocBlock(int fileDesc,int *blockNum,char **blockBuf);
int BF_DisposeBlock(int fileDesc,int *blockNum);
int BF_UnpinBlock(int fileDesc,int blockNum,int dirty);

