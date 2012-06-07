#define __USE_FILE_OFFSET64 1
#include <gui/filebrowser.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/vfs.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <unistd.h>
#include <libnet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <system/debug.h>
#include <system/safe_system.h>
#include <system/set_threadname.h>

#include <global.h>
#include <neutrino.h>

#include "zapit/channel.h"

#include <driver/fontrenderer.h>
#include <driver/rcinput.h>
#include <daemonc/remotecontrol.h>

#include "widget/menue.h"
#include "widget/messagebox.h"
#include "widget/hintbox.h"
#include "widget/colorchooser.h"
#include "widget/lcdcontroler.h"
#include "widget/keychooser.h"

#include "widget/stringinput.h"
#include "widget/stringinput_ext.h"

#include "driver/screen_max.h"

#include "bouquetlist.h"
#include "color.h"
#include "eventlist.h"
#include "infoviewer.h"

#include "extra_menu.h"

#ifdef ENABLE_GRAPHLCD
static nGLCD *nglcd = NULL;
#endif

#define ONOFF_OPTION_COUNT 2
const CMenuOptionChooser::keyval ONOFF_OPTIONS[ONOFF_OPTION_COUNT] = {
	{ 0, LOCALE_OPTIONS_OFF },
	{ 1, LOCALE_OPTIONS_ON }
};

#define BOOT_OPTION_COUNT 3
const CMenuOptionChooser::keyval BOOT_OPTIONS[BOOT_OPTION_COUNT] =
{
#define BOOT_NEUTRINO 0
#define BOOT_E2       1
#define BOOT_SPARK    2
	{ BOOT_NEUTRINO, LOCALE_EXTRAMENU_BOOT_UNCHANGED },
	{ BOOT_E2, LOCALE_EXTRAMENU_BOOT_ENIGMA2 },
	{ BOOT_SPARK, LOCALE_EXTRAMENU_BOOT_SPARK }
};

#define SWAP_OPTION_COUNT 4
const CMenuOptionChooser::keyval SWAP_OPTIONS[SWAP_OPTION_COUNT] =
{
#define KEY_SWAP_SWAPOFF 0
#define KEY_SWAP_SWAPRAM 1
#define KEY_SWAP_SWAPPART 2
#define KEY_SWAP_SWAPFILE 3
	{ KEY_SWAP_SWAPOFF, LOCALE_OPTIONS_OFF },
	{ KEY_SWAP_SWAPRAM, LOCALE_EXTRAMENU_SWAP_SWAPRAM },
	{ KEY_SWAP_SWAPPART, LOCALE_EXTRAMENU_SWAP_SWAPPART },
	{ KEY_SWAP_SWAPFILE, LOCALE_EXTRAMENU_SWAP_SWAPFILE }
};

static int touch(const char *filename) {
	int fn = open(filename, O_RDWR | O_CREAT, 0644);
	if (fn > -1) {
		close(fn);
		return -1;
	}
	return 0;
}

#if 0
static int safe_system(const char *command) {
	pid_t child = fork();
	switch(child){
		case -1:
			return -1;
		case 0:
			for(int i = 3; i < 256 /* arbitrary, but high enough */; i++)
				close(i);
			signal(SIGTERM, SIG_DFL);
			signal(SIGINT, SIG_DFL);
			signal(SIGHUP, SIG_DFL);
			execl("/bin/sh", "sh", "-c", command, NULL);
			exit(-1);
		default:
			int status;
			waitpid(child, &status, 0);
			return status;
	}
}
#endif


CExtraMenuSetup::CExtraMenuSetup(void)
{
	width = w_max (40, 10);
    selected = -1;
	CNeutrinoApp::getInstance()->EmuMenu = new EMU_Menu();
	CNeutrinoApp::getInstance()->ExtraMenu = this;
#ifdef ENABLE_GRAPHLCD
	if (!nglcd)
		new nGLCD;
#endif
}

CExtraMenuSetup::~CExtraMenuSetup()
{
}

int CExtraMenuSetup::exec(CMenuTarget* parent, const std::string &actionKey)
{
	int   res = menu_return::RETURN_REPAINT;

	if(actionKey == "tunerreset-usermenu" || actionKey == "tunerreset") {
		CHintBox *hintBox = new CHintBox(LOCALE_EXTRAMENU_TUNERRESET, g_Locale->getText(LOCALE_EXTRAMENU_TUNERRESET_RESTARTING));
		hintBox->paint();
		safe_system("/usr/local/bin/pzapit -esb;sleep 2;/usr/local/bin/pzapit -lsb;sleep 2;/usr/local/bin/pzapit -rz");
		hintBox->hide();
		delete hintBox;

		if (actionKey == "tunerreset-usermenu")
			return menu_return::RETURN_EXIT_ALL;
		return res;
	}

	if (parent)
			parent->hide();

	res = showExtraMenuSetup();

	return res;
}

