#!/bin/sh
myCam="oscam"
myOptions="-c /usr/keys"

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
	else
		ismounted=`mount | grep sda1`
		if [ ! -z "$ismounted" ]; then
			umount /dev/sda1
			umounted=`mount | grep sda1`
			if [ -z "$umounted" ]; then
				wget "http://127.0.0.1/web/message?text=USB%20unmounted,%20now%20you%20can%20disconnect%20hdd/stick&type=1&timeout=5" &
			else
				wget "http://127.0.0.1/web/message?text=ERROR&type=3&timeout=5" &
			fi
		else
			mount /dev/sda1 /autofs/sda1
			mount -o bind /autofs/sda1 /media/hdd
			remounted=`mount | grep sda1`
			if [ ! -z "$remounted" ]; then
				wget "http://127.0.0.1/web/message?text=USB%20(re)mounted,%20now%20you%20can%20use%20hdd/stick&type=1&timeout=5" &
			else
				wget "http://127.0.0.1/web/message?text=ERROR&type=3&timeout=5" &
			fi
		fi
	fi
;;

1)
	isCam=`pidof $myCam`;for i in $isCam; do kill $i;done;sleep 5;/usr/bin/${myCam} ${myOptions} &
;;
*)
	exit
;;
esac
exit

