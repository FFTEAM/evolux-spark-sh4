#Trick ALPHA-Version ;)
USERS=$(shell which sudo)
EVOLUXVERSION=$(shell cat $(prefix)/release_with_dev/etc/changelog.txt | grep -m1 Version | cut -d = -f2)
$(DEPDIR)/release_evolux: \
$(DEPDIR)/%release_evolux:
	rm -rf $(prefix)/release_evolux_with_dev || true
	rm -rf $(prefix)/uImage || true
	rm -rf $(prefix)/e2jffs2.img || true
	mkdir -p $(prefix)/release_evolux_with_dev
	$(USERS) cp -RP $(prefix)/release_neutrino_with_dev/* $(prefix)/release_evolux_with_dev/
	$(USERS) chmod 777 -R $(prefix)/release_evolux_with_dev
	if [ -e /usr/bin/python2.6 ] && [ -e $(buildprefix)/doEVOLUX.sh ] ; then \
		$(buildprefix)/doEVOLUX.sh; \
	fi;
	cp -RP $(prefix)/release_with_dev/lib/firmware/dvb-usb* $(prefix)/release_evolux_with_dev/lib/firmware/
	cp -RP $(prefix)/release_with_dev/var $(prefix)/release_evolux_with_dev/
	cp -RP $(prefix)/release_with_dev/sbin $(prefix)/release_evolux_with_dev/
	cp -RP $(prefix)/release_with_dev/usr/bin $(prefix)/release_evolux_with_dev/usr/
	cp -RP $(prefix)/release_with_dev/usr/sbin $(prefix)/release_evolux_with_dev/usr/
	cp -RP $(prefix)/release_with_dev/usr/script $(prefix)/release_evolux_with_dev/usr/
	cp -RP $(prefix)/release_with_dev/usr/share $(prefix)/release_evolux_with_dev/usr/
	cp -RP $(prefix)/release_with_dev/usr/tuxtxt $(prefix)/release_evolux_with_dev/usr/
	cp -RP $(prefix)/release_with_dev/usr/lib $(prefix)/release_evolux_with_dev/usr/
	mv $(prefix)/release_evolux_with_dev/usr/local/share/fonts/neutrino.ttf $(prefix)/release_evolux_with_dev/usr/share/fonts
	rm -rf $(prefix)/release_evolux_with_dev/usr/local/share/fonts
	cp -RP $(prefix)/release_with_dev/usr/local $(prefix)/release_evolux_with_dev/usr/
	cp -RP $(prefix)/release_with_dev/etc/network/interfaces $(prefix)/release_evolux_with_dev/etc/network/
	cp -RP $(prefix)/release_with_dev/etc/dropbear $(prefix)/release_evolux_with_dev/etc/
	cp -RP $(prefix)/release_with_dev/etc/enigma2 $(prefix)/release_evolux_with_dev/etc/
	cp -RP $(prefix)/release_with_dev/etc/fonts $(prefix)/release_evolux_with_dev/etc/
	cp -RP $(prefix)/release_with_dev/etc/tuxbox $(prefix)/release_evolux_with_dev/etc/
	cp -RP $(prefix)/release_with_dev/etc/Wireless $(prefix)/release_evolux_with_dev/etc/
	cp -RP $(prefix)/release_with_dev/etc/changelog.txt $(prefix)/release_evolux_with_dev/etc/
	echo "EvoLux" > $(prefix)/release_evolux_with_dev/etc/hostname
	cp -RP $(prefix)/release_with_dev/etc/image-version $(prefix)/release_evolux_with_dev/etc/
	if [ -e $(prefix)/release_with_dev/etc/graphlcd.conf ]; then \
		cp -RP $(prefix)/release_with_dev/etc/graphlcd.conf $(prefix)/release_evolux_with_dev/etc/; \
	fi;
	cp -RP $(prefix)/release_with_dev/bin/mumudvb $(prefix)/release_evolux_with_dev/bin/
	cp -RP $(prefix)/release_with_dev/etc/mumudvb $(prefix)/release_evolux_with_dev/etc/
	cp -RP $(prefix)/release_with_dev/etc/inetd.conf $(prefix)/release_evolux_with_dev/etc/
	cp -RP $(prefix)/release_with_dev/etc/localtime $(prefix)/release_evolux_with_dev/etc/
	cp -RP $(prefix)/release_with_dev/etc/rc.d/rc0.d $(prefix)/release_evolux_with_dev/etc/rc.d/
	cp -RP $(prefix)/release_with_dev/etc/rc.d/rc3.d $(prefix)/release_evolux_with_dev/etc/rc.d/
	cp -RP $(prefix)/release_with_dev/etc/rc.d/rc6.d $(prefix)/release_evolux_with_dev/etc/rc.d/
	cp -RP $(prefix)/release_with_dev/etc/init.d/ntpupdate.sh $(prefix)/release_evolux_with_dev/etc/init.d/
	cp -RP $(buildprefix)/root/etc/init.d/DisplayTime.sh $(prefix)/release_evolux_with_dev/etc/init.d/
	cp -RP $(buildprefix)/root/release/rcS_stm23_24_evolux_spark $(prefix)/release_evolux_with_dev/etc/init.d/rcS
	cp -RP $(prefix)/release_with_dev/lib/modules $(prefix)/release_evolux_with_dev/lib/
	cp -RP $(prefix)/release_evolux_with_dev/boot/uImage $(prefix)/
	cp -RP $(buildprefix)/root/etc/bootargs_* $(prefix)/release_evolux_with_dev/etc/
	cp -RP $(buildprefix)/root/etc/fw_env.config $(prefix)/release_evolux_with_dev/etc/
	cp -RP $(buildprefix)/root/bin/fw_printenv $(prefix)/release_evolux_with_dev/bin/
	cp -RP $(buildprefix)/root/bin/fw_setenv $(prefix)/release_evolux_with_dev/bin/
	mkdir -p $(prefix)/BootargsPack/bin
	mkdir -p $(prefix)/BootargsPack/etc
	cp -RP $(buildprefix)/root/etc/fw_env.config $(prefix)/BootargsPack/etc/
	cp -RP $(buildprefix)/root/etc/bootargs_* $(prefix)/BootargsPack/etc/
	cp -RP $(buildprefix)/root/bin/fw_printenv $(prefix)/BootargsPack/bin/
	cp -RP $(buildprefix)/root/bin/fw_setenv $(prefix)/BootargsPack/bin/
	cp -RP $(buildprefix)/root/bin/setmtdmode $(prefix)/BootargsPack/bin/
	( cd $(prefix) && cp -RP ../flash/spark/spark_oob.img $(prefix)/release_evolux_with_dev/sbin/ )
	( cd $(prefix) && cp -RP ../flash/spark/flash_E2_yaffs2.sh $(prefix)/ )
	( cd $(prefix) && cp -RP ../flash/spark/howto_flash_yaffs2_new3.txt $(prefix)/ )
	( cd $(prefix) && cp -RP ../flash/spark/orig-spark-plugin/Evolux-Orig-Spark-BootPlugin $(prefix)/ )
	( cd $(prefix) && ../flash/spark/spark.sh ); \
	if [ -e $(buildprefix)/makeUpdatePack.sh ] ; then \
		$(buildprefix)/makeUpdatePack.sh; \
	fi;
	touch $@

