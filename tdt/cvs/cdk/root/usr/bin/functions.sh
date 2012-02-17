#!/bin/sh
case $1 in
0)
	ismounted=`mount | grep sda1`
	if [ ! -z "$ismounted" ]; then
		umount /dev/sda1
	else
		mount /dev/sda1 /autofs/sda1
		mount -o bind /autofs/sda1 /media/hdd
esac
exit

