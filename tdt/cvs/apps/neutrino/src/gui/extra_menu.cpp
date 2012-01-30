#define __USE_FILE_OFFSET64 1
#include <gui/filebrowser.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/vfs.h>
#include "libnet.h"
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

#include <system/debug.h>

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

#include "bouquetlist.h"
#include "color.h"
#include "eventlist.h"
#include "infoviewer.h"

#include "extra_menu.h"

#define ONOFF_OPTION_COUNT 2

const CMenuOptionChooser::keyval ONOFF_OPTIONS[ONOFF_OPTION_COUNT] = {
	{ 0, LOCALE_ONOFF_OFF },
	{ 1, LOCALE_ONOFF_ON },
};

static int touch(const char *filename) {
	int fn = open(filename, O_RDWR | O_CREAT, 0644);
	if (fn > -1) {
		close(fn);
		return -1;
	}
	return 0;
}

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


static struct {
#define EXTRA_CAM_SELECTED "cam_selected"
        std::string	cam_selected;
#ifdef WITH_GRAPHLCD
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
#endif
} settings;

CConfigFile *configfile = NULL;

static bool saveSettings() {
	if (configfile) {
		configfile->setString(EXTRA_CAM_SELECTED, settings.cam_selected);
#ifdef WITH_GRAPHLCD
		configfile->setInt32(GLCD_ENABLE, settings.glcd_enable);
		configfile->setInt32(GLCD_COLOR_FG, settings.glcd_color_fg);
		configfile->setInt32(GLCD_COLOR_BG, settings.glcd_color_bg);
		configfile->setInt32(GLCD_COLOR_BAR, settings.glcd_color_bar);
		configfile->setInt32(GLCD_SIZE_CHANNEL, settings.glcd_percent_channel);
		configfile->setInt32(GLCD_SIZE_EPG, settings.glcd_percent_epg);
		configfile->setInt32(GLCD_SIZE_BAR, settings.glcd_percent_bar);
		configfile->setInt32(GLCD_SIZE_TIME, settings.glcd_percent_time);
		configfile->setInt32(GLCD_MIRROR_OSD, settings.glcd_mirror_osd);
		configfile->setString(GLCD_FONT, settings.glcd_font);
#endif
		configfile->saveConfig(EXTRA_SETTINGS_FILE);
		return true;
	}
	return false;
}

static bool initSettings() {
	settings.cam_selected = "disabled";
#ifdef WITH_GRAPHLCD
	settings.glcd_enable = 0;
	settings.glcd_color_fg = GLCD::cColor::White;
	settings.glcd_color_bg = GLCD::cColor::Blue;
	settings.glcd_color_bar = GLCD::cColor::Red;
	settings.glcd_percent_channel = 18;
	settings.glcd_percent_epg = 8;
	settings.glcd_percent_bar = 6;
	settings.glcd_percent_time = 22;
	settings.glcd_mirror_osd = 0;
	settings.glcd_font = FONTDIR "/neutrino.ttf";
#endif
}

static bool loadSettings() {
	if (!configfile) {
		initSettings();
		configfile = new CConfigFile('=');
		if (configfile->loadConfig(EXTRA_SETTINGS_FILE)) {
			settings.cam_selected = configfile->getString(EXTRA_CAM_SELECTED, "disabled");
#ifdef WITH_GRAPHLCD
			settings.glcd_enable = configfile->getInt32(GLCD_ENABLE, 0);
			settings.glcd_color_fg = configfile->getInt32(GLCD_COLOR_FG, GLCD::cColor::White);
			settings.glcd_color_bg = configfile->getInt32(GLCD_COLOR_BG, GLCD::cColor::Blue);
			settings.glcd_color_bar = configfile->getInt32(GLCD_COLOR_BAR, GLCD::cColor::Red);
			settings.glcd_percent_channel = configfile->getInt32(GLCD_SIZE_CHANNEL, 18);
			settings.glcd_percent_epg = configfile->getInt32(GLCD_SIZE_EPG, 8);
			settings.glcd_percent_bar = configfile->getInt32(GLCD_SIZE_BAR, 6);
			settings.glcd_percent_time = configfile->getInt32(GLCD_SIZE_TIME, 22);
			settings.glcd_mirror_osd = configfile->getInt32(GLCD_MIRROR_OSD, 0);
			settings.glcd_font = configfile->getString(GLCD_FONT, FONTDIR "/neutrino.ttf");
#endif
			return true;
		}
	}
	return false;
}

# if 0 /* unused code */
////////////////////////////// CORRECTVOLUME Menu ANFANG ////////////////////////////////////
#define CORRECTVOLUME_OPTION_COUNT 2
const CMenuOptionChooser::keyval CORRECTVOLUME_OPTIONS[CORRECTVOLUME_OPTION_COUNT] =
{
	{ 0, LOCALE_EXTRAMENU_CORRECTVOLUME_OFF },
	{ 1, LOCALE_EXTRAMENU_CORRECTVOLUME_ON },
};

CORRECTVOLUME_Menu::CORRECTVOLUME_Menu()
{
	frameBuffer = CFrameBuffer::getInstance();
	width = 600;
	hheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight();
	mheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight();
	height = hheight+13*mheight+ 10;

	x=(((g_settings.screen_EndX- g_settings.screen_StartX)-width) / 2) + g_settings.screen_StartX;
	y=(((g_settings.screen_EndY- g_settings.screen_StartY)-height) / 2) + g_settings.screen_StartY;
}
int CORRECTVOLUME_Menu::exec(CMenuTarget* parent, const std::string & actionKey)
{
	int res = menu_return::RETURN_REPAINT;

	if (parent)
	{
	parent->hide();
	}

	CORRECTVOLUMESettings();

	return res;
}

void CORRECTVOLUME_Menu::hide()
{
frameBuffer->paintBackgroundBoxRel(x,y, width,height);
}

void CORRECTVOLUME_Menu::CORRECTVOLUMESettings()
{
	int corrVol=0;
	int save_value=0;
	//UEBERPRUEFEN OB CORRECTVOLUME SCHON LAEUFT
	FILE* fd1 = fopen("/etc/.corrVol", "r");
	if(fd1)
	{
	corrVol=1;
	fclose(fd1);
	}
	int old_corrVol=corrVol;
	//MENU AUFBAUEN
	CMenuWidget* menu = new CMenuWidget(LOCALE_EXTRAMENU_CORRECTVOLUME, "settings.raw");
	menu->addItem(GenericMenuSeparator);
	menu->addItem(GenericMenuBack);
	menu->addItem(GenericMenuSeparatorLine);
	CMenuOptionChooser* oj1 = new CMenuOptionChooser(LOCALE_EXTRAMENU_CORRECTVOLUME_SELECT, &corrVol, CORRECTVOLUME_OPTIONS, CORRECTVOLUME_OPTION_COUNT,true);
	menu->addItem( oj1 );
	menu->exec (NULL, "");
	menu->hide ();
	delete menu;
	// UEBERPRUEFEN OB SICH WAS GEAENDERT HAT
	if (old_corrVol!=corrVol)
	{
	save_value=1;
	}
	// ENDE UEBERPRUEFEN OB SICH WAS GEAENDERT HAT

	// AUSFUEHREN NUR WENN SICH WAS GEAENDERT HAT
	if (save_value==1)
	{
	if (corrVol==1)
	{
	//CORRECTVOLUME STARTEN
	system("touch /etc/.corrVol");
	safe_system("/etc/init.d/corrVol.sh &");
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "CORRECTVOLUME Activated!", 450, 2); // UTF-8("")
	}
	if (corrVol==0)
	{
	//CORRECTVOLUME BEENDEN
	system("rm /etc/.corrVol");
	system("killall -9 corrVol.sh");
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "CORRECTVOLUME Deactivated!", 450, 2); // UTF-8("")
	}
}
//ENDE CORRECTVOLUME
}
////////////////////////////// CORRECTVOLUME Menu ENDE //////////////////////////////////////
#endif

