# tuxbox/neutrino
if ENABLE_SPARK7162
TRIPLEX_ON="-DPLATFORM_SPARK7162"
else
TRIPLEX_ON=
endif
$(targetprefix)/var/etc/.version:
	echo "imagename=Ntrino-HD" > $@
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
	if [ ! -d $(appsdir)/libstb-hal ]; then \
		git clone git://gitorious.org/~martii/neutrino-mp/martiis-libstb-hal.git $(appsdir)/libstb-hal; \
	fi
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
			CPPFLAGS="$(CPPFLAGS) -DEVOLUX -DMARTII -D__KERNEL_STRICT_NAMES -DPLATFORM_SPARK -I$(driverdir)/frontcontroller/aotom $(TRIPLEX_ON)"

$(DEPDIR)/libstb-hal.do_prepare:
	touch $@

$(DEPDIR)/libstb-hal.do_compile: $(appsdir)/libstb-hal/config.status
	cd $(appsdir)/libstb-hal && $(MAKE)
	touch $@

$(DEPDIR)/libstb-hal: libstb-hal.do_prepare libstb-hal.do_compile
	$(MAKE) -C $(appsdir)/libstb-hal install DESTDIR=$(targetprefix) DATADIR=$(targetprefix)/usr/local/share/
	touch $@

libstb-hal-clean libstb-hal-distclean:
	rm -f $(DEPDIR)/libstb-hal*
	cd $(appsdir)/libstb-hal && \
		$(MAKE) distclean && \
		find $(appsdir)/libstb-hal -name "Makefile.in" -exec rm -rf {} \; && \
		rm -rf $(appsdir)/libstb-hal/autom4te.cache

libstb-hal-update:
	rm -f $(DEPDIR)/libstb-hal*
	rm -rf $(appsdir)/libstb-hal && \
	git clone git://gitorious.org/~martii/neutrino-mp/martiis-libstb-hal.git $(appsdir)/libstb-hal
#
# neutrino-mp
#
 
$(appsdir)/neutrino-mp/config.status: bootstrap curl libogg libboost libdvbsipp libvorbis libvorbisidec libungif freetype libpng libid3tag openssl libmad libgif jpeg sdparm nfs-utils libstb-hal libusb2 libopenthreads alsa-lib alsa-lib-dev alsa-utils alsaplayer alsaplayer-dev neutrino-mp-plugins graphlcd
	if [ ! -d $(appsdir)/neutrino-mp ]; then \
		git clone git://gitorious.org/neutrino-mp/neutrino-mp.git $(appsdir)/neutrino-mp; \
		cd $(appsdir)/neutrino-mp; \
		patch -p1 < $(buildprefix)/Patches/neutrino-mp.diff; \
	fi
	export PATH=$(hostprefix)/bin:$(PATH) && \
	cd $(appsdir)/neutrino-mp && \
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
			--with-boxtype=spark \
			--with-stb-hal-includes=$(appsdir)/libstb-hal/include \
			--with-stb-hal-build=$(appsdir)/libstb-hal \
			PKG_CONFIG=$(hostprefix)/bin/pkg-config \
			PKG_CONFIG_PATH=$(targetprefix)/usr/lib/pkgconfig \
			CPPFLAGS="$(CPPFLAGS) -DHAVE_SPARK_HARDWARE -DUSE_FBPAN -DEVOLUX -DMARTII -DCPU_FREQ -D__KERNEL_STRICT_NAMES -DNEW_LIBCURL -DPLATFORM_SPARK -I$(driverdir)/frontcontroller/aotom -I$(driverdir)/bpamem $(TRIPLEX_ON)"

$(DEPDIR)/neutrino-mp.do_prepare:
	touch $@

$(DEPDIR)/neutrino-mp.do_compile: $(appsdir)/neutrino-mp/config.status
	echo -e "\n/* correct FB_DEVICE for Spark/Spark7162  */\n#define FB_DEVICE \"/dev/fb0\"" >> $(appsdir)/neutrino-mp/config.h; \
	cd $(appsdir)/neutrino-mp && $(MAKE)
	touch $@

