#Trick ALPHA-Version ;)
$(DEPDIR)/release_evolux_neutrino-hd2: \
$(DEPDIR)/%release_evolux_neutrino-hd2:
	rm -rf $(prefix)/release_evolux_neutrino-hd2_with_dev || true
	rm -rf $(prefix)/uImage || true
	rm -rf $(prefix)/e2jffs2.img || true
	mkdir -p $(prefix)/release_evolux_neutrino-hd2_with_dev
	$(USERS) cp -RP $(prefix)/release_neutrino-hd_with_dev/* $(prefix)/release_evolux_neutrino-hd2_with_dev/
	$(USERS) chmod 777 -R $(prefix)/release_evolux_neutrino-hd2_with_dev
	mv $(prefix)/release_evolux_neutrino-hd2_with_dev/usr/local/share $(prefix)/release_evolux_neutrino-hd2_with_dev/usr/local/share_nhd
	mkdir -p $(prefix)/release_evolux_neutrino-hd2_with_dev/usr/local/share
	mv $(prefix)/release_evolux_neutrino-hd2_with_dev/usr/local/bin $(prefix)/release_evolux_neutrino-hd2_with_dev/usr/local/bin_nhd
	mkdir -p $(prefix)/release_evolux_neutrino-hd2_with_dev/usr/local/bin
	if [ -e /usr/bin/python2.6 ] && [ -e $(buildprefix)/doEVOLUX.sh ]; then \
		$(buildprefix)/doEVOLUX.sh; \
	fi;
	cp -RP $(prefix)/release_neutrino-hd2_with_dev/var $(prefix)/release_evolux_neutrino-hd2_with_dev/
	cp -RP $(prefix)/release_neutrino-hd2_with_dev/sbin $(prefix)/release_evolux_neutrino-hd2_with_dev/
	cp -RP $(prefix)/release_neutrino-hd2_with_dev/usr/bin $(prefix)/release_evolux_neutrino-hd2_with_dev/usr/
	cp -RP $(prefix)/release_neutrino-hd2_with_dev/usr/sbin $(prefix)/release_evolux_neutrino-hd2_with_dev/usr/
	cp -RP $(prefix)/release_neutrino-hd2_with_dev/usr/script $(prefix)/release_evolux_neutrino-hd2_with_dev/usr/
	cp -RP $(prefix)/release_neutrino-hd2_with_dev/usr/share $(prefix)/release_evolux_neutrino-hd2_with_dev/usr/
	cp -RP $(prefix)/release_neutrino-hd2_with_dev/usr/lib/*.so* $(prefix)/release_evolux_neutrino-hd2_with_dev/usr/lib/
	cp -RP $(prefix)/release_neutrino-hd2_with_dev/usr/local/share $(prefix)/release_evolux_neutrino-hd2_with_dev/usr/local/share_nhd2
	cp -RP $(prefix)/release_evolux_neutrino-hd2_with_dev/usr/local/share $(prefix)/release_evolux_neutrino-hd2_with_dev/usr/share/fonts
	cp -RP $(prefix)/release_neutrino-hd2_with_dev/usr/local/bin $(prefix)/release_evolux_neutrino-hd2_with_dev/usr/local/bin_nhd2
	if [ -e $(prefix)/release_neutrino-hd2_with_dev/etc/changelog.txt ]; then \
		cp -RP $(prefix)/release_neutrino-hd2_with_dev/etc/changelog.txt $(prefix)/release_evolux_neutrino-hd2_with_dev/etc/; \
	fi
	cp -RP $(prefix)/release_evolux_neutrino-hd2_with_dev/usr/local/share_nhd/neutrino/httpd $(prefix)/release_evolux_neutrino-hd2_with_dev/usr/local/share_nhd2/neutrino/
	if [ -e $(prefix)/release_evolux_neutrino-hd2_with_dev/usr/share/fonts/share ]; then \
		rm -rf $(prefix)/release_evolux_neutrino-hd2_with_dev/usr/share/fonts/share; \
	fi
	if [ -e $(prefix)/release_evolux_neutrino-hd2_with_dev/media/hdd ]; then \
		rm -rf $(prefix)/release_evolux_neutrino-hd2_with_dev/media/hdd; \
	fi
	( cd $(prefix)/release_evolux_neutrino-hd2_with_dev/media && ln -sf /autofs/sda1 hdd )
	if [ -e $(prefix)/release_evolux_neutrino-hd2_with_dev/media/hdd2 ]; then \
		rm -rf $(prefix)/release_evolux_neutrino-hd2_with_dev/media/hdd2; \
	fi
	( cd $(prefix)/release_evolux_neutrino-hd2_with_dev/media && ln -sf /autofs/sda2 hdd2 )
	cp -RP $(prefix)/release_evolux_neutrino-hd2_with_dev/usr/local/share_nhd/fonts/* $(prefix)/release_evolux_neutrino-hd2_with_dev/usr/share/fonts/
	rm -rf $(prefix)/release_evolux_neutrino-hd2_with_dev/usr/local/share_nhd/fonts
	( cd $(prefix)/release_evolux_neutrino-hd2_with_dev/usr/local/share_nhd && ln -sf /usr/share/fonts fonts )
	( cd $(prefix)/release_evolux_neutrino-hd2_with_dev/usr/local/share_nhd2 && ln -sf /usr/share/fonts fonts )
	echo "EvoHD2" > $(prefix)/release_evolux_neutrino-hd2_with_dev/etc/hostname
	cp -RP $(buildprefix)/root/release/rcS_stm23_24_evolux_spark $(prefix)/release_evolux_neutrino-hd2_with_dev/etc/init.d/rcS
	cp -RP $(prefix)/release_neutrino-hd2_with_dev/lib/modules $(prefix)/release_evolux_neutrino-hd2_with_dev/lib/
	cp -RP $(buildprefix)/root/usr/script/user_script.sh.example $(prefix)/release_evolux_neutrino-hd2_with_dev/usr/script/
#	cp -RP $(prefix)/release_evolux_neutrino-hd2_with_dev/boot/uImage $(prefix)/
	cp $(kernelprefix)/$(kernelpath)/arch/sh/boot/uImage $(prefix)/
	( cd $(prefix) && cd ../flash/spark/orig-spark-plugin/root/plugin/var/etc && cp * $(prefix)/release_evolux_neutrino-hd2_with_dev/etc/ )
	cp -RP $(buildprefix)/root/bin/fw_printenv $(prefix)/release_evolux_neutrino-hd2_with_dev/bin/
	cp -RP $(buildprefix)/root/bin/fw_setenv $(prefix)/release_evolux_neutrino-hd2_with_dev/bin/
	mkdir -p $(prefix)/BootargsPack/bin
	mkdir -p $(prefix)/BootargsPack/etc
	( cd $(prefix) && cd ../flash/spark/orig-spark-plugin/root/plugin/var/etc && cp * $(prefix)/BootargsPack/etc/ )
	cp -RP $(buildprefix)/root/bin/fw_printenv $(prefix)/BootargsPack/bin/
	cp -RP $(buildprefix)/root/bin/fw_setenv $(prefix)/BootargsPack/bin/
	cp -RP $(buildprefix)/root/bin/setmtdmode $(prefix)/BootargsPack/bin/
	( cd $(prefix) && cp -RP ../flash/spark/spark_oob.img $(prefix)/release_evolux_neutrino-hd2_with_dev/sbin/ )
	( cd $(prefix) && cp -RP ../flash/spark/flash_E2_yaffs2.sh $(prefix)/ )
	( cd $(prefix) && cp -RP ../flash/spark/howto_flash_yaffs2_new3.txt $(prefix)/ )
	( cd $(prefix) && cp -RP ../flash/spark/orig-spark-plugin/Evolux-Orig-Spark-BootPlugin $(prefix)/ )
	rm -rf $(prefix)/release_evolux_neutrino-hd2_with_dev/sbin/fsck.nfs
	rm -rf $(prefix)/release_evolux_neutrino-hd2_with_dev/usr/local/share/neutrino/icons/mbox_white.raw
	
#if ENABLE_MULTI_YAFFS2
#	( cd $(prefix) && cd ../flash/spark && ./spark_multi_yaffs2.sh )
#else
#	( cd $(prefix) && cd ../flash/spark && ./spark.sh )
#endif
	if [ -e $(buildprefix)/makeUpdatePack.sh ] ; then \
		$(buildprefix)/makeUpdatePack.sh; \
	fi;
	touch $@

