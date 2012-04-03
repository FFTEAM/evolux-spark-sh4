
# Plugins
$(DEPDIR)/enigma2-pli-plugins: enigma2-pli-openwebif enigma2-pli-networkbrowser

#
# enigma2-pli-openwebif
#
$(DEPDIR)/enigma2-pli-openwebif.do_prepare: bootstrap python pythoncheetah
	rm -rf e2openplugin-OpenWebif
	git clone git://github.com/E2OpenPlugins/e2openplugin-OpenWebif.git
	touch $@

$(DEPDIR)/enigma2-pli-openwebif.do_compile: $(DEPDIR)/enigma2-pli-openwebif.do_prepare
	cd e2openplugin-OpenWebif && \
		$(BUILDENV) \
		cp -a plugin $(targetprefix)/usr/lib/enigma2/python/Plugins/Extensions/OpenWebif
	touch $@

$(DEPDIR)/min-enigma2-pli-openwebif $(DEPDIR)/std-enigma2-pli-openwebif $(DEPDIR)/max-enigma2-pli-openwebif \
$(DEPDIR)/enigma2-pli-openwebif: \
$(DEPDIR)/%enigma2-pli-openwebif: $(DEPDIR)/enigma2-pli-openwebif.do_compile
#	@DISTCLEANUP_enigma2-pli-openwebif@
	@[ "x$*" = "x" ] && touch $@ || true

#
# enigma2-pli-openpli-plugins-enigma2
#
$(DEPDIR)/enigma2-pli-openpli-plugins-enigma2.do_prepare: 
	rm -rf openpli-plugins-enigma2
	git clone git://openpli.git.sourceforge.net/gitroot/openpli/plugins-enigma2 openpli-plugins-enigma2
	touch $@

#
# enigma2-pli-networkbrowser
#

$(DEPDIR)/enigma2-pli-networkbrowser.do_prepare: $(DEPDIR)/enigma2-pli-openpli-plugins-enigma2.do_prepare 
	cd openpli-plugins-enigma2 && \
	patch -p1 < $(buildprefix)/Patches/enigma2-pli-networkbrowser-support_autofs.patch
	touch $@

$(DEPDIR)/enigma2-pli-networkbrowser.do_compile: $(DEPDIR)/enigma2-pli-networkbrowser.do_prepare 
	cd openpli-plugins-enigma2/networkbrowser/src/lib && \
		$(BUILDENV) \
		sh4-linux-gcc -shared -o netscan.so \
			-I $(targetprefix)/usr/include/python2.6 \
			-include Python.h \
			errors.h \
			list.c \
			list.h \
			main.c \
			nbtscan.c \
			nbtscan.h \
			range.c \
			range.h \
			showmount.c \
			showmount.h \
			smb.h \
			smbinfo.c \
			smbinfo.h \
			statusq.c \
			statusq.h \
			time_compat.h
	cd openpli-plugins-enigma2/networkbrowser && \
		mkdir -p $(targetprefix)/usr/lib/enigma2/python/Plugins/SystemPlugins/NetworkBrowser && \
		cp -a po $(targetprefix)/usr/lib/enigma2/python/Plugins/SystemPlugins/NetworkBrowser/ && \
		cp -a meta $(targetprefix)/usr/lib/enigma2/python/Plugins/SystemPlugins/NetworkBrowser/ && \
		cp -a src/* $(targetprefix)/usr/lib/enigma2/python/Plugins/SystemPlugins/NetworkBrowser/ && \
		cp -a src/lib/netscan.so $(targetprefix)/usr/lib/enigma2/python/Plugins/SystemPlugins/NetworkBrowser/ && \
		rm -rf $(targetprefix)/usr/lib/enigma2/python/Plugins/SystemPlugins/NetworkBrowser/lib
	touch $@

$(DEPDIR)/min-enigma2-pli-networkbrowser $(DEPDIR)/std-enigma2-pli-networkbrowser $(DEPDIR)/max-enigma2-pli-networkbrowser \
$(DEPDIR)/enigma2-pli-networkbrowser: \
$(DEPDIR)/%enigma2-pli-networkbrowser: $(DEPDIR)/enigma2-pli-networkbrowser.do_compile
#	@DISTCLEANUP_enigma2-pli-networkbrowser@
	@[ "x$*" = "x" ] && touch $@ || true


