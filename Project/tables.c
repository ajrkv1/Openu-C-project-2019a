#include <stdlib.h>
#include <string.h>
#include "header.h"
#include <ctype.h>
#include <stdio.h>


/*a dynamic array to store the symbols*/ 
static symbol *symTable = NULL;
static int numOfSymbols = 0;

/*externs list + pointer*/
static externNode *exhead = NULL;
static externNode *exptr = NULL;

/*entries list + pointer*/
static entryNode *enhead = NULL;
static entryNode *enptr = NULL;

/*data list + pointer*/
static dataNode *dahead = NULL;
static dataNode *daptr = NULL;

/*instructions list + pointer*/
static instructionNode *insthead = NULL;
static instructionNode *instptr = NULL;

/*binary instructions list + pointer*/
static binInstNode *bihead=NULL;
static binInstNode *biptr = NULL;

/*binary data list + pointer*/
static binDataNode *bdhead = NULL;
static binDataNode *bdptr = NULL;

/*external with addresses list + pointer*/
static externListNode *elhead = NULL;
static externListNode *elptr = NULL;

/*entry with addresses list + pointer*/
static entryListNode *enlhead = NULL;
static entryListNode *enlptr = NULL;

/*base64 characters array*/ 
char base64[] = BASE64;

/*returns 1 if the lbl is invalid*/
int validSym(char *lbl){
	int i;
	char *invalid[] = {OPERATIONS,"data","string","extern","entry"};/*a string array containing all of the ivvalid symbol names*/
	if(strlen(lbl)>MAX_LBL)/*checks if the length of the symbol is not over the allowed*/
		return 1;	
	if(!isalpha(*lbl)){/*checkes if the first char is a letter*/
		return 1;
	}
	
	for(i=0;i<INVALID_SYMBOLS_NUM;i++)/*checkes if invalid*/
		if(strcmp(lbl,invalid[i])==0)
			return 1;

	while(*lbl!='\0'){/*chackes if all characters in the sym are allowed*/
		if(!isalnum(*lbl))
			return 1;
		lbl++;	
	}	
	
	return 0;
}

void addSymbol(char *lbl,int isData,int address){/*adds the symbol to the symtable*/
	symbol sym;

	if(numOfSymbols==0) /*if there are no symbols*/
		symTable = malloc(sizeof(symbol));	
	
	sym.symlbl = lbl;
	sym.symaddress = address;
	sym.isData = isData;/*ells if it is a data label for further notice*/
	if(validSym(lbl)){
		printf("\nError in line %d - invalid label\n",LC);
		ERROR++;
		return;	
	}	
	if(!isNewSymbol(sym)){
		printf("\nError in line %d - already defined Label\n",LC);
		ERROR++;
		return;
	}
			
	symTable = realloc(symTable,(++numOfSymbols)*sizeof(symbol));/*adding one space in symTable*/
	
	if(symTable==NULL){/*if reallocing went wrong*/
		printf("\nError at line %d - out of memory\n",LC);
		ERROR++;
		return;
	}	
	
	symTable[numOfSymbols-1] = sym;/*adding sym to the table*/
}

/*returns 1 if its a new symbol*/
int isNewSymbol(symbol sym){
	int  i;
	for(i=0;i<numOfSymbols;i++){
		if(strcmp(symTable[i].symlbl,sym.symlbl)==0)
			return 0;
	}
	return 1;
}

/*returns the address of lbl. returns -1 if lbl doesn't exist*/
int getLblAddress(char *lbl){
	int i;
	for(i=0;i<numOfSymbols;i++){
		if(strcmp(lbl,symTable[i].symlbl) == 0)/*if its the same label*/
			return symTable[i].symaddress;
	}
	return -1;
}

