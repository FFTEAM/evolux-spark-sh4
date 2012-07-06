#!/bin/sh
CURDIR=`pwd`
KATIDIR=${CURDIR%/cvs/cdk}
#################
RELEASE=0
BETA=1
SNAPSHOT=$RELEASE
TIMESTAMP=`date +%Y%m%d%H%M`
#BUILDREV=`git describe | cut -f2 -d'-'`
if [ -e $KATIDIR/tufsbox/release_neutrino-hd2 ] || [ -e $KATIDIR/tufsbox/release_neutrino-hd ] || [ -e $KATIDIR/tufsbox/release_neutrino ] && [ ! -e $KATIDIR/release-enigma2-pli-nightly ] && [ ! -e $KATIDIR/release-enigma2 ]; then
	INTERNAL=3
	MyChangelog="cvs/cdk/own_build/neutrino/etc/changelog.txt"
elif [ ! -e $KATIDIR/tufsbox/release_neutrino-hd2 ] && [ -e $KATIDIR/release-enigma2-pli-nightly ] || [ -e $KATIDIR/release-enigma2 ]; then
	INTERNAL=1
	MyChangelog="cvs/cdk/own_build/enigma2/etc/changelog.txt"
elif [ -e $KATIDIR/tufsbox/release_neutrino-hd2_with_dev ] || [ -e $KATIDIR/tufsbox/release_neutrino-hd_with_dev ] || [ -e $KATIDIR/tufsbox/release_neutrino_with_dev ] && [ ! -e $KATIDIR/release-enigma2-pli-nightly_with_dev ] && [ ! -e $KATIDIR/release-enigma2_with_dev ]; then
	INTERNAL=3
	MyChangelog="cvs/cdk/own_build/neutrino/etc/changelog.txt"
elif [ ! -e $KATIDIR/tufsbox/release_neutrino-hd2_with_dev ] && [ -e $KATIDIR/release-enigma2-pli-nightly_with_dev ] || [ -e $KATIDIR/release-enigma2_with_dev ]; then
	INTERNAL=1
	MyChangelog="cvs/cdk/own_build/enigma2/etc/changelog.txt"
else
	INTERNAL=2
	MyChangelog="cvs/cdk/own_build/vdr/etc/changelog.txt"
fi


version1=`cat $KATIDIR/$MyChangelog | grep -m1 Version= | cut -d = -f2 | cut -d . -f1`
version2=`cat $KATIDIR/$MyChangelog | grep -m1 Version= | cut -d = -f2 | cut -d . -f2`
version3=`cat $KATIDIR/$MyChangelog | grep -m1 Version= | cut -d = -f2 | cut -d . -f3`
BUILDREV=$version1$version2$version3
#################

version=`git describe`
cat $KATIDIR/cvs/cdk/root/var/etc/.version | head -n 6 > $KATIDIR/cvs/cdk/root/var/etc/.version.new

#########################################
# Original Neutrino Release Cycle Format:
# SBBBYYYYMMTTHHMM
# | |   | | |  | |_MM=Minutes 0 -59
# | |   | | |  |___HH=Hours 0 - 23
# | |   | | |______TT=Day 0-31
# | |   | |________MM=Month 0-12
# | |   |__________YYYY=Year
# | |______________BBB=Build-Revision (interesting for Image-Groups!)
# |________________S=Snapshot Version: 0 = Release, 1 = Beta, 2 = Internal
#########################################

echo "version=$SNAPSHOT$BUILDREV$TIMESTAMP" >> $KATIDIR/cvs/cdk/root/var/etc/.version.new
echo "git=$version" >> $KATIDIR/cvs/cdk/root/var/etc/.version.new
exit
