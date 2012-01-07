#!/bin/sh
## Variablen definieren
ipaddr=192.168.178.20
serverip=192.168.178.27
gateway=192.168.178.1
ethaddr=00:50:fd:ff:80:2d
netmask=255.255.255.0

SCRIPTPATH=/devel/E2/shrink
newarchiv=`cat $SCRIPTPATH/rev.version`
oldarchiv=$(($newarchiv-1))
updatepath=$HOME/ENIGMA2
update=sysupdate$newarchiv.tar.gz
tdt=$updatepath/release_incl_kernel_rev_$newarchiv.tar.gz
dev=/devel/E2/dev.tar.gz
ARCHIVEPATH=/devel/E2/release$newarchiv
NOT_AWK3="$ARCHIVEPATH/usr/lib/python2.6/collections.py"
E2DIR=$HOME/newsvn/mt_build/stock/.roote23p1ufs910
VARDIR=$HOME/newsvn/mt_build/stock/.dat3p1ufs910
BACKUP=$HOME/newsvn/mt_build/stock/BACKUP
makeimagepath=$HOME/newsvn/mt_build
pythonpath=$E2DIR/usr/lib/python2.6

#sudo su

echo "START MAKEIMAGE NOW. TAKES AROUND 10MIN. TO END!"
#bootscripte erstellen
if [ -e $SCRIPTPATH/boot_orig_mod ]; then
rm $SCRIPTPATH/boot_orig_mod
rm $SCRIPTPATH/boot_flash_mod_own
rm $SCRIPTPATH/boot_flash_mod_public

var0=0
LIMIT=500
while [ $var0 -lt $LIMIT ]
do
  echo -e " " >> $SCRIPTPATH/boot_orig_mod 
  echo -e " " >> $SCRIPTPATH/boot_flash_mod_own      
  echo -e " " >> $SCRIPTPATH/boot_flash_mod_public           
  var0=`expr $var0 + 1`
done

echo -e "set bootargs ''console=ttyAS0,115200 root=/dev/nfs nfsroot=/devel/E2/EVO rw ip=$ipaddr:$serverip:$gateway:$netmask:kathrein:eth0:off mem=64m coprocessor_mem=4m@0x10000000,4m@0x10400000 rootdelay=4 nwhwconf=device:eth0,hwaddr:$ethaddr init=/bin/devinit''" >> $SCRIPTPATH/boot_orig_mod
echo -e " setenv bootcmd 'nfs a5000000 $serverip:/devel/E2/EVO/boot/uImage; bootm a5000000'" >> $SCRIPTPATH/boot_orig_mod
echo -e " boot" >> $SCRIPTPATH/boot_orig_mod
echo -e " " >> $SCRIPTPATH/boot_orig_mod

echo -e "set bootargs ''console=ttyAS0,115200 root=/dev/nfs nfsroot=/devel/E2/EVO rw ip=$ipaddr:$serverip:$gateway:$netmask:kathrein:eth0:off mem=64m coprocessor_mem=4m@0x10000000,4m@0x10400000 rootdelay=4 nwhwconf=device:eth0,hwaddr:$ethaddr init=/bin/devinit''" >> $SCRIPTPATH/boot_flash_mod_own
echo -e " setenv bootcmd 'nfs a5000000 $serverip:/devel/E2/EVO/boot/uImage; bootm a5000000'" >> $SCRIPTPATH/boot_flash_mod_own
echo -e " boot" >> $SCRIPTPATH/boot_flash_mod_own
echo -e " " >> $SCRIPTPATH/boot_flash_mod_own

echo -e "set bootargs ''console=ttyAS0,115200 root=/dev/nfs nfsroot=/devel/E2/EVO rw ip=$ipaddr:$serverip:$gateway:$netmask:kathrein:eth0:off mem=64m coprocessor_mem=4m@0x10000000,4m@0x10400000 rootdelay=4 nwhwconf=device:eth0,hwaddr:$ethaddr init=/bin/devinit''" >> $SCRIPTPATH/boot_flash_mod_public
echo -e " setenv bootcmd 'nfs a5000000 $serverip:/devel/E2/EVO/boot/uImage; bootm a5000000'" >> $SCRIPTPATH/boot_flash_mod_public
echo -e " boot" >> $SCRIPTPATH/boot_flash_mod_public
echo -e " " >> $SCRIPTPATH/boot_flash_mod_public
fi
find $SCRIPTPATH -xdev -name '*.*~' | xargs rm -f
find $SCRIPTPATH -xdev -name '*~' | xargs rm -f

