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
KATIDIR=${CURDIR%/cvs/cdk}
export PATH=/usr/sbin:/sbin:$PATH

CONFIGPARAM=" \
 --enable-maintainer-mode \
 --prefix=$KATIDIR/tufsbox \
 --with-cvsdir=$KATIDIR/cvs \
 --with-customizationsdir=$KATIDIR/custom \
 --with-archivedir=$HOME/Archive \
 --enable-ccache \
 --enable-flashrules \
 --with-stockdir=$KATIDIR/stock"

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
echo -e "Build SPARK STM24-209 Player191 now...\n"
TARGET="--enable-spark"
CONFIGPARAM="$CONFIGPARAM $TARGET"

##############################################

KERNEL="--enable-stm24 --enable-p0209"
CONFIGPARAM="$CONFIGPARAM $KERNEL"

##############################################
read -p "Activate debug (y/N)? "
if [ "$REPLY" == "y" ] || [ "$REPLY" == "Y" ]; then
	CONFIGPARAM="$CONFIGPARAM --enable-debug"
else
	CONFIGPARAM="$CONFIGPARAM"
fi

##############################################

cd ../driver/
echo "# Automatically generated config: don't edit" > .config
echo "#" >> .config
echo "export CONFIG_ZD1211REV_B=y" >> .config
echo "export CONFIG_ZD1211=n"		>> .config
cd - >/dev/null 2>&1

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
       ln -s stmfb-3.1_stm24_0102 stmfb
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
       ln -s stmfb-3.1_stm24_0102 stmfb
       cd -  >/dev/null 2>&1

##############################################

echo -e "\nMulticom:"
echo "   1) Multicom 3.2.4 rc3 (Recommended for Player191)"
echo "   2) Multicom 3.2.4 (Experimental)"
case $2 in
        [1-2]) REPLY=$2
        echo -e "\nSelected multicom: $REPLY\n"
        ;;
        *)
        read -p "Select multicom (1-3)? ";;
esac

case "$REPLY" in
	1) MULTICOM="--enable-multicom324"
       cd ../driver/include/
       if [ -L multicom ]; then
          rm multicom
       fi

       ln -s ../multicom-3.2.4_rc3/include multicom
       cd - >/dev/null 2>&1

       cd ../driver/
       if [ -L multicom ]; then
          rm multicom
       fi

       ln -s multicom-3.2.4_rc3 multicom
       echo "export CONFIG_MULTICOM324=y" >> .config
       cd - >/dev/null 2>&1
    ;;
	2) MULTICOM="--enable-multicom324"
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
    ;;
	*) MULTICOM="--enable-multicom324";;
esac

##############################################

MEDIAFW=""

##############################################

EXTERNAL_LCD="--enable-externallcd"

##############################################
echo ""
read -p "Activate multi-YAFFS2 (jffs2 not work if active!) (y/N)? "
if [ "$REPLY" == "y" ] || [ "$REPLY" == "Y" ]; then
	CONFIGPARAM="$CONFIGPARAM --enable-multi-yaffs2"
else
	CONFIGPARAM="$CONFIGPARAM"
fi

##############################################

CONFIGPARAM="$CONFIGPARAM $PLAYER $MULTICOM $MEDIAFW $EXTERNAL_LCD"

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

echo "-----------------------"
echo "Your build enivroment is ready :-)"
echo "Your next step could be:"
#echo "make yaud-enigma2"
echo "make yaud-enigma2-nightly"
echo "make yaud-neutrino"
#echo "make yaud-vdr"
#echo "make yaud-enigma1-hd"
echo "make evolux"
echo "make evolux-full"
echo "-----------------------"

