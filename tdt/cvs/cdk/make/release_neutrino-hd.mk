#Trick ALPHA-Version ;)
$(DEPDIR)/min-release_neutrino-hd $(DEPDIR)/std-release_neutrino-hd $(DEPDIR)/max-release_neutrino-hd $(DEPDIR)/ipk-release_neutrino-hd $(DEPDIR)/release_neutrino-hd: \
$(DEPDIR)/%release_neutrino-hd:
	rm -rf $(prefix)/release_neutrino-hd || true
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd && \
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/bin && \
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/sbin && \
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/boot && \
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/dev && \
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/dev.static && \
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/etc && \
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/etc/fonts && \
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/etc/init.d && \
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/etc/network && \
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/etc/network/if-down.d && \
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/etc/network/if-post-down.d && \
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/etc/network/if-pre-up.d && \
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/etc/network/if-up.d && \
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/etc/tuxbox && \
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/media/hdd && \
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/media/hdd/movie && \
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/media/hdd/music && \
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/media/hdd/picture && \
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/lib && \
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/lib/modules && \
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/ram && \
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/var && \
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/var/etc && \
	export CROSS_COMPILE=$(target)- && \
		$(MAKE) install -C @DIR_busybox@ CONFIG_PREFIX=$(prefix)/release_neutrino-hd && \
	cp -a $(targetprefix)/bin/* $(prefix)/release_neutrino-hd/bin/ && \
	rm $(prefix)/release_neutrino-hd/bin/showiframe && \
	rm $(prefix)/release_neutrino-hd/bin/stslave && \
	cp -dp $(targetprefix)/bin/hotplug $(prefix)/release_neutrino-hd/sbin/ && \
	cp -dp $(targetprefix)/sbin/init $(prefix)/release_neutrino-hd/sbin/ && \
	cp -dp $(targetprefix)/sbin/killall5 $(prefix)/release_neutrino-hd/sbin/ && \
	cp -dp $(targetprefix)/sbin/portmap $(prefix)/release_neutrino-hd/sbin/ && \
	cp -dp $(targetprefix)/sbin/mke2fs $(prefix)/release_neutrino-hd/sbin/ && \
	cp -dp $(targetprefix)/sbin/mkfs.ext2 $(prefix)/release_neutrino-hd/sbin/ && \
	cp -dp $(targetprefix)/sbin/mkfs.ext3 $(prefix)/release_neutrino-hd/sbin/ && \
	cp -dp $(targetprefix)/sbin/fsck $(prefix)/release_neutrino-hd/sbin/ && \
	cp -dp $(targetprefix)/sbin/fsck.ext2 $(prefix)/release_neutrino-hd/sbin/ && \
	cp -dp $(targetprefix)/sbin/fsck.ext3 $(prefix)/release_neutrino-hd/sbin/ && \
	cp -dp $(targetprefix)/sbin/fsck.nfs $(prefix)/release_neutrino-hd/sbin/ && \
	cp -dp $(targetprefix)/sbin/sfdisk $(prefix)/release_neutrino-hd/sbin/ && \
	cp -dp $(targetprefix)/sbin/tune2fs $(prefix)/release_neutrino-hd/sbin/ && \
	cp -dp $(targetprefix)/usr/bin/rdate $(prefix)/release_neutrino-hd/sbin/ && \
	cp -dp $(targetprefix)/etc/init.d/portmap $(prefix)/release_neutrino-hd/etc/init.d/ && \
	cp -dp $(buildprefix)/root/etc/init.d/udhcpc $(prefix)/release_neutrino-hd/etc/init.d/ && \
	cp -dp $(targetprefix)/usr/bin/ffmpeg $(prefix)/release_neutrino-hd/sbin/ && \
	\
	cp $(targetprefix)/boot/video_7100.elf $(prefix)/release_neutrino-hd/boot/video.elf && \
	cp $(targetprefix)/boot/video_7111.elf $(prefix)/release_neutrino-hd/boot/video.elf && \
	\
	cp $(targetprefix)/boot/audio.elf $(prefix)/release_neutrino-hd/boot/audio.elf && \
	cp $(targetprefix)/boot/audio_7111.elf $(prefix)/release_neutrino-hd/boot/audio.elf && \
	\
	cp -a $(targetprefix)/dev/* $(prefix)/release_neutrino-hd/dev/ && \
	cp -f $(buildprefix)/root/release/fstab_spark $(prefix)/release_neutrino-hd/etc/fstab && \
	cp -dp $(buildprefix)/root/etc/tuxbox/timezone.xml $(prefix)/release_neutrino-hd/etc/ && \
	cp -dp $(targetprefix)/etc/group $(prefix)/release_neutrino-hd/etc/ && \
	cp -dp $(targetprefix)/etc/host.conf $(prefix)/release_neutrino-hd/etc/ && \
	cp -dp $(targetprefix)/etc/hostname $(prefix)/release_neutrino-hd/etc/ && \
	cp -dp $(targetprefix)/etc/hosts $(prefix)/release_neutrino-hd/etc/ && \
	echo "127.0.0.1       localhost.localdomain   localhost" > $(prefix)/release_neutrino-hd/etc/hosts && \
	touch $(prefix)/release_neutrino-hd/etc/nsswitch.conf && \
	echo "hosts: files dns" > $(prefix)/release_neutrino-hd/etc/nsswitch.conf && \
	cp -dp $(targetprefix)/etc/inittab $(prefix)/release_neutrino-hd/etc/ && \
	cp -dp $(targetprefix)/etc/localtime $(prefix)/release_neutrino-hd/etc/ && \
	cp -dp $(targetprefix)/etc/mtab $(prefix)/release_neutrino-hd/etc/ && \
	cp -dp $(targetprefix)/etc/passwd $(prefix)/release_neutrino-hd/etc/ && \
	cp -dp $(targetprefix)/etc/profile $(prefix)/release_neutrino-hd/etc/ && \
	cp -dp $(targetprefix)/etc/protocols $(prefix)/release_neutrino-hd/etc/ && \
	cp -dp $(targetprefix)/etc/resolv.conf $(prefix)/release_neutrino-hd/etc/ && \
	cp -dp $(targetprefix)/etc/services $(prefix)/release_neutrino-hd/etc/ && \
	cp -dp $(targetprefix)/etc/shells $(prefix)/release_neutrino-hd/etc/ && \
	cp -dp $(targetprefix)/etc/shells.conf $(prefix)/release_neutrino-hd/etc/ && \
	cp -dp $(targetprefix)/etc/vsftpd.conf $(prefix)/release_neutrino-hd/etc/ && \
	sed "s@listen=YES@listen=NO@g" -i $(prefix)/release_neutrino-hd/etc/vsftpd.conf && \
	echo "8001 stream tcp nowait root /bin/streamproxy streamproxy" > $(prefix)/release_neutrino-hd/etc/inetd.conf && \
	echo "ftp stream tcp nowait root /usr/bin/vsftpd vsftpd" >> $(prefix)/release_neutrino-hd/etc/inetd.conf && \
	echo "telnet stream tcp nowait root /usr/sbin/telnetd telnetd -i -l /bin/login" >> $(prefix)/release_neutrino-hd/etc/inetd.conf && \
	echo "ssh stream tcp nowait root /bin/dropbear dropbear -i" >> $(prefix)/release_neutrino-hd/etc/inetd.conf && \
	cp -dp $(targetprefix)/etc/vdstandby.cfg $(prefix)/release_neutrino-hd/etc/ && \
	cp -dp $(targetprefix)/etc/network/interfaces $(prefix)/release_neutrino-hd/etc/network/ && \
	cp -dp $(targetprefix)/etc/network/options $(prefix)/release_neutrino-hd/etc/network/ && \
	cp -dp $(targetprefix)/etc/init.d/umountfs $(prefix)/release_neutrino-hd/etc/init.d/ && \
	cp -dp $(targetprefix)/etc/init.d/sendsigs $(prefix)/release_neutrino-hd/etc/init.d/ && \
	cp -dp $(targetprefix)/etc/init.d/halt $(prefix)/release_neutrino-hd/etc/init.d/ && \
	mkdir -p $(prefix)/release_neutrino-hd/usr/local/share/config/tuxtxt/ && \
	cp $(buildprefix)/root/release/reboot $(prefix)/release_neutrino-hd/etc/init.d/ && \
	echo "576i50" > $(prefix)/release_neutrino-hd/etc/videomode && \
	cp $(buildprefix)/root/release/rcS_stm23_24_neutrino_spark $(prefix)/release_neutrino-hd/etc/init.d/rcS && \
	cp $(buildprefix)/root/etc/init.d/bu_restore.sh $(prefix)/release_neutrino-hd/etc/init.d/ && \
	chmod 755 $(prefix)/release_neutrino-hd/etc/init.d/rcS && \
	cp $(buildprefix)/root/release/mountvirtfs $(prefix)/release_neutrino-hd/etc/init.d/ && \
	cp $(buildprefix)/root/release/mme_check $(prefix)/release_neutrino-hd/etc/init.d/ && \
	cp $(buildprefix)/root/release/mountall $(prefix)/release_neutrino-hd/etc/init.d/ && \
	cp $(buildprefix)/root/release/hostname $(prefix)/release_neutrino-hd/etc/init.d/ && \
	cp $(buildprefix)/root/release/vsftpd $(prefix)/release_neutrino-hd/etc/init.d/ && \
	cp $(buildprefix)/root/release/bootclean.sh $(prefix)/release_neutrino-hd/etc/init.d/ && \
	cp $(buildprefix)/root/release/networking $(prefix)/release_neutrino-hd/etc/init.d/ && \
	cp $(buildprefix)/root/release/getfb.awk $(prefix)/release_neutrino-hd/etc/init.d/ && \
	cp -rd $(targetprefix)/lib/* $(prefix)/release_neutrino-hd/lib/ && \
	find $(prefix)/release_neutrino-hd/lib/ -name '*.so*' -exec sh4-linux-strip --strip-unneeded {} \;

	cp -dp $(targetprefix)/sbin/mkfs $(prefix)/release_neutrino-hd/sbin/

	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/avs/avs.ko $(prefix)/release_neutrino-hd/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/boxtype/boxtype.ko $(prefix)/release_neutrino-hd/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/simu_button/simu_button.ko $(prefix)/release_neutrino-hd/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/e2_proc/e2_proc.ko $(prefix)/release_neutrino-hd/lib/modules/

	echo "pingulux" > $(prefix)/release_neutrino-hd/etc/hostname

	cp $(buildprefix)/root/release/umountfs $(prefix)/release_neutrino-hd/etc/init.d/
	cp $(buildprefix)/root/release/rc $(prefix)/release_neutrino-hd/etc/init.d/
	cp $(buildprefix)/root/release/sendsigs $(prefix)/release_neutrino-hd/etc/init.d/
	rm -f $(prefix)/release_neutrino-hd/sbin/halt
	cp -f $(targetprefix)/sbin/halt $(prefix)/release_neutrino-hd/sbin/
	cp $(buildprefix)/root/release/halt_spark_enigma2 $(prefix)/release_neutrino-hd/etc/init.d/halt
	chmod 755 $(prefix)/release_neutrino-hd/etc/init.d/umountfs
	chmod 755 $(prefix)/release_neutrino-hd/etc/init.d/rc
	chmod 755 $(prefix)/release_neutrino-hd/etc/init.d/sendsigs
	chmod 755 $(prefix)/release_neutrino-hd/etc/init.d/halt
	mkdir -p $(prefix)/release_neutrino-hd/etc/rc.d/rc0.d
	ln -s ../init.d $(prefix)/release_neutrino-hd/etc/rc.d
	ln -fs halt $(prefix)/release_neutrino-hd/sbin/reboot
	ln -fs halt $(prefix)/release_neutrino-hd/sbin/poweroff
	ln -s ../init.d/sendsigs $(prefix)/release_neutrino-hd/etc/rc.d/rc0.d/S20sendsigs
	ln -s ../init.d/umountfs $(prefix)/release_neutrino-hd/etc/rc.d/rc0.d/S40umountfs
	ln -s ../init.d/halt $(prefix)/release_neutrino-hd/etc/rc.d/rc0.d/S90halt
	mkdir -p $(prefix)/release_neutrino-hd/etc/rc.d/rc6.d
	ln -s ../init.d/sendsigs $(prefix)/release_neutrino-hd/etc/rc.d/rc6.d/S20sendsigs
	ln -s ../init.d/umountfs $(prefix)/release_neutrino-hd/etc/rc.d/rc6.d/S40umountfs
	ln -s ../init.d/reboot $(prefix)/release_neutrino-hd/etc/rc.d/rc6.d/S90reboot
	ln -fs init $(prefix)/release_neutrino-hd/sbin/telinit

	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/frontcontroller/aotom/aotom.ko $(prefix)/release_neutrino-hd/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stmcore-display-sti7111.ko $(prefix)/release_neutrino-hd/lib/modules/

	rm -f $(prefix)/release_neutrino-hd/lib/firmware/dvb-fe-avl2108.fw
	rm -f $(prefix)/release_neutrino-hd/lib/firmware/dvb-fe-stv6306.fw
	rm -f $(prefix)/release_neutrino-hd/lib/firmware/dvb-fe-cx24116.fw
	rm -f $(prefix)/release_neutrino-hd/lib/firmware/dvb-fe-cx21143.fw
	rm -f $(prefix)/release_neutrino-hd/lib/firmware/component_7105_pdk7105.fw
	mv $(prefix)/release_neutrino-hd/lib/firmware/component_7111_mb618.fw $(prefix)/release_neutrino-hd/lib/firmware/component.fw
	rm -f $(prefix)/release_neutrino-hd/bin/evremote
	rm -f $(prefix)/release_neutrino-hd/bin/gotosleep
	cp -f $(buildprefix)/root/usr/bin/mkfs.jffs2 $(prefix)/release_neutrino-hd/usr/bin/
	cp -f $(buildprefix)/root/sbin/mkyaffs2 $(prefix)/release_neutrino-hd/sbin/
	mkdir -p $(prefix)/release_neutrino-hd/usr/lib
	cp -RP $(buildprefix)/root/usr/lib/liblzo2.so.2* $(prefix)/release_neutrino-hd/usr/lib/
	cp -RP $(buildprefix)/root/lib/libproc* $(prefix)/release_neutrino-hd/lib/
	cp -RP $(targetprefix)/usr/lib/libevent*.so* $(prefix)/release_neutrino-hd/usr/lib/
	cp -RP $(targetprefix)/usr/lib/libnfsi*.so* $(prefix)/release_neutrino-hd/usr/lib/
	cp -RP $(targetprefix)/usr/lib/libopkg*.so* $(prefix)/release_neutrino-hd/usr/lib/
#	cp -RP $(buildprefix)/root/lib/libwrap* $(prefix)/release_neutrino-hd/lib/
	cp -RP $(buildprefix)/root/usr/bin/rpcinfo $(prefix)/release_neutrino-hd/usr/bin/
	cp -RP $(buildprefix)/root/sbin/ntpdate $(prefix)/release_neutrino-hd/sbin/
	rm $(prefix)/release_neutrino-hd/bin/ps
	mkdir -p $(prefix)/release_neutrino-hd/usr/lib/opkg/info
	touch $(prefix)/release_neutrino-hd/usr/lib/opkg/info/opkg.list
	mkdir -p $(prefix)/release_neutrino-hd/etc/opkg
	touch $(prefix)/release_neutrino-hd/etc/opkg/opkg.conf
	echo "src/gz graugans http://packages.evolux.yourweb.de" > $(prefix)/release_neutrino-hd/etc/opkg/opkg.conf
	cp -RP $(buildprefix)/root/bin/ps $(prefix)/release_neutrino-hd/bin/
	cp -RP $(buildprefix)/root/bin/dropbear $(prefix)/release_neutrino-hd/bin/
	cp -RP $(buildprefix)/root/bin/dropbearkey $(prefix)/release_neutrino-hd/bin/
	cp -RP $(buildprefix)/root/etc/init.d/dropbear $(prefix)/release_neutrino-hd/etc/init.d/
	cp -RP $(buildprefix)/root/etc/init.d/DisplayTime.sh $(prefix)/release_neutrino-hd/etc/init.d/
	touch $(prefix)/release_neutrino-hd/etc/.time
	mkdir -p $(prefix)/release_neutrino-hd/etc/dropbear
	cp -f $(buildprefix)/root/usr/bin/backup_pingulux_image.sh $(prefix)/release_neutrino-hd/usr/bin/
	cp -f $(buildprefix)/root/usr/sbin/iw* $(prefix)/release_neutrino-hd/usr/sbin/
	cp -f $(buildprefix)/root/usr/sbin/wpa* $(prefix)/release_neutrino-hd/usr/sbin/
	cp -f $(targetprefix)/usr/sbin/exportfs $(prefix)/release_neutrino-hd/usr/sbin/
	cp -f $(targetprefix)/usr/sbin/showmount $(prefix)/release_neutrino-hd/usr/sbin/
	cp -f $(targetprefix)/usr/sbin/rpc* $(prefix)/release_neutrino-hd/usr/sbin/
	mkdir -p $(prefix)/release_neutrino-hd/var
	cp -RP $(targetprefix)/var/lib $(prefix)/release_neutrino-hd/var/
	cp -RP $(buildprefix)/root/etc/Wireless $(prefix)/release_neutrino-hd/etc/
	cp -RP $(targetprefix)/etc/exports $(prefix)/release_neutrino-hd/etc/
	cp -f $(buildprefix)/root/usr/lib/libiw.so.29 $(prefix)/release_neutrino-hd/usr/lib/
	cp -f $(buildprefix)/root/usr/lib/libgnu* $(prefix)/release_neutrino-hd/usr/lib/
	cp -f $(buildprefix)/root/usr/lib/libgcrypt* $(prefix)/release_neutrino-hd/usr/lib/
	cp -f $(buildprefix)/root/usr/lib/libgpg* $(prefix)/release_neutrino-hd/usr/lib/
	cp -dp $(buildprefix)/root/etc/lircrc $(prefix)/release_neutrino-hd/etc/
	cp -dp $(buildprefix)/root/etc/lircd_spark.conf $(prefix)/release_neutrino-hd/etc/lircd.conf
	cp -dp $(targetprefix)/usr/bin/lircd $(prefix)/release_neutrino-hd/usr/bin/
	cp -dp $(targetprefix)/usr/bin/irexec $(prefix)/release_neutrino-hd/usr/bin/
#	cp -dp $(buildprefix)/root/etc/lircd_spark.conf.amiko $(prefix)/release_neutrino-hd/etc/lircd.conf.amiko
	cp -dp $(buildprefix)/root/usr/bin/functions.sh $(prefix)/release_neutrino-hd/usr/bin/
#	cp -dp $(buildprefix)/root/bin/evremote2.amiko $(prefix)/release_neutrino-hd/bin/
	cp -RP $(buildprefix)/root/etc/init.d/setupETH.sh $(prefix)/release_neutrino-hd/etc/init.d/
	cp -RP $(buildprefix)/root/etc/init.d/nfs* $(prefix)/release_neutrino-hd/etc/init.d/

if ENABLE_P0209
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/smartcard/smartcard.ko $(prefix)/release_neutrino-hd/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/wireless/rtl8192cu/8192cu.ko $(prefix)/release_neutrino-hd/lib/modules/rt8192cu.ko
	cp -f $(buildprefix)/root/lib/modules/rt8712u_stm24-209.ko $(prefix)/release_neutrino-hd/lib/modules/rt8712u.ko
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/wireless/rt2870sta/rt2870sta.ko $(prefix)/release_neutrino-hd/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/wireless/rt3070sta/rt3070sta.ko $(prefix)/release_neutrino-hd/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/wireless/rt5370sta/rt5370sta.ko $(prefix)/release_neutrino-hd/lib/modules/
endif
if ENABLE_P0210
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/smartcard/smartcard.ko $(prefix)/release_neutrino-hd/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/wireless/rtl8192cu/8192cu.ko $(prefix)/release_neutrino-hd/lib/modules/rt8192cu.ko
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/wireless/rtl871x/8712u.ko $(prefix)/release_neutrino-hd/lib/modules/rt8712u.ko
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/wireless/rt2870sta/rt2870sta.ko $(prefix)/release_neutrino-hd/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/wireless/rt3070sta/rt3070sta.ko $(prefix)/release_neutrino-hd/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/wireless/rt5370sta/rt5370sta.ko $(prefix)/release_neutrino-hd/lib/modules/
endif

#	[ -d $(targetprefix)/lib/modules/$(KERNELVERSION)/kernel/drivers/media/common/tuners ] && cp $(targetprefix)/lib/modules/$(KERNELVERSION)/kernel/drivers/media/common/tuners/* $(prefix)/release_neutrino-hd/lib/modules/
#	[ -d $(targetprefix)/lib/modules/$(KERNELVERSION)/kernel/drivers/media/dvb/dvb-usb ] && cp $(targetprefix)/lib/modules/$(KERNELVERSION)/kernel/drivers/media/dvb/dvb-usb/* $(prefix)/release_neutrino-hd/lib/modules/
#	[ -d $(targetprefix)/lib/modules/$(KERNELVERSION)/kernel/drivers/media/dvb/frontends ] && cp $(targetprefix)/lib/modules/$(KERNELVERSION)/kernel/drivers/media/dvb/frontends/* $(prefix)/release_neutrino-hd/lib/modules/
	[ -f $(targetprefix)/lib/modules/$(KERNELVERSION)/kernel/drivers/net/tun.ko ] && cp $(targetprefix)/lib/modules/$(KERNELVERSION)/kernel/drivers/net/tun.ko $(prefix)/release_neutrino-hd/lib/modules/

	cp -f $(buildprefix)/root/sbin/flash* $(prefix)/release_neutrino-hd/sbin
	cp -f $(buildprefix)/root/sbin/nand* $(prefix)/release_neutrino-hd/sbin
	[ -e $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/cpu_frequ/cpu_frequ.ko ] && cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/cpu_frequ/cpu_frequ.ko $(prefix)/release_neutrino-hd/lib/modules || true

	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stmfb.ko $(prefix)/release_neutrino-hd/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/multicom/embxshell/embxshell.ko $(prefix)/release_neutrino-hd/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/multicom/embxmailbox/embxmailbox.ko $(prefix)/release_neutrino-hd/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/multicom/embxshm/embxshm.ko $(prefix)/release_neutrino-hd/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/multicom/mme/mme_host.ko $(prefix)/release_neutrino-hd/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/bpamem/bpamem.ko $(prefix)/release_neutrino-hd/lib/modules/

	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/frontends/*.ko $(prefix)/release_neutrino-hd/lib/modules/

	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/compcache/lzo-kmod/lzo1x_compress.ko $(prefix)/release_neutrino-hd/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/compcache/lzo-kmod/lzo1x_decompress.ko $(prefix)/release_neutrino-hd/lib/modules/

	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/compcache/ramzswap.ko $(prefix)/release_neutrino-hd/lib/modules/

	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stm_v4l2.ko $(prefix)/release_neutrino-hd/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stmvbi.ko $(prefix)/release_neutrino-hd/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stmvout.ko $(prefix)/release_neutrino-hd/lib/modules/

	[ -e $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/pti/pti.ko ] && cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/pti/pti.ko $(prefix)/release_neutrino-hd/lib/modules/ || true
	[ -e $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/pti_np/pti.ko ] && cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/pti_np/pti.ko $(prefix)/release_neutrino-hd/lib/modules/ || true

	find $(prefix)/release_neutrino-hd/lib/modules/ -name '*.ko' -exec sh4-linux-strip --strip-unneeded {} \;
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
			cp player2/linux/drivers/$$mod $(prefix)/release_neutrino-hd/lib/modules/; \
			sh4-linux-strip --strip-unneeded $(prefix)/release_neutrino-hd/lib/modules/`basename $$mod`; \
		else \
			touch $(prefix)/release_neutrino-hd/lib/modules/`basename $$mod`; \
		fi;\
	done

	rm -rf $(prefix)/release_neutrino-hd/lib/modules/$(KERNELVERSION)

	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/media
	ln -sf /media/hdd $(prefix)/release_neutrino-hd/hdd
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/media/dvd

	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/mnt
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/mnt/usb
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/mnt/hdd
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/mnt/nfs

	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/root

	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/proc
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/sys
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/tmp

	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/usr
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/usr/bin
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/usr/lib
	cp -p $(targetprefix)/usr/sbin/vsftpd $(prefix)/release_neutrino-hd/usr/bin/

	cp -p $(targetprefix)/usr/bin/killall $(prefix)/release_neutrino-hd/usr/bin/
	cp -p $(targetprefix)/usr/sbin/ethtool $(prefix)/release_neutrino-hd/usr/sbin/

	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/usr/tuxtxt


#######################################################################################
	mkdir -p $(prefix)/release_neutrino-hd/tuxbox/config
	mkdir -p $(prefix)/release_neutrino-hd/var/plugins
	cp -aR $(buildprefix)/root/var/plugins/* $(prefix)/release_neutrino-hd/var/plugins/
	mkdir -p $(prefix)/release_neutrino-hd/lib/tuxbox
	mkdir -p $(prefix)/release_neutrino-hd/usr/lib/tuxbox
	mkdir -p $(prefix)/release_neutrino-hd/usr/keys
	mkdir -p $(prefix)/release_neutrino-hd/var/tuxbox #/config
	mkdir -p $(prefix)/release_neutrino-hd/var/run/lirc
	mkdir -p $(prefix)/release_neutrino-hd/share/tuxbox
	mkdir -p $(prefix)/release_neutrino-hd/var/share/icons
#	mkdir -p $(prefix)/release_neutrino-hd/var/usr/local/share/config
	mkdir -p $(prefix)/release_neutrino-hd/usr/local/share/neutrino/icons
#	mkdir -p $(prefix)/release_neutrino-hd/media/net
	( cd $(prefix)/release_neutrino-hd/media && ln -sf /mnt/nfs net )
#	( cd $(prefix)/release_neutrino-hd/var && ln -s ../usr/bin emu )
	( cd $(prefix)/release_neutrino-hd/var && ln -s ../usr/keys keys )
	( cd $(prefix)/release_neutrino-hd/var/tuxbox && ln -s ../../../usr/local/share/config )
#	( cd $(prefix)/release_neutrino-hd/var/tuxbox/config && ln -s ../../../usr/local/share/config/neutrino.conf )
#	( cd $(prefix)/release_neutrino-hd/var/tuxbox/config && ln -s ../../../usr/local/share/config/Sportster.conf )
#	( cd $(prefix)/release_neutrino-hd/var/tuxbox/config && ln -s ../../../usr/local/share/config/nhttpd.conf )
	( cd $(prefix)/release_neutrino-hd/share/tuxbox && ln -s /usr/local/share/neutrino )
	( cd $(prefix)/release_neutrino-hd/var/share/icons/ && ln -s /usr/local/share/neutrino/icons/logo )
	( cd $(prefix)/release_neutrino-hd/ && ln -s /usr/local/share/neutrino/icons/logo logos )
	( cd $(prefix)/release_neutrino-hd/usr/lib && ln -s libcrypto.so.0.9.8 libcrypto.so.0.9.7 )
	( cd $(prefix)/release_neutrino-hd/lib/tuxbox && ln -s /var/plugins )
	( cd $(prefix)/release_neutrino-hd/var/tuxbox && ln -s /var/plugins )
	( cd $(prefix)/release_neutrino-hd/usr/lib/tuxbox && ln -s /var/plugins )
	rm -rf $(prefix)/release_neutrino-hd/media/sda*
	cp -RP $(appsdir)/neutrino/data/icons/* $(prefix)/release_neutrino-hd/usr/local/share/neutrino/icons/
	rm $(prefix)/release_neutrino-hd/usr/local/share/neutrino/icons/Makefile*

#######################################################################################
#######################################################################################
#######################################################################################

	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/usr/share

#######################################################################################


#######################################################################################

#######################################################################################

	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/usr/share/zoneinfo
	cp -aR $(buildprefix)/root/usr/share/zoneinfo/* $(prefix)/release_neutrino-hd/usr/share/zoneinfo/

#######################################################################################

	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/usr/share/udhcpc
	cp -aR $(buildprefix)/root/usr/share/udhcpc/* $(prefix)/release_neutrino-hd/usr/share/udhcpc/


#######################################################################################
#######################################################################################
#######################################################################################
#######################################################################################

	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/usr/local

#######################################################################################

	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/usr/local/bin
	cp $(targetprefix)/usr/local/bin/neutrino $(prefix)/release_neutrino-hd/usr/local/bin/
	cp $(targetprefix)/usr/local/bin/pzapit $(prefix)/release_neutrino-hd/usr/local/bin/
	cp $(targetprefix)/usr/local/bin/mhwepg $(prefix)/release_neutrino-hd/usr/local/bin/
	cp $(targetprefix)/usr/local/bin/sectionsdcontrol $(prefix)/release_neutrino-hd/usr/local/bin/
	sh4-linux-strip --strip-unneeded $(prefix)/release_neutrino-hd/usr/local/bin/*
#	find $(prefix)/release_neutrino-hd/usr/local/bin/ -name  neutrino -exec sh4-linux-strip --strip-unneeded {} \;
#	find $(prefix)/release_neutrino-hd/usr/local/bin/ -name  pzapit -exec sh4-linux-strip --strip-unneeded {} \;
#	find $(prefix)/release_neutrino-hd/usr/local/bin/ -name  sectionsdcontrol -exec sh4-linux-strip --strip-unneeded {} \;

#######################################################################################

	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/usr/local/share
	cp -aR $(targetprefix)/usr/local/share/iso-codes $(prefix)/release_neutrino-hd/usr/local/share/
#	TODO: Channellist ....
	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/usr/local/share/config
	cp -aR $(buildprefix)/root/usr/local/share/config/* $(prefix)/release_neutrino-hd/usr/local/share/config/
	cp -aR $(targetprefix)/usr/local/share/neutrino $(prefix)/release_neutrino-hd/usr/local/share/
#	( cd $(prefix)/release_neutrino-hd/usr/local/share/config/tuxtxt/ && ln -sf tuxtxt2_720.conf tuxtxt2.conf )
	rm $(prefix)/release_neutrino-hd/usr/local/share/config/tuxtxt/tuxtxt2_*
#	TODO: HACK
	cp -aR $(targetprefix)/$(targetprefix)/usr/local/share/neutrino/* $(prefix)/release_neutrino-hd/usr/local/share/neutrino
#######################################################################################
#	cp -aR $(targetprefix)/usr/local/share/fonts $(prefix)/release_neutrino-hd/usr/local/share/
	mkdir -p $(prefix)/release_neutrino-hd/usr/local/share/fonts
	cp $(buildprefix)/root/usr/share/fonts/tuxtxt.ttf $(prefix)/release_neutrino-hd/usr/local/share/fonts/
	cp $(buildprefix)/root/usr/share/fonts/russian.ttf $(prefix)/release_neutrino-hd/usr/local/share/fonts/

#       Font libass
	mkdir -p $(prefix)/release_neutrino-hd/usr/share/fonts
	cp $(buildprefix)/root/usr/share/fonts/FreeSans.ttf $(prefix)/release_neutrino-hd/usr/share/fonts/

	cp -aR $(targetprefix)/usr/local/share/fonts/micron.ttf $(prefix)/release_neutrino-hd/usr/local/share/fonts/neutrino.ttf
	cp $(appsdir)/neutrino/src/nhttpd/web/{Y_Baselib.js,Y_VLC.js} $(prefix)/release_neutrino-hd/usr/local/share/neutrino/httpd-y/
#	rm $(prefix)/release_neutrino-hd/usr/local/share/neutrino/httpd-y/{Y_Baselib.js.gz,Y_VLC.js.gz}
#######################################################################################
	echo "pingulux-rev#: " > $(prefix)/release_neutrino-hd/etc/imageinfo
	git describe >> $(prefix)/release_neutrino-hd/etc/imageinfo
	$(buildprefix)/root/release/neutrino_version.sh
	rm -rf $(prefix)/release_neutrino-hd/var/etc

#	echo 'comment=created by pingulux-git' > $(buildprefix)/root/var/etc/.version.new
#	echo 'imagename=Neutrino for Pingulux' >> $(buildprefix)/root/var/etc/.version.new
#	echo 'homepage=http://gitorious.org/open-duckbox-project-sh4/pingulux-git' >> $(buildprefix)/root/var/etc/.version.new
#	echo 'creator=git-developer' >> $(buildprefix)/root/var/etc/.version.new
#	echo 'docs=http://gitorious.org/open-duckbox-project-sh4/pingulux-git/commits' >> $(buildprefix)/root/var/etc/.version.new
#	echo 'forum=http://kathif.vs120005.hl-users.com/index.php?sid=' >> $(buildprefix)/root/var/etc/.version.new
	cp $(buildprefix)/root/var/etc/.version.new $(prefix)/release_neutrino-hd/etc/.version
	ln -s ../etc $(prefix)/release_neutrino-hd/var
	ln -sf ../etc/.version $(prefix)/release_neutrino-hd
#######################################################################################

	$(INSTALL_DIR) $(prefix)/release_neutrino-hd/usr/lib
	rm -rf $(prefix)/release_neutrino-hd/usr/lib/tuxbox
	cp -R $(targetprefix)/usr/lib/* $(prefix)/release_neutrino-hd/usr/lib/
	rm -rf $(prefix)/release_neutrino-hd/usr/lib/libnfsidmap
	cp -R $(targetprefix)/usr/local/lib/* $(prefix)/release_neutrino-hd/usr/lib/
	rm -rf $(prefix)/release_neutrino-hd/usr/lib/alsa-lib
	rm -rf $(prefix)/release_neutrino-hd/usr/lib/alsaplayer
	rm -rf $(prefix)/release_neutrino-hd/usr/lib/engines
	rm -rf $(prefix)/release_neutrino-hd/usr/lib/enigma2
	rm -rf $(prefix)/release_neutrino-hd/usr/lib/gconv
	mkdir -m 0755 $(prefix)/release_neutrino-hd/usr/lib/gconv
	cp -p $(targetprefix)/usr/lib/gconv/gconv-modules $(targetprefix)/usr/lib/gconv/ISO8859-1.so $(prefix)/release_neutrino-hd/usr/lib/gconv
	rm -rf $(prefix)/release_neutrino-hd/usr/lib/libxslt-plugins
	rm -rf $(prefix)/release_neutrino-hd/usr/lib/pkgconfig
	rm -rf $(prefix)/release_neutrino-hd/usr/lib/sigc++-1.2
	rm -rf $(prefix)/release_neutrino-hd/usr/lib/X11
	mkdir -p $(prefix)/release_neutrino-hd/usr/local/share/neutrino/icons/logo
	( cd $(prefix)/release_neutrino-hd/usr/local/share/neutrino/httpd-y && ln -s /usr/local/share/neutrino/icons/logo )
	( cd $(prefix)/release_neutrino-hd/usr/local/share/neutrino/httpd-y && ln -s /usr/local/share/neutrino/icons/logo logos )
	( cd $(prefix)/release_neutrino-hd/usr/local/share/neutrino && ln -s /usr/local/share/neutrino/httpd-y httpd )
	( cd $(prefix)/release_neutrino-hd/var && ln -s /usr/local/share/neutrino/httpd-y httpd )
	find $(prefix)/release_neutrino-hd/usr/lib/ -name '*.so*' -exec sh4-linux-strip --strip-unneeded {} \;
	touch $(prefix)/release_neutrino-hd/etc/.fsck
	cp -f $(buildprefix)/root/etc/init.d/Swap.sh $(prefix)/release_neutrino-hd/etc/init.d/
	( cd $(prefix) && cd ../flash/spark/orig-spark-plugin/root/plugin/var/etc && cp * $(prefix)/release_neutrino-hd/etc/ )
	cp -f $(buildprefix)/root/bin/fw_printenv $(prefix)/release_neutrino-hd/bin/
	cp -f $(buildprefix)/root/bin/fw_setenv $(prefix)/release_neutrino-hd/bin/

#	cp -f $(buildprefix)/root/release/auto.usb $(prefix)/release_neutrino-hd/etc/
#	echo 'sda    -fstype=auto,noatime,nodiratime          :/dev/sda' >> $(prefix)/release_neutrino-hd/etc/auto.usb
#	echo 'sda1   -fstype=auto,noatime,nodiratime          :/dev/sda1' >> $(prefix)/release_neutrino-hd/etc/auto.usb
#	echo 'sda2   -fstype=auto,noatime,nodiratime          :/dev/sda2' >> $(prefix)/release_neutrino-hd/etc/auto.usb
#	echo 'sda3   -fstype=auto,noatime,nodiratime          :/dev/sda3' >> $(prefix)/release_neutrino-hd/etc/auto.usb
#
######## FOR YOUR OWN CHANGES use these folder in cdk/own_build/neutrino #############
#	rm $(prefix)/release_neutrino-hd/bin/mount
	cp -RP $(buildprefix)/own_build/neutrino/* $(prefix)/release_neutrino-hd/

#	cp $(kernelprefix)/$(kernelpath)/arch/sh/boot/uImage $(prefix)/release_neutrino-hd/boot/

	cp -dp $(buildprefix)/root/usr/sbin/blkid $(prefix)/release_neutrino-hd/usr/bin/
	cp -dp $(targetprefix)/usr/bin/rdate $(prefix)/release_neutrino-hd/usr/bin/
	cp -RP $(buildprefix)/root/bin/fbshot $(prefix)/release_neutrino-hd/bin/
#	cp -RP $(buildprefix)/root/bin/lcd4linux $(prefix)/release_neutrino-hd/bin/
	cp -RP $(buildprefix)/root/usr/lib/libusb* $(prefix)/release_neutrino-hd/usr/lib/
#	cp -RP $(buildprefix)/root/usr/lib/libiconv* $(prefix)/release_neutrino-hd/usr/lib/
#	cp -RP $(buildprefix)/root/usr/lib/libgd* $(prefix)/release_neutrino-hd/usr/lib/
	cp -RP $(buildprefix)/root/usr/lib/libpng* $(prefix)/release_neutrino-hd/usr/lib/
	( cd $(prefix)/release_neutrino-hd/usr/lib && ln -sf libjpeg.so.8.4.0 libjpeg.so.62 )
#	cp -RP $(buildprefix)/root/etc/init.d/extDisplay.sh $(prefix)/release_neutrino-hd/etc/init.d/
#	cp -RP $(buildprefix)/root/etc/lcd4linux.conf $(prefix)/release_neutrino-hd/etc/
	cp -RP $(targetprefix)/usr/lib/tuxbox/plugins/* $(prefix)/release_neutrino-hd/var/plugins/
	rm -rf $(prefix)/release_neutrino-hd/usr/lib/tuxbox/plugins/*
	rm $(prefix)/release_neutrino-hd/var/plugins/*.la
	cp -RP $(targetprefix)/usr/var/tuxbox/config/* $(prefix)/release_neutrino-hd/usr/local/share/config/
#	cp -RP $(targetprefix)/usr/bin/blockad $(prefix)/release_neutrino-hd/bin/
	rm $(prefix)/release_neutrino-hd/var/plugins/blockad*
#	rm $(prefix)/release_neutrino-hd/usr/lib/tuxbox/plugins/blockad*
	rm $(prefix)/release_neutrino-hd/usr/local/share/config/blockad*
	cp -f $(buildprefix)/root/usr/local/share/config/zapit/supplemental_pids.conf $(prefix)/release_neutrino-hd/usr/local/share/config/zapit/
	touch $(prefix)/release_neutrino-hd/etc/.EVOfirstboot
	cp -f $(buildprefix)/root/etc/modules.available $(prefix)/release_neutrino-hd/etc/
if ENABLE_MULTI_YAFFS2
	cp -RP $(buildprefix)/root/sbin/mkyaffs2 $(prefix)/release_neutrino-hd/sbin/
	cp -RP $(buildprefix)/root/sbin/unspare2 $(prefix)/release_neutrino-hd/sbin/
endif
	[ -e $(kernelprefix)/$(kernelpath)/drivers/net/wireless/ath/ath.ko ] && cp $(kernelprefix)/$(kernelpath)/drivers/net/wireless/ath/ath.ko $(prefix)/release_neutrino-hd/lib/modules/ath.ko || true
	[ -e $(kernelprefix)/$(kernelpath)/drivers/net/wireless/zd1201.ko ] && cp $(kernelprefix)/$(kernelpath)/drivers/net/wireless/zd1201.ko $(prefix)/release_neutrino-hd/lib/modules/zd1201.ko || true
	[ -e $(kernelprefix)/$(kernelpath)/drivers/usb/serial/ftdi_sio.ko ] && cp $(kernelprefix)/$(kernelpath)/drivers/usb/serial/ftdi_sio.ko $(prefix)/release_neutrino-hd/lib/modules/ftdi_sio.ko || true
	[ -e $(kernelprefix)/$(kernelpath)/drivers/usb/serial/pl2303.ko ] && cp $(kernelprefix)/$(kernelpath)/drivers/usb/serial/pl2303.ko $(prefix)/release_neutrino-hd/lib/modules || true
	[ -e $(kernelprefix)/$(kernelpath)/drivers/usb/serial/usbserial.ko ] && cp $(kernelprefix)/$(kernelpath)/drivers/usb/serial/usbserial.ko $(prefix)/release_neutrino-hd/lib/modules || true
#	[ -e $(kernelprefix)/$(kernelpath)/fs/ntfs/ntfs.ko ] && cp $(kernelprefix)/$(kernelpath)/fs/ntfs/ntfs.ko $(prefix)/release_neutrino-hd/lib/modules || true
	[ -e $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/cpu_frequ/cpu_frequ.ko ] && cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/cpu_frequ/cpu_frequ.ko $(prefix)/release_neutrino-hd/lib/modules || true
	[ -e cp $(targetprefix)/lib/modules/$(KERNELVERSION)/kernel/fs/autofs4/autofs4.ko ] && $(targetprefix)/lib/modules/$(KERNELVERSION)/kernel/fs/autofs4/autofs4.ko $(prefix)/release_neutrino-hd/lib/modules/ || true

	find $(prefix)/release_neutrino-hd/ -name '*.a' -exec rm -f {} \;
	find $(prefix)/release_neutrino-hd/ -name '*.o' -exec rm -f {} \;
	find $(prefix)/release_neutrino-hd/ -name '*.la' -exec rm -f {} \;
#
# AUTOFS
#
#	cp -f $(targetprefix)/usr/sbin/automount $(prefix)/release_neutrino-hd/usr/sbin/; \
#	ln -sf /usr/lib/autofs/mount_ext2.so $(prefix)/release_neutrino-hd/usr/lib/autofs/mount_ext3.so; \
#	cp $(kernelprefix)/$(kernelpath)/fs/autofs4/autofs4.ko $(prefix)/release_neutrino-hd/lib/modules; \
#	cp -f $(buildprefix)/root/release/auto.usb $(prefix)/release_neutrino-hd/etc/;
	cp -f $(buildprefix)/root/release/autofs $(prefix)/release_neutrino-hd/etc/init.d/
	cp -f $(buildprefix)/root/usr/sbin/automount $(prefix)/release_neutrino-hd/usr/sbin/
	cp -f $(buildprefix)/root/release/auto.hotplug $(prefix)/release_neutrino-hd/etc/
	cp -f $(buildprefix)/root/release/auto.network $(prefix)/release_neutrino-hd/etc/

#graphlcd Stuff
	if [ -e $(prefix)/release_neutrino-hd/usr/lib/libglcddrivers.so ]; then \
		( cd $(prefix)/release_neutrino-hd/usr/lib && ln -sf libglcddrivers.so.2.1.0 libglcddrivers.so && ln -sf libglcddrivers.so.2.1.0 libglcddrivers.so.2 && ln -sf libglcdgraphics.so.2.1.0 libglcdgraphics.so && ln -sf libglcdgraphics.so.2.1.0 libglcdgraphics.so.2 &&  ln -sf libglcdskin.so.2.1.0 libglcdskin.so &&  ln -sf libglcdskin.so.2.1.0 libglcdskin.so.2 ); \
		cp -f $(targetprefix)/etc/graphlcd.conf $(prefix)/release_neutrino-hd/etc/graphlcd.conf; \
	fi

	touch $@
