## Makefile for Spark
# http://www.stlinux.com/docs/getting_started-1.0/doc.php?print=1

##################################################################################################

# Set up the build environment
include make/buildenv.mk

# Include Archive targets (SPECS derived)
include make/archive-specs.mk

# Include host apps targets
# ipkg-utils
include make/hostapps.mk

# Include ipk stuff
include make/ipkg.mk

# Include bootstrap targets (SPECS derived)
include make/bootstrap.mk

#  Include u-boot targets
include make/u-boot.mk

# Include adapted etc files and etc read-write files
include make/root-etc.mk

# Include bare-os targets (mainly SPECS derived)
include make/bare-os.mk

# Include linux kernel targets
include make/linux-kernel.mk

# Include busybox targets
include make/busybox.mk

# Include contrib lib targets
include make/contrib-libs.mk

# Include contrib apps (SPECS derived)
include make/contrib-apps-specs.mk

# Include contrib apps
include make/contrib-apps.mk

# Include net apps
include make/net-apps.mk

# Include divers tools targets
include make/diverse-tools.mk

# More misc libs, mostly nonessential
include make/misc_libs.mk

# Misc tools, not essential
include make/misc_tools.mk

# Plugins
include make/plugins.mk

# Include enigma2 gui targets
#include make/enigma2.mk

# Include enigma2-nightly gui targets
include make/enigma2-nightly.mk

# Include enigma2-pli-nightly gui targets
include make/enigma2-pli-nightly.mk
include make/enigma2-pli-plugins.mk

# Include neutrino gui targets
include make/neutrino.mk

# Include neutrino-hd gui targets
include make/neutrino-hd.mk

# Include neutrino-mp gui targets
include make/neutrino-mp.mk

# Include neutrino-hd gui targets
include make/neutrino-hd2.mk

# A small, but absolutely essential library
include make/libtuxbox.mk

# Include yaud targets
include make/yaud.mk

# Include record targets
include make/record.mk

# Include prog targets
include make/prog.mk

# Include stg-framebuffer targets (SPECS derived)
# (stgfb X... freetype directfb)
include make/stg-framebuffer-specs.mk

# Include misc targets (SPECS derived)
# (splashutils)
include make/misc-specs.mk

# Strip libraries of symbols not needed.
include make/reduce-libs.mk

# A number of cleaning targets
include make/cleantargets.mk

# "Phony" make targets
include make/phony.mk

# Create the TAGS file
include make/tags.mk

# Target for building source distributions (hardly used these days of CVS :-)
include make/disttargets.mk

# vdr
include make/vdrdev2.mk
include make/release_vdrdev2.mk

#Trick
include make/release.mk

include make/release-enigma2-pli-nightly.mk

include make/release_neutrino.mk

include make/release_neutrino-hd.mk

include make/release_neutrino-mp.mk

include make/release_neutrino-hd2.mk

include make/release_evolux.mk

include make/release_evolux_pli.mk

include make/release_evolux_neutrino-hd2.mk

include make/release_evolux_neutrino-hd_pli.mk

include make/release_evolux_neutrino-hd2_pli.mk

include make/release_evolux_neutrino-hd_vdr2.mk

include make/release_evolux_triple.mk

# Give the user rope to hang himself :-).  (Note: read from the
# generated Makefile during make run, automake or configure does not
# see it.)
-include ./Makefile.local

