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


#ifndef __VW_LIB_H__
#define __VW_LIB_H__

#include "functions.h"

int VW_createSelect(int argc, char *argv[]);
int VW_createJoin(int argc, char *argv[]);
int VW_delete(int argc, char *argv[]);
int VW_insert(int argc, char *argv[]);
int VW_destroy(int argc, char *argv[]);

#endif   /*  __VW_LIB_H__  */
