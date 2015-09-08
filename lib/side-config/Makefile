build:
	gcc -c -std=c11 -Wall -Werror -fPIC  config.c -o config.o
	gcc -shared -o side-config.so config.o
install:
	sudo mv side-config.so /usr/lib/libside-config.so
	sudo mkdir /usr/include/side -p
	sudo cp config.h /usr/include/side/
	rm config.o
tester:
	gcc -std=c99 test.c -o test -lside-config
