# tuxbox/enigma2

$(DEPDIR)/enigma2-pli-nightly.do_prepare:
	touch $@

$(appsdir)/enigma2-pli-nightly/config.status: bootstrap freetype expat fontconfig libpng jpeg libgif libfribidi libid3tag libmad libsigc libreadline \
		libdvbsipp python libxml2 libxslt elementtree zope-interface twisted pyopenssl lxml libxmlccwrap ncurses-dev libdreamdvd2 tuxtxt32bpp sdparm gstreamer gst_plugins_dvbmediasink $(EXTERNALLCD_DEP)
	cd $(appsdir)/enigma2-pli-nightly && \
		./autogen.sh && \
		sed -e 's|#!/usr/bin/python|#!$(crossprefix)/bin/python|' -i po/xml2po.py && \
		./configure \
			--host=$(target) \
			--without-libsdl \
			--with-datadir=/usr/local/share \
			--with-libdir=/usr/lib \
			--with-plugindir=/usr/lib/tuxbox/plugins \
			--prefix=/usr \
			--datadir=/usr/local/share \
			--sysconfdir=/etc \
			STAGING_INCDIR=$(hostprefix)/usr/include \
			STAGING_LIBDIR=$(hostprefix)/usr/lib \
			PKG_CONFIG=$(hostprefix)/bin/pkg-config \
			PKG_CONFIG_PATH=$(targetprefix)/usr/lib/pkgconfig \
			PY_PATH=$(targetprefix)/usr \
			CPPFLAGS="$(CPPFLAGS) -DPLATFORM_SPARK -I$(driverdir)/include -I $(buildprefix)/$(KERNEL_DIR)/include"


$(DEPDIR)/enigma2-pli-nightly.do_compile: $(appsdir)/enigma2-pli-nightly/config.status
	cd $(appsdir)/enigma2-pli-nightly && \
		$(MAKE) all
	touch $@

$(DEPDIR)/enigma2-pli-nightly: enigma2-pli-nightly.do_prepare enigma2-pli-nightly.do_compile
	$(MAKE) -C $(appsdir)/enigma2-pli-nightly install DESTDIR=$(targetprefix)
	if [ -e $(targetprefix)/usr/bin/enigma2 ]; then \
		$(target)-strip $(targetprefix)/usr/bin/enigma2; \
	fi
	if [ -e $(targetprefix)/usr/local/bin/enigma2 ]; then \
		$(target)-strip $(targetprefix)/usr/local/bin/enigma2; \
	fi
	touch $@

enigma2-pli-nightly-clean enigma2-pli-nightly-distclean:
	rm -f $(DEPDIR)/enigma2-pli-nightly*
	cd $(appsdir)/enigma2-pli-nightly && \
		$(MAKE) distclean && \
		find $(appsdir)/enigma2-pli-nightly -name "Makefile.in" -exec rm -rf {} \; && \
		rm -rf $(appsdir)/enigma2-pli-nightly/autom4te.cache

$(DEPDIR)/enigma2-pli-nightly-misc:
#	workarounds to allow rebuild
	find $(targetprefix)/usr/local/share/enigma2/ -name .svn |xargs rm -fr
	rm -f $(targetprefix)/usr/local/etc

	$(INSTALL_DIR) $(targetprefix)/usr/bin && \
	$(INSTALL_DIR) $(targetprefix)/usr/share && \
	$(LN_SF) /usr/local/share/enigma2 $(targetprefix)/usr/share/enigma2 && \
	$(INSTALL_DIR) $(targetprefix)/usr/share/etc && \
	$(LN_SF) /usr/local/share/enigma2 $(targetprefix)/usr/share/etc/enigma2 && \
	cp -rd root/usr/local/share/enigma2/* $(targetprefix)/usr/local/share/enigma2/ && \
	$(INSTALL_DIR) $(targetprefix)/usr/share/fonts && \
	cp -rd root/usr/share/fonts/* $(targetprefix)/usr/share/fonts/ && \
	$(INSTALL_DIR) $(targetprefix)/etc && \
	$(INSTALL_DIR) $(targetprefix)/etc/enigma2 && \
	$(LN_SF) /etc $(targetprefix)/usr/local/etc && \
	cp -rd root/etc/enigma2/* $(targetprefix)/etc/enigma2/ && \
	$(INSTALL_FILE) root/etc/videomode $(targetprefix)/etc/ && \
	$(INSTALL_FILE) root/etc/lircd.conf $(targetprefix)/etc/ && \
	$(INSTALL_FILE) root/etc/inetd.conf $(targetprefix)/etc/ && \
	$(INSTALL_FILE) root/etc/image-version $(targetprefix)/etc/ && \
	$(INSTALL_DIR) $(targetprefix)/etc/tuxbox && \
	$(INSTALL_FILE) root/etc/tuxbox/satellites.xml $(targetprefix)/etc/tuxbox/ && \
	$(INSTALL_FILE) root/etc/tuxbox/tuxtxt2.conf $(targetprefix)/etc/tuxbox/ && \
	$(INSTALL_FILE) root/etc/tuxbox/cables.xml $(targetprefix)/etc/tuxbox/ && \
	$(INSTALL_FILE) root/etc/tuxbox/terrestrial.xml $(targetprefix)/etc/tuxbox/ && \
	$(INSTALL_FILE) root/etc/tuxbox/timezone.xml $(targetprefix)/etc/tuxbox/ && \
	$(INSTALL_DIR) $(targetprefix)/boot && \
	$(INSTALL_DIR) $(targetprefix)/media/{hdd,dvd} && \
	$(INSTALL_DIR) $(targetprefix)/hdd/{music,picture,movie} && \
	$(INSTALL_DIR) $(targetprefix)/usr/tuxtxt && \
	chmod 755 $(targetprefix)/usr/tuxtxt && \
	$(INSTALL_FILE) root/usr/tuxtxt/tuxtxt2.conf $(targetprefix)/usr/tuxtxt/
	rm -f $(targetprefix)/usr/local/share/enigma2/keymap_tf7700.xml
	touch $@