////////////////////////////// TUNERRESET Menu ANFANG ////////////////////////////////////
#define TUNERRESET_OPTION_COUNT 2
const CMenuOptionChooser::keyval TUNERRESET_OPTIONS[TUNERRESET_OPTION_COUNT] =
{
	{ 0, LOCALE_EXTRAMENU_TUNERRESET_OFF },
	{ 1, LOCALE_EXTRAMENU_TUNERRESET_ON },
};

TUNERRESET_Menu::TUNERRESET_Menu()
{
	frameBuffer = CFrameBuffer::getInstance();
	width = 600;
	hheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight();
	mheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight();
	height = hheight+13*mheight+ 10;

	x=(((g_settings.screen_EndX- g_settings.screen_StartX)-width) / 2) + g_settings.screen_StartX;
	y=(((g_settings.screen_EndY- g_settings.screen_StartY)-height) / 2) + g_settings.screen_StartY;
}

int TUNERRESET_Menu::exec(CMenuTarget* parent, const std::string & actionKey)
{
	int res = menu_return::RETURN_REPAINT;
	if(actionKey == "tunerreset") 
	{
		this->TunerReset();
		return res;
	}

	if (parent)
		parent->hide();

	TUNERRESETSettings();

	return res;
}

void TUNERRESET_Menu::hide()
{
	frameBuffer->paintBackgroundBoxRel(x,y, width,height);
}

void TUNERRESET_Menu::TUNERRESETSettings()
{
	//MENU AUFBAUEN
	CMenuWidget* menu = new CMenuWidget(LOCALE_EXTRAMENU_TUNERRESET, "settings.raw");
	menu->addItem(GenericMenuSeparator);
	menu->addItem(GenericMenuBack);
	menu->addItem(GenericMenuSeparatorLine);
	menu->addItem(new CMenuForwarder(LOCALE_EXTRAMENU_TUNERRESET_RESTART, true, "", this, "tunerreset", CRCInput::RC_red, NEUTRINO_ICON_BUTTON_RED));
	menu->exec (NULL, "");
	menu->hide ();
	delete menu;
}
bool TUNERRESET_Menu::TunerReset()
{
	//TUNERRESET STARTEN
	CHintBox * TunerResetBox = new CHintBox(LOCALE_EXTRAMENU_TUNERRESET_RESTART, "bitte warten, Tuner wird resettet");
	TunerResetBox->paint();

	safe_system("/usr/local/bin/pzapit -esb ; sleep 2 ; /usr/local/bin/pzapit -lsb");
	TunerResetBox->hide();
	delete TunerResetBox;
}
//ENDE TUNERRESET

////////////////////////////// TUNERRESET Menu ENDE //////////////////////////////////////

# if 0 /* unused code */
////////////////////////////// AMOUNT Menu ANFANG ////////////////////////////////////
#define AMOUNT_OPTION_COUNT 2
const CMenuOptionChooser::keyval AMOUNT_OPTIONS[AMOUNT_OPTION_COUNT] =
{
	{ 0, LOCALE_EXTRAMENU_AMOUNT_OFF },
	{ 1, LOCALE_EXTRAMENU_AMOUNT_ON },
};

AMOUNT_Menu::AMOUNT_Menu()
{
	frameBuffer = CFrameBuffer::getInstance();
	width = 600;
	hheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight();
	mheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight();
	height = hheight+13*mheight+ 10;

	x=(((g_settings.screen_EndX- g_settings.screen_StartX)-width) / 2) + g_settings.screen_StartX;
	y=(((g_settings.screen_EndY- g_settings.screen_StartY)-height) / 2) + g_settings.screen_StartY;
}
int AMOUNT_Menu::exec(CMenuTarget* parent, const std::string & actionKey)
{
	int res = menu_return::RETURN_REPAINT;

	if (parent)
	{
		parent->hide();
	}

	AMOUNTSettings();

	return res;
}

void AMOUNT_Menu::hide()
{
	frameBuffer->paintBackgroundBoxRel(x,y, width,height);
}

void AMOUNT_Menu::AMOUNTSettings()
{
	int amount=0;
	int save_value=0;
	//UEBERPRUEFEN OB AMOUNT SCHON LAEUFT
	FILE* fd1 = fopen("/etc/.byLabel", "r");
	FILE* fd2 = fopen("/etc/.byDev", "r");
	if(fd1)
	{
	amount=1;
	fclose(fd1);
	}
	if(fd2)
	{
	amount=1;
	fclose(fd2);
	}
	int old_amount=amount;
	//MENU AUFBAUEN
	CMenuWidget* menu = new CMenuWidget(LOCALE_EXTRAMENU_AMOUNT, "settings.raw");
	menu->addItem(GenericMenuSeparator);
	menu->addItem(GenericMenuBack);
	menu->addItem(GenericMenuSeparatorLine);
	CMenuOptionChooser* oj1 = new CMenuOptionChooser(LOCALE_EXTRAMENU_AMOUNT_SELECT, &amount, AMOUNT_OPTIONS, AMOUNT_OPTION_COUNT,true);
	menu->addItem( oj1 );
	menu->exec (NULL, "");
	menu->hide ();
	delete menu;
	// UEBERPRUEFEN OB SICH WAS GEAENDERT HAT
	if (old_amount!=amount)
	{
	save_value=1;
	}
	// ENDE UEBERPRUEFEN OB SICH WAS GEAENDERT HAT

	// AUSFUEHREN NUR WENN SICH WAS GEAENDERT HAT
	if (save_value==1)
	{
	if (amount==1)
	{
	//AMOUNT STARTEN
	system("touch /etc/.byLabel");
	safe_system("/etc/init.d/amount.sh &");
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "AMOUNT byLabel Activated!", 450, 2); // UTF-8("")
	}
	if (amount==0)
	{
	//AMOUNT BEENDEN
	system("rm /etc/.byLabel");
	system("rm /etc/.byDev");
//	system("killall -9 amount.sh");
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "AMOUNT byLabel Deactivated!\nPlease REBOOT", 450, 2); // UTF-8("")
	}
}
//ENDE AMOUNT
}
////////////////////////////// AMOUNT Menu ENDE //////////////////////////////////////
#endif

#if 0
////////////////////////////// CheckFS Menu ANFANG ////////////////////////////////////
#define CHECKFS_OPTION_COUNT 2
const CMenuOptionChooser::keyval CHECKFS_OPTIONS[CHECKFS_OPTION_COUNT] =
{
	{ 0, LOCALE_EXTRAMENU_CHECKFS_OFF },
	{ 1, LOCALE_EXTRAMENU_CHECKFS_ON },
};

CHECKFS_Menu::CHECKFS_Menu()
{
	frameBuffer = CFrameBuffer::getInstance();
	width = 600;
	hheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight();
	mheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight();
	height = hheight+13*mheight+ 10;

	x=(((g_settings.screen_EndX- g_settings.screen_StartX)-width) / 2) + g_settings.screen_StartX;
	y=(((g_settings.screen_EndY- g_settings.screen_StartY)-height) / 2) + g_settings.screen_StartY;
}

int CHECKFS_Menu::exec(CMenuTarget* parent, const std::string & actionKey)
{
	int res = menu_return::RETURN_REPAINT;

	if (parent)
		parent->hide();

	CHECKFSSettings();

	return res;
}

void CHECKFS_Menu::hide()
{
	frameBuffer->paintBackgroundBoxRel(x,y, width,height);
}

