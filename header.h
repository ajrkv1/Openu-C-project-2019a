#include <stdio.h>
#define OPCODE_LEN 16 
#define MAX_LINE_LEN 100 
#define OPERATIONS "mov","cmp","add","sub","not","clr","lea","inc","dec","jmp","bne","red","prn","jsr","rts","stop"
#define START_ADDRESS 100
#define MAX_LBL 31
#define MAX_LINE_LEN 100
#define MAX_COMM 7
#define INVALID_SYMBOLS_NUM 20
#define NUM_OF_COMM 16
#define BASE64 "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
#define FIRST6BITS 63


/*a union to hold an operand no matter what is it's type*/
typedef union operand{
	int num;
	char *lbl;
	int reg;
	} operand;

/*enum to define what type of operand are we using*/
typedef enum opType{NUM,REG,LBL,NONE} opType;

/*structure for symbol in symbol table*/
typedef struct symbol{
	 char *symlbl;
	int symaddress;
	int isData;
} symbol;

void boot();
int validSym(char *);
void addSymbol(char *,int,int);
int isNewSymbol(symbol);
void addExtern(char *);
int isNewExtern(char *);
void addEntry(char *);
int isnewEntry(char *);
void addData(int);
void addInstruction(int,operand,operand,opType,opType);
void finishSymbolTable();
void checkEntries();
void validateInst();
void createBinInst();
void createBinData();
void createExternalList();
void createEntryList();
void printLists(char *);
void computeLine(FILE *);
void freeAll();

extern int LC;
extern int DC;
extern int IC;
extern int ERROR;

/*bitfield for register operand type*/
typedef struct binReg{
	unsigned int are:2;
	unsigned int dest:5;
	unsigned int origin:5;
} binReg;

/*instruction word*/
typedef struct binInstruction{
	unsigned int are:2;
	unsigned int destOp:3;
	unsigned int opcode:4;
	unsigned int sourceOp:3;
} binInstruction;



typedef struct infoword{
	unsigned int are:2;
	signed int info:10;/*using signed because info can be negative*/
} infoword;

typedef struct dataword{
	int data:12;	
} dataword;

/*binary data list*/
typedef struct binDataNode{
	dataword data;
	struct binDataNode *next;
} binDataNode;

/*binary instruction list node*/
typedef struct binInstNode{
	union binInstOp{
		binInstruction inst;
		infoword op;
		binReg reg;
	}inst;
	enum instOp {INST,OP,REGI}instop;
	struct binInstNode *next;
} binInstNode;



typedef struct instructionNode{
		int opcode;
		operand op1;
		operand op2;
		opType optype1;
		opType optype2;
		int address;
		struct instructionNode *next;
} instructionNode;

typedef struct entryNode{
		char* entryName;
		struct entryNode *next;
} entryNode;

typedef struct externNode{
		char* externName;
		struct externNode *next;
} externNode;

typedef struct dataNode{
		int data;
		struct dataNode *next;
} dataNode;

typedef struct externListNode{
	char *name;
	int address;
	struct externListNode *next;
} externListNode;

typedef struct entryListNode{
	char *name;
	int address;
	struct entryListNode *next;
} entryListNode;

	




