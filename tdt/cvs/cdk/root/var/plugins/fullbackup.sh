#!/bin/sh

B=/autofs/sda1/enigma2-`date '+%Y%m%d%H%M%S'`
R=yaffs2
O=oob.img
U=uImage
mkdir $B
cd $B || exit

touch .running

while [ -f .running ]
do
	echo please wait > /dev/vfd
	sleep 3
done &

set `dd if=/dev/mtd5 bs=4 skip=3 count=1 | hexdump -C | head -1`
Z=0x$2$3$4$5
Z=`printf "%d" $Z`
Z=$((64 + $Z))
Z1=$((1 + $Z / 8192))
dd if=/dev/mtd5 of=$U bs=8192 count=$Z1
# truncate may or may not be available. If it isn't the uImage file
# will be slightly too large, which won't pose a problem.
truncate -s $Z $U 2>/dev/null

# Create YAFFS2 image
unspare2 /dev/mtd6 oob.img
mkdir yaffs2
mount --bind / $R
mkyaffs2 -o $O $R e2yaffs2.img
umount $R
rmdir $R
rm $O

# Create restore script
cat >> restore.sh <<EOT
#!/bin/sh
flash_eraseall /dev/mtd5
nandwrite -a -p -m /dev/mtd5 uImage
flash_eraseall /dev/mtd6
nandwrite -a -o /dev/mtd6 e2yaffs2.img
EOT
chmod 755 restore.sh

rm .running
sleep 4

echo done > /dev/vfd