void CHECKFS_Menu::CHECKFSSettings()
{
#define DOTFILE_CHECKFS "/etc/.checkfs"
	int checkfs = access(DOTFILE_CHECKFS, F_OK) ? 0 : 1;
	int old_checkfs=checkfs;
	//MENU AUFBAUEN
	CMenuWidget* menu = new CMenuWidget(LOCALE_EXTRAMENU_CHECKFS, "settings.raw");
	menu->addItem(GenericMenuSeparator);
	menu->addItem(GenericMenuBack);
	menu->addItem(GenericMenuSeparatorLine);
	CMenuOptionChooser* oj1 = new CMenuOptionChooser(LOCALE_EXTRAMENU_CHECKFS_SELECT, &checkfs, CHECKFS_OPTIONS, CHECKFS_OPTION_COUNT,true);
	menu->addItem( oj1 );
	menu->exec (NULL, "");
	menu->hide ();
	delete menu;
	// UEBERPRUEFEN OB SICH WAS GEAENDERT HAT
	if (old_checkfs!=checkfs)
	{
		if (checkfs==1)
		{
			//CHECKFS STARTEN
			touch(DOTFILE_CHECKFS);
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "CHECKFS Activated!", 450, 2); // UTF-8("")
		} else {
			//CHECKFS BEENDEN
			unlink(DOTFILE_CHECKFS);
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "CHECKFS Deactivated!", 450, 2); // UTF-8("")
		}
	}
}
////////////////////////////// CheckFS Menu ENDE //////////////////////////////////////
#endif

////////////////////////////// EMU choose Menu ANFANG ////////////////////////////////////

#define EMU_OPTION_COUNT 7
static EMU_Menu::emu_list EMU_list[EMU_OPTION_COUNT] =
{
	  { "disabled" }
	, { "mgcamd", "rm -f /tmp/camd.socket >/dev/null; /usr/bin/mgcamd /var/keys/mg_cfg >/dev/null 2>&1 &",
		"kill -9 $(pidof mgcamd)", false }
	, { "incubusCamd", "rm -f /tmp/camd.socket 2>/dev/null ; /usr/bin/incubusCamd >/dev/null 2>&1 &",
		"kill -9 $(pidof incubusCamd)", false }
	, { "camd3", "/usr/bin/camd3 /var/keys/camd3.config >/dev/null 2>&1 &",
		"kill -9 $(pidof camd3)", false }
	, { "mbox", "/usr/bin/mbox /var/keys/mbox.cfg >/dev/null 2>&1 &"
		"kill -9 $(pidof mbox) ; rm -f /tmp/share.* /tmp/mbox.ver /tmp/*.info 2>/dev/null", false }
	, { "oscam", "/usr/bin/oscam -b -c /var/keys >/dev/null 2>&1 &", "kill -9 $(pidof oscam)", false }
	, { "spcs", "/usr/bin/spcs -c /var/keys >/dev/null 2>&1 &", "kill -9 $(pidof spcs)", false }
};

int EMU_Menu::get_installed_count() {
	return installed_count;
}

int EMU_Menu::update_installed()
{
	installed_count = 0;

	for (int i = 1; i < EMU_OPTION_COUNT; i++) {
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
	for (int i = 1; i < EMU_OPTION_COUNT && !selected; i++)
		if (!settings.cam_selected.compare(EMU_list[i].procname) && EMU_list[i].installed)
			selected = i;
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
	suspended = false;
	selected = 0;

	frameBuffer = CFrameBuffer::getInstance();
	width = 600;
	hheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight();
	mheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight();
	height = hheight+13*mheight+ 10;

	x=(((g_settings.screen_EndX- g_settings.screen_StartX)-width) / 2) + g_settings.screen_StartX;
	y=(((g_settings.screen_EndY- g_settings.screen_StartY)-height) / 2) + g_settings.screen_StartY;

	loadSettings();
	update_installed();
	update_selected();

	if (selected > 0){
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
}

int EMU_Menu::exec(CMenuTarget* parent, const std::string & actionKey)
{
	int res = menu_return::RETURN_REPAINT;

	bool doReset = false;
	int emu = selected;

	if(actionKey == "disable")
		emu = 0;
	else if (actionKey == "reset") {
		doReset = true;
		emu = selected;
	} else
		for (emu = 1; emu < EMU_OPTION_COUNT; emu++)
			if (!strcmp(EMU_list[emu].procname, actionKey.c_str()))
				break;

	if (emu < EMU_OPTION_COUNT) {
		int emu_old = selected;
		if ((emu_old != emu) || doReset) {
			if (emu_old) {
				safe_system(EMU_list[emu_old].stop_command);
				string m = " " + string(EMU_list[emu_old].procname) + " is now inactive ";
				ShowHintUTF(LOCALE_MESSAGEBOX_INFO, m.c_str(), 450, 2); // UTF-8("")
			}
			if (emu) {
				safe_system(EMU_list[emu].start_command);

				string cmd = "(" + string(EMU_list[emu].start_command);
				if (is_scrambled())
					safe_system("sleep 2; /usr/local/bin/pzapit -rz >/dev/null 2>&1");
				string m = " " + string(EMU_list[emu].procname) + " is now active ";
				ShowHintUTF(LOCALE_MESSAGEBOX_INFO, m.c_str(), 450, 2); // UTF-8("")
			}
			settings.cam_selected = string(EMU_list[emu].procname);
			selected = emu;
		}

		g_RCInput->postMsg(CRCInput::RC_red, 0); // reenter menu // FIXME?
		return menu_return::RETURN_EXIT_ALL;
	}

	if (parent)
		parent->hide();

	EMU_Menu_Settings();

	return res;
}

void EMU_Menu::hide()
{
	frameBuffer->paintBackgroundBoxRel(x,y, width,height);
}

void EMU_Menu::EMU_Menu_Settings()
{
	int emu = selected;
	int emu_old = emu;

	update_installed();

	CMenuWidget* menu = new CMenuWidget(LOCALE_EXTRAMENU_EMU, "settings.raw");
	menu->addItem(GenericMenuSeparator);
	menu->addItem(GenericMenuBack);
	menu->addItem(GenericMenuSeparatorLine);

	int shortcut = 1;
	for (int i = 1; i < EMU_OPTION_COUNT; i++)
		if (EMU_list[i].installed)
			menu->addItem(new CMenuForwarderNonLocalized(EMU_list[i].procname, true,
				(i == selected) ? g_Locale->getText(LOCALE_ONOFF_ON)
						: g_Locale->getText(LOCALE_ONOFF_OFF),
				this, EMU_list[i].procname, CRCInput::convertDigitToKey(shortcut++)),
				(i == selected));

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
	if (selected && !suspended) {
		safe_system(EMU_list[selected].stop_command);
		suspended = true;
	}
}

void EMU_Menu::resume()
{
	if (selected && suspended) {
		safe_system(EMU_list[selected].start_command);
		if (is_scrambled())
			safe_system("sleep 2; /usr/local/bin/pzapit -rz >/dev/null 2>&1");
		suspended = false;
	}
}

////////////////////////////// EMU Menu ENDE //////////////////////////////////////

////////////////////////////// DISPLAYTIME Menu ANFANG ////////////////////////////////////
#define DISPLAYTIME_OPTION_COUNT 2
const CMenuOptionChooser::keyval DISPLAYTIME_OPTIONS[DISPLAYTIME_OPTION_COUNT] =
{
	{ 0, LOCALE_EXTRAMENU_DISPLAYTIME_OFF },
	{ 1, LOCALE_EXTRAMENU_DISPLAYTIME_ON },
};

DISPLAYTIME_Menu::DISPLAYTIME_Menu()
{
	frameBuffer = CFrameBuffer::getInstance();
	width = 600;
	hheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight();
	mheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight();
	height = hheight+13*mheight+ 10;

	x=(((g_settings.screen_EndX- g_settings.screen_StartX)-width) / 2) + g_settings.screen_StartX;
	y=(((g_settings.screen_EndY- g_settings.screen_StartY)-height) / 2) + g_settings.screen_StartY;
}
int DISPLAYTIME_Menu::exec(CMenuTarget* parent, const std::string & actionKey)
{
	int res = menu_return::RETURN_REPAINT;

	if (parent)
		parent->hide();

	DISPLAYTIMESettings();

	return res;
}

void DISPLAYTIME_Menu::hide()
{
	frameBuffer->paintBackgroundBoxRel(x,y, width,height);
}

void DISPLAYTIME_Menu::DISPLAYTIMESettings()
{
#define DOTFILE_DISPLAYTIME "/etc/.time"
	int displaytime = access(DOTFILE_DISPLAYTIME, F_OK) ? 0 : 1;
	int old_displaytime=displaytime;
	//MENU AUFBAUEN
	CMenuWidget* menu = new CMenuWidget(LOCALE_EXTRAMENU_DISPLAYTIME, "settings.raw");
	menu->addItem(GenericMenuSeparator);
	menu->addItem(GenericMenuBack);
	menu->addItem(GenericMenuSeparatorLine);
	CMenuOptionChooser* oj1 = new CMenuOptionChooser(LOCALE_EXTRAMENU_DISPLAYTIME_SELECT, &displaytime, DISPLAYTIME_OPTIONS, DISPLAYTIME_OPTION_COUNT,true);
	menu->addItem( oj1 );
	menu->exec (NULL, "");
	menu->hide ();
	delete menu;
	// UEBERPRUEFEN OB SICH WAS GEAENDERT HAT
	if (old_displaytime!=displaytime)
	{
		if (displaytime==1)
		{
			//DisplayTime STARTEN
			touch("/etc/.time");
			safe_system("/etc/init.d/DisplayTime.sh >/dev/null 2>&1 &");
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "DISPLAYTIME Activated!", 450, 2); // UTF-8("")
		} else {
			//DisplayTime BEENDEN
			unlink("/etc/.time");
			safe_system("killall -9 DisplayTime.sh");
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "DISPLAYTIME Deactivated!", 450, 2); // UTF-8("")
		}
	}
}
////////////////////////////// DisplayTime Menu ENDE //////////////////////////////////////

