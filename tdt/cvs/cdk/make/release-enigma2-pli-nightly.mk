# auxiliary targets for model-specific builds
release_common_utils:
#       remove the slink to busybox
	rm -f $(prefix)/release-enigma2-pli/sbin/halt
	cp -f $(targetprefix)/sbin/halt $(prefix)/release-enigma2-pli/sbin/
	cp $(buildprefix)/root/release-enigma2-pli/umountfs $(prefix)/release-enigma2-pli/etc/init.d/
	cp $(buildprefix)/root/release-enigma2-pli/rc $(prefix)/release-enigma2-pli/etc/init.d/
	cp $(buildprefix)/root/release-enigma2-pli/sendsigs $(prefix)/release-enigma2-pli/etc/init.d/
	chmod 755 $(prefix)/release-enigma2-pli/etc/init.d/umountfs
	chmod 755 $(prefix)/release-enigma2-pli/etc/init.d/rc
	chmod 755 $(prefix)/release-enigma2-pli/etc/init.d/sendsigs
	chmod 755 $(prefix)/release-enigma2-pli/etc/init.d/halt
	mkdir -p $(prefix)/release-enigma2-pli/etc/rc.d/rc0.d
	ln -s ../init.d $(prefix)/release-enigma2-pli/etc/rc.d
	ln -fs halt $(prefix)/release-enigma2-pli/sbin/reboot
	ln -fs halt $(prefix)/release-enigma2-pli/sbin/poweroff
	ln -s ../init.d/sendsigs $(prefix)/release-enigma2-pli/etc/rc.d/rc0.d/S20sendsigs
	ln -s ../init.d/umountfs $(prefix)/release-enigma2-pli/etc/rc.d/rc0.d/S40umountfs
	ln -s ../init.d/halt $(prefix)/release-enigma2-pli/etc/rc.d/rc0.d/S90halt
	mkdir -p $(prefix)/release-enigma2-pli/etc/rc.d/rc6.d
	ln -s ../init.d/sendsigs $(prefix)/release-enigma2-pli/etc/rc.d/rc6.d/S20sendsigs
	ln -s ../init.d/umountfs $(prefix)/release-enigma2-pli/etc/rc.d/rc6.d/S40umountfs
	ln -s ../init.d/reboot $(prefix)/release-enigma2-pli/etc/rc.d/rc6.d/S90reboot

