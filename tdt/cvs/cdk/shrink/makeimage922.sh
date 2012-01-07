#!/bin/sh
## Variablen definieren
ipaddr=192.168.178.31
serverip=192.168.178.27
gatewayip=192.168.178.1
ethaddr=00:50:fd:ff:80:2d
netmask=255.255.255.0

SCRIPTPATH=/devel/E2/shrink
newarchiv=`cat $SCRIPTPATH/rev922.version`
oldarchiv=$(($newarchiv-1))
updatepath=/home/juppi/ufs922
update=sysupdate$newarchiv.ufs922.tar.gz
tdt=$updatepath/release_incl_kernel_rev_$newarchiv'_ufs922'.tar.gz
dev=/devel/E2/922/dev922.tar.gz
ARCHIVEPATH=/devel/E2/922/922_rev$newarchiv
NOT_AWK3="$ARCHIVEPATH/usr/lib/python2.6/collections.py"
E2DIR=/home/juppi/newsvn/mt_build/stock/.roote23p1ufs922
VARDIR=/home/juppi/newsvn/mt_build/stock/.dat3p1ufs922
USRDIR=/home/juppi/newsvn/mt_build/stock/.usre23p1ufs922
BACKUP=/home/juppi/newsvn/mt_build/stock/BACKUP
KERNELDIR=/home/juppi/newsvn/mt_build/stock
makeimagepath=/home/juppi/newsvn/mt_build
pythonpath=$E2DIR/usr/lib/python2.6

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

echo -e " set bootargs ''console=ttyAS0,115200 root=/dev/nfs nfsroot=/devel/E2/922/922_rev$newarchiv rw ip=$ipaddr:$serverip:$gatewayip:$netmask:kathrein:eth0:off mem=128m coprocessor_mem=4m@0x10000000,4m@0x10400000 init=/bin/devinit''" >> $SCRIPTPATH/boot_orig_mod
echo -e " setenv bootcmd 'nfs a5000000 $serverip:/devel/E2/922/922_rev$newarchiv/boot/uImage; bootm a5000000'" >> $SCRIPTPATH/boot_orig_mod
echo -e " boot" >> $SCRIPTPATH/boot_orig_mod
echo -e " " >> $SCRIPTPATH/boot_orig_mod

echo -e " set bootargs ''console=ttyAS0,115200 root=/dev/nfs nfsroot=/devel/E2/922/922_rev$newarchiv.flash rw ip=$ipaddr:$serverip:$gatewayip:$netmask:kathrein:eth0:off mem=128m coprocessor_mem=4m@0x10000000,4m@0x10400000 init=/bin/devinit''" >> $SCRIPTPATH/boot_flash_mod_own
echo -e " setenv bootcmd 'nfs a5000000 $serverip:/devel/E2/922/922_rev$newarchiv.flash/boot/uImage; bootm a5000000'" >> $SCRIPTPATH/boot_flash_mod_own
echo -e " boot" >> $SCRIPTPATH/boot_flash_mod_own
echo -e " " >> $SCRIPTPATH/boot_flash_mod_own

echo -e " set bootargs ''console=ttyAS0,115200 root=/dev/nfs nfsroot=/devel/E2/922/922_rev$newarchiv.public rw ip=$ipaddr:$serverip:$gatewayip:$netmask:kathrein:eth0:off mem=128m coprocessor_mem=4m@0x10000000,4m@0x10400000 init=/bin/devinit''" >> $SCRIPTPATH/boot_flash_mod_public
echo -e " setenv bootcmd 'nfs a5000000 $serverip:/devel/E2/922/922_rev$newarchiv.public/boot/uImage; bootm a5000000'" >> $SCRIPTPATH/boot_flash_mod_public
echo -e " boot" >> $SCRIPTPATH/boot_flash_mod_public
echo -e " " >> $SCRIPTPATH/boot_flash_mod_public
fi

find $SCRIPTPATH -xdev -name '*.*~' | xargs rm -rf
find $SCRIPTPATH -xdev -name '*~' | xargs rm -rf

sudo rm -rf $ARCHIVEPATH

