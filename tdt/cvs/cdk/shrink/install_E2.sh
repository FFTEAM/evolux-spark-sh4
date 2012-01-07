#!/bin/sh
install=/mnt/hdd0/media
filename=NP922_rev1091public.tar.gz

cd $install
echo " "
echo "extracting new-kf-ufs922-e2 on sda5 now..."
echo " "
echo "-------------------------------------"
if [ -e $install/$filename ]; then
	tar -xzvf $filename
	sync
#        chmod 777 -R $install
#        chmod 640 -R $install/dev
	mkdir $install/hdd2
	mount /dev/sda3 $install/hdd2
	echo " "
	echo "copy kernel to sda3 now..."
	echo " "
	echo "-------------------------------------"
	cp $install/boot/uImage $install/hdd2
	sync
	umount $install/hdd2
	rm -rf $install/hdd2
	echo " "
	IP=`cat $install/etc/network/interfaces | grep address | cut -b10-24`
	GW=`cat $install/etc/network/interfaces | grep gateway | cut -b10-24`
	NM=`cat $install/etc/network/interfaces | grep netmask | cut -b10-24`
	cp $install/etc/fw_env.config /etc
	sleep 2
	$install/bin/fw_setenv ipaddr $IP
	sleep 2
	$install/bin/fw_setenv gateway $GW
	sleep 2
	$install/bin/fw_setenv serverip $GW
	sleep 2
	$install/bin/fw_setenv netmask $NM
	sleep 2
	$install/bin/fw_setenv bootargs console=ttyAS0,115200 root=/dev/sda5 rw ip=$IP:$GW:$GW:$NM:kathrein:eth0:off mem=89m coprocessor_mem=4m@0x10000000,4m@0x10400000 rootdelay=4 init=/bin/devinit
	sleep 2
	$install/bin/fw_setenv boote2 'run bootargs;ide reset;ext2load ide 0:3 a5000000 uImage;bootm a5000000'
	sleep 2
	$install/bin/fw_setenv bootcmd 'run boote2'
	sleep 2
	$install/bin/fw_printenv
	echo " "
	echo "___________________________________________________"
	echo "REBOTING NOW AND E2 MIGHT BE AUTOMATICLY STARTUP..."
	echo "___________________________________________________"
	echo "FINISHED, REBOOT NOW..."
	sleep 5
	reboot 
	
else
	echo " "
	echo "-----------------------------------"
	echo "$filename not found!"
	echo "please copy $filename first via ftp to:"
	echo "/mnt/hdd0/media !!!"
	echo " "
	echo "-----------------------------------"
fi

echo " "
echo "FINISHED!"

exit
