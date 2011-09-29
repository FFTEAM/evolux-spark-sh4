#!/bin/sh
CURDIR=`pwd`
NRELDIR='../../tufsbox/release_neutrino'
TARGET=`cat $CURDIR/lastChoice | awk -F '--enable-' '{print $5}' | cut -d ' ' -f 1`

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
	tar -xzf $CURDIR/tempdevs.tar.gz -C $NRELDIR/dev/ && rm $CURDIR/tempdevs.tar.gz
}

[ -e $NRELDIR/dev/ ] && [ -e $NRELDIR/dev/vfd ] && exit;

case $TARGET in
	ufs910)
		MAKEDEV="$CURDIR/root/sbin/MAKEDEV"
		make_default
		echo "Erstelle weitere /dev für $TARGET..."
		make_devs
	;;
	flash_ufs910)
		MAKEDEV="$CURDIR/root/sbin/MAKEDEV_flash"
		make_default
		echo "Erstelle weitere /dev für $TARGET..."
		echo "ln -sf /dev/mtdblock2 root" >> $CURDIR/.fakeroot
		make_devs
	;;
	ufs912)
		MAKEDEV="$CURDIR/root/sbin/MAKEDEV"
		make_default
		echo "Erstelle weitere /dev für $TARGET..."
		make_devs
	;;
	ufs922)
		MAKEDEV="$CURDIR/root/sbin/MAKEDEV_dual_tuner"
#		make_default
		echo -e "Erstelle weitere /dev für $TARGET...\nSorry, $TARGET wird derzeit nicht unterstützt."
#		make_devs
	;;
	tf7700)
		MAKEDEV="$CURDIR/root/sbin/MAKEDEV_dual_tuner"
#		make_default
		echo -e "Erstelle weitere /dev für $TARGET...\nSorry, $TARGET wird derzeit nicht unterstützt."
#		make_devs
	;;
	fortis_hdbox)
		MAKEDEV="$CURDIR/root/sbin/MAKEDEV_dual_tuner"
		make_default
		echo "Erstelle weitere /dev für $TARGET..."
		echo "mknod -m 666 rc c 147 1" >> $CURDIR/.fakeroot
		echo "mknod -m 666 sci0 c 253 0" >> $CURDIR/.fakeroot
		echo "mknod -m 666 sci1 c 253 1" >> $CURDIR/.fakeroot
		make_devs
	;;
	hl101)
		MAKEDEV="$CURDIR/root/sbin/MAKEDEV"
#		make_default
		echo -e "Erstelle weitere /dev für $TARGET...\nSorry, $TARGET wird derzeit nicht unterstützt."
#		make_devs
	;;
	vip)
		MAKEDEV="$CURDIR/root/sbin/MAKEDEV_dual_tuner"
#		make_default
		echo -e "Erstelle weitere /dev für $TARGET...\nSorry, $TARGET wird derzeit nicht unterstützt."
#		make_devs
	;;
	cuberevo)
		MAKEDEV="$CURDIR/root/sbin/MAKEDEV_dual_tuner"
#		make_default
		echo -e "Erstelle weitere /dev für $TARGET...\nSorry, $TARGET wird derzeit nicht unterstützt."
#		make_devs
	;;
	cuberevo_mini)
		MAKEDEV="$CURDIR/root/sbin/MAKEDEV"
#		make_default
		echo -e "Erstelle weitere /dev für $TARGET...\nSorry, $TARGET wird derzeit nicht unterstützt."
#		make_devs
	;;
	cuberevo_mini2)
		MAKEDEV="$CURDIR/root/sbin/MAKEDEV"
#		make_default
		echo -e "Erstelle weitere /dev für $TARGET...\nSorry, $TARGET wird derzeit nicht unterstützt."
#		make_devs
	;;
	cuberevo_250hd)
		MAKEDEV="$CURDIR/root/sbin/MAKEDEV_no_CI"
