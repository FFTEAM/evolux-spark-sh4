#Trick ALPHA-Version ;)
$(DEPDIR)/release_evolux_neutrino-hd_pli: \
$(DEPDIR)/%release_evolux_neutrino-hd_pli:
	rm -rf $(prefix)/release_evolux_neutrino-hd_pli_with_dev || true
	rm -rf $(prefix)/uImage || true
	rm -rf $(prefix)/e2jffs2.img || true
	mkdir -p $(prefix)/release_evolux_neutrino-hd_pli_with_dev
	$(USERS) cp -RP $(prefix)/release_neutrino-hd_with_dev/* $(prefix)/release_evolux_neutrino-hd_pli_with_dev/
	$(USERS) chmod 777 -R $(prefix)/release_evolux_neutrino-hd_pli_with_dev
	if [ -e /usr/bin/python2.6 ] && [ -e $(buildprefix)/doEVOLUX.sh ] ; then \
		$(buildprefix)/doEVOLUX.sh; \
	fi;
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/lib/firmware/dvb-usb* $(prefix)/release_evolux_neutrino-hd_pli_with_dev/lib/firmware/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/var $(prefix)/release_evolux_neutrino-hd_pli_with_dev/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/sbin $(prefix)/release_evolux_neutrino-hd_pli_with_dev/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/usr/bin $(prefix)/release_evolux_neutrino-hd_pli_with_dev/usr/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/usr/sbin $(prefix)/release_evolux_neutrino-hd_pli_with_dev/usr/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/usr/script $(prefix)/release_evolux_neutrino-hd_pli_with_dev/usr/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/usr/share $(prefix)/release_evolux_neutrino-hd_pli_with_dev/usr/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/usr/lib $(prefix)/release_evolux_neutrino-hd_pli_with_dev/usr/
	mv $(prefix)/release_evolux_neutrino-hd_pli_with_dev/usr/local/share/fonts/*.ttf $(prefix)/release_evolux_neutrino-hd_pli_with_dev/usr/share/fonts
	rm -rf $(prefix)/release_evolux_neutrino-hd_pli_with_dev/usr/local/share/fonts
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/usr/local $(prefix)/release_evolux_neutrino-hd_pli_with_dev/usr/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/network/interfaces $(prefix)/release_evolux_neutrino-hd_pli_with_dev/etc/network/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/dropbear $(prefix)/release_evolux_neutrino-hd_pli_with_dev/etc/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/enigma2 $(prefix)/release_evolux_neutrino-hd_pli_with_dev/etc/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/fonts $(prefix)/release_evolux_neutrino-hd_pli_with_dev/etc/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/tuxbox $(prefix)/release_evolux_neutrino-hd_pli_with_dev/etc/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/Wireless $(prefix)/release_evolux_neutrino-hd_pli_with_dev/etc/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/changelog.txt $(prefix)/release_evolux_neutrino-hd_pli_with_dev/etc/
	echo "EvoLux" > $(prefix)/release_evolux_neutrino-hd_pli_with_dev/etc/hostname
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/image-version $(prefix)/release_evolux_neutrino-hd_pli_with_dev/etc/
	if [ -e $(prefix)/release-enigma2-pli-nightly_with_dev/etc/graphlcd.conf ]; then \
		cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/graphlcd.conf $(prefix)/release_evolux_neutrino-hd_pli_with_dev/etc/; \
	fi;
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/bin/mumudvb $(prefix)/release_evolux_neutrino-hd_pli_with_dev/bin/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/mumudvb $(prefix)/release_evolux_neutrino-hd_pli_with_dev/etc/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/inetd.conf $(prefix)/release_evolux_neutrino-hd_pli_with_dev/etc/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/localtime $(prefix)/release_evolux_neutrino-hd_pli_with_dev/etc/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/rc.d/rc0.d $(prefix)/release_evolux_neutrino-hd_pli_with_dev/etc/rc.d/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/rc.d/rc3.d $(prefix)/release_evolux_neutrino-hd_pli_with_dev/etc/rc.d/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/rc.d/rc6.d $(prefix)/release_evolux_neutrino-hd_pli_with_dev/etc/rc.d/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/init.d/ntpupdate.sh $(prefix)/release_evolux_neutrino-hd_pli_with_dev/etc/init.d/
	cp -RP $(buildprefix)/root/etc/init.d/DisplayTime.sh $(prefix)/release_evolux_neutrino-hd_pli_with_dev/etc/init.d/
	cp -RP $(buildprefix)/root/release/rcS_stm23_24_evolux_spark $(prefix)/release_evolux_neutrino-hd_pli_with_dev/etc/init.d/rcS
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/lib/modules $(prefix)/release_evolux_neutrino-hd_pli_with_dev/lib/
#	cp -RP $(prefix)/release_evolux_neutrino-hd_pli_with_dev/boot/uImage $(prefix)/
	cp $(kernelprefix)/$(kernelpath)/arch/sh/boot/uImage $(prefix)/
	( cd $(prefix) && cd ../flash/spark/orig-spark-plugin/root/plugin/var/etc && cp * $(prefix)/release_evolux_neutrino-hd_pli_with_dev/etc/ )
	cp -RP $(buildprefix)/root/bin/fw_printenv $(prefix)/release_evolux_neutrino-hd_pli_with_dev/bin/
	cp -RP $(buildprefix)/root/bin/fw_setenv $(prefix)/release_evolux_neutrino-hd_pli_with_dev/bin/
	mkdir -p $(prefix)/BootargsPack/bin
	mkdir -p $(prefix)/BootargsPack/etc
	( cd $(prefix) && cd ../flash/spark/orig-spark-plugin/root/plugin/var/etc && cp * $(prefix)/BootargsPack/etc/ )
	cp -RP $(buildprefix)/root/bin/fw_printenv $(prefix)/BootargsPack/bin/
	cp -RP $(buildprefix)/root/bin/fw_setenv $(prefix)/BootargsPack/bin/
	cp -RP $(buildprefix)/root/bin/setmtdmode $(prefix)/BootargsPack/bin/
	( cd $(prefix) && cp -RP ../flash/spark/spark_oob.img $(prefix)/release_evolux_neutrino-hd_pli_with_dev/sbin/ )
	( cd $(prefix) && cp -RP ../flash/spark/flash_E2_yaffs2.sh $(prefix)/ )
	( cd $(prefix) && cp -RP ../flash/spark/howto_flash_yaffs2_new3.txt $(prefix)/ )
	( cd $(prefix) && cp -RP ../flash/spark/orig-spark-plugin/Evolux-Orig-Spark-BootPlugin $(prefix)/ )
	rm -rf $(prefix)/release_evolux_neutrino-hd_pli_with_dev/sbin/fsck.nfs
	rm -rf $(prefix)/release_evolux_neutrino-hd_pli_with_dev/usr/local/share/neutrino/icons/mbox_white.raw
	
#if ENABLE_MULTI_YAFFS2
#	( cd $(prefix) && cd ../flash/spark && ./spark_multi_yaffs2.sh )
#else
#	( cd $(prefix) && cd ../flash/spark && ./spark.sh )
#endif
	if [ -e $(buildprefix)/makeUpdatePack.sh ] ; then \
		$(buildprefix)/makeUpdatePack.sh; \
	fi;
	touch $@

