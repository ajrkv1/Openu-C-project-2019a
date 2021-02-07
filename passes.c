#include "header.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*a fuction that does the first pass*/
void firstPass(FILE *fp){	
	boot();	
	computeLine(fp);
	finishSymbolTable();	
}
/*a function to do the second pass*/
void secondPass(char *filename){
	checkEntries();
	createBinInst();
	createBinData();
	createExternalList();
	createEntryList();
	printLists(filename);
	freeAll();
	
}
int main(int argc,char *argv[]){
	FILE *fp;
	char *filename = malloc(100*sizeof(char));
	strcpy(filename,argv[1]);	
	fp = fopen(strcat(filename,".as"),"r");
	/*prints an error if file cant be opened*/
	if(!fp){
		printf("couldn't open file %s",filename);
		return 1;
	}
	/*passes the first pass*/
	firstPass(fp);
	/*if Errors were found in the first pass they will be added to 'ERROR' counter that is 0 to begin with*/
	if(ERROR==0)
		secondPass(argv[1]);
	return 0;
}		