int CExtraMenuSetup::showExtraMenuSetup()
{
	CMenuWidget* m = new CMenuWidget(LOCALE_MAINSETTINGS_HEAD, NEUTRINO_ICON_SETTINGS, width);
	selected = m->getSelected();
	int shortcut = 1;

	m->addIntroItems(LOCALE_EXTRAMENU_SETTINGS);

	m->addItem(new CMenuForwarder(LOCALE_EXTRAMENU_EMU, true, NULL, 
		CNeutrinoApp::getInstance()->EmuMenu, NULL, CRCInput::RC_red, NEUTRINO_ICON_BUTTON_RED));

	m->addItem(new CMenuForwarder(LOCALE_EXTRAMENU_TUNERRESET, true, "",
		this, "tunerreset", CRCInput::RC_red, NEUTRINO_ICON_BUTTON_GREEN));

#ifdef ENABLE_GRAPHLCD
	m->addItem(new CMenuForwarder(LOCALE_EXTRAMENU_GLCD, true, NULL,
		new GLCD_Menu(), NULL, CRCInput::RC_blue, NEUTRINO_ICON_BUTTON_BLUE));
#endif

	m->addItem(new CMenuForwarder(LOCALE_EXTRAMENU_KERNELOPTIONS, true, NULL,
		new KernelOptions_Menu(), NULL, CRCInput::convertDigitToKey(shortcut++)));

// >> FritzCallMonitor support, part 1 of 2
#define DOTFILE_FRITZCALL "/etc/.fritzcall"
	int fritzcall = access(DOTFILE_FRITZCALL, F_OK) ? 0 : 1;
	int old_fritzcall = fritzcall;
	m->addItem(new CMenuOptionChooser(LOCALE_EXTRAMENU_FRITZCALL, &fritzcall,
		ONOFF_OPTIONS, ONOFF_OPTION_COUNT, true, NULL,
		CRCInput::convertDigitToKey(shortcut++)));
// << FritzCallMonitor support, part 1 of 2

// >> NFS-Server support, part 1 of 2
#define DOTFILE_NFSSERVER "/etc/.nfsserver"
	char ip[16] = {0};
	char mask[16] = {0};
	char broadcast[16] = {0};
	netGetIP(g_settings.ifname, ip, mask, broadcast);
	int nfsserver = access(DOTFILE_NFSSERVER, F_OK) ? 0 : 1;
	int old_nfsserver = nfsserver;
	m->addItem(new CMenuOptionChooser(LOCALE_EXTRAMENU_NFSSERVER, &nfsserver,
		ONOFF_OPTIONS, ONOFF_OPTION_COUNT, *ip, NULL,
		CRCInput::convertDigitToKey(shortcut++)));
// << NFS-Server support, part 1 of 2

// >> File System Check, part 1 of 2
#define DOTFILE_FSCK_BOOT "/etc/.fsck_boot"
#define DOTFILE_FSCK_SHUTDOWN "/etc/.fsck"
	int fsck_boot = access(DOTFILE_FSCK_BOOT, F_OK) ? 0 : 1;
	int fsck_shutdown = access(DOTFILE_FSCK_SHUTDOWN, F_OK) ? 0 : 1;
	int old_fsck_boot = fsck_boot;
	int old_fsck_shutdown = fsck_shutdown;
	m->addItem(new CMenuOptionChooser(LOCALE_EXTRAMENU_FSCK_BOOT, &fsck_boot,
		ONOFF_OPTIONS, ONOFF_OPTION_COUNT, true, NULL,
		CRCInput::convertDigitToKey(shortcut++)));
	m->addItem(new CMenuOptionChooser(LOCALE_EXTRAMENU_FSCK_SHUTDOWN, &fsck_shutdown,
		ONOFF_OPTIONS, ONOFF_OPTION_COUNT, true, NULL,
		CRCInput::convertDigitToKey(shortcut++)));
// << File System Check, part 1 of 2

// >> Swap support, part 1 of 2
#define DOTFILE_SWAPON  "/etc/.swapon"
#define DOTFILE_SWAPRAM "/etc/.swapram"
#define DOTFILE_SWAPPART "/etc/.swappart"
#define DOTFILE_SWAPFILE "/etc/.swapfile"
	int swap = access(DOTFILE_SWAPON, F_OK) ? KEY_SWAP_SWAPOFF : -1;
	if (swap != KEY_SWAP_SWAPOFF) {
		if (!access(DOTFILE_SWAPRAM, F_OK))
			swap = KEY_SWAP_SWAPRAM;
		else if (!access(DOTFILE_SWAPPART, F_OK))
			swap = KEY_SWAP_SWAPPART;
		else if (!access(DOTFILE_SWAPFILE, F_OK))
			swap = KEY_SWAP_SWAPFILE;
	}
	int old_swap = swap;
	m->addItem(new CMenuOptionChooser(LOCALE_EXTRAMENU_SWAP_SELECT, &swap, SWAP_OPTIONS, SWAP_OPTION_COUNT, true, NULL,
		CRCInput::convertDigitToKey(shortcut++)));
// << Swap support, part 1 of 2

#if 0
	// this is now handled by Service > Software Update
	CEvoluxSoftwareUpdate_Menu *evoUpdateMenu = new CEvoluxSoftwareUpdate_Menu();

	if (!access("/etc/enigma2/settings", R_OK))
		m->addItem(new CMenuForwarder(LOCALE_EXTRAMENU_EVOLUXUPDATE, true, NULL,
			evoUpdateMenu, NULL, CRCInput::convertDigitToKey(shortcut++)));
#endif

// >> Boot Selection, part 1 of 2
#define DOTFILE_BOOTE2 "/etc/.start_enigma2"
#define DOTFILE_BOOTSPARK "/etc/.start_spark"
	int boot = BOOT_NEUTRINO;
	if (!access(DOTFILE_BOOTSPARK, F_OK))
		boot = BOOT_SPARK;
	else if (!access(DOTFILE_BOOTE2, F_OK))
		boot = BOOT_E2;
	int old_boot = boot;

	m->addItem(new CMenuOptionChooser(LOCALE_EXTRAMENU_BOOT_HEAD, &boot,
		BOOT_OPTIONS, BOOT_OPTION_COUNT, true, NULL,
		CRCInput::convertDigitToKey(shortcut++)));
// << Boot Selection, part 1 of 2

	int res = m->exec (NULL, "");
	m->hide ();
	m->setSelected(selected);
	delete m;
#if 0
	delete evoUpdateMenu;
#endif

	std::string hintText = "";

// >> Swap support, part 2 of 2
	if (old_swap != swap) {
		switch (old_swap) {
		case KEY_SWAP_SWAPFILE:
			system("swapoff /dev/loop0 ; losetup -d /dev/loop0");
		case KEY_SWAP_SWAPRAM:
		case KEY_SWAP_SWAPPART:
			system("swapoff -a");
			if (hintText.length())
				hintText += "\n";
			hintText += "SWAP " + string(g_Locale->getText(SWAP_OPTIONS[old_swap].value)) + " " + string(g_Locale->getText(LOCALE_EXTRAMENU_ENABLED));
		}

		unlink(DOTFILE_SWAPON);
		unlink(DOTFILE_SWAPRAM);
		unlink(DOTFILE_SWAPPART);
		unlink(DOTFILE_SWAPFILE);

		switch(swap) {
		case KEY_SWAP_SWAPRAM:
			touch(DOTFILE_SWAPRAM);
			break;
		case KEY_SWAP_SWAPPART:
			touch(DOTFILE_SWAPPART);
			break;
		case KEY_SWAP_SWAPFILE:
			touch(DOTFILE_SWAPFILE);
			break;
		}
		if (swap != KEY_SWAP_SWAPOFF) {
			touch(DOTFILE_SWAPON);
			safe_system("/etc/init.d/Swap.sh >/dev/null 2>&1 &");
			if (hintText.length())
				hintText += "\n";
			hintText += "SWAP " + string(g_Locale->getText(SWAP_OPTIONS[swap].value)) + " " + string(g_Locale->getText(LOCALE_EXTRAMENU_ENABLED));
		}
	}
// << Swap support, part 2 of 2

// >> FritzCallMonitor support, part 2 of 2
	if (old_fritzcall != fritzcall) {
		if (hintText.length())
			hintText += "\n";
		hintText += string(g_Locale->getText(LOCALE_EXTRAMENU_FRITZCALL)) + " ";
		if (fritzcall) {
			touch(DOTFILE_FRITZCALL);
			safe_system("/var/plugins/fritzcall/fb.sh start >/dev/null 2>&1 &");
			hintText += string(g_Locale->getText(LOCALE_EXTRAMENU_ENABLED));
		} else {
			unlink(DOTFILE_FRITZCALL);
			safe_system("/var/plugins/fritzcall/fb.sh stop >/dev/null 2>&1 &");
			hintText += string(g_Locale->getText(LOCALE_EXTRAMENU_DISABLED));
		}
	}
// << FritzCallMonitor support, part 2 of 2

// >> NFS-Server support, part 2 of 2
	if (old_nfsserver != nfsserver) {
		if (hintText.length())
			hintText += "\n";
		hintText += string(g_Locale->getText(LOCALE_EXTRAMENU_NFSSERVER)) + " ";
		if (nfsserver) {
			struct sockaddr_in ip_sin;
			struct sockaddr_in mask_sin;
			inet_aton(ip, &ip_sin.sin_addr);
			inet_aton(mask, &mask_sin.sin_addr);
			ip_sin.sin_addr.s_addr &= mask_sin.sin_addr.s_addr;
			strncpy(ip, inet_ntoa(ip_sin.sin_addr), sizeof(ip));
			touch(DOTFILE_NFSSERVER);
			char cmd[1024];
			snprintf(cmd, sizeof(cmd),
				"sed 's@/media/hdd.*@/media/hdd %s/%s(rw,async,no_root_squash)@g' -i /etc/exports;"
				"/etc/init.d/nfs-common start;/etc/init.d/nfs-kernel-server start",
				ip, mask);
			safe_system(cmd);
			hintText += string(g_Locale->getText(LOCALE_EXTRAMENU_ENABLED));
		} else {
			unlink(DOTFILE_NFSSERVER);
			safe_system("/etc/init.d/nfs-kernel-server stop;/etc/init.d/nfs-common stop");
			hintText += string(g_Locale->getText(LOCALE_EXTRAMENU_DISABLED));
		}
	}
// << NFS-Server support, part 2 of 2

// >> File System Check, part 2 of 2
	if (old_fsck_boot != fsck_boot) {
		if (hintText.length())
			hintText += "\n";
		hintText += string(g_Locale->getText(LOCALE_EXTRAMENU_FSCK_BOOT)) + " ";
		if (fsck_boot) {
			touch(DOTFILE_FSCK_BOOT);
			hintText += string(g_Locale->getText(LOCALE_EXTRAMENU_ENABLED));
		} else {
			unlink(DOTFILE_FSCK_BOOT);
			hintText += string(g_Locale->getText(LOCALE_EXTRAMENU_DISABLED));
		}
	}
	if (old_fsck_shutdown != fsck_shutdown) {
		if (hintText.length())
			hintText += "\n";
		hintText += string(g_Locale->getText(LOCALE_EXTRAMENU_FSCK_SHUTDOWN)) + " ";
		if (fsck_shutdown) {
			touch(DOTFILE_FSCK_SHUTDOWN);
			hintText += string(g_Locale->getText(LOCALE_EXTRAMENU_ENABLED));
		} else {
			unlink(DOTFILE_FSCK_SHUTDOWN);
			hintText += string(g_Locale->getText(LOCALE_EXTRAMENU_DISABLED));
		}
	}
// << File System Check, part 2 of 2

// >> Boot Selection, part 2 of 2
	if (boot != old_boot)
	{
		CHintBox *b = NULL;
		if(boot == BOOT_SPARK || old_boot == BOOT_SPARK) {
			b = new CHintBox(LOCALE_EXTRAMENU_BOOT_BOOTARGS_HEAD, g_Locale->getText(LOCALE_EXTRAMENU_BOOT_BOOTARGS_TEXT));
			b->paint();
		}
		if (boot == BOOT_SPARK) {
			touch(DOTFILE_BOOTSPARK);
			system("fw_setenv -s /etc/bootargs_orig");
			if (hintText.length())
				hintText += "\n";
			hintText += "Spark " + string(g_Locale->getText(LOCALE_EXTRAMENU_BOOT_CHANGED));
		}
		if (old_boot == BOOT_SPARK) {
			unlink(DOTFILE_BOOTSPARK);
			system("fw_setenv -s /etc/bootargs_evolux");
		}
		if(b) {
			b->hide();
			delete b;
		}
		if (boot == BOOT_E2 && old_boot == BOOT_NEUTRINO) {
			if (hintText.length())
				hintText += "\n";
			if (access("/usr/local/bin/enigma2", X_OK)) {
				hintText += "E2 " + string(g_Locale->getText(LOCALE_EXTRAMENU_BOOT_UNSUPPORTED));
			} else {
				touch(DOTFILE_BOOTE2);
				hintText += "E2 " + string(g_Locale->getText(LOCALE_EXTRAMENU_BOOT_CHANGED));
			}
		}
		else if (boot == BOOT_NEUTRINO && old_boot == BOOT_E2)
			unlink(DOTFILE_BOOTE2);
		if (boot == BOOT_NEUTRINO) {
			if (hintText.length())
				hintText += "\n";
			hintText += string(g_Locale->getText(LOCALE_EXTRAMENU_BOOT_THIS));
		}
	}
// << Boot Selection, part 2 of 2

	if (hintText.length())
		ShowHintUTF(LOCALE_MESSAGEBOX_INFO, hintText.c_str(), 450, 2);

	return res;
}