sudo rm -rf $ARCHIVEPATH

# tdt entpacken und sysupdate drüberbügeln
if [ -e $updatepath/sysupdate$oldarchiv ]; then
cd $updatepath/sysupdate$oldarchiv
sudo tar -czvf $updatepath/sysupdate$newarchiv.tar.gz ./ >/dev/null 2>&1
mv /$updatepath/sysupdate$oldarchiv /$updatepath/sysupdate$newarchiv
fi
sudo rm -rf $ARCHIVEPATH
mkdir $ARCHIVEPATH
sudo rm -rf $ARCHIVEPATH.flash
mkdir $ARCHIVEPATH.flash
cp $tdt $ARCHIVEPATH
cp $updatepath/$update $ARCHIVEPATH/
cd /devel/E2
rm /devel/E2/EVO
ln -s $ARCHIVEPATH /devel/E2/EVO
cd $ARCHIVEPATH
sudo tar -xzvf $ARCHIVEPATH/release_incl_kernel_rev_$newarchiv.tar.gz >/dev/null 2>&1
sudo chmod 777 -R $ARCHIVEPATH
cd $ARCHIVEPATH
cp $makeimagepath/stock/uImage3p1miniufs910.ub $ARCHIVEPATH/boot/uImage
#rm -rf $ARCHIVEPATH/usr/lib/python2.6
#cp -rp $HOME/newsvn/mt_build/stock/python2.6 $ARCHIVEPATH/usr/lib/

#epg
if [ -e $ARCHIVEPATH/hdd/epg.dat ]; then
rm $ARCHIVEPATH/hdd/epg.dat
fi
if [ -e $ARCHIVEPATH/epg/epg.dat ]; then
rm $ARCHIVEPATH/epg/epg.dat
fi

