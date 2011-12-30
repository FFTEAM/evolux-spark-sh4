#!/bin/bash
CURDIR=`pwd`
echo "deb http://old-releases.ubuntu.com/ubuntu/ jaunty main restricted" > /etc/apt/sources.list
echo "deb-src http://old-releases.ubuntu.com/ubuntu/ jaunty main" > /etc/apt/sources.list
sudo apt-get update
sudo apt-get remove rpm
sudo apt-get -y install rpm=4.4.2.3-2ubuntu1
echo "please lock new/old rpm version now by packetmanger!!!"
sleep 5
sudo ln -sf /bin/bash /bin/sh
#	cd /usr/lib32
#	sudo ln -sf n -s libz.so.1.2.3.4 libz.so
#	touch "$CURDIR/.ubu1010"
apt-get -y install subversion \
git-core \
ccache \
automake \
libncurses5-dev \
flex \
bison \
gawk \
texinfo \
gettext \
cfv \
fakeroot \
xfslibs-dev \
zlib1g-dev \
libtool \
g++ \
gcc-multilib \
swig \
pkg-config \
ndisgtk \
git-core \
mtd-utils \
squashfs-tools \
help2man \
diffstat \
texi2html \
bitbake \
monotone \
rar \
unrar \
ffmpeg \
sysfsutils \
libdvdcss2 \
libssl-dev \
libbz2-dev \
libgtk2.0-dev \
padevchooser \
ubuntu-restricted-extras \
jam \
libgif-dev \
imagemagick \
optipng \
pngquant \
gsynaptics \
ghex \
gparted \
ntfsprogs \
libmpc-dev \
libgmp3-dev \
libmpfr-dev \
g++-multilib \
g++-4.2-multilib \
lib32z1-dev \
autopoint \
ntfs-config \
lib32z1-dev

exit

