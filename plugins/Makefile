build:
	gcc -o ../panel_plugins/clock.so -shared -fPIC `pkg-config --libs --cflags glib-2.0 gmodule-2.0 gtk+-3.0` clock.c -lside-plugin
	gcc -o ../wallpaper_plugins/desktop_clock.so -shared -fPIC `pkg-config --libs --cflags glib-2.0 gmodule-2.0 gtk+-3.0` desktop_clock.c -lside-plugin
	gcc -o ../panel_plugins/volume.so -shared -fPIC volume.c `pkg-config --libs --cflags glib-2.0 gmodule-2.0 gtk+-3.0` -lasound -lside-plugin
	gcc -o ../panel_plugins/user.so -shared -fPIC user.c `pkg-config --libs --cflags glib-2.0 gmodule-2.0 gtk+-3.0` -lside-plugin
	gcc -o ../panel_plugins/icon_app_button.so -shared -fPIC `pkg-config --libs --cflags glib-2.0 gmodule-2.0 gtk+-3.0` "icon_app_button.c" -lside-plugin
