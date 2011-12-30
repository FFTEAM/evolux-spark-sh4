#!/bin/sh
CURDIR=`pwd`
PINGUDIR=${CURDIR%/cvs/cdk}
E2DIR=$PINGUDIR/tufsbox/release_with_dev
NEUTRINODIR=$PINGUDIR/tufsbox/release_neutrino_with_dev
EVOLUXDIR=$PINGUDIR/tufsbox/release_EVOLUX_with_dev
DESTINATION="/devel/E2"
DATE=`date +%Y%m%d`
# originally created by schischu and konfetti
# fedora parts prepared by lareq
# fedora/suse/ubuntu scripts merged by kire pudsje (kpc)

# make sure defines have not already been defined
UBUNTU=
FEDORA=
SUSE=
# Try to detect the distribution
if `which lsb_release > /dev/null 2>&1`; then 
	case `lsb_release -s -i` in
		Debian*) UBUNTU=1; USERS="su -c";;
		Fedora*) FEDORA=1; USERS="sudo";;
		SUSE*)   SUSE=1;   USERS="su";;
		Ubuntu*) UBUNTU=1; USERS="sudo";;
	esac
fi
# Not detected by lsb_release, try release files
if [ -z "$FEDORA$SUSE$UBUNTU" ]; then
	if   [ -f /etc/redhat-release ]; then FEDORA=1; USERS="sudo"; 
	elif [ -f /etc/fedora-release ]; then FEDORA=1; USERS="sudo"; 
	elif [ -f /etc/SuSE-release ];   then SUSE=1; USERS="su";
	elif [ -f /etc/debian_version ]; then UBUNTU=1; USERS="su -c";
	fi
