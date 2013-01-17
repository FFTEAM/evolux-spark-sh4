
min-prepare-yaud std-prepare-yaud max-prepare-yaud: \
%prepare-yaud:
	-rm -rf $(prefix)/$*cdkroot
	-rm -rf $(prefix)/$*cdkroot-rpmdb

#
# BOOTSTRAP
#
$(DEPDIR)/min-bootstrap $(DEPDIR)/std-bootstrap $(DEPDIR)/max-bootstrap $(DEPDIR)/bootstrap: \
$(DEPDIR)/%bootstrap: \
		%libtool \
		%$(FILESYSTEM) \
		| %$(GLIBC) \
		%$(CROSS_LIBGCC) \
		%$(LIBSTDC)
	@[ "x$*" = "x" ] && touch -r RPMS/sh4/$(STLINUX)-sh4-$(LIBSTDC)-$(GCC_VERSION).sh4.rpm $@ || true

#
# BARE-OS
#
min-bare-os std-bare-os max-bare-os bare-os: \
%bare-os: \
		%bootstrap \
		%$(LIBTERMCAP) \
		%$(NCURSES_BASE) \
		%$(NCURSES) \
		%$(BASE_PASSWD) \
		%$(MAKEDEV) \
		%$(BASE_FILES) \
		%module_init_tools \
		%busybox \
		\
		%libz \
		%$(INITSCRIPTS) \
		%openrdate \
		%$(NETBASE) \
		%$(BC) \
		%$(SYSVINIT) \
		%$(DISTRIBUTIONUTILS) \
		\
		%e2fsprogs \
		%jfsutils \
		%opkg \
		%u-boot-utils \
		%diverse-tools
#		%$(RELEASE) \
#		%$(FINDUTILS) \
#

min-net-utils std-net-utils max-net-utils net-utils: \
%net-utils:	\
		%$(NETKIT_FTP) \
		%autofs \
		%portmap \
		%$(NFSSERVER) \
		%vsftpd \
		%ethtool \
		%$(CIFS)

min-disk-utils std-disk-utils max-disk-utils disk-utils: \
%disk-utils:	\
		%$(XFSPROGS) \
		%util-linux \
		%$(SG3)

#dummy targets
#really ugly
min-:

std-:

max-:


#
# YAUD
#
yaud-vdr2: yaud-none lirc stslave\
		boot-elf remote firstboot vdrdev2 release_vdrdev2
	@TUXBOX_YAUD_CUSTOMIZE@
#make flash img
if ENABLE_MULTI_YAFFS2
	( cd $(prefix) && cd ../flash/spark && ./spark_multi_yaffs2.sh )
else
	( cd $(prefix) && cd ../flash/spark && ./spark.sh )
endif



yaud-neutrino: yaud-none lirc stslave\
		boot-elf remote firstboot neutrino neutrino-plugins release_neutrino
	@TUXBOX_YAUD_CUSTOMIZE@
#make flash img
if ENABLE_MULTI_YAFFS2
	( cd $(prefix) && cd ../flash/spark && ./spark_multi_yaffs2.sh )
else
	( cd $(prefix) && cd ../flash/spark && ./spark.sh )
endif

yaud-neutrino-hd: yaud-none lirc stslave\
		boot-elf remote firstboot neutrino-hd neutrino-plugins release_neutrino-hd
	@TUXBOX_YAUD_CUSTOMIZE@
#make flash img
if ENABLE_MULTI_YAFFS2
	( cd $(prefix) && cd ../flash/spark && ./spark_multi_yaffs2.sh )
else
	( cd $(prefix) && cd ../flash/spark && ./spark.sh )
endif

yaud-neutrino-mp: yaud-none lirc stslave\
		boot-elf remote firstboot neutrino-mp neutrino-plugins release_neutrino-mp
	@TUXBOX_YAUD_CUSTOMIZE@
#make flash img
if ENABLE_MULTI_YAFFS2
	( cd $(prefix) && cd ../flash/spark && ./spark_multi_yaffs2.sh )
else
	( cd $(prefix) && cd ../flash/spark && ./spark.sh )
endif

