#!/bin/sh
export LD_LIBRARY_PATH=/lib:$LD_LIBRARY_PATH

tel=1
export TZ=MEZ-1MESZ,M3.5.0/2,M10.5.0/3
while [ 1 -eq 1 ]; do
    Stime=`date "+%H%M`

    # Update display when new minute
    if [ "$Stime" != "$Oldtime" ]; then
#        /bin/fp_control -t "     $Stime"
	echo "$Stime" > /dev/vfd
        Oldtime=$Stime
    fi

    # Update display each 30 sec.
    if [ $tel -gt 30 ]; then
#        /bin/fp_control -t "     $Stime"
	echo "$Stime" > /dev/vfd
        tel=1
    fi

    tel=`expr $tel + 1`
    sleep 1
done
exit
