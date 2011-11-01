############ Patches Kernel 23 ###############

PATCH_STR=_0123

STM23_DVB_PATCH = linux-sh4-linuxdvb_stm23$(PATCH_STR).patch

COMMONPATCHES_23 = \
		linux-squashfs3.0_lzma_stm23.patch \
		linux-sh4-cpp_stm23.patch \
		linux-sh4-time_stm23.patch \
		linux-sh4-cmdline-printk_stm23.patch \
		linux-sh4-shksyms-gcc43_stm23.patch \
		$(if $(P0123),linux-sh4-mtd_stm23$(PATCH_STR).patch) \
		$(if $(P0123),linux-sh4-strcpy_stm23$(PATCH_STR).patch) \
		$(if $(P0123),linux-sh4-asm_mov_0xffffff_stm23$(PATCH_STR).patch) \
		linux-sh4-cifs-unaligned-mem-access-kernel_stm23.patch \
		linux-tune_stm23.patch \
		linux-ftdi_sio.c_stm23.patch \
		$(STM23_DVB_PATCH)

SPARKPATCHES_23 = $(COMMONPATCHES_23) \
		$(if $(P0119),linux-sh4-spark_setup_stm23$(PATCH_STR).patch) \
		$(if $(P0119),linux-sh4-spark_sound_stm23$(PATCH_STR).patch) \
		$(if $(P0123),linux-sh4-spark_setup_stm23$(PATCH_STR).patch) \
		$(if $(P0123),linux-sh4-spark_sound_stm23$(PATCH_STR).patch) \
		fortis_hdbox_dvb_core_stm23.patch

KERNELPATCHES_23 = $(if $(TF7700),$(TF7700PATCHES_23)) \
		$(if $(SPARK),$(SPARKPATCHES_23))

############ Patches Kernel 23 End ###############

############ Patches Kernel 24 ###############

if ENABLE_P0201
PATCH_STR=_0201
endif

if ENABLE_P0205
PATCH_STR=_0205
endif

if ENABLE_P0206
PATCH_STR=_0206
endif

if ENABLE_P0207
PATCH_STR=_0207
endif

if ENABLE_HAVANA_P0207_5
PATCH_STR=_0207_5
GIT_STR=207-5
endif

STM24_DVB_PATCH = linux-sh4-linuxdvb_stm24$(PATCH_STR).patch

COMMONPATCHES_24 = \
		$(STM24_DVB_PATCH) \
		linux-sh4-sound_stm24$(PATCH_STR).patch \
		linux-sh4-time_stm24$(PATCH_STR).patch \
		linux-sh4-init_mm_stm24$(PATCH_STR).patch \
		linux-sh4-copro_stm24$(PATCH_STR).patch \
		linux-sh4-strcpy_stm24$(PATCH_STR).patch \
		linux-squashfs-lzma_stm24$(PATCH_STR).patch \
		linux-sh4-ext23_as_ext4_stm24$(PATCH_STR).patch \
		bpa2_procfs_stm24$(PATCH_STR).patch \
		xchg_fix_stm24$(PATCH_STR).patch \
		mm_cache_update_stm24$(PATCH_STR).patch \
		$(if $(P0207),linux-sh4-ehci_stm24$(PATCH_STR).patch) \
		$(if $(P0207),linux-ftdi_sio.c_stm24$(PATCH_STR).patch) \
		linux-sh4-lzma-fix_stm24$(PATCH_STR).patch \
		linux-tune_stm24.patch

SPARK_PATCHES_24 = $(COMMONPATCHES_24) \
		linux-sh4-spark_setup_stm24$(PATCH_STR).patch \
		linux-sh4-stmmac_stm24$(PATCH_STR).patch \
		linux-sh4-lmb_stm24$(PATCH_STR).patch \
		linux-sh4-i2c-stm-downgrade_stm24$(PATCH_STR).patch
		
KERNELPATCHES_24 =  \
		$(if $(SPARK),$(SPARK_PATCHES_24))
                
############ Patches Kernel 24 End ###############

############ Patches Havana ###############

HAVANA_STM24_DVB_PATCH = linux-sh4-linuxdvb_havana_stm24$(PATCH_STR).patch

COMMONPATCHES_HAVANA_STM24 = \
		$(HAVANA_STM24_DVB_PATCH) \
		linux-sh4-init_mm_havana_stm24$(PATCH_STR).patch