/*adding a label to the extern list*/
void addExtern(char *name){
	externNode ex;
	if(!isNewExtern(name)){
		printf("Error at line %d - extern already exists\n",LC);	
		ERROR++;
		return;
	}
	if(exhead==NULL){
		exhead = malloc(sizeof(externNode));/*create list if none exsists*/
		exptr = exhead;
	}else{
		exptr->next = malloc(sizeof(externNode));
		if(exptr->next==NULL){
			printf("\nError at line %d - out of memory\n",LC);
			ERROR++;
			return;
		}	
		exptr = exptr->next;
	}
	
	
	ex.externName = name;	
	ex.next = NULL;
	*exptr = ex;
	addSymbol(name,0,0);/*add to symbol table*/
}

/*returns 1 if ex is not in externs list*/  
int isNewExtern(char *ex){
	externNode *temp;
	if(exhead==NULL)
		return 1;
	temp = exhead;
	while(temp){
		if(strcmp(temp->externName,ex)==0)
			return 0;
	temp = temp->next;
	}
	return 1;
}

/*returns 1 if entry is not in entries list*/
int isNewEntry(char *entry){
	
	entryNode *temp;
	if(enhead==NULL)
		return 1;
	temp = enhead;
	while(temp){
		if(strcmp(temp->entryName,entry)==0)
			return 0;
		temp=temp->next;
	}
	return 1;
}

/*adds name to entryList*/
void addEntry(char *name){
	entryNode ent;	

	if(!isNewEntry(name)){/*checkes if new entry*/
		printf("Error at line %d - entry already exists\n",LC);
		ERROR++;
		return;
	}
	if(enhead==NULL){/*adds to list' creates one if none exsists*/
		enhead = malloc(sizeof(entryNode));
		enptr = enhead;
	}
	else{
		enptr = enhead;
		while(enptr->next)
			enptr = enptr->next;
		enptr->next = malloc(sizeof(entryNode));
		if(enptr->next==NULL){
			printf("\nError at line %d - out of memory\n",LC);
			ERROR++;
			return;
		}	
		enptr = enptr->next;
	}	
	
	
	ent.entryName = name;	
	ent.next = NULL;
	*enptr = ent;
}

/*adds data to data list*/
void addData(int data){
	
	dataNode dat;
	
	if(dahead==NULL){/*adds data to list. create list if none exsists*/
		dahead = malloc(sizeof(dataNode));
		daptr = dahead;
	}
	else{
		daptr->next = malloc(sizeof(dataNode));
		if(daptr->next==NULL){
			printf("\nError at line %d - out of memory\n",LC);
			ERROR++;
			return;
		}
		daptr=daptr->next;	
	}
	dat.data = data;	
	dat.next = NULL;	
	*daptr = dat;
	DC++;/*adds one to the data counter*/
}

/*add an instruction to the instruction list*/ 
void addInstruction(int opcode,operand op1,operand op2, opType optype1, opType optype2){
	instructionNode inst;
	inst.address = IC+START_ADDRESS;
	if(!insthead){/*adds one space to the list. creates one if none exsists.*/
		insthead = malloc(sizeof(instructionNode));	
		instptr = insthead;
	}else{
		instptr->next = malloc(sizeof(instructionNode));
		if(instptr->next==NULL){
			printf("\nError at line %d - out of memory\n",LC);
			ERROR++;
			return;
		}
		instptr=instptr->next;	
	}
	inst.opcode = opcode;
	inst.op1 = op1;
	inst.op2 = op2;
	inst.optype1 = optype1;
	inst.optype2 = optype2;
	inst.next = NULL;	
	*instptr = inst;
	IC++;
	if(optype1!=NONE)/*increases IC by one for each of the operands*/
		IC++;
	if(optype2!=NONE&&!(optype1==REG&&optype2==REG))
		IC++;
}

/*finishes the symbol table by adding IC to all of the data symbols*/
void finishSymbolTable(){
	int i;
	for(i=0;i<numOfSymbols;i++){
		if(symTable[i].isData)
			symTable[i].symaddress+=(IC+START_ADDRESS);
	}
}

