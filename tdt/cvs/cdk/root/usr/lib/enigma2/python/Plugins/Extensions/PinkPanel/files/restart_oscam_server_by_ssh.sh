#!/bin/sh
# generic paths ##################################################################
MYIP=`cat /etc/enigma2/settings | grep config.plugins.PinkPanel.serverip | cut -d = -f2`
MYRESTARTSCRIPTPATH="/var/media/ftp/uStor01/addons/oscam/oscam_restart.sh"
##################################################################################
# do_it ##########################################################################
echo "Restart Server(Fritzbox)-Oscam now..."
ssh -y -i /etc/dropbear/dropbear_dss_host_key root@$MYIP "$MYRESTARTSCRIPTPATH"
echo "Server(Fritzbox)-Oscam restartet!"
##################################################################################
exit

