#!/bin/sh
SCRIPTPATH=/devel/E2/shrink
newarchiv=`cat $SCRIPTPATH/rev.version`
cd /home/juppi/ufs910/mt_build/tufsbox/cdkflash/public
wput --reupload ftp://pinky@kathi-forum.de:Heimwerker37@kathif.vs120005.hl-users.com:21/EV/ ./EV-CAT-DD-3p1-E2-ufs910-$newarchiv-FLASH.rar
wput --reupload ftp://pinky@kathi-forum.de:Heimwerker37@kathif.vs120005.hl-users.com:21/EV/ ./EV-KCC-3p1-E2-ufs910-$newarchiv-FLASH.rar
wput --reupload ftp://pinky@kathi-forum.de:Heimwerker37@kathif.vs120005.hl-users.com:21/EV/ ./EV-FLASHSUITE-3p1-E2-ufs910-$newarchiv-FLASH.rar
#sudo rm -rf $ARCHIVEPATH.public/*
#sudo rm -rf /devel/E2/release$newarchiv.flash
exit

