# auxiliary targets for model-specific builds
release-enigma2-pli-nightly_common_utils:
#       remove the slink to busybox
	rm -f $(prefix)/release-enigma2-pli-nightly/sbin/halt
	cp -f $(targetprefix)/sbin/halt $(prefix)/release-enigma2-pli-nightly/sbin/
	cp $(buildprefix)/root/release/umountfs $(prefix)/release-enigma2-pli-nightly/etc/init.d/
	cp $(buildprefix)/root/release/rc $(prefix)/release-enigma2-pli-nightly/etc/init.d/
	cp $(buildprefix)/root/release/sendsigs $(prefix)/release-enigma2-pli-nightly/etc/init.d/
	chmod 755 $(prefix)/release-enigma2-pli-nightly/etc/init.d/umountfs
	chmod 755 $(prefix)/release-enigma2-pli-nightly/etc/init.d/rc
	chmod 755 $(prefix)/release-enigma2-pli-nightly/etc/init.d/sendsigs
	chmod 755 $(prefix)/release-enigma2-pli-nightly/etc/init.d/halt
	mkdir -p $(prefix)/release-enigma2-pli-nightly/etc/rc.d/rc0.d
	ln -s ../init.d $(prefix)/release-enigma2-pli-nightly/etc/rc.d
	ln -fs halt $(prefix)/release-enigma2-pli-nightly/sbin/reboot
	ln -fs halt $(prefix)/release-enigma2-pli-nightly/sbin/poweroff
	ln -s ../init.d/sendsigs $(prefix)/release-enigma2-pli-nightly/etc/rc.d/rc0.d/S20sendsigs
	ln -s ../init.d/umountfs $(prefix)/release-enigma2-pli-nightly/etc/rc.d/rc0.d/S40umountfs
	ln -s ../init.d/halt $(prefix)/release-enigma2-pli-nightly/etc/rc.d/rc0.d/S90halt
	mkdir -p $(prefix)/release-enigma2-pli-nightly/etc/rc.d/rc6.d
	ln -s ../init.d/sendsigs $(prefix)/release-enigma2-pli-nightly/etc/rc.d/rc6.d/S20sendsigs
	ln -s ../init.d/umountfs $(prefix)/release-enigma2-pli-nightly/etc/rc.d/rc6.d/S40umountfs
	ln -s ../init.d/reboot $(prefix)/release-enigma2-pli-nightly/etc/rc.d/rc6.d/S90reboot

release-enigma2-pli-nightly_spark:
	echo "EvoPLI" > $(prefix)/release-enigma2-pli-nightly/etc/hostname

	cp $(buildprefix)/root/release/umountfs $(prefix)/release-enigma2-pli-nightly/etc/init.d/
	cp $(buildprefix)/root/release/rc $(prefix)/release-enigma2-pli-nightly/etc/init.d/
	cp $(buildprefix)/root/release/sendsigs $(prefix)/release-enigma2-pli-nightly/etc/init.d/
	rm -f $(prefix)/release-enigma2-pli-nightly/sbin/halt
	cp -f $(targetprefix)/sbin/halt $(prefix)/release-enigma2-pli-nightly/sbin/
	cp $(buildprefix)/root/release/halt_spark_enigma2 $(prefix)/release-enigma2-pli-nightly/etc/init.d/halt
	chmod 755 $(prefix)/release-enigma2-pli-nightly/etc/init.d/umountfs
	chmod 755 $(prefix)/release-enigma2-pli-nightly/etc/init.d/rc
	chmod 755 $(prefix)/release-enigma2-pli-nightly/etc/init.d/sendsigs
	chmod 755 $(prefix)/release-enigma2-pli-nightly/etc/init.d/halt
	mkdir -p $(prefix)/release-enigma2-pli-nightly/usr/script
	mkdir -p $(prefix)/release-enigma2-pli-nightly/etc/rc.d/rc0.d
	ln -s ../init.d $(prefix)/release-enigma2-pli-nightly/etc/rc.d
	ln -fs halt $(prefix)/release-enigma2-pli-nightly/sbin/reboot
	ln -fs halt $(prefix)/release-enigma2-pli-nightly/sbin/poweroff
	ln -s ../init.d/sendsigs $(prefix)/release-enigma2-pli-nightly/etc/rc.d/rc0.d/S20sendsigs
	ln -s ../init.d/umountfs $(prefix)/release-enigma2-pli-nightly/etc/rc.d/rc0.d/S40umountfs
	ln -s ../init.d/halt $(prefix)/release-enigma2-pli-nightly/etc/rc.d/rc0.d/S90halt
	mkdir -p $(prefix)/release-enigma2-pli-nightly/etc/rc.d/rc6.d
	ln -s ../init.d/sendsigs $(prefix)/release-enigma2-pli-nightly/etc/rc.d/rc6.d/S20sendsigs
	ln -s ../init.d/umountfs $(prefix)/release-enigma2-pli-nightly/etc/rc.d/rc6.d/S40umountfs
	ln -s ../init.d/reboot $(prefix)/release-enigma2-pli-nightly/etc/rc.d/rc6.d/S90reboot
	ln -fs init $(prefix)/release-enigma2-pli-nightly/sbin/telinit

	cp -f $(buildprefix)/root/release/fstab_spark $(prefix)/release-enigma2-pli-nightly/etc/fstab
	cp $(buildprefix)/root/boot/startup.mp4 $(prefix)/release-enigma2-pli-nightly/boot/startup.mp4

	mkdir -p $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2/picon
	cp -f $(buildprefix)/root/usr/local/share/enigma2/keymap_spark.xml $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2/keymap.xml
	cp -f $(buildprefix)/root/usr/sbin/iw* $(prefix)/release-enigma2-pli-nightly/usr/sbin/
	cp -f $(buildprefix)/root/usr/sbin/wpa* $(prefix)/release-enigma2-pli-nightly/usr/sbin/
	cp -RP $(buildprefix)/root/etc/Wireless $(prefix)/release-enigma2-pli-nightly/etc/
	cp -f $(buildprefix)/root/usr/lib/libiw.so.29 $(prefix)/release-enigma2-pli-nightly/usr/lib/
	cp -f $(buildprefix)/root/usr/lib/libgnu* $(prefix)/release-enigma2-pli-nightly/usr/lib/
	cp -f $(buildprefix)/root/usr/lib/libgcrypt* $(prefix)/release-enigma2-pli-nightly/usr/lib/
	cp -f $(buildprefix)/root/usr/lib/libgpg* $(prefix)/release-enigma2-pli-nightly/usr/lib/
	cp -RP $(buildprefix)/root/usr/lib/libusb* $(prefix)/release-enigma2-pli-nightly/usr/lib/
	cp -RP $(buildprefix)/root/usr/lib/libiconv* $(prefix)/release-enigma2-pli-nightly/usr/lib/
	cp -RP $(buildprefix)/root/usr/lib/libpng* $(prefix)/release-enigma2-pli-nightly/usr/lib/
	cp -RP $(buildprefix)/root/etc/init.d/setupETH.sh $(prefix)/release-enigma2-pli-nightly/etc/init.d/
	cp -RP $(buildprefix)/root/etc/init.d/nfs_speedtest.sh $(prefix)/release-enigma2-pli-nightly/etc/init.d/

