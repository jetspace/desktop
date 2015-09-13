build:
	gcc -c -std=c99 -Wall -Werror -fPIC  config.c -o config.o
	gcc -shared -o ../../libs/libside-config.so config.o
	rm config.o
tester:
	gcc -std=c99 test.c -o test -lside-config
