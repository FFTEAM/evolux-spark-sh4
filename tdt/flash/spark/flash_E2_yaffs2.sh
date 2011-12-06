#!/bin/sh
### /storage/c/enigma2/flash_E2_yaffs2.sh ###
#if [ -e /storage/c/enigma2/fw_env.config ]; then
#cp -RP /storage/c/enigma2/fw_env.config /etc/
#fi
chmod 755 /etc/fw_env.config
echo -e "\nFlashtool:"
echo " what you'll do?"
echo "    1) FLASH YAFFS2"
echo "    2) FLASH JFFS2 (default)"
echo "    *) exit"

case $1 in
[1-9] | 1[0-9]) REPLY=$1
echo -e "\nSelected option: $REPLY\n"
;;
*)
read -p "Select option (1-2)? ";;
esac

case "$REPLY" in
1)
if [ -e /storage/c/enigma2/e2yaffs2.img ]; then
flash_eraseall /dev/mtd5
nandwrite -a -p -m /dev/mtd5 /storage/c/enigma2/uImage
flash_eraseall /dev/mtd6
nandwrite -a -o /dev/mtd6 /storage/c/enigma2/e2yaffs2.img
#/storage/c/enigma2/fw_setenv bootargs_enigma2 "console=ttyAS0,115200 root=/dev/mtdblock6 rootfstype=yaffs2 rw init=/bin/devinit coprocessor_mem=4m@0x40000000,4m@0x40400000 printk=1 nwhwconf=device:eth0,hwaddr:00:80:E1:12:40:69 rw ip=172.100.100.249:172.100.100.174:172.100.100.174:255.255.0.0:LINUX7109:eth0:off bigphysarea=6000 stmmaceth=msglvl:0,phyaddr:2,watchdog:5000"
#/storage/c/enigma2/fw_printenv
echo "done! please turn off ac, hold ok-button and turn ac on again."
echo "press arrow-down, than ok-button, and e2/neutrino bootup."
else
echo "ERROR! e2yaffs2.img not found on stick!"
exit
fi
;;
2)
if [ -e /storage/c/enigma2/e2jffs2.img ]; then
flash_eraseall /dev/mtd5
nandwrite -a -p -m /dev/mtd5 /storage/c/enigma2/uImage
flash_eraseall /dev/mtd6
nandwrite -a -m -p /dev/mtd6 /storage/c/enigma2/e2yaffs2.img
#/storage/c/enigma2/fw_setenv bootargs_enigma2 "console=ttyAS0,115200 root=/dev/mtdblock6 rootfstype=jffs2 rw init=/bin/devinit coprocessor_mem=4m@0x40000000,4m@0x40400000 printk=1 nwhwconf=device:eth0,hwaddr:00:80:E1:12:40:69 rw ip=172.100.100.249:172.100.100.174:172.100.100.174:255.255.0.0:LINUX7109:eth0:off bigphysarea=6000 stmmaceth=msglvl:0,phyaddr:2,watchdog:5000"
#/storage/c/enigma2/fw_printenv
echo "done! please turn off ac, hold ok-button and turn ac on again."
echo "press arrow-down, than ok-button, and e2/neutrino bootup."
else
echo "ERROR! e2jffs2.img not found on stick!"
exit
fi
;;
*)
exit
;;
esac
exit

