build:
	gcc -o ../clock.so -shared -fPIC `pkg-config --libs --cflags glib-2.0 gmodule-2.0 gtk+-3.0` clock.c
	gcc -o ../volume.so -shared -fPIC volume.c `pkg-config --libs --cflags glib-2.0 gmodule-2.0 gtk+-3.0` -lasound
