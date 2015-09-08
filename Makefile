run:
	dbus-launch	side-panel &
	dbus-launch	side-wallpaper-service &
install:
	sudo cp gsettings/org.jetspace.desktop.panel.gschema.xml /usr/share/glib-2.0/schemas/
	sudo cp gsettings/org.jetspace.desktop.session.gschema.xml /usr/share/glib-2.0/schemas/
	sudo cp gsettings/org.jetspace.desktop.search.gschema.xml /usr/share/glib-2.0/schemas/
	sudo cp gsettings/org.jetspace.desktop.editor.gschema.xml /usr/share/glib-2.0/schemas/
	sudo glib-compile-schemas /usr/share/glib-2.0/schemas/
	sudo mkdir /usr/lib/jetspace -p
	sudo mkdir /usr/lib/jetspace/panel -p
	sudo mkdir /usr/lib/jetspace/panel/plugins -p
	sudo mkdir /usr/share/icons/jetspace -p
	sudo cp icons/JetSpace.png /usr/share/icons/jetspace/
	sudo cp icons/speaker.png /usr/share/icons/jetspace/
	sudo cp desktop/side-settings-explorer.desktop /usr/share/applications/
	sudo cp desktop/side-edit.desktop /usr/share/applications/
	sudo cp desktop/side.desktop /usr/share/xsessions/
	sudo cp desktop/side.desktop /etc/X11/sessions/
	sudo cp desktop/side-view.desktop /usr/share/applications/
	sudo cp desktop/side-files.desktop /usr/share/applications/
	sudo mv side-wallpaper-service /bin/
	sudo mv side-wallpaper-settings /bin/
	sudo mv side-panel /bin/
	sudo mv side-panel-settings /bin/
	sudo mv side-settings-explorer /bin/
	sudo mv side-gtk-settings /bin/
	sudo mv side-session-settings /bin/
	sudo mv side-mime-settings /bin/
	sudo mv side-session /bin/
	sudo mv side-notifyd /bin/
	sudo mv side-search /bin/
	sudo mv side-open /bin/
	sudo mv side-edit /bin/
	sudo mv side-view /bin/
	sudo mv side-files /bin/
	sudo mv clock.so /usr/lib/jetspace/panel/plugins/
	sudo mv volume.so /usr/lib/jetspace/panel/plugins/
	sudo mv user.so /usr/lib/jetspace/panel/plugins/
	sudo mv icon_app_button.so /usr/lib/jetspace/panel/plugins/
	sudo cp translations/de.mo /usr/share/locale/de/LC_MESSAGES/side.mo
	mkdir -p /etc/side/
	sudo cp open/mime.conf /etc/side/

installuserconf:
	mkdir -p ~/.config/side
	cp open/mime.conf ~/.config/side/

clean:
	rm side-panel
	rm side-wallpaper-service
	rm side-panel-settings
	rm side-wallpaper-settings
	rm side-settings-explorer
	rm volume.so
	rm clock.so
	rm side-session
