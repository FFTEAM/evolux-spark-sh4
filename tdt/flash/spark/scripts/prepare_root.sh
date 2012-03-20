#!/bin/bash
CURDIR=$1
RELEASEDIR=$2
TMPROOTDIR=$3
TMPKERNELDIR=$4

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

$USERS rm -rf $TMPROOTDIR/*
cd $RELEASEDIR
$USERS tar -czvf $RELEASEDIR.tar.gz ./
cd ../$RELEASEDIR
$USERS tar -xzvf $RELEASEDIR.tar.gz -C $TMPROOTDIR
# --- BOOT ---
mv $TMPROOTDIR/boot/uImage $TMPKERNELDIR/uImage


# --- ROOT ---
#cd $TMPROOTDIR/dev/
#MAKEDEV="$USERS $TMPROOTDIR/sbin/MAKEDEV -p $TMPROOTDIR/etc/passwd -g $TMPROOTDIR/etc/group"
#${MAKEDEV} std
#${MAKEDEV} fd
#${MAKEDEV} hda hdb
#${MAKEDEV} sda sdb sdc sdd
#${MAKEDEV} scd0 scd1
#${MAKEDEV} st0 st1
#${MAKEDEV} sg
#${MAKEDEV} ptyp ptyq
#${MAKEDEV} console
#${MAKEDEV} ttyAS0 ttyAS1 ttyAS2 ttyAS3
#${MAKEDEV} lp par audio video fb rtc lirc st200 alsasnd mme bpamem
#${MAKEDEV} ppp busmice
#${MAKEDEV} input i2c mtd
#${MAKEDEV} dvb
cd -

