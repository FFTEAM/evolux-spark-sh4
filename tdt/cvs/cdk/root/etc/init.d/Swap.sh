#!/bin/sh
#swappart
swapoff -a
alwaysthere=`cat /etc/fstab | grep swap | tail -n1 | cut -d p -f2 | cut -d s -f2`
loopswap=`cat /proc/swaps | cut -b10`
if [ "$alwaysthere" = w ]; then
   sed -i '$d' /etc/fstab
fi
if [ -e /etc/.start_enigma2 ]; then
	isactive=`cat /etc/enigma2/settings | grep config.plugins.PinkPanel.SwapArt= | cut -d = -f2`
else
	if [ -e /etc/.swappart ]; then
		isactive=swappart
	elif [ -e /etc/.swapfile ]; then
		isactive=swapfile
	else
		isactive=swapram
	fi
fi
if [ "$isactive" = swappart ] || [ "$isactive" = swapfile ] || [ "$isactive" = swapram ] || [ -z $isactive ]; then
	if [ "$isactive" = swappart ]; then
		swappart=`fdisk -l | grep swap | cut -d / -f3 | cut -b1-4`
		echo -e -n "\n/dev/$swappart     none                swap    sw\n" >> /etc/fstab
		swapon -a
		echo "SWAPPART ON"
		echo "SWP" > /dev/vfd
	fi
	if [ "$isactive" = swapram ] || [ -z $isactive ]; then
#		if [ -e /lib/modules/lzo1x_decompress.ko ]; then
			alwaysthere=`lsmod | grep -m2 ramzswap0`
			if [ -z $alwaysthere ]; then
				#insmod /lib/modules/lzo1x_compress.ko
				#insmod /lib/modules/lzo1x_decompress.ko
				insmod /lib/modules/ramzswap.ko disksize_kb=32768
				mknod /dev/ramzswap0 b 253 0 >/dev/null 2>&1
				echo "SWAPRAM ON"
				echo "SWR" > /dev/vfd
				loopswap=`cat /proc/swaps | grep loop -m1 | cut -d " " -f1`
				if [ ! -z $loopswap ];then
					swapoff -a
					swapoff $loopswap
					swapon /dev/ramzswap0
				else
					(swapoff -a;swapon /dev/ramzswap0)
				fi 
			fi
#		fi
	fi
	if [ "$isactive" = swapfile ]; then
		. /etc/init.d/swapsize.old
		size=`cat /etc/enigma2/settings | grep config.plugins.PinkPanel.SwapFileSize= | cut -d = -f2`
		if [ -z "$size" ]; then
			size=16384
		fi
		isHDD=`mount | grep sda`
		if [ ! -z "$isHDD" ]; then
			if [ ! -e /media/hdd/swap ]; then
				mkdir -p /media/hdd/swap
			fi
			if [ ! -e /media/hdd/swap/swapfile ] && [ "$isactive" = swapfile ]; then
				echo "SWAP create $(($size / 1000)) MB swapfile"
				echo "CRS" > /dev/vfd
				loopswapramold=`cat /proc/swaps | grep ram -m1 | cut -d " " -f1`
				loopswapold=`cat /proc/swaps | grep loop -m1 | cut -d " " -f1`
				swapoff -a
				if [ ! -z $loopswapramold ]; then
					swapoff $loopswapramold
				fi
				if [ ! -z $loopswapold ]; then
					swapoff $loopswapold
				fi
				swapoff /dev/loop0
				dd if=/dev/zero of=/media/hdd/swap/swapfile bs=1024 count=$size
				losetup /dev/loop0 /media/hdd/swap/swapfile
				mkswap /dev/loop0
				swapon /dev/loop0
				sync
			elif [ "$swapsize" != "$size" ] && [ "$isactive" = swapfile ]; then
				echo "SWAP create NEW $(($size / 1000)) MB swapfile"
				echo "CRS" > /dev/vfd
				loopswap=`cat /proc/swaps | cut -b10`
				swapoff -a
				rm /media/hdd/swap/swapfile
				dd if=/dev/zero of=/media/hdd/swap/swapfile bs=1024 count=$size
				echo "swapsize=$size" > /etc/init.d/swapsize.old
				loopswapnew=$(($loopswap + 1))
				losetup /dev/loop$loopswapnew /media/hdd/swap/swapfile
				mkswap /dev/loop$loopswapnew
				loopswapold=`cat /proc/swaps | grep loop -m1 | cut -d " " -f1`
				loopswapramold=`cat /proc/swaps | grep ram -m1 | cut -d " " -f1`
				swapoff $loopswapold
				if [ ! -z $loopswapramold ]; then
					swapoff $loopswapramold
				fi
				swapon /dev/loop$loopswapnew
				sync
			elif [ -e /media/hdd/swap/swapfile ] && [ "$swapsize" = "$size" ] && [ "$isactive" = swapfile ]; then
				loopswap=`cat /proc/swaps | grep loop -m1 | cut -d " " -f1`
				echo "ACTIVATING SWAPFILE"
				echo "SWF" > /dev/vfd
				if [ ! -z $loopswap ]; then
					swapoff -a
					swapoff $loopswap
					losetup $loopswap /media/hdd/swap/swapfile
					mkswap $loopswap
					swapon $loopswap
				else
					swapoff -a
					swapoff /dev/loop0
					losetup /dev/loop0 /media/hdd/swap/swapfile
					mkswap /dev/loop0
					swapon /dev/loop0
				fi
			fi
		fi
	fi
	swapon -a
fi
exit