#if 0 /* unused code */
////////////////////////////// WWWDATE Menu ANFANG ////////////////////////////////////
#define WWWDATE_OPTION_COUNT 2
const CMenuOptionChooser::keyval WWWDATE_OPTIONS[WWWDATE_OPTION_COUNT] =
{
	{ 0, LOCALE_EXTRAMENU_WWWDATE_OFF },
	{ 1, LOCALE_EXTRAMENU_WWWDATE_ON },
};

WWWDATE_Menu::WWWDATE_Menu()
{
	frameBuffer = CFrameBuffer::getInstance();
	width = 600;
	hheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight();
	mheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight();
	height = hheight+13*mheight+ 10;

	x=(((g_settings.screen_EndX- g_settings.screen_StartX)-width) / 2) + g_settings.screen_StartX;
	y=(((g_settings.screen_EndY- g_settings.screen_StartY)-height) / 2) + g_settings.screen_StartY;
}
int WWWDATE_Menu::exec(CMenuTarget* parent, const std::string & actionKey)
{
	int res = menu_return::RETURN_REPAINT;

	if (parent)
		parent->hide();

	WWWDATESettings();

	return res;
}

void WWWDATE_Menu::hide()
{
	frameBuffer->paintBackgroundBoxRel(x,y, width,height);
}

void WWWDATE_Menu::WWWDATESettings()
{
	int wwwdate=0;
	int save_value=0;
	//UEBERPRUEFEN OB WWWDATE SCHON LAEUFT
	FILE* fd1 = fopen("/etc/.wwwdate", "r");
	if(fd1)
	{
		wwwdate=1;
		fclose(fd1);
	}
	int old_wwwdate=wwwdate;
	//MENU AUFBAUEN
	CMenuWidget* menu = new CMenuWidget(LOCALE_EXTRAMENU_WWWDATE, "settings.raw");
	menu->addItem(GenericMenuSeparator);
	menu->addItem(GenericMenuBack);
	menu->addItem(GenericMenuSeparatorLine);
	CMenuOptionChooser* oj1 = new CMenuOptionChooser(LOCALE_EXTRAMENU_WWWDATE_SELECT, &wwwdate, WWWDATE_OPTIONS, WWWDATE_OPTION_COUNT,true);
	menu->addItem( oj1 );
	menu->exec (NULL, "");
	menu->hide ();
	delete menu;
	// UEBERPRUEFEN OB SICH WAS GEAENDERT HAT
	if (old_wwwdate!=wwwdate)
	{
		save_value=1;
	}
	// ENDE UEBERPRUEFEN OB SICH WAS GEAENDERT HAT

	// AUSFUEHREN NUR WENN SICH WAS GEAENDERT HAT
	if (save_value==1)
	{
		if (wwwdate==1)
		{
			//WWWDATE STARTEN
			system("touch /etc/.wwwdate");
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "WWWDATE Activated!", 450, 2); // UTF-8("")
		}
		if (wwwdate==0)
		{
			//WWWDATE BEENDEN
			system("rm /etc/.wwwdate");
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "WWWDATE Deactivated!", 450, 2); // UTF-8("")
		}
	}
	//ENDE WWWDATE
}
////////////////////////////// WWWDATE Menu ENDE //////////////////////////////////////
#endif

////////////////////////////// SWAP choose Menu ANFANG ////////////////////////////////////

#define SWAP_OPTION_COUNT 3
const CMenuOptionChooser::keyval SWAP_OPTIONS[SWAP_OPTION_COUNT] =
{
#define KEY_SWAP_SWAPRAM 0
#define KEY_SWAP_SWAPPART 1
#define KEY_SWAP_SWAPFILE 2
	{ KEY_SWAP_SWAPRAM, LOCALE_EXTRAMENU_SWAP_SWAPRAM },
	{ KEY_SWAP_SWAPPART, LOCALE_EXTRAMENU_SWAP_SWAPPART },
	{ KEY_SWAP_SWAPFILE, LOCALE_EXTRAMENU_SWAP_SWAPFILE }
};

SWAP_Menu::SWAP_Menu()
{
	frameBuffer = CFrameBuffer::getInstance();
	width = 600;
	hheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight();
	mheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight();
	height = hheight+13*mheight+ 10;

	x=(((g_settings.screen_EndX- g_settings.screen_StartX)-width) / 2) + g_settings.screen_StartX;
	y=(((g_settings.screen_EndY- g_settings.screen_StartY)-height) / 2) + g_settings.screen_StartY;
}

int SWAP_Menu::exec(CMenuTarget* parent, const std::string & actionKey)
{
	int res = menu_return::RETURN_REPAINT;

	if (parent)
		parent->hide();

	SWAP_Menu_Settings();

	return res;
}

void SWAP_Menu::hide()
{
	frameBuffer->paintBackgroundBoxRel(x,y, width,height);
}

string SWAP_Menu::start_swap(int swaptype) {
	switch(swaptype) {
		case KEY_SWAP_SWAPRAM:
			safe_system("/etc/init.d/Swap.sh >/dev/null 2>&1 &");
			return "SWAP-RAM activated";
		case KEY_SWAP_SWAPPART:
			safe_system("/etc/init.d/Swap.sh >/dev/null 2>&1 &");
			return "SWAP partition activated";
		case KEY_SWAP_SWAPFILE:
			safe_system("/etc/init.d/Swap.sh >/dev/null 2>&1 &");
			return "SWAP file activated";
		default:
			return "Internal error.";
	}
}

string SWAP_Menu::stop_swap(int swaptype) {
	switch(swaptype) {
		case KEY_SWAP_SWAPRAM:
			system("swapoff -a");
			return "SWAPRAM deactivated";
		case KEY_SWAP_SWAPPART:
			system("swapoff -a");
			return "SWAP partition deactivated";
		case KEY_SWAP_SWAPFILE:
			system("swapoff /dev/loop0 ; losetup -d /dev/loop0 ; swapoff -a");
			return "SWAP file deactivated";
		default:
			return "Internal error.";
	}
}

