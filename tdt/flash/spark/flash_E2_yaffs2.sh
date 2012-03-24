#!/bin/sh
### /storage/c/enigma2/flash_E2_yaffs2.sh ####
#if [ -e /storage/c/enigma2/etc/fw_env.config ]; then
#cp -RP /storage/c/enigma2/etc/fw_env.config /etc/
#chmod 755 /etc/fw_env.config
#fi

echo -e "\nFlashtool:"
#echo " what you'll do?"
echo ""
echo "    FLASH EVOLUX NOW..."
#echo ""
#echo "    2) FLASH JFFS2"
echo ""
#echo "    *) exit"

#case $1 in
#[1-9] | 1[0-9]) REPLY=$1
#echo -e "\nSelected option: $REPLY\n"
#;;
#*)
#read -p "Select option (1-2)? ";;
#esac

#case "$REPLY" in
#1)
if [ -e /storage/c/enigma2/e2yaffs2.img ] && [ -e /storage/c/enigma2/etc/fw_env.config ]; then
read -p "are you sure? start flashing EVOLUX now (Y/n) ? "
if [ "$REPLY" == "Y" ] || [ "$REPLY" == "y" ]; then
cp -RP /storage/c/enigma2/etc/fw_env.config /etc/
chmod 755 /etc/fw_env.config
flash_eraseall /dev/mtd5
nandwrite -a -p -m /dev/mtd5 /storage/c/enigma2/uImage
flash_eraseall /dev/mtd6
nandwrite -a -o /dev/mtd6 /storage/c/enigma2/e2yaffs2.img
else
exit
fi
#read -p "are you sure? change bootargs now to YAFFS2 (Y/n) ? "
#if [ "$REPLY" == "Y" ] || [ "$REPLY" == "y" ]; then
echo "copy your orig Bootargs to /storage/c/enigma2/myBootargs.txt"
/storage/c/enigma2/bin/fw_printenv > /storage/c/enigma2/myBootargs.txt
#echo "change bootargs now..."
oldEVOargs=`/storage/c/enigma2/bin/fw_printenv | grep yaffs2`
EVObootset=`/storage/c/enigma2/bin/fw_printenv | grep boot_system=enigma2`
if [ ! -z $oldEVOargs ] || [ -z $EVObootset ]; then
/storage/c/enigma2/bin/setmtdmode
/storage/c/enigma2/bin/fw_setenv -s /storage/c/enigma2/etc/bootargs_evolux_yaffs2
/storage/c/enigma2/bin/setmtdmode -l
fi
#/storage/c/enigma2/bin/fw_printenv
echo "All Done!"
echo "We reboot in 10s and Evolux should booting with neutrino..."
sleep 10
reboot -f
#else
#exit
#fi
else
echo "ERROR! Not all files found on stick!"
exit
fi
#;;
# 2)
#if [ -e /storage/c/enigma2/e2jffs2.img ] && [ -e /storage/c/enigma2/etc/fw_env.config ]; then
#read -p "are you sure? start flashing JFFS2 now (Y/n) ? "
#if [ "$REPLY" == "Y" ] || [ "$REPLY" == "y" ]; then
#flash_eraseall /dev/mtd5
#nandwrite -a -p -m /dev/mtd5 /storage/c/enigma2/uImage
#flash_eraseall /dev/mtd6
#nandwrite -a -m -p /dev/mtd6 /storage/c/enigma2/e2jffs2.img
#else
#exit
#fi
#read -p "are you sure? change bootargs now to JFFS2 (Y/n) ? "
#if [ "$REPLY" == "Y" ] || [ "$REPLY" == "y" ]; then
#echo "copy your orig Bootargs to /storage/c/enigma2/myBootargs.txt"
#/storage/c/enigma2/bin/fw_printenv > /storage/c/enigma2/myBootargs.txt
#echo "change bootargs now..."
#/storage/c/enigma2/bin/setmtdmode
#/storage/c/enigma2/bin/fw_setenv -s /storage/c/enigma2/etc/bootargs_evolux_jffs2
#/storage/c/enigma2/bin/setmtdmode -l
#/storage/c/enigma2/bin/fw_printenv
#echo "All Done! Bootargs also set to JFFS2!"
#echo "We reboot in 10s and your e2jjfs2.img should booting..."
#sleep 10
#reboot -f
#else
#exit
#fi
#else
#echo "ERROR! Not all files found on stick!"
#exit
#fi
#;;
#*)
#exit
#;;
#esac
exit

