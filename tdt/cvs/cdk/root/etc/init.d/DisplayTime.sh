#!/bin/sh
export LD_LIBRARY_PATH=/lib:$LD_LIBRARY_PATH
export TZ=MEZ-1MESZ,M3.5.0/2,M10.5.0/3
while [ 1 ]
do
        set `date "+%H %M %S"`
        echo $1$2 >/dev/vfd
        sleep $((60 - $3))
done
exit
