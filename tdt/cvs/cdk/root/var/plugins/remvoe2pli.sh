#!/bin/sh
wget -q -O /dev/null "http://127.0.0.1/control/message?popup=E2PLI deleted!%20Please reboot in 30seconds.."
(rm -rf /etc/enigma2;rm -rf /usr/lib/enigma2;rm -rf /usr/lib/python2.6;rm -rf /usr/lib/gstreamer-0.10;rm -rf /usr/local/share/enigma2 &) &
exit

