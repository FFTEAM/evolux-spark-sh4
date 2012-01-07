#!/bin/sh
## Variablen definieren
SCRIPTPATH=/devel/E2/shrink
newarchiv=`cat $SCRIPTPATH/rev.version`
oldarchiv=$(($newarchiv-1))
updatepath=/home/juppi/ENIGMA2
update=sysupdate$newarchiv.tar.gz
tdt=$updatepath/release_incl_kernel_rev_$newarchiv.tar.gz
dev=/devel/E2/dev.tar.gz
ARCHIVEPATH=/devel/E2/release$newarchiv
NOT_AWK3="$ARCHIVEPATH/usr/lib/python2.6/collections.py"
E2DIR=/home/juppi/ufs910/mt_build/stock/.roote2
VARDIR=/home/juppi/ufs910/mt_build/stock/.dat
BACKUP=/home/juppi/ufs910/mt_build/stock/BACKUP
makeimagepath=/home/juppi/ufs910/mt_build
pythonpath=$E2DIR/usr/lib/python2.6

echo "MAKE PUBLIC"
#sed s"@^\\(\\s*\\)#mount -t jffs2 /dev/mtdblock3 /var\$@\\mount -t jffs2 /dev/mtdblock3 /var@" -i $E2DIR/etc/init.d/mountvirtfs
cp $updatepath/lircd $VARDIR/usr/bin/
rm -rf $VARDIR/emu
rm -rf $VARDIR/keys
rm $VARDIR/etc/.backup
rm $VARDIR/usr/lib/enigma2/python/Plugins/Extensions/Softcam
rm -rf $E2DIR/usr/lib/enigma2/python/Extensions/Softcam
rm -rf $E2DIR/emu
rm -rf $E2DIR/keys
if [ -e $VARDIR/mnt/E2 ]; then
   rm $VARDIR/mnt/E2
   rm $VARDIR/mnt/FATTEST
   rm $VARDIR/mnt/TEST
fi
echo "src/gz cross http://example-server.de/cross/" > $VARDIR/etc/ipkg.conf
cp /home/juppi/ufs910/mt_build/stock/settings $VARDIR/usr/local/share/enigma2/
echo "# /etc/fstab: static file system information." >$VARDIR/etc/fstab
echo "#" >>$VARDIR/etc/fstab
echo "# <file system> <mount point><type>  <options><dump>  <pass>" >>$VARDIR/etc/fstab
echo "#/dev/root /auto defaults,errors=remount-ro,noatime,nodiratime0 0" >>$VARDIR/etc/fstab
echo "proc /proc proc defaults 0 0" >>$VARDIR/etc/fstab
echo "tmpfs /tmp tmpfs defaults 0 0" >>$VARDIR/etc/fstab
touch $VARDIR/etc/.evofirstboot
rm $VARDIR/etc/init.d/birdie.sh
if [ -e $VARDIR/usr/script/ufs910/dd.sh ]; then
rm $VARDIR/usr/script/ufs910/dd.sh
fi
if [ -e $VARDIR/usr/script/ufs922/dd.sh ]; then
rm $VARDIR/usr/script/ufs922/dd.sh
fi
#bootlogo
#mkdir $VARDIR/boot
#mv $E2DIR/boot/bootlogo.mvi $VARDIR/boot/
#cp /home/juppi/Desktop/TMPGEnc-2.525.64.184-EN-Free/bootlogo_kids.mvi $VARDIR/boot/
#cp /home/juppi/Desktop/TMPGEnc-2.525.64.184-EN-Free/radio_kids.mvi $VARDIR/usr/local/share/enigma2/
#cp /home/juppi/Desktop/TMPGEnc-2.525.64.184-EN-Free/saver1_kids.mvi $VARDIR/saver/
#cp /home/juppi/Desktop/TMPGEnc-2.525.64.184-EN-Free/saver2_kids.mvi $VARDIR/saver/
#mv $VARDIR/saver/saver1_kids.mvi $VARDIR/saver/saver1_kids.mvi.off 
#mv $VARDIR/saver/saver2_kids.mvi $VARDIR/saver/saver2_kids.mvi.off 
#cd $E2DIR/boot
#ln -s /var/boot/bootlogo.mvi
#cd $E2DIR

