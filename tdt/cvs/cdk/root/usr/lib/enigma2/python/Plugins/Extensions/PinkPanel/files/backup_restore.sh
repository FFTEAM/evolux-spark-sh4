#!/bin/sh
DATE=`date +%Y%m%d`
DIRECTORY="/media/hdd"
mycams='oscam CCcam Scam gbox camd3 incubusCamd mbox mgcamd'

ISHDD=`mount | grep "$DIRECTORY"`
if [ ! -z "$ISHDD" ]; then
	case $1 in
		backup)
			if [ ! -e "$DIRECTORY/myBU" ]; then
				mkdir -p "$DIRECTORY/myBU"
			fi
			echo "copy settings, plugs to tmp now..."
			sleep 2
			mkdir -p /tmp/myBU/etc/enigma2
			mkdir -p /tmp/myBU/etc/network
			mkdir -p /tmp/myBU/usr/bin
			mkdir -p /tmp/myBU/usr/keys
			mkdir -p /tmp/myBU/usr/lib/enigma2/python/Plugins/Extensions

			for i in $mycams;do
				find /usr/bin -name "$i" -exec cp -f {} /tmp/myBU/usr/bin/ \;
			done
			cp -RP /usr/keys/* /tmp/myBU/usr/keys/ >/dev/null 2>&1
			cp -RP /etc/enigma2/settings /tmp/myBU/etc/enigma2/ >/dev/null 2>&1
			cp -RP /etc/fstab /tmp/myBU/etc/ >/dev/null 2>&1
			cp -RP /etc/profile /tmp/myBU/etc/ >/dev/null 2>&1
			cp -RP /etc/resolv.conf /tmp/myBU/etc/ >/dev/null 2>&1
			cp -RP /etc/network/interfaces /tmp/myBU/etc/network/ >/dev/null 2>&1
			cp -RP /usr/lib/enigma2/python/Plugins/Extensions/* /tmp/myBU/usr/lib/enigma2/python/Plugins/Extensions/ >/dev/null 2>&1
			echo "done!"
			sleep 2
			echo "create now $DIRECTORY/myBU/mySysBackup.tar.gz ..."
			sleep 2
			cd /tmp/myBU
			tar -czvf "$DIRECTORY/myBU/mySysBackup.tar.gz" ./ >/dev/null 2>&1
			cd /
			rm -rf /tmp/myBU
			sync
			echo "Backup finished!"
			;;

		restore)
			if [ -e "$DIRECTORY/myBU/mySysBackup.tar.gz" ]; then
				echo "mySysBackup.tar.gz FOUND, restoring now..."
				sleep 2
				tar -xzvf "$DIRECTORY/myBU/mySysBackup.tar.gz" -C / >/dev/null 2>&1
				sync
				echo "DONE! Pingulux REBOOTING in 30 seconds..."
				sleep 2
				(sleep 30; reboot -f) &
			else
				echo "NO mySysBackup.tar.gz FOUND!!!"
				exit 0
			fi
			;;
		*)
			exit 0
			;;
	esac		
else
	echo "NO HDD/STICK FOUND ON USB!"
	exit 0
fi
exit