yaud-neutrino-hd2: yaud-none lirc stslave\
		boot-elf remote firstboot neutrino-hd2 neutrino-plugins release_neutrino-hd2
	@TUXBOX_YAUD_CUSTOMIZE@
#make flash img
if ENABLE_MULTI_YAFFS2
	( cd $(prefix) && cd ../flash/spark && ./spark_multi_yaffs2.sh )
else
	( cd $(prefix) && cd ../flash/spark && ./spark.sh )
endif

evolux: yaud-neutrino \
		yaud-enigma2-nightly \
		release_evolux
	@TUXBOX_YAUD_CUSTOMIZE@
#make flash img
if ENABLE_MULTI_YAFFS2
	( cd $(prefix) && cd ../flash/spark && ./spark_multi_yaffs2.sh )
else
	( cd $(prefix) && cd ../flash/spark && ./spark.sh )
endif

evolux-full: yaud-none host-python lirc stslave \
		yaud-neutrino \
		yaud-enigma2-nightly \
		release_evolux
	@TUXBOX_YAUD_CUSTOMIZE@
#make flash img
if ENABLE_MULTI_YAFFS2
	( cd $(prefix) && cd ../flash/spark && ./spark_multi_yaffs2.sh )
else
	( cd $(prefix) && cd ../flash/spark && ./spark.sh )
endif

evolux-pli: yaud-neutrino \
		yaud-enigma2-pli-nightly \
		release_evolux_pli
	@TUXBOX_YAUD_CUSTOMIZE@
#make flash img
if ENABLE_MULTI_YAFFS2
	( cd $(prefix) && cd ../flash/spark && ./spark_multi_yaffs2.sh )
else
	( cd $(prefix) && cd ../flash/spark && ./spark.sh )
endif

evolux-pli-full: yaud-none host-python lirc stslave \
		yaud-neutrino \
		yaud-enigma2-pli-nightly \
		release_evolux_pli
	@TUXBOX_YAUD_CUSTOMIZE@
#make flash img
if ENABLE_MULTI_YAFFS2
	( cd $(prefix) && cd ../flash/spark && ./spark_multi_yaffs2.sh )
else
	( cd $(prefix) && cd ../flash/spark && ./spark.sh )
endif

evolux-neutrino-hd-pli: yaud-neutrino-hd \
		yaud-enigma2-pli-nightly \
		release_evolux_neutrino-hd_pli
	@TUXBOX_YAUD_CUSTOMIZE@
#make flash img
if ENABLE_MULTI_YAFFS2
	( cd $(prefix) && cd ../flash/spark && ./spark_multi_yaffs2.sh )
else
	( cd $(prefix) && cd ../flash/spark && ./spark.sh )
endif

evolux-neutrino-hd-pli-full: yaud-none host-python lirc stslave \
		yaud-neutrino-hd \
		yaud-enigma2-pli-nightly \
		release_evolux_neutrino-hd_pli
	@TUXBOX_YAUD_CUSTOMIZE@
#make flash img
if ENABLE_MULTI_YAFFS2
	( cd $(prefix) && cd ../flash/spark && ./spark_multi_yaffs2.sh )
else
	( cd $(prefix) && cd ../flash/spark && ./spark.sh )
endif

evolux-neutrino-hd2-pli: yaud-enigma2-pli-nightly \
		yaud-neutrino-hd2 \
		release_evolux_neutrino-hd2_pli
	@TUXBOX_YAUD_CUSTOMIZE@
#make flash img
if ENABLE_MULTI_YAFFS2
	( cd $(prefix) && cd ../flash/spark && ./spark_multi_yaffs2.sh )
else
	( cd $(prefix) && cd ../flash/spark && ./spark.sh )
endif

evolux-neutrino-hd2-pli-full: yaud-none host-python lirc stslave \
		yaud-enigma2-pli-nightly \
		yaud-neutrino-hd2 \
		release_evolux_neutrino-hd2_pli
	@TUXBOX_YAUD_CUSTOMIZE@
#make flash img
if ENABLE_MULTI_YAFFS2
	( cd $(prefix) && cd ../flash/spark && ./spark_multi_yaffs2.sh )
else
	( cd $(prefix) && cd ../flash/spark && ./spark.sh )
endif

