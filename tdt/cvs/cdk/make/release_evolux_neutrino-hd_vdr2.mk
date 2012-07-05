#Trick ALPHA-Version ;)
USERS=$(shell which sudo)
EVOLUXVERSION=$(shell cat $(prefix)/release_vdrdev2_with_dev/etc/changelog.txt | grep -m1 Version | cut -d = -f2)
$(DEPDIR)/release_evolux_neutrino-hd_vdr2: \
$(DEPDIR)/%release_evolux_neutrino-hd_vdr2:
	rm -rf $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev || true
	rm -rf $(prefix)/uImage || true
	rm -rf $(prefix)/e2jffs2.img || true
	mkdir -p $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/etc/config
	$(USERS) cp -RP $(prefix)/release_neutrino-hd_with_dev/* $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/
	$(USERS) chmod 777 -R $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev
	if [ -e /usr/bin/python2.6 ] && [ -e $(buildprefix)/doEVOLUX.sh ] ; then \
		$(buildprefix)/doEVOLUX.sh; \
	fi;
	cp -RP $(prefix)/release_vdrdev2_with_dev/usr/local/share/vdr $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/local/share/
	cp -RP $(prefix)/release_vdrdev2_with_dev/sbin $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/
	cp -RP $(prefix)/release_vdrdev2_with_dev/usr/bin $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/
	cp -RP $(prefix)/release_vdrdev2_with_dev/usr/sbin $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/
	cp -RP $(prefix)/release_vdrdev2_with_dev/usr/script $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/
	cp -RP $(prefix)/release_vdrdev2_with_dev/usr/share $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/
	cp -RP $(prefix)/release_vdrdev2_with_dev/usr/lib $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/
	mv $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/local/share/fonts/*.ttf $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/share/fonts
	rm -rf $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/local/share/fonts
	( cd $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/local/share && ln -s /usr/share/fonts fonts )
	cp -RP $(prefix)/release_vdrdev2_with_dev/usr/local $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/
	cp -RP $(prefix)/release_vdrdev2_with_dev/etc/changelog.txt $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/etc/
	echo "EVOVDR" > $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/etc/hostname
	if [ -e $(prefix)/release_vdrdev2_with_dev/etc/graphlcd.conf ]; then \
		cp -RP $(prefix)/release_vdrdev2_with_dev/etc/graphlcd.conf $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/etc/; \
	fi;
	cp -RP $(prefix)/release_vdrdev2_with_dev/etc/fonts $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/local/share/vdr/
	rm -rf $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/etc/fonts
	( cd $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/etc/ && ln -sf /usr/local/share/vdr/fonts fonts )
	cp -RP $(prefix)/release_vdrdev2_with_dev/lib/modules $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/lib/
	cp -RP $(prefix)/release_vdrdev2_with_dev/usr/lib/vdr $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/lib/
	cp -RP $(prefix)/release_vdrdev2_with_dev/usr/lib/*Magic* $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/lib/
	cp -RP $(prefix)/release_vdrdev2_with_dev/usr/lib/*alsa* $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/lib/
	cp -RP $(prefix)/release_vdrdev2_with_dev/usr/lib/*tiff* $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/lib/
	cp -RP $(prefix)/release_vdrdev2_with_dev/usr/lib/libcap* $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/lib/
	cp -RP $(prefix)/release_vdrdev2_with_dev/usr/lib/libuci* $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/lib/
	cp -RP $(prefix)/release_vdrdev2_with_dev/usr/lib/locale $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/lib/
	cp -RP $(prefix)/release_vdrdev2_with_dev/usr/lib/gconv $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/lib/
	cp -RP $(prefix)/release_vdrdev2_with_dev/sbin/uci $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/sbin/
	cp -RP $(prefix)/release_vdrdev2_with_dev/usr/bin/setup-* $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/bin/
	cp -RP $(prefix)/release_vdrdev2_with_dev/usr/bin/sync-* $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/bin/
	cp -RP $(prefix)/release_vdrdev2_with_dev/usr/local/share/vdr $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/local/share/
	cp -RP $(prefix)/release_vdrdev2_with_dev/etc/vdr $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/etc/
	cp -RP $(appsdir)/vdr/vdr-1.7.28/diseqc.conf $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/local/share/vdr/diseqc.conf.example
	cp -RP $(buildprefix)/root/usr/script/user_script.sh.example $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/script/
	if [ -e $(targetprefix)/usr/share/alsa ]; then \
		cp -RP $(targetprefix)/usr/share/alsa $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/share/; \
	fi
#	cp -RP $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/boot/uImage $(prefix)/
	( cd $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/var && ln -sf ../usr/local/share/vdr vdr )
	rm -rf $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/local/share/vdr/plugins/text2skin/anthra*
	rm -rf $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/local/share/vdr/plugins/text2skin/Egal*
	rm -rf $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/local/share/vdr/plugins/text2skin/Narrow*
	rm -rf $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/local/share/vdr/themes/anthra*
	rm -rf $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/local/share/vdr/themes/Egal*
	rm -rf $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/usr/local/share/vdr/themes/Narrow*
	cp $(kernelprefix)/$(kernelpath)/arch/sh/boot/uImage $(prefix)/
	( cd $(prefix) && cd ../flash/spark/orig-spark-plugin/root/plugin/var/etc && cp * $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/etc/ )
	cp -RP $(buildprefix)/root/bin/fw_printenv $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/bin/
	cp -RP $(buildprefix)/root/bin/fw_setenv $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/bin/
	mkdir -p $(prefix)/BootargsPack/bin
	mkdir -p $(prefix)/BootargsPack/etc
	( cd $(prefix) && cd ../flash/spark/orig-spark-plugin/root/plugin/var/etc && cp * $(prefix)/BootargsPack/etc/ )
	cp -RP $(buildprefix)/root/bin/fw_printenv $(prefix)/BootargsPack/bin/
	cp -RP $(buildprefix)/root/bin/fw_setenv $(prefix)/BootargsPack/bin/
	cp -RP $(buildprefix)/root/bin/setmtdmode $(prefix)/BootargsPack/bin/
	( cd $(prefix) && cp -RP ../flash/spark/spark_oob.img $(prefix)/release_evolux-neutrino-hd_vdr2_with_dev/sbin/ )
	( cd $(prefix) && cp -RP ../flash/spark/flash_E2_yaffs2.sh $(prefix)/ )
	( cd $(prefix) && cp -RP ../flash/spark/howto_flash_yaffs2_new3.txt $(prefix)/ )
	( cd $(prefix) && cp -RP ../flash/spark/orig-spark-plugin/Evolux-Orig-Spark-BootPlugin $(prefix)/ )
#if ENABLE_MULTI_YAFFS2
#	( cd $(prefix) && cd ../flash/spark && ./spark_multi_yaffs2.sh )
#else
#	( cd $(prefix) && cd ../flash/spark && ./spark.sh )
#endif
	if [ -e $(buildprefix)/makeUpdatePack.sh ] ; then \
		$(buildprefix)/makeUpdatePack.sh; \
	fi;
	touch $@

