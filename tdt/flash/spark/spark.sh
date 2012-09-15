#!/bin/sh
#
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

if [ -e $TUFSBOXDIR/release_vdrdev2_with_dev/etc/changelog.txt ]; then
	EVOLUXVERSION=`cat $TUFSBOXDIR/release_vdrdev2_with_dev/etc/changelog.txt | grep -m1 Version | cut -d = -f2`
	cp -RP $TUFSBOXDIR/release_vdrdev2_with_dev/etc/changelog.txt $TUFSBOXDIR/
fi

if [ -e $TUFSBOXDIR/release-neutrino-hd_with_dev/etc/changelog.txt ]; then
	EVOLUXVERSION=`cat $TUFSBOXDIR/release_with_dev/etc/changelog.txt | grep -m1 Version | cut -d = -f2`
	cp -RP $TUFSBOXDIR/release-neutrino-hd_with_dev/etc/changelog.txt $TUFSBOXDIR/
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
	if [ ! -d $TUFSBOXDIR/release_evolux_with_dev ] && [ ! -d $TUFSBOXDIR/release_evolux_pli_with_dev ] && [ ! -d $TUFSBOXDIR/release_with_dev ] && [ ! -d $TUFSBOXDIR/release ] && [ ! -d $TUFSBOXDIR/release-enigma2-pli-nightly_with_dev ] && [ ! -d $TUFSBOXDIR/release_neutrino-hd_with_dev ] && [ ! -d $TUFSBOXDIR/release_evolux_neutrino-hd_pli_with_dev ] && [ ! -d $TUFSBOXDIR/release_evolux-neutrino-hd_vdr2_with_dev ]; then
		echo "   1) Prepare Ntrino     jffs2"
	fi
fi
if [ -d $TUFSBOXDIR/release_neutrino-hd_with_dev ]; then
	if [ ! -d $TUFSBOXDIR/release_evolux_with_dev ] && [ ! -d $TUFSBOXDIR/release_evolux_pli_with_dev ] && [ ! -d $TUFSBOXDIR/release_with_dev ] && [ ! -d $TUFSBOXDIR/release ] && [ ! -d $TUFSBOXDIR/release-enigma2-pli-nightly_with_dev ] && [ ! -d $TUFSBOXDIR/release_neutrino_with_dev ] && [ ! -d $TUFSBOXDIR/release_evolux_neutrino-hd_pli_with_dev ] && [ ! -d $TUFSBOXDIR/release_evolux-neutrino-hd_vdr2_with_dev ]; then
		echo "   2) Prepare Ntrino-HD     jffs2"
	fi
fi
if [ -d $TUFSBOXDIR/release_with_dev ]; then
	if [ ! -d $TUFSBOXDIR/release_evolux_with_dev ] && [ ! -d $TUFSBOXDIR/release_evolux_pli_with_dev ] && [ ! -d $TUFSBOXDIR/release_evolux_neutrino-hd_pli_with_dev ] && [ ! -d $TUFSBOXDIR/release_evolux-neutrino-hd_vdr2_with_dev ]; then
		echo "   3) Prepare Enigma2      jffs2"
	fi
fi
if [ -d $TUFSBOXDIR/release-enigma2-pli-nightly_with_dev ]; then
	if [ ! -d $TUFSBOXDIR/release_evolux_with_dev ] && [ ! -d $TUFSBOXDIR/release_evolux_pli_with_dev ] && [ ! -d $TUFSBOXDIR/release_evolux_neutrino-hd_pli_with_dev ] && [ ! -d $TUFSBOXDIR/release_evolux-neutrino-hd_vdr2_with_dev ]; then
		echo "   4) Prepare Enigma2-PLI  jffs2"
	fi
fi
if [ -d $TUFSBOXDIR/release_evolux_with_dev ]; then
	echo "   5) Prepare Evolux       jffs2"
fi
if [ -d $TUFSBOXDIR/release_evolux_pli_with_dev ]; then
	echo "   6) Prepare Evolux-PLI   jffs2"
fi
if [ -d $TUFSBOXDIR/release_evolux_neutrino-hd_pli_with_dev ]; then
	echo "   7) Prepare Evolux-NTRINO-HD-PLI   jffs2"
fi
if [ -d $TUFSBOXDIR/release_vdrdev2_with_dev ]; then
	if [ ! -d $TUFSBOXDIR/release_evolux_with_dev ] && [ ! -d $TUFSBOXDIR/release_evolux_pli_with_dev ] && [ ! -d $TUFSBOXDIR/release_with_dev ] && [ ! -d $TUFSBOXDIR/release ] && [ ! -d $TUFSBOXDIR/release-enigma2-pli-nightly_with_dev ] && [ ! -d $TUFSBOXDIR/release_neutrino-hd_with_dev ] && [ ! -d $TUFSBOXDIR/release_evolux_neutrino-hd_pli_with_dev ] && [ ! -d $TUFSBOXDIR/release_neutrino_with_dev ] && [ ! -d $TUFSBOXDIR/release_evolux_neutrino-hd_vdr2_with_dev ]; then
		echo "   8) Prepare VDR2     jffs2"
	fi