KERNELPATCHES_HAVANA_STM24 =  \
		$(if $(SPARK),$(SPARKPATCHES_HAVANA_STM24))

############ Patches Havana End ###############

#
# KERNEL-HEADERS
#
$(DEPDIR)/kernel-headers: linux-kernel.do_prepare
	cd $(KERNEL_DIR) && \
		$(INSTALL) -d $(targetprefix)/usr/include && \
		cp -a include/linux $(targetprefix)/usr/include && \
		cp -a include/asm-sh $(targetprefix)/usr/include/asm && \
		cp -a include/asm-generic $(targetprefix)/usr/include && \
		cp -a include/mtd $(targetprefix)/usr/include
	touch $@

if STM22
else !STM22
KERNELHEADERS := linux-kernel-headers
if STM23
if ENABLE_P0119
# STM23 && ENABLE_P0119
KERNELHEADERS_VERSION := 2.6.23.17_stm23_0119-41
KERNELHEADERS_SPEC := stm-target-kernel-headers-kbuild.spec
KERNELHEADERS_SPEC_PATCH := stm-target-kernel-headers-kbuild.spec.diff
KERNELHEADERS_PATCHES :=
else !ENABLE_P0119
# STM23 && !ENABLE_P0119
KERNELHEADERS_VERSION := 2.6.23.17_stm23_0123-41
KERNELHEADERS_SPEC := stm-target-kernel-headers-kbuild.spec
KERNELHEADERS_SPEC_PATCH := stm-target-kernel-headers-kbuild_0123.spec.diff
KERNELHEADERS_PATCHES :=
endif !ENABLE_P0119
else !STM23
# if STM24
if ENABLE_P0205
KERNELHEADERS_VERSION := 2.6.32.16_stm24_0205-43
else
if ENABLE_P0206
KERNELHEADERS_VERSION := 2.6.32.16-44
else
if ENABLE_P0207
KERNELHEADERS_VERSION := 2.6.32.16-44
else
if ENABLE_HAVANA_P0207_5
KERNELHEADERS_VERSION := 2.6.32.16-44
else
KERNELHEADERS_VERSION := 2.6.32.10_stm24_0201-42
endif
endif
endif
endif

KERNELHEADERS_SPEC := stm-target-kernel-headers-kbuild.spec
KERNELHEADERS_SPEC_PATCH :=
KERNELHEADERS_PATCHES :=
# endif STM24
endif !STM23
KERNELHEADERS_RPM := RPMS/noarch/$(STLINUX)-sh4-$(KERNELHEADERS)-$(KERNELHEADERS_VERSION).noarch.rpm

$(KERNELHEADERS_RPM): \
		$(if $(KERNELHEADERS_SPEC_PATCH),Patches/$(KERNELHEADERS_SPEC_PATCH)) \
		$(if $(KERNELHEADERS_PATCHES),$(KERNELHEADERS_PATCHES:%=Patches/%)) \
		Archive/$(STLINUX)-target-$(KERNELHEADERS)-$(KERNELHEADERS_VERSION).src.rpm \
		| linux-kernel.do_prepare
	rpm $(DRPM) --nosignature -Uhv $(lastword $^) && \
	$(if $(KERNELHEADERS_SPEC_PATCH),( cd SPECS && patch -p1 $(KERNELHEADERS_SPEC) < ../Patches/$(KERNELHEADERS_SPEC_PATCH) ) &&) \
	$(if $(KERNELHEADERS_PATCHES),cp $(KERNELHEADERS_PATCHES:%=Patches/%) SOURCES/ &&) \
	export PATH=$(hostprefix)/bin:$(PATH) && \
	rpmbuild $(DRPMBUILD) -bb -v --clean --target=sh4-linux SPECS/$(KERNELHEADERS_SPEC)

$(DEPDIR)/max-$(KERNELHEADERS) \
$(DEPDIR)/$(KERNELHEADERS): \
$(DEPDIR)/%$(KERNELHEADERS): $(KERNELHEADERS_RPM)
	@rpm $(DRPM) --ignorearch --nodeps -Uhv \
		--badreloc --relocate $(targetprefix)=$(prefix)/$*cdkroot $(lastword $^)
	touch $@
endif !STM22



#
# HOST-KERNEL
#

# IMPORTANT: it is expected that only one define is set
MODNAME = $(SPARK)

