adlist.o:adlist.c adlist.h
	gcc -g -c adlist.c
	
test:test.c adlist.o 
	gcc -g -c test.c
	gcc -o test test.o adlist.o


clean:
	rm -rf *.o
	rm -rf test adlist

