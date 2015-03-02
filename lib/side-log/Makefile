build:
	gcc -c -Wall -Werror -fPIC log.c -o log.o -std=c99 -g
	gcc -shared -o side-log.so log.o
clean:
	rm test
	rm log.o
	rm side-log.so
install:
	sudo mv side-log.so /usr/lib/libside-log.so
	sudo mkdir /usr/include/side -p
	sudo cp log.h /usr/include/side/
	rm log.o
tester:
	gcc -std=c99 -Wall test.c -o test -lside-log
