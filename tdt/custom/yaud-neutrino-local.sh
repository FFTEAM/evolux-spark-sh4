#!/bin/sh
CURDIR=`pwd`
NRELDIR='../../tufsbox/release_neutrino'
CHANGEDIR='../../tufsbox'
TARGET=`cat $CURDIR/lastChoice | awk -F '--enable-' '{print $5}' | cut -d ' ' -f 1`
BUILDDIR='../../cvs/cdk'
# originally created by schischu and konfetti
# fedora parts prepared by lareq
# fedora/suse/ubuntu scripts merged by kire pudsje (kpc)

# make sure defines have not already been defined
UBUNTU=
FEDORA=
SUSE=
# Try to detect the distribution
if `which lsb_release > /dev/null 2>&1`; then 
	case `lsb_release -s -i` in
		Debian*) UBUNTU=1; USERS="sudo";;
		Fedora*) FEDORA=1; USERS="sudo";;
		SUSE*)   SUSE=1;   USERS="su";;
		Ubuntu*) UBUNTU=1; USERS="sudo";;
	esac
fi
# Not detected by lsb_release, try release files
if [ -z "$FEDORA$SUSE$UBUNTU" ]; then
	if   [ -f /etc/redhat-release ]; then FEDORA=1; USERS="sudo"; 
	elif [ -f /etc/fedora-release ]; then FEDORA=1; USERS="sudo"; 
	elif [ -f /etc/SuSE-release ];   then SUSE=1; USERS="su";
	elif [ -f /etc/debian_version ]; then UBUNTU=1; USERS="sudo";
	fi
fi

function make_default() {
	echo "Erstelle Standard /dev für alle Boxen..."
	echo "${MAKEDEV} std" > $CURDIR/.fakeroot
	echo "${MAKEDEV} bpamem" >> $CURDIR/.fakeroot
	echo "${MAKEDEV} console" >> $CURDIR/.fakeroot
	echo "${MAKEDEV} ttyAS0 ttyAS1 ttyAS2 ttyAS3" >> $CURDIR/.fakeroot
	echo "${MAKEDEV} ttyusb" >> $CURDIR/.fakeroot
	echo "${MAKEDEV} tun" >> $CURDIR/.fakeroot
	echo "${MAKEDEV} sd" >> $CURDIR/.fakeroot
	echo "${MAKEDEV} scd0 scd1" >> $CURDIR/.fakeroot
	echo "${MAKEDEV} sg" >> $CURDIR/.fakeroot
	echo "${MAKEDEV} st0 st1" >> $CURDIR/.fakeroot
	echo "${MAKEDEV} ptyp ptyq ptmx" >> $CURDIR/.fakeroot
	echo "${MAKEDEV} lp par audio fb rtc lirc st200 alsasnd" >> $CURDIR/.fakeroot
	echo "${MAKEDEV} input i2c mtd" >> $CURDIR/.fakeroot
	echo "${MAKEDEV} dvb mme" >> $CURDIR/.fakeroot
	echo "${MAKEDEV} ppp busmice" >> $CURDIR/.fakeroot
	echo "${MAKEDEV} fd" >> $CURDIR/.fakeroot
	echo "${MAKEDEV} video" >> $CURDIR/.fakeroot
	echo "ln -sf /dev/input/mouse0 mouse" >> $CURDIR/.fakeroot
	echo "mkdir -p pts" >> $CURDIR/.fakeroot
	echo "mkdir -p shm" >> $CURDIR/.fakeroot
	echo "mknod -m 0666 fuse c 10 229" >> $CURDIR/.fakeroot
	echo "mknod -m 0666 vfd c 147 0" >> $CURDIR/.fakeroot
}

function make_devs() {
	rm -rf $CURDIR/tmpdev && rm -f $CURDIR/tempdevs.tar.gz && mkdir -p $CURDIR/tmpdev && cd $CURDIR/tmpdev
	echo "tar czf $CURDIR/tempdevs.tar.gz ./" >> $CURDIR/.fakeroot
	chmod 755 $CURDIR/.fakeroot && fakeroot -- $CURDIR/.fakeroot && rm -f $CURDIR/.fakeroot
	cd ..
	rm -rf $CURDIR/tmpdev
	$USERS tar -xzf $CURDIR/tempdevs.tar.gz -C $NRELDIR/dev/ && rm $CURDIR/tempdevs.tar.gz
}

[ -e $NRELDIR/dev/ ] && [ -e $NRELDIR/dev/vfd ] && exit;

case $TARGET in
	spark)
		MAKEDEV="$CURDIR/root/sbin/MAKEDEV"
		make_default
		echo "Erstelle weitere /dev für $TARGET..."
		echo "mknod -m 666 rc c 147 1" >> $CURDIR/.fakeroot
		echo "mknod -m 666 sci0 c 169 0" >> $CURDIR/.fakeroot
		make_devs
	;;
	spark7162)
		MAKEDEV="$CURDIR/root/sbin/MAKEDEV"
#		make_default
		echo -e "Erstelle weitere /dev für $TARGET...\nSorry, $TARGET wird derzeit nicht unterstützt."
#		make_devs
	;;
esac
$USERS chmod 777 -R $CHANGEDIR/release_neutrino/dev
if [ -e $CHANGEDIR/release_neutrino_with_dev ]; then
	$USERS rm -rf $CHANGEDIR/release_neutrino_with_dev
fi

mv $CHANGEDIR/release_neutrino $CHANGEDIR/release_neutrino_with_dev
if [ -e $BUILDDIR/own_build/neutrino/boot/audio.elf ] || [ -e $BUILDDIR/own_build/neutrino/boot/video.elf ]; then
	cp -RP $BUILDDIR/own_build/neutrino/boot/* $CHANGEDIR/release_neutrino_with_dev/boot/
else
	echo "#############################"
	echo "#############################"
	echo "  audio/video.elf NOT FOUND "
	echo "in cdk/own_build/enigma2/boot !!!"
	echo "please put them in, "
	echo "otherway it won't boot"
	echo "##############################"
	echo "##############################"
	sleep 30
fi
if [ -e $PATCHDIR/custom/myPatches_Neutrino.diff ]; then
	cd $CHANGEDIR/release_with_dev && patch -p1 < "../../custom/myPatches_Neutrino.diff"
fi
	#$USERS chown root:root $CHANGEDIR/release_neutrino_with_dev/bin/lcd4linux
	#$USERS chmod 700 $CHANGEDIR/release_neutrino_with_dev/etc/lcd4linux.conf
#	cp -RP $CHANGEDIR/release_neutrino_with_dev/boot/uImage $CHANGEDIR/
#	$CHANGEDIR/host/bin/mkfs.jffs2 -r $CHANGEDIR/release_neutrino_with_dev -o $CHANGEDIR/e2jffs2.img -e 0x20000 -n
#	( cd $CHANGEDIR && tar -czvf Neutrino_on_Pingulux.tar.gz e2jffs2.img uImage )
echo "--- Erledigt ---"
exit
