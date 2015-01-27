plugin:
	gcc -o plugin.so -shared -fPIC `pkg-config --libs --cflags glib-2.0 gmodule-2.0 gtk+-3.0` plugin.c
	sudo mv plugin.so /usr/lib/jetspace/panel/plugins/libplugin.so
no_plugin:
	sudo rm /usr/lib/jetspace/panel/plugins/libplugin.so
