if STM22
kernelpath="linux-sh4"
else
kernelpath="linux-sh4"
endif
#Trick ALPHA-Version ;)
$(DEPDIR)/min-release_neutrino $(DEPDIR)/std-release_neutrino $(DEPDIR)/max-release_neutrino $(DEPDIR)/ipk-release_neutrino $(DEPDIR)/release_neutrino: \
$(DEPDIR)/%release_neutrino:
	rm -rf $(prefix)/release_neutrino || true
	$(INSTALL_DIR) $(prefix)/release_neutrino && \
	$(INSTALL_DIR) $(prefix)/release_neutrino/bin && \
	$(INSTALL_DIR) $(prefix)/release_neutrino/sbin && \
	$(INSTALL_DIR) $(prefix)/release_neutrino/boot && \
	$(INSTALL_DIR) $(prefix)/release_neutrino/dev && \
	$(INSTALL_DIR) $(prefix)/release_neutrino/dev.static && \
	$(INSTALL_DIR) $(prefix)/release_neutrino/etc && \
	$(INSTALL_DIR) $(prefix)/release_neutrino/etc/fonts && \
	$(INSTALL_DIR) $(prefix)/release_neutrino/etc/init.d && \
	$(INSTALL_DIR) $(prefix)/release_neutrino/etc/network && \
	$(INSTALL_DIR) $(prefix)/release_neutrino/etc/network/if-down.d && \
	$(INSTALL_DIR) $(prefix)/release_neutrino/etc/network/if-post-down.d && \
	$(INSTALL_DIR) $(prefix)/release_neutrino/etc/network/if-pre-up.d && \
	$(INSTALL_DIR) $(prefix)/release_neutrino/etc/network/if-up.d && \
	$(INSTALL_DIR) $(prefix)/release_neutrino/etc/tuxbox && \
	$(INSTALL_DIR) $(prefix)/release_neutrino/media/hdd && \
	$(INSTALL_DIR) $(prefix)/release_neutrino/media/hdd/movie && \
	$(INSTALL_DIR) $(prefix)/release_neutrino/media/hdd/music && \
	$(INSTALL_DIR) $(prefix)/release_neutrino/media/hdd/picture && \
	$(INSTALL_DIR) $(prefix)/release_neutrino/lib && \
	$(INSTALL_DIR) $(prefix)/release_neutrino/lib/modules && \
	$(INSTALL_DIR) $(prefix)/release_neutrino/ram && \
	$(INSTALL_DIR) $(prefix)/release_neutrino/var && \
	$(INSTALL_DIR) $(prefix)/release_neutrino/var/etc && \
	export CROSS_COMPILE=$(target)- && \
		$(MAKE) install -C @DIR_busybox@ CONFIG_PREFIX=$(prefix)/release_neutrino && \
	cp -a $(targetprefix)/bin/* $(prefix)/release_neutrino/bin/ && \
	ln -s /bin/showiframe $(prefix)/release_neutrino/usr/bin/showiframe && \
	cp -dp $(targetprefix)/bin/hotplug $(prefix)/release_neutrino/sbin/ && \
	cp -dp $(targetprefix)/sbin/init $(prefix)/release_neutrino/sbin/ && \
	cp -dp $(targetprefix)/sbin/killall5 $(prefix)/release_neutrino/sbin/ && \
	cp -dp $(targetprefix)/sbin/portmap $(prefix)/release_neutrino/sbin/ && \
	cp -dp $(targetprefix)/sbin/mke2fs $(prefix)/release_neutrino/sbin/ && \
	cp -dp $(targetprefix)/sbin/mkfs.ext2 $(prefix)/release_neutrino/sbin/ && \
	cp -dp $(targetprefix)/sbin/mkfs.ext3 $(prefix)/release_neutrino/sbin/ && \
	cp -dp $(targetprefix)/sbin/fsck $(prefix)/release_neutrino/sbin/ && \
	cp -dp $(targetprefix)/sbin/fsck.ext2 $(prefix)/release_neutrino/sbin/ && \
	cp -dp $(targetprefix)/sbin/fsck.ext3 $(prefix)/release_neutrino/sbin/ && \
	cp -dp $(targetprefix)/sbin/fsck.nfs $(prefix)/release_neutrino/sbin/ && \
	cp -dp $(targetprefix)/sbin/sfdisk $(prefix)/release_neutrino/sbin/ && \
	cp -dp $(targetprefix)/sbin/tune2fs $(prefix)/release_neutrino/sbin/ && \
	cp -dp $(targetprefix)/sbin/blkid $(prefix)/release_neutrino/sbin/ && \
	cp -dp $(targetprefix)/usr/bin/rdate $(prefix)/release_neutrino/sbin/ && \
	cp -dp $(targetprefix)/etc/init.d/portmap $(prefix)/release_neutrino/etc/init.d/ && \
	cp -dp $(buildprefix)/root/etc/init.d/udhcpc $(prefix)/release_neutrino/etc/init.d/ && \
	cp -dp $(targetprefix)/usr/bin/ffmpeg $(prefix)/release_neutrino/sbin/ && \
	\
	cp $(targetprefix)/boot/video_7100.elf $(prefix)/release_neutrino/boot/video.elf && \
	cp $(targetprefix)/boot/video_7111.elf $(prefix)/release_neutrino/boot/video.elf && \
	\
	cp $(targetprefix)/boot/audio.elf $(prefix)/release_neutrino/boot/audio.elf && \
	cp $(targetprefix)/boot/audio_7111.elf $(prefix)/release_neutrino/boot/audio.elf && \
	\
	cp -a $(targetprefix)/dev/* $(prefix)/release_neutrino/dev/ && \
	cp -f $(buildprefix)/root/release/fstab_spark $(prefix)/release_neutrino/etc/fstab && \
	cp -dp $(targetprefix)/etc/group $(prefix)/release_neutrino/etc/ && \
	cp -dp $(targetprefix)/etc/host.conf $(prefix)/release_neutrino/etc/ && \
	cp -dp $(targetprefix)/etc/hostname $(prefix)/release_neutrino/etc/ && \
	cp -dp $(targetprefix)/etc/hosts $(prefix)/release_neutrino/etc/ && \
	cp -dp $(targetprefix)/etc/inittab $(prefix)/release_neutrino/etc/ && \
	cp -dp $(targetprefix)/etc/localtime $(prefix)/release_neutrino/etc/ && \
	cp -dp $(targetprefix)/etc/mtab $(prefix)/release_neutrino/etc/ && \
	cp -dp $(targetprefix)/etc/passwd $(prefix)/release_neutrino/etc/ && \
	cp -dp $(targetprefix)/etc/profile $(prefix)/release_neutrino/etc/ && \
	cp -dp $(targetprefix)/etc/protocols $(prefix)/release_neutrino/etc/ && \
	cp -dp $(targetprefix)/etc/resolv.conf $(prefix)/release_neutrino/etc/ && \
	cp -dp $(targetprefix)/etc/services $(prefix)/release_neutrino/etc/ && \
	cp -dp $(targetprefix)/etc/shells $(prefix)/release_neutrino/etc/ && \
	cp -dp $(targetprefix)/etc/shells.conf $(prefix)/release_neutrino/etc/ && \
	cp -dp $(targetprefix)/etc/timezone.xml $(prefix)/release_neutrino/etc/ && \
	cp -dp $(targetprefix)/etc/vsftpd.conf $(prefix)/release_neutrino/etc/ && \
	cp -dp $(targetprefix)/etc/vdstandby.cfg $(prefix)/release_neutrino/etc/ && \
	cp -dp $(targetprefix)/etc/network/interfaces $(prefix)/release_neutrino/etc/network/ && \
	cp -dp $(targetprefix)/etc/network/options $(prefix)/release_neutrino/etc/network/ && \
	cp -dp $(targetprefix)/etc/init.d/umountfs $(prefix)/release_neutrino/etc/init.d/ && \
	cp -dp $(targetprefix)/etc/init.d/sendsigs $(prefix)/release_neutrino/etc/init.d/ && \
	cp -dp $(targetprefix)/etc/init.d/halt $(prefix)/release_neutrino/etc/init.d/ && \
	mkdir -p $(prefix)/release_neutrino/usr/local/share/config/tuxtxt/ && \
	cp $(buildprefix)/root/etc/tuxbox/tuxtxt2.conf $(prefix)/release_neutrino/usr/local/share/config/tuxtxt/ && \
	cp $(buildprefix)/root/release/reboot $(prefix)/release_neutrino/etc/init.d/ && \
	echo "576i50" > $(prefix)/release_neutrino/etc/videomode && \
	cp $(buildprefix)/root/release/rcS_stm23_neutrino$(if $(SPARK),_$(SPARK)) $(prefix)/release_neutrino/etc/init.d/rcS && \
	chmod 755 $(prefix)/release_neutrino/etc/init.d/rcS && \
	cp $(buildprefix)/root/release/mountvirtfs $(prefix)/release_neutrino/etc/init.d/ && \
	cp $(buildprefix)/root/release/mme_check $(prefix)/release_neutrino/etc/init.d/ && \
	cp $(buildprefix)/root/release/mountall $(prefix)/release_neutrino/etc/init.d/ && \
	cp $(buildprefix)/root/release/hostname $(prefix)/release_neutrino/etc/init.d/ && \
	cp $(buildprefix)/root/release/vsftpd $(prefix)/release_neutrino/etc/init.d/ && \
	cp $(buildprefix)/root/release/bootclean.sh $(prefix)/release_neutrino/etc/init.d/ && \
	cp $(buildprefix)/root/release/networking $(prefix)/release_neutrino/etc/init.d/ && \
	cp $(buildprefix)/root/release/getfb.awk $(prefix)/release_neutrino/etc/init.d/ && \
	cp -rd $(targetprefix)/lib/* $(prefix)/release_neutrino/lib/ && \
	rm -f $(prefix)/release_neutrino/lib/*.a && \
	rm -f $(prefix)/release_neutrino/lib/*.o && \
	rm -f $(prefix)/release_neutrino/lib/*.la && \
	find $(prefix)/release_neutrino/lib/ -name '*.so*' -exec sh4-linux-strip --strip-unneeded {} \;
	cp $(kernelprefix)/$(kernelpath)/fs/autofs4/autofs4.ko $(prefix)/release_neutrino/lib/modules

if STM24
	cp -dp $(targetprefix)/sbin/mkfs $(prefix)/release_neutrino/sbin/
endif

if !STM22
	cp $(buildprefix)/root/release/rcS_stm23_neutrino$(if $(SPARK),_$(SPARK)) $(prefix)/release_neutrino/etc/init.d/rcS
endif
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/avs/avs.ko $(prefix)/release_neutrino/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/boxtype/boxtype.ko $(prefix)/release_neutrino/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/simu_button/simu_button.ko $(prefix)/release_neutrino/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/e2_proc/e2_proc.ko $(prefix)/release_neutrino/lib/modules/

if ENABLE_SPARK

	echo "pingulux" > $(prefix)/release_neutrino/etc/hostname

	cp $(buildprefix)/root/release/umountfs $(prefix)/release_neutrino/etc/init.d/
	cp $(buildprefix)/root/release/rc $(prefix)/release_neutrino/etc/init.d/
	cp $(buildprefix)/root/release/sendsigs $(prefix)/release_neutrino/etc/init.d/
	rm -f $(prefix)/release_neutrino/sbin/halt
	cp -f $(targetprefix)/sbin/halt $(prefix)/release_neutrino/sbin/
	cp $(buildprefix)/root/release/halt_spark_enigma2 $(prefix)/release_neutrino/etc/init.d/halt
	chmod 755 $(prefix)/release_neutrino/etc/init.d/umountfs
	chmod 755 $(prefix)/release_neutrino/etc/init.d/rc
	chmod 755 $(prefix)/release_neutrino/etc/init.d/sendsigs
	chmod 755 $(prefix)/release_neutrino/etc/init.d/halt
	mkdir -p $(prefix)/release_neutrino/etc/rc.d/rc0.d
	ln -s ../init.d $(prefix)/release_neutrino/etc/rc.d
	ln -fs halt $(prefix)/release_neutrino/sbin/reboot
	ln -fs halt $(prefix)/release_neutrino/sbin/poweroff
	ln -s ../init.d/sendsigs $(prefix)/release_neutrino/etc/rc.d/rc0.d/S20sendsigs
	ln -s ../init.d/umountfs $(prefix)/release_neutrino/etc/rc.d/rc0.d/S40umountfs
	ln -s ../init.d/halt $(prefix)/release_neutrino/etc/rc.d/rc0.d/S90halt
	mkdir -p $(prefix)/release_neutrino/etc/rc.d/rc6.d
	ln -s ../init.d/sendsigs $(prefix)/release_neutrino/etc/rc.d/rc6.d/S20sendsigs
	ln -s ../init.d/umountfs $(prefix)/release_neutrino/etc/rc.d/rc6.d/S40umountfs
	ln -s ../init.d/reboot $(prefix)/release_neutrino/etc/rc.d/rc6.d/S90reboot
	ln -fs init $(prefix)/release_neutrino/sbin/telinit

	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/frontcontroller/aotom/aotom.ko $(prefix)/release_neutrino/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stmcore-display-sti7111.ko $(prefix)/release_neutrino/lib/modules/

	rm -f $(prefix)/release_neutrino/lib/firmware/dvb-fe-avl2108.fw
	rm -f $(prefix)/release_neutrino/lib/firmware/dvb-fe-stv6306.fw
	rm -f $(prefix)/release_neutrino/lib/firmware/dvb-fe-cx24116.fw
	rm -f $(prefix)/release_neutrino/lib/firmware/dvb-fe-cx21143.fw
	rm -f $(prefix)/release_neutrino/lib/firmware/component_7105_pdk7105.fw
	mv $(prefix)/release_neutrino/lib/firmware/component_7111_mb618.fw $(prefix)/release_neutrino/lib/firmware/component.fw
	rm -f $(prefix)/release_neutrino/bin/evremote
	rm -f $(prefix)/release_neutrino/bin/gotosleep

	cp -dp $(buildprefix)/root/etc/lircd_spark.conf $(prefix)/release_neutrino/etc/lircd.conf
	cp -dp $(targetprefix)/usr/bin/lircd $(prefix)/release_neutrino/usr/bin/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/smartcard/smartcard.ko $(prefix)/release_neutrino/lib/modules/
	cp -RP $(buildprefix)/root/etc/init.d/setupETH.sh $(prefix)/release_neutrino/etc/init.d/
if STM23
	cp -f $(buildprefix)/root/release/vfd_spark$(KERNELSTMLABEL)_noptk.ko $(prefix)/release_neutrino/lib/modules/vfd.ko
	cp -f $(buildprefix)/root/lib/modules/* $(prefix)/release_neutrino/lib/modules/
else
	cp -f $(buildprefix)/root/release/vfd_spark$(KERNELSTMLABEL).ko $(prefix)/release_neutrino/lib/modules/vfd.ko
endif
	cp -f $(buildprefix)/root/release/encrypt_spark$(KERNELSTMLABEL).ko $(prefix)/release_neutrino/lib/modules/encrypt.ko
	[ -e $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/cpu_frequ/cpu_frequ.ko ] && cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/cpu_frequ/cpu_frequ.ko $(prefix)/release/lib/modules || true

endif
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stmfb.ko $(prefix)/release_neutrino/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/multicom/embxshell/embxshell.ko $(prefix)/release_neutrino/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/multicom/embxmailbox/embxmailbox.ko $(prefix)/release_neutrino/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/multicom/embxshm/embxshm.ko $(prefix)/release_neutrino/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/multicom/mme/mme_host.ko $(prefix)/release_neutrino/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/bpamem/bpamem.ko $(prefix)/release_neutrino/lib/modules/

	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/frontends/*.ko $(prefix)/release_neutrino/lib/modules/

	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/compcache/lzo-kmod/lzo1x_compress.ko $(prefix)/release_neutrino/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/compcache/lzo-kmod/lzo1x_decompress.ko $(prefix)/release_neutrino/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/compcache/ramzswap.ko $(prefix)/release_neutrino/lib/modules/

if ENABLE_PLAYER131
	[ -e $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/pti/pti.ko ] && cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/pti/pti.ko $(prefix)/release_neutrino/lib/modules/ || true
	[ -e $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/pti_np/pti.ko ] && cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/pti_np/pti.ko $(prefix)/release_neutrino/lib/modules/ || true

	find $(prefix)/release_neutrino/lib/modules/ -name '*.ko' -exec sh4-linux-strip --strip-unneeded {} \;
	cd $(targetprefix)/lib/modules/$(KERNELVERSION)/extra && \
	for mod in \
		sound/pseudocard/pseudocard.ko \
		sound/silencegen/silencegen.ko \
		stm/mmelog/mmelog.ko \
		stm/monitor/stm_monitor.ko \
		media/video/stm/stm_v4l2.ko \
		media/dvb/stm/dvb/stmdvb.ko \
		sound/ksound/ksound.ko \
		media/dvb/stm/mpeg2_hard_host_transformer/mpeg2hw.ko \
		media/dvb/stm/backend/player2.ko \
		media/dvb/stm/h264_preprocessor/sth264pp.ko \
		media/dvb/stm/allocator/stmalloc.ko \
		stm/platform/platform.ko \
		stm/platform/p2div64.ko \
	;do \
		if [ -e player2/linux/drivers/$$mod ] ; then \
			cp player2/linux/drivers/$$mod $(prefix)/release_neutrino/lib/modules/; \
			sh4-linux-strip --strip-unneeded $(prefix)/release_neutrino/lib/modules/`basename $$mod`; \
		else \
			touch $(prefix)/release_neutrino/lib/modules/`basename $$mod`; \
		fi;\
	done
endif

if ENABLE_PLAYER179
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stm_v4l2.ko $(prefix)/release_neutrino/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stmvbi.ko $(prefix)/release_neutrino/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stmvout.ko $(prefix)/release_neutrino/lib/modules/

	[ -e $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/pti/pti.ko ] && cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/pti/pti.ko $(prefix)/release_neutrino/lib/modules/ || true
	[ -e $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/pti_np/pti.ko ] && cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/pti_np/pti.ko $(prefix)/release_neutrino/lib/modules/ || true

	find $(prefix)/release_neutrino/lib/modules/ -name '*.ko' -exec sh4-linux-strip --strip-unneeded {} \;
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
			cp player2/linux/drivers/$$mod $(prefix)/release_neutrino/lib/modules/; \
			sh4-linux-strip --strip-unneeded $(prefix)/release_neutrino/lib/modules/`basename $$mod`; \
		else \
			touch $(prefix)/release_neutrino/lib/modules/`basename $$mod`; \
		fi;\
	done
endif

if ENABLE_PLAYER191
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stm_v4l2.ko $(prefix)/release_neutrino/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stmvbi.ko $(prefix)/release_neutrino/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stmvout.ko $(prefix)/release_neutrino/lib/modules/

	[ -e $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/pti/pti.ko ] && cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/pti/pti.ko $(prefix)/release_neutrino/lib/modules/ || true
	[ -e $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/pti_np/pti.ko ] && cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/pti_np/pti.ko $(prefix)/release_neutrino/lib/modules/ || true

	find $(prefix)/release_neutrino/lib/modules/ -name '*.ko' -exec sh4-linux-strip --strip-unneeded {} \;
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
			cp player2/linux/drivers/$$mod $(prefix)/release_neutrino/lib/modules/; \
			sh4-linux-strip --strip-unneeded $(prefix)/release_neutrino/lib/modules/`basename $$mod`; \
		else \
			touch $(prefix)/release_neutrino/lib/modules/`basename $$mod`; \
		fi;\
	done
endif

if STM22
	rm $(prefix)/release_neutrino/lib/modules/p2div64.ko
endif
	rm -rf $(prefix)/release_neutrino/lib/modules/$(KERNELVERSION)

	$(INSTALL_DIR) $(prefix)/release_neutrino/media
	ln -sf /media/hdd $(prefix)/release_neutrino/hdd
	$(INSTALL_DIR) $(prefix)/release_neutrino/media/dvd

	$(INSTALL_DIR) $(prefix)/release_neutrino/mnt
	$(INSTALL_DIR) $(prefix)/release_neutrino/mnt/usb
	$(INSTALL_DIR) $(prefix)/release_neutrino/mnt/hdd
	$(INSTALL_DIR) $(prefix)/release_neutrino/mnt/nfs

	$(INSTALL_DIR) $(prefix)/release_neutrino/root

	$(INSTALL_DIR) $(prefix)/release_neutrino/proc
	$(INSTALL_DIR) $(prefix)/release_neutrino/sys
	$(INSTALL_DIR) $(prefix)/release_neutrino/tmp

	$(INSTALL_DIR) $(prefix)/release_neutrino/usr
	$(INSTALL_DIR) $(prefix)/release_neutrino/usr/bin
	$(INSTALL_DIR) $(prefix)/release_neutrino/usr/lib
	cp -p $(targetprefix)/usr/sbin/vsftpd $(prefix)/release_neutrino/usr/bin/

	cp -p $(targetprefix)/usr/bin/killall $(prefix)/release_neutrino/usr/bin/
	cp -p $(targetprefix)/usr/sbin/ethtool $(prefix)/release_neutrino/usr/sbin/

	$(INSTALL_DIR) $(prefix)/release_neutrino/usr/tuxtxt


#######################################################################################
	mkdir -p $(prefix)/release_neutrino/tuxbox/config
	mkdir -p $(prefix)/release_neutrino/var/plugins
	mkdir -p $(prefix)/release_neutrino/lib/tuxbox
	mkdir -p $(prefix)/release_neutrino/usr/lib/tuxbox
	mkdir -p $(prefix)/release_neutrino/usr/keys
	mkdir -p $(prefix)/release_neutrino/var/tuxbox/config
	mkdir -p $(prefix)/release_neutrino/share/tuxbox
	mkdir -p $(prefix)/release_neutrino/var/share/icons
#	mkdir -p $(prefix)/release_neutrino/var/usr/local/share/config
	mkdir -p $(prefix)/release_neutrino/usr/local/share/neutrino/icons
	mkdir -p $(prefix)/release_neutrino/media/net
#	( cd $(prefix)/release_neutrino/var && ln -s ../usr/bin emu )
	( cd $(prefix)/release_neutrino/var && ln -s ../usr/keys keys )
	( cd $(prefix)/release_neutrino/var/tuxbox/config && ln -s ../../../usr/local/share/config/neutrino.conf )
	( cd $(prefix)/release_neutrino/share/tuxbox && ln -s /usr/local/share/neutrino )
	( cd $(prefix)/release_neutrino/var/share/icons/ && ln -s /usr/local/share/neutrino/icons/logo )
	( cd $(prefix)/release_neutrino/ && ln -s /usr/local/share/neutrino/icons/logo logos )
	( cd $(prefix)/release_neutrino/usr/lib && ln -s libcrypto.so.0.9.8 libcrypto.so.0.9.7 )
	( cd $(prefix)/release_neutrino/lib/tuxbox && ln -s /var/plugins )
	( cd $(prefix)/release_neutrino/var/tuxbox && ln -s /var/plugins )
	( cd $(prefix)/release_neutrino/usr/lib/tuxbox && ln -s /var/plugins )
	rm -rf $(prefix)/release_neutrino/media/sda*
	cp -RP $(appsdir)/neutrino/data/icons/* $(prefix)/release_neutrino/usr/local/share/neutrino/icons/

#######################################################################################
#######################################################################################
#######################################################################################

	$(INSTALL_DIR) $(prefix)/release_neutrino/usr/share

#######################################################################################


#######################################################################################

#######################################################################################

	$(INSTALL_DIR) $(prefix)/release_neutrino/usr/share/zoneinfo
	cp -aR $(buildprefix)/root/usr/share/zoneinfo/* $(prefix)/release_neutrino/usr/share/zoneinfo/

#######################################################################################

	$(INSTALL_DIR) $(prefix)/release_neutrino/usr/share/udhcpc
	cp -aR $(buildprefix)/root/usr/share/udhcpc/* $(prefix)/release_neutrino/usr/share/udhcpc/


#######################################################################################
#######################################################################################
#######################################################################################
#######################################################################################

	$(INSTALL_DIR) $(prefix)/release_neutrino/usr/local

#######################################################################################

	$(INSTALL_DIR) $(prefix)/release_neutrino/usr/local/bin
	cp $(targetprefix)/usr/local/bin/neutrino $(prefix)/release_neutrino/usr/local/bin/
	cp $(targetprefix)/usr/local/bin/pzapit $(prefix)/release_neutrino/usr/local/bin/
	cp $(targetprefix)/usr/local/bin/sectionsdcontrol $(prefix)/release_neutrino/usr/local/bin/
	find $(prefix)/release_neutrino/usr/local/bin/ -name  neutrino -exec sh4-linux-strip --strip-unneeded {} \;
	find $(prefix)/release_neutrino/usr/local/bin/ -name  pzapit -exec sh4-linux-strip --strip-unneeded {} \;
	find $(prefix)/release_neutrino/usr/local/bin/ -name  sectionsdcontrol -exec sh4-linux-strip --strip-unneeded {} \;

#######################################################################################

	$(INSTALL_DIR) $(prefix)/release_neutrino/usr/local/share
	cp -aR $(targetprefix)/usr/local/share/iso-codes $(prefix)/release_neutrino/usr/local/share/
#	TODO: Channellist ....
	$(INSTALL_DIR) $(prefix)/release_neutrino/usr/local/share/config
	cp -aR $(buildprefix)/root/usr/local/share/config/* $(prefix)/release_neutrino/usr/local/share/config/
	cp -aR $(targetprefix)/usr/local/share/neutrino $(prefix)/release_neutrino/usr/local/share/
#	TODO: HACK
	cp -aR $(targetprefix)/$(targetprefix)/usr/local/share/neutrino/* $(prefix)/release_neutrino/usr/local/share/neutrino
#######################################################################################
#	cp -aR $(targetprefix)/usr/local/share/fonts $(prefix)/release_neutrino/usr/local/share/
	mkdir -p $(prefix)/release_neutrino/usr/local/share/fonts
	cp $(buildprefix)/root/usr/share/fonts/tuxtxt.ttf $(prefix)/release_neutrino/usr/local/share/fonts/

#       Font libass
	mkdir -p $(prefix)/release_neutrino/usr/share/fonts
	cp $(buildprefix)/root/usr/share/fonts/FreeSans.ttf $(prefix)/release_neutrino/usr/share/fonts/

	cp -aR $(targetprefix)/usr/local/share/fonts/micron.ttf $(prefix)/release_neutrino/usr/local/share/fonts/neutrino.ttf
	cp $(appsdir)/neutrino/src/nhttpd/web/{Y_Baselib.js,Y_VLC.js} $(prefix)/release_neutrino/usr/local/share/neutrino/httpd-y/
#	rm $(prefix)/release_neutrino/usr/local/share/neutrino/httpd-y/{Y_Baselib.js.gz,Y_VLC.js.gz}
#######################################################################################
	echo "duckbox-rev#: " > $(prefix)/release_neutrino/etc/imageinfo
	git describe >> $(prefix)/release_neutrino/etc/imageinfo
	$(buildprefix)/root/release/neutrino_version.sh
	rm -rf $(prefix)/release_neutrino/var/etc
#	fix img info
	echo 'comment=created by pingulux-git' > $(buildprefix)/root/var/etc/.version.new
	echo 'imagename=Neutrino for Pingulux' >> $(buildprefix)/root/var/etc/.version.new
	echo 'homepage=http://gitorious.org/open-duckbox-project-sh4/pingulux-git' >> $(buildprefix)/root/var/etc/.version.new
	echo 'creator=git-developer' >> $(buildprefix)/root/var/etc/.version.new
	echo 'docs=http://gitorious.org/open-duckbox-project-sh4/pingulux-git/commits' >> $(buildprefix)/root/var/etc/.version.new
	echo 'forum=http://kathif.vs120005.hl-users.com/index.php?sid=' >> $(buildprefix)/root/var/etc/.version.new
	mv $(buildprefix)/root/var/etc/.version.new $(prefix)/release_neutrino/etc/.version
	ln -s ../etc $(prefix)/release_neutrino/var
	ln -sf ../etc/.version $(prefix)/release_neutrino
#######################################################################################

	$(INSTALL_DIR) $(prefix)/release_neutrino/usr/lib

	cp -R $(targetprefix)/usr/lib/* $(prefix)/release_neutrino/usr/lib/
	cp -R $(targetprefix)/usr/local/lib/* $(prefix)/release_neutrino/usr/lib/
	rm -rf $(prefix)/release_neutrino/usr/lib/alsa-lib
	rm -rf $(prefix)/release_neutrino/usr/lib/alsaplayer
	rm -rf $(prefix)/release_neutrino/usr/lib/engines
	rm -rf $(prefix)/release_neutrino/usr/lib/enigma2
	rm -rf $(prefix)/release_neutrino/usr/lib/gconv
	rm -rf $(prefix)/release_neutrino/usr/lib/libxslt-plugins
	rm -rf $(prefix)/release_neutrino/usr/lib/pkgconfig
	rm -rf $(prefix)/release_neutrino/usr/lib/sigc++-1.2
	rm -rf $(prefix)/release_neutrino/usr/lib/X11
	rm -f $(prefix)/release_neutrino/usr/lib/*.a
	rm -f $(prefix)/release_neutrino/usr/lib/*.o
	rm -f $(prefix)/release_neutrino/usr/lib/*.la
	mkdir -p $(prefix)/release_neutrino/usr/local/share/neutrino/icons/logo
	( cd $(prefix)/release_neutrino/usr/local/share/neutrino/httpd-y && ln -s /usr/local/share/neutrino/icons/logo )
	( cd $(prefix)/release_neutrino/usr/local/share/neutrino/httpd-y && ln -s /usr/local/share/neutrino/icons/logo logos )
	( cd $(prefix)/release_neutrino/usr/local/share/neutrino && ln -s /usr/local/share/neutrino/httpd-y httpd )
	( cd $(prefix)/release_neutrino/var && ln -s /usr/local/share/neutrino/httpd-y httpd )
	find $(prefix)/release_neutrino/usr/lib/ -name '*.so*' -exec sh4-linux-strip --strip-unneeded {} \;

	cp -f $(buildprefix)/root/etc/init.d/Swap.sh $(prefix)/release_neutrino/etc/init.d/
#	install autofs
	cp -f $(targetprefix)/usr/sbin/automount $(prefix)/release_neutrino/usr/sbin/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/kernel/fs/autofs4/autofs4.ko $(prefix)/release_neutrino/lib/modules
	cp -f $(buildprefix)/root/release/auto.usb $(prefix)/release_neutrino/etc/
	echo 'sda    -fstype=auto,noatime,nodiratime          :/dev/sda' >> $(prefix)/release_neutrino/etc/auto.usb
	echo 'sda1   -fstype=auto,noatime,nodiratime          :/dev/sda1' >> $(prefix)/release_neutrino/etc/auto.usb
	echo 'sda2   -fstype=auto,noatime,nodiratime          :/dev/sda2' >> $(prefix)/release_neutrino/etc/auto.usb
	echo 'sda3   -fstype=auto,noatime,nodiratime          :/dev/sda3' >> $(prefix)/release_neutrino/etc/auto.usb
#
######## FOR YOUR OWN CHANGES use these folder in cdk/own_build/neutrino #############
#	rm $(prefix)/release_neutrino/bin/mount
	cp -RP $(buildprefix)/own_build/neutrino/* $(prefix)/release_neutrino/

	cp $(kernelprefix)/$(kernelpath)/arch/sh/boot/uImage $(prefix)/release_neutrino/boot/

	cp -dp $(targetprefix)/sbin/blkid $(prefix)/release_neutrino/usr/bin/
	cp -dp $(targetprefix)/usr/bin/rdate $(prefix)/release_neutrino/usr/bin/

if STM24
	[ -e $(kernelprefix)/$(kernelpath)/fs/autofs4/autofs4.ko ] && cp $(kernelprefix)/$(kernelpath)/fs/autofs4/autofs4.ko $(prefix)/release_neutrino/lib/modules || true
	[ -e $(kernelprefix)/$(kernelpath)/drivers/usb/serial/ftdi_sio.ko ] && cp $(kernelprefix)/$(kernelpath)/drivers/usb/serial/ftdi_sio.ko $(prefix)/release_neutrino/lib/modules/ftdi.ko || true
	[ -e $(kernelprefix)/$(kernelpath)/drivers/usb/serial/pl2303.ko ] && cp $(kernelprefix)/$(kernelpath)/drivers/usb/serial/pl2303.ko $(prefix)/release_neutrino/lib/modules || true
	[ -e $(kernelprefix)/$(kernelpath)/drivers/usb/serial/usbserial.ko ] && cp $(kernelprefix)/$(kernelpath)/drivers/usb/serial/usbserial.ko $(prefix)/release_neutrino/lib/modules || true
	[ -e $(kernelprefix)/$(kernelpath)/fs/ntfs/ntfs.ko ] && cp $(kernelprefix)/$(kernelpath)/fs/ntfs/ntfs.ko $(prefix)/release_neutrino/lib/modules || true
	[ -e $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/cpu_frequ/cpu_frequ.ko ] && cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/cpu_frequ/cpu_frequ.ko $(prefix)/release/lib/modules || true
endif

	touch $@