# tdt entpacken und sysupdate drüberbügeln
if [ -e $updatepath/sysupdate$oldarchiv.ufs922 ]; then
cd $updatepath/sysupdate$oldarchiv.ufs922
sudo tar -czvf $updatepath/sysupdate$newarchiv.ufs922.tar.gz ./ >/dev/null 2>&1
mv /$updatepath/sysupdate$oldarchiv.ufs922 /$updatepath/sysupdate$newarchiv.ufs922
fi
sudo rm -rf $ARCHIVEPATH
mkdir $ARCHIVEPATH
sudo rm -rf $ARCHIVEPATH.flash
mkdir $ARCHIVEPATH.flash
cp $tdt $ARCHIVEPATH
cp $updatepath/$update $ARCHIVEPATH/
cd $ARCHIVEPATH
sudo tar -xzvf $ARCHIVEPATH/release_incl_kernel_rev_$newarchiv'_ufs922.tar'.gz >/dev/null 2>&1
sudo chmod 777 -R $ARCHIVEPATH
rm $ARCHIVEPATH/bin/mount
rm -rf $ARCHIVEPATH/etc
rm -rf $ARCHIVEPATH/mnt
#rm -rf $ARCHIVEPATH/hdd
#rm -rf $ARCHIVEPATH/media
rm -rf $ARCHIVEPATH/usr/local/share/enigma2/skin_default
rm -rf $ARCHIVEPATH/usr/lib/python2.6
sudo tar -xzvf $ARCHIVEPATH/sysupdate$newarchiv.ufs922.tar.gz >/dev/null 2>&1
sudo chmod 777 -R $ARCHIVEPATH
cd /devel/E2/922
rm /devel/E2/922/EVO
ln -s $ARCHIVEPATH /devel/E2/922/EVO
cd $ARCHIVEPATH
echo "P A T C H I N G   PY/XML ... W A I T ..."
cd /devel/E2/922/EVO && patch -p1 </devel/E2/shrink/Patches/EVOimage_patches_ufs922.patch
rm $ARCHIVEPATH/dev/*
rm $ARCHIVEPATH/var/etc/.firstboot
cp $dev $ARCHIVEPATH/dev/
cd $ARCHIVEPATH/dev
sudo tar -xzvf dev922.tar.gz >/dev/null 2>&1
rm $ARCHIVEPATH/dev/*.tar.gz
rm $ARCHIVEPATH/*.tar.gz
cp $updatepath/enigma2.$newarchiv.ufs922 $ARCHIVEPATH/usr/local/bin/
mv $ARCHIVEPATH/usr/local/bin/enigma2.$newarchiv.ufs922 $ARCHIVEPATH/usr/local/bin/enigma2
#bootlogo
cp /home/juppi/Desktop/TMPGEnc-2.525.64.184-EN-Free/bootlogo922.mvi $ARCHIVEPATH/boot
mv $ARCHIVEPATH/boot/bootlogo922.mvi $ARCHIVEPATH/boot/bootlogo.mvi.orig
cd $ARCHIVEPATH/boot
rm $ARCHIVEPATH/boot/bootlogo.mvi
ln -s /var/boot/bootlogo.mvi
mkdir $ARCHIVEPATH/var/boot
cd $ARCHIVEPATH/var/boot
ln -s /boot/bootlogo.mvi.orig bootlogo.mvi
cp /home/juppi/Desktop/TMPGEnc-2.525.64.184-EN-Free/radio.mvi $ARCHIVEPATH/usr/local/share/enigma2
cp /home/juppi/Desktop/TMPGEnc-2.525.64.184-EN-Free/bootlogo922_kids.mvi $ARCHIVEPATH/boot
mv $ARCHIVEPATH/boot/bootlogo922_kids.mvi $ARCHIVEPATH/boot/bootlogo_kids.mvi
cp $KERNELDIR/uImage3p1ufs922.ub $ARCHIVEPATH/boot/uImage
#mv $ARCHIVEPATH/boot/uImage3p1ufs922.ub $ARCHIVEPATH/boot/uImage
sudo chmod 777 -R $ARCHIVEPATH/boot
cp /home/juppi/Desktop/TMPGEnc-2.525.64.184-EN-Free/radio_kids.mvi $ARCHIVEPATH/usr/local/share/enigma2/
#cp /home/juppi/Desktop/TMPGEnc-2.525.64.184-EN-Free/saver1_kids.mvi $ARCHIVEPATH/var/saver/
#cp /home/juppi/Desktop/TMPGEnc-2.525.64.184-EN-Free/saver2_kids.mvi $ARCHIVEPATH/var/saver/
#mv $ARCHIVEPATH/var/saver/saver1_kids.mvi $ARCHIVEPATH/var/saver/saver1_kids.mvi.off.bu 
#mv $ARCHIVEPATH/var/saver/saver2_kids.mvi $ARCHIVEPATH/var/saver/saver2_kids.mvi.off.bu 

echo "===================================================================================="
echo "PLEASE CHANGE EVO # IN /stock/EVO_rcS.ufs922 MANUALLY BEFORE PACKAGING FOR FLASH !!!"
echo "===================================================================================="
#old922version=`cat /home/juppi/newsvn/sysupdate$newarchiv.ufs922/etc/init.d/rcS | grep -m1 "LOAD EVO" | cut -d " " -f7`
#sed s"@^\\(\\s*\\)   echo LOAD EVO $old922version > /dev/vfd\$@\\   echo LOAD EVO $newarchiv > /dev/vfd@" -i $ARCHIVEPATH/etc/init.d/rcS
mv $ARCHIVEPATH/var/etc/swapsize.old $ARCHIVEPATH/var/etc/swapsize.bu
imageinfo=`cat $ARCHIVEPATH/var/etc/imageinfo | grep -v "BASED"`
imageinfo="$imageinfo\n\nBASED ON TDT-REV-$newarchiv"
echo -e "$imageinfo" > $ARCHIVEPATH/var/etc/imageinfo

#new rm list
rm $ARCHIVEPATH/bin/tfd2mtd
rm $ARCHIVEPATH/bin/tffpctl
rm $ARCHIVEPATH/bin/stslave
#rm $ARCHIVEPATH/etc/fonts/fonts.conf
#rm $ARCHIVEPATH/etc/fonts/conf.d/20-fix-globaladvance.conf
#rm $ARCHIVEPATH/etc/fonts/conf.d/70-no-bitmaps.conf
#rm $ARCHIVEPATH/etc/fonts/conf.d/80-delicious.conf
#rm $ARCHIVEPATH/etc/fonts/conf.d/65-khmer.conf
#rm $ARCHIVEPATH/etc/fonts/conf.d/51-local.conf
#rm $ARCHIVEPATH/etc/fonts/conf.d/10-sub-pixel-bgr.conf
#rm $ARCHIVEPATH/etc/fonts/conf.d/30-metric-aliases.conf
#rm $ARCHIVEPATH/etc/fonts/conf.d/40-nonlatin.conf
#rm $ARCHIVEPATH/etc/fonts/conf.d/60-latin.conf
#rm $ARCHIVEPATH/etc/fonts/conf.d/49-sansserif.conf
#rm $ARCHIVEPATH/etc/fonts/conf.d/69-unifont.conf
#rm $ARCHIVEPATH/etc/fonts/conf.d/65-nonlatin.conf
#rm $ARCHIVEPATH/etc/fonts/conf.d/10-autohint.conf
#rm $ARCHIVEPATH/etc/fonts/conf.d/10-sub-pixel-vrgb.conf
#rm $ARCHIVEPATH/etc/fonts/conf.d/45-latin.conf
#rm $ARCHIVEPATH/etc/fonts/conf.d/50-user.conf
#rm $ARCHIVEPATH/etc/fonts/conf.d/30-urw-aliases.conf
#rm $ARCHIVEPATH/etc/fonts/conf.d/10-no-sub-pixel.conf
#rm $ARCHIVEPATH/etc/fonts/conf.d/65-fonts-persian.conf
#rm $ARCHIVEPATH/etc/fonts/conf.d/70-yes-bitmaps.conf
#rm $ARCHIVEPATH/etc/fonts/conf.d/20-unhint-small-vera.conf
#rm $ARCHIVEPATH/etc/fonts/conf.d/10-sub-pixel-vbgr.conf
#rm $ARCHIVEPATH/etc/fonts/conf.d/10-unhinted.conf
#rm $ARCHIVEPATH/etc/fonts/conf.d/90-synthetic.conf
#rm $ARCHIVEPATH/etc/fonts/conf.d/10-sub-pixel-rgb.conf
#rm $ARCHIVEPATH/etc/fonts/conf.dl/25-unhint-nonlatin.conf
#rm $ARCHIVEPATH/etc/fonts/fonts.dtd
rm $ARCHIVEPATH/lib/libnss_hesiod-2.5.so
rm $ARCHIVEPATH/lib/libanl-2.5.so
rm $ARCHIVEPATH/lib/libSegFault.so
rm $ARCHIVEPATH/lib/libpcprofile.so
rm $ARCHIVEPATH/lib/libBrokenLocale-2.5.so
rm $ARCHIVEPATH/lib/libnss_nisplus-2.5.so
rm $ARCHIVEPATH/lib/libmemusage.so
rm $ARCHIVEPATH/lib/libss.so.2.0
rm $ARCHIVEPATH/lib/libthread_db-1.0.so
rm $ARCHIVEPATH/usr/lib/libfbsplashrender.so.1.0.0
rm $ARCHIVEPATH/usr/lib/libalsaplayer.so.0.0.2
rm $ARCHIVEPATH/usr/lib/libpthread.so
rm $ARCHIVEPATH/usr/lib/libtermcap.so.2.0.8
rm $ARCHIVEPATH/usr/lib/libasound.so.2.0.0
rm $ARCHIVEPATH/usr/lib/libfontconfig.so.1.3.0
rm $ARCHIVEPATH/usr/lib/libform.so.5.5
rm $ARCHIVEPATH/usr/lib/liblcms.so.1.0.16
rm $ARCHIVEPATH/usr/lib/libreadline.so.5.2
rm $ARCHIVEPATH/usr/lib/libc.so
rm $ARCHIVEPATH/usr/lib/libexslt.so.0.8.13
rm $ARCHIVEPATH/usr/lib/libhistory.so.5.2
rm $ARCHIVEPATH/usr/lib/libpng.so.3.16.0
rm $ARCHIVEPATH/usr/lib/libopen.so.0.0
rm $ARCHIVEPATH/usr/lib/libmenu.so.5.5
rm $ARCHIVEPATH/usr/lib/libpanel.so.5.5
rm $ARCHIVEPATH/usr/lib/libexpat.so.1.5.2
rm $ARCHIVEPATH/usr/lib/libmng.so.1.0.0
rm $ARCHIVEPATH/usr/lib/libfbsplash.so.1.0.0
rm $ARCHIVEPATH/usr/lib/libcurses.so
rm $ARCHIVEPATH/sbin/killall5
#rm $ARCHIVEPATH/usr/lib/python2.6/lib-dynload/_curses_panel.so # 11564 Bytes
#rm $ARCHIVEPATH/usr/lib/python2.6/lib-dynload/readline.so # 21664 Bytes
#rm $ARCHIVEPATH/usr/lib/python2.6/lib-dynload/nis.so # 10936 Bytes
#rm $ARCHIVEPATH/usr/lib/python2.6/lib-dynload/_curses.so # 78832 Bytes
#rm $ARCHIVEPATH/usr/lib/python2.6/site-packages/twisted/protocols/amp.py # 73470 Bytes
#rm $ARCHIVEPATH/usr/lib/python2.6/lib-dynload/parser.so # 60092 Bytes
#rm $ARCHIVEPATH/usr/lib/python2.6/site-packages/twisted/protocols/ftp.py # 83585 Bytes
#rm $ARCHIVEPATH/usr/lib/python2.6/site-packages/twisted/protocols/sip.py # 37484 Bytes
#neu wech
#rm $ARCHIVEPATH/usr/lib/python2.6/lib-dynload/audioop.so
#rm $ARCHIVEPATH/usr/lib/python2.6/lib-dynload/_bytesio.so
#rm $ARCHIVEPATH/usr/lib/python2.6/lib-dynload/_ctypes_test.so
#rm $ARCHIVEPATH/usr/lib/python2.6/lib-dynload/_fileio.so
#rm $ARCHIVEPATH/usr/lib/python2.6/lib-dynload/future_builtins.so
#rm $ARCHIVEPATH/usr/lib/python2.6/lib-dynload/imageop.so
#rm $ARCHIVEPATH/usr/lib/python2.6/lib-dynload/linuxaudiodev.so
#rm $ARCHIVEPATH/usr/lib/python2.6/lib-dynload/_lsprof.so
#rm $ARCHIVEPATH/usr/lib/python2.6/lib-dynload/_multibytecodec.so
#rm $ARCHIVEPATH/usr/lib/python2.6/lib-dynload/ossaudiodev.so

#lzma
if [ ! -e $ARCHIVEPATH/lzma ]; then
   mkdir $ARCHIVEPATH/lzma
fi
cd $ARCHIVEPATH/boot
lzma audio.elf
lzma video.elf
ln -s /lzma/audio.elf
ln -s /lzma/video.elf
#cd $ARCHIVEPATH/lib/modules
#lzma player2.ko
#ln -s /lzma/player2.ko

#png optimieren
list=`find $ARCHIVEPATH -name "*.png" -print`
for i in $list
do
if [ ! -L $i ]
then
advpng -z -4 $i >/dev/null 2>&1
fi
done
###########################################
## .py Files holen

rm $SCRIPTPATH/pyshrinklist.txt 2>/dev/null
find $ARCHIVEPATH -name "*.py" -print >> $SCRIPTPATH/pyshrinklist.txt
list=`cat $SCRIPTPATH/pyshrinklist.txt`

###########################################
## Funktionen

backup() {
  file=`echo -E "$file" | sed s/"# public symbols"/"@ public symbols"/`
  file=`echo -E "$file" | sed 's/^\s*#.*coding/@coding/'`
}

restore() {
  file=`echo -E "$file" | sed s/"@ public symbols"/"# public symbols"/`
  file=`echo -E "$file" | sed 's/^\s*@coding/# -*- coding/'`
} 

softshrink() {
  file=`echo -E "$file" | tr -d "\r"`
  file=`echo -E "$file" | sed s/"\s*$"//`
  file=`echo -E "$file" | sed '/^\s*<!--.*-->\s*$/d'`
  file=`echo -E "$file" | sed '/^\s*<!--/,/-->\s*$/ {/.*/d}'`
  file=`echo -E "$file" | sed /"^\s*#.*$"/d`
}

