#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*buffer to hold the line*/
char *buf = NULL;
/*pointer to start of buf so it could be freed*/
char *bufhead = NULL;
/*writes the line from fp to buf char at a time until hits enter or EOF. returns 1 if hits EOF*/ 
int readLine(FILE *fp){
	int i;	
	int c;
	buf = (char *)malloc(MAX_LINE_LEN*sizeof(char));
	for(i=0;i<MAX_LINE_LEN;i++){		
		c = fgetc(fp);
		buf[i] = c;
		if(c=='\n'){
			*(buf+i+1) = '\0';
			return 0;
		}
		if(c == EOF){
			buf[i] = '\n';
			buf[i+1] = '\0';
			return 1;
		}
	
	}
	return 1;
}	
/*returns the label of the line. if there is none it returns NULL*/
char *findlbl(){
	char *lbl = malloc(MAX_LBL*sizeof(char));
	strcpy(lbl,buf);/*copies buf so buf wont be changed*/
	strtok(lbl,":");
	if(strcmp(lbl,buf)==0)
		return NULL;
	return lbl;
}			 
/*skips the current word*/
void skipWord(){
	while(*buf!=' '&&*buf!='\t'&&*buf!='\n'&&*buf!=',')
			buf++;
}
/*returns 1 if buf is at the end of a line*/
int isEmpty(){
	if(*buf == EOF||*buf == '\n')
		return 1;
	return 0;
}
/*skips all blank char*/
void skipblanks(){
	while(*buf=='\t'||*buf==' ')
		buf++;
}
/*returns 0 if buf points to ','*/
int noComma(){
	return *buf!=','? 1:0;
}
/*returns 1 if buf points to '.' which tells buf is a guide sentence*/
int isGuide(){
	return *buf=='.'? 1:0;
}
/*returns 1 if buf is a lower case letter, which means its an instruction sentence*/
int isInstruct(){
	return islower(*buf)? 1:0;
}
/*returns 1 if buf starts with ';' which means its a ducumentation sentence*/
int isDoc(){
	return *buf==';'? 1:0;
}
/*skips until ':' - skips the label*/
void skiplbl(){
		while(*buf!=':'&&*buf!='\n')
			buf++;
		buf++;
}
/*skips to the next comma*/
void skipToComma(){
	while(*buf!=','&&*buf!='\n'&&*buf!=EOF)
		buf++;
	if(*buf==',')	
		buf++;
}
/*returns 1 if comm is one of the 16 legal commands*/
int validComm(char comm[]){
	char *commands[] ={OPERATIONS};/*an array with the legal operations*/
	int i;
	int found = 0;
	for(i=0;i<NUM_OF_COMM;i++){
		if(strcmp(commands[i],comm)==0)
			found = 1;
	}
	return found;
}
/*return the next registers number if buf is pointing to a register call*/
int getReg(){
	buf++;/*skips to the next char*/
	if(*buf!='r'){
		printf("Error at line %d - invalid register",LC);
		ERROR++;
		return -1;
	}	
	buf++;
	if(*buf<'0'||*buf>'7'){/*if the num is not between 0 and 7 it is not a leagl register*/
		printf("Error at line %d - invalid register",LC);
		ERROR++;
		return -1;
	}
	return atoi(buf);/*returns the num of the registers digit*/
}		
/*returns the next word*/
char *getWord(){
	char *lbl = malloc(MAX_LINE_LEN*sizeof(char));/*a string to hold the word*/
	int i;
	/*copies letter to letter until end of the word*/
	for(i=0;buf[i]!=' '&&buf[i]!='\t'&&buf[i]!='\n'&&buf[i]!='\0'&&buf[i]!=','&&buf[i]!=EOF;i++){
			lbl[i] = buf[i];
	
	}
	lbl[i] = '\0';
	if(strcmp("",lbl)==0)/*if there is no word*/
		lbl = NULL;
	return lbl;	
}
/*returns the opcode of command comm.returns -1 if comm is invalid*/
int getOpcode(char *comm){
	char *commands[] = {OPERATIONS};
	int i;
	for(i=0;i<NUM_OF_COMM;i++){
		if(strcmp(commands[i],comm)==0)
			return i;
	}
	return -1;
}
/*returns the sum of the origin operand desting methods nums*/
int getOrigin(char *comm){
	if(!strcmp(comm,"cmp")||!strcmp(comm,"mov")||!strcmp(comm,"add")||!strcmp(comm,"sub"))
		return 9;
	if(!strcmp(comm,"lea"))
		return 3;
	return 0;
}	
/*returns the sum of the destination operand desting methods nums*/		
int getDest(char *comm){
	if(!strcmp(comm,"prn")||!strcmp(comm,"cmp"))
		return 9;
	if(!strcmp(comm,"stop")||!strcmp(comm,"rts"))
		return 0;
	return 8;
}

	
/*computes the instruction lines*/		
void computeInstruct(){	
	char *comm;
	operand op1, op2;
	opType optype1,optype2;
	int dest;
	int origin;		
	int opcode;
	comm = getWord();/*gets the command*/
	if(!validComm(comm)){
		printf("\nError in line %d - invalid command\n",LC);
		ERROR++;
		return;
	}
	/*gets origin and destination*/
	origin = getOrigin(comm);
	dest = getDest(comm);
	/*skips to first operand*/
	skipWord();
	skipblanks();
	/*gets origin of num*/
	if(origin){
		if(origin == 9){
			if(isdigit(*buf)||((*buf == '-'||*buf == '+')&&isdigit(*(buf+1)))){/*if it is a valid num*/
				op1.num = atoi(buf);
				optype1 = NUM;
			}
			else if(*buf == '@'){/*if it is a register*/
				op1.reg = getReg();
				optype1 = REG;
			}
			else if(isalpha(*buf)){/*if it is a label*/
				optype1 = LBL;
			}
		}
		if(origin == 3){
			if(isalpha(*buf)){
				op1.lbl = getWord();
				optype1 = LBL;
			}
		
			if(*buf == '@'){
				printf("\nError at line %d - %s cannot get a register as input\n",LC,comm);
				ERROR++;
				return;
			}
			if(isdigit(*buf)||((*buf == '-'||*buf == '+')&&isdigit(*(buf+1)))){
				printf("\nError at line %d - %s cannot get a number as input\n",LC,comm);
				ERROR++;
				return;
			}
		}
		
		skipToComma();/*skips to next operand*/
		skipblanks();
	}else{
		optype1 = NONE;
	}
	
	if(dest){
		if(dest==8||dest==9){
			if(*buf == '@'){/*gets register*/
				op2.reg = getReg();
				optype2 = REG;
			}
			else if(isalpha(*buf)){/*gets lbl*/
				op2.lbl = getWord();
				optype2 = LBL;
			}
		}
		if(dest == 9){
			if(isdigit(*buf)||((*buf == '-'||*buf == '+')&&isdigit(*(buf+1)))){/*if valid num gets num*/
				op2.num = atoi(buf);
				optype2 = NUM;
			}
		}
		if(dest ==8&&optype2!=LBL&&optype2!=REG){
			if(isdigit(*buf)||((*buf == '-'||*buf == '+')&&isdigit(*(buf+1)))){
printf("\nbuf is %s",buf);
				printf("\nError at line %d - %s cannot get a number as input\n",LC,comm);
				ERROR++;
				return;
			}
		}
		skipWord();			
	}
	else{
		optype2 = NONE;
	}

	opcode = getOpcode(comm);
	if(opcode == -1){
		printf("\nError at line %d - no such command\n",LC);
		ERROR++;
		return;
	}
	
	skipblanks();
	if(*buf!='\n'){/*if buf didnt reach the end of the file*/
		printf("\nError at line %d - text after instruction\n",LC);
		ERROR++;
		return;
	}
	addInstruction(opcode, op1, op2, optype1, optype2);/*adds the instruction to the instruction list*/		
	
}
/*returns 1 if the command is "entry"*/
int isEntry(){
	char *temp = getWord();/*gets the next word from buf*/
	if(strcmp(temp,"entry")==0)/*chacks if it is "entry"*/
		return 1;
	return 0;
}	
	
