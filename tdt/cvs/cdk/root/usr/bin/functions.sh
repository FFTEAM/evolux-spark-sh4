#!/bin/sh
case $1 in
0)
	if [ ! -e /etc/.start_enigma2 ]; then
		ismounted=`mount | grep sda1`
		if [ ! -z "$ismounted" ]; then
			umount /dev/sda1
			umounted=`mount | grep sda1`
			if [ -z "$umounted" ]; then
				wget -O /dev/null http://127.0.0.1/control/message?popup="USB unmounted, now you can disconnect hdd/stick" &
			else
				wget -O /dev/null http://127.0.0.1/control/message?popup="ERROR" &
			fi
		else
			mount /dev/sda1 /autofs/sda1
			mount -o bind /autofs/sda1 /media/hdd
			remounted=`mount | grep sda1`
			if [ ! -z "$remounted" ]; then
				wget -O /dev/null http://127.0.0.1/control/message?popup="USB (re)mounted, now you can use hdd/stick" &
			else
				wget -O /dev/null http://127.0.0.1/control/message?popup="ERROR" &
			fi
		fi
	fi
;;

*)
	exit
;;
esac
exit