if DEBUG
DEBUG_STR=.debug
else !DEBUG
DEBUG_STR=
endif !DEBUG

HOST_KERNEL := host-kernel
if STM22
HOST_KERNEL_VERSION := 2.6.17.14_stm22_0041-41
HOST_KERNEL_SPEC := stm-$(HOST_KERNEL).spec
HOST_KERNEL_SPEC_PATCH :=
HOST_KERNEL_PATCHES := $(KERNELPATCHES_41)
HOST_KERNEL_CONFIG := linux-$(subst _stm22_,-,$(KERNELVERSION))_$(MODNAME).config$(DEBUG_STR)
HOST_KERNEL_SRC_RPM := $(STLINUX)-$(HOST_KERNEL)-source-$(HOST_KERNEL_VERSION).src.rpm
HOST_KERNEL_RPM := RPMS/noarch/$(STLINUX)-$(HOST_KERNEL)-source-$(HOST_KERNEL_VERSION).noarch.rpm
else !STM22
if STM23
HOST_KERNEL_VERSION := 2.6.23.17$(KERNELSTMLABEL)-$(KERNELLABEL)
HOST_KERNEL_SPEC := stm-$(HOST_KERNEL)-sh4.spec
HOST_KERNEL_SPEC_PATCH :=
HOST_KERNEL_PATCHES := $(KERNELPATCHES_23)
HOST_KERNEL_CONFIG := linux-sh4-$(subst _stm23_,-,$(KERNELVERSION))_$(MODNAME).config$(DEBUG_STR)
HOST_KERNEL_SRC_RPM := $(STLINUX)-$(HOST_KERNEL)-source-sh4-$(HOST_KERNEL_VERSION).src.rpm
HOST_KERNEL_RPM := RPMS/noarch/$(STLINUX)-$(HOST_KERNEL)-source-sh4-$(HOST_KERNEL_VERSION).noarch.rpm
else !STM23
# if STM24

if ENABLE_P0201
HOST_KERNEL_VERSION := 2.6.32.10$(KERNELSTMLABEL)-$(KERNELLABEL)
else
if ENABLE_P0205
HOST_KERNEL_VERSION := 2.6.32.16$(KERNELSTMLABEL)-$(KERNELLABEL)
else
if ENABLE_P0206
HOST_KERNEL_VERSION := 2.6.32.28$(KERNELSTMLABEL)-$(KERNELLABEL)
else
if ENABLE_P0207
HOST_KERNEL_VERSION := 2.6.32.28$(KERNELSTMLABEL)-$(KERNELLABEL)
else
if ENABLE_HAVANA_P0207_5
HOST_KERNEL_VERSION := 2.6.32.28$(KERNELSTMLABEL)-$(KERNELLABEL)
endif
endif
endif
endif
endif

if !ENABLE_HAVANA_P0207_5

HOST_KERNEL_SPEC := stm-$(HOST_KERNEL)-sh4.spec
HOST_KERNEL_SPEC_PATCH :=
HOST_KERNEL_PATCHES := $(KERNELPATCHES_24)
HOST_KERNEL_CONFIG := linux-sh4-$(subst _stm24_,-,$(KERNELVERSION))_$(MODNAME).config$(DEBUG_STR)
HOST_KERNEL_SRC_RPM := $(STLINUX)-$(HOST_KERNEL)-source-sh4-$(HOST_KERNEL_VERSION).src.rpm
HOST_KERNEL_RPM := RPMS/noarch/$(STLINUX)-$(HOST_KERNEL)-source-sh4-$(HOST_KERNEL_VERSION).noarch.rpm

endif

# endif STM24
endif !STM23
endif !STM22

if ENABLE_HAVANA_P0207_5

HAVANA_STM24_KERNEL_PATCHES := $(KERNELPATCHES_HAVANA_STM24)
HAVANA_STM24_KERNEL_CONFIG := linux-sh4-$(subst _stm24_,-,$(KERNELVERSION))_havana_$(MODNAME).config$(DEBUG_STR)

$(DEPDIR)/linux-kernel.do_prepare:
#close if necessary
	[ -d "$(KERNEL_DIR)" ] || \
	git clone git://git.stlinux.com/havana/com.st.havana.kernel.git $(KERNEL_DIR);
#reset and checkout everytime
	cd $(KERNEL_DIR) && \
	git reset --hard && \
	git checkout com.st.havana.kernel-$(GIT_STR);
