#!/bin/sh
MODDIR=/lib/modules
if [ ! -e /etc/.startenigma2 ] && [ -e /etc/.wlan ]; then
	isWlan=true
	. /usr/local/share/config/wlan.conf
elif [ ! -e /etc/.startenigma2 ] && [ ! -e /etc/.wlan ]; then
	isWlan=""
else
	isWlan=`grep -m1 config.plugins.PinkPanel.WlanOn /etc/enigma2/settings | cut -d = -f2`
	myWlanEssid=`grep -m1 config.plugins.PinkPanel.WlanEssid /etc/enigma2/settings | cut -d = -f2`
	myWlanAuthmode=`grep -m1 config.plugins.PinkPanel.WlanAuthMode /etc/enigma2/settings | cut -d = -f2`
	myWlanKey=`grep -m1 config.plugins.PinkPanel.WlanPSK /etc/enigma2/settings | cut -d = -f2`
	haveWlanRTL=`grep -m1 config.plugins.PinkPanel.WlanisRTL /etc/enigma2/settings | cut -d = -f2`
	myWlanRTL=`grep -m1 config.plugins.PinkPanel.WlanRTLmode /etc/enigma2/settings | cut -d = -f2`
	isDHCPoff=`grep -m1 config.plugins.PinkPanel.WlanDHCP /etc/enigma2/settings | cut -d = -f2`
	myIpaddr=`grep -m1 config.plugins.PinkPanel.Wlanip /etc/enigma2/settings | cut -d = -f2`
fi
#startETH() {
	if [ -e /etc/network/interfaces ]; then
		ip addr flush eth0
		/etc/init.d/networking stop
		if [ -z $isWlan ]; then
			DHCP=`grep -c dhcp /etc/network/interfaces`
			if [ "$DHCP" -eq "1" ]; then
				ifdown ra0 
				ifdown wlan0
#				ifconfig eth0 up
				echo "Starting ETH0 dhcp"
				/etc/init.d/udhcpc start
			fi
		else
			if [ ! -z "$haveWlanRTL" ]; then
				if [ -z "$myWlanRTL" ]; then
					insmod $MODDIR/rt3070sta.ko
				else
					insmod $MODDIR/rt2870sta.ko
				fi
				myIface=ra0
			else
				myIface=wlan0
			fi
			ifconfig $myIface up
			iwconfig $myIface mode managed
			iwconfig $myIface essid "$myWlanEssid"
			if [ -z "$myWlanAuthmode" ]; then
				mode=WPA2PSK
				etype=AES
			else
				mode=WPAPSK
				etype=TKIP
			fi
			iwpriv $myIface set AuthMode=$mode
			iwpriv $myIface set WPAPSK="$myWlanKey"
			iwpriv $myIface set EncrypType=$etype
			echo "Starting WLAN ..."
			if [ -z "$isDHCPoff" ]; then
				udhcpc -i $myIface
			else
				udhcpc -i $myIface -r "$myIpaddr"
			fi
			iwconfig $myIface
			(sleep 5; ifdown eth0) &
		fi
		/etc/init.d/networking start
	fi
#}
