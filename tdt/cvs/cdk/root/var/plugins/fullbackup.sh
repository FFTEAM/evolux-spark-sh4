#!/bin/sh

PATH=/usr/bin:/bin:/usr/local/bin:/var/bin:/usr/sbin:/sbin
export PATH

B=/autofs/sda1/enigma2-`date '+%Y%m%d%H%M%S'`
R=yaffs2
O=oob.img
U=uImage
OUT=/tmp/.mkyaffs2-out
mkdir $B
cd $B || exit

touch .running

T="Please wait"
T2=""
while [ -f .running ]
do
	TXT=`echo $T$T2 | sed -e 's/%/%25/g' -e 's/ /%20/g'`
	wget -qO /dev/null "http://127.0.0.1/control/message?popup=$TXT"
	sleep 1
	T2=""
	if [ -f $OUT ]
	then
		PERC=`tail -c3 $OUT | sed 's/%//' | egrep '^([0-9]| )[0-9]$'`
		if [ "$PERC" != "" ]
		then
			T2=": $PERC/100"
		fi
	fi
	T=`cat .running` 2>/dev/null
done &

echo "Creating uImage" > .running
set `dd if=/dev/mtd5 bs=4 skip=3 count=1 | hexdump -C | head -1`
Z=0x$2$3$4$5
Z=`printf "%d" $Z`
Z=$((64 + $Z))
Z1=$((1 + $Z / 8192))
dd if=/dev/mtd5 of=$U bs=8192 count=$Z1
# truncate may or may not be available. If it isn't the uImage file
# will be slightly too large, which won't pose a problem.
truncate -s $Z $U 2>/dev/null

echo "Creating YAFFS2 image" > .running
# Create YAFFS2 image
unspare2 /dev/mtd6 oob.img
mkdir yaffs2
mount --bind / $R
mkyaffs2 -o $O $R e2yaffs2.img > $OUT
rm $OUT
umount $R
rmdir $R
rm $O

echo "Creating restore script" > .running
# Create restore script
cat >> restore.sh <<EOT
#!/bin/sh
flash_eraseall /dev/mtd5
nandwrite -a -p -m /dev/mtd5 uImage
flash_eraseall /dev/mtd6
nandwrite -a -o /dev/mtd6 e2yaffs2.img
EOT
chmod 755 restore.sh
echo "Done. Image is in $B" > .running
echo "Done. Image is in $B"
sleep 3
rm .running

