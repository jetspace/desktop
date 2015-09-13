build:
	gcc -c -Wall -Werror -fPIC plugin.c -o plugin.o -std=c99 -g `pkg-config --libs --cflags gtk+-3.0`
	gcc -shared -o ../../libs/libside-plugin.so plugin.o
	rm plugin.o
clean:
	rm test
tester:
	gcc -std=c99 -Wall test.c -o test -lside-plugin `pkg-config --libs --cflags gtk+-3.0`
