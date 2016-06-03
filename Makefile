all:
	gcc -o test test.c log.c -g -lpthread
clean:
	-rm *.txt
	-rm test
