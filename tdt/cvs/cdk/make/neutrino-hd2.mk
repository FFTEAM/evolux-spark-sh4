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
# neutrino-hd2
#

$(appsdir)/neutrino-hd2/config.status: bootstrap curl libogg libboost libvorbis libvorbisidec libungif freetype libpng libid3tag libflac openssl libmad libgif jpeg sdparm nfs-utils openthreads alsa-lib alsa-lib-dev alsa-utils alsaplayer alsaplayer-dev
	export PATH=$(hostprefix)/bin:$(PATH) && \
	cd $(appsdir)/neutrino-hd2 && \
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
			--with-boxtype=duckbox \
			PKG_CONFIG=$(hostprefix)/bin/pkg-config \
			PKG_CONFIG_PATH=$(targetprefix)/usr/lib/pkgconfig \
			CPPFLAGS="$(CPPFLAGS) -DEVOLUX -DSCREENSHOT -DCPU_FREQ -D__KERNEL_STRICT_NAMES -DNEW_LIBCURL -DPLATFORM_SPARK -I$(driverdir)/frontcontroller/aotom -I$(driverdir)/bpamem -I$(driverdir)"

$(DEPDIR)/neutrino-hd2.do_prepare: $(appsdir)/neutrino-hd2/config.status
	touch $@

$(DEPDIR)/neutrino-hd2.do_compile: $(appsdir)/neutrino-hd2/config.status
	cd $(appsdir)/neutrino-hd2 && $(MAKE)
	touch $@

$(DEPDIR)/neutrino-hd2: curl libogg libboost libvorbis libvorbisidec libungif freetype libpng libid3tag openssl libmad libgif jpeg sdparm nfs-utils openthreads alsa-lib alsa-lib-dev alsa-utils alsaplayer alsaplayer-dev neutrino-hd2.do_prepare neutrino-hd2.do_compile
	$(MAKE) -C $(appsdir)/neutrino-hd2 install DESTDIR=$(targetprefix) DATADIR=/usr/local/share/
	touch $@

neutrino-hd2-clean neutrino-hd2-distclean:
	rm -f $(DEPDIR)/neutrino-hd2
	rm -f $(DEPDIR)/neutrino-hd2.do_compile
	rm -f $(DEPDIR)/neutrino-hd2.do_prepare
	cd $(appsdir)/neutrino-hd2 && \
		$(MAKE) distclean && \
		find $(appsdir)/neutrino-hd2 -name "Makefile.in" -exec rm -rf {} \; && \
		rm -rf $(appsdir)/neutrino-hd2/autom4te.cache && \
		rm -rf $(appsdir)/neutrino-hd2/aclocal.m4 && \
		rm -rf $(appsdir)/neutrino-hd2/Configure && \
		rm -rf $(appsdir)/neutrino-hd2/config.guess && \
		rm -rf $(appsdir)/neutrino-hd2/config.sub && \
		rm -rf $(appsdir)/neutrino-hd2/COPYING && \
		rm -rf $(appsdir)/neutrino-hd2/depcomp && \
		rm -rf $(appsdir)/neutrino-hd2/INSTALL && \
		rm -rf $(appsdir)/neutrino-hd2/install-sh && \
		rm -rf $(appsdir)/neutrino-hd2/ltmain.sh && \
		rm -rf $(appsdir)/neutrino-hd2/missing


