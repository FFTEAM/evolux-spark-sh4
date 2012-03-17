/*
 * $Header: /cvsroot/tuxbox/apps/tuxbox/neutrino/src/system/configure_network.cpp,v 1.6 2003/03/26 17:53:12 thegoodguy Exp $
 *
 * (C) 2003 by thegoodguy <thegoodguy@berlios.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */
#include <config.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include "configure_network.h"
#include "libnet.h"             /* netGetNameserver, netSetNameserver   */
#include "network_interfaces.h" /* getInetAttributes, setInetAttributes */
#include <stdlib.h>             /* system                               */
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/wireless.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>

CNetworkConfig::CNetworkConfig(void)
{
	char our_nameserver[16];
	netGetNameserver(our_nameserver);
	nameserver = our_nameserver;
	interface = "eth0";

	struct ifaddrs *ifap = NULL;
	if (!getifaddrs(&ifap)) {
		// Get name of first active interface
		struct ifaddrs *it = ifap;
		while (it) {
			if ((it->ifa_addr->sa_family == AF_INET) && it->ifa_name && strcmp(it->ifa_name, "lo") && (it->ifa_flags & IFF_RUNNING)) {
				interface = std::string(it->ifa_name);
				break;
			}
			it = it->ifa_next;
		}
		freeifaddrs(ifap);
	}

	getInterfaceConfig();
}

void CNetworkConfig::getInterfaceConfig(bool copy)
{
	address = "";
	netmask = "";
	broadcast = "";
	gateway = "";
	inet_static = getInetAttributes(interface, automatic_start, address, netmask, broadcast, gateway);
	if (isWireless()) {
		std::string scriptName = "/etc/network/if-pre-up.d/wlan";
		std::ifstream F(scriptName.c_str());
		if(F.is_open()) {
			std::string line;
			std::string authmode = "WPA2PSK";
			while (std::getline(F, line)) {
				if (line.length() < 5)
					continue;
				if (!line.compare(0, 3, "E=\""))
					wlan_essid = line.substr(3, line.length() - 4);
				else if (!line.compare(0, 3, "A=\""))
					authmode = line.substr(3, line.length() - 4);
				else if (!line.compare(0, 3, "K=\""))
					wlan_key = line.substr(3, line.length() - 4);
			}
			F.close();
			if (authmode == "WPAPSK")
				wlan_mode = "WPA";
			else
				wlan_mode = "WPA2";
		}
	}
	if (copy)
		copy_to_orig();
}

void CNetworkConfig::getDHCPInterfaceConfig(void)
{
	if (!inet_static) {
		char our_ip[16];
		char our_mask[16];
		char our_broadcast[16];
		char our_gateway[16];
		char our_nameserver[16];

		netGetIP((char *) active_interface.c_str(), our_ip, our_mask, our_broadcast);
		netGetDefaultRoute(our_gateway);
		netGetNameserver(our_nameserver);
		address = std::string(our_ip);
		netmask = std::string(our_mask);
		broadcast = std::string(our_broadcast);
		gateway = std::string(our_gateway);
	}
}

void CNetworkConfig::copy_to_orig(void)
{
	orig_automatic_start = automatic_start;
	orig_address         = address;
	orig_netmask         = netmask;
	orig_broadcast       = broadcast;
	orig_gateway         = gateway;
	orig_inet_static     = inet_static;
	orig_interface       = interface;
	orig_wlan_essid      = wlan_essid;
	orig_wlan_key        = wlan_key;
	orig_wlan_mode       = wlan_mode;
}

void CNetworkConfig::copy_from_orig(void)
{
	automatic_start = orig_automatic_start;
	address         = orig_address;
	netmask         = orig_netmask;
	broadcast       = orig_broadcast;
	gateway         = orig_gateway;
	inet_static     = orig_inet_static;
	interface       = orig_interface;
	wlan_essid      = orig_wlan_essid;
	wlan_key        = orig_wlan_key;
	wlan_mode       = orig_wlan_mode;
}

bool CNetworkConfig::modified_from_orig(void)
{
	return (
		(orig_interface       != active_interface) ||
		(orig_automatic_start != automatic_start)  ||
		(orig_address         != address        )  ||
		(orig_netmask         != netmask        )  ||
		(orig_broadcast       != broadcast      )  ||
		(orig_gateway         != gateway        )  ||
		(orig_inet_static     != inet_static    )  ||
		(orig_interface       != interface      )  ||
		(orig_wlan_essid      != wlan_essid     )  ||
		(orig_wlan_key        != wlan_key       )  ||
		(orig_wlan_mode       != wlan_mode      )
	);
}

