build:
	gcc -std=c99 settings_panel.c -o ../settings_plugins/side-panel-settings.so -shared -fPIC `pkg-config --libs --cflags glib-2.0 gmodule-2.0 gtk+-3.0` -lside-apps -lside-plugin
	gcc -std=c99 settings_wallpaper.c -o ../settings_plugins/side-wallpaper-settings.so -shared -fPIC `pkg-config --libs --cflags glib-2.0 gmodule-2.0 gtk+-3.0` -lside-plugin
	gcc -std=c99 settings_explorer.c -o ../bin/side-settings-explorer `pkg-config --libs --cflags glib-2.0 gtk+-3.0` -lside-widgets -lside-plugin
	gcc -std=c99 settings_gtk.c -o ../settings_plugins/side-gtk-settings.so -shared -fPIC `pkg-config --libs --cflags gtk+-3.0 glib-2.0 gmodule-2.0` -lside-widgets -lside-plugin
	gcc -std=c99 settings_session.c -o ../settings_plugins/side-session-settings.so -shared -fPIC `pkg-config --libs --cflags gtk+-3.0 glib-2.0 gmodule-2.0` -lside-plugin
	gcc -std=c99 settings_mime.c -o ../settings_plugins/side-mime-settings.so -shared -fPIC `pkg-config --libs --cflags gtk+-3.0 glib-2.0 gmodule-2.0` -ljetspace-configkit -lside-plugin
	gcc -std=c99 settings_sysinfo.c -o ../settings_plugins/side-sysinfo-settings.so -shared -fPIC `pkg-config --libs --cflags gtk+-3.0 glib-2.0 gmodule-2.0` -lside-plugin