#makeflashimage vorbereitung
rm -rf $E2DIR/*
rm -rf $VARDIR/*
cd $ARCHIVEPATH
sudo tar -czvf /devel/E2/release$newarchiv.tar.gz ./ >/dev/null 2>&1
cp /devel/E2/release$newarchiv.tar.gz $E2DIR/
cd $E2DIR
sudo tar -xzvf release$newarchiv.tar.gz >/dev/null 2>&1
sudo chmod 777 -R $E2DIR

#makeflashimage
#rm $E2DIR/var/usr
rm $E2DIR/boot/uImage
if [ -e $E2DIR/boot/vmlinux.ub ]; then
rm $E2DIR/boot/vmlinux.ub
fi
mv $E2DIR/var/* ../.dat3p1ufs910
find . -name 'keys' | xargs rm -rf >/dev/null 2>&1
find . -name 'emu' | xargs rm -rf >/dev/null 2>&1

if [ -e $VARDIR/usr/local/share/enigma2/mnt ]; then
   rm $VARDIR/usr/local/share/enigma2/mnt
fi

if [ -e $E2DIR/lib/modules/autofs4.ko ]; then
   rm $E2DIR/lib/modules/autofs4.ko
fi
if [ -e $E2DIR/lib/modules/mini_fo.ko ]; then
   rm $E2DIR/lib/modules/mini_fo.ko
fi
if [ -e $E2DIR/lib/modules/tun.ko ]; then
   rm $E2DIR/lib/modules/tun.ko
fi
rm -rf $E2DIR/usr/share/alsa
mv $E2DIR/usr/script $VARDIR/usr/
mv $E2DIR/usr/lib/ipkg $VARDIR/usr/lib/
cd $E2DIR/usr/lib
ln -s /var/usr/lib/ipkg
cd $E2DIR
mv $E2DIR/usr/tuxtxt $VARDIR/usr/
cd $E2DIR/usr
ln -s /var/usr/tuxtxt
cd $E2DIR
rm -rf $VARDIR/usr/local/share/enigma2/satellites.xml
mv $VARDIR/usr/local/share/enigma2/extended_satellites.xml $E2DIR/usr/local/share/
cd $VARDIR/etc/tuxbox
ln -s /usr/local/share/extended_satellites.xml
cd $E2DIR
rm -rf $pythonpath/lib-dynload/_testcapi.so
rm -rf $pythonpath/lib-dynload/cmath.so
rm -rf $pythonpath/lib-dynload/mmap.so
rm -rf $pythonpath/lib-dynload/strop.so
rm -rf $pythonpath/site-packages/twisted/internet/_dumbwin32proc.py
rm -rf $pythonpath/site-packages/twisted/internet/_win32serialport.py
rm -rf $pythonpath/site-packages/twisted/internet/_win32stdio.py
rm -rf $pythonpath/site-packages/twisted/internet/test
rm -rf $pythonpath/site-packages/twisted/python/_epoll.so
rm -rf $pythonpath/site-packages/twisted/python/test
rm -rf $pythonpath/site-packages/twisted/web2/test
rm -rf $pythonpath/site-packages/zope/interface/DEPENDENCIES.cfg
rm -rf $pythonpath/site-packages/zope/interface/PUBLICATION.cfg
rm -rf $pythonpath/site-packages/zope/interface/SETUP.cfg
rm -rf $pythonpath/site-packages/zope/interface/common
rm -rf $pythonpath/site-packages/zope/interface/tests
rm -rf $VARDIR/usr/local/share/enigma2/wait1.png
rm -rf $VARDIR/usr/local/share/enigma2/wait2.png
rm -rf $VARDIR/usr/local/share/enigma2/wait3.png
rm -rf $VARDIR/usr/local/share/enigma2/wait4.png
mv $VARDIR/usr/local/share/enigma2/skin_default $E2DIR/usr/local/share/
if [ -e $VARDIR/usr/local/share/enigma2/picon ]; then
mv $VARDIR/usr/local/share/enigma2/picon $E2DIR/usr/local/share/
fi
mv $VARDIR/usr/local/share/enigma2/skin_default.xml $E2DIR/usr/local/share/
#rm $VARDIR/usr/local/share/enigma2/skin.xml
mv $VARDIR/usr/local/share/enigma2/skin.xml $E2DIR/usr/local/share/
#mv $VARDIR/usr/local/share/enigma2/skin_birdie.xml $E2DIR/usr/local/share/
#mv $VARDIR/usr/local/share/enigma2/skin_normal.xml $E2DIR/usr/local/share/
#rm -rf $VARDIR/usr/local/share/enigma2/EV-HD_Shadow3
#mv $VARDIR/usr/local/share/enigma2/EV-HD_Shadow3 $E2DIR/usr/local/share/
cd $VARDIR/usr/local/share/enigma2
ln -s /usr/local/share/skin_default
ln -s /usr/local/share/skin_default.xml
ln -s /usr/local/share/skin.xml
touch $VARDIR/etc/.orig
mv $VARDIR/usr/local/share/enigma2/EVO-blackGlass-HD $E2DIR/usr/local/share/
listing=`ls $E2DIR/usr/local/share/EVO-blackGlass-HD`
mkdir $VARDIR/usr/local/share/enigma2/EVO-blackGlass-HD
cd $VARDIR/usr/local/share/enigma2/EVO-blackGlass-HD
for p in $listing
 do
   ln -s /usr/local/share/EVO-blackGlass-HD/$p
 done

#ln -s /usr/local/share/picon
#listing=`ls $E2DIR/usr/local/share/EV-HD_Shadow3`
#mkdir $VARDIR/usr/local/share/enigma2/EV-HD_Shadow3
#cd $VARDIR/usr/local/share/enigma2/EV-HD_Shadow3
#for a in $listing
# do
#   ln -s /usr/local/share/EV-HD_Shadow3/$a
# done



#qtembedded
#listing=`ls $updatepath/QTembedded/lib`
#mkdir $VARDIR/usr/QTembedded
#cd $E2DIR/usr/lib
#for b in $listing
# do
#   ln -s /var/usr/QTembedded/lib/$b
# done
#rm $VARDIR/usr/QTembedded/lib/fonts
#rm $VARDIR/usr/QTembedded/lib/qt-4.4.3
#ln -s $VARDIR/usr/QTembedded/lib/fonts
#ln -s $VARDIR/usr/QTembedded/lib/qt-4.4.3
#mkdir $VARDIR/usr/share/qt-4.4.3
#cd $E2DIR/usr/share
#ln -s /var/usr/QTembedded/share/qt-4.4.3

mv $E2DIR/lib/libipkg.so.0 $VARDIR/lib/
cd $E2DIR/lib
ln -s /var/lib/libipkg.so.0
cp $makeimagepath/stock/allerlei $VARDIR/etc/init.d/
cd $E2DIR/usr/local/share/skin_default
rm $E2DIR/usr/local/share/skin_default/spinner
ln -s /var/usr/local/share/enigma2/spinner
cd $E2DIR
mv $VARDIR/etc/swapsize.bu $VARDIR/etc/swapsize.old
#radio
#mv $VARDIR/media/radio $E2DIR/
#listing=`ls $E2DIR/radio`
#mkdir $VARDIR/media/radio
#cd $VARDIR/media/radio
#for c in $listing
# do
#   ln -s /radio/$c
# done

rm $E2DIR/*.tar.gz
echo "Cleanup done"
#flashimage nochmals packen um from nfs zu booten
cp -rp $VARDIR/* $E2DIR/var/
cp $ARCHIVEPATH/boot/uImage $E2DIR/boot/
cd $E2DIR
sudo tar -czvf /devel/E2/release$newarchiv.flash.tar.gz ./ >/dev/null 2>&1
if [ ! -e /devel/E2/release$newarchiv.flash ]; then
   mkdir /devel/E2/release$newarchiv.flash
fi
if [ -e /devel/E2/release$newarchiv.flash ]; then
   sudo rm -rf /devel/E2/release$newarchiv.flash/*
fi
cp /devel/E2/release$newarchiv.flash.tar.gz /devel/E2/release$newarchiv.flash
cd /devel/E2/release$newarchiv.flash
sudo tar -xzvf release$newarchiv.flash.tar.gz >/dev/null 2>&1
sudo chmod 777 -R /devel/E2/release$newarchiv.flash
rm -rf $E2DIR/var/*
rm -rf $E2DIR/boot/uImage*
#mv $E2DIR/usr/local/share/EV-HD_Shadow3 $VARDIR/usr/local/share/enigma2/
#cutecom teil 2
cutecom
echo "weiter gehts..."
cd /devel/E2/release$newarchiv.flash
sudo tar -czvf /devel/E2/release$newarchiv.flash.tar.gz ./ >/dev/null 2>&1
sudo rm -rf /devel/E2/release_nfs/*
cp /devel/E2/release$newarchiv.flash.tar.gz /devel/E2/release_nfs/ 
cd /devel/E2/release_nfs
sudo tar -xzvf release$newarchiv.flash.tar.gz >/dev/null 2>&1
sudo chmod 777 -R /devel/E2/release_nfs
rm /devel/E2/release_nfs/release$newarchiv.flash.tar.gz

echo "MAKE OWN"
#lircd
mkdir $VARDIR/usr/bin
#cp $updatepath/lircd $VARDIR/usr/bin/ #macht public
#cd $E2DIR/usr/bin
#ln -s /var/usr/bin/lircd

#   mkdir $VARDIR/saver
#   mv $VARDIR/saver/* $E2DIR/saver/
#   listing=`ls $E2DIR/saver`
#   cd $VARDIR/saver
#   for m in $listing
#    do
#      ln -s /saver/$m
#   done
#cd $E2DIR
#   mkdir $VARDIR/saver
#   mkdir $E2DIR/saver
#   mv $VARDIR/saver/* $E2DIR/saver/
#   mv $E2DIR/saver/saver1_kids.mvi.off.bu $E2DIR/saver/saver1_kids.mvi.off
#   mv $E2DIR/saver/saver2_kids.mvi.off.bu $E2DIR/saver/saver2_kids.mvi.off
#   listing=`ls $E2DIR/saver`
#   cd $VARDIR/saver
#   for m in $listing
#    do
#      ln -s /saver/$m
#   done
mv $E2DIR/usr/bin/tuxtxt $VARDIR/usr/bin/
cd $E2DIR/usr/bin
ln -s /var/usr/bin/tuxtxt
cp $dev $E2DIR/dev/
cd $E2DIR/dev
sudo tar -xzvf dev.tar.gz
sudo rm ./dev.tar.gz
sudo chmod 777 -R $E2DIR/dev
cd $E2DIR
ln -s /var/log
#final flashimage bauen
#   sed s"@^\\(\\s*\\)#mount -t jffs2 /dev/mtdblock3 /var\$@\\mount -t jffs2 /dev/mtdblock3 /var@" -i $E2DIR/etc/init.d/mountvirtfs
   rm -rf $makeimagepath/tufsbox/cdkflash/mtd*.ubimage*
   rm -rf $makeimagepath/tufsbox/cdkflash/mtd*.enigma2*
   rm -rf $makeimagepath/tufsbox/cdkflash/var-stock-enigma2*
   rm -rf $makeimagepath/tufsbox/cdkflash/*.update*
   rm -rf $makeimagepath/tufsbox/cdkflash/stock*.img*

cd $E2DIR
touch $VARDIR/etc/.evofirstboot
touch $E2DIR/etc/.3p1mini
cp $makeimagepath/stock/rcS.ufs910 $E2DIR/etc/init.d/rcS
cp $makeimagepath/stock/EVO_rcS.ufs910 $VARDIR/etc/init.d/EVO_rcS
echo "config.Nims.0.diseqcMode=single" > $VARDIR/usr/local/share/enigma2/settings
echo "config.Nims.0.configMode=simple" >> $VARDIR/usr/local/share/enigma2/settings
cp $dev $E2DIR/dev/
cd $E2DIR/dev
sudo tar -xzvf dev.tar.gz
sudo chmod 777 -R $E2DIR/dev
sudo rm $E2DIR/dev/dev.tar.gz
touch $E2DIR/etc/.3p1mini
   cd $makeimagepath/cvs/cdk
   make clean
#   ./make.sh 3p1miniufs910
   ./make.sh 3p1ufs910
#   make flash-stock-enigma23p1miniufs910-full
   make flash-stock-enigma23p1ufs910-full
#   mv $makeimagepath/tufsbox/cdkflash/mtdblock3.root-stock.enigma2 $makeimagepath/tufsbox/cdkflash/mtdblock2.root-stock.enigma2
#   mv $makeimagepath/tufsbox/cdkflash/mtdblock6.var-stock.enigma2 $makeimagepath/tufsbox/cdkflash/mtdblock3.var-stock.enigma2
#cp $makeimagepath/stock/rcS.ufs910 $E2DIR/etc/init.d/rcS

   sudo chmod 777 -R $makeimagepath/tufsbox/cdkflash

   if [ ! -e $makeimagepath/tufsbox/cdkflash/own_ufs910 ]; then
      mkdir $makeimagepath/tufsbox/cdkflash/own_ufs910
   fi

   if [ -e $makeimagepath/tufsbox/cdkflash/own_ufs910/mtd*.ubimage* ] || [ -e $makeimagepath/tufsbox/cdkflash/own_ufs910/mtd*.enigma2* ] || [ -e $makeimagepath/tufsbox/cdkflash/own_ufs910/var-stock-enigma2* ] || [ -e $makeimagepath/tufsbox/cdkflash/own_ufs910/*.update* ] || [ -e $makeimagepath/tufsbox/cdkflash/own_ufs910/*stock*.img* ]; then
      rm -rf $makeimagepath/tufsbox/cdkflash/own_ufs910/mtd*.ubimage*
      rm -rf $makeimagepath/tufsbox/cdkflash/own_ufs910/mtd*.enigma2*
      rm -rf $makeimagepath/tufsbox/cdkflash/own_ufs910/var-stock-enigma2*
      rm -rf $makeimagepath/tufsbox/cdkflash/own_ufs910/*.update*
      rm -rf $makeimagepath/tufsbox/cdkflash/own_ufs910/stock*.img*
      rm -rf $makeimagepath/tufsbox/cdkflash/*.update*
   fi

   mv $makeimagepath/tufsbox/cdkflash/mtdblock1*.ubimage* $makeimagepath/tufsbox/cdkflash/own_ufs910/
   mv $makeimagepath/tufsbox/cdkflash/mtdblock2*.enigma2* $makeimagepath/tufsbox/cdkflash/own_ufs910/
   mv $makeimagepath/tufsbox/cdkflash/mtdblock3*.enigma2* $makeimagepath/tufsbox/cdkflash/own_ufs910/
   mv $makeimagepath/tufsbox/cdkflash/stock*.img* $makeimagepath/tufsbox/cdkflash/own_ufs910/
      rm -rf $makeimagepath/tufsbox/cdkflash/own_ufs910/*.update*


#fi
cd $E2DIR
#$SCRIPTPATH/makepublic.sh &
exit


