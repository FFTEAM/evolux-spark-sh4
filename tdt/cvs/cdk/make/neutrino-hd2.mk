# tuxbox/neutrino

$(targetprefix)/var/etc/.version:
	echo "imagename=Ntrino-HD2" > $@
	echo "homepage=http://gitorious.org/open-duckbox-project-sh4" >> $@
	echo "creator=`id -un`" >> $@
	echo "docs=http://gitorious.org/open-duckbox-project-sh4/pages/Home" >> $@
	echo "forum=http://gitorious.org/open-duckbox-project-sh4" >> $@
	echo "version=0100`date +%Y%m%d%H%M`" >> $@
	echo "git =`git describe`" >> $@

#
# neutrino-hd2
#
 
$(appsdir)/neutrino-hd2/config.status: bootstrap curl libogg libboost libvorbis libvorbisidec libungif freetype libpng libid3tag libflac openssl libmad libgif jpeg sdparm nfs-utils openthreads alsa-lib alsa-lib-dev alsa-utils alsaplayer alsaplayer-dev libusb2 graphlcd libdvbsipp
	if [ ! -d $(appsdir)/neutrino-hd2-exp ]; then \
		svn co http://neutrinohd2.googlecode.com/svn/branches/nhd2-exp $(appsdir)/neutrino-hd2-exp; \
		cd $(appsdir)/neutrino-hd2-exp; \
		patch -p1 < "$(buildprefix)/Patches/neutrino.hd2-exp.diff"; \
		fgrep -r SPARK_7162 * | grep -v Binärdatei | cut -d : -f1 > find.txt && wrongSetup=`cat ./find.txt`; \
		for i in "$wrongSetup"; do \
		sed "s@SPARK_7162@SPARK7162@g" -i "$i"; \
		done; \
	fi
if ENABLE_SPARK
	export PATH=$(hostprefix)/bin:$(PATH) && \
	cd $(appsdir)/neutrino-hd2-exp && \
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
			--enable-libeplayer3 \
			--enable-graphlcd \
			--enable-libass \
			PKG_CONFIG=$(hostprefix)/bin/pkg-config \
			PKG_CONFIG_PATH=$(targetprefix)/usr/lib/pkgconfig \
			CPPFLAGS="$(CPPFLAGS) -DEVOLUX -DCPU_FREQ -D__KERNEL_STRICT_NAMES -DNEW_LIBCURL -DPLATFORM_SPARK -I$(driverdir)/frontcontroller/aotom -I$(driverdir)/bpamem -I$(driverdir)"
else
	export PATH=$(hostprefix)/bin:$(PATH) && \
	cd $(appsdir)/neutrino-hd2-exp && \
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
			--with-boxtype=spark7162 \
			--enable-libeplayer3 \
			--enable-graphlcd \
			--enable-libass \
			PKG_CONFIG=$(hostprefix)/bin/pkg-config \
			PKG_CONFIG_PATH=$(targetprefix)/usr/lib/pkgconfig \
			CPPFLAGS="$(CPPFLAGS) -DEVOLUX -DFB_BLIT -DCPU_FREQ -D__KERNEL_STRICT_NAMES -DNEW_LIBCURL -DPLATFORM_SPARK7162 -I$(driverdir)/frontcontroller/aotom -I$(driverdir)/bpamem -I$(driverdir)"
endif

$(DEPDIR)/neutrino-hd2.do_prepare:
	touch $@

$(DEPDIR)/neutrino-hd2.do_compile: $(appsdir)/neutrino-hd2/config.status
	cd $(appsdir)/neutrino-hd2-exp && $(MAKE)
	touch $@

$(DEPDIR)/neutrino-hd2: neutrino-hd2.do_prepare neutrino-hd2.do_compile
	$(MAKE) -C $(appsdir)/neutrino-hd2-exp install DESTDIR=$(targetprefix) DATADIR=/usr/local/share/
	touch $@

neutrino-hd2-clean:
	rm -f $(DEPDIR)/*neutrino*
	cd $(appsdir)/neutrino-hd2-exp && \
		$(MAKE) clean && \
		find $(appsdir)/neutrino-hd2-exp -name "Makefile.in" -exec rm -rf {} \; && \
		rm -rf $(appsdir)/neutrino-hd2-exp/autom4te.cache && \
		rm -rf $(appsdir)/neutrino-hd2-exp/aclocal.m4 && \
		rm -rf $(appsdir)/neutrino-hd2-exp/Configure && \
		rm -rf $(appsdir)/neutrino-hd2-exp/config.guess && \
		rm -rf $(appsdir)/neutrino-hd2-exp/config.sub && \
		rm -rf $(appsdir)/neutrino-hd2-exp/COPYING && \
		rm -rf $(appsdir)/neutrino-hd2-exp/depcomp && \
		rm -rf $(appsdir)/neutrino-hd2-exp/INSTALL && \
		rm -rf $(appsdir)/neutrino-hd2-exp/install-sh && \
		rm -rf $(appsdir)/neutrino-hd2-exp/ltmain.sh && \
		rm -rf $(appsdir)/neutrino-hd2-exp/missing

neutrino-hd2-distclean:
	rm -f $(DEPDIR)/*neutrino*
	rm -rf $(appsdir)/neutrino-hd2-exp

neutrino-hd2-update:
	rm -f $(DEPDIR)/*neutrino*
	cd $(appsdir)/neutrino-hd2-exp && \
		$(MAKE) clean && \
		find $(appsdir)/neutrino-hd2-exp -name "Makefile.in" -exec rm -rf {} \; && \
		rm -rf $(appsdir)/neutrino-hd2-exp/autom4te.cache && \
		rm -rf $(appsdir)/neutrino-hd2-exp/aclocal.m4 && \
		rm -rf $(appsdir)/neutrino-hd2-exp/Configure && \
		rm -rf $(appsdir)/neutrino-hd2-exp/config.guess && \
		rm -rf $(appsdir)/neutrino-hd2-exp/config.sub && \
		rm -rf $(appsdir)/neutrino-hd2-exp/COPYING && \
		rm -rf $(appsdir)/neutrino-hd2-exp/depcomp && \
		rm -rf $(appsdir)/neutrino-hd2-exp/INSTALL && \
		rm -rf $(appsdir)/neutrino-hd2-exp/install-sh && \
		rm -rf $(appsdir)/neutrino-hd2-exp/ltmain.sh && \
		rm -rf $(appsdir)/neutrino-hd2-exp/missing && \
		svn up
