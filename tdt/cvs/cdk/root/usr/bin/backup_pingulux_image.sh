#!/bin/sh
DIRECTORY=$1
#DIRECTORY="/media/hdd"
MTDBOOT=5
MTDROOT=6
#IMAGENAME="Evolution"
IMAGENAME="Evolux-yaffs2"
#ISAVAILABLE=`mount | grep sda1`
#if [ ! -z "$ISAVAILABLE" ]; then
	if grep -qs 'spark' /proc/stb/info/model ; then
		BOXTYPE=spark
		OPTIONS="-e 0x20000 -n"
	else
		echo "Box not found !!!"
		exit 0
	fi


	echo $BOXTYPE " found"

	DATE=`date +%Y%m%d`
#	MKFS=/usr/bin/mkfs.jffs2
	MKFS=/sbin/mkyaffs2
	BACKUPIMAGE="$IMAGENAME-$BOXTYPE-$DATE.img"

	if [ ! -f $MKFS ] ; then
		echo $MKFS" not found"
		exit 0
	fi

	rm -rf "$DIRECTORY/tmp/root"
	mkdir -p "$DIRECTORY/tmp/root"
	if [ ! -e "$DIRECTORY/myBU" ]; then
		mkdir -p "$DIRECTORY/myBU"
	fi

#	mount -t jffs2 /dev/mtdblock$MTDROOT "$DIRECTORY/tmp/root"
	mount -t yaffs2 /dev/mtdblock$MTDROOT "$DIRECTORY/tmp/root"

	echo "Copying uImage"
#	dd if=/dev/mtdblock$MTDBOOT of="/tmp/uImage" bs=1024 count=2560
	cp /boot/uImage "$DIRECTORY/myBU/uImage-$IMAGENAME-$BOXTYPE-$DATE"

#	echo "create root.jffs2"
	echo "create root.yaffs2"
#	$MKFS --root="$DIRECTORY/tmp/root" --faketime --output="$DIRECTORY/tmp/$BACKUPIMAGE" $OPTIONS
	$MKFS -o /sbin/spark_oob.img "$DIRECTORY/tmp/root" "$DIRECTORY/tmp/$BACKUPIMAGE"

	mv "$DIRECTORY/tmp/$BACKUPIMAGE" "$DIRECTORY/myBU/"
#	mv "/tmp/uImage" "$DIRECTORY/myBU/uImage"
	if [ -f "$DIRECTORY/myBU/$BACKUPIMAGE" ] ; then
		echo "$BACKUPIMAGE can be found in: $DIRECTORY/myBU"
	else
		echo "error"
	fi
	sync
	echo "b.r forhike"
	echo "and Dreamy2010"
	umount "$DIRECTORY/tmp/root"
	rm -rf "$DIRECTORY/tmp/root"
#else
#	echo "no hdd/stick found!"
#	exit 0
#fi
exit
