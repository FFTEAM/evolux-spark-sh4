#!/bin/sh

[ $# -lt 3 ] && exit
#echo $*

CONFIG=$1               # Config
COMMIT=$2               # Commit switch [0=no,1=yes]
INITD=$3                # INITD switch [0=no,1=yes]

SYSCONFIG=/etc/vdr/sysconfig

if [ -e $SYSCONFIG ]; then
  while read ENTRY; do
	# ignore comment lines and remove <CR> characters if any
	#ENTRY=`echo $ENTRY | tr -d '\r' | grep -v "[\#]"`
	if [ -n "$ENTRY" ]; then
		LEFT1=${ENTRY%%.*}
		RIGHT=${ENTRY#*.}
		LEFT2=${RIGHT%%.*}
		if [ "$LEFT1.$LEFT2" = "$CONFIG" ]; then
			ENTRY=`echo $ENTRY | tr -d '"'`
			echo "uci set `echo $ENTRY`"
			uci set `echo $ENTRY`
			MODIFIED="1"
		fi
	fi
  done < $SYSCONFIG
fi

# Commit changes
if [ "$2" == "1" ] && [ -n "$MODIFIED" ]; then
	echo "uci commit $CONFIG"
	uci commit $CONFIG
else
	exit
fi

# Activate changes
[ $? -eq 0 -a "$INITD" = "1" ] && /etc/init.d/${CONFIG%%.*} restart