static struct {
#define EXTRA_CAM_SELECTED "cam_selected"
        std::string	cam_selected;
#ifdef ENABLE_GRAPHLCD
#define GLCD_ENABLE "glcd_enable"
        int	glcd_enable;
#define GLCD_COLOR_FG "glcd_color_fg"
        uint32_t	glcd_color_fg;
#define GLCD_COLOR_BG "glcd_color_bg"
        uint32_t	glcd_color_bg;
#define GLCD_COLOR_BAR "glcd_color_bar"
        uint32_t	glcd_color_bar;
#define GLCD_FONT "glcd_font"
        string		glcd_font;
#define GLCD_SIZE_CHANNEL "glcd_percent_channel"
        int		glcd_percent_channel;
#define GLCD_SIZE_EPG "glcd_percent_epg"
        int		glcd_percent_epg;
#define GLCD_SIZE_BAR "glcd_percent_bar"
        int		glcd_percent_bar;
#define GLCD_SIZE_TIME "glcd_percent_time"
        int		glcd_percent_time;
#define GLCD_MIRROR_OSD "glcd_mirror_osd"
        int		glcd_mirror_osd;
#define GLCD_TIME_IN_STANDBY "glcd_time_in_standby"
        int		glcd_time_in_standby;
#endif
} settings;

CConfigFile *configfile = NULL;

static bool saveSettings() {
	if (configfile) {
		configfile->setString(EXTRA_CAM_SELECTED, settings.cam_selected);
#ifdef ENABLE_GRAPHLCD
		configfile->setInt32(GLCD_ENABLE, settings.glcd_enable);
		configfile->setInt32(GLCD_COLOR_FG, settings.glcd_color_fg);
		configfile->setInt32(GLCD_COLOR_BG, settings.glcd_color_bg);
		configfile->setInt32(GLCD_COLOR_BAR, settings.glcd_color_bar);
		configfile->setInt32(GLCD_SIZE_CHANNEL, settings.glcd_percent_channel);
		configfile->setInt32(GLCD_SIZE_EPG, settings.glcd_percent_epg);
		configfile->setInt32(GLCD_SIZE_BAR, settings.glcd_percent_bar);
		configfile->setInt32(GLCD_SIZE_TIME, settings.glcd_percent_time);
		configfile->setInt32(GLCD_MIRROR_OSD, settings.glcd_mirror_osd);
		configfile->setInt32(GLCD_TIME_IN_STANDBY, settings.glcd_time_in_standby);
		configfile->setString(GLCD_FONT, settings.glcd_font);
#endif
		configfile->saveConfig(EXTRA_SETTINGS_FILE);
		return true;
	}
	return false;
}

static bool initSettings() {
	settings.cam_selected = "disabled";
#ifdef ENABLE_GRAPHLCD
	settings.glcd_enable = 0;
	settings.glcd_color_fg = GLCD::cColor::White;
	settings.glcd_color_bg = GLCD::cColor::Blue;
	settings.glcd_color_bar = GLCD::cColor::Red;
	settings.glcd_percent_channel = 18;
	settings.glcd_percent_epg = 8;
	settings.glcd_percent_bar = 6;
	settings.glcd_percent_time = 22;
	settings.glcd_mirror_osd = 0;
	settings.glcd_time_in_standby = 0;
	settings.glcd_font = FONTDIR "/neutrino.ttf";
#endif
}

static bool loadSettings() {
	if (!configfile) {
		initSettings();
		configfile = new CConfigFile('=');
		if (configfile->loadConfig(EXTRA_SETTINGS_FILE)) {
			settings.cam_selected = configfile->getString(EXTRA_CAM_SELECTED, "disabled");
#ifdef ENABLE_GRAPHLCD
			settings.glcd_enable = configfile->getInt32(GLCD_ENABLE, 0);
			settings.glcd_color_fg = configfile->getInt32(GLCD_COLOR_FG, GLCD::cColor::White);
			settings.glcd_color_bg = configfile->getInt32(GLCD_COLOR_BG, GLCD::cColor::Blue);
			settings.glcd_color_bar = configfile->getInt32(GLCD_COLOR_BAR, GLCD::cColor::Red);
			settings.glcd_percent_channel = configfile->getInt32(GLCD_SIZE_CHANNEL, 18);
			settings.glcd_percent_epg = configfile->getInt32(GLCD_SIZE_EPG, 8);
			settings.glcd_percent_bar = configfile->getInt32(GLCD_SIZE_BAR, 6);
			settings.glcd_percent_time = configfile->getInt32(GLCD_SIZE_TIME, 22);
			settings.glcd_mirror_osd = configfile->getInt32(GLCD_MIRROR_OSD, 0);
			settings.glcd_time_in_standby = configfile->getInt32(GLCD_TIME_IN_STANDBY, 0);
			settings.glcd_font = configfile->getString(GLCD_FONT, FONTDIR "/neutrino.ttf");
#endif
			return true;
		}
	}
	return false;
}

////////////////////////////// EMU Menu START ////////////////////////////////////

#define EMU_OPTION_COUNT 7
static EMU_Menu::emu_list EMU_list[EMU_OPTION_COUNT] =
{
	  { "mgcamd", "rm -f /tmp/camd.socket >/dev/null; /usr/bin/mgcamd /var/keys/mg_cfg >/dev/null 2>&1 &", "kill -9 $(pidof mgcamd)", false }
	, { "incubusCamd", "rm -f /tmp/camd.socket 2>/dev/null ; /usr/bin/incubusCamd >/dev/null 2>&1 &", "kill -9 $(pidof incubusCamd)", false }
	, { "camd3", "/usr/bin/camd3 /var/keys/camd3.config >/dev/null 2>&1 &", "kill -9 $(pidof camd3)", false }
	, { "mbox", "/usr/bin/mbox /var/keys/mbox.cfg >/dev/null 2>&1 &", "kill -9 $(pidof mbox) ; rm -f /tmp/share.* /tmp/mbox.ver /tmp/*.info 2>/dev/null", false }
	, { "oscam", "/usr/bin/oscam -b -c /var/keys >/dev/null 2>&1 &", "kill -9 $(pidof oscam)", false }
	, { "spcs", "/usr/bin/spcs -c /var/keys >/dev/null 2>&1 &", "kill -9 $(pidof spcs)", false }
	, { "gbox", "/usr/bin/gbox >/dev/null 2>&1 &", "touch /tmp/gbox.kill", false }
};

int EMU_Menu::get_installed_count() {
	return installed_count;
}

int EMU_Menu::update_installed()
{
	installed_count = 0;

	for (int i = 0; i < EMU_OPTION_COUNT; i++) {
		string e = "/usr/bin/" + string(EMU_list[i].procname);
		if (!access(e.c_str(), X_OK)) {
			EMU_list[i].installed = true;
			installed_count++;
		}
	}

	return installed_count;
}

int EMU_Menu::update_selected()
{
	for (int i = 0; i < EMU_OPTION_COUNT; i++)
		if (!settings.cam_selected.compare(EMU_list[i].procname) && EMU_list[i].installed){
			selected = i;
			break;
		}
	return selected;
}

static bool is_scrambled(void){
	bool res = true;
	CChannelList *channelList = CNeutrinoApp::getInstance()->channelList;
	if (!channelList)
		return res;
	int curnum = channelList->getActiveChannelNumber();
	if (curnum < -1)
		return res;
	CZapitChannel *channel = channelList->getChannel(curnum);
	if (!channel)
		return res;
	return channel->scrambled ? true : false;
}

EMU_Menu::EMU_Menu()
{
	width = w_max (40, 10);
	selected = -1;
	suspended = false;

	loadSettings();
	update_installed();
	update_selected();

	if (selected > -1){
		bool scrambled = false;
		ifstream zc (CONFIGDIR "/zapit/zapit.conf");
		string line;
		while (zc.good() && !scrambled)
			if (getline(zc, line) && !line.compare((line, "lastChannelTVScrambled=true")))
					scrambled = true;
		zc.close();

		string cmd = "(" + string(EMU_list[selected].start_command)
			+ ( scrambled ? "sleep 2 ; /usr/local/bin/pzapit -rz" : "" )
			+ " >/dev/null 2>&1) &";
		safe_system(cmd.c_str());
	}
	for (int i = 0; i < EMU_OPTION_COUNT; i++)
		EMU_list[i].cmf = NULL;
}

