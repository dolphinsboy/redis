dict.o:dict.c dict.h
	gcc -g -c dict.c

test:test.c dict.o
	gcc -g -c test.c
	gcc -o test test.o dict.o

clean:
	rm -rf *.o
	rm -rf test 
