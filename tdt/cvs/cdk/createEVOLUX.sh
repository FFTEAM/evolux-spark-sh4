#! /bin/sh
CURDIR=`pwd`
PINGUDIR=${CURDIR%/cvs/cdk}
E2DIR=$PINGUDIR/tufsbox/release_with_dev
NEUTRINODIR=$PINGUDIR/tufsbox/release_neutrino_with_dev
EVOLUXDIR=$PINGUDIR/tufsbox/release_EVOLUX_with_dev
DESTINATION="/devel/E2"
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
sudo rm -rf $EVOLUXDIR/*
cd $NEUTRINODIR
sudo tar -czvf $PINGUDIR/tufsbox/release_neutrino_with_dev_for_EVOLUX.tar.gz ./
sudo tar -xzvf $PINGUDIR/tufsbox/release_neutrino_with_dev_for_EVOLUX.tar.gz -C $EVOLUXDIR/
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
cp -RP $E2DIR/etc/init.d/ntpupdate.sh $EVOLUXDIR/etc/init.d/
cp -RP $CURDIR/root/release/rcS_stm23_evolux_spark $EVOLUXDIR/etc/init.d/
cd $EVOLUXDIR
sudo tar -czvf../release_EVOLUX_with_dev.tar.gz ./
cp -RP ../release_EVOLUX_with_dev.tar.gz $DESTINATION/
sudo rm -rf $DESTINATION/enigma2/*
cd $DESTINATION && sudo tar -xzvf release_EVOLUX_with_dev.tar.gz -C $DESTINATION/enigma2/
sudo chmod 777 -R $DESTINATION/enigma2
cp -RP $EVOLUXDIR/boot/uImage $PINGUDIR/tufsbox/
myVERSION=`cat $EVOLUXDIR/etc/changelog.txt | grep -m1 Version | cut -d = -f2`
$PINGUDIR/tufsbox/host/bin/mkfs.jffs2 -r $EVOLUXDIR -o $PINGUDIR/tufsbox/e2jffs2.img -e 0x20000 -n
cd $PINGUDIR/tufsbox
tar -czvf EvoLux_on_Pingulux_v$myVERSION.tar.gz e2jffs2.img uImage changelog.txt

