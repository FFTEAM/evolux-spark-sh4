#!/bin/sh
path=/home/juppi/Desktop/enigma2-skin-yads-hd_1.0.0_all.ipk_FILES/usr/share/enigma2/EVO-YADS-HD
list=`find $path -name "*.png" -print`
for i in $list
do
if [ ! -L $i ]
then
advpng -z -4 $i
fi
done
exit