#if not exist make a clean copy so we can make patches easier
	[ -d "$(KERNEL_DIR).orig" ] || \
	cp -ra $(KERNEL_DIR) $(KERNEL_DIR).orig
#if not exist create symlink
	[ -d "linux-sh4" ] || \
	ln -s $(KERNEL_DIR) linux-sh4
	$(if $(HAVANA_STM24_KERNEL_PATCHES),cd $(KERNEL_DIR) && cat $(HAVANA_STM24_KERNEL_PATCHES:%=../Patches/%) | patch -p1)
	$(INSTALL) -m644 Patches/$(HAVANA_STM24_KERNEL_CONFIG) $(KERNEL_DIR)/.config
	-rm $(KERNEL_DIR)/localversion*
	echo "$(KERNELSTMLABEL)" > $(KERNEL_DIR)/localversion-stm
	$(MAKE) -C $(KERNEL_DIR) ARCH=sh oldconfig
	$(MAKE) -C $(KERNEL_DIR) ARCH=sh include/asm
	$(MAKE) -C $(KERNEL_DIR) ARCH=sh include/linux/version.h
	rm $(KERNEL_DIR)/.config
	touch $@

$(DEPDIR)/linux-kernel.do_compile: \
		bootstrap-cross \
		linux-kernel.do_prepare \
		Patches/$(HAVANA_STM24_KERNEL_CONFIG) \
		config.status \
		| $(HOST_U_BOOT_TOOLS)
	-rm $(DEPDIR)/linux-kernel*.do_compile
	cd $(KERNEL_DIR) && \
		export PATH=$(hostprefix)/bin:$(PATH) && \
		$(MAKE) ARCH=sh CROSS_COMPILE=$(target)- mrproper && \
		@M4@ ../Patches/$(HAVANA_STM24_KERNEL_CONFIG) > .config && \
		$(MAKE) ARCH=sh CROSS_COMPILE=$(target)- uImage modules
	touch $@

else

if STM23_HAVANA
$(DEPDIR)/linux-kernel.do_prepare:
	rm -rf $(KERNEL_DIR)
	git clone git://git.stlinux.com/havana/com.st.havana.kernel.git $(KERNEL_DIR);
	$(INSTALL) -m644 Patches/mb618se_defconfig $(KERNEL_DIR)/.config
	-rm $(KERNEL_DIR)/localversion*
	echo "$(KERNELSTMLABEL)" > $(KERNEL_DIR)/localversion-stm
	$(MAKE) -C $(KERNEL_DIR) ARCH=sh oldconfig
	$(MAKE) -C $(KERNEL_DIR) ARCH=sh include/asm
	$(MAKE) -C $(KERNEL_DIR) ARCH=sh include/linux/version.h
	rm $(KERNEL_DIR)/.config
	touch $@

$(DEPDIR)/linux-kernel.do_compile: \
		bootstrap-cross \
		linux-kernel.do_prepare \
		Patches/mb618se_defconfig \
		config.status \
		| $(HOST_U_BOOT_TOOLS)
	-rm $(DEPDIR)/linux-kernel*.do_compile
	cd $(KERNEL_DIR) && \
		export PATH=$(hostprefix)/bin:$(PATH) && \
		$(MAKE) ARCH=sh CROSS_COMPILE=$(target)- mrproper && \
		@M4@ ../$(word 3,$^)	> .config && \
		$(MAKE) ARCH=sh CROSS_COMPILE=$(target)- uImage modules
	touch $@

else !STM23_HAVANA
##################################################################################
#stlinux23

$(HOST_KERNEL_RPM): \
		$(if $(HOST_KERNEL_SPEC_PATCH),Patches/$(HOST_KERNEL_SPEC_PATCH)) \
		Archive/$(HOST_KERNEL_SRC_RPM)
	rpm $(DRPM) --nosignature --nodeps -Uhv $(lastword $^) && \
	$(if $(HOST_KERNEL_SPEC_PATCH),( cd SPECS; patch -p1 $(HOST_KERNEL_SPEC) < ../Patches/$(HOST_KERNEL_SPEC_PATCH) ) &&) \
	rpmbuild $(DRPMBUILD) -ba -v --clean --target=sh4-linux SPECS/$(HOST_KERNEL_SPEC)

