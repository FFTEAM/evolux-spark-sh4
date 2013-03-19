#Trick ALPHA-Version ;)
$(DEPDIR)/release_evolux_triple: \
$(DEPDIR)/%release_evolux_triple:
	rm -rf $(prefix)/release_evolux_triple_with_dev || true
	rm -rf $(prefix)/uImage || true
	rm -rf $(prefix)/e2jffs2.img || true
	mkdir -p $(prefix)/release_evolux_triple_with_dev
	$(USERS) cp -RP $(prefix)/release_neutrino-hd_with_dev/* $(prefix)/release_evolux_triple_with_dev/
	$(USERS) chmod 777 -R $(prefix)/release_evolux_triple_with_dev
if ENABLE_SPARK
	mv $(prefix)/release_evolux_triple_with_dev/usr/local/share $(prefix)/release_evolux_triple_with_dev/usr/local/share_nhd
	mkdir -p $(prefix)/release_evolux_triple_with_dev/usr/local/share
	mv $(prefix)/release_evolux_triple_with_dev/usr/local/bin $(prefix)/release_evolux_triple_with_dev/usr/local/bin_nhd
endif
	mkdir -p $(prefix)/release_evolux_triple_with_dev/usr/local/bin
	cp -RP $(prefix)/release_neutrino-hd2_with_dev/var $(prefix)/release_evolux_triple_with_dev/
	cp -RP $(prefix)/release_neutrino-hd2_with_dev/sbin $(prefix)/release_evolux_triple_with_dev/
	cp -RP $(prefix)/release_neutrino-hd2_with_dev/usr/bin $(prefix)/release_evolux_triple_with_dev/usr/
	cp -RP $(prefix)/release_neutrino-hd2_with_dev/usr/sbin $(prefix)/release_evolux_triple_with_dev/usr/
	cp -RP $(prefix)/release_neutrino-hd2_with_dev/usr/script $(prefix)/release_evolux_triple_with_dev/usr/
	cp -RP $(prefix)/release_neutrino-hd2_with_dev/usr/share $(prefix)/release_evolux_triple_with_dev/usr/
	cp -RP $(appsdir)/neutrino-hd2-exp/data/fonts/*.otb $(prefix)/release_evolux_triple_with_dev/usr/share/fonts/
	cp -RP $(prefix)/release_neutrino-hd2_with_dev/usr/lib/*.so* $(prefix)/release_evolux_triple_with_dev/usr/lib/
if ENABLE_SPARK
	cp -RP $(prefix)/release_neutrino-hd2_with_dev/usr/local/share $(prefix)/release_evolux_triple_with_dev/usr/local/share_nhd2
	cp -RP $(prefix)/release_neutrino-hd2_with_dev/usr/local/bin $(prefix)/release_evolux_triple_with_dev/usr/local/bin_nhd2
	cp -RP $(prefix)/release_evolux_triple_with_dev/usr/local/share $(prefix)/release_evolux_triple_with_dev/usr/share/fonts
else
	cp -RP $(prefix)/release_evolux_triple_with_dev/usr/local/share/fonts $(prefix)/release_evolux_triple_with_dev/usr/share/fonts
	rm -rf $(prefix)/release_evolux_triple_with_dev/usr/local/share/fonts
	cp -RP $(prefix)/release_neutrino-hd2_with_dev/usr/local/share $(prefix)/release_evolux_triple_with_dev/usr/local/
	cp -RP $(prefix)/release_neutrino-hd2_with_dev/usr/share/fonts/* $(prefix)/release_evolux_triple_with_dev/usr/share/fonts/
	cp -RP $(prefix)/release_neutrino-hd2_with_dev/usr/local/bin $(prefix)/release_evolux_triple_with_dev/usr/local/
endif
if ENABLE_SPARK7162
	if [ -e $(prefix)/release_neutrino-hd2_with_dev/etc/changelog.txt ]; then \
		cp -RP $(prefix)/release_neutrino-hd2_with_dev/etc/changelog.txt $(prefix)/release_evolux_triple_with_dev/etc/; \
	fi
else
	if [ -e $(prefix)/release-enigma2-pli-nightly_with_dev/etc/changelog.txt ]; then \
		cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/changelog.txt $(prefix)/release_evolux_triple_with_dev/etc/; \
	fi
endif
	if [ -e $(prefix)/release_evolux_triple_with_dev/usr/share/fonts/share ]; then \
		rm -rf $(prefix)/release_evolux_triple_with_dev/usr/share/fonts/share; \
	fi
	if [ -e $(prefix)/release_evolux_triple_with_dev/usr/share/fonts/fonts ]; then \
		rm -rf $(prefix)/release_evolux_triple_with_dev/usr/share/fonts/fonts; \
	fi
	if [ -e $(prefix)/release_evolux_triple_with_dev/media/hdd ]; then \
		rm -rf $(prefix)/release_evolux_triple_with_dev/media/hdd; \
	fi
	( cd $(prefix)/release_evolux_triple_with_dev/media && ln -sf /autofs/sda1 hdd )
	if [ -e $(prefix)/release_evolux_triple_with_dev/media/hdd2 ]; then \
		rm -rf $(prefix)/release_evolux_triple_with_dev/media/hdd2; \
	fi
	( cd $(prefix)/release_evolux_triple_with_dev/media && ln -sf /autofs/sda2 hdd2 )
if ENABLE_SPARK
	cp -RP $(prefix)/release_evolux_triple_with_dev/usr/local/share_nhd/fonts/* $(prefix)/release_evolux_triple_with_dev/usr/share/fonts/
	rm -rf $(prefix)/release_evolux_triple_with_dev/usr/local/share_nhd/fonts
	( cd $(prefix)/release_evolux_triple_with_dev/usr/local/share_nhd && ln -sf /usr/share/fonts fonts )
#	( cd $(prefix)/release_evolux_triple_with_dev/usr/local/share_nhd2 && ln -sf /usr/share/fonts fonts )
	cp -RP $(appsdir)/neutrino-hd2-exp/lib/libtuxtxt/tuxtxt2.conf $(prefix)/release_evolux_triple_with_dev/usr/local/share_nhd2/config/tuxtxt/
else
	cp -RP $(prefix)/release_neutrino-hd2_with_dev/usr/share/fonts/* $(prefix)/release_evolux_triple_with_dev/usr/share/fonts/
	cp -RP $(prefix)/release_neutrino-hd2_with_dev/usr/local/share $(prefix)/release_evolux_triple_with_dev/usr/local/
#	rm -rf $(prefix)/release_evolux_triple_with_dev/usr/local/share/fonts
#	( cd $(prefix)/release_evolux_triple_with_dev/usr/local/share && ln -sf /usr/share/fonts fonts )
	cp -RP $(appsdir)/neutrino-hd2-exp/lib/libtuxtxt/tuxtxt2.conf $(prefix)/release_evolux_triple_with_dev/usr/local/share/config/tuxtxt/
endif
	cp -RP $(prefix)/release_neutrino-hd2_with_dev/lib/modules $(prefix)/release_evolux_triple_with_dev/lib/
	cp -RP $(targetprefix)/usr/local/lib/tuxbox/plugins/* $(prefix)/release_evolux_triple_with_dev/var/plugins/
if ENABLE_SPARK7162
	rm $(prefix)/release_evolux_triple_with_dev/var/plugins/startnhd*
	rm $(prefix)/release_evolux_triple_with_dev/var/plugins/starte2*
	rm $(prefix)/release_evolux_triple_with_dev/var/plugins/tuxcom*
	rm $(prefix)/release_evolux_triple_with_dev/usr/local/share/neutrino/locale/*
	cp -RP $(appsdir)/neutrino-hd2-exp/data/locale/deutsch.locale $(prefix)/release_evolux_triple_with_dev/usr/local/share/neutrino/locale/
	cp -RP $(appsdir)/neutrino-hd2-exp/data/locale/english.locale $(prefix)/release_evolux_triple_with_dev/usr/local/share/neutrino/locale/
endif
if ENABLE_SPARK
	cp -RP $(prefix)/release_evolux_triple_with_dev/usr/local/share_nhd/neutrino/httpd $(prefix)/release_evolux_triple_with_dev/usr/local/share_nhd2/neutrino/
#	chmod 755 -R $(prefix)/release_evolux_triple_with_dev/usr/local/share_nhd2/neutrino/httpd/scripts
#else
#	chmod 755 -R $(prefix)/release_evolux_triple_with_dev/usr/local/share/neutrino/httpd/scripts
endif
	( cd $(prefix)/release_evolux_triple_with_dev/share && ln -sf /usr/local/share/iso-codes iso-codes )
	$(USERS) chmod 777 $(prefix)/release_evolux_triple_with_dev/lib/lib*
	$(USERS) chmod 777 $(prefix)/release_evolux_triple_with_dev/usr/lib/lib*
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/var $(prefix)/release_evolux_triple_with_dev/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/sbin $(prefix)/release_evolux_triple_with_dev/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/usr/bin $(prefix)/release_evolux_triple_with_dev/usr/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/usr/sbin $(prefix)/release_evolux_triple_with_dev/usr/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/usr/script $(prefix)/release_evolux_triple_with_dev/usr/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/usr/share $(prefix)/release_evolux_triple_with_dev/usr/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/usr/lib $(prefix)/release_evolux_triple_with_dev/usr/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/usr/local $(prefix)/release_evolux_triple_with_dev/usr/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/network/interfaces $(prefix)/release_evolux_triple_with_dev/etc/network/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/enigma2 $(prefix)/release_evolux_triple_with_dev/etc/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/fonts $(prefix)/release_evolux_triple_with_dev/etc/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/tuxbox $(prefix)/release_evolux_triple_with_dev/etc/
#	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/changelog.txt $(prefix)/release_evolux_triple_with_dev/etc/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/image-version $(prefix)/release_evolux_triple_with_dev/etc/
	if [ -e $(prefix)/release-enigma2-pli-nightly_with_dev/etc/graphlcd.conf ]; then \
		cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/graphlcd.conf $(prefix)/release_evolux_triple_with_dev/etc/; \
	fi;
#	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/bin/mumudvb $(prefix)/release_evolux_triple_with_dev/bin/
#	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/mumudvb $(prefix)/release_evolux_triple_with_dev/etc/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/inetd.conf $(prefix)/release_evolux_triple_with_dev/etc/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/localtime $(prefix)/release_evolux_triple_with_dev/etc/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/rc.d/rc3.d $(prefix)/release_evolux_triple_with_dev/etc/rc.d/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/init.d/ntpupdate.sh $(prefix)/release_evolux_triple_with_dev/etc/init.d/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/dropbear $(prefix)/release_evolux_triple_with_dev/etc/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/Wireless $(prefix)/release_evolux_triple_with_dev/etc/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/rc.d/rc0.d $(prefix)/release_evolux_triple_with_dev/etc/rc.d/
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/etc/rc.d/rc6.d $(prefix)/release_evolux_triple_with_dev/etc/rc.d/
#	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/lib/firmware/dvb-usb* $(prefix)/release_evolux_triple_with_dev/lib/firmware/
if ENABLE_SPARK7162
	rm -rf $(prefix)/release_evolux_triple_with_dev/usr/lib/enigma2/python/Plugins/Extensions/PPDisplayTime
	cp -RP $(appsdir)/neutrino-hd-plugins/tuxcom/tuxcom $(prefix)/release_evolux_triple_with_dev/var/plugins/tuxcom.so
	cp -RP $(appsdir)/neutrino-hd-plugins/tuxcom/tuxcom.cfg $(prefix)/release_evolux_triple_with_dev/var/plugins/
endif
	cp -RP $(buildprefix)/root/etc/init.d/DisplayTime.sh $(prefix)/release_evolux_triple_with_dev/etc/init.d/
	cp -RP $(buildprefix)/root/release/rcS_stm23_24_evolux_spark $(prefix)/release_evolux_triple_with_dev/etc/init.d/rcS
if ENABLE_SPARK
	cp -RP $(buildprefix)/root/var/plugins/tuxcom.* $(prefix)/release_evolux_triple_with_dev/var/plugins/
	echo "EvoTRIPLE" > $(prefix)/release_evolux_triple_with_dev/etc/hostname
else
	echo "EvoTRIPLEX" > $(prefix)/release_evolux_triple_with_dev/etc/hostname
	[ ! -e $(prefix)/release_evolux_triple_with_dev/etc/.nhd2 ] && touch $(prefix)/release_evolux_triple_with_dev/etc/.nhd2 || true
endif
	cp -RP $(prefix)/release-enigma2-pli-nightly_with_dev/lib/modules $(prefix)/release_evolux_triple_with_dev/lib/
	cp -RP $(buildprefix)/root/usr/script/user_script.sh.example $(prefix)/release_evolux_triple_with_dev/usr/script/
	if [ -e $(targetprefix)/usr/share/alsa ]; then \
		cp -RP $(targetprefix)/usr/share/alsa $(prefix)/release_evolux_triple_with_dev/usr/share/; \
	fi
	( cd $(prefix)/release_evolux_triple_with_dev/bin && rm $(prefix)/release_evolux_triple_with_dev/bin/sh && ln -sf bash sh )
	rm $(prefix)/release_evolux_triple_with_dev/sbin/fsck.jfs
	rm $(prefix)/release_evolux_triple_with_dev/sbin/*ext4*
#	( cd $(prefix)/release_evolux_triple_with_dev/usr/local/share/enigma2/countries && rm a*.png b*.png c*.png dk.png ee.png es.png f*.png g*.png h*.png i*.png l*.png nl.png pt.png ro.png s*.png t*.png u*.png x*.png y*.png  )
	rm $(prefix)/release_evolux_triple_with_dev/usr/local/share/enigma2/freesat*
	if [ -e /usr/bin/python2.6 ] && [ -e $(buildprefix)/doEVOLUX.sh ]; then \
		$(buildprefix)/doEVOLUX.sh; \
	fi;
#	cp -RP $(prefix)/release_evolux_triple_with_dev/boot/uImage $(prefix)/
	cp -RP $(targetprefix)/boot/uImage $(prefix)/
	( cd $(prefix) && cp -RP ../flash/spark/spark_oob.img $(prefix)/release_evolux_triple_with_dev/sbin/ )
if ENABLE_SPARK
	( cd $(prefix) && cd ../flash/spark/orig-spark-plugin/root/plugin/var/etc && cp * $(prefix)/release_evolux_triple_with_dev/etc/ )
	cp -RP $(buildprefix)/root/bin/fw_printenv $(prefix)/release_evolux_triple_with_dev/bin/
	cp -RP $(buildprefix)/root/bin/fw_setenv $(prefix)/release_evolux_triple_with_dev/bin/
	mkdir -p $(prefix)/BootargsPack/bin
	mkdir -p $(prefix)/BootargsPack/etc
	( cd $(prefix) && cd ../flash/spark/orig-spark-plugin/root/plugin/var/etc && cp * $(prefix)/BootargsPack/etc/ )
	cp -RP $(buildprefix)/root/bin/fw_printenv $(prefix)/BootargsPack/bin/
	cp -RP $(buildprefix)/root/bin/fw_setenv $(prefix)/BootargsPack/bin/
	cp -RP $(buildprefix)/root/bin/setmtdmode $(prefix)/BootargsPack/bin/
	( cd $(prefix) && cp -RP ../flash/spark/flash_E2_yaffs2.sh $(prefix)/ )
	( cd $(prefix) && cp -RP ../flash/spark/howto_flash_yaffs2_new3.txt $(prefix)/ )
else
	( cd $(prefix) && cp -RP ../flash/spark/flash_E2_yaffs2_triplex.sh $(prefix)/ )
	( cd $(prefix) && cp -RP ../flash/spark/howto_flash_yaffs2_new3_triplex.txt $(prefix)/ )
endif
	( cd $(prefix) && cp -RP ../flash/spark/orig-spark-plugin/Evolux-Orig-Spark-BootPlugin $(prefix)/ )
	rm -rf $(prefix)/release_evolux_triple_with_dev/sbin/fsck.nfs
	rm -rf $(prefix)/release_evolux_triple_with_dev/usr/local/share/neutrino/icons/mbox_white.raw
#if ENABLE_MULTI_YAFFS2
#	( cd $(prefix) && cd ../flash/spark && ./spark_multi_yaffs2.sh )
#else
#	( cd $(prefix) && cd ../flash/spark && ./spark.sh )
#endif
	if [ -e /usr/bin/python2.6 ] && [ -e $(buildprefix)/doEVOLUX.sh ] ; then \
		$(buildprefix)/doEVOLUX.sh; \
	fi;
	if [ -e $(buildprefix)/makeUpdatePack.sh ] ; then \
		$(buildprefix)/makeUpdatePack.sh; \
	fi;
	touch $@