hardshrink() {
  file=`echo -E "$file" | awk -f $SCRIPTPATH/pyshrink1.awk`
  file=`echo -E "$file" | awk -f $SCRIPTPATH/pyshrink2.awk`
  grep "	" $i >/dev/null 2>&1; ret1=$?
  echo "$NOT_AWK3" | grep "$i" >/dev/null 2>&1; ret2=$?
  if [ $ret1 -ne 0 -a $ret2 -ne 0 ]
  then
   file=`echo -E "$file" | awk -f $SCRIPTPATH/pyshrink3.awk`
  fi
  file=`echo -E "$file" | sed s/" #.*"//g`
}

lastshrink() {
  file=`echo -E "$file" | sed /"^\s*$"/d`
}

###########################################
## shrinke alle .py Files

for i in $list
do
 echo "Shrinke: $i" >/dev/null 2>&1
 file=`cat $i`

 # tr -d "\r"				-> lösche alle \r
 # sed s/"\s*$"//`			-> lösche alle endenden Leerzeichen, Tabs ,...
 # sed '/^\s*<!--.*-->\s*$/d'		-> lösche alles zwischen <!-- und -->
 # sed '/^\s*<!--/,/-->\s*$/ {/.*/d}'	-> lösche alles zwischen <!-- und --> (mehrere Zeilen)
 # sed /"^\s*#.*$"/d			-> lösche alle Zeilen die mit # beginnen
 # sed s/" #.*"//g			-> lösche alles hinter einen leerzeichen# inkl. leerzeichen#
 # sed /"^\s*$"/d			-> lösche alle Leerzeilen

 backup
 softshrink
 hardshrink
 lastshrink
 restore

 echo -E "$file" > $i.tmp.py
 unset ret
 ret=`$SCRIPTPATH/mkpyc.py $i.tmp.py 2>&1`
 if [ ! -z "$ret" ]
 then
  echo "Repariere: $i"
  fehler=`expr $fehler + 1`
  file=`cat $i`
  backup
  softshrink
  lastshrink
  restore
  echo -E "$file" > $i
 else
  echo -E "$file" > $i
 fi
 rm $i.tmp.py 2>/dev/null
 rm $i.tmp.pyc 2>/dev/null