int CNetworkConfig::setWLAN(){
	std::ofstream F("/etc/network/if-pre-up.d/wlan");
	if(F.is_open()) {
		chmod("/etc/network/if-pre-up.d/wlan", 0755);
		std::string authmode = "WPA2PSK"; // WPA2
		std::string encryptype = "AES"; // WPA2
		if (wlan_mode == "WPA") {
			authmode = "WPAPSK";
			encryptype = "TKIP";
		}
		F << "#!/bin/sh\n"
                  << "# AUTOMATICALLY GENERATED. DO NOT MODIFY.\n"
		  << "grep $IFACE: /proc/net/wireless >/dev/null 2>&1 || exit 0\n"
		  << "kill -9 $(pidof wpa_supplicant 2>/dev/null) 2>/dev/null\n"
		  << "E=\"" << wlan_essid << "\"\n"
		  << "A=\"" << authmode << "\"\n"
		  << "C=\"" << encryptype << "\"\n"
		  << "K=\"" << wlan_key << "\"\n"
		  << "ifconfig $IFACE down\n"
		  << "ifconfig $IFACE up\n"
		  << "iwconfig $IFACE mode managed\n"
		  << "iwconfig $IFACE essid \"$E\"\n"
		  << "iwpriv $IFACE set AuthMode=$A\n"
		  << "iwpriv $IFACE set EncrypType=$C\n"
		  << "if ! iwpriv $IFACE set \"WPAPSK=$K\"\n"
		  << "then\n"
		  << "\t/usr/sbin/wpa_supplicant -B -i$IFACE -c/etc/wpa_supplicant.conf\n"
		  << "\tsleep 3\n"
		  << "fi\n";
		F.close();
		
		F.open("/etc/wpa_supplicant.conf");
		if(F.is_open()) {
			F << "ctrl_interface=/var/run/wpa_supplicant\n\n"
			  << "network={\n"
			  << "\tscan_ssid=1\n"
			  << "\tssid=\"" << wlan_essid << "\"\n"
			  << "\tkey_mgmt=WPA-PSK\n"
			  << "\tproto=" << wlan_mode << "\n"
			  << "\tpairwise=TKIP\n"
			  << "\tgroup=TKIP\n"
			  << "\tpsk=\"" << wlan_key << "\"\n"
			  << "}\n";
			F.close();
		}
		return 0;
	}
	return -1;
}

void CNetworkConfig::commitConfig(void)
{
	if (modified_from_orig())
	{
		copy_to_orig();

		struct ifaddrs *ifap = NULL;
		if (!getifaddrs(&ifap)) {
			struct ifaddrs *it = ifap;
			while (it) {
				if (it->ifa_name && strcmp(it->ifa_name, "lo") && strcmp(it->ifa_name, orig_interface.c_str())) {
					interface = std::string(it->ifa_name);
					getInterfaceConfig(false);
					if (inet_static)
						setStaticAttributes(interface, false, address, netmask, broadcast, gateway);
					else
						setDhcpAttributes(interface, false);
				}
				it = it->ifa_next;
			}
			freeifaddrs(ifap);
		}

		copy_from_orig();

		addLoopbackDevice("lo", true);

		if (inet_static)
			setStaticAttributes(interface, true, address, netmask, broadcast, gateway);
		else
			setDhcpAttributes(interface, true);

		active_interface = interface;

		if (isWireless())
			setWLAN();
	}

	if (nameserver != orig_nameserver)
	{
		orig_nameserver = nameserver;
		netSetNameserver(nameserver.c_str());
	}
}

int mysystem(char * cmd, char * arg1, char * arg2)
{
        int pid, i;
        switch (pid = fork())
        {
                case -1: /* can't fork */
                        perror("fork");
                        return -1;

                case 0: /* child process */
                        for(i = 3; i < 256; i++)
                                close(i);
                        if(execlp(cmd, cmd, arg1, arg2, NULL))
                        {
                                perror("exec");
                        }
                        exit(0);
                default: /* parent returns to calling process */
                        break;
        }
	waitpid(pid, 0, 0);
	return 0;
}

void CNetworkConfig::startNetwork(void)
{
	char buf[40];
	snprintf(buf, sizeof(buf), "/sbin/ifup %s", interface.c_str());
	system(buf);
}

void CNetworkConfig::stopNetwork(void)
{
	struct ifaddrs *ifap = NULL;
	if (!getifaddrs(&ifap)) {
		struct ifaddrs *it = ifap;
		while (it) {
			if (strcmp(it->ifa_name, "lo")) {
				char buf[40];
				snprintf(buf, sizeof(buf), "/sbin/ifdown %s", it->ifa_name);
				system(buf);

			}
			it = it->ifa_next;
		}
		freeifaddrs(ifap);
	}
}

void CNetworkConfig::setBroadcast(void) {
	struct in_addr sin_a, sin_m, sin_b;
        if (1 != inet_pton(AF_INET, address.c_str(), &sin_a))
		return;
        if (1 != inet_pton(AF_INET, netmask.c_str(), &sin_m))
		return;
	sin_b.s_addr = sin_a.s_addr | ~sin_m.s_addr;
	char buf[20];
	broadcast = std::string(inet_ntop(AF_INET, &sin_b, buf, sizeof(buf)));
}

bool CNetworkConfig::isWireless() {
        struct iwreq wrq;
        strncpy(wrq.ifr_name, interface.c_str(), IFNAMSIZ);
        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if(sock < 0)
                return false;
        bool res = !ioctl(sock, SIOCGIWNAME, &wrq);
        close(sock);
        return res;
}