int EMU_Menu::exec(CMenuTarget* parent, const std::string & actionKey)
{
	int res = menu_return::RETURN_REPAINT;

	bool doReset = false;
	int emu = EMU_OPTION_COUNT;

	if(actionKey == "disable") {
		emu = -1;
	} else if (actionKey == "reset") {
		doReset = true;
		emu = selected;
	} else if (actionKey == "reset-usermenu") {
		doReset = true;
		emu = selected;
		res = menu_return::RETURN_EXIT_ALL;
	} else
		for (emu = 0; emu < EMU_OPTION_COUNT; emu++)
			if (!strcmp(EMU_list[emu].procname, actionKey.c_str()))
				break;

fprintf(stderr, "### selected: %d emu: %d\n", selected,emu);
	if (emu < EMU_OPTION_COUNT) {
		if (selected > -1) {
			safe_system(EMU_list[selected].stop_command);
			string m = " " + string(EMU_list[selected].procname) + " " + g_Locale->getText(LOCALE_EXTRAMENU_DISABLED);
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, m.c_str(), 450, 2); // UTF-8("")
			if (EMU_list[selected].cmf)
				EMU_list[selected].cmf->setOptionValue(g_Locale->getText(LOCALE_OPTIONS_OFF));
		}
		if (emu > -1 && (emu != selected || doReset)) {
			safe_system(EMU_list[emu].start_command);

			string cmd = "(" + string(EMU_list[emu].start_command);
			if (is_scrambled())
				safe_system("sleep 2; /usr/local/bin/pzapit -rz >/dev/null 2>&1");
			string m = " " + string(EMU_list[emu].procname) + " " + g_Locale->getText(LOCALE_EXTRAMENU_ENABLED);
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, m.c_str(), 450, 2); // UTF-8("")
			if (EMU_list[emu].cmf)
				EMU_list[emu].cmf->setOptionValue(g_Locale->getText(LOCALE_OPTIONS_ON));
			settings.cam_selected = string(EMU_list[emu].procname);
			selected = emu;
		} else
			selected = -1;
		return res;
	}

	if (parent)
		parent->hide();

	EMU_Menu_Settings();

	return res;
}

void EMU_Menu::hide()
{
}

void EMU_Menu::EMU_Menu_Settings()
{
	int emu = selected;
	int emu_old = emu;

	update_installed();

	CMenuWidget* menu = new CMenuWidget(LOCALE_EXTRAMENU_EMU, "settings");
	menu->addItem(GenericMenuSeparator);
	menu->addItem(GenericMenuBack);
	menu->addItem(GenericMenuSeparatorLine);

	int shortcut = 1;
	for (int i = 0; i < EMU_OPTION_COUNT; i++)
		if (EMU_list[i].installed) {
			EMU_list[i].cmf = new CMenuForwarderNonLocalized(EMU_list[i].procname, true,
				g_Locale->getText((i == selected) ? LOCALE_OPTIONS_ON : LOCALE_OPTIONS_OFF),
				this, EMU_list[i].procname, CRCInput::convertDigitToKey(shortcut++));
			menu->addItem(EMU_list[i].cmf, (i == selected));
		}

	menu->addItem(GenericMenuSeparatorLine);
	menu->addItem(new CMenuForwarder(LOCALE_EXTRAMENU_EMU_RESTART, true, "", this, "reset",
			CRCInput::RC_red, NEUTRINO_ICON_BUTTON_RED));
	menu->addItem(new CMenuForwarder(LOCALE_EXTRAMENU_EMU_DISABLE, true, "", this, "disable",
			CRCInput::RC_blue, NEUTRINO_ICON_BUTTON_BLUE));

	menu->exec(NULL, "");
        menu->hide();
        delete menu;
	saveSettings();
}

void EMU_Menu::suspend()
{
	if (selected > -1 && !suspended) {
		safe_system(EMU_list[selected].stop_command);
		suspended = true;
	}
}

void EMU_Menu::resume()
{
	if (selected  > -1 && suspended) {
		safe_system(EMU_list[selected].start_command);
		if (is_scrambled())
			safe_system("sleep 2; /usr/local/bin/pzapit -rz >/dev/null 2>&1");
		suspended = false;
	}
}

////////////////////////////// EMU Menu END //////////////////////////////////////

