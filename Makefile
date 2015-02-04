run:
	dbus-launch	./side-panel &
	dbus-launch	./side-wallpaper-service &
install:
	sudo cp gsettings/org.jetspace.desktop.panel.gschema.xml /usr/share/glib-2.0/schemas/
	sudo glib-compile-schemas /usr/share/glib-2.0/schemas/
	sudo mkdir /usr/lib/jetspace
	sudo mkdir /usr/lib/jetspace/panel
	sudo mkdir /usr/lib/jetspace/panel/plugins
	@echo "Warning: Only installed GSetting Schemas, no binarys"
clean:
	rm side-panel
	rm side-wallpaper-service
	rm side-panel-settings
	rm side-wallpaper-settings