evolux-neutrino-hd2: yaud-neutrino-hd \
		yaud-neutrino-hd2 \
		release_evolux_neutrino-hd2
	@TUXBOX_YAUD_CUSTOMIZE@
#make flash img
if ENABLE_MULTI_YAFFS2
	( cd $(prefix) && cd ../flash/spark && ./spark_multi_yaffs2.sh )
else
	( cd $(prefix) && cd ../flash/spark && ./spark.sh )
endif

evolux-neutrino-hd2-full: yaud-none lirc stslave \
		yaud-neutrino-hd \
		yaud-neutrino-hd2 \
		release_evolux_neutrino-hd2
	@TUXBOX_YAUD_CUSTOMIZE@
#make flash img
if ENABLE_MULTI_YAFFS2
	( cd $(prefix) && cd ../flash/spark && ./spark_multi_yaffs2.sh )
else
	( cd $(prefix) && cd ../flash/spark && ./spark.sh )
endif

evolux-neutrino-hd-vdr2: yaud-neutrino-hd \
		yaud-vdr2 \
		release_evolux_neutrino-hd_vdr2
	@TUXBOX_YAUD_CUSTOMIZE@
#make flash img
if ENABLE_MULTI_YAFFS2
	( cd $(prefix) && cd ../flash/spark && ./spark_multi_yaffs2.sh )
else
	( cd $(prefix) && cd ../flash/spark && ./spark.sh )
endif


evolux-neutrino-hd-vdr2-full: yaud-none lirc stslave \
		yaud-neutrino-hd \
		yaud-vdr2 \
		release_evolux_neutrino-vdr2
	@TUXBOX_YAUD_CUSTOMIZE@
#make flash img
if ENABLE_MULTI_YAFFS2
	( cd $(prefix) && cd ../flash/spark && ./spark_multi_yaffs2.sh )
else
	( cd $(prefix) && cd ../flash/spark && ./spark.sh )
endif

evolux-triple: 	yaud-neutrino-hd \
		yaud-enigma2-pli-nightly \
		yaud-neutrino-hd2 \
		release_evolux_triple
	@TUXBOX_YAUD_CUSTOMIZE@
#make flash img
if ENABLE_MULTI_YAFFS2
	( cd $(prefix) && cd ../flash/spark && ./spark_multi_yaffs2.sh )
else
	( cd $(prefix) && cd ../flash/spark && ./spark.sh )
endif

evolux-triple-full: yaud-none host-python lirc stslave \
		yaud-neutrino-hd \
		yaud-enigma2-pli-nightly \
		yaud-neutrino-hd2 \
		release_evolux_triple
	@TUXBOX_YAUD_CUSTOMIZE@
#make flash img
if ENABLE_MULTI_YAFFS2
	( cd $(prefix) && cd ../flash/spark && ./spark_multi_yaffs2.sh )
else
	( cd $(prefix) && cd ../flash/spark && ./spark.sh )
endif

yaud-enigma2-nightly: yaud-none host-python lirc stslave \
		boot-elf remote firstboot enigma2-nightly enigma2-nightly-misc release
	@TUXBOX_YAUD_CUSTOMIZE@
#make flash img
if ENABLE_MULTI_YAFFS2
	( cd $(prefix) && cd ../flash/spark && ./spark_multi_yaffs2.sh )
else
	( cd $(prefix) && cd ../flash/spark && ./spark.sh )
endif

yaud-enigma2-pli-nightly: yaud-none host-python lirc \
		boot-elf remote firstboot enigma2-pli-nightly enigma2-pli-nightly-misc  enigma2-pli-plugins release-enigma2-pli-nightly
	@TUXBOX_YAUD_CUSTOMIZE@
#make flash img
if ENABLE_MULTI_YAFFS2
	( cd $(prefix) && cd ../flash/spark && ./spark_multi_yaffs2.sh )
else
	( cd $(prefix) && cd ../flash/spark && ./spark.sh )
endif

yaud-none:	bare-os \
		libdvdcss \
		libdvdread \
		libdvdnav \
		linux-kernel \
		net-utils \
		disk-utils \
		driver \
		alsa-lib \
		alsa-utils \
		misc-tools
	@TUXBOX_YAUD_CUSTOMIZE@