fi
doOptimizeE2() {
	SOURCE="$PINGUDIR/tufsbox/release_with_dev"
	cd /usr/bin
	$USERS ln -sf python2.6 python
	cd $CURDIR
	#py recompilen
	#list=`cat $SCRIPTPATH/startpy.txt`
	list=`find $SOURCE -name "*.py" -print`
	for i in $list
	do
	bn=`basename "$i"`
	if [ "$bn" == "mytest.py" ]; then continue; fi
	dn=`dirname "$i"`
	#x=`find $ARCHIVEPATH -name "$i" -print`
	#bn=`basename "$x"`
	#dn=`dirname "$x"`
	cd $dn
	py_compilefiles $bn
	# python -O /usr/bin/py_compilefiles filename # would compile pyo but pingu e2 dont boot with pyo only by pyc!
	bnc="$bn"c
	if [ -e $bnc ]; then rm $bn; fi
	#rm $dn/$bn
	done
	if [ -e /usr/bin/python2.7 ]; then
		cd /usr/bin
		$USERS ln -sf python2.7 python
	fi
	cd $CURDIR

	rm $SOURCE/etc/enigma2/*dbe*.tv
	rm $SOURCE/etc/enigma2/*.tv_org
	rm $SOURCE/etc/enigma2/*_org
	rm $SOURCE/etc/enigma2/*_tv_.tv
	rm $SOURCE/etc/enigma2/skin_user.xml
	rm $SOURCE/etc/enigma2/extended_satellites.xml
	rm $SOURCE/etc/tuxbox/cables.xml
	rm $SOURCE/etc/tuxbox/terrestrial.xml
	mkdir -p $SOURCE/usr/keys

	echo "config.plugins.PinkPanel.usePPSkin=true" >> $SOURCE/etc/enigma2/settings
	echo "config.skin.primary_skin=PP-MD-Skin/skin.xml" >> $SOURCE/etc/enigma2/settings
	#echo "config.skin.primary_skin=EVO-blackGlass-HD/skin.xml" >> $SOURCE/etc/enigma2/settings
	echo "config.usage.on_long_powerpress=shutdown" >> $SOURCE/etc/enigma2/settings
	echo "config.psi.saturation=135" >> $SOURCE/etc/enigma2/settings
	echo "config.psi.contrast=135" >> $SOURCE/etc/enigma2/settings
	echo "config.psi.brightness=118" >> $SOURCE/etc/enigma2/settings
	echo "config.av.policy_43=scale" >> $SOURCE/etc/enigma2/settings
	#echo "config.av.videomode.HDMI=720p" >> $SOURCE/etc/enigma2/settings
	#echo "config.av.colorformat_hdmi=hdmi_422" >> $SOURCE/etc/enigma2/settings
	#echo "config.av.videoport=HDMI" >> $SOURCE/etc/enigma2/settings
	echo "config.usage.on_short_powerpress=shutdown" >> $SOURCE/etc/enigma2/settings
	echo "config.usage.infobar_timeout=3" >> $SOURCE/etc/enigma2/settings
	echo "config.usage.show_infobar_on_event_change=false" >> $SOURCE/etc/enigma2/settings
	echo "config.usage.show_event_progress_in_servicelist=true" >> $SOURCE/etc/enigma2/settings
	#echo "config.audio.volume=25" >> $SOURCE/etc/enigma2/settings

	mkdir -p $SOURCE/usr/local/share/enigma2/PP-MD-Skin; rm $SOURCE/usr/lib/enigma2/python/Plugins/Extensions/PinkPanel/Skin/PP-MD-Skin/skin.xml; mv $SOURCE/usr/lib/enigma2/python/Plugins/Extensions/PinkPanel/Skin/PP-MD-Skin/skin_pingu.xml $SOURCE/usr/lib/enigma2/python/Plugins/Extensions/PinkPanel/Skin/PP-MD-Skin/skin.xml;for i in $(ls $SOURCE/usr/lib/enigma2/python/Plugins/Extensions/PinkPanel/Skin/PP-MD-Skin); do ln -s "/usr/lib/enigma2/python/Plugins/Extensions/PinkPanel/Skin/PP-MD-Skin/$i" "$SOURCE/usr/local/share/enigma2/PP-MD-Skin/$i"; done

	rm $SOURCE/usr/lib/enigma2/python/Screens/InfoBarGenerics.py
	rm $SOURCE/usr/lib/enigma2/python/Screens/InfoBar.py
	rm $SOURCE/usr/lib/enigma2/python/Components/PPInfoBar.py
	rm $SOURCE/usr/lib/enigma2/python/Plugins/Extensions/PinkPanel/*.py
	rm $SOURCE/usr/lib/enigma2/python/Components/Renderer/chNumber.py


	for i in cs ar et fr hr is lt nl sl sv uk ca da el es fi fy hu it lv pt sk sr tr
	do
	   rm -rf $SOURCE/usr/local/share/enigma2/po/$i
	done

	for i in ad.png dk.png fi.png hu.png lv.png se.png ua.png ae.png ee.png fr.png is.png missing.png pt.png si.png x-fy.png cz.png gr.png it.png nl.png ro.png sk.png yu.png es.png hr.png lt.png tr.png
	do
	   rm -rf $SOURCE/usr/local/share/enigma2/countries/$i
	done

	echo "nameserver 192.168.178.1" > $SOURCE/etc/resolv.conf
	echo "auto lo" > $SOURCE/etc/network/interfaces
	echo "iface lo inet loopback" >> $SOURCE/etc/network/interfaces
	echo "auto eth0" >> $SOURCE/etc/network/interfaces
	echo "iface eth0 inet dhcp" >> $SOURCE/etc/network/interfaces
	if [ -e $SOURCE/etc/changelog.txt ]; then
		VersionPart1=`cat $SOURCE/etc/changelog.txt | grep -m1 Version | cut -d = -f2 | cut -d . -f1`
		VersionPart2=`cat $SOURCE/etc/changelog.txt | grep -m1 Version | cut -d = -f2 | cut -d . -f2`
		VersionPart3=`cat $SOURCE/etc/changelog.txt | grep -m1 Version | cut -d = -f2 | cut -d . -f3`
		echo "version=0${VersionPart1}${VersionPart2}${VersionPart3}${DATE}0000" > $SOURCE/etc/image-version
	fi
#	cp -RP $SOURCE/etc/changelog.txt $PINGUDIR/tufsbox/
}
doOptimizeE2
cd $CURDIR
exit

