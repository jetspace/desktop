build:
	gcc -c -Wall -Werror -fPIC apps.c -o apps.o -std=c99
	gcc -shared -o side-apps.so apps.o
clean:
	rm test
	rm apps.o
	rm side-apps.so
install:
	sudo mv side-apps.so /usr/lib/libside-apps.so
	sudo mkdir /usr/include/side -p
	sudo cp apps.h /usr/include/side/
	rm apps.o
test:
	gcc -std=c99 test.c -o test -lside-apps