$(DEPDIR)/linux-kernel.do_prepare: \
		$(if $(HOST_KERNEL_PATCHES),$(HOST_KERNEL_PATCHES:%=Patches/%)) \
		$(HOST_KERNEL_RPM)
	@rpm $(DRPM) -ev $(HOST_KERNEL_SRC_RPM:%.src.rpm=%) || true
	rm -rf $(KERNEL_DIR)
	rm -rf linux{,-sh4}
	rpm $(DRPM) --ignorearch --nodeps -Uhv $(lastword $^)
	$(if $(HOST_KERNEL_PATCHES),cd $(KERNEL_DIR) && cat $(HOST_KERNEL_PATCHES:%=../Patches/%) | patch -p1)
	$(INSTALL) -m644 Patches/$(HOST_KERNEL_CONFIG) $(KERNEL_DIR)/.config
	-rm $(KERNEL_DIR)/localversion*
	echo "$(KERNELSTMLABEL)" > $(KERNEL_DIR)/localversion-stm
	$(MAKE) -C $(KERNEL_DIR) ARCH=sh oldconfig
	$(MAKE) -C $(KERNEL_DIR) ARCH=sh include/asm
	$(MAKE) -C $(KERNEL_DIR) ARCH=sh include/linux/version.h
	rm $(KERNEL_DIR)/.config
	touch $@

#dagobert: without stboard ->not sure if we need this
$(DEPDIR)/linux-kernel.do_compile: \
		bootstrap-cross \
		linux-kernel.do_prepare \
		Patches/$(HOST_KERNEL_CONFIG) \
		config.status \
		| $(HOST_U_BOOT_TOOLS)
	-rm $(DEPDIR)/linux-kernel*.do_compile
	cd $(KERNEL_DIR) && \
		export PATH=$(hostprefix)/bin:$(PATH) && \
		$(MAKE) ARCH=sh CROSS_COMPILE=$(target)- mrproper && \
		@M4@ ../Patches/$(HOST_KERNEL_CONFIG) > .config && \
		$(MAKE) ARCH=sh CROSS_COMPILE=$(target)- uImage modules
	touch $@

endif !STM23_HAVANA
endif

NFS_FLASH_SED_CONF=$(foreach param,XCONFIG_NFS_FS XCONFIG_LOCKD XCONFIG_SUNRPC,-e s"/^.*$(param)[= ].*/$(param)=m/")

if ENABLE_XFS
XFS_SED_CONF=$(foreach param,CONFIG_XFS_FS,-e s"/^.*$(param)[= ].*/$(param)=m/")
else
XFS_SED_CONF=-e ""
endif

if ENABLE_NFSSERVER
#NFSSERVER_SED_CONF=$(foreach param,CONFIG_NFSD CONFIG_NFSD_V3 CONFIG_NFSD_TCP,-e s"/^.*$(param)[= ].*/$(param)=y/")
NFSSERVER_SED_CONF=$(foreach param,CONFIG_NFSD,-e s"/^.*$(param)[= ].*/$(param)=y\nCONFIG_NFSD_V3=y\n\# CONFIG_NFSD_V3_ACL is not set\n\# CONFIG_NFSD_V4 is not set\nCONFIG_NFSD_TCP=y/")
else
NFSSERVER_SED_CONF=-e ""
endif

if ENABLE_NTFS
NTFS_SED_CONF=$(foreach param,CONFIG_NTFS_FS,-e s"/^.*$(param)[= ].*/$(param)=m/")
else
NTFS_SED_CONF=-e ""
endif

