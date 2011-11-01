# misc/tools

#$(appsdir)/misc/tools/config.status: bootstrap libpng
$(appsdir)/misc/tools/config.status: bootstrap
	export PATH=$(hostprefix)/bin:$(PATH) && \
	cd $(appsdir)/misc/tools && \
	libtoolize -f -c && \
	$(CONFIGURE) --prefix= \
	$(if $(MULTICOM322), --enable-multicom322) $(if $(MULTICOM324), --enable-multicom324)

$(DEPDIR)/min-misc-tools $(DEPDIR)/std-misc-tools $(DEPDIR)/max-misc-tools $(DEPDIR)/misc-tools: \
$(DEPDIR)/%misc-tools: driver libstdc++-dev libdvdnav libdvdcss libpng jpeg ffmpeg $(appsdir)/misc/tools/config.status
	$(MAKE) -C $(appsdir)/misc/tools all install DESTDIR=$(prefix)/$*cdkroot \
	CPPFLAGS="\
	$(if $(SPARK), -DPLATFORM_SPARK) \
	$(if $(PLAYER131), -DPLAYER131) \
	$(if $(PLAYER179), -DPLAYER179) \
	$(if $(PLAYER191), -DPLAYER191) \
	$(if $(STM22), -DSTM22)"
	[ "x$*" = "x" ] && touch $@ || true

flash-misc-tools: $(DEPDIR)/misc-tools \
		| $(flashprefix)/root
	$(MAKE) -C $(appsdir)/misc/tools install DESTDIR=$(flashprefix)/root
	@FLASHROOTDIR_MODIFIED@
	@TUXBOX_CUSTOMIZE@


flash-vfdctl: $(flashprefix)/root/bin/vfdctl

$(flashprefix)/root/bin/vfdctl: $(DEPDIR)/misc-tools \
		| $(flashprefix)/root
	$(MAKE) -C $(appsdir)/misc/tools/vfdctl install DESTDIR=$(flashprefix)/root
	touch $@
	@FLASHROOTDIR_MODIFIED@
	@TUXBOX_CUSTOMIZE@
