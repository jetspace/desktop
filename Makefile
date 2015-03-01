run:
	dbus-launch	side-panel &
	dbus-launch	side-wallpaper-service &
install:
	sudo cp gsettings/org.jetspace.desktop.panel.gschema.xml /usr/share/glib-2.0/schemas/
	sudo cp gsettings/org.jetspace.desktop.session.gschema.xml /usr/share/glib-2.0/schemas/
	sudo cp gsettings/org.jetspace.desktop.search.gschema.xml /usr/share/glib-2.0/schemas/
	sudo glib-compile-schemas /usr/share/glib-2.0/schemas/
	sudo mkdir /usr/lib/jetspace -p
	sudo mkdir /usr/lib/jetspace/panel -p
	sudo mkdir /usr/lib/jetspace/panel/plugins -p
	sudo mkdir /usr/share/icons/jetspace -p
	sudo cp icons/JetSpace.png /usr/share/icons/jetspace/
	sudo cp icons/speaker.png /usr/share/icons/jetspace/
	sudo cp desktop/side-settings-explorer.desktop /usr/share/applications/
	sudo cp desktop/side.desktop /usr/share/xsessions/
	sudo mv side-wallpaper-service /bin/
	sudo mv side-wallpaper-settings /bin/
	sudo mv side-panel /bin/
	sudo mv side-panel-settings /bin/
	sudo mv side-settings-explorer /bin/
	sudo mv side-gtk-settings /bin/
	sudo mv side-session-settings /bin/
	sudo mv side-session /bin/
	sudo mv side-notifyd /bin/
	sudo mv side-notify-popup /bin/
	sudo mv side-search /bin/
	sudo mv clock.so /usr/lib/jetspace/panel/plugins/
	sudo mv volume.so /usr/lib/jetspace/panel/plugins/
	sudo mv user.so /usr/lib/jetspace/panel/plugins/
clean:
	rm side-panel
	rm side-wallpaper-service
	rm side-panel-settings
	rm side-wallpaper-settings
	rm side-settings-explorer
	rm volume.so
	rm clock.so
	rm side-session
