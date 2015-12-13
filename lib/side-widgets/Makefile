build:
	gcc -c -Wall -Werror -fPIC widgets.c -o widgets.o -std=c99 -g `pkg-config --libs --cflags gtk+-3.0`
	gcc -shared -o ../../libs/libside-widgets.so widgets.o
	rm widgets.o
	gcc -c -Wall -Werror -fPIC timelabel.c -o timelabel.o -std=c99 -g `pkg-config --libs --cflags gtk+-3.0`
	gcc -shared -o ../../libs/libside-timelabel.so timelabel.o
	rm timelabel.o