if ENABLE_P0209
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/smartcard/smartcard.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/wireless/rtl8192cu/8192cu.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/rt8192cu.ko
	cp -f $(buildprefix)/root/lib/modules/rt8712u_stm24-209.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/rt8712u.ko
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/wireless/rt2870sta/rt2870sta.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/wireless/rt3070sta/rt3070sta.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/wireless/rt5370sta/rt5370sta.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/
endif
if ENABLE_P0210
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/smartcard/smartcard.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/wireless/rtl871x/8712u.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/rt8712u.ko
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/wireless/rtl8192cu/8192cu.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/rt8192cu.ko
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/wireless/rt2870sta/rt2870sta.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/wireless/rt3070sta/rt3070sta.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/wireless/rt5370sta/rt5370sta.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/
endif

	cp -f $(buildprefix)/root/sbin/flash* $(prefix)/release-enigma2-pli-nightly/sbin
	cp -f $(buildprefix)/root/sbin/nand* $(prefix)/release-enigma2-pli-nightly/sbin

	cp -dp $(buildprefix)/root/etc/lircrc $(prefix)/release-enigma2-pli-nightly/etc/
	cp -dp $(buildprefix)/root/etc/lircd_spark.conf $(prefix)/release-enigma2-pli-nightly/etc/lircd.conf
	cp -dp $(buildprefix)/root/etc/lircd_spark.conf.amiko $(prefix)/release-enigma2-pli-nightly/etc/lircd.conf.amiko
	cp -dp $(buildprefix)/root/bin/evremote2.amiko $(prefix)/release-enigma2-pli-nightly/bin/
	cp -dp $(buildprefix)/root/usr/bin/functions.sh $(prefix)/release-enigma2-pli-nightly/usr/bin/
	cp -dp $(targetprefix)/usr/bin/lircd $(prefix)/release-enigma2-pli-nightly/usr/bin/
	cp -dp $(targetprefix)/usr/bin/irexec $(prefix)/release-enigma2-pli-nightly/usr/bin/

	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/usr/share/fonts
	cp $(targetprefix)/usr/local/share/fonts/* $(prefix)/release-enigma2-pli-nightly/usr/share/fonts/

	cp $(kernelprefix)/linux-sh4/drivers/usb/serial/ftdi_sio.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/ftdi_sio.ko
	cp $(kernelprefix)/linux-sh4/drivers/usb/serial/pl2303.ko $(prefix)/release-enigma2-pli-nightly/lib/modules
	cp $(kernelprefix)/linux-sh4/drivers/usb/serial/usbserial.ko $(prefix)/release-enigma2-pli-nightly/lib/modules

	[ -e $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/cpu_frequ/cpu_frequ.ko ] && cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/cpu_frequ/cpu_frequ.ko $(prefix)/release-enigma2-pli-nightly/lib/modules || true

#	cp -f $(buildprefix)/root/release/auto.usb $(prefix)/release-enigma2-pli-nightly/etc/
#	echo 'sda    -fstype=auto,noatime,nodiratime          :/dev/sda' >> $(prefix)/release-enigma2-pli-nightly/etc/auto.usb
#	echo 'sda1   -fstype=auto,noatime,nodiratime          :/dev/sda1' >> $(prefix)/release-enigma2-pli-nightly/etc/auto.usb
#	echo 'sda2   -fstype=auto,noatime,nodiratime          :/dev/sda2' >> $(prefix)/release-enigma2-pli-nightly/etc/auto.usb
#	echo 'sda3   -fstype=auto,noatime,nodiratime          :/dev/sda3' >> $(prefix)/release-enigma2-pli-nightly/etc/auto.usb

	rm -f $(prefix)/release-enigma2-pli-nightly/lib/firmware/dvb-fe-avl2108.fw
	rm -f $(prefix)/release-enigma2-pli-nightly/lib/firmware/dvb-fe-stv6306.fw
	rm -f $(prefix)/release-enigma2-pli-nightly/lib/firmware/dvb-fe-cx24116.fw
	rm -f $(prefix)/release-enigma2-pli-nightly/lib/firmware/dvb-fe-cx21143.fw
	rm -f $(prefix)/release-enigma2-pli-nightly/bin/evremote
	rm -f $(prefix)/release-enigma2-pli-nightly/bin/gotosleep
	rm -f $(prefix)/release-enigma2-pli-nightly/bin/vdstandby

	mv $(prefix)/release-enigma2-pli-nightly/lib/firmware/component_7111_mb618.fw $(prefix)/release-enigma2-pli-nightly/lib/firmware/component.fw
	rm $(prefix)/release-enigma2-pli-nightly/lib/firmware/component_7105_pdk7105.fw
	cp -RP $(buildprefix)/root/lib/firmware/* $(prefix)/release-enigma2-pli-nightly/lib/firmware/

	cp -f $(buildprefix)/root/usr/local/share/enigma2/po/de/LC_MESSAGES/enigma2.mo.pingulux $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2/po/de/LC_MESSAGES/enigma2.mo
	cp -f $(buildprefix)/root/usr/bin/mkfs.jffs2 $(prefix)/release-enigma2-pli-nightly/usr/bin/
	cp -f $(buildprefix)/root/sbin/mkyaffs2 $(prefix)/release-enigma2-pli-nightly/sbin/
	( cd $(prefix) && cp -RP ../flash/spark/spark_oob.img $(prefix)/release-enigma2-pli-nightly/sbin/ )
	cp -RP $(buildprefix)/root/usr/lib/liblzo2.so.2* $(prefix)/release-enigma2-pli-nightly/usr/lib/
	cp -RP $(buildprefix)/root/lib/libproc* $(prefix)/release-enigma2-pli-nightly/lib/
	cp -RP $(buildprefix)/root/sbin/ntpdate $(prefix)/release-enigma2-pli-nightly/sbin/
	rm $(prefix)/release-enigma2-pli-nightly/bin/ps
	mkdir -p $(prefix)/release-enigma2-pli-nightly/usr/lib/opkg/info
	touch $(prefix)/release-enigma2-pli-nightly/usr/lib/opkg/info/opkg.list
	cp -RP $(buildprefix)/root/bin/ps $(prefix)/release-enigma2-pli-nightly/bin/
	cp -RP $(buildprefix)/root/bin/dropbear $(prefix)/release-enigma2-pli-nightly/bin/
	cp -RP $(buildprefix)/root/bin/dropbearkey $(prefix)/release-enigma2-pli-nightly/bin/
	cp -RP $(buildprefix)/root/etc/init.d/dropbear $(prefix)/release-enigma2-pli-nightly/etc/init.d/
	mkdir -p $(prefix)/release-enigma2-pli-nightly/etc/dropbear
	cp -RP $(buildprefix)/root/usr/lib/python2.6/site-packages/* $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/site-packages/
	cp -f $(buildprefix)/root/usr/bin/backup_pingulux_image.sh $(prefix)/release-enigma2-pli-nightly/usr/bin/
	cp -f $(buildprefix)/root/etc/init.d/Swap.sh $(prefix)/release-enigma2-pli-nightly/etc/init.d/
	cp -f $(buildprefix)/root/etc/init.d/ntpupdate.sh $(prefix)/release-enigma2-pli-nightly/etc/init.d/
	( cd $(prefix) && cd ../flash/spark/orig-spark-plugin/root/plugin/var/etc && cp * $(prefix)/release-enigma2-pli-nightly/etc/ )
	cp -f $(buildprefix)/root/bin/fw_printenv $(prefix)/release-enigma2-pli-nightly/bin/
	cp -f $(buildprefix)/root/bin/fw_setenv $(prefix)/release-enigma2-pli-nightly/bin/
	touch $(prefix)/release-enigma2-pli-nightly/etc/.fsck
	if [ ! -e $(prefix)/release-enigma2-pli-nightly/etc/rc.d/rc3.d ]; then \
		mkdir -p $(prefix)/release-enigma2-pli-nightly/etc/rc.d/rc3.d; \
	fi; 
	ln -sf ../init.d/Swap.sh $(prefix)/release-enigma2-pli-nightly/etc/rc.d/rc3.d/S51swap
	ln -sf ../init.d/ntpupdate.sh $(prefix)/release-enigma2-pli-nightly/etc/rc.d/rc3.d/S52ntpupdate
	cp -RP $(buildprefix)/root/etc/mumudvb $(prefix)/release-enigma2-pli-nightly/etc/
	cp -RP $(buildprefix)/root/bin/mumudvb $(prefix)/release-enigma2-pli-nightly/bin/
### del libcoolstream+libeplayer2 stuff as not needed for evolux-E2 ###
	rm -f $(prefix)/release-enigma2-pli-nightly/lib/libcool*
	rm -f $(prefix)/release-enigma2-pli-nightly/lib/libeplayer2*
# copy evo mc files ####
#	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2/skin_default
#	cp -RP $(buildprefix)/root/usr/local/share/enigma2/skin_default $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2/
#	cp -RP $(buildprefix)/root/usr/local/share/enigma2/skin_default.xml $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2/
#	cp -RP $(buildprefix)/root/usr/local/share/enigma2/skin.xml $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2/
#	cp -RP $(buildprefix)/root/usr/local/share/enigma2/prev.png $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2/
#	cp -RP $(buildprefix)/root/usr/local/share/enigma2/EVO-blackGlass-HD $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2/
#	cp -RP $(buildprefix)/root/usr/local/share/enigma2/Vali.HD.nano $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2/


#
# The main target depends on the model.
# IMPORTANT: it is assumed that only one variable is set. Otherwise the target name won't be resolved.
#
$(DEPDIR)/min-release-enigma2-pli-nightly $(DEPDIR)/std-release-enigma2-pli-nightly $(DEPDIR)/max-release-enigma2-pli-nightly $(DEPDIR)/ipk-release-enigma2-pli-nightly $(DEPDIR)/release-enigma2-pli-nightly: \
$(DEPDIR)/%release-enigma2-pli-nightly: release-enigma2-pli-nightly_base release-enigma2-pli-nightly_$(SPARK)$(SPARK7162)
	touch $@

release-enigma2-pli-nightly-clean:
	rm -f $(DEPDIR)/release-enigma2-pli-nightly
	rm -f $(DEPDIR)/release-enigma2-pli-nightly_base
	rm -f $(DEPDIR)/release-enigma2-pli-nightly_$(SPARK)
	rm -f $(DEPDIR)/release-enigma2-pli-nightly_common_utils
	rm -f $(DEPDIR)/release-enigma2-pli-nightly_cube_common

# the following target creates the common file base
release-enigma2-pli-nightly_base:
	rm -rf $(prefix)/release-enigma2-pli-nightly || true
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/bin && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/sbin && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/boot && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/dev && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/dev.static && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/etc && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/etc/fonts && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/etc/init.d && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/etc/network && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/etc/network/if-down.d && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/etc/network/if-post-down.d && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/etc/network/if-pre-up.d && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/etc/network/if-up.d && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/etc/tuxbox && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/etc/enigma2 && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/hdd && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/hdd/movie && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/hdd/music && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/hdd/picture && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/lib && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/lib/modules && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/ram && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/var && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/var/opkg && \
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2/po/de/LC_MESSAGES && \
	export CROSS_COMPILE=$(target)- && \
		$(MAKE) install -C @DIR_busybox@ CONFIG_PREFIX=$(prefix)/release-enigma2-pli-nightly && \
	ln -s  ../etc $(prefix)/release-enigma2-pli-nightly/var/etc && \
	cp -a $(targetprefix)/bin/* $(prefix)/release-enigma2-pli-nightly/bin/ && \
	cp -dp $(targetprefix)/bin/hotplug $(prefix)/release-enigma2-pli-nightly/sbin/ && \
	cp -dp $(targetprefix)/usr/bin/sdparm $(prefix)/release-enigma2-pli-nightly/sbin/ && \
	cp -dp $(targetprefix)/sbin/init $(prefix)/release-enigma2-pli-nightly/sbin/ && \
	cp -dp $(targetprefix)/sbin/killall5 $(prefix)/release-enigma2-pli-nightly/sbin/ && \
	cp -dp $(targetprefix)/sbin/portmap $(prefix)/release-enigma2-pli-nightly/sbin/ && \
	cp -dp $(targetprefix)/sbin/mke2fs $(prefix)/release-enigma2-pli-nightly/sbin/ && \
	ln -sf /sbin/mke2fs $(prefix)/release-enigma2-pli-nightly/sbin/mkfs.ext2 && \
	ln -sf /sbin/mke2fs $(prefix)/release-enigma2-pli-nightly/sbin/mkfs.ext3 && \
	ln -sf /sbin/mke2fs $(prefix)/release-enigma2-pli-nightly/sbin/mkfs.ext4 && \
	ln -sf /sbin/mke2fs $(prefix)/release-enigma2-pli-nightly/sbin/mkfs.ext4dev && \
	cp -dp $(targetprefix)/sbin/fsck $(prefix)/release-enigma2-pli-nightly/sbin/ && \
	cp -dp $(targetprefix)/sbin/e2fsck $(prefix)/release-enigma2-pli-nightly/sbin/ && \
	ln -sf /sbin/e2fsck $(prefix)/release-enigma2-pli-nightly/sbin/fsck.ext2 && \
	ln -sf /sbin/e2fsck $(prefix)/release-enigma2-pli-nightly/sbin/fsck.ext3 && \
	ln -sf /sbin/e2fsck $(prefix)/release-enigma2-pli-nightly/sbin/fsck.ext4 && \
	ln -sf /sbin/e2fsck $(prefix)/release-enigma2-pli-nightly/sbin/fsck.ext4dev && \
	cp -dp $(targetprefix)/sbin/jfs_fsck $(prefix)/release-enigma2-pli-nightly/sbin/ && \
	ln -sf /sbin/jfs_fsck $(prefix)/release-enigma2-pli-nightly/sbin/fsck.jfs && \
	cp -dp $(targetprefix)/sbin/jfs_mkfs $(prefix)/release-enigma2-pli-nightly/sbin/ && \
	ln -sf /sbin/jfs_mkfs $(prefix)/release-enigma2-pli-nightly/sbin/mkfs.jfs && \
	cp -dp $(targetprefix)/sbin/jfs_tune $(prefix)/release-enigma2-pli-nightly/sbin/ && \
	cp -dp $(targetprefix)/sbin/fsck.nfs $(prefix)/release-enigma2-pli-nightly/sbin/ && \
	cp -dp $(targetprefix)/sbin/sfdisk $(prefix)/release-enigma2-pli-nightly/sbin/ && \
	cp -dp $(targetprefix)/sbin/tune2fs $(prefix)/release-enigma2-pli-nightly/sbin/ && \
	cp -dp $(targetprefix)/etc/init.d/portmap $(prefix)/release-enigma2-pli-nightly/etc/init.d/ && \
	cp -dp $(buildprefix)/root/etc/init.d/udhcpc $(prefix)/release-enigma2-pli-nightly/etc/init.d/ && \
	cp -a $(targetprefix)/dev/* $(prefix)/release-enigma2-pli-nightly/dev/ && \
	cp -dp $(targetprefix)/etc/fstab $(prefix)/release-enigma2-pli-nightly/etc/ && \
	cp -dp $(targetprefix)/etc/group $(prefix)/release-enigma2-pli-nightly/etc/ && \
	cp -dp $(targetprefix)/etc/host.conf $(prefix)/release-enigma2-pli-nightly/etc/ && \
	cp -dp $(targetprefix)/etc/hostname $(prefix)/release-enigma2-pli-nightly/etc/ && \
	cp -dp $(targetprefix)/etc/hosts $(prefix)/release-enigma2-pli-nightly/etc/ && \
	cp -dp $(targetprefix)/etc/inittab $(prefix)/release-enigma2-pli-nightly/etc/ && \
	cp -dp $(targetprefix)/etc/localtime $(prefix)/release-enigma2-pli-nightly/etc/ && \
	cp -dp $(targetprefix)/etc/mtab $(prefix)/release-enigma2-pli-nightly/etc/ && \
	cp -dp $(targetprefix)/etc/passwd $(prefix)/release-enigma2-pli-nightly/etc/ && \
	cp -dp $(targetprefix)/etc/profile $(prefix)/release-enigma2-pli-nightly/etc/ && \
	cp -dp $(targetprefix)/etc/protocols $(prefix)/release-enigma2-pli-nightly/etc/ && \
	cp -dp $(targetprefix)/etc/resolv.conf $(prefix)/release-enigma2-pli-nightly/etc/ && \
	cp -dp $(targetprefix)/etc/services $(prefix)/release-enigma2-pli-nightly/etc/ && \
	cp -dp $(targetprefix)/etc/shells $(prefix)/release-enigma2-pli-nightly/etc/ && \
	cp -dp $(targetprefix)/etc/shells.conf $(prefix)/release-enigma2-pli-nightly/etc/ && \
	cp -dp $(targetprefix)/etc/vsftpd.conf $(prefix)/release-enigma2-pli-nightly/etc/ && \
	sed "s@listen=YES@listen=NO@g" -i $(prefix)/release-enigma2-pli-nightly/etc/vsftpd.conf && \
	echo "8001 stream tcp nowait root /bin/streamproxy streamproxy" > $(prefix)/release-enigma2-pli-nightly/etc/inetd.conf && \
	echo "ftp stream tcp nowait root /usr/bin/vsftpd vsftpd" >> $(prefix)/release-enigma2-pli-nightly/etc/inetd.conf && \
	echo "telnet stream tcp nowait root /usr/sbin/telnetd telnetd -i -l /bin/login" >> $(prefix)/release-enigma2-pli-nightly/etc/inetd.conf && \
	echo "ssh stream tcp nowait root /bin/dropbear dropbear -i" >> $(prefix)/release-enigma2-pli-nightly/etc/inetd.conf && \
	cp -dp $(targetprefix)/etc/image-version $(prefix)/release-enigma2-pli-nightly/etc/ && \
	cp $(buildprefix)/root/etc/timezone.xml $(prefix)/release-enigma2-pli-nightly/etc/ && \
	cp -dp $(targetprefix)/etc/vdstandby.cfg $(prefix)/release-enigma2-pli-nightly/etc/ && \
	cp -dp $(targetprefix)/etc/network/interfaces $(prefix)/release-enigma2-pli-nightly/etc/network/ && \
	cp -dp $(targetprefix)/etc/network/options $(prefix)/release-enigma2-pli-nightly/etc/network/ && \
	cp -dp $(targetprefix)/etc/init.d/umountfs $(prefix)/release-enigma2-pli-nightly/etc/init.d/ && \
	cp -dp $(targetprefix)/etc/init.d/sendsigs $(prefix)/release-enigma2-pli-nightly/etc/init.d/ && \
	cp -dp $(targetprefix)/etc/init.d/halt $(prefix)/release-enigma2-pli-nightly/etc/init.d/ && \
	cp $(buildprefix)/root/release/reboot $(prefix)/release-enigma2-pli-nightly/etc/init.d/ && \
	cp $(targetprefix)/etc/tuxbox/satellites.xml $(prefix)/release-enigma2-pli-nightly/etc/tuxbox/ && \
	cp $(targetprefix)/etc/tuxbox/cables.xml $(prefix)/release-enigma2-pli-nightly/etc/tuxbox/ && \
	cp $(targetprefix)/etc/tuxbox/terrestrial.xml $(prefix)/release-enigma2-pli-nightly/etc/tuxbox/ && \
	cp -RP $(buildprefix)/root/usr/local/share/enigma2/Vali.HD.nano $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2/ && \
	cp -RP $(appsdir)/enigma2-pli-nightly/data/skin_default $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2/ && \
	cp -RP $(appsdir)/enigma2-pli-nightly/data/*.xml $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2/ && \
	cp -RP $(appsdir)/enigma2-pli-nightly/data/free* $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2/ && \
	cp -RP $(appsdir)/enigma2-pli-nightly/data/*.png $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2/ && \
	cp -RP $(appsdir)/enigma2-pli-nightly/data/*.mvi $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2/ && \
	cp -RP $(appsdir)/enigma2-pli-nightly/data/*.conf $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2/ && \
	cp -RP $(appsdir)/enigma2-pli-nightly/data/countries $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2/ && \
	cp -RP $(targetprefix)/usr/local/share/enigma2/po/en $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2/po/ && \
	cp -RP $(targetprefix)/usr/local/share/enigma2/po/ru $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2/po/ && \
	cp -RP $(targetprefix)/usr/local/share/enigma2/po/no $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2/po/ && \
	cp -RP $(targetprefix)/usr/local/share/enigma2/po/pl $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2/po/ && \
	cp -RP $(targetprefix)/usr/local/share/enigma2/po/de $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2/po/ && \
	cp -RP $(appsdir)/enigma2-pli-nightly/data/extensions $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2/ && \
	cp -RP $(buildprefix)/root/usr/local/share/enigma2/keymap_spark.xml $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2/keymap.xml && \
	echo "576i50" > $(prefix)/release-enigma2-pli-nightly/etc/videomode && \
	cp -R $(targetprefix)/etc/fonts/* $(prefix)/release-enigma2-pli-nightly/etc/fonts/ && \
	cp $(buildprefix)/root/release/rcS_stm23_24_spark $(prefix)/release-enigma2-pli-nightly/etc/init.d/rcS
	chmod 755 $(prefix)/release-enigma2-pli-nightly/etc/init.d/rcS && \
	cp $(buildprefix)/root/release/mountvirtfs $(prefix)/release-enigma2-pli-nightly/etc/init.d/ && \
	cp $(buildprefix)/root/release/mme_check $(prefix)/release-enigma2-pli-nightly/etc/init.d/ && \
	cp $(buildprefix)/root/release/mountall $(prefix)/release-enigma2-pli-nightly/etc/init.d/ && \
	cp $(buildprefix)/root/release/hostname $(prefix)/release-enigma2-pli-nightly/etc/init.d/ && \
	cp $(buildprefix)/root/release/vsftpd $(prefix)/release-enigma2-pli-nightly/etc/init.d/ && \
	cp $(buildprefix)/root/release/bootclean.sh $(prefix)/release-enigma2-pli-nightly/etc/init.d/ && \
	cp $(buildprefix)/root/release/networking $(prefix)/release-enigma2-pli-nightly/etc/init.d/ && \
	cp $(buildprefix)/root/bin/autologin $(prefix)/release-enigma2-pli-nightly/bin/ && \
	cp $(buildprefix)/root/bin/vdstandby $(prefix)/release-enigma2-pli-nightly/bin/ && \
	mkdir -p $(prefix)/release-enigma2-pli-nightly/var/run/lirc && \
	cp -rd $(targetprefix)/lib/* $(prefix)/release-enigma2-pli-nightly/lib/ && \
	rm -f $(prefix)/release-enigma2-pli-nightly/lib/*.a && \
	rm -f $(prefix)/release-enigma2-pli-nightly/lib/*.o && \
	rm -f $(prefix)/release-enigma2-pli-nightly/lib/*.la && \
	$(USERS) chmod 755 -R $(prefix)/release-enigma2-pli-nightly/lib
	find $(prefix)/release-enigma2-pli-nightly/lib/ -name '*.so*' -exec sh4-linux-strip --strip-unneeded {} \;

	cp -dp $(targetprefix)/sbin/mkfs $(prefix)/release-enigma2-pli-nightly/sbin/

	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stm_v4l2.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stmvbi.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stmvout.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/
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
		echo `pwd` player2/linux/drivers/$$mod; \
		if [ -e player2/linux/drivers/$$mod ]; then \
			cp player2/linux/drivers/$$mod $(prefix)/release-enigma2-pli-nightly/lib/modules/; \
			sh4-linux-strip --strip-unneeded $(prefix)/release-enigma2-pli-nightly/lib/modules/`basename $$mod`; \
		else \
			touch $(prefix)/release-enigma2-pli-nightly/lib/modules/`basename $$mod`; \
		fi; \
		echo "."; \
	done
	echo "touched";

	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/avs/avs.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/boxtype/boxtype.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/simu_button/simu_button.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/e2_proc/e2_proc.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/

	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stmfb.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/multicom/embxshell/embxshell.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/multicom/embxmailbox/embxmailbox.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/multicom/embxshm/embxshm.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/multicom/mme/mme_host.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/

	[ -e $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/pti/pti.ko ] && cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/pti/pti.ko $(prefix)/release-enigma2-pli-nightly/lib/modules || true
	[ -e $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/pti_np/pti.ko ] && cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/pti_np/pti.ko $(prefix)/release-enigma2-pli-nightly/lib/modules || true

	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/compcache/lzo-kmod/lzo1x_compress.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/compcache/lzo-kmod/lzo1x_decompress.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/
	cp $(buildprefix)/root/lib/modules/stm24/* $(prefix)/release-enigma2-pli-nightly/lib/modules/

	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/compcache/ramzswap.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/bpamem/bpamem.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/stgfb/stmfb/stmcore-display-sti7111.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/
	cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/frontcontroller/aotom/aotom.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/
	find $(prefix)/release-enigma2-pli-nightly/lib/modules/ -name '*.ko' -exec sh4-linux-strip --strip-unneeded {} \;

	rm -rf $(prefix)/release-enigma2-pli-nightly/lib/autofs
	rm -rf $(prefix)/release-enigma2-pli-nightly/lib/modules/$(KERNELVERSION)

	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/media
	ln -s /media/hdd $(prefix)/release-enigma2-pli-nightly/hdd
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/media/dvd

	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/mnt
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/mnt/usb
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/mnt/hdd
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/mnt/nfs

	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/root

	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/proc
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/sys
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/tmp

	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/usr
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/usr/bin
	cp -p $(targetprefix)/usr/sbin/vsftpd $(prefix)/release-enigma2-pli-nightly/usr/bin/
	cp -p $(targetprefix)/usr/bin/python $(prefix)/release-enigma2-pli-nightly/usr/bin/

	cp -p $(targetprefix)/usr/bin/killall $(prefix)/release-enigma2-pli-nightly/usr/bin/
	cp -p $(targetprefix)/usr/bin/opkg-cl $(prefix)/release-enigma2-pli-nightly/usr/bin/opkg
	cp -p $(targetprefix)/usr/bin/ffmpeg $(prefix)/release-enigma2-pli-nightly/sbin/
#	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/usr/tuxtxt
#	cp -p $(targetprefix)/usr/bin/tuxtxt $(prefix)/release-enigma2-pli-nightly/usr/bin/
#	cp $(buildprefix)/root/usr/tuxtxt/tuxtxt2.conf $(prefix)/release-enigma2-pli-nightly/usr/tuxtxt/
	
	if [ -e $(buildprefix)/.deps/neutrino ]; then \
		cp -p $(targetprefix)/usr/bin/grab $(prefix)/release-enigma2-pli-nightly/usr/bin/; \
	fi
	cp -p $(targetprefix)/usr/sbin/ethtool $(prefix)/release-enigma2-pli-nightly/usr/sbin/



	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/usr/share

	ln -s /usr/local/share/enigma2 $(prefix)/release-enigma2-pli-nightly/usr/share/enigma2

	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/usr/share/fonts
	cp $(targetprefix)/usr/share/fonts/ae_AlMateen.ttf $(prefix)/release-enigma2-pli-nightly/usr/share/fonts/
	cp $(targetprefix)/usr/share/fonts/lcd.ttf $(prefix)/release-enigma2-pli-nightly/usr/share/fonts/
	cp $(targetprefix)/usr/share/fonts/md_khmurabi_10.ttf $(prefix)/release-enigma2-pli-nightly/usr/share/fonts/
	cp $(targetprefix)/usr/share/fonts/nmsbd.ttf $(prefix)/release-enigma2-pli-nightly/usr/share/fonts/
	cp $(targetprefix)/usr/share/fonts/tuxtxt.ttf $(prefix)/release-enigma2-pli-nightly/usr/share/fonts/
	cp $(targetprefix)/usr/share/fonts/goodtime.ttf $(prefix)/release-enigma2-pli-nightly/usr/share/fonts/
	cp $(targetprefix)/usr/share/fonts/valis_enigma.ttf $(prefix)/release-enigma2-pli-nightly/usr/share/fonts/
	cp $(targetprefix)/usr/share/fonts/valis_lcd.ttf $(prefix)/release-enigma2-pli-nightly/usr/share/fonts/
	cp $(targetprefix)/usr/share/fonts/seg.ttf $(prefix)/release-enigma2-pli-nightly/usr/share/fonts/
	cp $(targetprefix)/usr/share/fonts/seg_internat.ttf $(prefix)/release-enigma2-pli-nightly/usr/share/fonts/
	cp $(targetprefix)/usr/share/fonts/Symbols.ttf $(prefix)/release-enigma2-pli-nightly/usr/share/fonts/
	if [ -e $(targetprefix)/usr/share/fonts/tuxtxt.otb ]; then \
		cp $(targetprefix)/usr/share/fonts/tuxtxt.otb $(prefix)/release-enigma2-pli-nightly/usr/share/fonts/; \
	fi
	if [ -e $(targetprefix)/usr/local/share/fonts/andale.ttf ]; then \
		cp $(targetprefix)/usr/local/share/fonts/andale.ttf $(prefix)/release-enigma2-pli-nightly/usr/share/fonts/; \
	fi
	
#       Font libass
	cp $(buildprefix)/root/usr/share/fonts/FreeSans.ttf $(prefix)/release-enigma2-pli-nightly/usr/share/fonts/

	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/usr/share/zoneinfo
	cp -aR $(buildprefix)/root/usr/share/zoneinfo/* $(prefix)/release-enigma2-pli-nightly/usr/share/zoneinfo/

	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/usr/share/udhcpc
	cp -aR $(buildprefix)/root/usr/share/udhcpc/* $(prefix)/release-enigma2-pli-nightly/usr/share/udhcpc/

	if [ -e $(targetprefix)/usr/share/alsa ]; then \
		mkdir $(prefix)/release-enigma2-pli-nightly/usr/share/alsa/; \
		mkdir $(prefix)/release-enigma2-pli-nightly/usr/share/alsa/cards/; \
		mkdir $(prefix)/release-enigma2-pli-nightly/usr/share/alsa/pcm/; \
		cp $(targetprefix)/usr/share/alsa/alsa.conf          $(prefix)/release-enigma2-pli-nightly/usr/share/alsa/alsa.conf; \
		cp $(targetprefix)/usr/share/alsa/cards/aliases.conf $(prefix)/release-enigma2-pli-nightly/usr/share/alsa/cards/; \
		cp $(targetprefix)/usr/share/alsa/pcm/default.conf   $(prefix)/release-enigma2-pli-nightly/usr/share/alsa/pcm/; \
		cp $(targetprefix)/usr/share/alsa/pcm/dmix.conf      $(prefix)/release-enigma2-pli-nightly/usr/share/alsa/pcm/; fi

	ln -sf /usr/local/share/keymaps $(prefix)/release-enigma2-pli-nightly/usr/share/keymaps

	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/usr/local
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/usr/local/bin
	if [ -e $(targetprefix)/usr/bin/enigma2 ]; then \
		cp -f $(targetprefix)/usr/bin/enigma2 $(prefix)/release-enigma2-pli-nightly/usr/local/bin/enigma2; \
	fi
	if [ -e $(targetprefix)/usr/local/bin/enigma2 ]; then \
		cp -f $(targetprefix)/usr/local/bin/enigma2 $(prefix)/release-enigma2-pli-nightly/usr/local/bin/enigma2; \
	fi
	find $(prefix)/release-enigma2-pli-nightly/usr/local/bin/ -name  enigma2 -exec sh4-linux-strip --strip-unneeded {} \;


	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/usr/local/share
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/usr/local/share/keymaps
	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2
#	cp -a $(targetprefix)/usr/local/share/enigma2/* $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2
	cp -a $(targetprefix)/etc/enigma2/* $(prefix)/release-enigma2-pli-nightly/etc/enigma2

	cp -f $(buildprefix)/root/usr/local/share/enigma2/keymap_spark.xml $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2/keymap.xml

	ln -sf /usr/share/fonts $(prefix)/release-enigma2-pli-nightly/usr/local/share/fonts

	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/usr/lib
	cp -R $(targetprefix)/usr/lib/* $(prefix)/release-enigma2-pli-nightly/usr/lib/
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/engines
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/gconv
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/libxslt-plugins
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/pkgconfig
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/sigc++-1.2
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/X11
	rm -f $(prefix)/release-enigma2-pli-nightly/usr/lib/*.a
	rm -f $(prefix)/release-enigma2-pli-nightly/usr/lib/*.o
	rm -f $(prefix)/release-enigma2-pli-nightly/usr/lib/*.la
	$(USERS) chmod 755 -R $(prefix)/release-enigma2-pli-nightly/usr/lib
	find $(prefix)/release-enigma2-pli-nightly/usr/lib/ -name '*.so*' -exec sh4-linux-strip --strip-unneeded {} \;

	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2
	cp -a $(targetprefix)/usr/lib/enigma2/* $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/
	if test -d $(targetprefix)/usr/local/lib/enigma2; then \
		cp -a $(targetprefix)/usr/local/lib/enigma2/* $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/; \
	fi
#	if [ -e $(targetprefix)/usr/lib/enigma2/python/Plugins/SystemPlugins/Tuxtxt ]; then \
#		cp -RP $(targetprefix)/usr/lib/enigma2/python/Plugins/SystemPlugins/Tuxtxt $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/Plugins/SystemPlugins/; \
#	fi

	cp -RP $(buildprefix)/root/usr/lib/enigma2/python/Components $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/
	cp -RP $(buildprefix)/root/usr/lib/enigma2/python/Screens $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/
	cp -RP $(buildprefix)/root/usr/lib/enigma2/python/Plugins/Extensions/AutomaticVolumeAdjustment $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/Plugins/Extensions/
	cp -RP $(buildprefix)/root/usr/lib/enigma2/python/Plugins/Extensions/EVOMediaCenter $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/Plugins/Extensions/
	cp -RP $(buildprefix)/root/usr/lib/enigma2/python/Plugins/Extensions/PinguExplorer $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/Plugins/Extensions/
	cp -RP $(buildprefix)/root/usr/lib/enigma2/python/Plugins/Extensions/PinkPanel $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/Plugins/Extensions/
	cp -RP $(buildprefix)/root/usr/lib/enigma2/python/Plugins/SystemPlugins $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/Plugins/

#	Dont remove pyo files, remove pyc instead
	find $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/ -name '*.pyc' -exec rm -f {} \;
	find $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/ -name '*.pyo' -exec rm -f {} \;
	find $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/ -name '*.a' -exec rm -f {} \;
	find $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/ -name '*.o' -exec rm -f {} \;
	find $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/ -name '*.la' -exec rm -f {} \;
	find $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/ -name '*.so*' -exec sh4-linux-strip --strip-unneeded {} \;

#	Delete unnecessary plugins #
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/Plugins/DemoPlugins
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/Plugins/SystemPlugins/FrontprocessorUpgrade
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/Plugins/SystemPlugins/NFIFlash
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/Plugins/Extensions/FileManager
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/Plugins/Extensions/DVDBurn
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/Plugins/Extensions/DVDPlayer
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/Plugins/Extensions/CutListEditor
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/Plugins/Extensions/MediaScanner
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/Plugins/Extensions/TuxboxPlugins
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/Plugins/Extensions/WebInterface
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/Plugins/SystemPlugins/SoftwareManager
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/Plugins/SystemPlugins/CrashlogAutoSubmit
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/Plugins/SystemPlugins/TempFanControl
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/Plugins/SystemPlugins/DefaultServicesScanner
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/Plugins/SystemPlugins/DiseqcTester
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/Plugins/SystemPlugins/CommonInterfaceAssignment
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/Plugins/SystemPlugins/CableScan
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/Plugins/SystemPlugins/OSD*
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/Plugins/SystemPlugins/VideoClippingSetup

	$(INSTALL_DIR) $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6
	cp -a $(targetprefix)/usr/lib/python2.6/* $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/test
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/site-packages/lxml
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/site-packages/lxml-2.0.5-py2.6.egg-info
	rm -f $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/site-packages/libxml2mod.so
	rm -f $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/site-packages/libxsltmod.so
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/site-packages/OpenSSL/test
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/site-packages/setuptools
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/site-packages/setuptools-0.6c8-py2.6.egg-info
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/site-packages/zope.interface-3.3.0-py2.6.egg-info
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/site-packages/twisted/test
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/site-packages/twisted/conch
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/site-packages/twisted/mail
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/site-packages/twisted/manhole
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/site-packages/twisted/names
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/site-packages/twisted/news
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/site-packages/twisted/trial
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/site-packages/twisted/words
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/site-packages/twisted/application
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/site-packages/twisted/enterprise
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/site-packages/twisted/flow
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/site-packages/twisted/lore
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/site-packages/twisted/pair
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/site-packages/twisted/runner
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/site-packages/twisted/scripts
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/site-packages/twisted/tap
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/site-packages/twisted/topfiles
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/site-packages/Twisted-8.2.0-py2.6.egg-info
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/bsddb
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/compiler
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/config
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/ctypes
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/curses
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/distutils
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/email

#	Dont remove pyo files, remove pyc instead
	find $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/ -name '*.pyc' -exec rm -f {} \;
	find $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/ -name '*.pyo' -exec rm -f {} \;
	find $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/ -name '*.a' -exec rm -f {} \;
	find $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/ -name '*.o' -exec rm -f {} \;
	find $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/ -name '*.la' -exec rm -f {} \;
	find $(prefix)/release-enigma2-pli-nightly/usr/lib/python2.6/ -name '*.so*' -exec sh4-linux-strip --strip-unneeded {} \;

	cp -RP $(buildprefix)/root/etc/modules.available $(prefix)/release-enigma2-pli-nightly/etc/
	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/Plugins/Extensions/FlashBackup
#### displaytime plugin not longer needed so we remove it #####
#	rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/Plugins/Extensions/PPDisplayTime
#	cp -RP $(buildprefix)/root/etc/enigma2/* $(prefix)/release-enigma2-pli-nightly/etc/enigma2/
	cp -RP $(buildprefix)/root/etc/tuxbox/satellites.xml $(prefix)/release-enigma2-pli-nightly/etc/tuxbox/
	touch $(prefix)/release-enigma2-pli-nightly/etc/.start_enigma2
#	cp -RP $(buildprefix)/root/usr/lib/enigma2/python/Components/Converter $(prefix)/release-enigma2-pli-nightly/usr/lib/enigma2/python/Components/
#	rm $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2/keymap_*.xml
	touch $(prefix)/release-enigma2-pli-nightly/etc/changelog.txt

######## FOR YOUR OWN CHANGES use these folder in cdk/own_build/enigma2 #############
#	rm $(prefix)/release-enigma2-pli-nightly/bin/showiframe
#	rm $(prefix)/release-enigma2-pli-nightly/usr/local/share/enigma2/radio.mvi

	cp -RP $(buildprefix)/own_build/enigma2/* $(prefix)/release-enigma2-pli-nightly/
	cp -RP $(buildprefix)/root/bin/fbshot $(prefix)/release-enigma2-pli-nightly/bin/

	cp $(kernelprefix)/linux-sh4/arch/sh/boot/uImage $(prefix)/release-enigma2-pli-nightly/boot/

	[ -e $(kernelprefix)/linux-sh4/drivers/net/wireless/zd1201.ko ] && cp $(kernelprefix)/linux-sh4/drivers/net/wireless/zd1201.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/zd1201.ko || true
	[ -e $(kernelprefix)/linux-sh4/drivers/usb/serial/ftdi_sio.ko ] && cp $(kernelprefix)/linux-sh4/drivers/usb/serial/ftdi_sio.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/ftdi_sio.ko || true
	[ -e $(kernelprefix)/linux-sh4/drivers/usb/serial/pl2303.ko ] && cp $(kernelprefix)/linux-sh4/drivers/usb/serial/pl2303.ko $(prefix)/release-enigma2-pli-nightly/lib/modules || true
	[ -e $(kernelprefix)/linux-sh4/drivers/usb/serial/usbserial.ko ] && cp $(kernelprefix)/linux-sh4/drivers/usb/serial/usbserial.ko $(prefix)/release-enigma2-pli-nightly/lib/modules || true
	[ -e $(kernelprefix)/linux-sh4/fs/ntfs/ntfs.ko ] && cp $(kernelprefix)/linux-sh4/fs/ntfs/ntfs.ko $(prefix)/release-enigma2-pli-nightly/lib/modules || true
	[ -e $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/cpu_frequ/cpu_frequ.ko ] && cp $(targetprefix)/lib/modules/$(KERNELVERSION)/extra/cpu_frequ/cpu_frequ.ko $(prefix)/release-enigma2-pli-nightly/lib/modules || true
	[ -e cp $(targetprefix)/lib/modules/$(KERNELVERSION)/kernel/fs/autofs4/autofs4.ko ] && $(targetprefix)/lib/modules/$(KERNELVERSION)/kernel/fs/autofs4/autofs4.ko $(prefix)/release-enigma2-pli-nightly/lib/modules/ || true

#
# AUTOFS
#
#	cp -f $(targetprefix)/usr/sbin/automount $(prefix)/release-enigma2-pli-nightly/usr/sbin/; \
#	ln -s /usr/lib/autofs/mount_ext2.so $(prefix)/release-enigma2-pli-nightly/usr/lib/autofs/mount_ext3.so; \
#	cp $(kernelprefix)/$(kernelpath)/fs/autofs4/autofs4.ko $(prefix)/release_neutrino/lib/modules; \
#	cp -f $(buildprefix)/root/release/auto.usb $(prefix)/release-enigma2-pli-nightly/etc/;
	cp -f $(buildprefix)/root/release/autofs $(prefix)/release-enigma2-pli-nightly/etc/init.d/
	cp -f $(buildprefix)/root/usr/sbin/automount $(prefix)/release-enigma2-pli-nightly/usr/sbin/
	cp -f $(buildprefix)/root/release/auto.hotplug $(prefix)/release-enigma2-pli-nightly/etc/
	cp -f $(buildprefix)/root/release/auto.network $(prefix)/release-enigma2-pli-nightly/etc/

#
# GSTREAMER
#
	if [ -d $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10 ]; then \
		rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/libgstfft*; \
		rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/*; \
		cp -a $(targetprefix)/usr/bin/gst-launch* $(prefix)/release-enigma2-pli-nightly/usr/bin/; \
		sh4-linux-strip --strip-unneeded $(prefix)/release-enigma2-pli-nightly/usr/bin/gst-launch*; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstalsa.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstapp.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstasf.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstassrender.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstaudioconvert.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstaudioparsers.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstaudioresample.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstautodetect.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstavi.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstcdxaparse.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstcoreelements.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstcoreindexers.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstdecodebin.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstdecodebin2.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstdvbaudiosink.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstdvbvideosink.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstdvdsub.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstflac.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstflv.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstfragmented.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgsticydemux.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstid3demux.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstisomp4.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstmad.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstmatroska.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstmpegaudioparse.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstmpegdemux.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstmpegstream.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstogg.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstplaybin.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstrtmp.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstrtp.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstrtpmanager.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstrtsp.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstsouphttpsrc.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstsubparse.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgsttypefindfunctions.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstudp.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstvcdsrc.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstwavparse.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		if [ -e $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/libgstffmpeg.so ]; then \
			cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstffmpeg.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
			cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstffmpegscale.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
			cp -a $(targetprefix)/usr/lib/gstreamer-0.10/libgstpostproc.so $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/; \
		fi; \
		sh4-linux-strip --strip-unneeded $(prefix)/release-enigma2-pli-nightly/usr/lib/gstreamer-0.10/*; \
	fi

#
# DIRECTFB
#
	if [ -d $(prefix)/release-enigma2-pli-nightly/usr/lib/directfb-1.4-5 ]; then \
		rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/directfb-1.4-5/gfxdrivers/*.a; \
		rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/directfb-1.4-5/gfxdrivers/*.la; \
		rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/directfb-1.4-5/gfxdrivers/*.o; \
		rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/directfb-1.4-5/inputdrivers/*; \
		cp -a $(targetprefix)/usr/lib/directfb-1.4-5/inputdrivers/libdirectfb_enigma2remote.so $(prefix)/release-enigma2-pli-nightly/usr/lib/directfb-1.4-5/inputdrivers/; \
		rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/directfb-1.4-5/systems/*.a; \
		rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/directfb-1.4-5/systems/*.la; \
		rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/directfb-1.4-5/systems/*.o; \
		rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/directfb-1.4-5/systems/libdirectfb_dummy.so; \
		rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/directfb-1.4-5/systems/libdirectfb_fbdev.so; \
		rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/directfb-1.4-5/wm/*.a; \
		rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/directfb-1.4-5/wm/*.la; \
		rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/directfb-1.4-5/wm/*.o; \
		rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/directfb-1.4-5/interfaces/IDirectFBFont/*.a; \
		rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/directfb-1.4-5/interfaces/IDirectFBFont/*.la; \
		rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/directfb-1.4-5/interfaces/IDirectFBFont/*.o; \
		rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/directfb-1.4-5/interfaces/IDirectFBImageProvider/*.a; \
		rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/directfb-1.4-5/interfaces/IDirectFBImageProvider/*.la; \
		rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/directfb-1.4-5/interfaces/IDirectFBImageProvider/*.o; \
		rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/directfb-1.4-5/interfaces/IDirectFBVideoProvider/*.a; \
		rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/directfb-1.4-5/interfaces/IDirectFBVideoProvider/*.la; \
		rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/directfb-1.4-5/interfaces/IDirectFBVideoProvider/*.o; \
	fi
	if [ -d $(prefix)/release-enigma2-pli-nightly/usr/lib/icu ]; then \
		rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/icu; \
	fi
	if [ -d $(prefix)/release-enigma2-pli-nightly/usr/lib/glib-2.0 ]; then \
		rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/glib-2.0; \
	fi
	if [ -d $(prefix)/release-enigma2-pli-nightly/usr/lib/gio ]; then \
		rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/gio; \
	fi
	if [ -d $(prefix)/release-enigma2-pli-nightly/usr/lib/enchant ]; then \
		rm -rf $(prefix)/release-enigma2-pli-nightly/usr/lib/enchant; \
	fi

#graphlcd Stuff
	if [ -e $(prefix)/release-enigma2-pli/usr/lib/libglcddrivers.so ]; then \
		( cd $(prefix)/release-enigma2-pli/usr/lib && ln -sf libglcddrivers.so.2.1.0 libglcddrivers.so && ln -sf libglcddrivers.so.2.1.0 libglcddrivers.so.2 && ln -sf libglcdgraphics.so.2.1.0 libglcdgraphics.so && ln -sf libglcdgraphics.so.2.1.0 libglcdgraphics.so.2 &&  ln -sf libglcdskin.so.2.1.0 libglcdskin.so &&  ln -sf libglcdskin.so.2.1.0 libglcdskin.so.2 ); \
		cp -f $(targetprefix)/etc/graphlcd.conf $(prefix)/release-enigma2-pli/etc/graphlcd.conf; \
	fi

	find $(prefix)/release-enigma2-pli-nightly/ -name 'Makefile*' -exec rm {} \;

