main:main.o dict.a
	gcc -o main main.o dict.a

main.o:main.c
	gcc -c main.c

dict.a:dict.h dict.c
	gcc -c dict.c
	ar -rc dict.a dict.o

test:test.c
	gcc -o test test.c

clean:
	rm -rf main *.o dict.a test


