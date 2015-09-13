build:
	gcc -c -Wall -Werror -fPIC log.c -o log.o -std=c99 -g
	gcc -shared -o ../../libs/libside-log.so log.o
	rm log.o
clean:
	rm test
tester:
	gcc -std=c99 -Wall test.c -o test -lside-log
