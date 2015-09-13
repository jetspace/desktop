LIBPATH="/usr/lib/"
BINPATH="/bin/"

build:
	@echo "building libs"
	mkdir libs -p
	$(MAKE) -C lib/side-config
	$(MAKE) -C lib/side-app
	$(MAKE) -C lib/side-log
	$(MAKE) -C lib/side-plugin


	make install-libs

	@echo "building tools"
	mkdir bin -p
	mkdir panel_plugins -p
	mkdir wallpaper_plugins -p
	$(MAKE) -C panel
	$(MAKE) -C wallpaper-service
	$(MAKE) -C open
	$(MAKE) -C plugins
	$(MAKE) -C search
	$(MAKE) -C session
	$(MAKE) -C settings
	$(MAKE) -C tools/editor
	$(MAKE) -C tools/filemanager
	$(MAKE) -C tools/view

clean:
	rm -r bin
	rm -r wallpaper_plugins
	rm -r panel_plugins
	rm -r libs

install-libs:
	sudo mv libs/* $(LIBPATH)
	sudo mkdir -p /usr/include/side/
	sudo cp lib/side-app/apps.h /usr/include/side/
	sudo cp lib/side-config/config.h /usr/include/side/
	sudo cp lib/side-log/log.h /usr/include/side/
	sudo cp lib/side-plugin/plugin.h /usr/include/side/

install:
	sudo mv bin/* $(BINPATH)
	sudo cp gsettings/* /usr/share/glib-2.0/schemas/
	sudo glib-compile-schemas /usr/share/glib-2.0/schemas/
	sudo mkdir /usr/lib/jetspace -p
	sudo mkdir /usr/lib/jetspace/panel -p
	sudo mkdir /usr/lib/jetspace/panel/plugins -p
	sudo mkdir /usr/lib/jetspace/wallpaper/plugins -p
	sudo mkdir /usr/share/icons/jetspace -p
	sudo cp icons/* /usr/share/icons/jetspace/
	sudo cp desktop/side-settings-explorer.desktop /usr/share/applications/
	sudo cp desktop/side-edit.desktop /usr/share/applications/
	sudo cp desktop/side.desktop /usr/share/xsessions/
	sudo cp desktop/side.desktop /etc/X11/sessions/
	sudo cp desktop/side-view.desktop /usr/share/applications/
	sudo cp desktop/side-files.desktop /usr/share/applications/
	sudo mv wallpaper_plugins/* /usr/lib/jetspace/wallpaper/plugins/
	sudo mv panel_plugins/* /usr/lib/jetspace/panel/plugins/
	sudo cp translations/de.mo /usr/share/locale/de/LC_MESSAGES/side.mo
	mkdir -p /etc/side/
	sudo cp open/mime.conf /etc/side/

installuserconf:
	mkdir -p ~/.config/side
	cp open/mime.conf ~/.config/side/
