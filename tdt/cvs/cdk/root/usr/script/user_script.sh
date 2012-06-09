#!/bin/sh
if [ -e /etc/.corrvol ]; then
myvol=`cat /etc/.lastVol`
(sleep 11; echo $myvol > /proc/stb/avs/0/volume) &
fi
exit