release_spark:
	echo "spark" > $(prefix)/release-enigma2-pli/etc/hostname
	rm -f $(prefix)/release-enigma2-pli/sbin/halt
	cp $(buildprefix)/root/release-enigma2-pli/halt_spark $(prefix)/release-enigma2-pli/etc/init.d/halt
	chmod 777 $(prefix)/release-enigma2-pli/etc/init.d/halt
	cp -f $(targetprefix)/sbin/halt $(prefix)/release-enigma2-pli/sbin/
	cp $(buildprefix)/root/release-enigma2-pli/umountfs $(prefix)/release-enigma2-pli/etc/init.d/
	cp $(buildprefix)/root/release-enigma2-pli/rc $(prefix)/release-enigma2-pli/etc/init.d/
	cp $(buildprefix)/root/release-enigma2-pli/sendsigs $(prefix)/release-enigma2-pli/etc/init.d/
	chmod 755 $(prefix)/release-enigma2-pli/etc/init.d/umountfs
	chmod 755 $(prefix)/release-enigma2-pli/etc/init.d/rc
	chmod 755 $(prefix)/release-enigma2-pli/etc/init.d/sendsigs
	chmod 755 $(prefix)/release-enigma2-pli/etc/init.d/halt
	mkdir -p $(prefix)/release-enigma2-pli/etc/rc.d/rc0.d
	ln -s ../init.d $(prefix)/release-enigma2-pli/etc/rc.d
	ln -fs halt $(prefix)/release-enigma2-pli/sbin/reboot
	ln -fs halt $(prefix)/release-enigma2-pli/sbin/poweroff
	ln -s ../init.d/sendsigs $(prefix)/release-enigma2-pli/etc/rc.d/rc0.d/S20sendsigs
	ln -s ../init.d/umountfs $(prefix)/release-enigma2-pli/etc/rc.d/rc0.d/S40umountfs
	ln -s ../init.d/halt $(prefix)/release-enigma2-pli/etc/rc.d/rc0.d/S90halt
	mkdir -p $(prefix)/release-enigma2-pli/etc/rc.d/rc6.d
	ln -s ../init.d/sendsigs $(prefix)/release-enigma2-pli/etc/rc.d/rc6.d/S20sendsigs
	ln -s ../init.d/umountfs $(prefix)/release-enigma2-pli/etc/rc.d/rc6.d/S40umountfs
	ln -s ../init.d/reboot $(prefix)/release-enigma2-pli/etc/rc.d/rc6.d/S90reboot
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/smartcard/smartcard.ko $(prefix)/release-enigma2-pli/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/frontends/lnb/lnb.ko $(prefix)/release-enigma2-pli/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stmcore-display-sti7111.ko $(prefix)/release-enigma2-pli/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/frontcontroller/aotom/aotom.ko $(prefix)/release-enigma2-pli/lib/modules/
	cp $(targetprefix)/boot/video_7111.elf $(prefix)/release-enigma2-pli/boot/video.elf
	cp $(targetprefix)/boot/audio_7111.elf $(prefix)/release-enigma2-pli/boot/audio.elf
	cp -f $(buildprefix)/root/usr/local/share/enigma2/keymap_spark.xml $(prefix)/release-enigma2-pli/usr/local/share/enigma2/keymap.xml
	cp -f $(buildprefix)/root/sbin/flash_* $(prefix)/release-enigma2-pli/sbin
	cp -f $(buildprefix)/root/sbin/nand* $(prefix)/release-enigma2-pli/sbin

	cp -dp $(buildprefix)/root/etc/lircd_spark.conf $(prefix)/release-enigma2-pli/etc/lircd.conf
	cp -p $(targetprefix)/usr/bin/lircd $(prefix)/release-enigma2-pli/usr/bin/
	mkdir -p $(prefix)/release-enigma2-pli/var/run/lirc

	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/usr/local/share/fonts
	cp $(targetprefix)/usr/local/share/fonts/* $(prefix)/release-enigma2-pli/usr/share/fonts/

	cp $(kernelprefix)/linux-sh4/drivers/usb/serial/ftdi_sio.ko $(prefix)/release-enigma2-pli/lib/modules/ftdi.ko
	cp $(kernelprefix)/linux-sh4/drivers/usb/serial/pl2303.ko $(prefix)/release-enigma2-pli/lib/modules
	cp $(kernelprefix)/linux-sh4/fs/autofs4/autofs4.ko $(prefix)/release-enigma2-pli/lib/modules

	rm -f $(prefix)/release-enigma2-pli/lib/firmware/dvb-fe-avl2108.fw
	rm -f $(prefix)/release-enigma2-pli/lib/firmware/dvb-fe-stv6306.fw
	rm -f $(prefix)/release-enigma2-pli/lib/firmware/dvb-fe-cx24116.fw
	rm -f $(prefix)/release-enigma2-pli/lib/firmware/dvb-fe-cx21143.fw
	rm -f $(prefix)/release-enigma2-pli/bin/evremote
	rm -f $(prefix)/release-enigma2-pli/bin/gotosleep
	rm -f $(prefix)/release-enigma2-pli/bin/vdstandby

release_spark7162:
	echo "spark7162" > $(prefix)/release-enigma2-pli/etc/hostname
	rm -f $(prefix)/release-enigma2-pli/sbin/halt
	cp $(buildprefix)/root/release-enigma2-pli/halt_spark $(prefix)/release-enigma2-pli/etc/init.d/halt
	chmod 777 $(prefix)/release-enigma2-pli/etc/init.d/halt
	cp -f $(targetprefix)/sbin/halt $(prefix)/release-enigma2-pli/sbin/
	cp $(buildprefix)/root/release-enigma2-pli/umountfs $(prefix)/release-enigma2-pli/etc/init.d/
	cp $(buildprefix)/root/release-enigma2-pli/rc $(prefix)/release-enigma2-pli/etc/init.d/
	cp $(buildprefix)/root/release-enigma2-pli/sendsigs $(prefix)/release-enigma2-pli/etc/init.d/
	chmod 755 $(prefix)/release-enigma2-pli/etc/init.d/umountfs
	chmod 755 $(prefix)/release-enigma2-pli/etc/init.d/rc
	chmod 755 $(prefix)/release-enigma2-pli/etc/init.d/sendsigs
	chmod 755 $(prefix)/release-enigma2-pli/etc/init.d/halt
	mkdir -p $(prefix)/release-enigma2-pli/etc/rc.d/rc0.d
	ln -s ../init.d $(prefix)/release-enigma2-pli/etc/rc.d
	ln -fs halt $(prefix)/release-enigma2-pli/sbin/reboot
	ln -fs halt $(prefix)/release-enigma2-pli/sbin/poweroff
	ln -s ../init.d/sendsigs $(prefix)/release-enigma2-pli/etc/rc.d/rc0.d/S20sendsigs
	ln -s ../init.d/umountfs $(prefix)/release-enigma2-pli/etc/rc.d/rc0.d/S40umountfs
	ln -s ../init.d/halt $(prefix)/release-enigma2-pli/etc/rc.d/rc0.d/S90halt
	mkdir -p $(prefix)/release-enigma2-pli/etc/rc.d/rc6.d
	ln -s ../init.d/sendsigs $(prefix)/release-enigma2-pli/etc/rc.d/rc6.d/S20sendsigs
	ln -s ../init.d/umountfs $(prefix)/release-enigma2-pli/etc/rc.d/rc6.d/S40umountfs
	ln -s ../init.d/reboot $(prefix)/release-enigma2-pli/etc/rc.d/rc6.d/S90reboot

	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/smartcard/smartcard.ko $(prefix)/release-enigma2-pli/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stmcore-display-sti7105.ko $(prefix)/release-enigma2-pli/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/frontcontroller/aotom/aotom.ko $(prefix)/release-enigma2-pli/lib/modules/
	cp $(targetprefix)/boot/video_7105.elf $(prefix)/release-enigma2-pli/boot/video.elf
	cp $(targetprefix)/boot/audio_7105.elf $(prefix)/release-enigma2-pli/boot/audio.elf
	cp -f $(buildprefix)/root/usr/local/share/enigma2/keymap_spark.xml $(prefix)/release-enigma2-pli/usr/local/share/enigma2/keymap.xml
	cp -f $(buildprefix)/root/release-enigma2-pli/tuner.ko$(KERNELSTMLABEL)_spark7162 $(prefix)/release-enigma2-pli/lib/modules/spark7162.ko
	cp -f $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/i2c_spi/i2s.ko $(prefix)/release-enigma2-pli/lib/modules/
	cp -f $(buildprefix)/root/sbin/flashcp $(prefix)/release-enigma2-pli/sbin
	cp -f $(buildprefix)/root/sbin/flash_* $(prefix)/release-enigma2-pli/sbin
	cp -f $(buildprefix)/root/sbin/nand* $(prefix)/release-enigma2-pli/sbin

	cp -dp $(buildprefix)/root/etc/lircd_spark7162.conf $(prefix)/release-enigma2-pli/etc/lircd.conf
	cp -p $(targetprefix)/usr/bin/lircd $(prefix)/release-enigma2-pli/usr/bin/
	mkdir -p $(prefix)/release-enigma2-pli/var/run/lirc

	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/usr/local/share/fonts
	cp $(targetprefix)/usr/local/share/fonts/* $(prefix)/release-enigma2-pli/usr/share/fonts/

	cp $(kernelprefix)/linux-sh4/drivers/usb/serial/ftdi_sio.ko $(prefix)/release-enigma2-pli/lib/modules/ftdi.ko
	cp $(kernelprefix)/linux-sh4/drivers/usb/serial/pl2303.ko $(prefix)/release-enigma2-pli/lib/modules
	cp $(kernelprefix)/linux-sh4/drivers/usb/serial/usbserial.ko $(prefix)/release-enigma2-pli/lib/modules
	cp $(kernelprefix)/linux-sh4/fs/autofs4/autofs4.ko $(prefix)/release-enigma2-pli/lib/modules

	rm -f $(prefix)/release-enigma2-pli/lib/firmware/dvb-fe-avl2108.fw
	rm -f $(prefix)/release-enigma2-pli/lib/firmware/dvb-fe-stv6306.fw
	rm -f $(prefix)/release-enigma2-pli/lib/firmware/dvb-fe-cx24116.fw
	rm -f $(prefix)/release-enigma2-pli/lib/firmware/dvb-fe-cx21143.fw
	rm -f $(prefix)/release-enigma2-pli/bin/evremote
	rm -f $(prefix)/release-enigma2-pli/bin/gotosleep
	rm -f $(prefix)/release-enigma2-pli/bin/vdstandby

#
# The main target depends on the model.
# IMPORTANT: it is assumed that only one variable is set. Otherwise the target name won't be resolved.
#
$(DEPDIR)/min-release $(DEPDIR)/std-release $(DEPDIR)/max-release $(DEPDIR)/ipk-release $(DEPDIR)/release: \
$(DEPDIR)/%release: release_base release_$(SPARK)$(SPARK7162)
	touch $@


enigma2-pli-nightly-clean:
	rm -f $(DEPDIR)/enigma2-pli-nightly*
	cd $(appsdir)/enigma2-pli-nightly && \
		$(MAKE) distclean && \
		find $(appsdir)/enigma2-pli-nightly -name "Makefile.in" -exec rm -rf {} \; && \
		rm -rf $(appsdir)/enigma2-pli-nightly/autom4te.cache

enigma2-nightly-distclean:
	rm -f $(DEPDIR)/enigma2-pli-nightly*
	rm -rf $(appsdir)/enigma2-pli-nightly
	rm -rf $(appsdir)/enigma2-pli-nightly.newest
	rm -rf $(appsdir)/enigma2-pli-nightly.org
	rm -rf $(appsdir)/enigma2-pli-nightly.patched


# the following target creates the common file base
release_base:
	rm -rf $(prefix)/release || true
	$(INSTALL_DIR) $(prefix)/release && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/bin && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/sbin && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/boot && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/dev && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/dev.static && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/etc && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/etc/fonts && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/etc/init.d && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/etc/network && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/etc/network/if-down.d && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/etc/network/if-post-down.d && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/etc/network/if-pre-up.d && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/etc/network/if-up.d && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/etc/tuxbox && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/etc/enigma2 && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/hdd && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/hdd/movie && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/hdd/music && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/hdd/picture && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/lib && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/lib/modules && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/ram && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/var && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/var/etc && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/var/opkg && \
	export CROSS_COMPILE=$(target)- && \
		$(MAKE) install -C @DIR_busybox@ CONFIG_PREFIX=$(prefix)/release && \
	touch $(prefix)/release-enigma2-pli/var/etc/.firstboot && \
	cp -a $(targetprefix)/bin/* $(prefix)/release-enigma2-pli/bin/ && \
	ln -s /bin/showiframe $(prefix)/release-enigma2-pli/usr/bin/showiframe && \
	cp -dp $(targetprefix)/bin/hotplug $(prefix)/release-enigma2-pli/sbin/ && \
	cp -dp $(targetprefix)/usr/bin/sdparm $(prefix)/release-enigma2-pli/sbin/ && \
	cp -dp $(targetprefix)/sbin/init $(prefix)/release-enigma2-pli/sbin/ && \
	cp -dp $(targetprefix)/sbin/killall5 $(prefix)/release-enigma2-pli/sbin/ && \
	cp -dp $(targetprefix)/sbin/portmap $(prefix)/release-enigma2-pli/sbin/ && \
	cp -dp $(targetprefix)/sbin/mke2fs $(prefix)/release-enigma2-pli/sbin/ && \
	ln -sf /sbin/mke2fs $(prefix)/release-enigma2-pli/sbin/mkfs.ext2 && \
	ln -sf /sbin/mke2fs $(prefix)/release-enigma2-pli/sbin/mkfs.ext3 && \
	ln -sf /sbin/mke2fs $(prefix)/release-enigma2-pli/sbin/mkfs.ext4 && \
	ln -sf /sbin/mke2fs $(prefix)/release-enigma2-pli/sbin/mkfs.ext4dev && \
	cp -dp $(targetprefix)/sbin/fsck $(prefix)/release-enigma2-pli/sbin/ && \
	cp -dp $(targetprefix)/sbin/e2fsck $(prefix)/release-enigma2-pli/sbin/ && \
	ln -sf /sbin/e2fsck $(prefix)/release-enigma2-pli/sbin/fsck.ext2 && \
	ln -sf /sbin/e2fsck $(prefix)/release-enigma2-pli/sbin/fsck.ext3 && \
	ln -sf /sbin/e2fsck $(prefix)/release-enigma2-pli/sbin/fsck.ext4 && \
	ln -sf /sbin/e2fsck $(prefix)/release-enigma2-pli/sbin/fsck.ext4dev && \
	cp -dp $(targetprefix)/sbin/jfs_fsck $(prefix)/release-enigma2-pli/sbin/ && \
	ln -sf /sbin/jfs_fsck $(prefix)/release-enigma2-pli/sbin/fsck.jfs && \
	cp -dp $(targetprefix)/sbin/jfs_mkfs $(prefix)/release-enigma2-pli/sbin/ && \
	ln -sf /sbin/jfs_mkfs $(prefix)/release-enigma2-pli/sbin/mkfs.jfs && \
	cp -dp $(targetprefix)/sbin/jfs_tune $(prefix)/release-enigma2-pli/sbin/ && \
	cp -dp $(targetprefix)/sbin/fsck.nfs $(prefix)/release-enigma2-pli/sbin/ && \
	cp -dp $(targetprefix)/sbin/sfdisk $(prefix)/release-enigma2-pli/sbin/ && \
	cp -dp $(targetprefix)/sbin/tune2fs $(prefix)/release-enigma2-pli/sbin/ && \
	cp -dp $(targetprefix)/etc/init.d/portmap $(prefix)/release-enigma2-pli/etc/init.d/ && \
	cp -dp $(buildprefix)/root/etc/init.d/udhcpc $(prefix)/release-enigma2-pli/etc/init.d/ && \
	cp -dp $(targetprefix)/sbin/MAKEDEV$(if $(SPARK),_no_CI) $(prefix)/release-enigma2-pli/sbin/MAKEDEV && \
	cp -dp $(targetprefix)/usr/bin/grep $(prefix)/release-enigma2-pli/bin/ && \
	cp -dp $(targetprefix)/usr/bin/egrep $(prefix)/release-enigma2-pli/bin/ && \
	cp $(targetprefix)/boot/audio.elf $(prefix)/release-enigma2-pli/boot/audio.elf && \
	cp -a $(targetprefix)/dev/* $(prefix)/release-enigma2-pli/dev/ && \
	cp -dp $(targetprefix)/etc/fstab $(prefix)/release-enigma2-pli/etc/ && \
	cp -dp $(targetprefix)/etc/group $(prefix)/release-enigma2-pli/etc/ && \
	cp -dp $(targetprefix)/etc/host.conf $(prefix)/release-enigma2-pli/etc/ && \
	cp -dp $(targetprefix)/etc/hostname $(prefix)/release-enigma2-pli/etc/ && \
	cp -dp $(targetprefix)/etc/hosts $(prefix)/release-enigma2-pli/etc/ && \
	cp -dp $(targetprefix)/etc/inetd.conf $(prefix)/release-enigma2-pli/etc/ && \
	cp -dp $(targetprefix)/etc/inittab $(prefix)/release-enigma2-pli/etc/ && \
	cp -dp $(targetprefix)/etc/localtime $(prefix)/release-enigma2-pli/etc/ && \
	cp -dp $(targetprefix)/etc/mtab $(prefix)/release-enigma2-pli/etc/ && \
	cp -dp $(targetprefix)/etc/passwd $(prefix)/release-enigma2-pli/etc/ && \
	cp -dp $(targetprefix)/etc/profile $(prefix)/release-enigma2-pli/etc/ && \
	cp -dp $(targetprefix)/etc/protocols $(prefix)/release-enigma2-pli/etc/ && \
	cp -dp $(targetprefix)/etc/resolv.conf $(prefix)/release-enigma2-pli/etc/ && \
	cp -dp $(targetprefix)/etc/services $(prefix)/release-enigma2-pli/etc/ && \
	cp -dp $(targetprefix)/etc/shells $(prefix)/release-enigma2-pli/etc/ && \
	cp -dp $(targetprefix)/etc/shells.conf $(prefix)/release-enigma2-pli/etc/ && \
	cp -dp $(targetprefix)/etc/vsftpd.conf $(prefix)/release-enigma2-pli/etc/ && \
	cp -dp $(targetprefix)/etc/inetd.conf $(prefix)/release-enigma2-pli/etc/ && \
	cp -dp $(targetprefix)/etc/image-version $(prefix)/release-enigma2-pli/etc/ && \
	cp -dp $(targetprefix)/etc/timezone.xml $(prefix)/release-enigma2-pli/etc/ && \
	cp -dp $(targetprefix)/etc/vdstandby.cfg $(prefix)/release-enigma2-pli/etc/ && \
	cp -dp $(targetprefix)/etc/network/interfaces $(prefix)/release-enigma2-pli/etc/network/ && \
	cp -dp $(targetprefix)/etc/network/options $(prefix)/release-enigma2-pli/etc/network/ && \
	cp -dp $(targetprefix)/etc/init.d/umountfs $(prefix)/release-enigma2-pli/etc/init.d/ && \
	cp -dp $(targetprefix)/etc/init.d/sendsigs $(prefix)/release-enigma2-pli/etc/init.d/ && \
	cp -dp $(targetprefix)/etc/init.d/halt $(prefix)/release-enigma2-pli/etc/init.d/ && \
	cp $(buildprefix)/root/release-enigma2-pli/reboot $(prefix)/release-enigma2-pli/etc/init.d/ && \
	cp $(targetprefix)/etc/tuxbox/satellites.xml $(prefix)/release-enigma2-pli/etc/tuxbox/ && \
	cp $(targetprefix)/etc/tuxbox/cables.xml $(prefix)/release-enigma2-pli/etc/tuxbox/ && \
	cp $(targetprefix)/etc/tuxbox/terrestrial.xml $(prefix)/release-enigma2-pli/etc/tuxbox/ && \
	ln -sf /etc/timezone.xml $(prefix)/release-enigma2-pli/etc/tuxbox/timezone.xml && \
	echo "576i50" > $(prefix)/release-enigma2-pli/etc/videomode && \
	cp -R $(targetprefix)/etc/fonts/* $(prefix)/release-enigma2-pli/etc/fonts/ && \
	cp $(buildprefix)/root/release-enigma2-pli/rcS$(if $(SPARK),_$(SPARK))$(if $(SPARK7162),_$(SPARK7162)) $(prefix)/release-enigma2-pli/etc/init.d/rcS && \
	chmod 755 $(prefix)/release-enigma2-pli/etc/init.d/rcS && \
	cp $(buildprefix)/root/release-enigma2-pli/mountvirtfs $(prefix)/release-enigma2-pli/etc/init.d/ && \
	cp $(buildprefix)/root/release-enigma2-pli/mme_check $(prefix)/release-enigma2-pli/etc/init.d/ && \
	cp $(buildprefix)/root/release-enigma2-pli/mountall $(prefix)/release-enigma2-pli/etc/init.d/ && \
	cp $(buildprefix)/root/release-enigma2-pli/hostname $(prefix)/release-enigma2-pli/etc/init.d/ && \
	cp $(buildprefix)/root/release-enigma2-pli/vsftpd $(prefix)/release-enigma2-pli/etc/init.d/ && \
	cp $(buildprefix)/root/release-enigma2-pli/bootclean.sh $(prefix)/release-enigma2-pli/etc/init.d/ && \
	cp $(buildprefix)/root/release-enigma2-pli/networking $(prefix)/release-enigma2-pli/etc/init.d/ && \
	cp $(buildprefix)/root/bootscreen/bootlogo.mvi $(prefix)/release-enigma2-pli/boot/ && \
	cp $(buildprefix)/root/bin/autologin $(prefix)/release-enigma2-pli/bin/ && \
	cp $(buildprefix)/root/bin/vdstandby $(prefix)/release-enigma2-pli/bin/ && \
	cp -rd $(targetprefix)/lib/* $(prefix)/release-enigma2-pli/lib/ && \
	rm -f $(prefix)/release-enigma2-pli/lib/*.a && \
	rm -f $(prefix)/release-enigma2-pli/lib/*.o && \
	rm -f $(prefix)/release-enigma2-pli/lib/*.la && \
	find $(prefix)/release-enigma2-pli/lib/ -name '*.so*' -exec sh4-linux-strip --strip-unneeded {} \;

if STM24
	cp -dp $(targetprefix)/sbin/mkfs $(prefix)/release-enigma2-pli/sbin/
endif

if !STM22
	cp $(buildprefix)/root/release-enigma2-pli/rcS_stm23$(if $(SPARK),_$(SPARK))$(if $(SPARK7162),_$(SPARK7162)) $(prefix)/release-enigma2-pli/etc/init.d/rcS
endif

if ENABLE_PLAYER191
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stm_v4l2.ko $(prefix)/release-enigma2-pli/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stmvbi.ko $(prefix)/release-enigma2-pli/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stmvout.ko $(prefix)/release-enigma2-pli/lib/modules/
	cd $(targetprefix)/lib/modules/$(KERNELVERSION)/extra && \
	for mod in \
		sound/pseudocard/pseudocard.ko \
		sound/silencegen/silencegen.ko \
		stm/mmelog/mmelog.ko \
		stm/monitor/stm_monitor.ko \
		media/dvb/stm/dvb/stmdvb.ko \
		sound/ksound/ksound.ko \
		sound/kreplay/kreplay.ko \
		sound/kreplay/kreplay-fdma.ko \
		sound/ksound/ktone.ko \
		media/dvb/stm/mpeg2_hard_host_transformer/mpeg2hw.ko \
		media/dvb/stm/backend/player2.ko \
		media/dvb/stm/h264_preprocessor/sth264pp.ko \
		media/dvb/stm/allocator/stmalloc.ko \
		stm/platform/platform.ko \
		stm/platform/p2div64.ko \
		media/sysfs/stm/stmsysfs.ko \
	;do \
		echo `pwd` player2/linux/drivers/$$mod; \
		if [ -e player2/linux/drivers/$$mod ]; then \
			cp player2/linux/drivers/$$mod $(prefix)/release-enigma2-pli/lib/modules/; \
			sh4-linux-strip --strip-unneeded $(prefix)/release-enigma2-pli/lib/modules/`basename $$mod`; \
		else \
			touch $(prefix)/release-enigma2-pli/lib/modules/`basename $$mod`; \
		fi; \
		echo "."; \
	done
	echo "touched";
endif

	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/avs/avs.ko $(prefix)/release-enigma2-pli/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/boxtype/boxtype.ko $(prefix)/release-enigma2-pli/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/simu_button/simu_button.ko $(prefix)/release-enigma2-pli/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/e2_proc/e2_proc.ko $(prefix)/release-enigma2-pli/lib/modules/

	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stmfb.ko $(prefix)/release-enigma2-pli/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/multicom/embxshell/embxshell.ko $(prefix)/release-enigma2-pli/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/multicom/embxmailbox/embxmailbox.ko $(prefix)/release-enigma2-pli/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/multicom/embxshm/embxshm.ko $(prefix)/release-enigma2-pli/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/multicom/mme/mme_host.ko $(prefix)/release-enigma2-pli/lib/modules/

	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/frontends/*.ko $(prefix)/release-enigma2-pli/lib/modules/

	[ -e $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/pti/pti.ko ] && cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/pti/pti.ko $(prefix)/release-enigma2-pli/lib/modules || true
	[ -e $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/pti_np/pti.ko ] && cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/pti_np/pti.ko $(prefix)/release-enigma2-pli/lib/modules || true
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/compcache/lzo-kmod/lzo1x_compress.ko $(prefix)/release-enigma2-pli/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/compcache/lzo-kmod/lzo1x_decompress.ko $(prefix)/release-enigma2-pli/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/compcache/ramzswap.ko $(prefix)/release-enigma2-pli/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/bpamem/bpamem.ko $(prefix)/release-enigma2-pli/lib/modules/
	find $(prefix)/release-enigma2-pli/lib/modules/ -name '*.ko' -exec sh4-linux-strip --strip-unneeded {} \;

	rm -rf $(prefix)/release-enigma2-pli/lib/autofs
	rm -rf $(prefix)/release-enigma2-pli/lib/modules/$(KERNELVERSION)

	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/media
	ln -s /hdd $(prefix)/release-enigma2-pli/media/hdd
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/media/dvd

	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/mnt
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/mnt/usb
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/mnt/hdd
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/mnt/nfs

	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/root

	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/proc
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/sys
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/tmp

	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/usr
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/usr/bin
	cp -p $(targetprefix)/usr/sbin/vsftpd $(prefix)/release-enigma2-pli/usr/bin/
	cp -p $(targetprefix)/usr/bin/python $(prefix)/release-enigma2-pli/usr/bin/

	cp -p $(targetprefix)/usr/bin/killall $(prefix)/release-enigma2-pli/usr/bin/
	cp -p $(targetprefix)/usr/bin/opkg-cl $(prefix)/release-enigma2-pli/usr/bin/opkg
	cp -p $(targetprefix)/usr/bin/ffmpeg $(prefix)/release-enigma2-pli/sbin/
	if [ -e $(targetprefix)/usr/bin/tuxtxt ]; then \
		cp -p $(targetprefix)/usr/bin/tuxtxt $(prefix)/release-enigma2-pli/usr/bin/; \
	fi
	
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/usr/tuxtxt
	cp -p $(targetprefix)/etc/tuxbox/tuxtxt2.conf $(prefix)/release-enigma2-pli/usr/tuxtxt/
	
	cp -p $(targetprefix)/usr/sbin/ethtool $(prefix)/release-enigma2-pli/usr/sbin/

	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/usr/share

	ln -s /usr/local/share/enigma2 $(prefix)/release-enigma2-pli/usr/share/enigma2

	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/usr/share/fonts
	cp $(targetprefix)/usr/share/fonts/ae_AlMateen.ttf $(prefix)/release-enigma2-pli/usr/share/fonts/
	cp $(targetprefix)/usr/share/fonts/lcd.ttf $(prefix)/release-enigma2-pli/usr/share/fonts/
	cp $(targetprefix)/usr/share/fonts/md_khmurabi_10.ttf $(prefix)/release-enigma2-pli/usr/share/fonts/
	cp $(targetprefix)/usr/share/fonts/nmsbd.ttf $(prefix)/release-enigma2-pli/usr/share/fonts/
	cp $(targetprefix)/usr/share/fonts/tuxtxt.ttf $(prefix)/release-enigma2-pli/usr/share/fonts/
	if [ -e $(targetprefix)/usr/share/fonts/tuxtxt.otb ]; then \
		cp $(targetprefix)/usr/share/fonts/tuxtxt.otb $(prefix)/release-enigma2-pli/usr/share/fonts/; \
	fi
	cp $(targetprefix)/usr/share/fonts/goodtime.ttf $(prefix)/release-enigma2-pli/usr/share/fonts/
	cp $(targetprefix)/usr/share/fonts/valis_enigma.ttf $(prefix)/release-enigma2-pli/usr/share/fonts/
	cp $(targetprefix)/usr/share/fonts/valis_lcd.ttf $(prefix)/release-enigma2-pli/usr/share/fonts/
	cp $(targetprefix)/usr/share/fonts/seg.ttf $(prefix)/release-enigma2-pli/usr/share/fonts/
	cp $(targetprefix)/usr/share/fonts/seg_internat.ttf $(prefix)/release-enigma2-pli/usr/share/fonts/
	cp $(targetprefix)/usr/share/fonts/Symbols.ttf $(prefix)/release-enigma2-pli/usr/share/fonts/
	if [ -e $(targetprefix)/usr/local/share/fonts/andale.ttf ]; then \
		cp $(targetprefix)/usr/local/share/fonts/andale.ttf $(prefix)/release-enigma2-pli/usr/share/fonts/; \
	fi
	
#       Font libass
	cp $(buildprefix)/root/usr/share/fonts/FreeSans.ttf $(prefix)/release-enigma2-pli/usr/share/fonts/

	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/usr/share/zoneinfo
	cp -aR $(buildprefix)/root/usr/share/zoneinfo/* $(prefix)/release-enigma2-pli/usr/share/zoneinfo/

	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/usr/share/udhcpc
	cp -aR $(buildprefix)/root/usr/share/udhcpc/* $(prefix)/release-enigma2-pli/usr/share/udhcpc/

	if [ -e $(targetprefix)/usr/share/alsa ]; then \
		mkdir $(prefix)/release-enigma2-pli/usr/share/alsa/; \
		mkdir $(prefix)/release-enigma2-pli/usr/share/alsa/cards/; \
		mkdir $(prefix)/release-enigma2-pli/usr/share/alsa/pcm/; \
		cp $(targetprefix)/usr/share/alsa/alsa.conf          $(prefix)/release-enigma2-pli/usr/share/alsa/alsa.conf; \
		cp $(targetprefix)/usr/share/alsa/cards/aliases.conf $(prefix)/release-enigma2-pli/usr/share/alsa/cards/; \
		cp $(targetprefix)/usr/share/alsa/pcm/default.conf   $(prefix)/release-enigma2-pli/usr/share/alsa/pcm/; \
		cp $(targetprefix)/usr/share/alsa/pcm/dmix.conf      $(prefix)/release-enigma2-pli/usr/share/alsa/pcm/; fi

	ln -s /usr/local/share/keymaps $(prefix)/release-enigma2-pli/usr/share/keymaps

	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/usr/local
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/usr/local/bin
	if [ -e $(targetprefix)/usr/bin/enigma2 ]; then \
		cp -f $(targetprefix)/usr/bin/enigma2 $(prefix)/release-enigma2-pli/usr/local/bin/enigma2;fi
	if [ -e $(targetprefix)/usr/local/bin/enigma2 ]; then \
		cp -f $(targetprefix)/usr/local/bin/enigma2 $(prefix)/release-enigma2-pli/usr/local/bin/enigma2;fi
	find $(prefix)/release-enigma2-pli/usr/local/bin/ -name  enigma2 -exec sh4-linux-strip --strip-unneeded {} \;

	ln -s /etc $(prefix)/release-enigma2-pli/usr/local/etc

	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/usr/local/share
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/usr/local/share/keymaps
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/usr/local/share/enigma2
	cp -a $(targetprefix)/usr/local/share/enigma2/* $(prefix)/release-enigma2-pli/usr/local/share/enigma2
	cp -a $(targetprefix)/etc/enigma2/* $(prefix)/release-enigma2-pli/etc/enigma2

	cp -f $(buildprefix)/root/usr/local/share/enigma2/keymap_ufs910.xml $(prefix)/release-enigma2-pli/usr/local/share/enigma2/keymap.xml

	ln -s /usr/share/fonts $(prefix)/release-enigma2-pli/usr/local/share/fonts

	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/usr/lib
	cp -R $(targetprefix)/usr/lib/* $(prefix)/release-enigma2-pli/usr/lib/
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/engines
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/enigma2
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/gconv
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/libxslt-plugins
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/pkgconfig
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/sigc++-1.2
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/X11
	rm -f $(prefix)/release-enigma2-pli/usr/lib/*.a
	rm -f $(prefix)/release-enigma2-pli/usr/lib/*.o
	rm -f $(prefix)/release-enigma2-pli/usr/lib/*.la
	find $(prefix)/release-enigma2-pli/usr/lib/ -name '*.so*' -exec sh4-linux-strip --strip-unneeded {} \;

	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/usr/lib/enigma2
	cp -a $(targetprefix)/usr/lib/enigma2/* $(prefix)/release-enigma2-pli/usr/lib/enigma2/
	if test -d $(targetprefix)/usr/local/lib/enigma2; then \
		cp -a $(targetprefix)/usr/local/lib/enigma2/* $(prefix)/release-enigma2-pli/usr/lib/enigma2/; fi

#	Dont remove pyo files, remove pyc instead
	find $(prefix)/release-enigma2-pli/usr/lib/enigma2/ -name '*.pyc' -exec rm -f {} \;
	find $(prefix)/release-enigma2-pli/usr/lib/enigma2/ -name '*.a' -exec rm -f {} \;
	find $(prefix)/release-enigma2-pli/usr/lib/enigma2/ -name '*.o' -exec rm -f {} \;
	find $(prefix)/release-enigma2-pli/usr/lib/enigma2/ -name '*.la' -exec rm -f {} \;
	find $(prefix)/release-enigma2-pli/usr/lib/enigma2/ -name '*.so*' -exec sh4-linux-strip --strip-unneeded {} \;

#	Delete unnecessary plugins
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/enigma2/python/Plugins/DemoPlugins
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/enigma2/python/Plugins/SystemPlugins/FrontprocessorUpgrade
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/enigma2/python/Plugins/SystemPlugins/NFIFlash
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/enigma2/python/Plugins/Extensions/FileManager
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/enigma2/python/Plugins/Extensions/TuxboxPlugins

	$(INSTALL_DIR) $(prefix)/release-enigma2-pli/usr/lib/python2.6
	cp -a $(targetprefix)/usr/lib/python2.6/* $(prefix)/release-enigma2-pli/usr/lib/python2.6/
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/test
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/site-packages/lxml
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/site-packages/lxml-2.0.5-py2.6.egg-info
	rm -f $(prefix)/release-enigma2-pli/usr/lib/python2.6/site-packages/libxml2mod.so
	rm -f $(prefix)/release-enigma2-pli/usr/lib/python2.6/site-packages/libxsltmod.so
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/site-packages/OpenSSL/test
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/site-packages/setuptools
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/site-packages/setuptools-0.6c8-py2.6.egg-info
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/site-packages/zope.interface-3.3.0-py2.6.egg-info
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/site-packages/twisted/test
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/site-packages/twisted/conch
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/site-packages/twisted/mail
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/site-packages/twisted/manhole
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/site-packages/twisted/names
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/site-packages/twisted/news
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/site-packages/twisted/trial
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/site-packages/twisted/words
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/site-packages/twisted/application
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/site-packages/twisted/enterprise
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/site-packages/twisted/flow
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/site-packages/twisted/lore
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/site-packages/twisted/pair
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/site-packages/twisted/runner
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/site-packages/twisted/scripts
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/site-packages/twisted/tap
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/site-packages/twisted/topfiles
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/site-packages/Twisted-8.2.0-py2.6.egg-info
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/bsddb
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/compiler
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/config
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/ctypes
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/curses
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/distutils
	rm -rf $(prefix)/release-enigma2-pli/usr/lib/python2.6/email

#	Dont remove pyo files, remove pyc instead
	find $(prefix)/release-enigma2-pli/usr/lib/python2.6/ -name '*.pyc' -exec rm -f {} \;
	find $(prefix)/release-enigma2-pli/usr/lib/python2.6/ -name '*.a' -exec rm -f {} \;
	find $(prefix)/release-enigma2-pli/usr/lib/python2.6/ -name '*.o' -exec rm -f {} \;
	find $(prefix)/release-enigma2-pli/usr/lib/python2.6/ -name '*.la' -exec rm -f {} \;
	find $(prefix)/release-enigma2-pli/usr/lib/python2.6/ -name '*.so*' -exec sh4-linux-strip --strip-unneeded {} \;

######## FOR YOUR OWN CHANGES use these folder in cdk/own_build/enigma2 #############
	cp -RP $(buildprefix)/own_build/enigma2/* $(prefix)/release-enigma2-pli/

	cp $(kernelprefix)/linux-sh4/arch/sh/boot/uImage $(prefix)/release-enigma2-pli/boot/

	[ -e $(kernelprefix)/linux-sh4/fs/autofs4/autofs4.ko ] && cp $(kernelprefix)/linux-sh4/fs/autofs4/autofs4.ko $(prefix)/release-enigma2-pli/lib/modules || true
	[ -e $(kernelprefix)/linux-sh4/drivers/usb/serial/ftdi_sio.ko ] && cp $(kernelprefix)/linux-sh4/drivers/usb/serial/ftdi_sio.ko $(prefix)/release-enigma2-pli/lib/modules/ftdi.ko || true
	[ -e $(kernelprefix)/linux-sh4/drivers/usb/serial/pl2303.ko ] && cp $(kernelprefix)/linux-sh4/drivers/usb/serial/pl2303.ko $(prefix)/release-enigma2-pli/lib/modules || true
	[ -e $(kernelprefix)/linux-sh4/drivers/usb/serial/usbserial.ko ] && cp $(kernelprefix)/linux-sh4/drivers/usb/serial/usbserial.ko $(prefix)/release-enigma2-pli/lib/modules || true
	[ -e $(kernelprefix)/linux-sh4/fs/fuse/fuse.ko ] && cp $(kernelprefix)/linux-sh4/fs/fuse/fuse.ko $(prefix)/release-enigma2-pli/lib/modules || true
	[ -e $(kernelprefix)/linux-sh4/fs/ntfs/ntfs.ko ] && cp $(kernelprefix)/linux-sh4/fs/ntfs/ntfs.ko $(prefix)/release-enigma2-pli/lib/modules || true
	[ -e $(kernelprefix)/linux-sh4/fs/cifs/cifs.ko ] && cp $(kernelprefix)/linux-sh4/fs/cifs/cifs.ko $(prefix)/release-enigma2-pli/lib/modules || true
	[ -e $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/cpu_frequ/cpu_frequ.ko ] && cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/cpu_frequ/cpu_frequ.ko $(prefix)/release-enigma2-pli/lib/modules || true
	[ -e $(kernelprefix)/linux-sh4/fs/jfs/jfs.ko ] && cp $(kernelprefix)/linux-sh4/fs/jfs/jfs.ko $(prefix)/release-enigma2-pli/lib/modules || true
	[ -e $(kernelprefix)/linux-sh4/fs/nfsd/nfsd.ko ] && cp $(kernelprefix)/linux-sh4/fs/nfsd/nfsd.ko $(prefix)/release-enigma2-pli/lib/modules || true
	[ -e $(kernelprefix)/linux-sh4/fs/exportfs/exportfs.ko ] && cp $(kernelprefix)/linux-sh4/fs/exportfs/exportfs.ko $(prefix)/release-enigma2-pli/lib/modules || true
	[ -e $(kernelprefix)/linux-sh4/fs/nfs_common/nfs_acl.ko ] && cp $(kernelprefix)/linux-sh4/fs/nfs_common/nfs_acl.ko $(prefix)/release-enigma2-pli/lib/modules || true
	[ -e $(kernelprefix)/linux-sh4/fs/nfs/nfs.ko ] && cp $(kernelprefix)/linux-sh4/fs/nfs/nfs.ko $(prefix)/release-enigma2-pli/lib/modules || true

#
# AUTOFS
#
	if [ -d $(prefix)/release-enigma2-pli/usr/lib/autofs ]; then \
		cp -f $(targetprefix)/usr/sbin/automount $(prefix)/release-enigma2-pli/usr/sbin/; \
		ln -s /usr/lib/autofs/mount_ext2.so $(prefix)/release-enigma2-pli/usr/lib/autofs/mount_ext3.so; \
		if [ -e $(targetprefix)/lib/modules/$(KERNELVERSION)/kernel/fs/autofs4/autofs4.ko ]; then \
			cp $(targetprefix)/lib/modules/$(KERNELVERSION)/kernel/fs/autofs4/autofs4.ko $(prefix)/release-enigma2-pli/lib/modules; \
		fi; \
		cp -f $(buildprefix)/root/release-enigma2-pli/auto.usb $(prefix)/release-enigma2-pli/etc/; \
	fi

#
# GSTREAMER
#
	if [ -d $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10 ]; then \
		rm -rf $(prefix)/release-enigma2-pli/usr/lib/libgstfft*; \
		rm -rf $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/*; \
		cp -a $(targetprefix)/usr/bin/gst-launch* $(prefix)/release-enigma2-pli/usr/bin/; \
		sh4-linux-strip --strip-unneeded $(prefix)/release-enigma2-pli/usr/bin/gst-launch*; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstalsa.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstapp.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstasf.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstassrender.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstaudioconvert.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstaudioparsersbad.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstaudioresample.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstautodetect.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstavi.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstcdxaparse.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstcoreelements.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstcoreindexers.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstdecodebin.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstdecodebin2.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstdvbaudiosink.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstdvbvideosink.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstdvdsub.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstflac.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstflv.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstfragmented.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgsticydemux.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstid3demux.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstisomp4.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstmad.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstmatroska.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstmpegaudioparse.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstmpegdemux.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstmpegstream.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstogg.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstplaybin.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstrtmp.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstrtp.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstrtpmanager.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstrtsp.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstsouphttpsrc.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstsubparse.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgsttypefindfunctions.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstudp.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstvcdsrc.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstwavparse.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		if [ -e $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/libgstffmpeg.so ]; then \
			cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstffmpeg.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
			cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstffmpegscale.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
			cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstpostproc.so $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/; \
		fi; \
		sh4-linux-strip --strip-unneeded $(prefix)/release-enigma2-pli/usr/lib/gstreamer-0.10/*; \
	fi

#
# DIRECTFB
#
	if [ -d $(prefix)/release-enigma2-pli/usr/lib/directfb-1.4-5 ]; then \
		rm -rf $(prefix)/release-enigma2-pli/usr/lib/directfb-1.4-5/gfxdrivers/*.a; \
		rm -rf $(prefix)/release-enigma2-pli/usr/lib/directfb-1.4-5/gfxdrivers/*.la; \
		rm -rf $(prefix)/release-enigma2-pli/usr/lib/directfb-1.4-5/gfxdrivers/*.o; \
		rm -rf $(prefix)/release-enigma2-pli/usr/lib/directfb-1.4-5/inputdrivers/*; \
		cp -a $(targetprefix)/usr/lib/directfb-1.4-5/inputdrivers/libdirectfb_enigma2remote.so $(prefix)/release-enigma2-pli/usr/lib/directfb-1.4-5/inputdrivers/; \
		rm -rf $(prefix)/release-enigma2-pli/usr/lib/directfb-1.4-5/systems/*.a; \
		rm -rf $(prefix)/release-enigma2-pli/usr/lib/directfb-1.4-5/systems/*.la; \
		rm -rf $(prefix)/release-enigma2-pli/usr/lib/directfb-1.4-5/systems/*.o; \
		rm -rf $(prefix)/release-enigma2-pli/usr/lib/directfb-1.4-5/systems/libdirectfb_dummy.so; \
		rm -rf $(prefix)/release-enigma2-pli/usr/lib/directfb-1.4-5/systems/libdirectfb_fbdev.so; \
		rm -rf $(prefix)/release-enigma2-pli/usr/lib/directfb-1.4-5/wm/*.a; \
		rm -rf $(prefix)/release-enigma2-pli/usr/lib/directfb-1.4-5/wm/*.la; \
		rm -rf $(prefix)/release-enigma2-pli/usr/lib/directfb-1.4-5/wm/*.o; \
		rm -rf $(prefix)/release-enigma2-pli/usr/lib/directfb-1.4-5/interfaces/IDirectFBFont/*.a; \
		rm -rf $(prefix)/release-enigma2-pli/usr/lib/directfb-1.4-5/interfaces/IDirectFBFont/*.la; \
		rm -rf $(prefix)/release-enigma2-pli/usr/lib/directfb-1.4-5/interfaces/IDirectFBFont/*.o; \
		rm -rf $(prefix)/release-enigma2-pli/usr/lib/directfb-1.4-5/interfaces/IDirectFBImageProvider/*.a; \
		rm -rf $(prefix)/release-enigma2-pli/usr/lib/directfb-1.4-5/interfaces/IDirectFBImageProvider/*.la; \
		rm -rf $(prefix)/release-enigma2-pli/usr/lib/directfb-1.4-5/interfaces/IDirectFBImageProvider/*.o; \
		rm -rf $(prefix)/release-enigma2-pli/usr/lib/directfb-1.4-5/interfaces/IDirectFBVideoProvider/*.a; \
		rm -rf $(prefix)/release-enigma2-pli/usr/lib/directfb-1.4-5/interfaces/IDirectFBVideoProvider/*.la; \
		rm -rf $(prefix)/release-enigma2-pli/usr/lib/directfb-1.4-5/interfaces/IDirectFBVideoProvider/*.o; \
	fi
	if [ -d $(prefix)/release-enigma2-pli/usr/lib/icu ]; then \
		rm -rf $(prefix)/release-enigma2-pli/usr/lib/icu; \
	fi
	if [ -d $(prefix)/release-enigma2-pli/usr/lib/glib-2.0 ]; then \
		rm -rf $(prefix)/release-enigma2-pli/usr/lib/glib-2.0; \
	fi
	if [ -d $(prefix)/release-enigma2-pli/usr/lib/gio ]; then \
		rm -rf $(prefix)/release-enigma2-pli/usr/lib/gio; \
	fi
	if [ -d $(prefix)/release-enigma2-pli/usr/lib/enchant ]; then \
		rm -rf $(prefix)/release-enigma2-pli/usr/lib/enchant; \
	fi

