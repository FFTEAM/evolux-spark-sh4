P I N K - P A N E L - README...
####################################################
UseNeutrinoKeymap: ON
- RC setup changed like Neutrino-Style (keymap.xml)
####################################################
DisableSpinner: ON
- Spinner turned OFF anw won't be displayed (wait*.png.off)
####################################################
SidInTabNotFound ignore: ON
if InfobarGenerics.py exists: "sid in tab not found"-message disabled
####################################################
Use alternate DVB-Modules: ON (DREAMBOX ONLY!)
- if "tuning" problems f.e. OpenPLI-img try these ones
####################################################
Activate Oled-Dissi-Time: ON (DREAMBOX ONLY!)
- show big time+progressbar on box-display only
####################################################
Mount sata-hdd to CF: ON (options: notSet, sda1, sda2, sda3)
- mount hdd also to CF-folder so picons from internal hdd possible
####################################################
Use HDD-SwapPart: ON (options: notSet, sda2, sda3, sda4)
- if you have REAL swappartition on hdd you can use them as swap
####################################################
ONLY OPENPLI IMAGE!!!!!! (DREAMBOX ONLY!)
Use ConcHDMOD-Skin (OpenPLIonly): ON
- dl special hd-skin incl. crypt in infobar
  after dl selectable by settings-system-skin
####################################################
Fix ETH on boot (static only): ON (DREAMBOX ONLY!)
- on eth problems you can try setup STATIC-eth again by edit:
 /usr/lib/enigma2/python/Plugins/Extensions/PinkPanel/files/eth.conf
 to your right one! More info see .../files/eth.conf
####################################################

####################################################
.../PinkPanel/files/eth.conf: (DREAMBOX ONLY!)
### eth stuff ###
# please make your choice if you turn on : correctETH! #
# dm800 box-ipaddress #####
MY_IP=192.100.100.10
##########################
MY_NETMASK=255.255.255.0
MY_GATEWAY=192.100.100.1
MY_DNSSERVER=192.100.100.1
# for change mac set to 'y' an put your correct mac to new_mac ###
# not recommended!!! ###
CHANGE_MAC=n
MY_NEW_MAC=00:05:44:35:4D:77
####################################################

####################################################
PROXY-SERVER-SETUP...
Vorraussetzung isT das Ihr einen ssh-client wie "dropbear" auf eurerm Receiver laufen habt (entweder ab werk dabei wie beim openpli oder als plug erhältlich!)
Euer Router sollte zudem einen ssh-deamon laufen haben! (z.b. bei ner fritte "openssh", fritte-openssh lässt sich easy configgen via freetz-webif)
Baut man sich sein "freetz" selber kann man in "make menuconfig" auch "openssh" mit auswählen und nach flashen is alles da!!

Zuerst erstellen wir eine dss+rsa datei für dropbear auf dem receiver via telnet:

dropbearkey -t dss -f /etc/dropbear/dropbear_dss_host_key
dropbearkey -t rsa -f /etc/dropbear/dropbear_rsa_host_key

im telnet den output bis "Fingerprint"- zeile fügen wir in "known_hosts" im freetz openssh ein:

ssh-rsa AA...
ssh-dss AAA...

zuletzt noch in "auhtorized_keys" in freetz-openssh einfügen:

---root
ssh-dss AA....
####################################################

