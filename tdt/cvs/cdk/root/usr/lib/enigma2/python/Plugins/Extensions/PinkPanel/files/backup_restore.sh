#!/bin/sh
DATE=`date +%Y%m%d`
DIRECTORY="/media/hdd"
BUDIR="/tmp/myBU"
mycams='oscam CCcam Scam gbox camd3 incubusCamd mbox mgcamd'

ISHDD=`mount | grep "autofs/sda1"`
if [ ! -z "$ISHDD" ]; then
	case $1 in
		backup)
			if [ ! -e "$DIRECTORY/myBU" ]; then
				mkdir -p "$DIRECTORY/myBU"
			fi
			echo "copy settings, plugs to tmp now..."
			sleep 2
			mkdir -p $BUDIR/etc/enigma2
			mkdir -p $BUDIR/etc/network
			mkdir -p $BUDIR/usr/bin
			mkdir -p $BUDIR/usr/keys
			mkdir -p $BUDIR/usr/lib/enigma2/python/Plugins/Extensions

			for i in $mycams;do
				find /usr/bin -name "$i" -exec cp -f {} $BUDIR/usr/bin/ \;
			done
			cp -RP /usr/keys/* $BUDIR/usr/keys/ >/dev/null 2>&1
			cp -RP /etc/enigma2/settings $BUDIR/etc/enigma2/ >/dev/null 2>&1
			cp -RP /etc/fstab $BUDIR/etc/ >/dev/null 2>&1
			cp -RP /etc/profile $BUDIR/etc/ >/dev/null 2>&1
			cp -RP /etc/resolv.conf $BUDIR/etc/ >/dev/null 2>&1
			cp -RP /etc/network/interfaces $BUDIR/etc/network/ >/dev/null 2>&1
			cp -RP /usr/lib/enigma2/python/Plugins/Extensions/* $BUDIR/usr/lib/enigma2/python/Plugins/Extensions/ >/dev/null 2>&1
			rm -rf $BUDIR/usr/lib/enigma2/python/Plugins/Extensions/PinkPanel >/dev/null 2>&1
			echo "done!"
			sleep 2
			echo "create now $DIRECTORY/myBU/mySysBackup.tar.gz ..."
			sleep 2
			cd $BUDIR
			tar -czvf "$DIRECTORY/myBU/mySysBackup.tar.gz" ./ >/dev/null 2>&1
			cd /
			rm -rf $BUDIR
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
