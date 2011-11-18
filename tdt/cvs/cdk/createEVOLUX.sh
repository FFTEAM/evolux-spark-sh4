#! /bin/sh
CURDIR=`pwd`
PINGUDIR=${CURDIR%/cvs/cdk}
E2DIR=$PINGUDIR/tufsbox/release_with_dev
NEUTRINODIR=$PINGUDIR/tufsbox/release_neutrino_with_dev
EVOLUXDIR=$PINGUDIR/tufsbox/release_EVOLUX_with_dev
DESTINATION="/devel/E2"
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
		Debian*) UBUNTU=1; USERS="su -c";;
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
	elif [ -f /etc/debian_version ]; then UBUNTU=1; USERS="su -c";
	fi
fi
make clean
#rm .deps/*enigma2*
make yaud-enigma2-nightly
make clean
rm .deps/*neutrino*
make yaud-neutrino
cd $PINGUDIR/tufsbox
if [ ! -e $EVOLUXDIR ]; then
mkdir -p $EVOLUXDIR
fi
$USERS rm -rf $EVOLUXDIR/*
cd $NEUTRINODIR
$USERS tar -czvf $PINGUDIR/tufsbox/release_neutrino_with_dev_for_EVOLUX.tar.gz ./
$USERS  tar -xzvf $PINGUDIR/tufsbox/release_neutrino_with_dev_for_EVOLUX.tar.gz -C $EVOLUXDIR/
cp -RP $E2DIR/var $EVOLUXDIR/
cp -RP $E2DIR/sbin $EVOLUXDIR/
cp -RP $E2DIR/usr/bin $EVOLUXDIR/usr/
cp -RP $E2DIR/usr/sbin $EVOLUXDIR/usr/
cp -RP $E2DIR/usr/script $EVOLUXDIR/usr/
cp -RP $E2DIR/usr/share $EVOLUXDIR/usr/
cp -RP $E2DIR/usr/tuxtxt $EVOLUXDIR/usr/
cp -RP $E2DIR/usr/lib $EVOLUXDIR/usr/
mv $EVOLUXDIR/usr/local/share/fonts/neutrino.ttf $EVOLUXDIR/usr/share/fonts
rm -rf $EVOLUXDIR/usr/local/share/fonts
cp -RP $E2DIR/usr/local $EVOLUXDIR/usr/
cp -RP $E2DIR/etc/network/interfaces $EVOLUXDIR/etc/network/
cp -RP $E2DIR/etc/dropbear $EVOLUXDIR/etc/
cp -RP $E2DIR/etc/enigma2 $EVOLUXDIR/etc/
cp -RP $E2DIR/etc/fonts $EVOLUXDIR/etc/
cp -RP $E2DIR/etc/tuxbox $EVOLUXDIR/etc/
cp -RP $E2DIR/etc/Wireless $EVOLUXDIR/etc/
cp -RP $E2DIR/etc/changelog.txt $EVOLUXDIR/etc/
cp -RP $E2DIR/etc/hostname $EVOLUXDIR/etc/
cp -RP $E2DIR/etc/image-version $EVOLUXDIR/etc/
cp -RP $E2DIR/etc/localtime $EVOLUXDIR/etc/
cp -RP $E2DIR/etc/rc.d/rc0.d $EVOLUXDIR/etc/rc.d/
cp -RP $E2DIR/etc/rc.d/rc3.d $EVOLUXDIR/etc/rc.d/
cp -RP $E2DIR/etc/rc.d/rc6.d $EVOLUXDIR/etc/rc.d/
cp -RP $E2DIR/etc/init.d/ntpupdate.sh $EVOLUXDIR/etc/init.d/
cp -RP $CURDIR/root/release/rcS_stm23_evolux_spark $EVOLUXDIR/etc/init.d/rcS
cp -RP $E2DIR/lib/modules $EVOLUXDIR/lib/
rm $EVOLUXDIR/etc/.start_enigma2
cd $EVOLUXDIR
$USERS  tar -czvf../release_EVOLUX_with_dev.tar.gz ./
cp -RP ../release_EVOLUX_with_dev.tar.gz $DESTINATION/
$USERS  rm -rf $DESTINATION/enigma2/*
cd $DESTINATION && sudo tar -xzvf release_EVOLUX_with_dev.tar.gz -C $DESTINATION/enigma2/
$USERS  chmod 777 -R $DESTINATION/enigma2
cp -RP $EVOLUXDIR/boot/uImage $PINGUDIR/tufsbox/
myVERSION=`cat $EVOLUXDIR/etc/changelog.txt | grep -m1 Version | cut -d = -f2`
$PINGUDIR/tufsbox/host/bin/mkfs.jffs2 -r $EVOLUXDIR -o $PINGUDIR/tufsbox/e2jffs2.img -e 0x20000 -n
cd $PINGUDIR/tufsbox
tar -czvf EvoLux_on_Pingulux_v$myVERSION.tar.gz e2jffs2.img uImage changelog.txt

