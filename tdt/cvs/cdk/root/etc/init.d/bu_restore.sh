#!bin/sh
case $1 in
	backup)
	DESTINATION=`mount | grep "autofs/sda1"`
	if [ ! -z "$DESTINATION" ]; then
		MYBUPATH="/media/hdd"
	else
		MYBUPATH="/tmp"
	fi
	if [ -e "$MYBUPATH/myBackup" ]; then
		rm -rf "$MYBUPATH/myBackup"
	fi
		mkdir -p "$MYBUPATH/myBackup/etc/tuxbox" \
		"$MYBUPATH/myBackup/usr/script" \
		"$MYBUPATH/myBackup/usr/bin" \
		"$MYBUPATH/myBackup/usr/keys" \
		"$MYBUPATH/myBackup/var/plugins" \
		"$MYBUPATH/myBackup/usr/local/share/config"
	#	"$MYBUPATH/myBackup/etc/enigma2" \
	#	"$MYBUPATH/myBackup/usr/lib/tuxbox/plugins" \
	#	"$MYBUPATH/myBackup/usr/lib/enigma2/python/Plugins/Extensions" \
	#	"$MYBUPATH/myBackup/usr/lib/enigma2/python/Plugins/SystemPlugins" \
	#	"$MYBUPATH/myBackup/usr/local/share/enigma2"
	echo "Start backup.."
	#cp -RP /usr/local/share/enigma2/keymap.xml "$MYBUPATH/myBackup/usr/local/share/enigma2"
	cp -RP /usr/local/share/config/* "$MYBUPATH/myBackup/usr/local/share/config/"
	cp -RP /usr/script/* "$MYBUPATH/myBackup/usr/script/"
	cp -RP /etc/tuxbox/* "$MYBUPATH/myBackup/etc/tuxbox/"
	cp -RP /usr/keys/* "$MYBUPATH/myBackup/usr/keys/"
	cp -RP /var/plugins/* "$MYBUPATH/myBackup/var/plugins/"
	mycams='oscam gbox spcs camd3 incubusCamd mbox mgcamd'
	for i in $mycams;do cp -RP /usr/bin/$i "$MYBUPATH/myBackup/usr/bin/" ;done
	cd "$MYBUPATH/myBackup"
	tar -czvf ../myBackup.tar.gz ./
	rm -rf $MYBUPATH/myBackup/*
	mv "$MYBUPATH/myBackup.tar.gz" "$MYBUPATH/myBackup/"
	echo "backup finished!"
	;;
	restore)
	DESTINATION=`mount | grep sda1`
	if [ ! -z "$DESTINATION" ]; then
		MYBUPATH="/media/hdd"
	else
		MYBUPATH="/tmp"
	fi
	echo "Start restore..."
	tar -xzvf "$MYBUPATH/myBackup"/myBackup.tar.gz -C /
	echo "restore finished"
	;;
esac
exit

