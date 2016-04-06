INCLUDE=/Users/guosong/Desktop/Sina/Code/redis/redis-2.6.17/test_redis_structure/sds
LIB=/Users/guosong/Desktop/Sina/Code/redis/redis-2.6.17/test_redis_structure/sds

test:test.c
	gcc -g -c test.c -I$(INCLUDE)
	gcc -o test test.o -L$(LIB) -lsds

clean:
	rm -rf *.o
	rm -rf test