$(DEPDIR)/linux-kernel.cramfs.do_compile $(DEPDIR)/linux-kernel.squashfs.do_compile \
$(DEPDIR)/linux-kernel.jffs2.do_compile $(DEPDIR)/linux-kernel.usb.do_compile \
$(DEPDIR)/linux-kernel.focramfs.do_compile $(DEPDIR)/linux-kernel.fosquashfs.do_compile:
$(DEPDIR)/linux-kernel.%.do_compile: \
		bootstrap-cross \
		linux-kernel.do_prepare \
		Patches/linux-sh4-$(KERNELVERSION).stboards.c.m4 \
		Patches/$(HOST_KERNEL_CONFIG) \
		config.status \
		| $(DEPDIR)/$(HOST_U_BOOT_TOOLS)
	-rm $(DEPDIR)/linux-kernel*.do_compile
	cd $(KERNEL_DIR) && \
		export PATH=$(hostprefix)/bin:$(PATH) && \
		$(MAKE) ARCH=sh CROSS_COMPILE=$(target)- mrproper && \
		@M4@ -Drootfs=$* --define=rootsize=$(ROOT_PARTITION_SIZE) --define=datasize=$(DATA_PARTITION_SIZE) ../$(word 3,$^) \
					> drivers/mtd/maps/stboards.c && \
		@M4@ --define=btldrdef=$* ../Patches/$(HOST_KERNEL_CONFIG) \
					> .config && \
		sed $(NFS_FLASH_SED_CONF) -i .config && \
		sed $(XFS_SED_CONF) $(NFSSERVER_SED_CONF) $(NTFS_SED_CONF) -i .config && \
		$(MAKE) ARCH=sh CROSS_COMPILE=$(target)- uImage modules
	touch $@

$(DEPDIR)/min-linux-kernel $(DEPDIR)/std-linux-kernel $(DEPDIR)/max-linux-kernel \
$(DEPDIR)/linux-kernel: \
$(DEPDIR)/%linux-kernel: bootstrap $(DEPDIR)/linux-kernel.do_compile
	@$(INSTALL) -d $(prefix)/$*cdkroot/boot && \
	$(INSTALL) -d $(prefix)/$*$(notdir $(bootprefix)) && \
	$(INSTALL) -m644 $(KERNEL_DIR)/arch/sh/boot/uImage $(prefix)/$*$(notdir $(bootprefix))/vmlinux.ub && \
	$(INSTALL) -m644 $(KERNEL_DIR)/vmlinux $(prefix)/$*cdkroot/boot/vmlinux-sh4-$(KERNELVERSION) && \
	$(INSTALL) -m644 $(KERNEL_DIR)/System.map $(prefix)/$*cdkroot/boot/System.map-sh4-$(KERNELVERSION) && \
	$(INSTALL) -m644 $(KERNEL_DIR)/COPYING $(prefix)/$*cdkroot/boot/LICENSE
	cp $(KERNEL_DIR)/arch/sh/boot/uImage $(prefix)/$*cdkroot/boot/
#if STM22
	echo -e "ST Linux Distribution - Binary Kernel\n \
	CPU: sh4\n \
	$(if $(UFS910),PLATFORM: stb7100ref\n) \
	$(if $(FLASH_UFS910),PLATFORM: stb7100ref\n) \
	KERNEL VERSION: $(KERNELVERSION)\n" > $(prefix)/$*cdkroot/README.ST && \
	$(MAKE) -C $(KERNEL_DIR) ARCH=sh INSTALL_MOD_PATH=$(prefix)/$*cdkroot modules_install && \
	rm $(prefix)/$*cdkroot/lib/modules/$(KERNELVERSION)/build || true && \
	rm $(prefix)/$*cdkroot/lib/modules/$(KERNELVERSION)/source || true
#else
#endif
	@[ "x$*" = "x" ] && touch $@ || true
	@TUXBOX_YAUD_CUSTOMIZE@

$(DEPDIR)/driver: $(driverdir)/Makefile linux-kernel.do_compile
#	$(MAKE) -C $(KERNEL_DIR) $(MAKE_OPTS) ARCH=sh modules_prepare
	$(if $(PLAYER131),cp $(driverdir)/stgfb/stmfb/Linux/video/stmfb.h $(targetprefix)/usr/include/linux)
	$(if $(PLAYER179),cp $(driverdir)/stgfb/stmfb/linux/drivers/video/stmfb.h $(targetprefix)/usr/include/linux)
	$(if $(PLAYER191),cp $(driverdir)/stgfb/stmfb/linux/drivers/video/stmfb.h $(targetprefix)/usr/include/linux)
	$(if $(PLAYER191),cp $(driverdir)/stgfb/stmfb/linux/drivers/video/stmfb.h $(targetprefix)/usr/include/linux)
	cp $(driverdir)/player2/linux/include/linux/dvb/stm_ioctls.h $(targetprefix)/usr/include/linux/dvb
	$(MAKE) -C $(driverdir) ARCH=sh \
		KERNEL_LOCATION=$(buildprefix)/$(KERNEL_DIR) \
		$(if $(SPARK),SPARK=$(SPARK)) \
		$(if $(PLAYER131),PLAYER131=$(PLAYER131)) \
		$(if $(PLAYER179),PLAYER179=$(PLAYER179)) \
		$(if $(PLAYER191),PLAYER191=$(PLAYER191)) \
		$(if $(HAVANA_P0207_5),HAVANA_P0207_5=$(HAVANA_P0207_5)) \
		CROSS_COMPILE=$(target)-
	$(MAKE) -C $(driverdir) ARCH=sh \
		KERNEL_LOCATION=$(buildprefix)/$(KERNEL_DIR) \
		BIN_DEST=$(targetprefix)/bin \
		INSTALL_MOD_PATH=$(targetprefix) \
		$(if $(SPARK),SPARK=$(SPARK)) \
 		$(if $(PLAYER131),PLAYER131=$(PLAYER131)) \
		$(if $(PLAYER179),PLAYER179=$(PLAYER179)) \
		$(if $(PLAYER191),PLAYER191=$(PLAYER191)) \
		$(if $(HAVANA_P0207_5),HAVANA_P0207_5=$(HAVANA_P0207_5)) \
		install
	$(DEPMOD) -ae -b $(targetprefix) -F $(buildprefix)/$(KERNEL_DIR)/System.map -r $(KERNELVERSION)
	touch $@
	@TUXBOX_YAUD_CUSTOMIZE@

