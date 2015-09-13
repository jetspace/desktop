build:
	gcc -c -Wall -Werror -fPIC apps.c -o apps.o -std=c99 -g
	gcc -shared -o ../../libs/libside-apps.so apps.o
	rm apps.o
clean:
	rm test
tester:
	gcc -std=c99 test.c -o test -lside-apps