/*computes entry command*/
void computeEntry(){
	
	char *entryName;	
	skipWord();/*skips the comman "entry"*/
	skipblanks();
	entryName = getWord();/*reads entry's operand*/
	skipWord();
	skipblanks();
	if(*buf!='\n'){/*if buf didnt reach the end of the file*/
		printf("\nError at line %d - text after entry\n",LC);
		ERROR++;
		return;
	}
	if(validSym(entryName)){/*validating if the Sym is valid*/
		printf("\nError at line %d - invalid entry\n",LC);
		ERROR++;
		return ;
	}
	addEntry(entryName);/*adds entry to EntryList*/
}
/*computes the extern command*/
void computeExtern(){
	char *externName;	
	skipWord();
	skipblanks();
	externName = getWord();/*gets the externs operand*/ 
	skipWord();
	skipblanks();
	if(*buf!='\n'){
		printf("\nError at line %d - text after extern\n",LC);
		ERROR++;
		return;
	}
	if(validSym(externName)){
		printf("\nError atline %d - invalid extern\n",LC);
		ERROR++;
		return ;
	}
	addExtern(externName);/*adds to extern list*/
}
/*returns 1 if its an extern command*/
int isExtern(){
	char *temp = getWord();
	if(strcmp(temp,"extern")==0)
		return 1;
	return 0;
}	
/*computes data command*/
void computeData(){
	int data;
	skipWord();
	skipblanks();
	while(*buf!='\n'&&*buf!=EOF){/*works until buf reaches end of line*/
		if(!isdigit(*buf)&&!(((*buf!='+')||(*buf!='-'))&&(isdigit(*(buf+1))))){/*if its not a legal number prints error*/
			printf("\nError at line %d - invalid parameter to data command\n",LC);
			ERROR++;
			return;
		}
		data = atoi(buf);/*gets the data from the number*/
		addData(data);/*adds to data list*/
		skipWord();	
		skipblanks();
		if(noComma()&&*buf!='\n'&&*buf!=EOF){/*checks if there's comma or end of line*/
			printf("\nError at line %d - no comma\n",LC);
			ERROR++;
			return;
		}
		if(*buf==',')/*if it is comma skip the comma*/
			buf++;
		skipblanks();
		
	}
}	
/*returns 1 if it is data*/
int isData(){
	char *temp = getWord();
	if(strcmp(temp,"data")==0)
		return 1;
	return 0;
}
/*returns 1 if it is a string command*/
int isString(){
	char *temp = getWord();
	if(strcmp(temp,"string")==0)
		return 1;
	return 0;
}
/*computes string command*/
void computeString(){
	int ch;
	skipWord();
	skipblanks();
	if(*buf!='\"'){/*if buf is not '"' prints error*/
			printf("\nError at line %d - .string recives a string\n",LC);
			ERROR++;
			return;
		}
		buf++;
	
	while(*buf!='\"'){/*until the end of the string*/
		if(*buf=='\n'||*buf==EOF){/*prints error if there is no '"' at the end*/
			printf("\nError at line %d - string not closen\n",LC);
			ERROR++;
			return;
		}
		
		ch = *buf;
		buf++;
		addData((int)ch);/*adds the current char to the data list*/
	}
	buf++;
	skipblanks();
	if(*buf!='\n'){
		printf("\nError at line %d - text after command\n",LC);
			ERROR++;
			return;
	}
	addData(0);/*adds '0' to tell end of string*/ 
}	
/*computes the line and chooses between computing functions*/
void computeLine(FILE *fp){
	char *lbl;
	int isend = readLine(fp);/*reads the line*/
	bufhead = buf;
	LC++;/*moves the line counter by 1*/
	skipblanks();
	if(!isEmpty()&&!isDoc()){/*if it is not a documentation or an empty line*/ 
		lbl = findlbl();
		if(lbl){
			skiplbl();
		}
		skipblanks();
		if(isGuide()){/*if it is a guide command*/
			buf++;
			if(isEntry()){
				computeEntry();
			}
			else if(isExtern()){
				computeExtern();
				}
			else if(isData()){
				if(lbl)
					addSymbol(lbl,1,DC);/*add lbl to symbol table*/ 
				computeData();
			}
			else if(isString()){				
				if(lbl)
					addSymbol(lbl,1,DC);
				computeString();
			}
			else{/*if it is an unknown command*/
				printf("Error at line %d - Unknown command",LC);
				ERROR++;
				return;
			}	
		}else if(isInstruct()){
				if(lbl!=NULL)
					addSymbol(lbl,0,IC+START_ADDRESS);
				computeInstruct();
			}
		else{
				printf("Error at line %d - Unknown command",LC);
				ERROR++;
				return;
			}
	skipblanks();
	free(bufhead);
	}
	if(!isend)/*if it is not the end of the file*/
		computeLine(fp);
}			














