#!/bin/sh
case $1 in
0)
	ismounted=`mount | grep sda1`
	if [ ! -z "$ismounted" ]; then
		umount /dev/sda1
		wget -O /dev/null http://127.0.0.1/control/message?popup="USB unmounted, now you can disconnect hdd/stick" &
	else
		mount /dev/sda1 /autofs/sda1
		mount -o bind /autofs/sda1 /media/hdd
		wget -O /dev/null http://127.0.0.1/control/message?popup="USB (re)mounted, now you can use hdd/stick" &
;;

*)
	exit
;;
esac
exit

