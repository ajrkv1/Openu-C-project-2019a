#include "header.h"
int DC = 0;/*holds the current Data Counter value*/
int IC = 0;/*holds the current Instuction counter value*/
int ERROR = 0;/*Error counter*/
int LC = 0;/*line counter*/

/*reboots all of the counters*/
void boot(){
	
	DC = 0;
	IC = 0;
	ERROR = 0;
	LC = 0;
}

