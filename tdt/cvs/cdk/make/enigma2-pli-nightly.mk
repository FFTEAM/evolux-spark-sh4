# tuxbox/enigma2

$(DEPDIR)/enigma2-pli-nightly.do_prepare:
	touch $@

$(appsdir)/enigma2-pli-nightly/config.status: bootstrap freetype expat fontconfig libpng jpeg libgif libfribidi libid3tag libmad libsigc libreadline \
		libdvbsipp python libxml2 libxslt elementtree zope_interface twisted pyopenssl lxml libxmlccwrap ncurses-dev libdreamdvd2 tuxtxt32bpp sdparm  hotplug_e2 $(MEDIAFW_DEP) $(EXTERNALLCD_DEP)
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

