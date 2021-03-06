#!/bin/bash

if [ "$1" == -h ] || [ "$1" == --help ]; then
 echo "Parameter 1: target system (1-26)"
 echo "Parameter 2: kernel (1-9)"
 echo "Parameter 3: debug (Y/N)"
 echo "Parameter 4: player(1-2)"
 echo "Parameter 5: Multicom(1-2)"
 echo "Parameter 6: Media Framwork(1-2)"
 exit
fi

CURDIR=`pwd`
GMDIR=${CURDIR%/cvs/cdk}
export PATH=/usr/sbin:/sbin:$PATH

CONFIGPARAM=" \
 --enable-maintainer-mode \
 --prefix=$GMDIR/tufsbox \
 --with-cvsdir=$GMDIR/cvs \
 --with-customizationsdir=$GMDIR/custom \
 --enable-nfsserver \
 --enable-ccache"

##############################################
clear
echo "
  _______                     _____              _     _         _
 |__   __|                   |  __ \            | |   | |       | |
    | | ___  __ _ _ __ ___   | |  | |_   _  ____| | __| |_  __ _| | ___ ___
    | |/ _ \/ _\` | '_ \` _ \  | |  | | | | |/  __| |/ /| __|/ _\` | |/ _ | __|
    | |  __/ (_| | | | | | | | |__| | |_| |  (__|   < | |_| (_| | |  __|__ \\
    |_|\___|\__,_|_| |_| |_| |_____/ \__,_|\____|_|\_\ \__|\__,_|_|\___|___/

"

##############################################

# config.guess generates different answers for some packages
# Ensure that all packages use the same host by explicitly specifying it.

# First obtain the triplet
AM_VER=`automake --version | awk '{print $NF}' | grep -oEm1 "^[0-9]+.[0-9]+"`
host_alias=`/usr/share/automake-${AM_VER}/config.guess`

# Then undo Suse specific modifications, no harm to other distribution
case `echo ${host_alias} | cut -d '-' -f 1` in
  i?86) VENDOR=pc ;;
  *   ) VENDOR=unknown ;;
esac
host_alias=`echo ${host_alias} | sed -e "s/suse/${VENDOR}/"`

# And add it to the config parameters.
CONFIGPARAM="${CONFIGPARAM} --host=${host_alias} --build=${host_alias}"

##############################################
#if [ -e "$GMDIR/cvs/driver/player2_191" ]; then
#	rm "$GMDIR/cvs/driver/player2_191"
#fi
#if [ -e "$GMDIR/cvs/driver/frontcontroller/aotom" ]; then
#	rm "$GMDIR/cvs/driver/frontcontroller/aotom"
#fi
if [ -e "$GMDIR/cvs/cdk/.spark7162" ]; then
	rm "$GMDIR/cvs/cdk/.spark7162"
fi
if [ -e "$GMDIR/cvs/cdk/.p0211" ]; then
	rm "$GMDIR/cvs/cdk/.p0211"
fi
echo -e "Build SPARK STM24-209/210/211 Player191 now...\n"
read -p "Build for TRIPLEX(SPARK7162) (y/N)? "
if [ "$REPLY" == "y" ] || [ "$REPLY" == "Y" ]; then
	TARGET="--enable-spark7162"
	touch "$GMDIR/cvs/cdk/.spark7162"
	STMFBVERSION="0104"
#	cd "$GMDIR/cvs/driver/frontcontroller" && ln -sf aotom_spark7162 aotom
#	cd "$GMDIR/cvs/driver" && ln -sf player2_191_spark7162 player2_191
else
	TARGET="--enable-spark"
	#STMFBVERSION="0102"
	STMFBVERSION="0104"
#	cd "$GMDIR/cvs/driver/frontcontroller" && ln -sf aotom_spark aotom
#	cd "$GMDIR/cvs/driver" && ln -sf player2_191_spark player2_191
fi
cd $CURDIR
CONFIGPARAM="$CONFIGPARAM $TARGET"

##############################################

PLAYER="--enable-player191"
       cd ../driver/include/
       if [ -L player2 ]; then
          rm player2
       fi

       if [ -L stmfb ]; then
          rm stmfb
       fi
       ln -s player2_179 player2
       ln -s stmfb-3.1_stm24_$STMFBVERSION stmfb
       cd -  >/dev/null 2>&1

       cd ../driver/
       if [ -L player2 ]; then
          rm player2
       fi
       ln -s player2_191 player2
       echo "export CONFIG_PLAYER_191=y" >> .config
       cd -  >/dev/null 2>&1

       cd ../driver/stgfb
       if [ -L stmfb ]; then
          rm stmfb
       fi
       ln -s stmfb-3.1_stm24_$STMFBVERSION stmfb
       cd -  >/dev/null 2>&1

##############################################

##############################################
if [ ! -e "$GMDIR/cvs/cdk/.spark7162" ]; then
	read -p "Activate debug (SPARK ONLY) (y/N)? "
	if [ "$REPLY" == "y" ] || [ "$REPLY" == "Y" ]; then
		CONFIGPARAM="$CONFIGPARAM --enable-debug"
	else
		CONFIGPARAM="$CONFIGPARAM"
	fi
else
	echo "debug disabled"
	CONFIGPARAM="$CONFIGPARAM"
fi
##############################################
echo ""
echo -e "\nSTB :"
echo -e "\033[1;37;40m    1) STM24-209 (old)\033[0m"
echo -e "\033[1;37;42m    2) STM24-210 (standard)\033[0m"
#if [ -e "$GMDIR/cvs/cdk/.spark7162" ]; then
	echo -e "\033[1;37;43m    3) STM24-211 (experimental!)\033[0m"
#fi
read -p "Select STB (1-3)? "
case "$REPLY" in
	1) echo -e "\nSelected STB: $REPLY\n"
	   KERNEL="--enable-stm24 --enable-p0209";;
	2) echo -e "\nSelected Filesystem: $REPLY\n"
	   KERNEL="--enable-stm24 --enable-p0210";;
	3) echo -e "\nSelected Filesystem: $REPLY\n"
	   KERNEL="--enable-stm24 --enable-p0211"
	   touch "$GMDIR/cvs/cdk/.p0211";;
	*) echo -e "\nSelected Filesystem: $REPLY\n"
	   KERNEL="--enable-stm24 --enable-p0210";;
esac
#KERNEL="--enable-stm24 --enable-p0209"
CONFIGPARAM="$CONFIGPARAM $KERNEL"

##############################################

cd ../driver/
echo "# Automatically generated config: don't edit" > .config
echo "#" >> .config
echo "export CONFIG_ZD1211REV_B=y" >> .config
echo "export CONFIG_ZD1211=n"		>> .config
cd - >/dev/null 2>&1

MULTICOM="--enable-multicom324"
cd ../driver/include/
if [ -L multicom ]; then
	rm multicom
fi
ln -s ../multicom-3.2.4/include multicom
cd - >/dev/null 2>&1
cd ../driver/
if [ -L multicom ]; then
	rm multicom
fi
ln -s multicom-3.2.4 multicom
echo "export CONFIG_MULTICOM324=y" >> .config
cd - >/dev/null 2>&1

##############################################

echo -e "\nMedia Framework-NHD2:"
echo -e "\033[1;37;42m    1) eplayer3  (for NHD2, RECOMMENDED!)\033[0m"
echo -e "\033[1;37;43m    2) gstreamer (for NHD2, EXPERIMENTAL)\033[0m"
read -p "Select media framework (1-2)? "
case "$REPLY" in
	1) MEDIAFWNHD2=""
	;;
	2) MEDIAFWNHD2="--enable-mediafwgstreamernhd2"
	;;
	*) MEDIAFWNHD2=""
	;;
esac


MEDIAFW="--enable-mediafwgstreamer"
CONFIGPARAM="$CONFIGPARAM $MEDIAFW $MEDIAFWNHD2"

##############################################
if [ -z "$MEDIAFW" ]; then
	EXTERNAL_LCD="--enable-externallcd"
else
	EXTERNAL_LCD=""
fi
##############################################
#if [ ! -e "$GMDIR/cvs/cdk/.spark7162" ]; then
	echo ""
	echo -e "\nFilesystemtype:"
if [ ! -e "$GMDIR/cvs/cdk/.p0211" ]; then
	echo -e "\033[1;37;40m   1) JFFS2 (standard)\033[0m"
fi
#	if [ ! -e "$GMDIR/cvs/cdk/.p0211" ]; then
		echo -e "\033[1;37;42m   2) YAFFS2 (not needed changing bootargs)\033[0m"
#	fi
	read -p "Select Filesystem (1-2)? "
	case "$REPLY" in
		1) echo -e "\nSelected Filesystem: $REPLY\n"
		   MULTIYAFFS2="";;
		2) echo -e "\nSelected Filesystem: $REPLY\n"
		   MULTIYAFFS2="--enable-multi-yaffs2";;
		*) echo -e "\nSelected Filesystem: $REPLY\n"
		   MULTIYAFFS2="";;
	esac
#else
#	echo -e "\nFilesystem"
#	echo "JFFS2 (standard)"
#	MULTIYAFFS2=""
#fi
##############################################

CONFIGPARAM="$CONFIGPARAM $PLAYER $MULTICOM $EXTERNAL_LCD $MULTIYAFFS2"

##############################################

# Enable this option if you want to use the latest version of every package.
# The latest version might have solved some bugs, but might also have
# introduced new ones
# CONFIGPARAM="$CONFIGPARAM --enable-bleeding-edge"

##############################################

echo && \
echo "Performing autogen.sh..." && \
echo "------------------------" && \
./autogen.sh && \
echo && \
echo "Performing configure..." && \
echo "-----------------------" && \
echo && \
./configure $CONFIGPARAM

#Dagobert: I find it sometimes useful to know
#what I have build last in this directory ;)
echo $CONFIGPARAM >lastChoice
echo ""
echo "---------------------------------"
echo "Your build enivroment is ready :-)"
echo "Your next step could be:"
#echo "make yaud-enigma2"
echo "-----!!!OUTDATED!!!------------"
echo -e "\033[1;37;41m make yaud-enigma2-nightly\033[0m"
echo -e "\033[1;37;41m make yaud-neutrino\033[0m"
echo -e "\033[1;37;41m make evolux (evo-ntrino+E2)\033[0m"
echo -e "\033[1;37;41m make evolux-full (bootstrap + evo-ntrino + evo-e2)\033[0m"
echo -e "\033[1;37;41m make evolux-pli (evo-e2pli)\033[0m"
echo -e "\033[1;37;41m make evolux-pli-full (bootstrap + evo-ntrino + evo-e2pli)\033[0m"
echo "-----!!!EXPERIMENTAL!!!--------"
echo -e "\033[1;37;43m make yaud-vdr2\033[0m"
echo -e "\033[1;37;43m make evolux-neutrino-hd-vdr2 (evo-vdr2)\033[0m"
echo -e "\033[1;37;43m make evolux-neutrino-hd-vdr2-full (bootstrap + evo-vdr2)\033[0m"
#echo "make yaud-enigma1-hd"
echo "--!!!DEFAULT WITHOUT IMG-BUILD!!!--"
echo -e "\033[1;37;40m make yaud-enigma2-pli-nightly\033[0m"
echo -e "\033[1;37;40m make yaud-neutrino-hd\033[0m"
echo -e "\033[1;37;40m make yaud-neutrino-mp\033[0m"
echo -e "\033[1;37;40m make yaud-neutrino-hd2\033[0m"
echo "--!!!RECOMMENDED-WITH IMG-BUILD!!!--"
echo -e "\033[1;37;40m make evolux-neutrino-hd-pli (evo-nhd + evo-e2pli)\033[0m"
echo -e "\033[1;37;40m make evolux-neutrino-hd-pli-full (bootstrap + evo-nhd + evo-e2pli)\033[0m"
echo -e "\033[1;37;40m make evolux-neutrino-hd2 (evo-nhd2)\033[0m"
echo -e "\033[1;37;40m make evolux-neutrino-hd2-full (bootstrap + evo-nhd2)\033[0m"
echo -e "\033[1;37;40m make evolux-neutrino-hd2-pli (evo-nhd2 + evo-e2pli)\033[0m"
echo -e "\033[1;37;40m make evolux-neutrino-hd2-pli-full (bootstrap + evo-nhd2 + evo-e2pli)\033[0m"
echo "--!!!RECOMMENDED-FULL-ACTUAL!!!--"
echo -e "\033[1;37;42m make evolux-triple (evo-nhd + evo-nhd2 + evo-e2pli)\033[0m"
echo -e "\033[1;37;42m make evolux-triple-full (bootstrap + evo-nhd + evo-nhd2 + evo-e2pli)\033[0m"
echo "---------------------------------"

