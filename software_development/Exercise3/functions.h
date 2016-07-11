#ifndef __FUNCTIONS__
#define __FUNCTIONS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include "AM_Lib.h"
#include "HF_Lib.h"

#define MAXNAME 	40
#define MAXSTRINGLENGTH 255
#define RELCATSIZE	MAXNAME + (4* sizeof(int))
#define ATTRCATSIZE	(2*MAXNAME) + (4* sizeof(int)) + sizeof(char)
#define VIEWCATSIZE	5*MAXNAME + 2*sizeof(int) + sizeof(short int) + MAXSTRINGLENGTH
#define VIEWATTRCATSIZE	4*MAXNAME 
#define TRUE 1
#define FALSE 0 
#define UTE_OK  0
#define DME_OK 0
#define OK 0



typedef struct {
	char relname[MAXNAME];
	int relwidth;
	int attrcnt;
	int indexcnt;
	int rec_num;
	}relDesc;

typedef struct {
	char relname[MAXNAME];	
	char attrname[MAXNAME];	
	int offset;	
	int attrlength;		
	char attrtype;		
	int indexed;	
	int indexno;	
	} attrDesc; 


typedef struct {
	char viewname[MAXNAME];
	short type;
	char relname1[MAXNAME];
	char attrname1[MAXNAME];
	int op;
	char relname2[MAXNAME];
	char attrname2[MAXNAME];
	char value[MAXSTRINGLENGTH];
	int attrcnt;
	}viewDesc;

typedef struct {
	char viewname[MAXNAME];
	char viewattrname[MAXNAME];
	char relname[MAXNAME];
	char relattrname[MAXNAME];
	}viewAtrrDesc;

int compare_records(char *record1,char *record2,char *relation);
int insertInrelCat(char *relname,int relwidth,int attrcnt,int indexcnt,int rec_num);
int insertInattrCat(char *relname,char *attrname,int offset,int attrlength,char attrtype,int indexed,int indexno);
int isNumber(char *);
int isFloat(char *);
void create_relname(char *,char *,char *);
int find_operator(char *oper );
int print_results(char *filename,int recsize,int argc, char* argv[]);
int check_attributes(char *relation1,char *relation2,int attrcnt1);
int delete_record_from_indices(int record_id,char *record,char* relation);
int insert_record_into_indices(int record_id,char *record,char *relation);
int delete_attributes(char *relation);
int update_relations(int recid,char *record);
int update_attributes(int recid,char *record);
int giverelcatrecord(char* record,char *relation);
int giveattrcatrecord(char *record,char *relation,char *attribute);
void free_argument(char ***argument,int attrno);
int construct_argument(int argc,char *argv[],char *** argument,int *arg_num);

#endif