#		make_default
		echo -e "Erstelle weitere /dev für $TARGET...\nSorry, $TARGET wird derzeit nicht unterstützt."
#		make_devs
	;;
	cuberevo_9500hd)
		MAKEDEV="$CURDIR/root/sbin/MAKEDEV_dual_tuner"
#		make_default
		echo -e "Erstelle weitere /dev für $TARGET...\nSorry, $TARGET wird derzeit nicht unterstützt."
#		make_devs
	;;
	cuberevo_2000hd)
		MAKEDEV="$CURDIR/root/sbin/MAKEDEV_no_CI"
#		make_default
		echo -e "Erstelle weitere /dev für $TARGET...\nSorry, $TARGET wird derzeit nicht unterstützt."
#		make_devs
	;;
	cuberevo_mini_fta)
		MAKEDEV="$CURDIR/root/sbin/MAKEDEV_no_CI"
#		make_default
		echo -e "Erstelle weitere /dev für $TARGET...\nSorry, $TARGET wird derzeit nicht unterstützt."
#		make_devs
	;;
	homecast5101)
		MAKEDEV="$CURDIR/root/sbin/MAKEDEV"
#		make_default
		echo -e "Erstelle weitere /dev für $TARGET...\nSorry, $TARGET wird derzeit nicht unterstützt."
#		make_devs
	;;
	octagon1008)
		MAKEDEV="$CURDIR/root/sbin/MAKEDEV"
		make_default
		echo "Erstelle weitere /dev für $TARGET..."
		echo "mknod -m 666 rc c 147 1" >> $CURDIR/.fakeroot
		echo "mknod -m 666 sci0 c 169 0" >> $CURDIR/.fakeroot
		make_devs
	;;
	spark)
		MAKEDEV="$CURDIR/root/sbin/MAKEDEV"
		make_default
		echo "Erstelle weitere /dev für $TARGET..."
		echo "mknod -m 666 rc c 147 1" >> $CURDIR/.fakeroot
		echo "mknod -m 666 sci0 c 169 0" >> $CURDIR/.fakeroot
		make_devs
	;;
	atevio7500)
		MAKEDEV="$CURDIR/root/sbin/MAKEDEV_dual_tuner"
		make_default
		echo "Erstelle weitere /dev für $TARGET..."
		echo "mknod -m 666 rc c 147 1" >> $CURDIR/.fakeroot
		echo "mknod -m 666 sci0 c 169 0" >> $CURDIR/.fakeroot
		echo "mknod -m 666 sci1 c 169 1" >> $CURDIR/.fakeroot
		make_devs
	;;
	spark7162)
		MAKEDEV="$CURDIR/root/sbin/MAKEDEV"
#		make_default
		echo -e "Erstelle weitere /dev für $TARGET...\nSorry, $TARGET wird derzeit nicht unterstützt."
#		make_devs
	;;
	ipbox9900)
		MAKEDEV="$CURDIR/root/sbin/MAKEDEV_dual_tuner"
#		make_default
		echo -e "Erstelle weitere /dev für $TARGET...\nSorry, $TARGET wird derzeit nicht unterstützt."
#		make_devs
	;;
	ipbox99)
		MAKEDEV="$CURDIR/root/sbin/MAKEDEV_no_CI"
#		make_default
		echo -e "Erstelle weitere /dev für $TARGET...\nSorry, $TARGET wird derzeit nicht unterstützt."
#		make_devs
	;;
	ipbox55)
		MAKEDEV="$CURDIR/root/sbin/MAKEDEV_no_CI"
#		make_default
		echo -e "Erstelle weitere /dev für $TARGET...\nSorry, $TARGET wird derzeit nicht unterstützt."
#		make_devs
	;;
	hs7810a)
		MAKEDEV="$CURDIR/root/sbin/MAKEDEV"
#		make_default
		echo -e "Erstelle weitere /dev für $TARGET...\nSorry, $TARGET wird derzeit nicht unterstützt."
#		make_devs
	;;
esac
echo "--- Erledigt ---"
exit
