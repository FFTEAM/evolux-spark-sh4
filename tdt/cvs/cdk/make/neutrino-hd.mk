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
# openthreads
#
$(appsdir)/openthreads/config.status: bootstrap
	if [ ! -d $(appsdir)/openthreads ]; then \
		git clone --recursive git://c00lstreamtech.de/cst-public-libraries-openthreads.git $(appsdir)/openthreads; \
		cd $(appsdir)/openthreads && patch -p1 < "$(buildprefix)/Patches/libopenthreads.patch"; \
	fi
	export PATH=$(hostprefix)/bin:$(PATH) && \
	cd $(appsdir)/openthreads && \
	rm CMakeFiles/* -rf CMakeCache.txt cmake_install.cmake && \
	cmake . -DCMAKE_BUILD_TYPE=Release -DCMAKE_SYSTEM_NAME="Linux" \
		-DCMAKE_INSTALL_PREFIX="" \
		-DCMAKE_C_COMPILER="$(target)-gcc" \
		-DCMAKE_CXX_COMPILER="$(target)-g++" \
		-D_OPENTHREADS_ATOMIC_USE_GCC_BUILTINS_EXITCODE=1 && \
		find . -name cmake_install.cmake -print0 | xargs -0 \
		sed -i 's@SET(CMAKE_INSTALL_PREFIX "/usr/local")@SET(CMAKE_INSTALL_PREFIX "")@'

$(DEPDIR)/openthreads.do_prepare:
	touch $@

$(DEPDIR)/openthreads.do_compile: $(appsdir)/openthreads/config.status
	cd $(appsdir)/openthreads && $(MAKE)
	touch $@

$(DEPDIR)/openthreads: openthreads.do_prepare openthreads.do_compile 
	$(MAKE) -C $(appsdir)/openthreads install DESTDIR=$(targetprefix)/usr
	touch $@
#
# libstb-hal
#

$(appsdir)/libstb-hal/config.status: bootstrap
	if [ ! -d $(appsdir)/libstb-hal ]; then \
		git clone git://gitorious.org/~martii/neutrino-hd/martiis-libstb-hal.git $(appsdir)/libstb-hal; \
		cd $(appsdir)/libstb-hal; \
		patch -p1 < $(buildprefix)/Patches/neutrino-hd-martii-libstbhal.diff; \
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
	git clone git://gitorious.org/~martii/neutrino-hd/martiis-libstb-hal.git $(appsdir)/libstb-hal
#
# neutrino-hd
#
 
$(appsdir)/neutrino-hd/config.status: bootstrap curl libogg libboost libvorbis libvorbisidec libungif freetype libpng libid3tag openssl libmad libgif jpeg sdparm nfs-utils libstb-hal libusb2 openthreads alsa-lib alsa-lib-dev alsa-utils alsaplayer alsaplayer-dev neutrino-hd-plugins graphlcd
	if [ ! -d $(appsdir)/neutrino-hd ]; then \
		git clone git://gitorious.org/~martii/neutrino-hd/martiis-neutrino-hd-tripledragon.git $(appsdir)/neutrino-hd; \
		cd $(appsdir)/neutrino-hd; \
		patch -p1 < $(buildprefix)/Patches/neutrino-hd-martii.diff; \
		rm -rf $(appsdir)/neutrino-hd/lib/libtuxtxt; \
		cp -RP $(appsdir)/neutrino-hd-old/lib/libtuxtxt $(appsdir)/neutrino-hd/lib/; \
	fi
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
			CPPFLAGS="$(CPPFLAGS) -DEVOLUX -DMARTII -DCPU_FREQ -D__KERNEL_STRICT_NAMES -DNEW_LIBCURL -DPLATFORM_SPARK -I$(driverdir)/frontcontroller/aotom -I$(driverdir)/bpamem $(TRIPLEX_ON)"

$(DEPDIR)/neutrino-hd.do_prepare:
	touch $@

$(DEPDIR)/neutrino-hd.do_compile: $(appsdir)/neutrino-hd/config.status
	echo -e "\n/* correct FB_DEVICE for Spark/Spark7162  */\n#define FB_DEVICE \"/dev/fb0\"" >> $(appsdir)/neutrino-hd/config.h; \
	cd $(appsdir)/neutrino-hd && $(MAKE)
	touch $@

$(DEPDIR)/neutrino-hd: neutrino-hd.do_prepare neutrino-hd.do_compile
	$(MAKE) -C $(appsdir)/neutrino-hd install DESTDIR=$(targetprefix) DATADIR=/usr/local/share/
	touch $@

neutrino-hd-clean neutrino-hd-distclean: libstb-hal-clean neutrino-hd-plugins-clean
	rm -f $(DEPDIR)/neutrino-hd
	rm -f $(DEPDIR)/neutrino-hd.do_compile
	rm -f $(DEPDIR)/neutrino-hd.do_prepare
	cd $(appsdir)/neutrino-hd && \
		$(MAKE) distclean && \
		find $(appsdir)/neutrino-hd -name "Makefile.in" -exec rm -rf {} \; && \
		rm -rf $(appsdir)/neutrino-hd/autom4te.cache

neutrino-hd-update:
	rm -f $(DEPDIR)/*neutrino*
	cd $(appsdir)/neutrino-hd && \
		$(MAKE) clean && \
		find $(appsdir)/neutrino-hd -name "Makefile.in" -exec rm -rf {} \; && \
		rm -rf $(appsdir)/neutrino-hd/autom4te.cache && \
		rm -rf $(appsdir)/neutrino-hd/aclocal.m4 && \
		rm -rf $(appsdir)/neutrino-hd/Configure && \
		rm -rf $(appsdir)/neutrino-hd/config.guess && \
		rm -rf $(appsdir)/neutrino-hd/config.sub && \
		rm -rf $(appsdir)/neutrino-hd/COPYING && \
		rm -rf $(appsdir)/neutrino-hd/depcomp && \
		rm -rf $(appsdir)/neutrino-hd/INSTALL && \
		rm -rf $(appsdir)/neutrino-hd/install-sh && \
		rm -rf $(appsdir)/neutrino-hd/ltmain.sh && \
		rm -rf $(appsdir)/neutrino-hd/missing && \
		git pull

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
			CPPFLAGS="$(CPPFLAGS) -fno-rtti -fexceptions -rdynamic -ggdb -DEVOLUX -DMARTII -D__KERNEL_STRICT_NAMES -DPLATFORM_SPARK -I$(driverdir)/frontcontroller/aotom"

$(DEPDIR)/neutrino-hd-plugins.do_prepare:
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


