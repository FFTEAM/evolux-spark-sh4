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
#include <fcntl.h>

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

/*#define EXTRAMENU_ONOFF_OPTION_COUNT 2

const CMenuOptionChooser::keyval EXTRAMENU_ONOFF_OPTIONS[EXTRAMENU_ONOFF_OPTION_COUNT] =
{
{ 0, LOCALE_EXTRAMENU_ONOFF_OFF },
{ 1, LOCALE_EXTRAMENU_ONOFF_ON },
}; */

static int touch(const char *filename) {
	int fn = open(filename, O_RDWR | O_CREAT, 0644);
	if (fn > -1) {
		close(fn);
		return -1;
	}
	return 0;
}

static struct {
#define EXTRA_CAM_SELECTED "cam_selected"
        std::string     cam_selected;
#define EXTRA_FRITZCALL "fritzcall"
        bool            fritzcall;
} settings;

CConfigFile *configfile = NULL;

static bool saveSettings() {
	if (configfile) {
		configfile->setString(EXTRA_CAM_SELECTED, settings.cam_selected);
		configfile->setInt32(EXTRA_FRITZCALL, settings.fritzcall);
		configfile->saveConfig(EXTRA_SETTINGS_FILE);
		return true;
	}
	return false;
}

static bool initSettings() {
	settings.cam_selected = "disabled";
	settings.fritzcall = 0;
}

