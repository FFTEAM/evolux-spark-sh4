#!/bin/sh
hturl="http://127.0.0.1"
sid=$(wget -q -O - $hturl/control/zapto | cut -b13-16)
channame=`wget -q -O - $hturl/control/getubouquetsxml | grep "i=\"$sid\"" | cut -d '"' -f4`
echo "$channame" > /tmp/.channame

isRunning=`pidof lcd4linux`
while [ -z $isRunning ]
do
	sleep 30
	isRunning=`pidof lcd4linux`
	if [ -z $isRunning ]; then
		lcd4linux &
	fi
done

until false
do
	watchchan=$(wget -q -O - $hturl/control/zapto)
	watchchan_old=00000000
	if [ "$watchchan" != "$watchchan_old" ]; then
		sid=$(wget -q -O - $hturl/control/zapto | cut -b13-16)
		#channame=`wget -q -O - $hturl/control/getbouquetsxml | grep $sid | grep 's="192"' | cut -d '"' -f4`
		channame=`wget -q -O - $hturl/control/getubouquetsxml | grep "i=\"$sid\"" | cut -d '"' -f4`
		echo "$channame" > /tmp/.channame
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