fi
if [ -d $TUFSBOXDIR/release_evolux-neutrino-hd_vdr2_with_dev ]; then
	echo "   9) Prepare Evolux-NTRINO-HD-VDR2   jffs2"
fi
if [ -d $TUFSBOXDIR/release_evolux_triple_with_dev ]; then
	echo "   10) Prepare Evolux-TRIPLE   jffs2"
fi
echo "----------------------------"

read -t 10 -p "Select target (autoskip in 10s)? "
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
	2)  echo "Preparing Neutrino-HD jffs2..."
		$SCRIPTDIR/prepare_root.sh $CURDIR $TUFSBOXDIR/release_neutrino-hd_with_dev $TMPROOTDIR $TMPKERNELDIR
		echo "-----------------------------------------------------------------------"
		echo "Creating Ntrino-HD jffs2 and uImage..."
		$SCRIPTDIR/flash_part_w_fw.sh $CURDIR $TUFSBOXDIR $OUTDIR $TMPKERNELDIR $TMPROOTDIR
		cp -RP $OUTDIR/* $TUFSBOXDIR/
		cd $TUFSBOXDIR && tar -czvf NtrinoHD-JFFS2.tar.gz e2jffs2.img uImage
		cd $CURDIR
		echo "-----------------------------------------------------------------------";;
	3)  echo "Preparing Enigma2 jffs2..."
		$SCRIPTDIR/prepare_root.sh $CURDIR $TUFSBOXDIR/release_with_dev $TMPROOTDIR $TMPKERNELDIR
		echo "-----------------------------------------------------------------------"
		echo "Creating Enigma2 jffs2 and uImage..."
		$SCRIPTDIR/flash_part_w_fw.sh $CURDIR $TUFSBOXDIR $OUTDIR $TMPKERNELDIR $TMPROOTDIR
		cp -RP $OUTDIR/* $TUFSBOXDIR/
		cd $TUFSBOXDIR && tar -czvf E2-JFFS2.tar.gz e2jffs2.img uImage
		cd $CURDIR
		echo "-----------------------------------------------------------------------";;
	4)  echo "Preparing Enigma2-PLI jffs2..."
		$SCRIPTDIR/prepare_root.sh $CURDIR $TUFSBOXDIR/release-enigma2-pli-nightly_with_dev $TMPROOTDIR $TMPKERNELDIR
		echo "-----------------------------------------------------------------------"
		echo "Creating Enigma2 jffs2 and uImage..."
		$SCRIPTDIR/flash_part_w_fw.sh $CURDIR $TUFSBOXDIR $OUTDIR $TMPKERNELDIR $TMPROOTDIR
		cp -RP $OUTDIR/* $TUFSBOXDIR/
		cd $TUFSBOXDIR && tar -czvf E2-JFFS2.tar.gz e2jffs2.img uImage
		cd $CURDIR
		echo "-----------------------------------------------------------------------";;
	5)  echo "Preparing Evolux jffs2..."
		$SCRIPTDIR/prepare_root.sh $CURDIR $TUFSBOXDIR/release_evolux_with_dev $TMPROOTDIR $TMPKERNELDIR
		echo "-----------------------------------------------------------------------"
		echo "Creating Evolux jffs2 and uImage..."
		$SCRIPTDIR/flash_part_w_fw.sh $CURDIR $TUFSBOXDIR $OUTDIR $TMPKERNELDIR $TMPROOTDIR
		cp -RP $OUTDIR/* $TUFSBOXDIR/
		cd $TUFSBOXDIR && tar -czvf EvoLux_on_Pingulux_v$EVOLUXVERSION-JFFS2.tar.gz e2jffs2.img uImage changelog.txt howto_flash_yaffs2_new3.txt flash_E2_yaffs2.sh BootargsPack Evolux-Orig-Spark-BootPlugin
		cd $CURDIR
		echo "-----------------------------------------------------------------------";;
	6)  echo "Preparing Evolux-PLI jffs2..."
		$SCRIPTDIR/prepare_root.sh $CURDIR $TUFSBOXDIR/release_evolux_pli_with_dev $TMPROOTDIR $TMPKERNELDIR
		echo "-----------------------------------------------------------------------"
		echo "Creating Evolux-PLI jffs2 and uImage..."
		$SCRIPTDIR/flash_part_w_fw.sh $CURDIR $TUFSBOXDIR $OUTDIR $TMPKERNELDIR $TMPROOTDIR
		cp -RP $OUTDIR/* $TUFSBOXDIR/
		cd $TUFSBOXDIR && tar -czvf EvoLux_on_Pingulux_v$EVOLUXVERSION-JFFS2.tar.gz e2jffs2.img uImage changelog.txt howto_flash_yaffs2_new3.txt flash_E2_yaffs2.sh BootargsPack Evolux-Orig-Spark-BootPlugin
		cd $CURDIR
		echo "-----------------------------------------------------------------------";;
	7)  echo "Preparing Evolux-NTRINO-HD-PLI jffs2..."
		$SCRIPTDIR/prepare_root.sh $CURDIR $TUFSBOXDIR/release_evolux_neutrino-hd_pli_with_dev $TMPROOTDIR $TMPKERNELDIR
		echo "-----------------------------------------------------------------------"
		echo "Creating Evolux-NTRINO-HD-PLI jffs2 and uImage..."
		$SCRIPTDIR/flash_part_w_fw.sh $CURDIR $TUFSBOXDIR $OUTDIR $TMPKERNELDIR $TMPROOTDIR
		cp -RP $OUTDIR/* $TUFSBOXDIR/
		cd $TUFSBOXDIR && tar -czvf EvoLux_on_Pingulux_v$EVOLUXVERSION-JFFS2.tar.gz e2jffs2.img uImage changelog.txt howto_flash_yaffs2_new3.txt flash_E2_yaffs2.sh BootargsPack Evolux-Orig-Spark-BootPlugin
		cd $CURDIR
		echo "-----------------------------------------------------------------------";;
	8)  echo "Preparing VDR2 jffs2..."
		$SCRIPTDIR/prepare_root.sh $CURDIR $TUFSBOXDIR/release_vdrdev2_with_dev $TMPROOTDIR $TMPKERNELDIR
		echo "-----------------------------------------------------------------------"
		echo "Creating VDR2 jffs2 and uImage..."
		$SCRIPTDIR/flash_part_w_fw.sh $CURDIR $TUFSBOXDIR $OUTDIR $TMPKERNELDIR $TMPROOTDIR
		cp -RP $OUTDIR/* $TUFSBOXDIR/
		cd $TUFSBOXDIR && tar -czvf VDR2-JFFS2.tar.gz e2jffs2.img uImage
		cd $CURDIR
		echo "-----------------------------------------------------------------------";;
	9)  echo "Preparing Evolux-NTRINO-HD-VDR2 jffs2..."
		$SCRIPTDIR/prepare_root.sh $CURDIR $TUFSBOXDIR/release_evolux-neutrino-hd_vdr2_with_dev $TMPROOTDIR $TMPKERNELDIR
		echo "-----------------------------------------------------------------------"
		echo "Creating Evolux-NTRINO-HD-VDR2 jffs2 and uImage..."
		$SCRIPTDIR/flash_part_w_fw.sh $CURDIR $TUFSBOXDIR $OUTDIR $TMPKERNELDIR $TMPROOTDIR
		cp -RP $OUTDIR/* $TUFSBOXDIR/
		cd $TUFSBOXDIR && tar -czvf EvoLux-VDR2_on_Pingulux_v$EVOLUXVERSION-JFFS2.tar.gz e2jffs2.img uImage changelog.txt howto_flash_yaffs2_new3.txt flash_E2_yaffs2.sh BootargsPack Evolux-Orig-Spark-BootPlugin
		cd $CURDIR
		echo "-----------------------------------------------------------------------";;
	10)  echo "Preparing Evolux-TRIPLE jffs2..."
		$SCRIPTDIR/prepare_root.sh $CURDIR $TUFSBOXDIR/release_evolux_triple_with_dev $TMPROOTDIR $TMPKERNELDIR
		echo "-----------------------------------------------------------------------"
		echo "Creating Evolux-TRIPLE jffs2 and uImage..."
		$SCRIPTDIR/flash_part_w_fw.sh $CURDIR $TUFSBOXDIR $OUTDIR $TMPKERNELDIR $TMPROOTDIR
		cp -RP $OUTDIR/* $TUFSBOXDIR/
		cd $TUFSBOXDIR && tar -czvf EvoLux_TRIPLE_v$EVOLUXVERSION-JFFS2.tar.gz e2jffs2.img uImage changelog.txt howto_flash_yaffs2_new3.txt flash_E2_yaffs2.sh BootargsPack Evolux-Orig-Spark-BootPlugin
		cd $CURDIR
		echo "-----------------------------------------------------------------------";;
	*)  echo "Skipping...";;
esac

echo "-----------------------------------------------------------------------"
echo "Flashimage created:"
echo `ls $OUTDIR`