/*
	Neutrino graphlcd daemon thread

	(/) 2012 by martii, with portions shamelessly copied from infoviewer.cpp


	License: GPL

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifdef ENABLE_GRAPHLCD

#include <string>
#include <algorithm>
#include <sectionsdclient/sectionsdclient.h>

void sectionsd_getEventsServiceKey(t_channel_id serviceUniqueKey, CChannelEventList &eList, char search = 0, std::string search_text = "");
void sectionsd_getCurrentNextServiceKey(t_channel_id uniqueServiceKey, CSectionsdClient::responseGetCurrentNextInfoChannelID& current_next );

nGLCD::nGLCD() {
	lcd = NULL;
	Channel = "EvoLux";
	Epg = "Neutrino";
	scrollChannel = "EvoLux";
	scrollEpg = "Neutrino";

	sem_init(&sem, 0, 1);

	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK_NP);
	pthread_mutex_init(&mutex, &attr);

	stagingChannel = "";
	stagingEpg = "";
	channelLocked = false;
	doRescan = false;
	doStandby = false;
	doStandbyTime = false;
	doShowVolume = false;
	doSuspend = false;
	doExit = false;
	doMirrorOSD = false;
	fontsize_channel = 0;
	fontsize_epg = 0;
	fontsize_time = 0;
	doScrollChannel = false;
	doScrollEpg = false;
	percent_channel = 0;
	percent_time = 0;
	percent_epg = 0;
	percent_bar = 0;
	percent_time = 0;
	percent_space = 0;
	Scale = 0;
	bitmap = NULL;
	loadSettings();

	nglcd = this;

	if (!settings.glcd_enable)
		doSuspend = true;

	if (pthread_create (&thrGLCD, 0, nGLCD::Run, NULL) != 0 )
		fprintf(stderr, "ERROR: pthread_create(nGLCD::Init)\n");
}

void nGLCD::Lock(void)
{
	if (nglcd)
		pthread_mutex_lock(&nglcd->mutex);
}

void nGLCD::Unlock(void)
{
	if (nglcd)
		pthread_mutex_unlock(&nglcd->mutex);
}

nGLCD::~nGLCD() {
	Suspend();
	nglcd = NULL;
	if (lcd) {
		lcd->DeInit();
		delete lcd;
	}
}

void nGLCD::Exec() {
	if (!lcd)
		return;

	bitmap->Clear(settings.glcd_color_bg);

	if (CNeutrinoApp::getInstance()->recordingstatus) {
		bitmap->DrawRectangle(0, 0, bitmap->Width() - 1, bitmap->Height() - 1, settings.glcd_color_bar, false);
		bitmap->DrawRectangle(1, 1, bitmap->Width() - 2, bitmap->Height() - 2, settings.glcd_color_bar, false);
	}

	int off = 0;

	if (percent_channel) {
		off += percent_space;
		int fw = font_channel.Width(scrollChannel);
		if (fw && !doStandbyTime)
			bitmap->DrawText(MAX(0,(bitmap->Width() - 4 - fw)/2),
				off * bitmap->Height()/100, bitmap->Width() - 4, scrollChannel,
				&font_channel, settings.glcd_color_fg, GLCD::cColor::Transparent);
		off += percent_channel;
		off += percent_space;
		if (scrollChannel.length() > Channel.length())
			scrollChannel = scrollChannel.substr(1);
		else
			doScrollChannel = false;
	}

	if (percent_epg) {
		off += percent_space;
		int fw = font_epg.Width(scrollEpg);
		if (fw && !doStandbyTime)
			bitmap->DrawText(MAX(0,(bitmap->Width() - 4 - fw)/2),
				off * bitmap->Height()/100, bitmap->Width() - 4, scrollEpg,
				&font_epg, settings.glcd_color_fg, GLCD::cColor::Transparent);
		off += percent_epg;
		off += percent_space;
		if (scrollEpg.length() > Epg.length())
			scrollEpg = scrollEpg.substr(1);
		else
			doScrollEpg = false;
	}

	if (percent_bar) {
		off += percent_space;
		int bar_top = off * bitmap->Height()/100;
		off += percent_bar;
		int bar_bottom = off * bitmap->Height()/100;
		if (!doStandbyTime) {
			bitmap->DrawHLine(0, bar_top, bitmap->Width(), settings.glcd_color_fg);
			bitmap->DrawHLine(0, bar_bottom, bitmap->Width(), settings.glcd_color_fg);
			if (Scale)
				bitmap->DrawRectangle(0, bar_top + 1, Scale * (bitmap->Width() - 1)/100,
					bar_bottom - 1, settings.glcd_color_bar, true);
		}
		off += percent_space;
	}

	if (percent_time) {
		off += percent_space;
		char timebuf[10];
		strftime(timebuf, sizeof(timebuf), "%H:%M", tm);

		std::string Time = std::string(timebuf);

		bitmap->DrawText(MAX(0,(bitmap->Width() - 4 - font_time.Width(Time))/2),
			off * bitmap->Height()/100, bitmap->Width() - 1, Time,
			&font_time, settings.glcd_color_fg, GLCD::cColor::Transparent);
	}

	lcd->SetScreen(bitmap->Data(), bitmap->Width(), bitmap->Height());
	lcd->Refresh(true);
}

static bool sortByDateTime (const CChannelEvent& a, const CChannelEvent& b)
{
	return a.startTime < b.startTime;
}

void nGLCD::updateFonts() {
	bool changed = false;
	int percent = settings.glcd_percent_channel + settings.glcd_percent_epg + settings.glcd_percent_bar + settings.glcd_percent_time;

	if (percent < 100)
		percent = 100;

	// normalize values
	percent_channel = settings.glcd_percent_channel * 100 / percent;
	percent_epg = settings.glcd_percent_epg * 100 / percent;
	percent_bar = settings.glcd_percent_bar * 100 / percent;
	percent_time = settings.glcd_percent_time * 100 / percent;

	// calculate height
	int fontsize_channel_new = percent_channel * nglcd->lcd->Height() / 100;
	int fontsize_epg_new = percent_epg * nglcd->lcd->Height() / 100;
	int fontsize_time_new = percent_time * nglcd->lcd->Height() / 100;

	if (!fonts_initialized || (fontsize_channel_new != fontsize_channel)) {
		fontsize_channel = fontsize_channel_new;
		if (!font_channel.LoadFT2(settings.glcd_font, "UTF-8", fontsize_channel)) {
			settings.glcd_font = FONTDIR "/neutrino.ttf";
			font_channel.LoadFT2(settings.glcd_font, "UTF-8", fontsize_channel);
		}
		changed = true;
	}
	if (!fonts_initialized || (fontsize_epg_new != fontsize_epg)) {
		fontsize_epg = fontsize_epg_new;
		if (!font_epg.LoadFT2(settings.glcd_font, "UTF-8", fontsize_epg)) {
			settings.glcd_font = FONTDIR "/neutrino.ttf";
			font_epg.LoadFT2(settings.glcd_font, "UTF-8", fontsize_epg);
		}
		changed = true;
	}
	if (!fonts_initialized || (fontsize_time_new != fontsize_time)) {
		fontsize_time = fontsize_time_new;
		if (!font_time.LoadFT2(settings.glcd_font, "UTF-8", fontsize_time)) {
			settings.glcd_font = FONTDIR "/neutrino.ttf";
			font_time.LoadFT2(settings.glcd_font, "UTF-8", fontsize_time);
		}
		changed = true;
	}

	if (!changed)
		return;

	int div = 0;
	if (percent_channel)
		div += 2;
	if (percent_epg)
		div += 2;
	if (percent_bar)
		div += 2;
	if (percent_time)
		div += 2;
	percent_space = (100 - percent_channel - percent_time - percent_epg - percent_bar) / div;

	fonts_initialized = true;
}

void* nGLCD::Run(void *)
{
#ifdef EVOLUX
	set_threadname("nGLCD::Run");
#endif

	if (GLCD::Config.Load(kDefaultConfigFile) == false) {
		fprintf(stderr, "Error loading config file!\n");
		return NULL;
	}
	if ((GLCD::Config.driverConfigs.size() < 1)) {
		fprintf(stderr, "No driver config found!\n");
		return NULL;
	}

	struct timespec ts;
	CChannelEventList evtlist;
	CSectionsdClient::CurrentNextInfo info_CurrentNext;                                 
	t_channel_id channel_id = -1;
	info_CurrentNext.current_zeit.startzeit = 0;
	info_CurrentNext.current_zeit.dauer = 0;
	info_CurrentNext.flags = 0;

	nglcd->fonts_initialized = false;
	bool broken = false;

	do {
		if (broken) {
#ifdef GLCD_DEBUG
			fprintf(stderr, "No graphlcd display found ... sleeping for 30 seconds\n");
#endif
			clock_gettime(CLOCK_REALTIME, &ts);
			ts.tv_sec += 30;
			sem_timedwait(&nglcd->sem, &ts);
			broken = false;
			if (nglcd->doExit)
				break;
			if (!settings.glcd_enable)
				continue;
		} else
				while ((nglcd->doSuspend || nglcd->doStandby || !settings.glcd_enable) && !nglcd->doExit)
					sem_wait(&nglcd->sem);

		if (nglcd->doExit)
			break;

		int warmUp = 5;
		nglcd->lcd = GLCD::CreateDriver(GLCD::Config.driverConfigs[0].id, &GLCD::Config.driverConfigs[0]);
		if (!nglcd->lcd) {
#ifdef GLCD_DEBUG
			fprintf(stderr, "CreateDriver failed.\n");
#endif
			broken = true;
			continue;
		}
#ifdef GLCD_DEBUG
		fprintf(stderr, "CreateDriver succeeded.\n");
#endif
		if (nglcd->lcd->Init()) {
			delete nglcd->lcd;
			nglcd->lcd = NULL;
#ifdef GLCD_DEBUG
			fprintf(stderr, "LCD init failed.\n");
#endif
			broken = true;
			continue;
		}
#ifdef GLCD_DEBUG
		fprintf(stderr, "LCD init succeeded.\n");
#endif

		if (!nglcd->bitmap)
			nglcd->bitmap = new GLCD::cBitmap(nglcd->lcd->Width(), nglcd->lcd->Height(), settings.glcd_color_bg);

		nglcd->Update();

		nglcd->doMirrorOSD = false;

		while ((!nglcd->doSuspend && !nglcd->doStandby) && !nglcd->doExit && settings.glcd_enable) {
			if (nglcd->doMirrorOSD) {
				nglcd->bitmap->Clear(GLCD::cColor::Black);
				ts.tv_sec = 0; // don't wait
				static CFrameBuffer* fb = CFrameBuffer::getInstance();
				int fb_width = fb->getScreenWidth(true);
				int fb_height = fb->getScreenHeight(true);
				int lcd_width = nglcd->bitmap->Width();
				int lcd_height = nglcd->bitmap->Height();
				uint32_t *fbp = fb->getFrameBufferPointer();

				// determine OSD frame geometry

				int y_min = 0;
				for (int y = 0; y < fb_height && !y_min; y++) {
					for (int x = 0; x < fb_width; x++) {
						if (*(fbp + fb_width * y + x)) {
							y_min = y;
							break;
						}
					}
				}
				int y_max = 0;
				for (int y = fb_height - 1; y_min < y && !y_max; y--) {
					for (int x = 0; x < fb_width; x++) {
						if (*(fbp + fb_width * y + x)) { 
							y_max = y;
							break;
						}
					}
				}
				int x_min = fb_width - 1;
				for (int y = y_min; y < y_max; y++) {
					for (int x = 0; x < fb_width; x++) {
						if (*(fbp + fb_width * y + x) && x < x_min) {
							x_min = x;
							break;
						}
					}
				}
				int x_max = x_min;
				for (int y = y_min; y < y_max; y++) {
					for (int x = fb_width - 1; x > x_min; x--) {
						if (*(fbp + fb_width * y + x) && x > x_max) {
							x_max= x;
							break;
						}
					}
				}

				int fb_w = x_max - x_min;
				int fb_h = y_max - y_min;

				if (!fb_w || !fb_w) {
					usleep(500000);
					continue;
				}

				// Keep aspect by using the smallest up-scaling factor
				if (fb_width * fb_h > fb_height * fb_w) {
					int fb_w2 = fb_width * fb_h/fb_height;
					x_min += (fb_w - fb_w2)/2;
					fb_w = fb_w2;
				} else {
					int fb_h2 = fb_height * fb_w/fb_width;
					y_min += (fb_h - fb_h2)/2;
					fb_h = fb_h2;
				}
				// Compensate for rounding errors
				if (x_min < 0)
					x_min = 0;
				if (y_min < 0)
					y_min = 0;

				for (int y = 0; y < lcd_height; y++) {
					int ystride = y_min * fb_width;
					for (int x = 0; x < lcd_width; x++) {
						// FIXME: There might be some oscure bug somewhere that invalidate the address of bitmap->DrawPixel()
						nglcd->bitmap->DrawPixel(x, y, *(fbp + ystride + (y * fb_h / lcd_height) * fb_width
										 + x_min + (x * fb_w / lcd_width)));
					}
				}

				nglcd->lcd->SetScreen(nglcd->bitmap->Data(), lcd_width, lcd_height);
				nglcd->lcd->Refresh(true);
				continue;
			} // end of fb mirroring

			clock_gettime(CLOCK_REALTIME, &ts);
			nglcd->tm = localtime(&ts.tv_sec);
			nglcd->updateFonts();
			nglcd->Exec();
			clock_gettime(CLOCK_REALTIME, &ts);
			nglcd->tm = localtime(&ts.tv_sec);
			if (warmUp > 0) {
				ts.tv_sec += 4;
				warmUp--;
			} else {
				ts.tv_sec += 60 - nglcd->tm->tm_sec;
				ts.tv_nsec = 0;
			}

			if (!nglcd->doScrollChannel && !nglcd->doScrollEpg)
				sem_timedwait(&nglcd->sem, &ts);

			while(!sem_trywait(&nglcd->sem));

			if(nglcd->doRescan || nglcd->doSuspend || nglcd->doStandby || nglcd->doExit)
				break;

			if (nglcd->doShowVolume) {
				nglcd->Epg = "";
				if (nglcd->Channel.compare(g_Locale->getText(LOCALE_EXTRAMENU_GLCD_VOLUME))) {
					nglcd->Channel = g_Locale->getText(LOCALE_EXTRAMENU_GLCD_VOLUME);
					if (nglcd->font_channel.Width(nglcd->Channel) > nglcd->bitmap->Width() - 4)
						nglcd->scrollChannel = nglcd->Channel + "      " + nglcd->Channel + "      " + nglcd->Channel;
					else
						nglcd->scrollChannel = nglcd->Channel;
				}
				nglcd->scrollEpg = nglcd->Epg;
				nglcd->Scale = g_settings.current_volume;
				channel_id = -1;
			} else if (nglcd->channelLocked) {
				nglcd->Lock();
				if (nglcd->Epg.compare(nglcd->stagingEpg)) {
					nglcd->Epg = nglcd->stagingEpg;
					if (nglcd->font_epg.Width(nglcd->Epg) > nglcd->bitmap->Width() - 4)
						nglcd->scrollEpg = nglcd->Epg + "      " + nglcd->Epg + "      " + nglcd->Epg;
					else
						nglcd->scrollEpg = nglcd->Epg;
				}
				if (nglcd->Channel.compare(nglcd->stagingChannel)) {
					nglcd->Channel = nglcd->stagingChannel;
					if (nglcd->font_channel.Width(nglcd->Channel) > nglcd->bitmap->Width() - 4)
						nglcd->scrollChannel = nglcd->Channel + "      " + nglcd->Channel + "      " + nglcd->Channel;
					else
						nglcd->scrollChannel = nglcd->Channel;
				}
				nglcd->Scale = 0;
				channel_id = -1;
				nglcd->Unlock();
			} else {
				CChannelList *channelList = CNeutrinoApp::getInstance ()->channelList;
				if (!channelList)
					continue;
				t_channel_id new_channel_id = channelList->getActiveChannel_ChannelID ();
				if (new_channel_id < 0)
					continue;

				if ((new_channel_id != channel_id)) {
					nglcd->Channel = channelList->getActiveChannelName ();
					nglcd->Epg = "";
					nglcd->Scale = 0;
					nglcd->scrollEpg = nglcd->Epg;
					if (nglcd->font_channel.Width(nglcd->Channel) > nglcd->bitmap->Width() - 4) {
						nglcd->scrollChannel = nglcd->Channel + "      " + nglcd->Channel + "      " + nglcd->Channel;
						nglcd->doScrollChannel = true;
					} else {
						nglcd->scrollChannel = nglcd->Channel;
						nglcd->doScrollChannel = false;
					}
					warmUp = 1;
				}

				sectionsd_getCurrentNextServiceKey(channel_id & 0xFFFFFFFFFFFFULL, info_CurrentNext);
				if ((channel_id != new_channel_id) || (evtlist.empty())) {
					evtlist.clear();
					sectionsd_getEventsServiceKey(new_channel_id & 0xFFFFFFFFFFFFULL, evtlist);
					if (!evtlist.empty())
						sort(evtlist.begin(),evtlist.end(), sortByDateTime);
				}
				channel_id = new_channel_id;

				if (!evtlist.empty()) {
					CChannelEventList::iterator eli;
					for (eli=evtlist.begin(); eli!=evtlist.end(); ++eli) {
						if ((uint)eli->startTime + eli->duration > ts.tv_sec)
							break;
					}
					if (eli == evtlist.end()) {
						nglcd->Epg = nglcd->scrollEpg = "";
						nglcd->Scale = 0;
					} else {
						if (eli->description.compare(nglcd->Epg)) {
							nglcd->Epg = eli->description;
							if (nglcd->font_epg.Width(nglcd->Epg) > nglcd->bitmap->Width() - 4) {
								nglcd->scrollEpg = nglcd->Epg + "    " + nglcd->Epg + "    " + nglcd->Epg;
								nglcd->doScrollEpg = true;
							} else {
								nglcd->scrollEpg = nglcd->Epg;
								nglcd->doScrollEpg = false;
							}
						}

						if (eli->duration > 0)
						nglcd->Scale = (ts.tv_sec - eli->startTime) * 100 / eli->duration;
						if (nglcd->Scale > 100)
							nglcd->Scale = 100;
						else if (nglcd->Scale < 0)
							nglcd->Scale = 0;
					}
				}
			}
		}

		if(!settings.glcd_enable || nglcd->doSuspend || nglcd->doStandby || nglcd->doExit) {
			// for restart, don't blacken screen
			nglcd->bitmap->Clear(GLCD::cColor::Black);
			nglcd->lcd->SetScreen(nglcd->bitmap->Data(), nglcd->bitmap->Width(), nglcd->bitmap->Height());
			nglcd->lcd->Refresh(true);
		}
		if (nglcd->doRescan) {
		    nglcd->doRescan = false;
			nglcd->Update();
	    }
		nglcd->lcd->DeInit();
		delete nglcd->lcd;
		nglcd->lcd = NULL;
	} while(!nglcd->doExit);

	return NULL;
}

void nGLCD::Update() {
	if (nglcd)
		sem_post(&nglcd->sem);
}

void nGLCD::StandbyMode(bool b) {
	if (nglcd) {
		if (settings.glcd_time_in_standby)
			nglcd->doStandbyTime = b;
		else
		    nglcd->doStandby = b;
		nglcd->doMirrorOSD = false;
		nglcd->Update();
	}
}

void nGLCD::ShowVolume(bool b) {
	if (nglcd) {
		nglcd->doShowVolume = b;
		nglcd->Update();
	}
}

void nGLCD::MirrorOSD(bool b) {
	if (nglcd && settings.glcd_mirror_osd) {
		nglcd->doMirrorOSD = b;
		nglcd->Update();
	}
}

void nGLCD::Exit() {
	if (nglcd) {
		nglcd->doMirrorOSD = false;
		nglcd->doSuspend = false;
		nglcd->doExit = true;
		nglcd->Update();
		void *res;
		pthread_join(nglcd->thrGLCD, &res);
		delete nglcd;
	}
}

void nglcd_update() {
	if (nglcd) {
		nglcd->Update();
	}
}

void nGLCD::Rescan() {
	if (nglcd) {
		doRescan = true;
		nglcd->Update();
	}
}

void nGLCD::Suspend() {
	if (nglcd) {
		nglcd->doSuspend = true;
		nglcd->Update();
	}
}

void nGLCD::Resume() {
	if (nglcd) {
		nglcd->doSuspend = false;
		nglcd->Update();
	}
}

void nGLCD::lockChannel(string c)
{
	if(nglcd) {
		nglcd->Lock();
		nglcd->channelLocked = true;
		nglcd->stagingChannel = c;
		nglcd->stagingEpg = "";
		nglcd->Unlock();
		nglcd->Update();
	}
}

void nGLCD::unlockChannel(void)
{
	if(nglcd) {
		nglcd->channelLocked = false;
		nglcd->Update();
	}
}

#define KEY_GLCD_BLACK			0
#define KEY_GLCD_WHITE			1
#define KEY_GLCD_RED			2
#define KEY_GLCD_GREEN			3
#define KEY_GLCD_BLUE			4
#define KEY_GLCD_MAGENTA		5
#define KEY_GLCD_CYAN			6
#define KEY_GLCD_YELLOW			7
#define GLCD_COLOR_OPTION_COUNT 	8
static const CMenuOptionChooser::keyval GLCD_COLOR_OPTIONS[GLCD_COLOR_OPTION_COUNT] =
{
	  { KEY_GLCD_BLACK,	LOCALE_EXTRAMENU_GLCD_COLOR_BLACK }
	, { KEY_GLCD_WHITE,	LOCALE_EXTRAMENU_GLCD_COLOR_WHITE }
	, { KEY_GLCD_RED,	LOCALE_EXTRAMENU_GLCD_COLOR_RED }
	, { KEY_GLCD_GREEN,	LOCALE_EXTRAMENU_GLCD_COLOR_GREEN }
	, { KEY_GLCD_BLUE,	LOCALE_EXTRAMENU_GLCD_COLOR_BLUE }
	, { KEY_GLCD_MAGENTA,	LOCALE_EXTRAMENU_GLCD_COLOR_MAGENTA }
	, { KEY_GLCD_CYAN,	LOCALE_EXTRAMENU_GLCD_COLOR_CYAN }
	, { KEY_GLCD_YELLOW,	LOCALE_EXTRAMENU_GLCD_COLOR_YELLOW }
};

int GLCD_Menu::color2index(uint32_t color) {
	if (color == GLCD::cColor::Black)
		return KEY_GLCD_BLACK;
	if (color == GLCD::cColor::White)
		return KEY_GLCD_WHITE;
	if (color == GLCD::cColor::Red)
		return KEY_GLCD_RED;
	if (color == GLCD::cColor::Green)
		return KEY_GLCD_GREEN;
	if (color == GLCD::cColor::Blue)
		return KEY_GLCD_BLUE;
	if (color == GLCD::cColor::Magenta)
		return KEY_GLCD_MAGENTA;
	if (color == GLCD::cColor::Cyan)
		return KEY_GLCD_CYAN;
	if (color == GLCD::cColor::Yellow)
		return KEY_GLCD_YELLOW;
	return KEY_GLCD_BLACK;
}

uint32_t GLCD_Menu::index2color(int i) {
	switch(i) {
	case KEY_GLCD_BLACK:
		return GLCD::cColor::Black;
	case KEY_GLCD_WHITE:
		return GLCD::cColor::White;
	case KEY_GLCD_RED:
		return GLCD::cColor::Red;
	case KEY_GLCD_GREEN:
		return GLCD::cColor::Green;
	case KEY_GLCD_BLUE:
		return GLCD::cColor::Blue;
	case KEY_GLCD_MAGENTA:
		return GLCD::cColor::Magenta;
	case KEY_GLCD_CYAN:
		return GLCD::cColor::Cyan;
	case KEY_GLCD_YELLOW:
		return GLCD::cColor::Yellow;
	}
	return GLCD::cColor::ERRCOL;
}


GLCD_Menu::GLCD_Menu()
{
	width = w_max (40, 10);
    selected = -1;

	notifier = new GLCD_Menu_Notifier();

	if (!nglcd)
		new nGLCD;
}


int GLCD_Menu::exec(CMenuTarget* parent, const std::string & actionKey)
{
	int res = menu_return::RETURN_REPAINT;
	if(actionKey == "rescan") {
		if (nglcd)
		    nglcd->Rescan();
		return res;
	}
	if(actionKey == "select_font") {
		if(parent)
			parent->hide();
		CFileBrowser fileBrowser;
		CFileFilter fileFilter;
		fileFilter.addFilter("ttf");
		fileBrowser.Filter = &fileFilter;
		if (fileBrowser.exec(FONTDIR) == true) {
			settings.glcd_font = fileBrowser.getSelectedFile()->Name;
			if (nglcd) {
				nglcd->fonts_initialized = false;
				nglcd->Rescan();
			}
		}
		return res;
	}

	if (parent)
		parent->hide();

	GLCD_Menu_Settings();
	saveSettings();

	return res;
}

void GLCD_Menu::hide()
{
}

GLCD_Menu_Notifier::GLCD_Menu_Notifier ()
{
}

bool
GLCD_Menu_Notifier::changeNotify (const neutrino_locale_t OptionName, void *Data)
{
	if (!Data)
		return false;
	switch(OptionName) {
	case LOCALE_EXTRAMENU_GLCD_SELECT_FG:
		settings.glcd_color_fg = GLCD_Menu::index2color(*((int *) Data));
		break;
	case LOCALE_EXTRAMENU_GLCD_SELECT_BG:
		settings.glcd_color_bg = GLCD_Menu::index2color(*((int *) Data));
		break;
	case LOCALE_EXTRAMENU_GLCD_SELECT_BAR:
		settings.glcd_color_bar = GLCD_Menu::index2color(*((int *) Data));
		break;
	case LOCALE_EXTRAMENU_GLCD:
		if (nglcd) {
			if (settings.glcd_enable)
				nglcd->Resume();
			else
				nglcd->Suspend();
			return true;
		}
		break;
	case LOCALE_EXTRAMENU_GLCD_MIRROR_OSD:
		if (nglcd)
				nglcd->doMirrorOSD = settings.glcd_mirror_osd;
		break;
	case LOCALE_EXTRAMENU_GLCD_TIME_IN_STANDBY:
	case LOCALE_EXTRAMENU_GLCD_SIZE_CHANNEL:
	case LOCALE_EXTRAMENU_GLCD_SIZE_EPG:
	case LOCALE_EXTRAMENU_GLCD_SIZE_BAR:
	case LOCALE_EXTRAMENU_GLCD_SIZE_TIME:
		break;
	default:
		return false;
	}

	if (nglcd)
		nglcd->Update();
	return true;
}

void GLCD_Menu::GLCD_Menu_Settings()
{
	int color_bg = color2index(settings.glcd_color_bg);
	int color_fg = color2index(settings.glcd_color_fg);
	int color_bar = color2index(settings.glcd_color_bar);

	CMenuWidget* m = new CMenuWidget(LOCALE_EXTRAMENU_GLCD, NEUTRINO_ICON_SETTINGS, width);
	m->setSelected(selected);
	m->addItem(GenericMenuSeparator);
	m->addItem(GenericMenuBack);
	m->addItem(GenericMenuSeparatorLine);

	m->addItem(new CMenuOptionChooser(LOCALE_EXTRAMENU_GLCD, &settings.glcd_enable,
				ONOFF_OPTIONS, ONOFF_OPTION_COUNT, true, notifier));
	int shortcut = 1;
	m->addItem(GenericMenuSeparatorLine);
	m->addItem(new CMenuOptionChooser(LOCALE_EXTRAMENU_GLCD_SELECT_FG, &color_fg,
				GLCD_COLOR_OPTIONS, GLCD_COLOR_OPTION_COUNT, true, notifier,
				CRCInput::convertDigitToKey(shortcut++)));
	m->addItem(new CMenuOptionChooser(LOCALE_EXTRAMENU_GLCD_SELECT_BG, &color_bg,
				GLCD_COLOR_OPTIONS, GLCD_COLOR_OPTION_COUNT, true, notifier,
				CRCInput::convertDigitToKey(shortcut++)));
	m->addItem(new CMenuOptionChooser(LOCALE_EXTRAMENU_GLCD_SELECT_BAR, &color_bar,
				GLCD_COLOR_OPTIONS, GLCD_COLOR_OPTION_COUNT, true, notifier,
				CRCInput::convertDigitToKey(shortcut++)));
	m->addItem(new CMenuForwarder(LOCALE_EXTRAMENU_GLCD_FONT, true, settings.glcd_font, this, "select_font",
				CRCInput::convertDigitToKey(shortcut++)));
	m->addItem(new CMenuOptionNumberChooser(LOCALE_EXTRAMENU_GLCD_SIZE_CHANNEL,
				&settings.glcd_percent_channel, true, 0, 100, notifier));
	m->addItem(new CMenuOptionNumberChooser(LOCALE_EXTRAMENU_GLCD_SIZE_EPG,
				&settings.glcd_percent_epg, true, 0, 100, notifier));
	m->addItem(new CMenuOptionNumberChooser(LOCALE_EXTRAMENU_GLCD_SIZE_BAR,
				&settings.glcd_percent_bar, true, 0, 100, notifier));
	m->addItem(new CMenuOptionNumberChooser(LOCALE_EXTRAMENU_GLCD_SIZE_TIME,
				&settings.glcd_percent_time, true, 0, 100, notifier));
	m->addItem(GenericMenuSeparatorLine);
	m->addItem(new CMenuOptionChooser(LOCALE_EXTRAMENU_GLCD_TIME_IN_STANDBY, &settings.glcd_time_in_standby,
				ONOFF_OPTIONS, ONOFF_OPTION_COUNT, true, notifier,
				CRCInput::convertDigitToKey(shortcut++)));
	m->addItem(new CMenuOptionChooser(LOCALE_EXTRAMENU_GLCD_MIRROR_OSD, &settings.glcd_mirror_osd,
				ONOFF_OPTIONS, ONOFF_OPTION_COUNT, true, notifier,
				CRCInput::RC_green, NEUTRINO_ICON_BUTTON_GREEN));
	m->addItem(GenericMenuSeparatorLine);
	m->addItem(new CMenuForwarder(LOCALE_EXTRAMENU_GLCD_RESTART, true, "", this, "rescan",
				CRCInput::RC_red, NEUTRINO_ICON_BUTTON_RED));
	m->exec(NULL, "");
	m->hide();
	delete m;

	saveSettings();
}
#endif

KernelOptions_Menu::KernelOptions_Menu()
{
	width = w_max (40, 10);
}

int KernelOptions_Menu::exec(CMenuTarget* parent, const std::string & actionKey)
{
	int res = menu_return::RETURN_REPAINT;

	if (actionKey == "reset") {
		for (int i = 0; i < modules.size(); i++)
			modules[i].active = modules[i].active_orig;
		return res;
	}

	if (actionKey == "apply") {
		for (int i = 0; i < modules.size(); i++)
			if (modules[i].active != modules[i].active_orig) {
				FILE *f = fopen("/etc/modules.extra", "w");
					if (f) {
						chmod("/etc/modules.extra", 0644);
						for (int i = 0; i < modules.size(); i++) {
							if (modules[i].active) {
								for (int j = 0; j < modules[i].moduleList.size(); j++)
									fprintf(f, "%s\n", modules[i].moduleList[j].c_str());
							}
						}
					fclose(f);
				}
				for (int i = 0; i < modules.size(); i++) {
					char buf[80];
					if (modules[i].active)
						for (int j = 0; j < modules[i].moduleList.size(); j++) {
							snprintf(buf, sizeof(buf), "insmod /lib/modules/%s.ko", modules[i].moduleList[j].c_str());
							system(buf);
						}
					else
						for (int j = 0; j < modules[i].moduleList.size(); j++) {
							snprintf(buf, sizeof(buf), "rmmod %s", modules[i].moduleList[j].c_str());
							system(buf);
						}
					modules[i].active_orig = modules[i].active;
				}
				break;
			}
	}

	if (actionKey == "apply" || actionKey == "lsmod") {
		for (int i = 0; i < modules.size(); i++)
			modules[i].installed = false;
		FILE *f = fopen("/proc/modules", "r");
		if (f) {
			char buf[200];
			while (fgets(buf, sizeof(buf), f)) {
				char name[200];
				if (1 == sscanf(buf, "%s", name))
					for (int i = 0; i < modules.size(); i++) {
						if (name == modules[i].moduleList.back()) {
							modules[i].installed = true;
							break;
						}
				}
			}
			fclose(f);
		}

		string text = "";
		for (int i = 0; i < modules.size(); i++) {
			text += modules[i].comment + " (" + modules[i].moduleList.back() + ") ";
			// FIXME, localizations are missing (but rather not worth adding)
			if (modules[i].active) {
				if (modules[i].installed)
					text += " is enabled and loaded\n";
				else
					text += " is enabled but not loaded\n";
			} else {
				if (modules[i].installed)
					text += " is disabled but loaded\n";
				else
					text += " is disabled and not loaded\n";
			}
		}

		ShowHintUTF(LOCALE_EXTRAMENU_KERNELOPTIONS_LSMOD, text.c_str());

		return res;
	}

	if (parent)
		parent->hide();

	Settings();

	return res;
}

void KernelOptions_Menu::hide()
{
}

void KernelOptions_Menu::Settings()
{
	CMenuWidget* menu = new CMenuWidget(LOCALE_EXTRAMENU_KERNELOPTIONS, "settings");
	menu->addItem(GenericMenuSeparator);
	menu->addItem(GenericMenuBack);
	menu->addItem(new CMenuSeparator(CMenuSeparator::LINE | CMenuSeparator::STRING, LOCALE_EXTRAMENU_KERNELOPTIONS_MODULES));

	modules.clear();

	FILE *f = fopen("/etc/modules.available", "r");
	if (f) {
		char buf[200];
		while (fgets(buf, sizeof(buf), f)) {
			if (buf[0] == '#')
				continue;
			char *comment = strchr(buf, '#');
			if (!comment)
				continue;
			*comment++ = 0;
			while (*comment == ' ' || *comment == '\t')
				comment++;
			if (strlen(comment) < 1)
				continue;
			module m;
			m.active = m.active_orig = 0;
			m.installed = false;
			char *nl = strchr(comment, '\n');
			if (nl)
				*nl = 0;
			m.comment = string(comment);
			std::istringstream in(buf);
			std::string s;
			while (in >> s) {
				m.moduleList.push_back(s);
			}
			if (m.moduleList.size() > 0)
				modules.push_back(m);
		}
		fclose(f);
	}

	f = fopen("/etc/modules.extra", "r");
	if (f) {
		char buf[200];
		while (fgets(buf, sizeof(buf), f)) {
			char *t = strchr(buf, '#');
			if (t)
				*t = 0;
			char name[200];
			if (1 == sscanf(buf, "%s", name)) {
				int i;
				for (i = 0; i < modules.size(); i++)
					if (modules[i].moduleList.back() == name) {
						modules[i].active = modules[i].active_orig = 1;
						break;
					}
			}
		}
		fclose(f);
	}

	int shortcut = 0;

	for (int i = 0; i < modules.size(); i++) {
		menu->addItem(new CMenuOptionChooser(modules[i].comment.c_str(), &modules[i].active,
				ONOFF_OPTIONS, ONOFF_OPTION_COUNT, true));
	}

	menu->addItem(GenericMenuSeparatorLine);

	menu->addItem(new CMenuForwarder(LOCALE_EXTRAMENU_KERNELOPTIONS_RESET, true, "", this,
		"reset", CRCInput::RC_red, NEUTRINO_ICON_BUTTON_RED));
	menu->addItem(new CMenuForwarder(LOCALE_EXTRAMENU_KERNELOPTIONS_APPLY, true, "", this,
		"apply", CRCInput::RC_green, NEUTRINO_ICON_BUTTON_GREEN));
	menu->addItem(new CMenuForwarder(LOCALE_EXTRAMENU_KERNELOPTIONS_LSMOD, true, "", this,
		"lsmod", CRCInput::RC_yellow, NEUTRINO_ICON_BUTTON_YELLOW));
	menu->exec (NULL, "");
	menu->hide ();
	delete menu;
}

////////////////////////////// EVOLUXUPDATE Menu START ////////////////////////////////////
CEvoluxSoftwareUpdate::CEvoluxSoftwareUpdate()
{
	width = w_max (40, 10);
}

int CEvoluxSoftwareUpdate::exec(CMenuTarget* parent, const std::string & actionKey)
{
	int res = menu_return::RETURN_REPAINT;
	if(actionKey == "checkupdate") 
	{
		this->CheckUpdate();
		return res;
	}

	if (parent)
		parent->hide();

	Settings();

	return res;
}

void CEvoluxSoftwareUpdate::hide()
{
}

void CEvoluxSoftwareUpdate::Settings()
{
	//MENU AUFBAUEN
	CMenuWidget* menu = new CMenuWidget(LOCALE_EXTRAMENU_EVOLUXUPDATE, "settings");
	menu->addItem(GenericMenuSeparator);
	menu->addItem(GenericMenuBack);
	menu->addItem(GenericMenuSeparatorLine);
	menu->addItem(new CMenuForwarder(LOCALE_EXTRAMENU_EVOLUXUPDATE_UPDATE, true, "", this, "checkupdate", CRCInput::RC_red, NEUTRINO_ICON_BUTTON_RED));
	menu->exec (NULL, "");
	menu->hide ();
	delete menu;
}

bool CEvoluxSoftwareUpdate::CheckUpdate()
{
	//EVOLUXUPDATE STARTEN
	unlink("/tmp/EvoluxUpdatevailable");
	safe_system("oVersion=`wget -q -O - http://tinyurl.com/7gz7jpo | grep version | cut -d = -f2`;"
		   "lVersion=`grep version /etc/.version | cut -d = -f2`;"
		   "[ \"$lVersion\" != \"$oVersion\" ] && touch /tmp/EvoluxUpdatevailable");
	CHintBox *hintBox;
	if(!access("/tmp/EvoluxUpdatevailable", F_OK)) {
		hintBox = new CHintBox(LOCALE_EXTRAMENU_EVOLUXUPDATE_UPDATE, g_Locale->getText(LOCALE_EXTRAMENU_EVOLUXUPDATE_UPDATING));
		hintBox->paint();
		safe_system("wget -O - -q http://tinyurl.com/7fjrnm3 | tee /tmp/update.tar.gz | tar -tzf - && tar -xpzf /tmp/update.tar.gz -C /");
		unlink("/tmp/EvoluxUpdatevailable");
		unlink("/tmp/update.tar.gz");
		hintBox->hide();
		delete hintBox;
		hintBox = new CHintBox(LOCALE_EXTRAMENU_EVOLUXUPDATE_UPDATE, g_Locale->getText(LOCALE_EXTRAMENU_EVOLUXUPDATE_DONE));
	} else
		hintBox = new CHintBox(LOCALE_EXTRAMENU_EVOLUXUPDATE_UPDATE, g_Locale->getText(LOCALE_EXTRAMENU_EVOLUXUPDATE_NOTFOUND));

	hintBox->paint();
	sleep(3);
	hintBox->hide();
	delete hintBox;
}
//ENDE EVOLUXUPDATE

////////////////////////////// EVOLUXUPDATE Menu ENDE //////////////////////////////////////
// vim:ts=4
