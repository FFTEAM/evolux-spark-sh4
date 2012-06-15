# tuxbox/neutrino

$(targetprefix)/var/etc/.version:
	echo "imagename=Ntrino" > $@
	echo "homepage=http://gitorious.org/open-duckbox-project-sh4" >> $@
	echo "creator=`id -un`" >> $@
	echo "docs=http://gitorious.org/open-duckbox-project-sh4/pages/Home" >> $@
	echo "forum=http://gitorious.org/open-duckbox-project-sh4" >> $@
	echo "version=0100`date +%Y%m%d%H%M`" >> $@
	echo "git =`git describe`" >> $@

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

$(appsdir)/neutrino-hd/config.status: bootstrap graphlcd
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
			--enable-graphlcd \
			--with-boxtype=spark \
			"--with-stb-hal-includes=$(appsdir)/libstb-hal/include" \
			"--with-stb-hal-build=$(appsdir)/libstb-hal" \
			PKG_CONFIG=$(hostprefix)/bin/pkg-config \
			PKG_CONFIG_PATH=$(targetprefix)/usr/lib/pkgconfig \
			CPPFLAGS="$(CPPFLAGS) -DEVOLUX -DSCREENSHOT -DCPU_FREQ -D__KERNEL_STRICT_NAMES -DNEW_LIBCURL -DPLATFORM_SPARK -I$(driverdir)/frontcontroller/aotom -I$(driverdir)/bpamem"

$(DEPDIR)/neutrino-hd.do_prepare: $(appsdir)/neutrino-hd/config.status
	touch $@

$(DEPDIR)/neutrino-hd.do_compile: $(appsdir)/neutrino-hd/config.status
	cd $(appsdir)/neutrino-hd && $(MAKE)
	touch $@

$(DEPDIR)/neutrino-hd: curl libogg libboost libvorbis libungif freetype libpng libid3tag openssl libmad libgif jpeg sdparm nfs-utils libstb-hal openthreads neutrino-hd-plugins neutrino-hd.do_prepare neutrino-hd.do_compile
	$(MAKE) -C $(appsdir)/neutrino-hd install DESTDIR=$(targetprefix) DATADIR=/usr/local/share/
	touch $@

neutrino-hd-clean neutrino-hd-distclean: libstb-hal-clean
	rm -f $(DEPDIR)/neutrino-hd
	rm -f $(DEPDIR)/neutrino-hd.do_compile
	rm -f $(DEPDIR)/neutrino-hd.do_prepare
	cd $(appsdir)/neutrino-hd && \
		$(MAKE) distclean && \
		find $(appsdir)/neutrino-hd -name "Makefile.in" -exec rm -rf {} \; && \
		rm -rf $(appsdir)/neutrino-hd/autom4te.cache


#
# neutrino-hd-plugins
#

$(appsdir)/neutrino-hd-plugins/config.status: bootstrap
	export PATH=$(hostprefix)/bin:$(PATH) && \
	cd $(appsdir)/neutrino-hd-plugins && \
		ACLOCAL_FLAGS="-I $(hostprefix)/share/aclocal" ./autogen.sh && \
		$(BUILDENV) \
		./configure \
			--host=$(target) \
			--with-datadir=/usr/local/share \
			--with-libdir=/usr/lib \
			--with-boxtype=spark \
			PKG_CONFIG=$(hostprefix)/bin/pkg-config \
			PKG_CONFIG_PATH=$(targetprefix)/usr/lib/pkgconfig \
			CPPFLAGS="$(CPPFLAGS) -fno-rtti -fexceptions -rdynamic -ggdb -DEVOLUX -D__KERNEL_STRICT_NAMES -DPLATFORM_SPARK -I$(driverdir)/frontcontroller/aotom"

$(DEPDIR)/neutrino-hd-plugins.do_prepare: $(appsdir)/neutrino-hd-plugins/config.status
	touch $@

$(DEPDIR)/neutrino-hd-plugins.do_compile: $(appsdir)/neutrino-hd-plugins/config.status
	cd $(appsdir)/neutrino-hd-plugins && $(MAKE)
	touch $@

$(DEPDIR)/neutrino-hd-plugins: neutrino-hd-plugins.do_prepare neutrino-hd-plugins.do_compile
	$(MAKE) -C $(appsdir)/neutrino-hd-plugins install DESTDIR=$(targetprefix) DATADIR=$(targetprefix)/usr/local/share/
	touch $@

neutrino-hd-plugins-clean neutrino-hd-plugins-distclean:
	rm -f $(DEPDIR)/neutrino-hd-plugins
	rm -f $(DEPDIR)/neutrino-hd-plugins.do_compile
	rm -f $(DEPDIR)/neutrino-hd-plugins.do_prepare
	cd $(appsdir)/neutrino-hd-plugins && \
		$(MAKE) distclean && \
		find $(appsdir)/neutrino-hd-plugins -name "Makefile.in" -exec rm -rf {} \; && \
		rm -rf $(appsdir)/neutrino-hd-plugins/autom4te.cache


