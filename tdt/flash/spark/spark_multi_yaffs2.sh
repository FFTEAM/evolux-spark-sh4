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

if [ -e $TUFSBOXDIR/release-enigma2-pli-nightly_with_dev/etc/changelog.txt ]; then
	EVOLUXVERSION=`cat $TUFSBOXDIR/release-enigma2-pli-nightly_with_dev/etc/changelog.txt | grep -m1 Version | cut -d = -f2`
	cp -RP $TUFSBOXDIR/release-enigma2-pli-nightly_with_dev/etc/changelog.txt $TUFSBOXDIR/
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
		Debian*) UBUNTU=1; USERS="sudo";;
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
	elif [ -f /etc/debian_version ]; then UBUNTU=1; USERS="sudo";
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
if [ -d $TUFSBOXDIR/release_neutrino_with_dev ]; then
	if [ ! -d $TUFSBOXDIR/release_evolux_with_dev ] && [ ! -d $TUFSBOXDIR/release_evolux_pli_with_dev ] && [ ! -d $TUFSBOXDIR/release_with_dev ] && [ ! -d $TUFSBOXDIR/release ] && [ ! -d $TUFSBOXDIR/release-enigma2-pli-nightly_with_dev ]; then
		echo "   1) Prepare Neutrino     yaffs2"
	fi
fi
if [ -d $TUFSBOXDIR/release_with_dev ]; then
	if [ ! -d $TUFSBOXDIR/release_evolux_with_dev ] && [ ! -d $TUFSBOXDIR/release_evolux_pli_with_dev ]; then
		echo "   2) Prepare Enigma2      yaffs2"
	fi
fi
if [ -d $TUFSBOXDIR/release-enigma2-pli-nightly_with_dev ]; then
	if [ ! -d $TUFSBOXDIR/release_evolux_with_dev ] && [ ! -d $TUFSBOXDIR/release_evolux_pli_with_dev ]; then
		echo "   3) Prepare Enigma2-PLI  yaffs2"
	fi
fi
if [ -d $TUFSBOXDIR/release_evolux_with_dev ]; then
	echo "   4) Prepare Evolux       yaffs2"
fi
if [ -d $TUFSBOXDIR/release_evolux_pli_with_dev ]; then
	echo "   5) Prepare Evolux-PLI   yaffs2"
fi
read -t 10 -p "Select target (autoskip in 10s)? "
case "$REPLY" in
	0)  echo "Skipping...";;
	1)  echo "Creating Ntrino  yaffs2 and uImage..."
		./mkyaffs2 -o ./spark_oob.img $TUFSBOXDIR/release_neutrino_with_dev $CURDIR/out/e2yaffs2.img
		cp $TUFSBOXDIR/release_neutrino_with_dev/boot/uImage $CURDIR/out/uImage
		cp -RP $OUTDIR/* $TUFSBOXDIR/
		cd $TUFSBOXDIR && tar -czvf Neutrino-YAFFS2.tar.gz e2yaffs2.img uImage
		cd $CURDIR
		echo "-----------------------------------------------------------------------";;
	2)  echo "Creating E2 yaffs2 and uImage..."
		./mkyaffs2 -o ./spark_oob.img $TUFSBOXDIR/release_with_dev $CURDIR/out/e2yaffs2.img
		cp $TUFSBOXDIR/release_with_dev/boot/uImage $CURDIR/out/uImage
		cp -RP $OUTDIR/* $TUFSBOXDIR/
		cd $TUFSBOXDIR && tar -czvf E2-YAFFS2.tar.gz e2yaffs2.img uImage
		cd $CURDIR
		echo "-----------------------------------------------------------------------";;
	3)  echo "Creating E2-PLI yaffs2 and uImage..."
		./mkyaffs2 -o ./spark_oob.img $TUFSBOXDIR/release-enigma2-pli-nightly_with_dev $CURDIR/out/e2yaffs2.img
		cp $TUFSBOXDIR/release-enigma2-pli-nightly_with_dev/boot/uImage $CURDIR/out/uImage
		cp -RP $OUTDIR/* $TUFSBOXDIR/
		cd $TUFSBOXDIR && tar -czvf E2-YAFFS2.tar.gz e2yaffs2.img uImage
		cd $CURDIR
		echo "-----------------------------------------------------------------------";;
	4)  echo "Creating Evolux  yaffs2 and uImage..."
		./mkyaffs2 -o ./spark_oob.img $TUFSBOXDIR/release_evolux_with_dev $CURDIR/out/e2yaffs2.img
		cp $TUFSBOXDIR/release_evolux_with_dev/boot/uImage $CURDIR/out/uImage
		cp -RP $OUTDIR/* $TUFSBOXDIR/
		cd $TUFSBOXDIR && tar -czvf EvoLux_on_Pingulux_v$EVOLUXVERSION-YAFFS2.tar.gz e2yaffs2.img uImage changelog.txt howto_flash_yaffs2_new3.txt flash_E2_yaffs2.sh BootargsPack Evolux-Orig-Spark-BootPlugin
		cd $CURDIR
		echo "-----------------------------------------------------------------------";;
	5)  echo "Creating Evolux-PLI  yaffs2 and uImage..."
		./mkyaffs2 -o ./spark_oob.img $TUFSBOXDIR/release_evolux_pli_with_dev $CURDIR/out/e2yaffs2.img
		cp $TUFSBOXDIR/release_evolux_pli_with_dev/boot/uImage $CURDIR/out/uImage
		cp -RP $OUTDIR/* $TUFSBOXDIR/
		cd $TUFSBOXDIR && tar -czvf EvoLux_on_Pingulux_v$EVOLUXVERSION-YAFFS2.tar.gz e2yaffs2.img uImage changelog.txt howto_flash_yaffs2_new3.txt flash_E2_yaffs2.sh BootargsPack Evolux-Orig-Spark-BootPlugin
		cd $CURDIR
		echo "-----------------------------------------------------------------------";;
	*)  echo "Skipping...";;
esac

echo "-----------------------------------------------------------------------"
echo "Flashimage created:"
echo `ls $OUTDIR`


