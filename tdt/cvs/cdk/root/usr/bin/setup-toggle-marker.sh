#!/bin/sh

. /etc/vdr/sysconfig 2>/dev/null

eval marker=\${marker${1}_enabled}
#echo $1:$*:$marker

if [ "${marker}" = "1" ]; then
	for i in $*; do
		#echo "rm -f /etc/config/.no$i"
		rm -f /etc/config/.no$i
	done
else
	for i in $*; do
		#echo "touch /etc/config/.no$i"
		touch /etc/config/.no$i
	done
fi
