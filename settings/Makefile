build:
	gcc -std=c99 settings_panel.c -o ../bin/side-panel-settings `pkg-config --libs --cflags glib-2.0 gmodule-2.0 gtk+-3.0` -lside-apps
	gcc -std=c99 settings_wallpaper.c -o ../bin/side-wallpaper-settings `pkg-config --libs --cflags glib-2.0 gmodule-2.0 gtk+-3.0`
	gcc -std=c99 settings_explorer.c -o ../bin/side-settings-explorer `pkg-config --libs --cflags glib-2.0 gtk+-3.0`
	gcc -std=c99 settings_gtk.c -o ../bin/side-gtk-settings `pkg-config --libs --cflags gtk+-3.0 glib-2.0`
	gcc -std=c99 settings_session.c -o ../bin/side-session-settings `pkg-config --libs --cflags gtk+-3.0 glib-2.0`
	gcc -std=c99 settings_mime.c -o ../bin/side-mime-settings `pkg-config --libs --cflags gtk+-3.0 glib-2.0` -ljetspace-configkit
