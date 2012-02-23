#!/bin/sh
# IP of your NFS server
serverip=server-ip
# exported directory on your NFS server
exportdir=/your/exportdir
# mount point on spark
sparkmountpoint="/mnt/nfs"
# filesize to transfer in MBytes.
# At least 8 MByte. Good values are 32 or 64 MByte.
# Try 128 to be more accurate (takes much longer!)
filesize=32
# block sizes to test in KBytes, possible values are 1 2 4 8 16 32.
# values have to be separated with spaces. See examples below.
# blocksizelist="4 8 32"
# blocksizelist="16"
blocksizelist="4 8 16 32"
# wether to enable synchronous reading, writing. Possible values are "yes"
# or no. Normally synchronous reading or writing should be slower than
# asynchronous, so to save some time most people would say "no" here.
enablesynctests="no"


##################################################################
######## don't edit below this line ########
##################################################################

bs=8192
count=`expr $filesize \* 1048576 / $bs`
wlist=""
rlist=""
synctests="async"
if [ $enablesynctests = "yes" ]; then
synctests="sync "$synctests
fi

echo
echo "Measuring NFS throughput..."
for factor in $blocksizelist
do
for protocol in udp tcp
do
for synchronized in $synctests
do
size=`expr $factor \* 1024`
bitcount=`expr $bs \* $count \* 8`
umount $sparkmountpoint 2>/dev/null
mount -t nfs -o rw,soft,$protocol,nolock,$synchronized,rsize=$size,wsize=$size,rsize=$size $serverip:$exportdir $sparkmountpoint
echo "Mount options: "$protocol", "$synchronized", wsize="$size
echo "writing "$filesize" MBytes..."
a=`date +%s`
dd if=/dev/zero of=$sparkmountpoint/test bs=$bs count=$count 2>/dev/null;
if [ $? = "0" ]
then
z=`date +%s`
duration=`expr $z - $a`
throughput=`expr $bitcount / $duration`
echo "Success after "$duration" seconds"
else
throughput="Failure"
echo "Failure"
fi
wlist=$wlist$throughput" with "$protocol","$synchronized",wsize="$size"\n"

echo "Mount options: "$protocol", "$synchronized", rsize="$size
echo "reading "$filesize" MBytes..."
a=`date +%s`
dd of=/dev/null if=$sparkmountpoint/test bs=$bs count=$count 2>/dev/null;
if [ $? = "0" ]
then
z=`date +%s`
duration=`expr $z - $a`
throughput=`expr $bitcount / $duration`
echo "Success after "$duration" seconds"
else
throughput="Failure"
echo "Failure"
fi
rlist=$rlist$throughput" with "$protocol","$synchronized",rsize="$size"\n"
echo
done
done
done

echo "Results for write throughput:"
echo -e $wlist | sort -nr | sed 's/^\([0-9]*\)\([0-9]\{3\}\)\([0-9]\{3\}\(.*\)\)/\1.\2 Mbit\/s\4/g'

echo "Results for read throughput:"
echo -e $rlist | sort -nr | sed 's/^\([0-9]*\)\([0-9]\{3\}\)\([0-9]\{3\}\(.*\)\)/\1.\2 Mbit\/s\4/g'
