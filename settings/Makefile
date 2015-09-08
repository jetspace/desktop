build:
	gcc -std=c99 settings_panel.c -o ../side-panel-settings `pkg-config --libs --cflags glib-2.0 gmodule-2.0 gtk+-3.0` -lside-apps
	gcc -std=c99 settings_wallpaper.c -o ../side-wallpaper-settings `pkg-config --libs --cflags glib-2.0 gmodule-2.0 gtk+-3.0`
	gcc -std=c99 settings_explorer.c -o ../side-settings-explorer `pkg-config --libs --cflags glib-2.0 gtk+-3.0`
	gcc -std=c99 settings_gtk.c -o ../side-gtk-settings `pkg-config --libs --cflags gtk+-3.0 glib-2.0`
	gcc -std=c99 settings_session.c -o ../side-session-settings `pkg-config --libs --cflags gtk+-3.0 glib-2.0`
	gcc -std=c99 settings_mime.c -o ../side-mime-settings `pkg-config --libs --cflags gtk+-3.0 glib-2.0` -lside-config
