main: passes.o lineComputing.o tables.o counters.o header.h
	gcc -g -pedantic -ansi -Wall passes.o header.h lineComputing.o tables.o counters.o -o main
passes.o: header.h passes.c
	gcc -c -pedantic -ansi -Wall passes.c -o passes.o
lineComputing.o: counters.o header.h tables.o lineComputing.c
	gcc -c -pedantic -ansi -Wall lineComputing.c -o lineComputing.o	
tables.o: header.h counters.o tables.c
	gcc -c -pedantic -ansi -Wall tables.c -o tables.o
counters.o: counters.c
	gcc -c -pedantic -ansi -Wall counters.c -o counters.o
	
