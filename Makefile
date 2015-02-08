run:
	dbus-launch	side-panel &
	dbus-launch	side-wallpaper-service &
install:
	sudo cp gsettings/org.jetspace.desktop.panel.gschema.xml /usr/share/glib-2.0/schemas/
	sudo glib-compile-schemas /usr/share/glib-2.0/schemas/
	sudo mkdir /usr/lib/jetspace -p
	sudo mkdir /usr/lib/jetspace/panel -p
	sudo mkdir /usr/lib/jetspace/panel/plugins -p
	sudo cp desktop/side-settings-explorer.desktop /usr/share/applications/
	sudo mv side-wallpaper-service /bin/
	sudo mv side-wallpaper-settings /bin/
	sudo mv side-panel /bin/
	sudo mv side-panel-settings /bin/
	sudo mv side-settings-explorer /bin/
clean:
	rm side-panel
	rm side-wallpaper-service
	rm side-panel-settings
	rm side-wallpaper-settings
	rm side-settings-explorer
