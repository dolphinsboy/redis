ae.o:ae.h ae.c ae_epoll.c config.h
	gcc -g -c ae.c

ctab:ctab.c ae.o
	gcc -g -c ctab.c
	gcc -o ctab ae.o ctab.o

clean:
	rm -rf ctab ae.o