static bool loadSettings() {
	if (!configfile) {
		initSettings();
		configfile = new CConfigFile('=');
		if (configfile->loadConfig(EXTRA_SETTINGS_FILE)) {
			settings.cam_selected = configfile->getString(EXTRA_CAM_SELECTED, "disabled");
			settings.fritzcall = configfile->getInt32(EXTRA_FRITZCALL, 0);
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
	CMenuWidget* ExtraMenuSettings = new CMenuWidget(LOCALE_EXTRAMENU_CORRECTVOLUME, "settings.raw");
	ExtraMenuSettings->addItem(GenericMenuSeparator);
	ExtraMenuSettings->addItem(GenericMenuBack);
	ExtraMenuSettings->addItem(GenericMenuSeparatorLine);
	CMenuOptionChooser* oj1 = new CMenuOptionChooser(LOCALE_EXTRAMENU_CORRECTVOLUME_SELECT, &corrVol, CORRECTVOLUME_OPTIONS, CORRECTVOLUME_OPTION_COUNT,true);
	ExtraMenuSettings->addItem( oj1 );
	ExtraMenuSettings->exec (NULL, "");
	ExtraMenuSettings->hide ();
	delete ExtraMenuSettings;
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
	system("/etc/init.d/corrVol.sh &");
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
	CMenuWidget* ExtraMenuSettings = new CMenuWidget(LOCALE_EXTRAMENU_TUNERRESET, "settings.raw");
	ExtraMenuSettings->addItem(GenericMenuSeparator);
	ExtraMenuSettings->addItem(GenericMenuBack);
	ExtraMenuSettings->addItem(GenericMenuSeparatorLine);
	ExtraMenuSettings->addItem(new CMenuForwarder(LOCALE_EXTRAMENU_TUNERRESET_RESTART, true, "", this, "tunerreset", CRCInput::RC_red, NEUTRINO_ICON_BUTTON_RED));
	ExtraMenuSettings->exec (NULL, "");
	ExtraMenuSettings->hide ();
	delete ExtraMenuSettings;
}
bool TUNERRESET_Menu::TunerReset()
{
	//TUNERRESET STARTEN
	CHintBox * TunerResetBox = new CHintBox(LOCALE_EXTRAMENU_TUNERRESET_RESTART, "bitte warten, Tuner wird resettet");
	TunerResetBox->paint();

	system("/usr/local/bin/pzapit -esb ; sleep 2 ; /usr/local/bin/pzapit -lsb");
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
	CMenuWidget* ExtraMenuSettings = new CMenuWidget(LOCALE_EXTRAMENU_AMOUNT, "settings.raw");
	ExtraMenuSettings->addItem(GenericMenuSeparator);
	ExtraMenuSettings->addItem(GenericMenuBack);
	ExtraMenuSettings->addItem(GenericMenuSeparatorLine);
	CMenuOptionChooser* oj1 = new CMenuOptionChooser(LOCALE_EXTRAMENU_AMOUNT_SELECT, &amount, AMOUNT_OPTIONS, AMOUNT_OPTION_COUNT,true);
	ExtraMenuSettings->addItem( oj1 );
	ExtraMenuSettings->exec (NULL, "");
	ExtraMenuSettings->hide ();
	delete ExtraMenuSettings;
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
	system("/etc/init.d/amount.sh &");
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
	int checkfs = access(DOTFILE_CHECKFS, R_OK) ? 0 : 1;
	int old_checkfs=checkfs;
	//MENU AUFBAUEN
	CMenuWidget* ExtraMenuSettings = new CMenuWidget(LOCALE_EXTRAMENU_CHECKFS, "settings.raw");
	ExtraMenuSettings->addItem(GenericMenuSeparator);
	ExtraMenuSettings->addItem(GenericMenuBack);
	ExtraMenuSettings->addItem(GenericMenuSeparatorLine);
	CMenuOptionChooser* oj1 = new CMenuOptionChooser(LOCALE_EXTRAMENU_CHECKFS_SELECT, &checkfs, CHECKFS_OPTIONS, CHECKFS_OPTION_COUNT,true);
	ExtraMenuSettings->addItem( oj1 );
	ExtraMenuSettings->exec (NULL, "");
	ExtraMenuSettings->hide ();
	delete ExtraMenuSettings;
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
static const CMenuOptionChooser::keyval EMU_OPTIONS[EMU_OPTION_COUNT] =
{
#define KEY_EMU_NONE	0
#define KEY_EMU_MGCAMD	1
#define KEY_EMU_INCUBUS	2
#define KEY_EMU_CAMD3	3
#define KEY_EMU_MBOX	4
#define KEY_EMU_OSCAM	5
#define KEY_EMU_SPCS	6
	{ KEY_EMU_NONE,		LOCALE_EXTRAMENU_EMU_NONE },
	{ KEY_EMU_MGCAMD,	LOCALE_EXTRAMENU_EMU_MGCAMD },
	{ KEY_EMU_INCUBUS,	LOCALE_EXTRAMENU_EMU_INCUBUS },
	{ KEY_EMU_CAMD3,	LOCALE_EXTRAMENU_EMU_CAMD3 },
	{ KEY_EMU_MBOX,		LOCALE_EXTRAMENU_EMU_MBOX },
	{ KEY_EMU_OSCAM,	LOCALE_EXTRAMENU_EMU_OSCAM },
	{ KEY_EMU_SPCS,		LOCALE_EXTRAMENU_EMU_SPCS }
};

static EMU_Menu::emu_list EMU_list[EMU_OPTION_COUNT] =
{
	  { "disabled", LOCALE_EXTRAMENU_EMU_NONE }
	, { "mgcamd", LOCALE_EXTRAMENU_EMU_MGCAMDVERSION,
		"rm -f /tmp/camd.socket >/dev/null; /usr/bin/mgcamd /var/keys/mg_cfg >/dev/null 2>&1 &",
		"kill -9 $(pidof mgcamd)", false, false }
	, { "incubusCamd", LOCALE_EXTRAMENU_EMU_INCUBUSVERSION,
		"rm -f /tmp/camd.socket 2>/dev/null ; /usr/bin/incubusCamd >/dev/null 2>&1 &",
		"kill -9 $(pidof incubusCamd)",
		false, false }
	, { "camd3", LOCALE_EXTRAMENU_EMU_CAMD3VERSION,
		"/usr/bin/camd3 /var/keys/camd3.config >/dev/null 2>&1 &",
		"kill -9 $(pidof camd3)",
		false, false }
	, { "mbox", LOCALE_EXTRAMENU_EMU_MBOXVERSION,
		"/usr/bin/mbox /var/keys/mbox.cfg >/dev/null 2>&1 &"
		"kill -9 $(pidof mbox) ; rm -f /tmp/share.* /tmp/mbox.ver /tmp/*.info 2>/dev/null",
		false, false }
	, { "oscam", LOCALE_EXTRAMENU_EMU_OSCAMVERSION,
		"/usr/bin/oscam -b -c /var/keys >/dev/null 2>&1 &",
		"kill -9 $(pidof oscam)",
		false, false }
	, { "spcs", LOCALE_EXTRAMENU_EMU_SPCSVERSION,
		"/usr/bin/spcs -c /var/keys >/dev/null 2>&1 &",
		"kill -9 $(pidof spcs)",
		false, false }
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

	for (int i = 1; i < EMU_OPTION_COUNT; i++)
		strncpy(EMU_list[i].version, "N/A", 8);

	if (FILE *ev = fopen("/usr/bin/versions.txt", "r"))
	{
		char buffer[120];
		while(fgets(buffer, sizeof(buffer), ev))
		{
			sscanf(buffer, "mgcamd=%7s", EMU_list[KEY_EMU_MGCAMD].version);
			sscanf(buffer, "incubuscamd=%7s", EMU_list[KEY_EMU_INCUBUS].version);
			sscanf(buffer, "camd3=%7s", EMU_list[KEY_EMU_CAMD3].version);
			sscanf(buffer, "mbox=%7s", EMU_list[KEY_EMU_MBOX].version);
			sscanf(buffer, "oscam=%7s", EMU_list[KEY_EMU_OSCAM].version);
			sscanf(buffer, "spcs=%7s", EMU_list[KEY_EMU_SPCS].version);
		}
		fclose(ev);
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
		system(cmd.c_str());
	}
}

int EMU_Menu::exec(CMenuTarget* parent, const std::string & actionKey)
{
	int res = menu_return::RETURN_REPAINT;
	if(actionKey == "camdreset") {
		this->CamdReset();
		return res;
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

	//MENU AUFBAUEN
	CMenuWidget* ExtraMenuSettings = new CMenuWidget(LOCALE_EXTRAMENU_EMU, "settings.raw");
	ExtraMenuSettings->addItem(GenericMenuSeparator);
	ExtraMenuSettings->addItem(GenericMenuBack);
	ExtraMenuSettings->addItem(GenericMenuSeparatorLine);
	CMenuOptionChooser* oj2 = new CMenuOptionChooser(LOCALE_EXTRAMENU_EMU_SELECT, &emu, EMU_OPTIONS, EMU_OPTION_COUNT, true);
	ExtraMenuSettings->addItem(oj2);
	ExtraMenuSettings->addItem(new CMenuForwarder(LOCALE_EXTRAMENU_EMU_RESTART, true, "", this, "camdreset", CRCInput::RC_red, NEUTRINO_ICON_BUTTON_RED));
	ExtraMenuSettings->addItem(GenericMenuSeparatorLine);

	for (int i = 1; i < EMU_OPTION_COUNT; i++)
		if (EMU_list[i].installed)
			ExtraMenuSettings->addItem( new CMenuForwarder(EMU_list[i].loctxt, false, EMU_list[i].version));

	ExtraMenuSettings->exec(NULL, "");
	ExtraMenuSettings->hide();
	delete ExtraMenuSettings;

	if(emu > 0 && !EMU_list[emu].installed) {
		string m = " " + string(EMU_list[emu].procname) + " is not installed ";
		ShowHintUTF(LOCALE_MESSAGEBOX_INFO, m.c_str(), 450, 2); // UTF-8("")
	} else if (emu_old != emu) {
		if (emu_old) {
			// stop currently running emu
			system(EMU_list[emu_old].stop_command);
			string m = " " + string(EMU_list[emu_old].procname) + " is now inactive ";
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, m.c_str(), 450, 2); // UTF-8("")
		}
		if (emu) {
			system(EMU_list[emu].start_command);

			string cmd = "(" + string(EMU_list[emu].start_command);
			if (is_scrambled())
				system("sleep 2; /usr/local/bin/pzapit -rz >/dev/null 2>&1");
			string m = " " + string(EMU_list[emu].procname) + " is now active ";
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, m.c_str(), 450, 2); // UTF-8("")
		}
		settings.cam_selected = string(EMU_list[emu].procname);
		selected = emu;
	}

	saveSettings();
}

void EMU_Menu::suspend()
{
	if (selected && !suspended) {
		system(EMU_list[selected].stop_command);
		suspended = true;
	}
}

void EMU_Menu::resume()
{
	if (selected && suspended) {
		system(EMU_list[selected].start_command);
		if (is_scrambled())
			system("sleep 2; /usr/local/bin/pzapit -rz >/dev/null 2>&1");
		suspended = false;
	}
}

//EMU RESTARTEN
bool EMU_Menu::CamdReset()
{
	string m = "Please wait for " +  string(EMU_list[selected].procname) + " to restart.";
	CHintBox * CamdResetBox = new CHintBox(LOCALE_EXTRAMENU_EMU_RESTART, m.c_str());
	CamdResetBox->paint();
	system(EMU_list[selected].stop_command);
	sleep(1);
	system(EMU_list[selected].start_command);
	CChannelList *channelList = CNeutrinoApp::getInstance ()->channelList;
	int curnum = channelList->getActiveChannelNumber();
	CZapitChannel *channel = channelList->getChannel(curnum);
	if (is_scrambled)
		system("sleep 2; /usr/local/bin/pzapit -rz >/dev/null 2>&1");
	CamdResetBox->hide();
	delete CamdResetBox;
	return true;
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
	int displaytime = access(DOTFILE_DISPLAYTIME, R_OK) ? 0 : 1;
	int old_displaytime=displaytime;
	//MENU AUFBAUEN
	CMenuWidget* ExtraMenuSettings = new CMenuWidget(LOCALE_EXTRAMENU_DISPLAYTIME, "settings.raw");
	ExtraMenuSettings->addItem(GenericMenuSeparator);
	ExtraMenuSettings->addItem(GenericMenuBack);
	ExtraMenuSettings->addItem(GenericMenuSeparatorLine);
	CMenuOptionChooser* oj1 = new CMenuOptionChooser(LOCALE_EXTRAMENU_DISPLAYTIME_SELECT, &displaytime, DISPLAYTIME_OPTIONS, DISPLAYTIME_OPTION_COUNT,true);
	ExtraMenuSettings->addItem( oj1 );
	ExtraMenuSettings->exec (NULL, "");
	ExtraMenuSettings->hide ();
	delete ExtraMenuSettings;
	// UEBERPRUEFEN OB SICH WAS GEAENDERT HAT
	if (old_displaytime!=displaytime)
	{
		if (displaytime==1)
		{
			//DisplayTime STARTEN
			touch("/etc/.time");
			system("/etc/init.d/DisplayTime.sh >/dev/null 2>&1 &");
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "DISPLAYTIME Activated!", 450, 2); // UTF-8("")
		} else {
			//DisplayTime BEENDEN
			unlink("/etc/.time");
			system("killall -9 DisplayTime.sh");
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
	CMenuWidget* ExtraMenuSettings = new CMenuWidget(LOCALE_EXTRAMENU_WWWDATE, "settings.raw");
	ExtraMenuSettings->addItem(GenericMenuSeparator);
	ExtraMenuSettings->addItem(GenericMenuBack);
	ExtraMenuSettings->addItem(GenericMenuSeparatorLine);
	CMenuOptionChooser* oj1 = new CMenuOptionChooser(LOCALE_EXTRAMENU_WWWDATE_SELECT, &wwwdate, WWWDATE_OPTIONS, WWWDATE_OPTION_COUNT,true);
	ExtraMenuSettings->addItem( oj1 );
	ExtraMenuSettings->exec (NULL, "");
	ExtraMenuSettings->hide ();
	delete ExtraMenuSettings;
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

#define SWAP_ONOFF_OPTION_COUNT 2
const CMenuOptionChooser::keyval SWAP_ONOFF_OPTIONS[SWAP_ONOFF_OPTION_COUNT] =
{
	{ 0, LOCALE_EXTRAMENU_SWAP_OFF },
	{ 1, LOCALE_EXTRAMENU_SWAP_ON },
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
			system("/etc/init.d/Swap.sh >/dev/null 2>&1 &");
			return "SWAP-RAM activated";
		case KEY_SWAP_SWAPPART:
			system("/etc/init.d/Swap.sh >/dev/null 2>&1 &");
			return "SWAP partition activated";
		case KEY_SWAP_SWAPFILE:
			system("/etc/init.d/Swap.sh >/dev/null 2>&1 &");
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
	int swap_onoff = access(DOTFILE_SWAPON, R_OK) ? 0 : 1;
	int swap=0;

	if (!access(DOTFILE_SWAPRAM, R_OK))
		swap = KEY_SWAP_SWAPRAM;
	else if (!access(DOTFILE_SWAPPART, R_OK))
		swap = KEY_SWAP_SWAPPART;
	else if (!access(DOTFILE_SWAPFILE, R_OK))
		swap = KEY_SWAP_SWAPFILE;

	int old_swap=swap;
	int old_swap_onoff=swap_onoff;

	//MENU AUFBAUEN
	CMenuWidget* ExtraMenuSettings = new CMenuWidget(LOCALE_EXTRAMENU_SWAP, "settings.raw");
	ExtraMenuSettings->addItem(GenericMenuSeparator);
	ExtraMenuSettings->addItem(GenericMenuBack);
	ExtraMenuSettings->addItem(GenericMenuSeparatorLine);
	CMenuOptionChooser* oj1 = new CMenuOptionChooser(LOCALE_EXTRAMENU_SWAP_ONOFF, &swap_onoff, SWAP_ONOFF_OPTIONS, 		SWAP_ONOFF_OPTION_COUNT,true);
	ExtraMenuSettings->addItem( oj1 );
	ExtraMenuSettings->addItem(GenericMenuSeparatorLine);
	CMenuOptionChooser* oj2 = new CMenuOptionChooser(LOCALE_EXTRAMENU_SWAP_SELECT, &swap, SWAP_OPTIONS, SWAP_OPTION_COUNT,true);
	ExtraMenuSettings->addItem( oj2 );
	ExtraMenuSettings->exec (NULL, "");
	ExtraMenuSettings->hide ();
	delete ExtraMenuSettings;

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
	width = 600;
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
	if (!access(DOTFILE_BOOTSPARK, R_OK))
		boot = BOOT_SPARK;
	else if (!access(DOTFILE_BOOTE2, R_OK))
		boot = BOOT_E2;
	int old_boot = boot;

	CMenuWidget* ExtraMenuSettings = new CMenuWidget(LOCALE_EXTRAMENU_BOOT_HEAD, "settings.raw");
	ExtraMenuSettings->addItem(GenericMenuSeparator);
	ExtraMenuSettings->addItem(GenericMenuBack);
	ExtraMenuSettings->addItem(GenericMenuSeparatorLine);
	CMenuOptionChooser* oj1 = new CMenuOptionChooser(LOCALE_EXTRAMENU_BOOT_SELECT, &boot, BOOT_OPTIONS, BOOT_OPTION_COUNT,true);
	ExtraMenuSettings->addItem( oj1 );
#if 0
	ExtraMenuSettings->addItem(new CMenuForwarder(LOCALE_MAINMENU_REBOOT, true, "", this, "reboot", CRCInput::RC_red, NEUTRINO_ICON_BUTTON_RED));
	ExtraMenuSettings->addItem(GenericMenuSeparatorLine);
#endif
	ExtraMenuSettings->exec (NULL, "");
	ExtraMenuSettings->hide ();
	delete ExtraMenuSettings;

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
		}
		if (old_boot == BOOT_SPARK) {
			unlink(DOTFILE_BOOTSPARK);
			system("fw_setenv -s /etc/bootargs_evolux_yaffs2");
		}
		if(b) {
			b->hide();
			delete b;
		}
		if (boot == BOOT_E2 && old_boot == BOOT_NEUTRINO && !access("/usr/local/bin/enigma2", X_OK))
			touch(DOTFILE_BOOTE2);
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
	int fsck = access(DOTFILE_FSCK, R_OK) ? 0 : 1;
	int old_fsck=fsck;
	//MENU AUFBAUEN
	CMenuWidget* ExtraMenuSettings = new CMenuWidget(LOCALE_EXTRAMENU_FSCK, "settings.raw");
	ExtraMenuSettings->addItem(GenericMenuSeparator);
	ExtraMenuSettings->addItem(GenericMenuBack);
	ExtraMenuSettings->addItem(GenericMenuSeparatorLine);
	CMenuOptionChooser* oj1 = new CMenuOptionChooser(LOCALE_EXTRAMENU_FSCK_SELECT, &fsck, FSCK_OPTIONS, FSCK_OPTION_COUNT,true);
	ExtraMenuSettings->addItem( oj1 );
	ExtraMenuSettings->exec (NULL, "");
	ExtraMenuSettings->hide ();
	delete ExtraMenuSettings;
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
	int stmfb = access(DOTFILE_STMFB, R_OK) ? 0 : 1;
	int old_stmfb=stmfb;
	//MENU AUFBAUEN
	CMenuWidget* ExtraMenuSettings = new CMenuWidget(LOCALE_EXTRAMENU_STMFB, "settings.raw");
	ExtraMenuSettings->addItem(GenericMenuSeparator);
	ExtraMenuSettings->addItem(GenericMenuBack);
	ExtraMenuSettings->addItem(GenericMenuSeparatorLine);
	CMenuOptionChooser* oj1 = new CMenuOptionChooser(LOCALE_EXTRAMENU_STMFB_SELECT, &stmfb, STMFB_OPTIONS, STMFB_OPTION_COUNT,true);
	ExtraMenuSettings->addItem( oj1 );
	ExtraMenuSettings->exec (NULL, "");
	ExtraMenuSettings->hide ();
	delete ExtraMenuSettings;
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

void FRITZCALL_Menu::FRITZCALLSettings()
{
	loadSettings();
	int fritzcall = settings.fritzcall ? 1 : 0;
	int old_fritzcall=fritzcall;

	//MENU AUFBAUEN
	CMenuWidget* ExtraMenuSettings = new CMenuWidget(LOCALE_EXTRAMENU_FRITZCALL, "settings.raw");
	ExtraMenuSettings->addItem(GenericMenuSeparator);
	ExtraMenuSettings->addItem(GenericMenuBack);
	ExtraMenuSettings->addItem(GenericMenuSeparatorLine);
	CMenuOptionChooser* oj1 = new CMenuOptionChooser(LOCALE_EXTRAMENU_FRITZCALL_SELECT, &fritzcall, FRITZCALL_OPTIONS, FRITZCALL_OPTION_COUNT,true);
	ExtraMenuSettings->addItem( oj1 );
	ExtraMenuSettings->exec (NULL, "");
	ExtraMenuSettings->hide ();
	delete ExtraMenuSettings;
	// UEBERPRUEFEN OB SICH WAS GEAENDERT HAT
	if (old_fritzcall!=fritzcall)
	{
		if (fritzcall == 1)
		{
			//FRITZCALL STARTEN
			settings.fritzcall = true;
			system("/var/plugins/fritzcall/fb.sh start >/dev/null 2>&1 &");
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "FRITZCALLMONITOR activated!", 450, 2); // UTF-8("")
		} else {
			//FRITZCALL BEENDEN
			settings.fritzcall = false;
			system("/var/plugins/fritzcall/fb.sh stop >/dev/null 2>&1 &");
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "FRITZCALLMONITOR deactivated!", 450, 2); // UTF-8("")
		}
		saveSettings();
	}
	//ENDE FRITZCALL
}
////////////////////////////// FRITZCALL Menu ENDE //////////////////////////////////////