#define DOTFILE_SWAPON  "/etc/.swapon"
#define DOTFILE_SWAPRAM "/etc/.swapram"
#define DOTFILE_SWAPPART "/etc/.swappart"
#define DOTFILE_SWAPFILE "/etc/.swapfile"

void SWAP_Menu::unlink_dotfile(int swaptype) {
	switch(swaptype) {
		case KEY_SWAP_SWAPRAM:
			unlink(DOTFILE_SWAPRAM);
			return;
		case KEY_SWAP_SWAPPART:
			unlink(DOTFILE_SWAPPART);
			return;
		case KEY_SWAP_SWAPFILE:
			unlink(DOTFILE_SWAPFILE);
		default:
			return;
	}
}

void SWAP_Menu::touch_dotfile(int swaptype) {
	switch(swaptype) {
		case KEY_SWAP_SWAPRAM:
			touch(DOTFILE_SWAPRAM);
			return;
		case KEY_SWAP_SWAPPART:
			touch(DOTFILE_SWAPPART);
			return;
		case KEY_SWAP_SWAPFILE:
			touch(DOTFILE_SWAPFILE);
		default:
			return;
	}
}

void SWAP_Menu::SWAP_Menu_Settings()
{
	int swap_onoff = access(DOTFILE_SWAPON, F_OK) ? 0 : 1;
	int swap=0;

	if (!access(DOTFILE_SWAPRAM, F_OK))
		swap = KEY_SWAP_SWAPRAM;
	else if (!access(DOTFILE_SWAPPART, F_OK))
		swap = KEY_SWAP_SWAPPART;
	else if (!access(DOTFILE_SWAPFILE, F_OK))
		swap = KEY_SWAP_SWAPFILE;

	int old_swap=swap;
	int old_swap_onoff=swap_onoff;

	//MENU AUFBAUEN
	CMenuWidget* menu = new CMenuWidget(LOCALE_EXTRAMENU_SWAP, "settings.raw");
	menu->addItem(GenericMenuSeparator);
	menu->addItem(GenericMenuBack);
	menu->addItem(GenericMenuSeparatorLine);
	CMenuOptionChooser* oj1 = new CMenuOptionChooser(LOCALE_EXTRAMENU_SWAP_ONOFF, &swap_onoff, ONOFF_OPTIONS, ONOFF_OPTION_COUNT,true);
	menu->addItem( oj1 );
	menu->addItem(GenericMenuSeparatorLine);
	CMenuOptionChooser* oj2 = new CMenuOptionChooser(LOCALE_EXTRAMENU_SWAP_SELECT, &swap, SWAP_OPTIONS, SWAP_OPTION_COUNT,true);
	menu->addItem( oj2 );
	menu->exec (NULL, "");
	menu->hide ();
	delete menu;

	// UEBERPRUEFEN OB SICH WAS GEAENDERT HAT
	if ((old_swap != swap) && (swap_onoff == 1)) {
		if (old_swap_onoff == 0)
			stop_swap(old_swap);
		else
			unlink_dotfile(old_swap);
		touch_dotfile(swap);
		touch(DOTFILE_SWAPON);
		ShowHintUTF(LOCALE_MESSAGEBOX_INFO, start_swap(swap).c_str(), 450, 2); // UTF-8("")
	} else if ((old_swap!=swap) && (swap_onoff == 0)) {
		if (old_swap_onoff == 1) {
			unlink(DOTFILE_SWAPON);
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, stop_swap(old_swap).c_str(), 450, 2); // UTF-8("")
		}
		unlink_dotfile(old_swap);
		touch_dotfile(swap);
	} else if ((old_swap == swap) && (swap_onoff == 1)) {
		if (old_swap_onoff == 0) {
			touch_dotfile(swap); // might be the default value
			touch(DOTFILE_SWAPON);
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, start_swap(swap).c_str(), 450, 2); // UTF-8("")
		}
	} else if ((old_swap == swap) && (swap_onoff == 0)) {
		if (old_swap_onoff == 1) {
			unlink(DOTFILE_SWAPON);
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, stop_swap(swap).c_str(), 450, 2); // UTF-8("")
		}
	}
}

////////////////////////////// SWAP Menu ENDE //////////////////////////////////////

////////////////////////////// BOOT Menu ANFANG ////////////////////////////////////
#define BOOT_OPTION_COUNT 3
const CMenuOptionChooser::keyval BOOT_OPTIONS[BOOT_OPTION_COUNT] =
{
#define BOOT_NEUTRINO 0
#define BOOT_E2       1
#define BOOT_SPARK    2
#if 0
	{ BOOT_NEUTRINO, NONEXISTANT_LOCALE, "Neutrino" },
#else
	{ BOOT_NEUTRINO, LOCALE_EXTRAMENU_BOOT_UNCHANGED },
#endif
	{ BOOT_E2, NONEXISTANT_LOCALE, "E2" },
	{ BOOT_SPARK, NONEXISTANT_LOCALE, "Spark" }
};

BOOT_Menu::BOOT_Menu()
{
	frameBuffer = CFrameBuffer::getInstance();
	width = 500;
	hheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight();
	mheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight();
	height = hheight+13*mheight+ 10;

	x=(((g_settings.screen_EndX- g_settings.screen_StartX)-width) / 2) + g_settings.screen_StartX;
	y=(((g_settings.screen_EndY- g_settings.screen_StartY)-height) / 2) + g_settings.screen_StartY;
}

int BOOT_Menu::exec(CMenuTarget* parent, const std::string & actionKey)
{
	int res = menu_return::RETURN_REPAINT;
	if(actionKey == "reboot") {
                FILE *f = fopen("/tmp/.reboot", "w");
                fclose(f);
		CNeutrinoApp::getInstance()->ExitRun(true, 2);
		// not reached, hopefully...
                unlink("/tmp/.reboot");
		return res;
	}

	if (parent)
		parent->hide();

	BOOTSettings();

	return res;
}

void BOOT_Menu::hide()
{
	frameBuffer->paintBackgroundBoxRel(x,y, width,height);
}

void BOOT_Menu::BOOTSettings()
{
#define DOTFILE_BOOTE2 "/etc/.start_enigma2"
#define DOTFILE_BOOTSPARK "/etc/.start_spark"
	int boot = BOOT_NEUTRINO;
	if (!access(DOTFILE_BOOTSPARK, F_OK))
		boot = BOOT_SPARK;
	else if (!access(DOTFILE_BOOTE2, F_OK))
		boot = BOOT_E2;
	int old_boot = boot;

	CMenuWidget* menu = new CMenuWidget(LOCALE_EXTRAMENU_BOOT_HEAD, "settings.raw", width);
	menu->addItem(GenericMenuSeparator);
	menu->addItem(GenericMenuBack);
	menu->addItem(GenericMenuSeparatorLine);
	CMenuOptionChooser* oj1 = new CMenuOptionChooser(LOCALE_EXTRAMENU_BOOT_SELECT, &boot, BOOT_OPTIONS, BOOT_OPTION_COUNT,true);
	menu->addItem( oj1 );
#if 0
	menu->addItem(new CMenuForwarder(LOCALE_MAINMENU_REBOOT, true, "", this, "reboot", CRCInput::RC_red, NEUTRINO_ICON_BUTTON_RED));
	menu->addItem(GenericMenuSeparatorLine);
#endif
	menu->exec (NULL, "");
	menu->hide ();
	delete menu;

	if ((boot != old_boot)
         && (ShowLocalizedMessage (LOCALE_EXTRAMENU_BOOT_HEAD, LOCALE_MESSAGEBOX_ACCEPT, CMessageBox::mbrYes, CMessageBox::mbYes | CMessageBox::mbCancel) != CMessageBox::mbrCancel))
	{
		CHintBox *b = NULL;
		if(boot == BOOT_SPARK || old_boot == BOOT_SPARK) {
			b = new CHintBox(LOCALE_EXTRAMENU_BOOT_BOOTARGS_HEAD, g_Locale->getText(LOCALE_EXTRAMENU_BOOT_BOOTARGS_TEXT));
			b->paint();
		}
		if (boot == BOOT_SPARK) {
			touch(DOTFILE_BOOTSPARK);
			system("fw_setenv -s /etc/bootargs_orig");
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "Spark activated, please reboot now..!", 450, 2);
		}
		if (old_boot == BOOT_SPARK) {
			unlink(DOTFILE_BOOTSPARK);
			system("fw_setenv -s /etc/bootargs_evolux_yaffs2");
		}
		if(b) {
			b->hide();
			delete b;
		}
		if (boot == BOOT_E2 && old_boot == BOOT_NEUTRINO && !access("/usr/local/bin/enigma2", X_OK)) {
			touch(DOTFILE_BOOTE2);
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "E2 activated, please reboot now..!", 450, 2);
		}
		else if (boot == BOOT_NEUTRINO && old_boot == BOOT_E2)
			unlink(DOTFILE_BOOTE2);

	}
}
////////////////////////////// BOOT Menu ENDE //////////////////////////////////////