/*returns 1 if ex is in the extern list*/
int isEx(char *ex){
	externNode *temp = exhead;
	while(temp){
		if(strcmp(temp->externName,ex)==0)
			return 1;
		temp = temp->next;
	}
	return 0;
}

/*checks if all the entries are in the symTable*/
void checkEntries(){
	int  i;	
	int found = 0;
	enptr = enhead;
	if(enptr==NULL)
		return;
	while(enptr){/*checks for each one*/
		for(i=0;i<numOfSymbols;i++){
			if(strcmp(enptr->entryName,symTable[i].symlbl)==0)
				found=1;
		}
		if(!found){
			printf("\nError - entry symbol %s does not corespond to a symbol",enptr->entryName);
			ERROR++;
			return;
		}
		enptr=enptr->next;
	}
}

/*creates binary instruction list*/ 
void createBinInst(){
	if(!insthead)
		return;
	bihead = malloc(sizeof(binInstNode));
	biptr = bihead;
	instptr = insthead;
	while(instptr){/*passes on all of the instruction list*/
		biptr->inst.inst.opcode = instptr->opcode;
		biptr->instop = INST;
		switch (instptr->optype1){/*decides the destinating type of the operands*/
			case NUM:
				biptr->inst.inst.sourceOp = 1;
				break;
			case LBL:
				biptr->inst.inst.sourceOp = 3;
				break;		
			case REG:
				biptr->inst.inst.sourceOp = 5;
				break;		
			case NONE:
				biptr->inst.inst.sourceOp = 0;
				break;		
		}	

		switch (instptr->optype2){
			case NUM:
				biptr->inst.inst.destOp = 1;
				break;
			case LBL:
				biptr->inst.inst.destOp = 3;
				break;		
			case REG:
				biptr->inst.inst.destOp = 5;
				break;		
			case NONE:
				biptr->inst.inst.destOp = 0;
				break;		
		}		
		
		/*are is set in instruction to 0*/
		biptr->inst.inst.are = 0;
		
		/*creates a new node to store the operand's code, and enters the data acording to the type*/
		if(instptr->optype1!=NONE){
			biptr->next = malloc(sizeof(binInstNode));
			biptr = biptr->next;
			biptr->instop = OP;
		}
		if(instptr->optype1==NUM){
			biptr->inst.op.are = 0;
			biptr->inst.op.info = instptr->op1.num;
		}
		if(instptr->optype1==LBL){
			int address = getLblAddress(instptr->op1.lbl);
			if(isEx(instptr->op1.lbl)){
				biptr->inst.op.are = 1;
				biptr->inst.op.info = 0;
			}
			else {
				biptr->inst.op.are = 2;
				biptr->inst.op.info = address;
			}
		}
		if(instptr->optype1==REG){
			biptr->inst.reg.are = 0;
			biptr->inst.reg.origin = instptr->op1.reg;
			if(instptr->optype2==REG){/*if both are reg only one word will be used*/
				biptr->inst.reg.dest = instptr->op2.reg;
			}else{
				biptr->inst.reg.dest = 0;
				}
			biptr->instop = REGI;
		}
	
		if(instptr->optype2!=NONE&&!(instptr->optype1==REG&&instptr->optype2==REG)){
			biptr->next = malloc(sizeof(binInstNode));
			biptr = biptr->next;
			biptr->instop = OP;
				
			if(instptr->optype2==NUM){
				biptr->inst.op.are = 0;
				biptr->inst.op.info = instptr->op2.num;
			}
			if(instptr->optype2==LBL){
				int address = getLblAddress(instptr->op2.lbl);
				if(isEx(instptr->op2.lbl)){
					biptr->inst.op.are = 1;
					biptr->inst.op.info = 0;
				}
				else {
					biptr->inst.op.are = 2;
					biptr->inst.op.info = address;
				}
				
			}
			if(instptr->optype2==REG){
				biptr->inst.reg.are = 0;
				biptr->inst.reg.dest = instptr->op2.reg;
				biptr->instop = REGI;
	
			}
	}
		if(instptr->next){/*creates a node for the next instruction*/
			biptr->next = malloc(sizeof(binInstNode));
			biptr = biptr->next;
		}	
		instptr=instptr->next;
	}
}

