#!/bin/sh
hturl="http://127.0.0.1"
while [ 1 ]
do
	watchchan=`cat /tmp/.channame`
	watchchan_old=""
	if [ "$watchchan" != "$watchchan_old" ]; then
		sid=$(wget -q -O - $hturl/control/zapto | cut -b13-16)
		#channame=`wget -q -O - $hturl/control/getbouquetsxml | grep $sid | grep 's="192"' | cut -d '"' -f4`
		channame=`wget -q -O - $hturl/control/getubouquetsxml | grep "i=\"$sid\"" | cut -d '"' -f4`
		echo $channame > /tmp/.channame
		watchchan_old="$watchchan"
		sleep 5
	fi
	chanid=$(wget -q -O - $hturl/control/zapto)
	chaninf=$(wget -q -O - $hturl/control/epg?ext | grep -m 1 $chanid)
	percent=$(echo $chaninf | { read chanid start length eventid event; \
	echo $(( (`date +%s`-$start)\*100\/$length )); \
	})
	echo $percent > /tmp/.percent
done
exit
