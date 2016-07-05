SDS=/home/guosong/myredis_study/sds/

dict.o:dict.c dict.h
	gcc -g -c dict.c

test:test.c dict.o
	gcc -g -c test.c -I ${SDS}
	gcc -o test dict.o ${SDS}/sds.o test.o

clean:
	rm -rf *.o
	rm -rf test 
