#!/bin/sh
CURDIR=`pwd`
BASEDIR=$CURDIR/../..
TUFSBOXDIR=$BASEDIR/tufsbox
CDKDIR=$BASEDIR/cvs/cdk
SCRIPTDIR=$CURDIR/scripts
TMPDIR=$CURDIR/tmp
TMPROOTDIR=$TMPDIR/ROOT
TMPKERNELDIR=$TMPDIR/KERNEL
OUTDIR=$CURDIR/out

mkdir -p $OUTDIR >/dev/null 2>&1

if [ -e $TUFSBOXDIR/release_with_dev/etc/changelog.txt ]; then
	EVOLUXVERSION=`cat $TUFSBOXDIR/release_with_dev/etc/changelog.txt | grep -m1 Version | cut -d = -f2`
	cp -RP $TUFSBOXDIR/release_with_dev/etc/changelog.txt $TUFSBOXDIR/
fi

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

rm $CURDIR/out/*

if [ $# == "0" ]; then
  if [  -e $TMPDIR ]; then
    $USERS rm -rf $TMPDIR/*
  else
    mkdir $TMPDIR
  fi
fi

mkdir $TMPROOTDIR
mkdir $TMPKERNELDIR

echo "This script creates flashable images for Spark"
echo "Author: Schischu"
echo "Date: 05-01-2012"
echo "-----------------------------------------------------------------------"
echo "It's expected that a images was already build prior to this execution!"
echo "-----------------------------------------------------------------------"

echo "Checking targets..."
echo "Found targets:"
echo "   0) Skipping..."
if [ -e $TUFSBOXDIR/release_neutrino_with_dev ]; then
	echo "   1) Prepare Neutrino jffs2"
fi
if [ -e $TUFSBOXDIR/release_with_dev ]; then
	echo "   2) Prepare Enigma2  jffs2"
fi
if [ -e $TUFSBOXDIR/release_evolux_with_dev ]; then
	echo "   3) Prepare Evolux   jffs2"
fi
echo "----------------------------"

read -p "Select target (0-3)? "
case "$REPLY" in
	0)  echo "Skipping...";;
	1)  echo "Preparing Neutrino jffs2..."
		$SCRIPTDIR/prepare_root.sh $CURDIR $TUFSBOXDIR/release_neutrino_with_dev $TMPROOTDIR $TMPKERNELDIR
		echo "-----------------------------------------------------------------------"
		echo "Creating Ntrino jffs2 and uImage..."
		$SCRIPTDIR/flash_part_w_fw.sh $CURDIR $TUFSBOXDIR $OUTDIR $TMPKERNELDIR $TMPROOTDIR
		cp -RP $OUTDIR/* $TUFSBOXDIR/
		cd $TUFSBOXDIR && tar -czvf Ntrino-JFFS2.tar.gz e2jffs2.img uImage
		cd $CURDIR
		echo "-----------------------------------------------------------------------";;
	2)  echo "Preparing Enigma2 jffs2..."
		$SCRIPTDIR/prepare_root.sh $CURDIR $TUFSBOXDIR/release_with_dev $TMPROOTDIR $TMPKERNELDIR
		echo "-----------------------------------------------------------------------"
		echo "Creating Enigma2 jffs2 and uImage..."
		$SCRIPTDIR/flash_part_w_fw.sh $CURDIR $TUFSBOXDIR $OUTDIR $TMPKERNELDIR $TMPROOTDIR
		cp -RP $OUTDIR/* $TUFSBOXDIR/
		cd $TUFSBOXDIR && tar -czvf E2-JFFS2.tar.gz e2jffs2.img uImage
		cd $CURDIR
		echo "-----------------------------------------------------------------------";;
	3)  echo "Preparing Evolux jffs2..."
		$SCRIPTDIR/prepare_root.sh $CURDIR $TUFSBOXDIR/release_evolux_with_dev $TMPROOTDIR $TMPKERNELDIR
		echo "-----------------------------------------------------------------------"
		echo "Creating Evolux jffs2 and uImage..."
		$SCRIPTDIR/flash_part_w_fw.sh $CURDIR $TUFSBOXDIR $OUTDIR $TMPKERNELDIR $TMPROOTDIR
		cp -RP $OUTDIR/* $TUFSBOXDIR/
		cd $TUFSBOXDIR && tar -czvf EvoLux_on_Pingulux_v$EVOLUXVERSION-JFFS2.tar.gz e2jffs2.img uImage changelog.txt howto_flash_yaffs2_new3.txt flash_E2_yaffs2.sh BootargsPack Evolux-Orig-Spark-BootPlugin
		cd $CURDIR
		echo "-----------------------------------------------------------------------";;
	*)  "Invalid Input! Exiting..."
		exit 2;;
esac

echo "-----------------------------------------------------------------------"
echo "Flashimage created:"
echo `ls $OUTDIR`

echo "-----------------------------------------------------------------------"
echo "To flash the created image rename the *.img file to e2jffs2.img and "
echo "copy it and the uImage to the enigma2 folder (/enigma2) of your usb drive."
echo "Before flashing make sure that enigma2 is the default system on your box."
echo "To set enigma2 as your default system press OK for 5 sec on your box "
echo "while the box is starting. As soon as \"FoYc\" is being displayed press"
echo "DOWN (v) on your box to select \"ENIG\" and press OK to confirm"
echo "To start the flashing process press OK for 5 sec on your box "
echo "while the box is starting. As soon as \"Fact\" is being displayed press"
echo "RIGHT (->) on your box to start the update"
echo ""

