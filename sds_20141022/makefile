test_sds:test_sds.o sds.a
	gcc -o test_sds test_sds.o sds.a

test_sds.o:
	gcc -c -g test_sds.c

sds.a: sds.h sds.c
	gcc -c sds.c
	ar -rc sds.a sds.o

clean:
	rm -rf test_sds test_sds.o