////////////////////////////// FSCK Menu ANFANG ////////////////////////////////////
#define FSCK_OPTION_COUNT 2
const CMenuOptionChooser::keyval FSCK_OPTIONS[FSCK_OPTION_COUNT] =
{
	{ 0, LOCALE_EXTRAMENU_FSCK_OFF },
	{ 1, LOCALE_EXTRAMENU_FSCK_ON }
};

FSCK_Menu::FSCK_Menu()
{
	frameBuffer = CFrameBuffer::getInstance();
	width = 600;
	hheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight();
	mheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight();
	height = hheight+13*mheight+ 10;

	x=(((g_settings.screen_EndX- g_settings.screen_StartX)-width) / 2) + g_settings.screen_StartX;
	y=(((g_settings.screen_EndY- g_settings.screen_StartY)-height) / 2) + g_settings.screen_StartY;
}

int FSCK_Menu::exec(CMenuTarget* parent, const std::string & actionKey)
{
	int res = menu_return::RETURN_REPAINT;

	if (parent)
		parent->hide();

	FSCKSettings();

	return res;
}

void FSCK_Menu::hide()
{
	frameBuffer->paintBackgroundBoxRel(x,y, width,height);
}

void FSCK_Menu::FSCKSettings()
{
#define DOTFILE_FSCK "/etc/.fsck"
	int fsck = access(DOTFILE_FSCK, F_OK) ? 0 : 1;
	int old_fsck=fsck;
	//MENU AUFBAUEN
	CMenuWidget* menu = new CMenuWidget(LOCALE_EXTRAMENU_FSCK, "settings.raw");
	menu->addItem(GenericMenuSeparator);
	menu->addItem(GenericMenuBack);
	menu->addItem(GenericMenuSeparatorLine);
	CMenuOptionChooser* oj1 = new CMenuOptionChooser(LOCALE_EXTRAMENU_FSCK_SELECT, &fsck, FSCK_OPTIONS, FSCK_OPTION_COUNT,true);
	menu->addItem( oj1 );
	menu->exec (NULL, "");
	menu->hide ();
	delete menu;
	// UEBERPRUEFEN OB SICH WAS GEAENDERT HAT
	if (old_fsck!=fsck)
	{
		if (fsck == 1) {
			touch(DOTFILE_FSCK);
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "FSCK Activated, please reboot!", 450, 2); // UTF-8("")
		} else {
			unlink(DOTFILE_FSCK);
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "FSCK Deactivated!", 450, 2); // UTF-8("")
		}
	}
}
////////////////////////////// FSCK Menu ENDE //////////////////////////////////////

////////////////////////////// STMFB Menu ANFANG ////////////////////////////////////
#define STMFB_OPTION_COUNT 2
const CMenuOptionChooser::keyval STMFB_OPTIONS[STMFB_OPTION_COUNT] =
{
	{ 0, LOCALE_EXTRAMENU_STMFB_OFF },
	{ 1, LOCALE_EXTRAMENU_STMFB_ON }
};

STMFB_Menu::STMFB_Menu()
{
	frameBuffer = CFrameBuffer::getInstance();
	width = 600;
	hheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight();
	mheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight();
	height = hheight+13*mheight+ 10;

	x=(((g_settings.screen_EndX- g_settings.screen_StartX)-width) / 2) + g_settings.screen_StartX;
	y=(((g_settings.screen_EndY- g_settings.screen_StartY)-height) / 2) + g_settings.screen_StartY;
}
int STMFB_Menu::exec(CMenuTarget* parent, const std::string & actionKey)
{
	int res = menu_return::RETURN_REPAINT;

	if (parent)
		parent->hide();

	STMFBSettings();

	return res;
}

void STMFB_Menu::hide()
{
	frameBuffer->paintBackgroundBoxRel(x,y, width,height);
}

void STMFB_Menu::STMFBSettings()
{
#define DOTFILE_STMFB "/etc/.15m"
	int stmfb = access(DOTFILE_STMFB, F_OK) ? 0 : 1;
	int old_stmfb=stmfb;
	//MENU AUFBAUEN
	CMenuWidget* menu = new CMenuWidget(LOCALE_EXTRAMENU_STMFB, "settings.raw");
	menu->addItem(GenericMenuSeparator);
	menu->addItem(GenericMenuBack);
	menu->addItem(GenericMenuSeparatorLine);
	CMenuOptionChooser* oj1 = new CMenuOptionChooser(LOCALE_EXTRAMENU_STMFB_SELECT, &stmfb, STMFB_OPTIONS, STMFB_OPTION_COUNT,true);
	menu->addItem( oj1 );
	menu->exec (NULL, "");
	menu->hide ();
	delete menu;
	// UEBERPRUEFEN OB SICH WAS GEAENDERT HAT
	if (old_stmfb!=stmfb)
	{
		if (stmfb==1)
		{
			//STMFB STARTEN
			touch(DOTFILE_STMFB);
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "STMFB 15m Activated, please reboot!", 450, 2); // UTF-8("")
		} else {
			//STMFB BEENDEN
			unlink(DOTFILE_STMFB);
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "STMFB 15m Deactivated!", 450, 2); // UTF-8("")
		}
	}
}
////////////////////////////// STMFB Menu ENDE //////////////////////////////////////

////////////////////////////// FRITZCALL Menu ANFANG ////////////////////////////////////
#define FRITZCALL_OPTION_COUNT 2
const CMenuOptionChooser::keyval FRITZCALL_OPTIONS[FRITZCALL_OPTION_COUNT] =
{
	{ 0, LOCALE_EXTRAMENU_FRITZCALL_OFF },
	{ 1, LOCALE_EXTRAMENU_FRITZCALL_ON }
};

FRITZCALL_Menu::FRITZCALL_Menu()
{
	frameBuffer = CFrameBuffer::getInstance();
	width = 600;
	hheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight();
	mheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight();
	height = hheight+13*mheight+ 10;

	x=(((g_settings.screen_EndX- g_settings.screen_StartX)-width) / 2) + g_settings.screen_StartX;
	y=(((g_settings.screen_EndY- g_settings.screen_StartY)-height) / 2) + g_settings.screen_StartY;
}

int FRITZCALL_Menu::exec(CMenuTarget* parent, const std::string & actionKey)
{
	int res = menu_return::RETURN_REPAINT;

	if (parent)
		parent->hide();

	FRITZCALLSettings();

	return res;
}

void FRITZCALL_Menu::hide()
{
	frameBuffer->paintBackgroundBoxRel(x,y, width,height);
}

