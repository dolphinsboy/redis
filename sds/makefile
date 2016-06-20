sds.o:sds.c sds.h
	gcc -g -c sds.c

test:test.c sds.o
	gcc -g -c test.c
	gcc -o test sds.o test.o 

test_cond:test_cond.c
	gcc -g -c test_cond.c 
	gcc -o test_cond test_cond.o

clean:
	rm -rf *.o
	rm -rf test test_cond
