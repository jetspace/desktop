build:
	gcc -c -Wall -Werror -fPIC plugin.c -o plugin.o -std=c99 -g `pkg-config --libs --cflags gtk+-3.0`
	gcc -shared -o side-plugin.so plugin.o
clean:
	rm plugin.o
	rm side-plugin.so
	tester
install:
	sudo mv side-plugin.so /usr/lib/libside-plugin.so
	sudo mkdir /usr/include/side -p
	sudo cp plugin.h /usr/include/side/
	rm plugin.o
tester:
	gcc -std=c99 -Wall test.c -o test -lside-plugin `pkg-config --libs --cflags gtk+-3.0`
