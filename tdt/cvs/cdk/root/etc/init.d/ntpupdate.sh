#!/bin/sh
# made iscan

echo 'Start ntpdate'
if [ -f /sbin/ntpdate ] ; then
    if [ -f /etc/ntpserver ] ; then
	ntpServerAdr=`more /etc/ntpserver`
    else
	ntpServerAdr="dk.pool.ntp.org"
    fi
    /sbin/ntpdate $ntpServerAdr
    echo "done."
else
    echo "not done."
    echo "\n ntpdate software is missing."
fi