driver-clean:
	rm -f $(DEPDIR)/driver
	$(MAKE) -C $(driverdir) ARCH=sh \
		KERNEL_LOCATION=$(buildprefix)/$(KERNEL_DIR) \
		distclean

#------------------- Helper

linux-kernel.menuconfig linux-kernel.xconfig: \
linux-kernel.%:
	$(MAKE) -C $(KERNEL_DIR) ARCH=sh CROSS_COMPILE=sh4-linux- $*
	@echo
	@echo "You have to edit m a n u a l l y Patches/linux-$(KERNELVERSION).config to make changes permanent !!!"
	@echo ""
	diff linux/.config.old linux/.config

#-------------------

$(flashprefix)/root-cramfs/lib \
$(flashprefix)/root-squashfs/lib \
$(flashprefix)/root-jffs2/lib \
$(flashprefix)/root-usb/lib \
$(flashprefix)/root-focramfs/lib \
$(flashprefix)/root-fosquashfs/lib: \
$(flashprefix)/root-%/lib: \
		$(DEPDIR)/linux-kernel.%.do_compile
	-rm -rf $(flashprefix)/root-$*
	$(MAKE) -C $(KERNEL_DIR) ARCH=sh INSTALL_MOD_PATH=$(flashprefix)/root-$* modules_install
	-rm $(flashprefix)/root-$*/lib/modules/$(KERNELVERSION)/build
	-rm $(flashprefix)/root-$*/lib/modules/$(KERNELVERSION)/source
	@TUXBOX_CUSTOMIZE@

$(flashprefix)/root-disk/lib: \
$(flashprefix)/root-%/lib: \
		$(DEPDIR)/linux-kernel.do_compile
#		$(DEPDIR)/linux-kernel.%.do_compile
	-rm -rf $(flashprefix)/root-$*
	$(INSTALL_DIR) $(dir $@)/{boot,lib/firmware} && \
	$(INSTALL_FILE) $(KERNEL_DIR)/arch/sh/boot/uImage $(dir $@)/boot/vmlinux.ub && \
	$(CP_D) root/boot/ACF_*x.elf $(dir $@)/boot/ && \
	$(CP_D) root/boot/vid_*.elf $(dir $@)/boot/ && \
	$(CP_D) root/firmware/dvb-fe-cx24116.fw $(dir $@)/lib/firmware/ && \
	$(CP_D) root/firmware/dvb-fe-cx21143.fw $(dir $@)/lib/firmware/ && \
	$(MAKE) -C $(KERNEL_DIR) ARCH=sh INSTALL_MOD_PATH=$(flashprefix)/root-$* modules_install
	$(MAKE) -C $(driverdir) ARCH=sh KERNEL_LOCATION=$(buildprefix)/$(KERNEL_DIR) INSTALL_MOD_PATH=$(flashprefix)/root-$* install
	-rm $(flashprefix)/root-$*/lib/modules/$(KERNELVERSION)/build
	-rm $(flashprefix)/root-$*/lib/modules/$(KERNELVERSION)/source
	@TUXBOX_CUSTOMIZE@