$(DEPDIR)/neutrino-mp: neutrino-mp.do_prepare neutrino-mp.do_compile
	$(MAKE) -C $(appsdir)/neutrino-mp install DESTDIR=$(targetprefix) DATADIR=/usr/local/share/
	$(target)-strip $(targetprefix)/usr/local/bin/neutrino
	$(target)-strip $(targetprefix)/usr/local/bin/pzapit
	$(target)-strip $(targetprefix)/usr/local/bin/sectionsdcontrol
	$(target)-strip $(targetprefix)/usr/local/sbin/udpstreampes
	touch $@

neutrino-mp-clean neutrino-mp-distclean: libstb-hal-clean neutrino-mp-plugins-clean
	rm -f $(DEPDIR)/neutrino-mp
	rm -f $(DEPDIR)/neutrino-mp.do_compile
	rm -f $(DEPDIR)/neutrino-mp.do_prepare
	cd $(appsdir)/neutrino-mp && \
		$(MAKE) distclean && \
		find $(appsdir)/neutrino-mp -name "Makefile.in" -exec rm -rf {} \; && \
		rm -rf $(appsdir)/neutrino-mp/autom4te.cache

neutrino-mp-update:
	rm -f $(DEPDIR)/*neutrino*
	cd $(appsdir)/neutrino-mp && \
		$(MAKE) clean && \
		find $(appsdir)/neutrino-mp -name "Makefile.in" -exec rm -rf {} \; && \
		rm -rf $(appsdir)/neutrino-mp/autom4te.cache && \
		rm -rf $(appsdir)/neutrino-mp/aclocal.m4 && \
		rm -rf $(appsdir)/neutrino-mp/Configure && \
		rm -rf $(appsdir)/neutrino-mp/config.guess && \
		rm -rf $(appsdir)/neutrino-mp/config.sub && \
		rm -rf $(appsdir)/neutrino-mp/COPYING && \
		rm -rf $(appsdir)/neutrino-mp/depcomp && \
		rm -rf $(appsdir)/neutrino-mp/INSTALL && \
		rm -rf $(appsdir)/neutrino-mp/install-sh && \
		rm -rf $(appsdir)/neutrino-mp/ltmain.sh && \
		rm -rf $(appsdir)/neutrino-mp/missing && \
		git pull

#
# neutrino-mp-plugins
#

$(appsdir)/neutrino-mp-plugins/config.status: bootstrap
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
			CPPFLAGS="$(CPPFLAGS) -fno-rtti -fexceptions -rdynamic -ggdb -DEVOLUX -DMARTII -D__KERNEL_STRICT_NAMES -DPLATFORM_SPARK -I$(driverdir)/frontcontroller/aotom"

$(DEPDIR)/neutrino-mp-plugins.do_prepare:
	touch $@

$(DEPDIR)/neutrino-mp-plugins.do_compile: $(appsdir)/neutrino-mp-plugins/config.status
	cd $(appsdir)/neutrino-hd-plugins && $(MAKE)
	touch $@

$(DEPDIR)/neutrino-mp-plugins: neutrino-mp-plugins.do_prepare neutrino-mp-plugins.do_compile
	$(MAKE) -C $(appsdir)/neutrino-hd-plugins install DESTDIR=$(targetprefix) DATADIR=$(targetprefix)/usr/local/share/
	touch $@

neutrino-mp-plugins-clean neutrino-mp-plugins-distclean:
	rm -f $(DEPDIR)/neutrino-mp-plugins
	rm -f $(DEPDIR)/neutrino-mp-plugins.do_compile
	rm -f $(DEPDIR)/neutrino-mp-plugins.do_prepare
	cd $(appsdir)/neutrino-hd-plugins && \
		$(MAKE) distclean && \
		find $(appsdir)/neutrino-mp-plugins -name "Makefile.in" -exec rm -rf {} \; && \
		rm -rf $(appsdir)/neutrino-mp-plugins/autom4te.cache


