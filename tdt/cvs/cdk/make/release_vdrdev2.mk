#Trick ALPHA-Version ;)
$(DEPDIR)/min-release_vdrdev2 $(DEPDIR)/std-release_vdrdev2 $(DEPDIR)/max-release_vdrdev2 $(DEPDIR)/release_vdrdev2: \
$(DEPDIR)/%release_vdrdev2:
	rm -rf $(prefix)/release_vdrdev2 || true
	$(INSTALL_DIR) $(prefix)/release_vdrdev2 && \
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/bin && \
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/sbin && \
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/boot && \
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/dev && \
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/dev.static && \
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/etc && \
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/etc/fonts && \
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/etc/init.d && \
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/etc/network && \
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/etc/network/if-down.d && \
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/etc/network/if-post-down.d && \
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/etc/network/if-pre-up.d && \
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/etc/network/if-up.d && \
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/etc/tuxbox && \
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/hdd && \
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/hdd/movie && \
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/hdd/music && \
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/hdd/picture && \
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/lib && \
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/lib/modules && \
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/ram && \
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/var && \
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/var/etc && \
	export CROSS_COMPILE=$(target)- && \
		$(MAKE) install -C @DIR_busybox@ CONFIG_PREFIX=$(prefix)/release_vdrdev2 && \
	cp -a $(targetprefix)/bin/* $(prefix)/release_vdrdev2/bin/ && \
	ln -s /bin/showiframe $(prefix)/release_vdrdev2/usr/bin/showiframe && \
	cp -dp $(targetprefix)/bin/hotplug $(prefix)/release_vdrdev2/sbin/ && \
	cp -dp $(targetprefix)/sbin/init $(prefix)/release_vdrdev2/sbin/ && \
	cp -dp $(targetprefix)/sbin/killall5 $(prefix)/release_vdrdev2/sbin/ && \
	cp -dp $(targetprefix)/sbin/portmap $(prefix)/release_vdrdev2/sbin/ && \
	cp -dp $(targetprefix)/sbin/mke2fs $(prefix)/release_vdrdev2/sbin/ && \
	cp -dp $(targetprefix)/sbin/mkfs.ext2 $(prefix)/release_vdrdev2/sbin/ && \
	cp -dp $(targetprefix)/sbin/mkfs.ext3 $(prefix)/release_vdrdev2/sbin/ && \
	cp -dp $(targetprefix)/sbin/e2fsck $(prefix)/release_vdrdev2/sbin/ && \
	cp -dp $(targetprefix)/sbin/fsck $(prefix)/release_vdrdev2/sbin/ && \
	cp -dp $(targetprefix)/sbin/fsck.ext2 $(prefix)/release_vdrdev2/sbin/ && \
	cp -dp $(targetprefix)/sbin/fsck.ext3 $(prefix)/release_vdrdev2/sbin/ && \
	cp -dp $(targetprefix)/sbin/fsck.nfs $(prefix)/release_vdrdev2/sbin/ && \
	cp -dp $(targetprefix)/sbin/sfdisk $(prefix)/release_vdrdev2/sbin/ && \
	cp -dp $(targetprefix)/sbin/tune2fs $(prefix)/release_vdrdev2/sbin/ && \
	cp -dp $(targetprefix)/sbin/blkid $(prefix)/release_vdrdev2/sbin/ && \
	cp -dp $(targetprefix)/usr/bin/rdate $(prefix)/release_vdrdev2/sbin/ && \
	cp -dp $(targetprefix)/etc/init.d/portmap $(prefix)/release_vdrdev2/etc/init.d/ && \
	cp -dp $(buildprefix)/root/etc/init.d/udhcpc $(prefix)/release_vdrdev2/etc/init.d/ && \
	cp -a $(targetprefix)/dev/* $(prefix)/release_vdrdev2/dev/ && \
	cp -dp $(targetprefix)/etc/fstab $(prefix)/release_vdrdev2/etc/ && \
	cp -dp $(targetprefix)/etc/group $(prefix)/release_vdrdev2/etc/ && \
	cp -dp $(targetprefix)/etc/host.conf $(prefix)/release_vdrdev2/etc/ && \
	cp -dp $(targetprefix)/etc/hostname $(prefix)/release_vdrdev2/etc/ && \
	cp -dp $(targetprefix)/etc/hosts $(prefix)/release_vdrdev2/etc/ && \
	cp -dp $(targetprefix)/etc/inittab $(prefix)/release_vdrdev2/etc/ && \
	cp -dp $(buildprefix)/root/etc/lircd.conf* $(prefix)/release_vdrdev2/etc/ && \
	cp -dp $(targetprefix)/etc/mtab $(prefix)/release_vdrdev2/etc/ && \
	cp -dp $(targetprefix)/etc/passwd $(prefix)/release_vdrdev2/etc/ && \
	cp -dp $(targetprefix)/etc/profile $(prefix)/release_vdrdev2/etc/ && \
	cp -dp $(targetprefix)/etc/protocols $(prefix)/release_vdrdev2/etc/ && \
	cp -dp $(targetprefix)/etc/resolv.conf $(prefix)/release_vdrdev2/etc/ && \
	cp -dp $(targetprefix)/etc/services $(prefix)/release_vdrdev2/etc/ && \
	cp -dp $(targetprefix)/etc/shells $(prefix)/release_vdrdev2/etc/ && \
	cp -dp $(targetprefix)/etc/shells.conf $(prefix)/release_vdrdev2/etc/ && \
	cp -dp $(buildprefix)/root/etc/timezone.xml $(prefix)/release_vdrdev2/etc/ && \
	echo "127.0.0.1       localhost.localdomain   localhost" > $(prefix)/release_vdrdev2/etc/hosts && \
	touch $(prefix)/release_vdrdev2/etc/nsswitch.conf && \
	echo "hosts: files dns" > $(prefix)/release_vdrdev2/etc/nsswitch.conf && \
	cp -dp $(targetprefix)/etc/vsftpd.conf $(prefix)/release_vdrdev2/etc/ && \
	sed "s@listen=YES@listen=NO@g" -i $(prefix)/release_vdrdev2/etc/vsftpd.conf && \
	echo "8001 stream tcp nowait root /bin/streamproxy streamproxy" > $(prefix)/release_vdrdev2/etc/inetd.conf && \
	echo "ftp stream tcp nowait root /usr/bin/vsftpd vsftpd" >> $(prefix)/release_vdrdev2/etc/inetd.conf && \
	echo "telnet stream tcp nowait root /usr/sbin/telnetd telnetd -i -l /bin/login" >> $(prefix)/release_vdrdev2/etc/inetd.conf && \
	echo "ssh stream tcp nowait root /bin/dropbear dropbear -i" >> $(prefix)/release_vdrdev2/etc/inetd.conf && \
	cp -dp $(targetprefix)/etc/vdstandby.cfg $(prefix)/release_vdrdev2/etc/ && \
	cp -dp $(targetprefix)/etc/network/interfaces $(prefix)/release_vdrdev2/etc/network/ && \
	cp -dp $(targetprefix)/etc/network/options $(prefix)/release_vdrdev2/etc/network/ && \
	cp -dp $(targetprefix)/etc/init.d/umountfs $(prefix)/release_vdrdev2/etc/init.d/ && \
	cp -dp $(targetprefix)/etc/init.d/sendsigs $(prefix)/release_vdrdev2/etc/init.d/ && \
	cp -dp $(targetprefix)/etc/init.d/halt $(prefix)/release_vdrdev2/etc/init.d/ && \
	cp $(buildprefix)/root/release/reboot $(prefix)/release_vdrdev2/etc/init.d/ && \
	echo "576i50" > $(prefix)/release_vdrdev2/etc/videomode && \
	cp $(buildprefix)/root/release/rcS_stm23_24_neutrino_spark $(prefix)/release_vdrdev2/etc/init.d/rcS && \
	chmod 755 $(prefix)/release_vdrdev2/etc/init.d/rcS && \
	mkdir -p $(prefix)/release_vdrdev2/usr/local/share/vdr && \
	mkdir -p $(prefix)/release_vdrdev2/usr/local/share/vdr/plugins && \
	mkdir -p $(prefix)/release_vdrdev2/usr/local/share/vdr/themes && \
	mkdir -p $(prefix)/release_vdrdev2/usr/local/bin && \
	mkdir -p $(prefix)/release_vdrdev2/usr/lib/locale && \
	cp $(buildprefix)/root/var/vdr/plugins_vdrdev2.load $(prefix)/release_vdrdev2/usr/local/share/vdr/plugins.load && \
	cp $(buildprefix)/root/var/vdr/channels.conf $(prefix)/release_vdrdev2/usr/local/share/vdr/ && \
	cp $(buildprefix)/root/var/vdr/diseqc.conf $(prefix)/release_vdrdev2/usr/local/share/vdr/ && \
	cp $(buildprefix)/root/var/vdr/keymacros.conf $(prefix)/release_vdrdev2/usr/local/share/vdr/ && \
	cp $(buildprefix)/root/var/vdr/remote.conf $(prefix)/release_vdrdev2/usr/local/share/vdr/ && \
	cp $(buildprefix)/root/var/vdr/setup.conf $(prefix)/release_vdrdev2/usr/local/share/vdr/ && \
	cp $(buildprefix)/root/var/vdr/sources.conf $(prefix)/release_vdrdev2/usr/local/share/vdr/ && \
	cp $(buildprefix)/root/var/vdr/plugins/mplayersources.conf $(prefix)/release_vdrdev2/usr/local/share/vdr/plugins && \
	cp $(buildprefix)/root/usr/local/bin/mplayer.sh $(prefix)/release_vdrdev2/usr/local/bin/ && \
	chmod 755 $(prefix)/release_vdrdev2/usr/local/bin/mplayer.sh && \
	cp -rd $(buildprefix)/root/var/vdr/themes/* $(prefix)/release_vdrdev2/usr/local/share/vdr/themes/ && \
	cp $(buildprefix)/root/usr/local/bin/runvdr $(prefix)/release_vdrdev2/usr/local/bin/ && \
	chmod 755 $(prefix)/release_vdrdev2/usr/local/bin/runvdr && \
	cp $(buildprefix)/root/usr/local/bin/vdrshutdown $(prefix)/release_vdrdev2/usr/local/bin/ && \
	cp $(buildprefix)/root/release/mountvirtfs $(prefix)/release_vdrdev2/etc/init.d/ && \
	cp $(buildprefix)/root/release/mme_check $(prefix)/release_vdrdev2/etc/init.d/ && \
	cp $(buildprefix)/root/release/mountall $(prefix)/release_vdrdev2/etc/init.d/ && \
	cp $(buildprefix)/root/release/hostname $(prefix)/release_vdrdev2/etc/init.d/ && \
	cp $(buildprefix)/root/release/vsftpd $(prefix)/release_vdrdev2/etc/init.d/ && \
	cp $(buildprefix)/root/release/bootclean.sh $(prefix)/release_vdrdev2/etc/init.d/ && \
	cp $(buildprefix)/root/release/networking $(prefix)/release_vdrdev2/etc/init.d/ && \
	cp $(buildprefix)/root/bin/autologin $(prefix)/release_vdrdev2/bin/ && \
	cp -rd $(buildprefix)/root/usr/lib/locale/* $(prefix)/release_vdrdev2/usr/lib/locale/ && \
	cp -rd $(targetprefix)/lib/* $(prefix)/release_vdrdev2/lib/ && \
	rm -f $(prefix)/release_vdrdev2/lib/*.a && \
	rm -f $(prefix)/release_vdrdev2/lib/*.o && \
	rm -f $(prefix)/release_vdrdev2/lib/*.la && \
	find $(prefix)/release_vdrdev2/lib/ -name  *.so* -exec sh4-linux-strip --strip-unneeded {} \;
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/avs/avs.ko $(prefix)/release_vdrdev2/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/boxtype/boxtype.ko $(prefix)/release_vdrdev2/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/simu_button/simu_button.ko $(prefix)/release_vdrdev2/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/e2_proc/e2_proc.ko $(prefix)/release_vdrdev2/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/frontcontroller/aotom/aotom.ko $(prefix)/release_vdrdev2/lib/modules/
	mkdir -p $(prefix)/release_vdrdev2/usr/local/share/vdr
	mkdir -p $(prefix)/release_vdrdev2/etc/config
	cp -RP $(targetprefix)/etc/vdr/* $(prefix)/release_vdrdev2/usr/local/share/vdr/
	cp -RP $(buildprefix)/root/usr/lib/libuci* $(prefix)/release_vdrdev2/usr/lib/
	cp -RP $(buildprefix)/root/usr/sbin/uci $(prefix)/release_vdrdev2/sbin/
	cp -RP $(buildprefix)/root/usr/bin/setup-* $(prefix)/release_vdrdev2/usr/bin/
	cp -RP $(buildprefix)/root/usr/bin/sync-* $(prefix)/release_vdrdev2/usr/bin/
	cp -RP $(buildprefix)/root/usr/lib/gconv $(prefix)/release_vdrdev2/usr/lib/
	cp -RP $(buildprefix)/root/usr/share/fonts/seg.ttf $(prefix)/release_vdrdev2/etc/fonts/vdr.ttf
	cp -RP $(targetprefix)/etc/fonts/fonts.conf $(prefix)/release_vdrdev2/etc/fonts/
	cp -RP $(buildprefix)/root/sbin/ntpdate $(prefix)/release_vdrdev2/sbin/
	cp -RP $(buildprefix)/root/etc/init.d/ntpupdate.sh $(prefix)/release_vdrdev2/etc/init.d/
	touch $(prefix)/release_vdrdev2/etc/changelog.txt
	rm -rf $(prefix)/release_vdrdev2/etc/vdr
	echo "EVO_VDR2" > $(prefix)/release_vdrdev2/etc/hostname
	rm -f $(prefix)/release_vdrdev2/sbin/halt
	cp -f $(targetprefix)/sbin/halt $(prefix)/release_vdrdev2/sbin/
	cp $(buildprefix)/root/release/umountfs $(prefix)/release_vdrdev2/etc/init.d/
	cp $(buildprefix)/root/release/rc $(prefix)/release_vdrdev2/etc/init.d/
	cp $(buildprefix)/root/release/sendsigs $(prefix)/release_vdrdev2/etc/init.d/
	cp $(buildprefix)/root/release/halt_spark $(prefix)/release_vdrdev2/etc/init.d/halt
	chmod 755 $(prefix)/release_vdrdev2/etc/init.d/umountfs
	chmod 755 $(prefix)/release_vdrdev2/etc/init.d/rc
	chmod 755 $(prefix)/release_vdrdev2/etc/init.d/sendsigs
	chmod 755 $(prefix)/release_vdrdev2/etc/init.d/halt
	mkdir -p $(prefix)/release_vdrdev2/etc/rc.d/rc0.d
	ln -s ../init.d $(prefix)/release_vdrdev2/etc/rc.d
	ln -fs halt $(prefix)/release_vdrdev2/sbin/reboot
	ln -fs halt $(prefix)/release_vdrdev2/sbin/poweroff
	ln -s ../init.d/sendsigs $(prefix)/release_vdrdev2/etc/rc.d/rc0.d/S20sendsigs
	ln -s ../init.d/umountfs $(prefix)/release_vdrdev2/etc/rc.d/rc0.d/S40umountfs
	ln -s ../init.d/halt $(prefix)/release_vdrdev2/etc/rc.d/rc0.d/S90halt
	mkdir -p $(prefix)/release_vdrdev2/etc/rc.d/rc6.d
	ln -s ../init.d/sendsigs $(prefix)/release_vdrdev2/etc/rc.d/rc6.d/S20sendsigs
	ln -s ../init.d/umountfs $(prefix)/release_vdrdev2/etc/rc.d/rc6.d/S40umountfs
	ln -s ../init.d/reboot $(prefix)/release_vdrdev2/etc/rc.d/rc6.d/S90reboot
	cp -f $(targetprefix)/usr/sbin/automount $(prefix)/release_vdrdev2/usr/sbin/
	cp -f $(buildprefix)/root/release/auto.usb $(prefix)/release_vdrdev2/etc/
if ENABLE_SPARK
	mv $(prefix)/release_vdrdev2/lib/firmware/component_7111_mb618.fw $(prefix)/release_vdrdev2/lib/firmware/component.fw
	rm $(prefix)/release_vdrdev2/lib/firmware/component_7105_pdk7105.fw
else
	mv $(prefix)/release_vdrdev2/lib/firmware/component_7105_pdk7105.fw $(prefix)/release_vdrdev2/lib/firmware/component.fw
	rm $(prefix)/release_vdrdev2/lib/firmware/component_7111_mb618.fw
endif
	rm -f $(prefix)/release_vdrdev2/lib/firmware/dvb-fe-avl2108.fw
	rm -f $(prefix)/release_vdrdev2/lib/firmware/dvb-fe-stv6306.fw
	rm -f $(prefix)/release_vdrdev2/lib/firmware/dvb-fe-cx24116.fw
	rm -f $(prefix)/release_vdrdev2/lib/firmware/dvb-fe-cx21143.fw
	rm -f $(prefix)/release_vdrdev2/bin/evremote
	rm -f $(prefix)/release_vdrdev2/bin/gotosleep
if ENABLE_SPARK
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stmcore-display-sti7111.ko $(prefix)/release_vdrdev2/lib/modules/
else
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stmcore-display-sti7105.ko $(prefix)/release_vdrdev2/lib/modules/
endif
	rm -f $(prefix)/release_vdrdev2/lib/firmware/dvb-fe-cx21143.fw
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stmfb.ko $(prefix)/release_vdrdev2/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/multicom/embxshell/embxshell.ko $(prefix)/release_vdrdev2/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/multicom/embxmailbox/embxmailbox.ko $(prefix)/release_vdrdev2/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/multicom/embxshm/embxshm.ko $(prefix)/release_vdrdev2/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/multicom/mme/mme_host.ko $(prefix)/release_vdrdev2/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/bpamem/bpamem.ko $(prefix)/release_vdrdev2/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/compcache/lzo-kmod/lzo1x_compress.ko $(prefix)/release_vdrdev2/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/compcache/lzo-kmod/lzo1x_decompress.ko $(prefix)/release_vdrdev2/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/compcache/ramzswap.ko $(prefix)/release_vdrdev2/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stm_v4l2.ko $(prefix)/release_vdrdev2/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stmvbi.ko $(prefix)/release_vdrdev2/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stmvout.ko $(prefix)/release_vdrdev2/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/pti/pti.ko $(prefix)/release_vdrdev2/lib/modules/
	find $(prefix)/release_vdrdev2/lib/modules/ -name '*.ko' -exec sh4-linux-strip --strip-unneeded {} \;
	cd $(targetprefix)/lib/modules/$(KERNELVERSION)/extra && \
	for mod in \
		sound/pseudocard/pseudocard.ko \
		sound/silencegen/silencegen.ko \
		stm/mmelog/mmelog.ko \
		stm/monitor/stm_monitor.ko \
		media/dvb/stm/dvb/stmdvb.ko \
		sound/ksound/ksound.ko \
		media/dvb/stm/mpeg2_hard_host_transformer/mpeg2hw.ko \
		media/dvb/stm/backend/player2.ko \
		media/dvb/stm/h264_preprocessor/sth264pp.ko \
		media/dvb/stm/allocator/stmalloc.ko \
		stm/platform/platform.ko \
		stm/platform/p2div64.ko \
		media/sysfs/stm/stmsysfs.ko \
	;do \
		if [ -e player2/linux/drivers/$$mod ] ; then \
			cp player2/linux/drivers/$$mod $(prefix)/release_vdrdev2/lib/modules/; \
			sh4-linux-strip --strip-unneeded $(prefix)/release_vdrdev2/lib/modules/`basename $$mod`; \
		else \
			touch $(prefix)/release_vdrdev2/lib/modules/`basename $$mod`; \
		fi;\
	done
	cp -RP $(prefix)/cdkroot/boot/uImage $(prefix)/
if ENABLE_SPARK
	if [ -d $(prefix)/release_vdrdev2/boot/spark7162 ]; then \
		rm -rf $(prefix)/release_vdrdev2/boot/spark7162; \
	fi
else
	if [ -d $(prefix)/release_vdrdev2/boot/spark7162 ]; then \
		rm $(prefix)/release_vdrdev2/boot/*.elf; \
		cp $(prefix)/release_vdrdev2/boot/spark7162/*.elf $(prefix)/release_vdrdev2/boot/; \
		rm -rf $(prefix)/release_vdrdev2/boot/spark7162; \
	fi
	touch $(prefix)/release_vdrdev2/etc/.spark7162
endif
	find $(prefix)/release_vdrdev2/lib/ -name '*.so*' -exec sh4-linux-strip --strip-unneeded {} \;
	rm -rf $(prefix)/release_vdrdev2/lib/autofs
	rm -rf $(prefix)/release_vdrdev2/lib/modules/$(KERNELVERSION)
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/media
	ln -s /hdd $(prefix)/release_vdrdev2/media/hdd
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/media/dvd
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/mnt
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/mnt/usb
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/mnt/hdd
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/mnt/nfs
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/root
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/proc
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/sys
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/tmp
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/usr
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/usr/bin
	cp -p $(targetprefix)/usr/sbin/vsftpd $(prefix)/release_vdrdev2/usr/bin/
	cp -p $(targetprefix)/usr/bin/lircd $(prefix)/release_vdrdev2/usr/bin/
	cp -p $(targetprefix)/usr/bin/killall $(prefix)/release_vdrdev2/usr/bin/
	cp -p $(targetprefix)/usr/sbin/ethtool $(prefix)/release_vdrdev2/usr/sbin/
	$(INSTALL_DIR) $(prefix)/release_vdrdev2/usr/tuxtxt
#######################################################################################
#######################################################################################
#######################################################################################
#######################################################################################

	$(INSTALL_DIR) $(prefix)/release_vdrdev2/usr/share

#######################################################################################


#######################################################################################

#######################################################################################

	$(INSTALL_DIR) $(prefix)/release_vdrdev2/usr/share/zoneinfo
	cp -aR $(buildprefix)/root/usr/share/zoneinfo/* $(prefix)/release_vdrdev2/usr/share/zoneinfo/

#######################################################################################

	$(INSTALL_DIR) $(prefix)/release_vdrdev2/usr/share/udhcpc
	cp -aR $(buildprefix)/root/usr/share/udhcpc/* $(prefix)/release_vdrdev2/usr/share/udhcpc/


#######################################################################################
#######################################################################################
#######################################################################################
#######################################################################################

	$(INSTALL_DIR) $(prefix)/release_vdrdev2/usr/local

#######################################################################################

	$(INSTALL_DIR) $(prefix)/release_vdrdev2/usr/local/bin
	cp -rd $(targetprefix)/usr/local/bin/vdr $(prefix)/release_vdrdev2/usr/local/bin/
	find $(prefix)/release_vdrdev2/usr/local/bin/ -name  vdr -exec sh4-linux-strip --strip-unneeded {} \;
	cp -RP $(buildprefix)/root/bin/dropbear $(prefix)/release_vdrdev2/bin/
	cp -RP $(buildprefix)/root/bin/dropbearkey $(prefix)/release_vdrdev2/bin/
	mkdir -p $(prefix)/release_vdrdev2/etc/dropbear
#######################################################################################

#	$(INSTALL_DIR) $(prefix)/release_vdrdev2/usr/local/share
#	cp -aR $(targetprefix)/usr/local/share/iso-codes $(prefix)/release_vdrdev2/usr/local/share/
#	TODO: Channellist ....
#	$(INSTALL_DIR) $(prefix)/release_vdrdev2/usr/local/share/config
#	cp -aR $(buildprefix)/root/usr/local/share/config/* $(prefix)/release_vdrdev2/usr/local/share/config/
#	cp -aR $(targetprefix)/usr/local/share/vdr $(prefix)/release_vdrdev2/usr/local/share/
#	TODO: HACK
#	cp -aR $(targetprefix)/$(targetprefix)/usr/local/share/vdr/* $(prefix)/release_vdrdev2/usr/local/share/vdr
#	cp -aR $(targetprefix)/usr/local/share/fonts $(prefix)/release_vdrdev2/usr/local/share/
#	ln -s /usr/local/share/fonts/micron.ttf $(prefix)/release_vdrdev2/usr/local/share/fonts/vdr.ttf
	cp -a $(buildprefix)/root/usr/bin/amixer $(prefix)/release_vdrdev2/usr/bin/
	find $(prefix)/release_vdrdev2/usr/bin/ -name 'amixer' -exec sh4-linux-strip --strip-unneeded {} \;

#######################################################################################
	echo "duckbox-rev#: " > $(prefix)/release_vdrdev2/etc/imageinfo
	git describe --always >> $(prefix)/release_vdrdev2/etc/imageinfo
#######################################################################################

	$(INSTALL_DIR) $(prefix)/release_vdrdev2/usr/lib

	mkdir -p $(prefix)/release_vdrdev2/usr/local/lib
	cp -R $(targetprefix)/usr/lib/* $(prefix)/release_vdrdev2/usr/lib/
	cp -rd $(targetprefix)/usr/lib/libfontconfi* $(prefix)/release_vdrdev2/usr/lib/
	if [ -e $(targetprefix)/usr/share/alsa ]; then \
		cp -RP $(targetprefix)/usr/share/alsa $(prefix)/release_vdrdev2/usr/share/; \
	fi
	mkdir -p $(prefix)/release_vdrdev2/usr/lib/vdr/
	cp -rd $(targetprefix)/usr/lib/vdr/lib*.1.7.2* $(prefix)/release_vdrdev2/usr/lib/vdr/
	cp -rd $(targetprefix)/usr/lib/vdr/lib*.1.7.2* $(prefix)/release_vdrdev2/usr/lib/vdr/

#	rm -rf $(prefix)/release_vdrdev2/usr/lib/alsa-lib
#	rm -rf $(prefix)/release_vdrdev2/usr/lib/alsaplayer
	rm -rf $(prefix)/release_vdrdev2/usr/lib/engines
	rm -rf $(prefix)/release_vdrdev2/usr/lib/enigma2

	rm -rf $(prefix)/release_vdrdev2/usr/lib/gconv/*
	cp -rd $(targetprefix)/usr/lib/gconv/gconv-modules $(prefix)/release_vdrdev2/usr/lib/gconv/
	cp -rd $(targetprefix)/usr/lib/gconv/ISO8859-9.so $(prefix)/release_vdrdev2/usr/lib/gconv/
	cp -rd $(targetprefix)/usr/lib/gconv/ISO8859-15.so $(prefix)/release_vdrdev2/usr/lib/gconv/
	cp -rd $(targetprefix)/usr/lib/gconv/UTF-32.so $(prefix)/release_vdrdev2/usr/lib/gconv/
	mkdir -p $(prefix)/release_vdrdev2/usr/script
	mkdir -p $(prefix)/release_vdrdev2/usr/keys
	( cd $(prefix)/release_vdrdev2/var && ln -s /usr/keys keys )
	rm -rf $(prefix)/release_vdrdev2/usr/lib/libxslt-plugins
	rm -rf $(prefix)/release_vdrdev2/usr/lib/pkgconfig
	rm -rf $(prefix)/release_vdrdev2/usr/lib/sigc++-1.2
	rm -rf $(prefix)/release_vdrdev2/usr/lib/X11
	rm -rf $(prefix)/release_vdrdev2/usr/lib/tuxbox
#	rm -f $(prefix)/release_vdrdev2/usr/lib/*.a
#	rm -f $(prefix)/release_vdrdev2/usr/lib/*.o
#	rm -f $(prefix)/release_vdrdev2/usr/lib/*.la
	chmod 755 -R $(prefix)/release_vdrdev2/usr/lib/
	find $(prefix)/release_vdrdev2/usr/lib/ -name  *.a -exec rm {} \;
	find $(prefix)/release_vdrdev2/usr/lib/ -name  *.o -exec rm {} \;
	find $(prefix)/release_vdrdev2/usr/lib/ -name  *.la -exec rm {} \;
	find $(prefix)/release_vdrdev2/usr/lib/ -name  *.so* -exec sh4-linux-strip --strip-unneeded {} \;

######## FOR YOUR OWN CHANGES use these folder in cdk/own_build/vdr #############
	cp -RP $(buildprefix)/own_build/vdr/* $(prefix)/release_vdrdev2/
	ln -sf /usr/share/zoneinfo/CET $(prefix)/release_vdrdev2/etc/localtime
#######################################################################################
#######################################################################################

	$(INSTALL_DIR) $(prefix)/release_vdrdev2/usr/include/boost

#	mkdir -p $(prefix)/release_vdrdev2/usr/include/boost/
#	cp -rd $(targetprefix)/usr/include/boost/shared_container_iterator.hpp $(prefix)/release_vdrdev2/usr/include/boost/

#######################################################################################
#######################################################################################

	$(INSTALL_DIR) $(prefix)/release_vdrdev2/usr/share/locale

#	mkdir -p $(prefix)/release_vdrdev2/usr/share/locale/
	cp -rd $(targetprefix)/usr/share/locale/* $(prefix)/release_vdrdev2/usr/share/locale

#	cp -rd $(targetprefix)/usr/local/share/locale/* $(prefix)/release_vdrdev2/usr/local/share/locale
	mkdir -p $(prefix)/release_vdrdev2/usr/local/share/vdr/locale
	cp -RP $(targetprefix)/usr/local/share/vdr/locale/de_DE $(prefix)/release_vdrdev2/usr/local/share/vdr/locale/
	( cd $(prefix)/release_vdrdev2/usr/local/share && ln -sf ./vdr/locale locale )
#######################################################################################
#######################################################################################

	$(INSTALL_DIR) $(prefix)/release_vdrdev2/usr/share/locale

	( cd $(prefix)/release_vdrdev2/var && ln -sf /usr/local/share/vdr vdr )
#	cp -rd $(targetprefix)/var/vdr/remote.conf $(prefix)/release_vdrdev2/var/vdr/
#	cp -rd $(targetprefix)/var/vdr/sources.conf $(prefix)/release_vdrdev2/var/vdr/
#	cp -rd $(targetprefix)/var/vdr/channels.conf $(prefix)/release_vdrdev2/var/vdr/
#	cp -rd $(targetprefix)/var/vdr $(prefix)/release_vdrdev2/var/vdr/
	cp -RP $(buildprefix)/root/usr/local/share/vdr $(prefix)/release_vdrdev2/usr/local/share/
	( cd $(prefix)/release_vdrdev2/etc && ln -sf /usr/local/share/vdr vdr )
#######################################################################################
#######################################################################################
#######################################################################################
#######################################################################################

	[ -e $(kernelprefix)/linux-sh4/fs/autofs4/autofs4.ko ] && cp $(kernelprefix)/linux-sh4/fs/autofs4/autofs4.ko $(prefix)/release_vdrdev2/lib/modules || true
	[ -e $(kernelprefix)/linux-sh4/drivers/usb/serial/ftdi_sio.ko ] && cp $(kernelprefix)/linux-sh4/drivers/usb/serial/ftdi_sio.ko $(prefix)/release_vdrdev2/lib/modules/ftdi.ko || true
	[ -e $(kernelprefix)/linux-sh4/drivers/usb/serial/pl2303.ko ] && cp $(kernelprefix)/linux-sh4/drivers/usb/serial/pl2303.ko $(prefix)/release_vdrdev2/lib/modules || true
	[ -e $(kernelprefix)/linux-sh4/drivers/usb/serial/usbserial.ko ] && cp $(kernelprefix)/linux-sh4/drivers/usb/serial/usbserial.ko $(prefix)/release_vdrdev2/lib/modules || true
	[ -e $(kernelprefix)/linux-sh4/fs/ntfs/ntfs.ko ] && cp $(kernelprefix)/linux-sh4/fs/ntfs/ntfs.ko $(prefix)/release_vdrdev2/lib/modules || true
	[ -e $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/cpu_frequ/cpu_frequ.ko ] && cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/cpu_frequ/cpu_frequ.ko $(prefix)/release_vdrdev2/lib/modules || true

	touch $@
