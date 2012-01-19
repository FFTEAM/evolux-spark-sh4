# tuxbox/neutrino

$(targetprefix)/var/etc/.version:
	echo "imagename=Neutrino-HD" > $@
	echo "homepage=http://gitorious.org/open-duckbox-project-sh4" >> $@
	echo "creator=`id -un`" >> $@
	echo "docs=http://gitorious.org/open-duckbox-project-sh4/pages/Home" >> $@
	echo "forum=http://gitorious.org/open-duckbox-project-sh4" >> $@
	echo "version=0100`date +%Y%m%d%H%M`" >> $@
	echo "git =`git describe`" >> $@

#
#NIGHTLY
#

$(appsdir)/neutrino-nightly/config.status: bootstrap freetype jpeg libpng libgif libid3tag curl libmad libvorbisidec libboost openssl
	export PATH=$(hostprefix)/bin:$(PATH) && \
	cd $(appsdir)/neutrino-nightly && \
		ACLOCAL_FLAGS="-I $(hostprefix)/share/aclocal" ./autogen.sh && \
		$(BUILDENV) \
		./configure \
			--host=$(target) \
			--with-tremor \
			--with-libdir=/usr/lib \
			--with-datadir=/share/tuxbox \
			--with-fontdir=/share/fonts \
			--with-configdir=/var/tuxbox/config \
			--with-gamesdir=/var/tuxbox/games \
			--with-plugindir=/usr/lib/tuxbox/plugins \
			PKG_CONFIG=$(hostprefix)/bin/pkg-config \
			PKG_CONFIG_PATH=$(targetprefix)/usr/lib/pkgconfig \
			$(if $(PLAYER131),PLAYER131=$(PLAYER131)) \
			$(if $(PLAYER179),PLAYER179=$(PLAYER179)) \
			$(if $(PLAYER191),PLAYER191=$(PLAYER191)) \
			CPPFLAGS="$(CPPFLAGS) -D__KERNEL_STRICT_NAMES -DPLATFORM_SPARK -I$(driverdir)/include -I $(buildprefix)/$(KERNEL_DIR)/include"

#svn co http://www.coolstreamtech.de/coolstream_public_svn/THIRDPARTY/applications/neutrino-experimental/ --username coolstream --password coolstream $(appsdir)/neutrino-nightly
$(DEPDIR)/neutrino-nightly.do_prepare: Patches/neutrino-nightly.0.diff
	git clone git://novatux.git.sourceforge.net/gitroot/novatux/neutrino-experimental $(appsdir)/neutrino-nightly
	rm -rf $(appsdir)/neutrino-nightly/lib/libcoolstream
	rm -rf $(appsdir)/neutrino-nightly/src/zapit/include/linux
	cp -ra $(appsdir)/neutrino-nightly $(appsdir)/neutrino-nightly.org
	cd $(appsdir)/neutrino-nightly && patch -p1 <../../cdk/$(word 1,$^)
	touch $@

$(DEPDIR)/neutrino-nightly.do_compile: $(appsdir)/neutrino-nightly/config.status
	cd $(appsdir)/neutrino-nightly && \
		$(MAKE) all
	touch $@

$(DEPDIR)/neutrino-nightly: neutrino-nightly.do_prepare neutrino-nightly.do_compile
	$(MAKE) -C $(appsdir)/neutrino-nightly install DESTDIR=$(targetprefix)
	make $(targetprefix)/var/etc/.version
	$(target)-strip $(targetprefix)/usr/local/bin/neutrino
	$(target)-strip $(targetprefix)/usr/local/bin/pzapit
	$(target)-strip $(targetprefix)/usr/local/bin/sectionsdcontrol
	touch $@

neutrino-nightly-clean neutrino-nightly-distclean:
	rm -f $(DEPDIR)/neutrino-nightly
	rm -f $(DEPDIR)/neutrino-nightly.do_compile
	rm -f $(DEPDIR)/neutrino-nightly.do_prepare
	rm -rf $(appsdir)/neutrino-nightly.org
	rm -rf $(appsdir)/neutrino-nightly


#
#NORMAL
#

$(appsdir)/neutrino/config.status: bootstrap freetype libpng libid3tag openssl curl libmad libboost libgif jpeg
	export PATH=$(hostprefix)/bin:$(PATH) && \
	cd $(appsdir)/neutrino && \
		ACLOCAL_FLAGS="-I $(hostprefix)/share/aclocal" ./autogen.sh && \
		$(BUILDENV) \
		./configure \
			--host=$(target) \
			--without-libsdl \
			--with-datadir=/usr/local/share \
			--with-libdir=/usr/lib \
			--with-plugindir=/usr/lib/tuxbox/plugins \
			--with-fontdir=/usr/local/share/fonts \
			--with-configdir=/usr/local/share/config \
			--with-gamesdir=/usr/local/share/games \
			PKG_CONFIG=$(hostprefix)/bin/pkg-config \
			PKG_CONFIG_PATH=$(targetprefix)/usr/lib/pkgconfig \
			CPPFLAGS="$(CPPFLAGS) -D__KERNEL_STRICT_NAMES -DPLATFORM_SPARK -I$(driverdir)/include -I $(buildprefix)/$(KERNEL_DIR)/include"

$(DEPDIR)/neutrino.do_prepare: Patches/neutrino.patch
	touch $@

$(DEPDIR)/neutrino.do_compile: $(appsdir)/neutrino/config.status
	cd $(appsdir)/neutrino && \
		$(MAKE) all
	touch $@

$(DEPDIR)/neutrino: neutrino.do_prepare neutrino.do_compile
	$(MAKE) -C $(appsdir)/neutrino install DESTDIR=$(targetprefix) DATADIR=$(targetprefix)/usr/local/share/
	$(target)-strip $(targetprefix)/usr/local/bin/neutrino
	$(target)-strip $(targetprefix)/usr/local/bin/pzapit
	$(target)-strip $(targetprefix)/usr/local/bin/sectionsdcontrol
	touch $@

neutrino-clean neutrino-distclean:
	rm -f $(DEPDIR)/neutrino
	rm -f $(DEPDIR)/neutrino.do_compile
	rm -f $(DEPDIR)/neutrino.do_prepare
	cd $(appsdir)/neutrino && \
		$(MAKE) distclean && \
		find $(appsdir)/neutrino -name "Makefile.in" -exec rm -rf {} \; && \
		rm -rf $(appsdir)/neutrino/autom4te.cache

#libogg is needed
