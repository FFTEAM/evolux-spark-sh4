#!/bin/sh
### /storage/c/enigma2/flash_E2_yaffs2.sh ###
if [ -e /storage/c/enigma2/fw_env.config ]; then
cp -RP /storage/c/enigma2/fw_env.config /etc/
chmod 755 /etc/fw_env.config
fi

echo -e "\nFlashtool:"
echo " what you'll do?"
echo ""
echo "    1) FLASH YAFFS2"
echo ""
echo "    2) FLASH JFFS2"
echo ""
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
if [ -e /storage/c/enigma2/e2yaffs2.img ] && [ -e /storage/c/enigma2/fw_env.config ]; then
flash_eraseall /dev/mtd5
nandwrite -a -p -m /dev/mtd5 /storage/c/enigma2/uImage
flash_eraseall /dev/mtd6
nandwrite -a -o /dev/mtd6 /storage/c/enigma2/e2yaffs2.img
/storage/c/enigma2/setmtdmode
/storage/c/enigma2/fw_setenv -s /storage/c/enigma2/bootargs_evolux_yaffs2
/storage/c/enigma2/setmtdmode -l
/storage/c/enigma2/fw_printenv
echo "Done! Bootargs also set to Evolux-YAFFS2!"
echo "We reboot in 10s and Evolux should start with neutrino..."
sleep 10
reboot -f
else
echo "ERROR! Not all files found on stick!"
exit
fi
;;
2)
if [ -e /storage/c/enigma2/e2jffs2.img ] && [ -e /storage/c/enigma2/fw_env.config ]; then
flash_eraseall /dev/mtd5
nandwrite -a -p -m /dev/mtd5 /storage/c/enigma2/uImage
flash_eraseall /dev/mtd6
nandwrite -a -m -p /dev/mtd6 /storage/c/enigma2/e2jffs2.img
/storage/c/enigma2/setmtdmode
/storage/c/enigma2/fw_setenv -s /storage/c/enigma2/bootargs_evolux_jffs2
/storage/c/enigma2/setmtdmode -l
/storage/c/enigma2/fw_printenv
echo "Done! Bootargs also set to Evolux-JFFS2!"
echo "We reboot in 10s and Evolux should start with neutrino..."
sleep 10
reboot -f
else
echo "ERROR! Not all files found on stick!"
exit
fi
;;
*)
exit
;;
esac
exit

