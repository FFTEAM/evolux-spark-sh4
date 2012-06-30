#!/bin/sh

. /etc/vdr/sysconfig 2>/dev/null

for i in $*; do
	#echo $i
	eval module=\${module_${i}_enabled}
	#echo $module
	if [ "$module" = "1" ]; then
		lsmod | grep -q "$i" || insmod $i
	fi
	if [ "$module" = "0" ]; then
		lsmod | grep -q "$i" && rmmod $i
	fi
done