/*creates the binary data list according to the regular data list*/
void createBinData(){
	if(!dahead)
		return;
	bdhead = malloc(sizeof(binDataNode));
	bdptr = bdhead;
	daptr = dahead;
	while(daptr){
		bdptr->data.data = daptr->data;
		daptr = daptr->next;
		if(daptr){
			bdptr->next = malloc(sizeof(binDataNode));
			bdptr=bdptr->next;
		}
	}
}

/*creates the external list to be printed to .ext file*/
void createExternalList(){
	if(!exhead)/*if there are no externs it will not be made at all*/
		return;
	instptr = insthead;
	while(instptr){
		if(instptr->optype1 == LBL){
			if(isEx(instptr->op1.lbl)){/*if it find an extern it makes a new node and writes extern's name and address*/
				if(elhead==NULL){
					elhead = malloc(sizeof(externListNode));
					elptr =elhead;	
				}else{
					elptr->next = malloc(sizeof(externListNode));
					elptr = elptr->next;
				}
				elptr->name = instptr->op1.lbl;
				elptr->address = instptr->address+1;/*gets the instruction's ptr and adds 1 to get the operand's address*/
			}
		}

		if(instptr->optype2 == LBL){
			if(isEx(instptr->op2.lbl)){
				if(elhead==NULL){
					elhead = malloc(sizeof(externListNode));
					elptr =elhead;	
				}else{
					elptr->next = malloc(sizeof(externListNode));
					elptr = elptr->next;
				}
				elptr->name = instptr->op2.lbl;
				if(instptr->optype1!=NONE)/*if it is the second operand the address will be added by 2 to get the second operand. elsewhere the it will be added by 1*/ 
					elptr->address = instptr->address+2;
				else
					elptr->address = instptr->address+1;
			}
		}
		instptr = instptr->next;
	}
}

/*creates the entry's list to be printed by finding the address of each label defined as entry*/ 
void createEntryList(){
	int i;	
	if(!enhead)
		return;
	enlhead = malloc(sizeof(entryListNode));
	enlptr = enlhead;
	enptr = enhead;
	while(enptr!=NULL){	
		for(i=0;i<numOfSymbols;i++){
			if(strcmp(enptr->entryName,symTable[i].symlbl)==0){
				enlptr->name = 	symTable[i].symlbl;
				enlptr->address = symTable[i].symaddress;
			}
		}		
		enptr = enptr->next;
		if(enptr!=NULL){	
			enlptr->next = malloc(sizeof(entryListNode));		
			enlptr = enlptr->next;	
		}
	}

}

/*prints the entries list to .ent file*/ 
void printEntries(char *filename){
	FILE *entfp;	
	if(enlhead == NULL)/*if there are no entries file won't be opened*/
		return;
	entfp = fopen(strcat(filename,".ent"),"w+");/*opens a file*/
	if(entfp==NULL){/*if file didnwt open*/
		printf("couldn't open file %s",filename);
		return;
	}
		
	enlptr = enlhead;	
	while(enlptr){
		fprintf(entfp,"%s\t%d\n",enlptr->name,enlptr->address);/*prints the entries by the following format:	ENTRY_NAME	ADDRESS*/	
		enlptr=enlptr->next;
	}
}	

/*prints the extern list to .ext file*/
void printExterns(char *filename){
	FILE *extfp;	
	if(elhead==NULL)/*if there are no externs file won't be opened*/
		return;
	extfp = fopen(strcat(filename,".ext"),"w+");
	elptr = elhead;	
	while(elptr){
		fprintf(extfp,"%s\t%d\n",elptr->name,elptr->address);/*prints the externs by the following format:	EXTERN_NAME	ADDRESS*/
		elptr=elptr->next;
	}
}			