cp /home/juppi/ufs910/mt_build/stock/allerlei $VARDIR/etc/init.d/
#cp /home/juppi/ufs910/mt_build/stock/share/skin.xml $E2DIR/usr/local/share/
cp /home/juppi/ufs910/mt_build/stock/share/EV-HD_Shadow3/skin.xml $E2DIR/usr/local/share/EV-HD_Shadow3/
#if [ -e $VARDIR/epg/epg.da.lzma ]; then
#rm $VARDIR/epg/epg.da.lzma
#fi
#rm $E2DIR/usr/local/share/skin.xml
mv $VARDIR/usr/local/share/enigma2/skin.xml.higher.txt $E2DIR/usr/local/share/skin.xml
if [ ! -e /devel/E2/release$newarchiv.public ]; then
   mkdir /devel/E2/release$newarchiv.public
fi
sudo rm -rf /devel/E2/release$newarchiv.public/*
cd $E2DIR
sudo tar -czvf /devel/E2/release$newarchiv.public/release$newarchiv.root.public.tar.gz ./
cd /devel/E2/release$newarchiv.public
sudo tar -xzvf release$newarchiv.root.public.tar.gz
chmod 777 -R /devel/E2/release$newarchiv.public
cd $VARDIR
sudo tar -czvf /devel/E2/release$newarchiv.public/var/release$newarchiv.var.public.tar.gz ./
cd /devel/E2/release$newarchiv.public/var
sudo tar -xzvf release$newarchiv.var.public.tar.gz
cp /devel/E2/release$newarchiv/boot/uImage /devel/E2/release$newarchiv.public/boot/
sed s"@^\\(\\s*\\)mount -t jffs2 /dev/mtdblock3 /var\$@\\#mount -t jffs2 /dev/mtdblock3 /var@" -i /devel/E2/release$newarchiv.public/etc/init.d/mountvirtfs
if [ -e $VARDIR/etc/panel.blacklisted ];then
   rm $VARDIR/etc/panel.blacklisted
fi
cd $E2DIR
cutecom
cd $makeimagepath/cvs/cdk
make flash-stock-enigma2-full
mv $makeimagepath/tufsbox/cdkflash/mtdblock3.root-stock.enigma2 $makeimagepath/tufsbox/cdkflash/mtdblock2.root-stock.enigma2
mv $makeimagepath/tufsbox/cdkflash/mtdblock6.var-stock.enigma2 $makeimagepath/tufsbox/cdkflash/mtdblock3.var-stock.enigma2
sudo chmod 777 -R $makeimagepath/tufsbox/cdkflash
if [ ! -e $makeimagepath/tufsbox/cdkflash/public ]; then
   mkdir $makeimagepath/tufsbox/cdkflash/public
fi

if [ -e $makeimagepath/tufsbox/cdkflash/public/mtd*.ubimage ] || [ -e $makeimagepath/tufsbox/cdkflash/public/mtd*.enigma2 ] || [ -e $makeimagepath/tufsbox/cdkflash/public/var-stock-enigma2 ] || [ -e $makeimagepath/tufsbox/cdkflash/public/*.update ] || [ -e $makeimagepath/tufsbox/cdkflash/public/stock*.img ]; then
   rm -rf $makeimagepath/tufsbox/cdkflash/public/mtd*.ubimage
   rm -rf $makeimagepath/tufsbox/cdkflash/public/mtd*.enigma2
   rm -rf $makeimagepath/tufsbox/cdkflash/public/var-stock-enigma2
   rm -rf $makeimagepath/tufsbox/cdkflash/public/*.update
   rm -rf $makeimagepath/tufsbox/cdkflash/public/stock*.img
fi

mv $makeimagepath/tufsbox/cdkflash/stock*.img $makeimagepath/tufsbox/cdkflash/public/

cd $makeimagepath/tufsbox/cdkflash/public

mv $makeimagepath/tufsbox/cdkflash/mtdblock2.root-stock.enigma2 $makeimagepath/tufsbox/cdkflash/public/
mv $makeimagepath/tufsbox/cdkflash/mtdblock3.var-stock.enigma2 $makeimagepath/tufsbox/cdkflash/public/
cp  $makeimagepath/tufsbox/cdkflash/mtdblock1*.ubimage $makeimagepath/tufsbox/cdkflash/public/
rm $makeimagepath/tufsbox/cdkflash/public/*.rar
rm $makeimagepath/tufsbox/cdkflash/public/EV-KCC-3p1-E2-ufs910-$newarchiv-FLASH.img
mv $makeimagepath/tufsbox/cdkflash/public/stock-enigma2.img $makeimagepath/tufsbox/cdkflash/public/EV-KCC-3p1-E2-ufs910-$newarchiv-FLASH.img
#ghex2 $makeimagepath/tufsbox/cdkflash/public/EV-KIM-3p1-E2-$newarchiv-FLASH.kim
#echo -e "[Image]" >> $makeimagepath/tufsbox/cdkflash/public/EV-KIM-3p1-E2-$newarchiv-FLASH.kim
 #  echo -e "desc=EV-KCC-3p1-E2-ufs910-$newarchiv-FLASH.img" >> $makeimagepath/tufsbox/cdkflash/public/EV-KIM-3p1-E2-$newarchiv-FLASH.kim
#echo -e "binfile=EV-KCC-3p1-E2-ufs910-$newarchiv-FLASH.img" >> $makeimagepath/tufsbox/cdkflash/public/EV-KIM-3p1-E2-$newarchiv-FLASH.kim
#echo -e 'bootargs="console=ttyAS0,115200 root=/dev/mtdblock2 ip={IP}:{SERVERIP}:{GATEWAY}:255.255.255.0:kathrein:eth0:off mem=64m coprocessor_mem=4m@0x10000000,4m@0x10400000 rootdelay=1 nwhwconf=device:eth0,hwaddr:{MAC} init=/bin/devinit"' >> $makeimagepath/tufsbox/cdkflash/public/EV-KIM-3p1-E2-$newarchiv-FLASH.kim
#echo -e 'bootcmd="bootm a0040000"' >> $makeimagepath/tufsbox/cdkflash/public/EV-KIM-3p1-E2-$newarchiv-FLASH.kim#echo -e " " >> $makeimagepath/tufsbox/cdkflash/public/EV-KIM-3p1-E2-$newarchiv-FLASH.kim
cp $makeimagepath/tufsbox/cdkflash/EV-KIM-3p1-E2-$newarchiv-FLASH.kim $makeimagepath/tufsbox/cdkflash/public/EV-KIM-3p1-E2-$newarchiv-FLASH.kim
chmod 755 $makeimagepath/tufsbox/cdkflash/public/EV-KIM-3p1-E2-$newarchiv-FLASH.kim
cp $updatepath/tdt_readme.txt $makeimagepath/tufsbox/cdkflash/public/

rar a $makeimagepath/tufsbox/cdkflash/public/EV-KCC-3p1-E2-ufs910-$newarchiv-FLASH.rar ./EV-KCC-3p1-E2-ufs910-$newarchiv-FLASH.img ./EV-KIM-3p1-E2-$newarchiv-FLASH.kim ./tdt_readme.txt

rar a $makeimagepath/tufsbox/cdkflash/public/EV-CAT-DD-3p1-E2-ufs910-$newarchiv-FLASH.rar ./mtdblock1.kernel-enigma2.ubimage ./mtdblock2.root-stock.enigma2 ./mtdblock3.var-stock.enigma2 ./tdt_readme.txt

mv $makeimagepath/tufsbox/cdkflash/public/EV-KCC-3p1-E2-ufs910-$newarchiv-FLASH.img $makeimagepath/tufsbox/cdkflash/public/EV-FLASHSUITE-3p1-E2-ufs910-$newarchiv-FLASH.img
cp $updatepath/UFS910FlashSuite_V2.3.rar $makeimagepath/tufsbox/cdkflash/public/

rar a $makeimagepath/tufsbox/cdkflash/public/EV-FLASHSUITE-3p1-E2-ufs910-$newarchiv-FLASH.rar ./EV-FLASHSUITE-3p1-E2-ufs910-$newarchiv-FLASH.img ./tdt_readme.txt ./UFS910FlashSuite_V2.3.rar

rm $makeimagepath/tufsbox/cdkflash/*.update
#cd /home/juppi/ufs910/mt_build/tufsbox/cdkflash/public
#wput --reupload ftp://pinky@kathi-forum.de:Heimwerker37@kathif.vs120005.hl-users.com:21/EV/ ./EV-CAT-DD-3p1-E2-ufs910-$newarchiv-FLASH.rar
#wput --reupload ftp://pinky@kathi-forum.de:Heimwerker37@kathif.vs120005.hl-users.com:21/EV/ ./EV-KCC-3p1-E2-ufs910-$newarchiv-FLASH.rar
#wput --reupload ftp://pinky@kathi-forum.de:Heimwerker37@kathif.vs120005.hl-users.com:21/EV/ ./EV-FLASHSUITE-3p1-E2-ufs910-$newarchiv-FLASH.rar
#sudo rm -rf $ARCHIVEPATH.public/*
#sudo rm -rf /devel/E2/release$newarchiv.flash
exit

