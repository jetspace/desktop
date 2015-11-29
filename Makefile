LIBPATH=/usr/lib/
BINPATH=/bin/
CONFPATH=/etc/

RIGHTS=sudo

VERSION=$(shell ./version.sh | sed 's/-/./')

build:
	@echo Building Version $(VERSION)
	@echo "building libs"
	mkdir libs -p
	$(MAKE) -C lib/side-app
	$(MAKE) -C lib/side-plugin
	$(MAKE) -C lib/side-widgets


	make install-libs BUILDPREFIX=$(BUILDPREFIX) RIGHTS=$(RIGHTS)

	@echo "building tools"
	mkdir bin -p
	mkdir etc/udev/rules.d -p
	mkdir panel_plugins -p
	mkdir wallpaper_plugins -p
	mkdir settings_plugins -p
	$(MAKE) -C wallpaper-service
	$(MAKE) -C panel
	$(MAKE) -C open
	$(MAKE) -C plugins
	$(MAKE) -C search
	$(MAKE) -C session
	$(MAKE) -C settings
	$(MAKE) -C deamons
	$(MAKE) -C tools/editor
	$(MAKE) -C tools/filemanager
	$(MAKE) -C tools/view
	$(MAKE) -C tools/notifier

clean:
	rm -r bin
	rm -r wallpaper_plugins
	rm -r panel_plugins
	rm -r libs
	rm -r pkg

install-libs:
	$(RIGHTS) mv libs/* $(BUILDPREFIX)$(LIBPATH)
	$(RIGHTS) mkdir -p $(BUILDPREFIX)/usr/include/side/
	$(RIGHTS) cp lib/side-app/apps.h $(BUILDPREFIX)/usr/include/side/
	$(RIGHTS) cp lib/side-plugin/plugin.h $(BUILDPREFIX)/usr/include/side/
	$(RIGHTS) cp lib/side-widgets/widgets.h $(BUILDPREFIX)/usr/include/side/

install:
	$(RIGHTS) mv bin/* $(BUILDPREFIX)$(BINPATH)
	$(RIGHTS) mkdir -p $(BUILDPREFIX)$(CONFPATH)udev/rules.d/
	$(RIGHTS) mv etc/udev/rules.d/* $(BUILDPREFIX)$(CONFPATH)udev/rules.d/
	$(RIGHTS) cp gsettings/* $(BUILDPREFIX)/usr/share/glib-2.0/schemas/
ifndef NOCOMPILE
	sudo glib-compile-schemas /usr/share/glib-2.0/schemas/
endif
	$(RIGHTS) mkdir $(BUILDPREFIX)/usr/lib/jetspace -p
	$(RIGHTS) mkdir $(BUILDPREFIX)/usr/lib/jetspace/panel -p
	$(RIGHTS) mkdir $(BUILDPREFIX)/usr/lib/jetspace/panel/plugins -p
	$(RIGHTS) mkdir $(BUILDPREFIX)/usr/lib/jetspace/wallpaper/plugins -p
	$(RIGHTS) mkdir $(BUILDPREFIX)/usr/lib/jetspace/settings/ -p
	$(RIGHTS) mkdir $(BUILDPREFIX)/usr/share/icons/jetspace -p
	$(RIGHTS) cp icons/* $(BUILDPREFIX)/usr/share/icons/jetspace/
	$(RIGHTS) cp desktop/side-settings-explorer.desktop $(BUILDPREFIX)/usr/share/applications/
	$(RIGHTS) cp desktop/side-edit.desktop $(BUILDPREFIX)/usr/share/applications/
	$(RIGHTS) cp desktop/side.desktop $(BUILDPREFIX)/usr/share/xsessions/
	$(RIGHTS) mkdir $(BUILDPREFIX)/etc/X11/sessions/ -p
	$(RIGHTS) cp desktop/side.desktop $(BUILDPREFIX)/etc/X11/sessions/
	$(RIGHTS) cp desktop/side-view.desktop $(BUILDPREFIX)/usr/share/applications/
	$(RIGHTS) cp desktop/side-files.desktop $(BUILDPREFIX)/usr/share/applications/
	$(RIGHTS) mv wallpaper_plugins/* $(BUILDPREFIX)/usr/lib/jetspace/wallpaper/plugins/
	$(RIGHTS) mv settings_plugins/* $(BUILDPREFIX)/usr/lib/jetspace/settings/
	$(RIGHTS) mv panel_plugins/* $(BUILDPREFIX)/usr/lib/jetspace/panel/plugins/
	$(RIGHTS) cp translations/de.mo $(BUILDPREFIX)/usr/share/locale/de/LC_MESSAGES/side.mo
	$(RIGHTS) mkdir -p $(BUILDPREFIX)/etc/side/
	$(RIGHTS) cp open/mime.conf $(BUILDPREFIX)/etc/side/
	$(RIGHTS) mkdir -p $(BUILDPREFIX)/usr/share/themes/
	$(RIGHTS) cp themes/side* -r $(BUILDPREFIX)/usr/share/themes/

installuserconf:
	mkdir -p ~/.config/side
	cp open/mime.conf ~/.config/side/

package:
	mkdir pkg -p
	mkdir pkg/bin -p
	mkdir pkg/usr/share/glib-2.0/schemas/ -p
	mkdir pkg/usr/share/applications/ -p
	mkdir pkg/etc/side/ -p
	mkdir pkg/usr/lib/jetspace/panel/plugins -p
	mkdir pkg/usr/lib/jetspace/wallpaper/plugins -p
	mkdir pkg/usr/share/icons/jetspace -p
	mkdir pkg/usr/share/applications/ -p
	mkdir pkg/usr/share/locale/de/LC_MESSAGES/ -p
	mkdir pkg/usr/include/side/ -p
	mkdir pkg/usr/share/xsessions/ -p
	mkdir pkg/etc/X11/sessions/ -p
	make build BUILDPREFIX=pkg RIGHTS=
	make install BUILDPREFIX=pkg RIGHTS= NOCOMPILE=true

deb:
	make package
	mkdir pkg/DEBIAN -p
	cat pkgdata/debian/control | sed 's/SIDE_VERSION/$(VERSION)/g' | sed 's/SIDE_ARCH/$(shell dpkg --print-architecture)/g' > pkg/DEBIAN/control
	cp pkgdata/debian/postinst pkg/DEBIAN/postinst
	dpkg -b pkg side_$(VERSION)-0_$(shell dpkg --print-architecture).deb
	rm -r pkg

cleardeb:
	rm side*.deb

arch:

	cat pkgdata/arch/PKGBUILD | sed 's/VERSION/$(VERSION)/g' > PKGBUILD

cleararch:
	rm PKGBUILD
