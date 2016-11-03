all:
	gcc   test.c -o test -I./src -Lsrc/ -lkbus  -lpthread
clean:
	rm test
