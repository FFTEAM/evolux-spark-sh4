#!/bin/bash
CURDIR=`pwd`
if [ ! -e "$CURDIR/.ubu1010" ]; then
	echo "deb http://old-releases.ubuntu.com/ubuntu/ jaunty main restricted" > /etc/apt/sources.list
	echo "deb-src http://old-releases.ubuntu.com/ubuntu/ jaunty main" > /etc/apt/sources.list
	sudo apt-get update
	sudo apt-get remove rpm
	sudo apt-get -y install rpm=4.4.2.3-2ubuntu1
	echo "please lock new/old rpm verison now by packetmanger!!!"
	sleep 5
	sudo ln -sf /bin/bash /bin/sh
	cd /usr/lib32
	sudo ln -sf n -s libz.so.1.2.3.4 libz.so
	touch "$CURDIR/.ubu1010"
fi
exit