/*gets the first Base64 corresponding char to be printed in .ob file*/ 
char getFirstBase64(void *bin){
	int num = (*((int *)bin) & FIRST6BITS);
	return base64[num];
}

/*gets the first Base64 corresponding char to be printed in .ob file*/ 
char getSecondBase64(void *bin){ 
	int num = ((*((int *)bin)>>6) & FIRST6BITS);
	return base64[num];
}

/*prints the .ob file*/
void printObject(char *filename){
	
	FILE *obfp;
	char c1,c2;	
	void *ptr;
	if(bihead==NULL&&bdhead==NULL)
		return;	
	obfp = fopen(strcat(filename,".ob"),"w+");
	fprintf(obfp,"%d\t%d\n",IC,DC);
	/*prints the instruction section*/
	if(bihead!=NULL){
		biptr = bihead;
		while(biptr){
			ptr = &(biptr->inst);
			c1 = getFirstBase64(ptr);
			c2 = getSecondBase64(ptr);
			fprintf(obfp,"%c%c\n",c2,c1);
			biptr = biptr->next;
		}
	}
	/*prints the data section*/
	if(bdhead!=NULL){
		bdptr = bdhead;	
		while(bdptr){
			c1 = getFirstBase64(&(bdptr->data));		
			c2 = getSecondBase64(&(bdptr->data));
			fprintf(obfp,"%c%c\n",c2,c1);
			bdptr = bdptr->next;
		}
	}
}

/*prints all the files*/
void printLists(char *filename){
	char *ent = malloc(100*sizeof(char));
	char *ext = malloc(100*sizeof(char));
	char *ob = malloc(100*sizeof(char));
	strcpy(ent,filename);/*copying so filename wont be changed*/ 
	strcpy(ext,filename);
	strcpy(ob,filename);
	printEntries(ent);
	printExterns(ext);
	printObject(ob);
}
 
/*frees all of the lists*/
void freeAll(){
	if(numOfSymbols!=0){
		free(symTable);
	}
	
	if(exhead!=NULL){
		exptr = exhead->next;
		while(exptr!=NULL){
			free(exhead);
			exhead = exptr;
			exptr = exptr->next;
		}
		free(exhead);
	}
	
	if(enhead!=NULL){  
		enptr = enhead->next;
		while(enptr){
			free(enhead);
			enhead = enptr;
			enptr = enptr->next;
		}
		free(enhead);
	}
	
	if(dahead!=NULL){	
		daptr = dahead->next;
		while(daptr!=NULL){
			free(dahead);
			dahead = daptr;
			daptr = daptr->next;
		}
		free(dahead);
	}	

	if(insthead!=NULL){
		instptr = insthead->next;
		while(instptr){
			free(insthead);
			insthead = instptr;
			instptr = instptr->next;
		}
		free(insthead);
	}
	
	
	if(bihead!=NULL){
		biptr = bihead->next;
		while(biptr){
			free(bihead);
			bihead = biptr;
			biptr = biptr->next;
		}
		free(bihead);
	}
	
	if(bdhead!=NULL){
		bdptr = bdhead->next;
		while(bdptr){
			free(bdhead);
			bdhead = bdptr;
			bdptr = bdptr->next;
		}
		free(bdhead);
	}
	

	if(elhead!=NULL){
		elptr = elhead->next;
		while(elptr){
			free(elhead);
			elhead = elptr;
			elptr = elptr->next;
		}
		free(elhead);
	}
	
	if(enlhead!=NULL){
		enlptr = enlhead->next;
		while(enlptr){
			free(enlhead);
			enlhead = enlptr;
			enlptr = enlptr->next;
		}
		free(enlhead);
	}
	
}


