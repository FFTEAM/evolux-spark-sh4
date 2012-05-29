#
# libstb-hal
#

$(appsdir)/libstb-hal/config.status: bootstrap
	export PATH=$(hostprefix)/bin:$(PATH) && \
	cd $(appsdir)/libstb-hal && \
		ACLOCAL_FLAGS="-I $(hostprefix)/share/aclocal" ./autogen.sh && \
		$(BUILDENV) \
		./configure \
			--host=$(target) \
			--with-datadir=/usr/local/share \
			--with-libdir=/usr/lib \
			--with-boxtype=spark \
			PKG_CONFIG=$(hostprefix)/bin/pkg-config \
			PKG_CONFIG_PATH=$(targetprefix)/usr/lib/pkgconfig \
			CPPFLAGS="$(CPPFLAGS) -DEVOLUX -D__KERNEL_STRICT_NAMES -DPLATFORM_SPARK -I$(driverdir)/frontcontroller/aotom"

$(DEPDIR)/libstb-hal.do_prepare: $(appsdir)/libstb-hal/config.status
	touch $@

$(DEPDIR)/libstb-hal.do_compile: $(appsdir)/libstb-hal/config.status
	cd $(appsdir)/libstb-hal && $(MAKE)
	touch $@

$(DEPDIR)/libstb-hal: libstb-hal.do_prepare libstb-hal.do_compile
	$(MAKE) -C $(appsdir)/libstb-hal install DESTDIR=$(targetprefix) DATADIR=$(targetprefix)/usr/local/share/
	touch $@

libstb-hal-clean libstb-hal-distclean:
	rm -f $(DEPDIR)/libstb-hal
	rm -f $(DEPDIR)/libstb-hal.do_compile
	rm -f $(DEPDIR)/libstb-hal.do_prepare
	cd $(appsdir)/libstb-hal && \
		$(MAKE) distclean && \
		find $(appsdir)/libstb-hal -name "Makefile.in" -exec rm -rf {} \; && \
		rm -rf $(appsdir)/libstb-hal/autom4te.cache

#
# neutrino-hd
#

$(appsdir)/neutrino-hd/config.status: bootstrap
	export PATH=$(hostprefix)/bin:$(PATH) && \
	cd $(appsdir)/neutrino-hd && \
		ACLOCAL_FLAGS="-I $(hostprefix)/share/aclocal" ./autogen.sh && \
		$(BUILDENV) \
		./configure \
			--host=$(target) \
			--with-datadir=/usr/local/share \
			--with-libdir=/usr/lib \
			--with-plugindir=/usr/lib/tuxbox/plugins \
			--with-fontdir=/usr/local/share/fonts \
			--with-configdir=/usr/local/share/config \
			--with-gamesdir=/usr/local/share/games \
			--enable-freesatepg \
			--with-boxtype=spark \
			"--with-stb-hal-includes=$(appsdir)/libstb-hal/include" \
			"--with-stb-hal-build=$(appsdir)/libstb-hal" \
			PKG_CONFIG=$(hostprefix)/bin/pkg-config \
			PKG_CONFIG_PATH=$(targetprefix)/usr/lib/pkgconfig \
			CPPFLAGS="$(CPPFLAGS) -DEVOLUX -D__KERNEL_STRICT_NAMES -DNEW_LIBCURL -DPLATFORM_SPARK -I$(driverdir)/frontcontroller/aotom -I$(driverdir)/bpamem"

$(DEPDIR)/neutrino-hd.do_prepare: $(appsdir)/neutrino-hd/config.status
	touch $@

$(DEPDIR)/neutrino-hd.do_compile: $(appsdir)/neutrino-hd/config.status
	cd $(appsdir)/neutrino-hd && $(MAKE)
	touch $@

$(DEPDIR)/neutrino-hd: curl libogg libboost libvorbis libungif freetype libpng libid3tag openssl libmad libgif jpeg sdparm nfs-utils libstb-hal openthreads neutrino-hd.do_prepare neutrino-hd.do_compile
	$(MAKE) -C $(appsdir)/neutrino-hd install DESTDIR=$(targetprefix) DATADIR=$(targetprefix)/usr/local/share/
	touch $@

neutrino-hd-clean neutrino-hd-distclean:
	rm -f $(DEPDIR)/neutrino-hd
	rm -f $(DEPDIR)/neutrino-hd.do_compile
	rm -f $(DEPDIR)/neutrino-hd.do_prepare
	cd $(appsdir)/neutrino-hd && \
		$(MAKE) distclean && \
		find $(appsdir)/neutrino-hd -name "Makefile.in" -exec rm -rf {} \; && \
		rm -rf $(appsdir)/neutrino-hd/autom4te.cache