#define DOTFILE_FRITZCALL "/etc/.fritzcall"
void FRITZCALL_Menu::FRITZCALLSettings()
{
	int fritzcall = access(DOTFILE_FRITZCALL, F_OK) ? 0 : 1;
	int old_fritzcall=fritzcall;

	//MENU AUFBAUEN
	CMenuWidget* menu = new CMenuWidget(LOCALE_EXTRAMENU_FRITZCALL, "settings.raw");
	menu->addItem(GenericMenuSeparator);
	menu->addItem(GenericMenuBack);
	menu->addItem(GenericMenuSeparatorLine);
	CMenuOptionChooser* oj1 = new CMenuOptionChooser(LOCALE_EXTRAMENU_FRITZCALL_SELECT, &fritzcall, FRITZCALL_OPTIONS, FRITZCALL_OPTION_COUNT,true);
	menu->addItem( oj1 );
	menu->exec (NULL, "");
	menu->hide ();
	delete menu;
	// UEBERPRUEFEN OB SICH WAS GEAENDERT HAT
	if (old_fritzcall!=fritzcall)
	{
		if (fritzcall == 1)
		{
			//FRITZCALL STARTEN
			touch(DOTFILE_FRITZCALL);
			safe_system("/var/plugins/fritzcall/fb.sh start >/dev/null 2>&1 &");
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "FRITZCALLMONITOR activated!", 450, 2); // UTF-8("")
		} else {
			//FRITZCALL BEENDEN
			unlink(DOTFILE_FRITZCALL);
			safe_system("/var/plugins/fritzcall/fb.sh stop >/dev/null 2>&1 &");
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "FRITZCALLMONITOR deactivated!", 450, 2); // UTF-8("")
		}
	}
	//ENDE FRITZCALL
}
////////////////////////////// FRITZCALL Menu ENDE //////////////////////////////////////

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

#ifdef WITH_GRAPHLCD

void sectionsd_getEventsServiceKey(t_channel_id serviceUniqueKey, CChannelEventList &eList, char search = 0, std::string search_text = "");
void sectionsd_getCurrentNextServiceKey(t_channel_id uniqueServiceKey, CSectionsdClient::responseGetCurrentNextInfoChannelID& current_next );

#include <string>
#include <algorithm>

