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
 --enable-ccache \
 --enable-flashrules \
 --with-stockdir=$KATIDIR/stock"

##############################################

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

echo "Targets:"
echo "18) SPARK"

case $1 in
	[1-9] | 1[0-9] | 2[0-9]) REPLY=$1
	echo -e "\nSelected target: $REPLY\n"
	;;
	*)
	read -p "Select target (18)? ";;
esac

case "$REPLY" in
	18) TARGET="--enable-spark";;
	 *) TARGET="--enable-spark";;
esac
CONFIGPARAM="$CONFIGPARAM $TARGET"

case "$REPLY" in
        8) REPLY=$3
			echo -e "\nModels:"
			echo " 1) VIP1 v1 [ single tuner + 2 CI + 2 USB ]"
			echo " 2) VIP1 v2 [ single tuner + 2 CI + 1 USB + plug & play tuner (dvb-s2/t/c) ]"
			echo " 3) VIP2 v1 [ twin tuner ]"

        	read -p "Select Model (1-3)? "

			case "$REPLY" in
				1) MODEL="--enable-hl101";;
				2) MODEL="--enable-vip1_v2";;
				3) MODEL="--enable-vip2_v1";;
				*) MODEL="--enable-vip2_v1";;
			esac
			CONFIGPARAM="$CONFIGPARAM $MODEL"
        	;;
        *)
esac

##############################################

echo -e "\nKernel:"
echo " Maintained:"
echo "    4) STM 23 P0123"
echo "    6) STM 24 P0205"
echo "    8) STM 24 P0207 (Recommended)"
echo "   10) STM 24 P0209 (only spark)"

case $2 in
        [1-9] | 1[0-9]) REPLY=$2
        echo -e "\nSelected kernel: $REPLY\n"
        ;;
        *)
        read -p "Select kernel (1-10)? ";;
esac

case "$REPLY" in
	4) KERNEL="--enable-stm23 --enable-p0123";;
	6) KERNEL="--enable-stm24 --enable-p0205";STMFB="stm24";;
	8) KERNEL="--enable-stm24 --enable-p0207";STMFB="stm24";;
	10) KERNEL="--enable-stm24 --enable-p0209";STMFB="stm24";;
	*) KERNEL="--enable-stm23 --enable-p0123";;
esac
CONFIGPARAM="$CONFIGPARAM $KERNEL"

##############################################
if [ "$3" ]; then
 REPLY="$3"
 echo "Activate debug (y/N)? "
 echo -e "\nSelected option: $REPLY\n"
else
 REPLY=N
 read -p "Activate debug (y/N)? "
fi
[ "$REPLY" == "y" -o "$REPLY" == "Y" ] && CONFIGPARAM="$CONFIGPARAM --enable-debug"

##############################################

cd ../driver/
echo "# Automatically generated config: don't edit" > .config
echo "#" >> .config
echo "export CONFIG_ZD1211REV_B=y" >> .config
echo "export CONFIG_ZD1211=n"		>> .config
cd -

##############################################

echo -e "\nPlayer:"
echo "   2) Player 179"
echo "   3) Player 191 (Recommended)"
case $4 in
        [1-3]) REPLY=$4
        echo -e "\nSelected player: $REPLY\n"
        ;;
        *)
        read -p "Select player (1-3)? ";;
esac

case "$REPLY" in
	2) PLAYER="--enable-player179"
       cd ../driver/include/
       if [ -L player2 ]; then
          rm player2
       fi

       if [ -L stmfb ]; then
          rm stmfb
       fi
       ln -s player2_179 player2
       if [ "$STMFB" == "stm24" ]; then
           ln -s stmfb-3.1_stm24_0102 stmfb
       else
           ln -s stmfb-3.1_stm23_0032 stmfb
       fi
       cd -

       cd ../driver/
       if [ -L player2 ]; then
          rm player2
       fi
       ln -s player2_179 player2
       echo "export CONFIG_PLAYER_179=y" >> .config
       cd -

       cd ../driver/stgfb
       if [ -L stmfb ]; then
          rm stmfb
       fi
       if [ "$STMFB" == "stm24" ]; then
           ln -s stmfb-3.1_stm24_0102 stmfb
       else
           ln -s stmfb-3.1_stm23_0032 stmfb
       fi
       cd -
    ;;
	3) PLAYER="--enable-player191"
       cd ../driver/include/
       if [ -L player2 ]; then
          rm player2
       fi

       if [ -L stmfb ]; then
          rm stmfb
       fi
       ln -s player2_179 player2
       if [ "$STMFB" == "stm24" ]; then
           ln -s stmfb-3.1_stm24_0102 stmfb
       else
           ln -s stmfb-3.1_stm23_0032 stmfb
       fi
       cd -

       cd ../driver/
       if [ -L player2 ]; then
          rm player2
       fi
       ln -s player2_191 player2
       echo "export CONFIG_PLAYER_191=y" >> .config
       cd -

       cd ../driver/stgfb
       if [ -L stmfb ]; then
          rm stmfb
       fi
       if [ "$STMFB" == "stm24" ]; then
           ln -s stmfb-3.1_stm24_0102 stmfb
       else
           ln -s stmfb-3.1_stm23_0032 stmfb
       fi
       cd -
    ;;
	*) PLAYER="--enable-player131";;
esac

##############################################

echo -e "\nMulticom:"
echo "   1) Multicom 3.2.2     (Recommended for Player179)"
echo "   2) Multicom 3.2.4 rc3 (Recommended for Player191)"
case $5 in
        [1-2]) REPLY=$5
        echo -e "\nSelected multicom: $REPLY\n"
        ;;
        *)
        read -p "Select multicom (1-2)? ";;
esac

case "$REPLY" in
	1) MULTICOM="--enable-multicom322"
       cd ../driver/include/
       if [ -L multicom ]; then
          rm multicom
       fi

       ln -s multicom-3.2.2 multicom
       cd -

       cd ../driver/
       if [ -L multicom ]; then
          rm multicom
       fi

       ln -s multicom-3.2.2 multicom
       echo "export CONFIG_MULTICOM322=y" >> .config
       cd -
    ;;
	2) MULTICOM="--enable-multicom324"
       cd ../driver/include/
       if [ -L multicom ]; then
          rm multicom
       fi

       ln -s ../multicom-3.2.4_rc3/include multicom
       cd -

       cd ../driver/
       if [ -L multicom ]; then
          rm multicom
       fi

       ln -s multicom-3.2.4_rc3 multicom
       echo "export CONFIG_MULTICOM324=y" >> .config
       cd -
    ;;
	*) MULTICOM="--enable-multicom322";;
esac

##############################################

echo -e "\nMedia Framework:"
echo "   1) eplayer3  (Recommended)"
echo "   2) gstreamer (Only working with enigma2 diff0 at the moment)"
case $5 in
        [1-2]) REPLY=$6
        echo -e "\nSelected media framwork: $REPLY\n"
        ;;
        *)
        read -p "Select media framwork (1-2)? ";;
esac

case "$REPLY" in
	1) MEDIAFW="";;
	2) MEDIAFW="--enable-mediafwgstreamer";;
	*) MEDIAFW="";;
esac

##############################################

CONFIGPARAM="$CONFIGPARAM $PLAYER $MULTICOM $MEDIAFW"

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
echo "make yaud-enigma2"
echo "make yaud-enigma2-nightly"
echo "make yaud-neutrino"
echo "make yaud-vdr"
echo "make yaud-enigma1-hd"
echo "make evolux"
echo "make evolux-full"
echo "-----------------------"

