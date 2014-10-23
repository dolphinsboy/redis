test_list:test_list.o adlist.a
	gcc -o test_list test_list.o adlist.a

test_list.o:test_list.c
	gcc -c -g test_list.c

adlist.a: adlist.h adlist.c
	gcc -c adlist.c
	ar -rc adlist.a adlist.o

clean:
	rm -rf test_list test_list.o

