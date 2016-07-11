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


#ifndef __FUNCTIONS__
#define __FUNCTIONS__


#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "DM_Lib.h"
#include "UT_Lib.h"
#include "HF_Lib.h"

#define VWE_OK 0
#define OK 0 
#define RELCATSIZE	MAXNAME + (4* sizeof(int))
#define ATTRCATSIZE	(2*MAXNAME) + (4* sizeof(int)) + sizeof(char)
#define VIEWCATSIZE	5*MAXNAME + 2*sizeof(int) + sizeof(int) + MAXSTRINGLENGTH
#define VIEWATTRCATSIZE	4*MAXNAME	
#define TRUE 1
#define FALSE 0 
#define UTE_OK  0
#define DME_OK 0

char* reverse_op(int op);
int find_operator(char *oper );
int isNumber(char *str);
int isFloat(char *str);
int insertInViewCat(char *viewname,int type,char *relname1,char *attrname1,int op,char* relname2,char *attrname2,char *value,int attrcnt);
int insertInViewAttrCat(char *viewname,char *viewattrname,char *relname,char *relattrname);
int giveviewcatrecord(char* record,char *relation);
int giveViewattrcatrecord(char *record,char *relation,char *attribute);
int delete_viewattrs(char *view);
void free_argument(char ***argument);
void free_insert_argument(char ***argument);
void free_args(char ***argument,int argc)	;
int construct_select_arg(char ***argument,int argc,char* viewname,char* relation);
int select_arg(char ***argument,int argc,char* relation,char* destination);
int select_cond_arg(char *** argument,int argc,char* relation,char* destination,char*field,char* op,char* value);
int construct_select_cond_arg(char ***argument,int argc,char* viewname,char* relation,char* attrname,int op,char* value);
int construct_join_arg(char ***argument,int argc,char* viewname,char* relation,char* relname1,char* attrname1,int op,char* relname2,char*attrname2,char*inrel);
int construct_add_arg(char*** argument,int argc,char* relation1,char* relation2);
int construct_sub_arg(char*** argument,int argc,char* relation1,char* relation2);
int construct_destroy_arg(char*** argument,int argc,char* relation);
int construct_argument(char ***argument,char *argv[],int count,int mode);
int construct_insert_arg(char ***argument,char *argv[],int count,char *relname);
int construct_create_arg(char ***argument,char *argv[],int argc,char* viewname);
int update_views(char *view,char* initial_relation,char *temp,int mode);
int existing_views(char *name);


#endif