done

rm $SCRIPTPATH/pyshrinklist.txt 2>/dev/null
echo "Anzahl reparierte Fehler: $fehler"

#py recompilen
#list=`cat $SCRIPTPATH/startpy.txt`
list=`find $ARCHIVEPATH -name "*.py" -print`
for i in $list
do
bn=`basename "$i"`
if [ "$bn" == "mytest.py" ]; then continue; fi
dn=`dirname "$i"`
#x=`find $ARCHIVEPATH -name "$i" -print`
#bn=`basename "$x"`
#dn=`dirname "$x"`
cd $dn
py_compilefiles $bn
bnc="$bn"c
if [ -e $bnc ]; then rm $bn; fi
#rm $dn/$bn
done

cp $updatepath/sysupdate$newarchiv.ufs922/usr/lib/enigma2/python/Plugins/Extensions/EVOPanel/plugin.pyc $ARCHIVEPATH/usr/lib/enigma2/python/Plugins/Extensions/EVOPanel/
sudo chmod 777 -R $ARCHIVEPATH
#cutecom
cutecom
echo "weiter gehts..."

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
#rm -rf $USRDIR/*
cd $ARCHIVEPATH
sudo tar -czvf /devel/E2/922/922_rev$newarchiv.ufs922.tar.gz ./ >/dev/null 2>&1
cp /devel/E2/922/922_rev$newarchiv.ufs922.tar.gz $E2DIR/
cd $E2DIR
sudo tar -xzvf 922_rev$newarchiv.ufs922.tar.gz >/dev/null 2>&1
sudo chmod 777 -R $E2DIR
rm $E2DIR/*.tar.gz

#makeflashimage
# set -x
DIR=`pwd`
rm -rf $BACKUP/*
mkdir $BACKUP/SYS
mkdir $BACKUP/EXT
mkdir $BACKUP/PLUG
cd $E2DIR
cp $E2DIR/usr/lib/enigma2/python/Plugins/__init__.pyc  $BACKUP/PLUG/ 
cp $E2DIR/usr/lib/enigma2/python/Plugins/Plugin.pyc  $BACKUP/PLUG/ 
cp $E2DIR/usr/lib/enigma2/python/Plugins/SystemPlugins/__init__.pyc  $BACKUP/SYS/ 
cp $E2DIR/usr/lib/enigma2/python/Plugins/Extensions/__init__.pyc  $BACKUP/EXT/ 
rm -rf $E2DIR/usr/lib/python2.6
cp -rp /home/juppi/newsvn/mt_build/stock/python2.6 $E2DIR/usr/lib/
#rm $E2DIR/usr/lib/enigma2/python/Tools/ISO639.py
#rm $E2DIR/usr/lib/python2.6/encodings/utf_16_be.py
#rm $E2DIR/usr/lib/python2.6/encodings/utf_8.py
#rm $E2DIR/usr/lib/python2.6/encodings/latin_1.py
#rm $E2DIR/usr/lib/python2.6/mimetypes.py
find $E2DIR/usr/lib/enigma2/python/Plugins -name *.py | xargs rm -rf
rm $E2DIR/var/usr
rm $E2DIR/boot/uImage
if [ -e $E2DIR/boot/vmlinux.ub ]; then
rm $E2DIR/boot/vmlinux.ub
fi
#rm $E2DIR/bin/evtest
rm $E2DIR/bin/rset
rm $E2DIR/lib/libncurses*
#rm $E2DIR/lib/libcurses*
cd $E2DIR/usr/lib
rm $E2DIR/usr/lib/libssl.so.0.9.8 >/dev/null 2>&1
rm $E2DIR/usr/lib/libssl.so >/dev/null 2>&1
rm $E2DIR/usr/lib/libcrypto.so.0.9.8 >/dev/null 2>&1
rm $E2DIR/usr/lib/libcrypto.so >/dev/null 2>&1
ln -s /var/usr/lib/libcrypto.so.0.9.8
ln -s /var/usr/lib/libcrypto.so.0.9.8 libcrypto.so.0.9.7
ln -s /var/usr/lib/libcrypto.so.0.9.8 libcrypto.so
rm $E2DIR/usr/lib/python2.6/lib-dynload/_codecs_*.so >/dev/null 2>&1
rm -rf $E2DIR/usr/lib/python2.6/wsgiref >/dev/null 2>&1
rm -rf $E2DIR/usr/lib/python2.6/sqlite3 >/dev/null 2>&1
rm -rf $E2DIR/usr/lib/python2.6/hotshot >/dev/null 2>&1
rm -rf $E2DIR/usr/lib/python2.6/idlelib >/dev/null 2>&1
rm -rf $E2DIR/usr/lib/python2.6/lib-tk >/dev/null 2>&1
rm -rf $E2DIR/usr/lib/python2.6/plat-linux2 >/dev/null 2>&1
rm -rf $E2DIR/usr/lib/python2.6/json >/dev/null 2>&1
rm -rf $E2DIR/usr/lib/python2.6/lib2to3 >/dev/null 2>&1
rm -rf $E2DIR/usr/lib/python2.6/lib-old >/dev/null 2>&1
rm -rf $E2DIR/usr/lib/python2.6/multiprocessing >/dev/null 2>&1
rm -rf $E2DIR/usr/lib/python2.6/site-packages/OpenSSL >/dev/null 2>&1
cd $E2DIR
#stripit teil ##################################################
i=bigger:py:pyc
i1=`echo $i | cut -d":" -f2`
  i2=`echo $i | cut -d":" -f3`

  echo "Loesche alle: $i1 ohne $i2"
  l1=`find $E2DIR -name "*.$i1" -printf "#%p#\n" | sed s/$i1#/#/`
  l2=`find $E2DIR -name "*.$i2" -printf "#%p#\n" | sed s/$i2#/#/`
  for f1 in $l1
  do
   echo "$l2" | grep "$f1" >/dev/null
   if [ $? -ne 0 ]
   then
    echo -e "$f1$i1" | tr -d "#" >> /tmp/py-single-wech.txt
   fi
  done
single=`cat /tmp/py-single-wech.txt`
for y in $single
do
rm $y
done
#if [ -e /tmp/py-single-wech.txt ]; then
rm /tmp/py-single-wech.txt >/dev/null 2>&1
#fi
  echo "Loesche alle: $i1 > $i2 oder $i2 > $i1" >/dev/null 2>&1
  l1=`find $E2DIR -name "*.$i1" -printf "#%p#:%s\n" | sed s/$i1#/#/`
  l2=`find $E2DIR -name "*.$i2" -printf "#%p#:%s\n" | sed s/$i2#/#/`
  for f2 in $l2
  do
   f2n=`echo $f2 | cut -d":" -f1`
   f2s=`echo $f2 | cut -d":" -f2`
   f1=`echo "$l1" | tr " " "\n" | grep "$f2n"`
   if [ $? -eq 0 ]
   then
    f1n=`echo $f1 | cut -d":" -f1`
    f1s=`echo $f1 | cut -d":" -f2`
    if [ "$f2s" -gt "$f1s" ]
    then
     echo -e "$f2n$i2" | tr -d "#" >> /tmp/bigger-wech.txt
    else
     echo -e "$f1n$i1" | tr -d "#" >> /tmp/bigger-wech.txt
    fi
   fi
  done
bigger=`cat /tmp/bigger-wech.txt`
for z in $bigger
do
rm $z
done
rm /tmp/bigger-wech.txt >/dev/null 2>&1
cd $E2DIR
find ./usr/lib/enigma2/python | egrep '\.pyo$' > /tmp/pyweg
for i in `cat /tmp/pyweg`; do echo $i; mv $i $BACKUP ; done
echo "enigma2 done"
find ./usr/lib/python2.6 | egrep '\.pyo$' > /tmp/pyweg
for i in `cat /tmp/pyweg`; do echo $i; mv $i $BACKUP ; done
mkdir -p $VARDIR/usr/local/share
mv $E2DIR/usr/local/share/enigma2 $VARDIR/usr/local/share
mv $E2DIR/usr/local/share/keymaps $VARDIR/usr/local/share
cd $E2DIR/usr/local/share
ln -s /var/usr/local/share/enigma2
ln -s /var/usr/local/share/keymaps
#mkdir -p ./var/usr/lib/enigma2/python
#rm $E2DIR/var/usr/lib/enigma2/python/Plugins
mkdir -p $VARDIR/usr/lib/enigma2/python
mv $E2DIR/usr/lib/enigma2/python/* $VARDIR/usr/lib/enigma2/python/
rm -rf $E2DIR/usr/lib/enigma2/python
cd $E2DIR/usr/lib/enigma2
ln -s /var/usr/lib/enigma2/python
#ln -s /var/usr/lib/enigma2/python/Plugins ./usr/lib/enigma2/python/Plugins
echo "var move done"
cd $VARDIR/usr/local/share/enigma2/po
mv de en no ru ..
rm -rf *
#rm -rf ../en 
mv ../de .
mv ../en .
mv ../no .
mv ../ru .
cd $E2DIR
mv $E2DIR/usr/share/zoneinfo/* $BACKUP
cp $BACKUP/CET $E2DIR/usr/share/zoneinfo
#cp $BACKUP/GMT $E2DIR/usr/share/zoneinfo
cp $BACKUP/zone.tab $E2DIR/usr/share/zoneinfo
#cp $BACKUP/Berlin $E2DIR/usr/share/zoneinfo/Europe
echo "Language cleanup done"
cd $E2DIR
mv root $E2DIR/var
rm $E2DIR/usr/share/fonts/ae_AlMateen.ttf
rm $E2DIR/epg/epg.dat
rm $E2DIR/usr/share/fonts/md_khmurabi_10.ttf
rm $E2DIR/usr/share/fonts/nmsbd.ttf
#rm -rf $VARDIR/*
mv $E2DIR/var/* ../.dat3p1ufs922
#mv $E2DIR/hdd ../.dat
rm $VARDIR/usr/local/share/enigma2/*.radio_org
rm $VARDIR/usr/local/share/enigma2/*.tv_org
rm $VARDIR/usr/local/share/enigma2/userbouquet.dbe0*.tv
cd $VARDIR/usr/local/share/enigma2/countries
mv de.png $BACKUP
mv en.png $BACKUP
mv no.png $BACKUP
mv ru.png $BACKUP
rm $VARDIR/usr/local/share/enigma2/countries/*
mv $BACKUP/de.png $VARDIR/usr/local/share/enigma2/countries
mv $BACKUP/en.png $VARDIR/usr/local/share/enigma2/countries
mv $BACKUP/no.png $VARDIR/usr/local/share/enigma2/countries
mv $BACKUP/ru.png $VARDIR/usr/local/share/enigma2/countries
mkdir $VARDIR/usr/share
mv $E2DIR/usr/share/fonts $VARDIR/usr/share/fonts
cd $E2DIR/usr/share
ln -s /var/usr/share/fonts
cd $E2DIR
rm -rf $E2DIR/usr/share/alsa
rm -rf $VARDIR/usr/lib/enigma2/python/Plugins/Extensions/DVDBurn >/dev/null 2>&1
rm -rf $VARDIR/usr/lib/enigma2/python/Plugins/Extensions/AC3L* >/dev/null 2>&1
rm -rf $VARDIR/usr/lib/enigma2/python/Plugins/Extensions/autores* >/dev/null 2>&1
rm -rf $VARDIR/usr/lib/enigma2/python/Plugins/Extensions/WAKEUP* >/dev/null 2>&1
rm -rf $VARDIR/usr/lib/enigma2/python/Plugins/Extensions/MediaS* >/dev/null 2>&1
rm -rf $VARDIR/usr/lib/enigma2/python/Plugins/Extensions/FileM* >/dev/null 2>&1
rm -rf $VARDIR/usr/lib/enigma2/python/Plugins/Extensions/CutL* >/dev/null 2>&1
rm -rf $VARDIR/usr/lib/enigma2/python/Plugins/SystemPlugins/Pos* >/dev/null 2>&1
rm -rf $VARDIR/usr/lib/enigma2/python/Plugins/SystemPlugins/Config* >/dev/null 2>&1
rm -rf $VARDIR/usr/lib/enigma2/python/Plugins/SystemPlugins/Diseqc* >/dev/null 2>&1
rm -rf $VARDIR/usr/lib/enigma2/python/Plugins/SystemPlugins/NFI* >/dev/null 2>&1
rm -rf $VARDIR/usr/lib/enigma2/python/Plugins/SystemPlugins/Softw* >/dev/null 2>&1
rm -rf $VARDIR/usr/lib/enigma2/python/Plugins/Demo* >/dev/null 2>&1
rm -rf $VARDIR/usr/lib/enigma2/python/Plugins/SystemPlugins/Default* >/dev/null 2>&1
rm -rf $VARDIR/usr/lib/enigma2/python/Plugins/SystemPlugins/Diseq* >/dev/null 2>&1
rm -rf $VARDIR/usr/lib/enigma2/python/Plugins/SystemPlugins/Front* >/dev/null 2>&1
rm -rf $VARDIR/usr/lib/enigma2/python/Plugins/SystemPlugins/Tuxbox* >/dev/null 2>&1
cd $E2DIR
mv $E2DIR/var/* $VARDIR/
rm $VARDIR/keys/eep120.bin >/dev/null 2>&1
rm $VARDIR/keys/rom120.bin >/dev/null 2>&1
rm $VARDIR/keys/AutoRoll.Key >/dev/null 2>&1
rm $VARDIR/keys/Autoupdate.Key >/dev/null 2>&1
rm $VARDIR/emu/kbox >/dev/null 2>&1
ln -s /var/root
rm -rf $VARDIR/usr/local/share/enigma2/defaults/Dream
rm $E2DIR/sbin/fsck.ext2
rm $E2DIR/sbin/fsck.ext3
rm $E2DIR/sbin/fsck.nfs
rm $E2DIR/sbin/MAKEDEV
rm $E2DIR/bin/splash_util.static
rm $E2DIR/sbin/mkfs.ext2
rm $E2DIR/sbin/mkfs.ext3
cd $E2DIR/sbin
ln -s mke2fs mkfs.ext2
ln -s mke2fs mkfs.ext3
cd $E2DIR
rm $E2DIR/sbin/sfdisk
#rm $E2DIR/bin/tar
rm $E2DIR/bin/pidof
#rm $E2DIR/usr/lib/python2.6/lib-dynload/unicodedata.so
#rm $E2DIR/usr/lib/python2.6/lib-dynload/_ctypes.so
#rm $E2DIR/usr/lib/python2.6/lib-dynload/cPickle.so
#rm $E2DIR/usr/lib/python2.6/lib-dynload/_ssl.so
#rm $E2DIR/usr/lib/python2.6/lib-dynload/_hotshot.so
#rm $E2DIR/usr/lib/python2.6/lib-dynload/_csv.so
#rm $E2DIR/usr/lib/python2.6/lib-dynload/_json.so
#rm $E2DIR/usr/lib/python2.6/lib-dynload/_multiprocessing.so
rm $VARDIR/usr/local/share/enigma2/keymap.xml.own >/dev/null 2>&1
rm $VARDIR/usr/local/share/enigma2/keymap.xml~ >/dev/null 2>&1

if [ -e $VARDIR/usr/local/share/enigma2/mnt ]; then
   rm $VARDIR/usr/local/share/enigma2/mnt
fi

cd $E2DIR/bin
#ln -s ../../bin/busybox tar
ln -s ../../bin/busybox pidof
mv $E2DIR/boot/bootlogo.mvi.orig $E2DIR/boot/bootlogo.mvi.orig.bu
cd $E2DIR
cp /home/juppi/newsvn/mt_build/stock/prev.png $VARDIR/usr/local/share/enigma2/
find . -name '*.txt' | xargs rm -rf >/dev/null 2>&1
find . -name '*.c' | xargs rm -rf >/dev/null 2>&1
find . -name '*.h' | xargs rm -rf >/dev/null 2>&1
find . -name '*-info' | xargs rm -rf >/dev/null 2>&1
find . -name '*.doc' | xargs rm -rf >/dev/null 2>&1
find . -name '*marmalade.pyc' | xargs rm -rf >/dev/null 2>&1
find . -name '*cfsupport' | xargs rm -rf >/dev/null 2>&1
find . -name '*TODO' | xargs rm -rf >/dev/null 2>&1
find . -name '*README' | xargs rm -rf >/dev/null 2>&1
find . -name '*LICENSE' | xargs rm -rf >/dev/null 2>&1
find . -name '*iocpreactor' | xargs rm -rf >/dev/null 2>&1
find . -name '*test' | xargs rm -rf >/dev/null 2>&1
find . -name '*tests' | xargs rm -rf >/dev/null 2>&1
find . -name '*journal' | xargs rm -rf >/dev/null 2>&1
find . -name '*gps' | xargs rm -rf >/dev/null 2>&1
find . -name '*mice' | xargs rm -rf >/dev/null 2>&1
find . -name '*zsh' | xargs rm -rf >/dev/null 2>&1
find . -name '*~' | xargs rm -rf >/dev/null 2>&1
find . -name '*.own' | xargs rm -rf >/dev/null 2>&1
find . -name '*.py~' | xargs rm -rf >/dev/null 2>&1
find . -name '.*~' | xargs rm -rf >/dev/null 2>&1
find . -name '*.old' | xargs rm -rf >/dev/null 2>&1
find . -name '*.old~' | xargs rm -rf >/dev/null 2>&1
find . -name '*.ok' | xargs rm -rf >/dev/null 2>&1
find . -name '*.ok~' | xargs rm -rf >/dev/null 2>&1
find . -name '*.info' | xargs rm -rf >/dev/null 2>&1
find . -name '.nfs*' | xargs rm -rf >/dev/null 2>&1
find . -name 'ui' | xargs rm -rf >/dev/null 2>&1
find . -name '.nfs*' | xargs rm -rf >/dev/null 2>&1
find . -name '*.off' | xargs rm -rf >/dev/null 2>&1
find . -name '*.orig' | xargs rm -rf >/dev/null 2>&1
find . -name 'dd.sh' | xargs rm -rf >/dev/null 2>&1
find . -name '*ntfs-3g*' | xargs rm -rf >/dev/null 2>&1
cd $VARDIR
find . -name '*.txt' | xargs rm -rf >/dev/null 2>&1
find . -name '*.c' | xargs rm -rf >/dev/null 2>&1
find . -name '*.h' | xargs rm -rf >/dev/null 2>&1
find . -name '*-info' | xargs rm -rf >/dev/null 2>&1
find . -name '*.doc' | xargs rm -rf >/dev/null 2>&1
find . -name '*marmalade.pyc' | xargs rm -rf >/dev/null 2>&1
find . -name '*cfsupport' | xargs rm -rf >/dev/null 2>&1
find . -name '*TODO' | xargs rm -rf >/dev/null 2>&1
find . -name '*README' | xargs rm -rf >/dev/null 2>&1
find . -name '*LICENSE' | xargs rm -rf >/dev/null 2>&1
find . -name '*iocpreactor' | xargs rm -rf >/dev/null 2>&1
find . -name '*test' | xargs rm -rf >/dev/null 2>&1
find . -name '*tests' | xargs rm -rf >/dev/null 2>&1
find . -name '*journal' | xargs rm -rf >/dev/null 2>&1
find . -name '*gps' | xargs rm -rf >/dev/null 2>&1
find . -name '*mice' | xargs rm -rf >/dev/null 2>&1
find . -name '*zsh' | xargs rm -rf >/dev/null 2>&1
find . -name '*~' | xargs rm -rf >/dev/null 2>&1
find . -name '*.own' | xargs rm -rf >/dev/null 2>&1
find . -name '*.py~' | xargs rm -rf >/dev/null 2>&1
find . -name '.*~' | xargs rm -rf >/dev/null 2>&1
find . -name '*.old' | xargs rm -rf >/dev/null 2>&1
find . -name '*.old~' | xargs rm -rf >/dev/null 2>&1
find . -name '*.ok' | xargs rm -rf >/dev/null 2>&1
find . -name '*.ok~' | xargs rm -rf >/dev/null 2>&1
find . -name '*.info' | xargs rm -rf >/dev/null 2>&1
find . -name '.nfs*' | xargs rm -rf >/dev/null 2>&1
find . -name 'ui' | xargs rm -rf >/dev/null 2>&1
find . -name '.nfs*' | xargs rm -rf >/dev/null 2>&1
find . -name '*.off' | xargs rm -rf >/dev/null 2>&1
find . -name '*.orig' | xargs rm -rf >/dev/null 2>&1
find . -name 'dd.sh' | xargs rm -rf >/dev/null 2>&1
find . -name '*ntfs-3g*' | xargs rm -rf >/dev/null 2>&1
mv $E2DIR/boot/bootlogo.mvi.orig.bu $E2DIR/boot/bootlogo.mvi.orig
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
rm -rf $VARDIR/usr/local/share/enigma2/satellites.xml
mv $VARDIR/usr/local/share/enigma2/extended_satellites.xml $E2DIR/usr/local/share/
cd $E2DIR
#rm -rf $pythonpath/lib-dynload/_testcapi.so >/dev/null 2>&1
#rm -rf $pythonpath/lib-dynload/cmath.so >/dev/null 2>&1
#rm -rf $pythonpath/lib-dynload/mmap.so >/dev/null 2>&1
#rm -rf $pythonpath/lib-dynload/strop.so >/dev/null 2>&1
#rm -rf $pythonpath/site-packages/twisted/internet/_dumbwin32proc.py >/dev/null 2>&1
#rm -rf $pythonpath/site-packages/twisted/internet/_win32serialport.py >/dev/null 2>&1
#rm -rf $pythonpath/site-packages/twisted/internet/_win32stdio.py >/dev/null 2>&1
#rm -rf $pythonpath/site-packages/twisted/internet/test >/dev/null 2>&1
#rm -rf $pythonpath/site-packages/twisted/python/_epoll.so >/dev/null 2>&1
#rm -rf $pythonpath/site-packages/twisted/python/test >/dev/null 2>&1
#rm -rf $pythonpath/site-packages/twisted/web2/test >/dev/null 2>&1
#rm -rf $pythonpath/site-packages/zope/interface/DEPENDENCIES.cfg >/dev/null 2>&1
#rm -rf $pythonpath/site-packages/zope/interface/PUBLICATION.cfg >/dev/null 2>&1
#rm -rf $pythonpath/site-packages/zope/interface/SETUP.cfg >/dev/null 2>&1
#rm -rf $pythonpath/site-packages/zope/interface/common >/dev/null 2>&1
#rm -rf $pythonpath/site-packages/zope/interface/tests >/dev/null 2>&1
#rm -rf $VARDIR/usr/local/share/enigma2/wait1.png >/dev/null 2>&1
#rm -rf $VARDIR/usr/local/share/enigma2/wait2.png >/dev/null 2>&1
#rm -rf $VARDIR/usr/local/share/enigma2/wait3.png >/dev/null 2>&1
#rm -rf $VARDIR/usr/local/share/enigma2/wait4.png >/dev/null 2>&1
mv $VARDIR/usr/local/share/enigma2/skin_default $E2DIR/usr/local/share/
mv $VARDIR/usr/local/share/enigma2/picon $E2DIR/usr/local/share/
mv $VARDIR/usr/local/share/enigma2/skin_default.xml $E2DIR/usr/local/share/
rm $VARDIR/usr/local/share/enigma2/skin.xml
#rm $VARDIR/usr/local/share/enigma2/skin.xml $E2DIR/usr/local/share/
mv $VARDIR/usr/local/share/enigma2/skin_birdie.xml $E2DIR/usr/local/share/
mv $VARDIR/usr/local/share/enigma2/skin_normal.xml $E2DIR/usr/local/share/
cd $VARDIR/usr/local/share/enigma2
ln -s /usr/local/share/skin_default
ln -s /usr/local/share/skin_default.xml
#ln -s /usr/local/share/skin.xml
cd $E2DIR/usr/local/share/skin_default
rm $E2DIR/usr/local/share/skin_default/spinner
ln -s /var/usr/local/share/enigma2/spinner
cd $E2DIR
mv $VARDIR/etc/swapsize.bu $VARDIR/etc/swapsize.old
touch $VARDIR/etc/.orig
mv $VARDIR/usr/local/share/enigma2/EVO-blackGlass-HD $E2DIR/usr/local/share/
listing=`ls $E2DIR/usr/local/share/EVO-blackGlass-HD`
mkdir $VARDIR/usr/local/share/enigma2/EVO-blackGlass-HD
cd $VARDIR/usr/local/share/enigma2/EVO-blackGlass-HD
for p in $listing
 do
   ln -s /usr/local/share/EVO-blackGlass-HD/$p
 done

echo "Cleanup done"
#flashimage nochmals packen um from nfs zu booten
cp -rp $VARDIR/* $E2DIR/var/
#cp -rp $USRDIR/* $E2DIR/usr/lib/enigma2/python/
cp $ARCHIVEPATH/boot/uImage $E2DIR/boot/
cd $E2DIR
sudo tar -czvf /devel/E2/922/922_rev$newarchiv.flash.tar.gz ./ >/dev/null 2>&1
if [ ! -e /devel/E2/922/922_rev$newarchiv.flash ]; then
   mkdir /devel/E2/922/922_rev$newarchiv.flash
fi
if [ -e /devel/E2/922/922_rev$newarchiv.flash ]; then
   sudo rm -rf /devel/E2/922/922_rev$newarchiv.flash/*
fi
cp /devel/E2/922/922_rev$newarchiv.flash.tar.gz /devel/E2/922/922_rev$newarchiv.flash
cd /devel/E2/922/922_rev$newarchiv.flash
sudo tar -xzvf 922_rev$newarchiv.flash.tar.gz >/dev/null 2>&1
sudo chmod 777 -R /devel/E2/922/922_rev$newarchiv.flash
rm -rf $E2DIR/var/*
rm -rf $E2DIR/boot/uImage

#cutecom teil 2
cutecom
echo "weiter gehts..."
#cd /devel/E2/922/922_rev$newarchiv.ufs922.flash
#sudo tar -czvf /devel/E2/922/922_rev$newarchiv.ufs922.flash.tar.gz ./ >/dev/null 2>&1
#sudo rm -rf /devel/E2/922/922_rev_nfs/*
#cp /devel/E2/922/922_rev$newarchiv.ufs922.flash.tar.gz /devel/E2/922/922_rev_nfs/ 
#cd /devel/E2/922/922_rev_nfs
#sudo tar -xzvf 922_rev$newarchiv.ufs922.flash.tar.gz >/dev/null 2>&1
#sudo chmod 777 -R /devel/E2/922/922_rev_nfs
#rm /devel/E2/922/922_rev_nfs/922_rev$newarchiv.ufs922.flash.tar.gz
rm $VARDIR//usr/local/share/enigma2/wait*.png
echo "MAKE OWN"
#lircd
mkdir $VARDIR/usr/bin
if [ -e $E2DIR/usr/bin/lircd ]; then
rm $E2DIR/usr/bin/lircd
fi
if [ -e /etc/lircd.conf ]; then
rm $VARDIR/etc/lircd.conf
fi
if [ -e /etc/lircd.conf ]; then
rm $E2DIR/etc/lircd.conf
fi
#cd $E2DIR
#   mkdir $VARDIR/saver
#   mkdir $E2DIR/saver
#   mv $VARDIR/saver/* $E2DIR/saver/
#   mv $E2DIR/saver/saver1_kids.mvi.off.bu $E2DIR/saver/saver1_kids.mvi.off
#  mv $E2DIR/saver/saver2_kids.mvi.off.bu $E2DIR/saver/saver2_kids.mvi.off
#   listing=`ls $E2DIR/saver`
#   cd $VARDIR/saver
#   for m in $listing
#    do
#      ln -s /saver/$m
#  done
mv $E2DIR/usr/bin/tuxtxt $VARDIR/usr/bin/
cd $E2DIR/usr/bin
ln -s /var/usr/bin/tuxtxt
mv $E2DIR/lib/libipkg.so.0 $VARDIR/lib/
cd $E2DIR/lib
ln -s /var/lib/libipkg.so.0
cp $makeimagepath/stock/allerlei $VARDIR/etc/init.d/
cd $E2DIR
touch $VARDIR/etc/.evofirstboot
cp $makeimagepath/stock/rcS.ufs922 $E2DIR/etc/init.d/rcS
cp $makeimagepath/stock/EVO_rcS.ufs922 $VARDIR/etc/init.d/EVO_rcS
echo "config.Nims.0.diseqcMode=single" > $VARDIR/usr/local/share/enigma2/settings
echo "config.Nims.0.configMode=simple" >> $VARDIR/usr/local/share/enigma2/settings
cp $dev $E2DIR/dev/
cd $E2DIR/dev
sudo tar -xzvf dev922.tar.gz
sudo chmod 777 -R $E2DIR/dev
sudo rm $E2DIR/dev/dev922.tar.gz
cd $E2DIR
ln -s /var/log
#final flashimage bauen
#   sed s"@^\\(\\s*\\)#mount -t jffs2 -o noatime /dev/mtdblock4 /var >/dev/null 2>&1\$@\\mount -t jffs2 -o noatime /dev/mtdblock4 /var >/dev/null 2>&1@" -i $E2DIR/etc/init.d/rcS
#   sed s"@^\\(\\s*\\)#mount -t jffs2 -o noatime /dev/mtdblock3 /usr/lib/enigma2/python >/dev/null 2>&1\$@\\mount -t jffs2 -o noatime /dev/mtdblock3 /usr/lib/enigma2/python >/dev/null 2>&1@" -i $E2DIR/etc/init.d/rcS
if [ ! -e $makeimagepath/tufsbox/cdkflash ]; then
mkdir $makeimagepath/tufsbox/cdkflash
fi
   rm -rf $makeimagepath/tufsbox/cdkflash/mtd*.ubimage* >/dev/null 2>&1
   rm -rf $makeimagepath/tufsbox/cdkflash/mtd*.enigma2* >/dev/null 2>&1
   rm -rf $makeimagepath/tufsbox/cdkflash/var-stock-enigma2* >/dev/null 2>&1
   rm -rf $makeimagepath/tufsbox/cdkflash/usr-stock-enigma2* >/dev/null 2>&1
   rm -rf $makeimagepath/tufsbox/cdkflash/*.update* >/dev/null 2>&1
   cd $makeimagepath/cvs/cdk
   make clean
   ./make.sh 3p1ufs922
make flash-stock-enigma23p1ufs922-full
   sudo chmod 777 -R $makeimagepath/tufsbox/cdkflash

   if [ ! -e $makeimagepath/tufsbox/cdkflash/own ]; then
      mkdir $makeimagepath/tufsbox/cdkflash/own
   fi

      rm -rf $makeimagepath/tufsbox/cdkflash/own/mtd*.ubimage* >/dev/null 2>&1
      rm -rf $makeimagepath/tufsbox/cdkflash/own/mtd*.enigma2* >/dev/null 2>&1
      rm -rf $makeimagepath/tufsbox/cdkflash/own/var-stock-enigma2* >/dev/null 2>&1
      rm -rf $makeimagepath/tufsbox/cdkflash/own/*.update* >/dev/null 2>&1
      rm -rf $makeimagepath/tufsbox/cdkflash/*.update* >/dev/null 2>&1
      rm -rf $makeimagepath/tufsbox/cdkflash/var-stock-enigma2* >/dev/null 2>&1

   mv $makeimagepath/tufsbox/cdkflash/mtdblock1*.ubimage* $makeimagepath/tufsbox/cdkflash/own/
   mv $makeimagepath/tufsbox/cdkflash/mtdblock2*.enigma2* $makeimagepath/tufsbox/cdkflash/own/
   mv $makeimagepath/tufsbox/cdkflash/mtdblock3*.enigma2* $makeimagepath/tufsbox/cdkflash/own/
   mv $makeimagepath/tufsbox/cdkflash/stock*.img* $makeimagepath/tufsbox/cdkflash/own/
   rm -rf $makeimagepath/tufsbox/cdkflash/own/*.update* >/dev/null 2>&1


#fi
cd $E2DIR
#$SCRIPTPATH/makepublic.sh &
exit


