#!/bin/sh

[ $# -lt 1 ] && exit

SYSCONFIG=/etc/vdr/sysconfig

if [ -e $SYSCONFIG ]; then
	for i in $*; do
		[ -f /etc/config/$i ] && \
		{
			sed -e "/^${i}.*/d" -i ${SYSCONFIG}
			uci show $i >>${SYSCONFIG}
		}
	done
fi