static nGLCD *nglcd = NULL;

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
		if (fw)
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
		if (fw)
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
		bitmap->DrawHLine(0, bar_top, bitmap->Width(), settings.glcd_color_fg);
		bitmap->DrawHLine(0, bar_bottom, bitmap->Width(), settings.glcd_color_fg);
		if (Scale)
			bitmap->DrawRectangle(0, bar_top + 1, Scale * (bitmap->Width() - 1)/100,
				bar_bottom - 1, settings.glcd_color_bar, true);
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
	t_channel_id channel_id = -1;
	nglcd->fonts_initialized = false;
	bool broken = false;

	do {
		while (nglcd->doSuspend || nglcd->doStandby)
			sem_wait(&nglcd->sem);

		int warmUp = 5;
		if (broken) {
			fprintf(stderr, "No graphlcd display found ... sleeping 30 seconds\n");
			sleep (30);
			broken = false;
			continue;
		}
		nglcd->lcd = GLCD::CreateDriver(GLCD::Config.driverConfigs[0].id, &GLCD::Config.driverConfigs[0]);
		if (!nglcd->lcd) {
			fprintf(stderr, "CreateDriver failed.\n");
			broken = true;
			continue;
		}
		if (nglcd->lcd->Init() != 0) {
			delete nglcd->lcd;
			nglcd->lcd = NULL;
			fprintf(stderr, "LCD init failed.\n");
			broken = true;
			continue;
		}

		nglcd->bitmap = new GLCD::cBitmap(nglcd->lcd->Width(), nglcd->lcd->Height(), settings.glcd_color_bg);

		sem_post(&nglcd->sem);

		do  {
			if (nglcd->doMirrorOSD) {
				nglcd->bitmap->Clear(GLCD::cColor::Black);
				ts.tv_sec = 0; // don't wait
				static CFrameBuffer* fb = CFrameBuffer::getInstance();
				int fb_width = fb->scaleX(fb->getScreenWidth(true));
				int fb_height = fb->scaleX(fb->getScreenHeight(true));
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
						nglcd->bitmap->DrawPixel(x, y, *(fbp + ystride + (y * fb_h / lcd_height) * fb_width
										 + x_min + (x * fb_w / lcd_width)));
					}
				}

				nglcd->lcd->SetScreen(nglcd->bitmap->Data(), lcd_width, lcd_height);
				nglcd->lcd->Refresh(true);
				continue;
			}

			clock_gettime(CLOCK_REALTIME, &ts);
			nglcd->tm = localtime(&ts.tv_sec);
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

			if(nglcd->doRescan || nglcd->doSuspend || nglcd->doStandby)
				break;

			nglcd->updateFonts();

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

				if ((channel_id != new_channel_id) || (evtlist.empty())) {
					evtlist.clear();
					sectionsd_getEventsServiceKey(new_channel_id & 0xFFFFFFFFFFFFULL, evtlist);
					if (!evtlist.empty())
						sort(evtlist.begin(),evtlist.end(), sortByDateTime);
				}
				channel_id = new_channel_id;

				if (!evtlist.empty()) {
					CChannelEventList::iterator eli;
					for ( eli=evtlist.begin(); eli!=evtlist.end(); ++eli ) {
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

		} while(settings.glcd_enable);
		// either disabled, or restart, or shutdown permanently.

		if(!settings.glcd_enable || nglcd->doSuspend || nglcd->doStandby) {
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
	if (nglcd) {
		sem_post(&nglcd->sem);
	}
}

void nGLCD::StandbyMode(bool b) {
	if (nglcd) {
		nglcd->doStandby = b;
		nglcd->doMirrorOSD = false;
		sem_post(&nglcd->sem);
	}
}

void nGLCD::ShowVolume(bool b) {
	if (nglcd) {
		nglcd->doShowVolume = b;
		sem_post(&nglcd->sem);
	}
}

void nGLCD::MirrorOSD(bool b) {
	if (nglcd && settings.glcd_mirror_osd) {
		nglcd->doMirrorOSD = b;
		sem_post(&nglcd->sem);
	}
}

void nGLCD::Exit() {
	if (nglcd) {
		nglcd->doMirrorOSD = false;
		nglcd->doSuspend = true;
		nglcd->doExit = true;
		sem_post(&nglcd->sem);
		void *res;
		pthread_join(nglcd->thrGLCD, &res);
		delete nglcd;
	}
}

void nglcd_update() {
	if (nglcd) {
		sem_post(&nglcd->sem);
	}
}

void nGLCD::Rescan() {
	if (nglcd) {
		doRescan = true;
		sem_post(&nglcd->sem);
	}
}

void nGLCD::Suspend() {
	if (nglcd) {
		nglcd->doSuspend = true;
		sem_post(&nglcd->sem);
	}
}

void nGLCD::Resume() {
	if (nglcd) {
		nglcd->doSuspend = false;
		sem_post(&nglcd->sem);
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
		sem_post(&nglcd->sem);
	}
}

void nGLCD::unlockChannel(void)
{
	if(nglcd) {
		nglcd->channelLocked = false;
		sem_post(&nglcd->sem);
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
	frameBuffer = CFrameBuffer::getInstance();
	width = 600;
	hheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight();
	mheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight();
	height = hheight+13*mheight+ 10;

	x=(((g_settings.screen_EndX - g_settings.screen_StartX)-width) / 2) + g_settings.screen_StartX;
	y=(((g_settings.screen_EndY - g_settings.screen_StartY)-height) / 2) + g_settings.screen_StartY;

	notifier = new GLCD_Menu_Notifier();

	if (!nglcd && settings.glcd_enable)
		new nGLCD;
}


int GLCD_Menu::exec(CMenuTarget* parent, const std::string & actionKey)
{
	int res = menu_return::RETURN_REPAINT;
	if(actionKey == "rescan") {
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
	frameBuffer->paintBackgroundBoxRel(x,y, width,height);
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
		}
		break;
	case LOCALE_EXTRAMENU_GLCD_MIRROR_OSD:
		nglcd->doMirrorOSD = settings.glcd_mirror_osd;
		break;
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

	CMenuWidget* menu = new CMenuWidget(LOCALE_EXTRAMENU_GLCD, "settings.raw");
	menu->addItem(GenericMenuSeparator);
	menu->addItem(GenericMenuBack);
	menu->addItem(GenericMenuSeparatorLine);
	menu->addItem(new CMenuOptionChooser(LOCALE_EXTRAMENU_GLCD, &settings.glcd_enable,
				ONOFF_OPTIONS, ONOFF_OPTION_COUNT, true, notifier));
	int shortcut = 1;
	menu->addItem(GenericMenuSeparatorLine);
	menu->addItem(new CMenuOptionChooser(LOCALE_EXTRAMENU_GLCD_SELECT_FG, &color_fg,
				GLCD_COLOR_OPTIONS, GLCD_COLOR_OPTION_COUNT, true, notifier,
				CRCInput::convertDigitToKey(shortcut++)));
	menu->addItem(new CMenuOptionChooser(LOCALE_EXTRAMENU_GLCD_SELECT_BG, &color_bg,
				GLCD_COLOR_OPTIONS, GLCD_COLOR_OPTION_COUNT, true, notifier,
				CRCInput::convertDigitToKey(shortcut++)));
	menu->addItem(new CMenuOptionChooser(LOCALE_EXTRAMENU_GLCD_SELECT_BAR, &color_bar,
				GLCD_COLOR_OPTIONS, GLCD_COLOR_OPTION_COUNT, true, notifier,
				CRCInput::convertDigitToKey(shortcut++)));
	menu->addItem( new CMenuForwarder(LOCALE_EPGPLUS_SELECT_FONT_NAME, true, NULL, this, "select_font",
				CRCInput::convertDigitToKey(shortcut++)));
	menu->addItem(new CMenuOptionNumberChooser(LOCALE_EXTRAMENU_GLCD_SIZE_CHANNEL,
				&settings.glcd_percent_channel, true, 0, 100, notifier));
	menu->addItem(new CMenuOptionNumberChooser(LOCALE_EXTRAMENU_GLCD_SIZE_EPG,
				&settings.glcd_percent_epg, true, 0, 100, notifier));
	menu->addItem(new CMenuOptionNumberChooser(LOCALE_EXTRAMENU_GLCD_SIZE_BAR,
				&settings.glcd_percent_bar, true, 0, 100, notifier));
	menu->addItem(new CMenuOptionNumberChooser(LOCALE_EXTRAMENU_GLCD_SIZE_TIME,
				&settings.glcd_percent_time, true, 0, 100, notifier));
	menu->addItem(GenericMenuSeparatorLine);
	menu->addItem(new CMenuOptionChooser(LOCALE_EXTRAMENU_GLCD_MIRROR_OSD, &settings.glcd_mirror_osd,
				ONOFF_OPTIONS, ONOFF_OPTION_COUNT, true, notifier,
				CRCInput::RC_green, NEUTRINO_ICON_BUTTON_GREEN));
	menu->addItem(GenericMenuSeparatorLine);
	menu->addItem(new CMenuForwarder(LOCALE_EXTRAMENU_GLCD_RESTART, true, "", this, "rescan",
				CRCInput::RC_red, NEUTRINO_ICON_BUTTON_RED));
	menu->exec(NULL, "");
	menu->hide();
	delete menu;

	saveSettings();
}
#endif

////////////////////////////// EVOLUXUPDATE Menu ANFANG ////////////////////////////////////
#define EVOLUXUPDATE_OPTION_COUNT 2
const CMenuOptionChooser::keyval EVOLUXUPDATE_OPTIONS[EVOLUXUPDATE_OPTION_COUNT] =
{
	{ 0, LOCALE_EXTRAMENU_EVOLUXUPDATE_OFF },
	{ 1, LOCALE_EXTRAMENU_EVOLUXUPDATE_ON },
};

EVOLUXUPDATE_Menu::EVOLUXUPDATE_Menu()
{
	frameBuffer = CFrameBuffer::getInstance();
	width = 600;
	hheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight();
	mheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight();
	height = hheight+13*mheight+ 10;

	x=(((g_settings.screen_EndX- g_settings.screen_StartX)-width) / 2) + g_settings.screen_StartX;
	y=(((g_settings.screen_EndY- g_settings.screen_StartY)-height) / 2) + g_settings.screen_StartY;
}

int EVOLUXUPDATE_Menu::exec(CMenuTarget* parent, const std::string & actionKey)
{
	int res = menu_return::RETURN_REPAINT;
	if(actionKey == "checkupdate") 
	{
		this->CheckUpdate();
		return res;
	}

	if (parent)
		parent->hide();

	EVOLUXUPDATESettings();

	return res;
}

void EVOLUXUPDATE_Menu::hide()
{
	frameBuffer->paintBackgroundBoxRel(x,y, width,height);
}

void EVOLUXUPDATE_Menu::EVOLUXUPDATESettings()
{
	//MENU AUFBAUEN
	CMenuWidget* menu = new CMenuWidget(LOCALE_EXTRAMENU_EVOLUXUPDATE, "settings.raw");
	menu->addItem(GenericMenuSeparator);
	menu->addItem(GenericMenuBack);
	menu->addItem(GenericMenuSeparatorLine);
	menu->addItem(new CMenuForwarder(LOCALE_EXTRAMENU_EVOLUXUPDATE_UPDATE, true, "", this, "checkupdate", CRCInput::RC_red, NEUTRINO_ICON_BUTTON_RED));
	menu->exec (NULL, "");
	menu->hide ();
	delete menu;
}

#include <errno.h>
bool EVOLUXUPDATE_Menu::CheckUpdate()
{
	//EVOLUXUPDATE STARTEN
	unlink("/tmp/EvoluxUpdatevailable");
	safe_system("oVersion=`wget -q -O - http://tinyurl.com/7gz7jpo | grep version | cut -d = -f2`;"
		   "lVersion=`grep version /etc/.version | cut -d = -f2`;"
		   "[ \"$lVersion\" != \"$oVersion\" ] && touch /tmp/EvoluxUpdatevailable");
	if(!access("/tmp/EvoluxUpdatevailable", F_OK)) {
		CHintBox * CheckUpdateBox = new CHintBox(LOCALE_EXTRAMENU_EVOLUXUPDATE_UPDATE, "update found, performing update now...");
		CheckUpdateBox->paint();
		safe_system("wget -O - -q http://tinyurl.com/7fjrnm3 | tee /tmp/update.tar.gz | tar -tzf - && tar -xpzf /tmp/update.tar.gz -C /");
		unlink("/tmp/EvoluxUpdatevailable");
		unlink("/tmp/update.tar.gz");
		CheckUpdateBox->hide();
		delete CheckUpdateBox;
		CheckUpdateBox = new CHintBox(LOCALE_EXTRAMENU_EVOLUXUPDATE_UPDATE, "update done, please reboot now...");
		CheckUpdateBox->paint();
		sleep(3);
		CheckUpdateBox->hide();
		delete CheckUpdateBox;
	}
	else {
		CHintBox * CheckUpdateBox = new CHintBox(LOCALE_EXTRAMENU_EVOLUXUPDATE_UPDATE, "no update available!");
		CheckUpdateBox->paint();
		sleep(3);
		CheckUpdateBox->hide();
		delete CheckUpdateBox;
	}
}
//ENDE EVOLUXUPDATE

////////////////////////////// EVOLUXUPDATE Menu ENDE //////////////////////////////////////
// vim:ts=4
