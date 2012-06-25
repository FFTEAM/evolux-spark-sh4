# vdr
VERSION_VDR := 1.7.28-1


$(DEPDIR)/vdrdev2.do_compile: bootstrap freetype libxml2 jpeg libz libpng fontconfig libcap tiff expat bzip2 imagemagick
	cd $(appsdir)/vdr/vdr-1.7.28 && \
		$(BUILDENV) $(MAKE) all plugins install-bin install-conf install-plugins install-i18n \
		DESTDIR=$(targetprefix) \
		VIDEODIR=/hdd/movie \
		CONFDIR=/usr/local/share/vdr \
		PLUGINLIBDIR=/usr/lib/vdr
	touch $@

$(DEPDIR)/vdrdev2: vdrdev2.do_compile
	for i in anthra_1280_FS anthra_1920_OSo EgalT2 HD-Ready-anthras NarrowHD PearlHD; do \
	    echo "Plugin $$i:"; \
	    cd $(appsdir)/vdr/vdr-1.7.28/PLUGINS/src/$$i && $(MAKE) install DESTDIR=$(targetprefix); \
	    done;
	cp -RP $(appsdir)/vdr/vdr-1.7.28/PLUGINS/src/setup/examples/minivdr $(targetprefix)/etc/vdr/plugins/
	mv $(targetprefix)/etc/vdr/plugins/minivdr $(targetprefix)/etc/vdr/plugins/setup
	touch $@

vdrdev2-clean:
	-rm .deps/vdrdev2
	-rm .deps/vdrdev2.do_compile

vdrdev2-distclean:
	$(MAKE) -C $(appsdir)/vdr/vdr-1.7.28 clean clean-plugins
	-rm .deps/vdrdev2*
