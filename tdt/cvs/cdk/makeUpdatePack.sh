#!/bin/sh
HOMEDIR="/home/pinky/pingulux-git-stm24-209/tdt"
SOURCE="$HOMEDIR/tufsbox/release_evolux_with_dev"

### needed update files ###
cp -RP $HOMEDIR/cvs/cdk/own_build/enigma2/etc/changelog.txt $HOMEDIR/custom/update/etc/
cp -RP $SOURCE/etc/.version $HOMEDIR/custom/update/etc/
cp -RP $SOURCE/etc/imageinfo $HOMEDIR/custom/update/etc/
cp -RP $SOURCE/etc/image-version $HOMEDIR/custom/update/etc/
cp -RP $SOURCE/usr/local/share/neutrino/locale/* $HOMEDIR/custom/update/usr/local/share/neutrino/locale/
cp -RP $SOURCE/usr/local/bin/neutrino $HOMEDIR/custom/update/usr/local/bin/
cp -RP $SOURCE/usr/lib/enigma2/python/Plugins/Extensions/PinkPanel/plugin.pyc $HOMEDIR/custom/update/usr/lib/enigma2/python/Plugins/Extensions/PinkPanel/
###########################

cd $HOMEDIR/custom/update
tar -czvf $HOMEDIR/tufsbox/update.evolux.yaffs2.tar.gz ./ 
cp -RP $SOURCE/etc/.version $HOMEDIR/tufsbox/version
exit

