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

#ifndef AM_LIB_H
#define AM_LIB_H

#include "BF_Lib.h"
#include "helpful.h"

#define MAXSCANS 20
#define EQUAL 1
#define LESS_THAN 2
#define GREATER_THAN 3
#define LESS_THAN_OR_EQUAL 4
#define GREATER_THAN_OR_EQUAL 5
#define NOT_EQUAL 6
#define STRING 'c'
#define INTEGER 'i' 
#define FLOAT 'f'
#define NONE_TYPE ' '
#define AME_OK 0
#define WRONG_LENGTH -1			//to mikos tou pediou tis eggrafis den symvadizei me ton typo tou 
#define AM_MAX_INDEXNO_LENGTH 10
#define AM_MAX_FILE_NAME 200
#define UNAVAILABLEOP -2		//mi diathesimi leitourgia(diladi leitourgia diafori tou = ,!=)
#define SCANSTABLEFULL -3		//o pinakas twn anoixtwn sarwsewn einai pliris(den exei adeia thesi)
#define	INVALIDSCANDESC -4		//mi egkyro anagnwristiko energis sarwsis
#define ALREADYCLOSEDSCAN -5		//idi kleisth sarwsi
#define ERRORINBFLEVEL -6		//geniko sfalma tou BF epipedou
#define MAXDEPTH	-7		//exoume ftasei sto megisto vathos tou pinaka kai de mporoume na diplasiasoume pia
#define REC_NOT_FOUND	-8		//den vrethike h eggrafi pou dothike ws orisma
#define NOMORERECS	-9		//den yparxei allh eggrafh meta th dosmenh
#define OS_FAILURE	-10
#define AME_EOF 	-11
#define TRUE		1
#define FALSE		0

//----------------------------------structs and global variables-----------------------------------//
typedef struct{
 int last_rec;		//to anagnwristiko tis teleutaias eggrafis
 int cur_file;		//o arithmos pou antistoixei sto teleutaio arxeio pou sarwnetai
 int scan_open;  	//an einai anoixto gia sarwsi to antistoixo arxeio
 int operation;  	//h leitourgia gia thn opoia skanaretai to arxeio(=,<,>,<=,>=,!=)
 char *value;		//h timh tou pediou-kleidiou pros sugkrisi
 char type;		//apothikeyei ton typo tis eggrafis
 int length;		//apothikeyei to mikos se bytes tis eggrafis
 short int pointer;	//apothikeyei ton teleytaio short int 
}Scans_Table;

Scans_Table Scans[MAXSCANS];
int AM_errno;

#endif
