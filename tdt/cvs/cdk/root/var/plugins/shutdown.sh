#!/bin/sh
echo "SHUTDOWN"
sync
HDDON=`mount | grep sda1 | cut -d " " -f5`
if [ ! -z "$HDDON" ] && [ -e /etc/.fsck ]; then
	echo "hdd-check"
	echo "FSCK" > /dev/vfd
	umount /dev/sda1
	fsck.$HDDON -y /dev/sda1
fi
init 0
exit

