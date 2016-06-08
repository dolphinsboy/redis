sds.o:sds.c sds.h
	gcc -g -c sds.c

test:test.c sds.o
	gcc -g -c test.c
	gcc -o test sds.o test.o 

clean:
	rm -rf *.o
	rm -rf test
