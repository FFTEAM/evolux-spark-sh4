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

#define E2SETTINGS "/etc/enigma2/settings"

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

#define EMU_OPTION_COUNT 6
static const CMenuOptionChooser::keyval EMU_OPTIONS[EMU_OPTION_COUNT] =
{
#define KEY_EMU_MGCAMD	0
#define KEY_EMU_INCUBUS	1
#define KEY_EMU_CAMD3	2
#define KEY_EMU_MBOX	3
#define KEY_EMU_OSCAM	4
#define KEY_EMU_SPCS	5
	{ KEY_EMU_MGCAMD,	LOCALE_EXTRAMENU_EMU_MGCAMD },
	{ KEY_EMU_INCUBUS,	LOCALE_EXTRAMENU_EMU_INCUBUS },
	{ KEY_EMU_CAMD3,	LOCALE_EXTRAMENU_EMU_CAMD3 },
	{ KEY_EMU_MBOX,		LOCALE_EXTRAMENU_EMU_MBOX },
	{ KEY_EMU_OSCAM,	LOCALE_EXTRAMENU_EMU_OSCAM },
	{ KEY_EMU_SPCS,		LOCALE_EXTRAMENU_EMU_SPCS }
};

static EMU_Menu::emu_list EMU_list[EMU_OPTION_COUNT] =
{
	{ "mgcamd", LOCALE_EXTRAMENU_EMU_MGCAMDVERSION,
		"rm -f /tmp/camd.socket 2>/dev/null; /usr/bin/mgcamd /var/keys/mg_cfg >/dev/null 2>&1 &",
		"kill -9 $(pidof mgcamd)", false, false },
	{ "incubusCamd", LOCALE_EXTRAMENU_EMU_INCUBUSVERSION,
		"rm -f /tmp/camd.socket 2>/dev/null ; /usr/bin/incubusCamd >/dev/null 2>&1 &",
		"kill -9 $(pidof incubusCamd)",
		false, false },
	{ "camd3", LOCALE_EXTRAMENU_EMU_CAMD3VERSION,
		"/usr/bin/camd3 /var/keys/camd3.config >/dev/null 2>&1 &",
		"kill -9 $(pidof camd3)",
		false, false },
	{ "mbox", LOCALE_EXTRAMENU_EMU_MBOXVERSION,
		"/usr/bin/mbox /var/keys/mbox.cfg >/dev/null 2>&1 &"
		"kill -9 $(pidof mbox) ; rm -f /tmp/share.* /tmp/mbox.ver /tmp/*.info 2>/dev/null",
		false, false },
	{ "oscam", LOCALE_EXTRAMENU_EMU_OSCAMVERSION,
		"/usr/bin/oscam -b -c /var/keys >/dev/null 2>&1",
		"kill -9 $(pidof oscam)",
		false, false },
	{ "spcs", LOCALE_EXTRAMENU_EMU_SPCSVERSION,
		"/usr/bin/spcs -c /var/keys >/dev/null 2>&1 &",
		"kill -9 $(pidof spcs)",
		false, false }
};

#define EMU_ONOFF_OPTION_COUNT 2
const CMenuOptionChooser::keyval EMU_ONOFF_OPTIONS[EMU_ONOFF_OPTION_COUNT] =
{
	{ 0, LOCALE_EXTRAMENU_EMU_OFF },
	{ 1, LOCALE_EXTRAMENU_EMU_ON },
};

#define EMU_RESTART_OPTION_COUNT 2
const CMenuOptionChooser::keyval EMU_RESTART_OPTIONS[EMU_RESTART_OPTION_COUNT] =
{
	{ 0, LOCALE_EXTRAMENU_EMU_RESTART_OFF },
	{ 1, LOCALE_EXTRAMENU_EMU_RESTART_ON }
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

	for (int i = 0; i < EMU_OPTION_COUNT; i++)
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
	E2_Config *cfg = new E2_Config(E2SETTINGS);
	string CamSelect = cfg->get("config.plugins.PinkPanel.CamSelect");
	string CamAutostart = cfg->get("config.plugins.PinkPanel.CamAutostart");
	delete cfg;

	autostart = (CamAutostart.length() && !CamAutostart.compare("true"));
	selected = -1;

	if (CamSelect.length())
		for (int i = 0; i < EMU_OPTION_COUNT; i++)
			if (!CamSelect.compare(EMU_list[i].procname)) {
				if (EMU_list[i].installed)
					selected = i;
				return selected;
			}
	return -1;
}

#define EMU_START_SCRIPT "/tmp/emu-start.sh"
#define EMU_STOP_SCRIPT "/tmp/emu-stop.sh"

EMU_Menu::EMU_Menu()
{
	frameBuffer = CFrameBuffer::getInstance();
	width = 600;
	hheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight();
	mheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight();
	height = hheight+13*mheight+ 10;

	x=(((g_settings.screen_EndX- g_settings.screen_StartX)-width) / 2) + g_settings.screen_StartX;
	y=(((g_settings.screen_EndY- g_settings.screen_StartY)-height) / 2) + g_settings.screen_StartY;

	update_installed();
	update_selected();

	if ((selected > -1) && autostart){
		string cmd = "(" + string(EMU_list[selected].start_command) +
			"sleep 2 ; /usr/local/bin/pzapit -rz >/dev/null 2>&1" + // <- FIXME. No need for this when viewing a clear channel.
			") &";
		system(cmd.c_str());
		running = true;
		create_startstop_scripts(selected);
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

void EMU_Menu::create_startstop_scripts(int i)
{
	ofstream startscript (EMU_START_SCRIPT);
	if(startscript.is_open()) {
		startscript << "#!/bin/sh" << endl
			    << EMU_list[selected].start_command << endl;
		startscript.close();
		chmod(EMU_START_SCRIPT, 0755);
	}
	ofstream stopscript (EMU_STOP_SCRIPT);
	if(stopscript.is_open()) {
		stopscript << "#!/bin/sh" << endl
			   << EMU_list[selected].stop_command << endl;
		stopscript.close();
		chmod(EMU_STOP_SCRIPT, 0755);
	}
}

void EMU_Menu::remove_startstop_scripts(void) {
	unlink(EMU_STOP_SCRIPT);
	unlink(EMU_START_SCRIPT);
}

void EMU_Menu::EMU_Menu_Settings()
{
	int emu_onoff = running ? 1 : 0;
	int emu_onoff_new = emu_onoff;
	int emu = (selected < 0) ? 0 : selected;
	int emu_autostart = autostart ? 1 : 0;
	int emu_autostart_new = emu_autostart;

	//MENU AUFBAUEN
	CMenuWidget* ExtraMenuSettings = new CMenuWidget(LOCALE_EXTRAMENU_EMU, "settings.raw");
	ExtraMenuSettings->addItem(GenericMenuSeparator);
	ExtraMenuSettings->addItem(GenericMenuBack);
	ExtraMenuSettings->addItem(GenericMenuSeparatorLine);
	CMenuOptionChooser* oj1 = new CMenuOptionChooser(LOCALE_EXTRAMENU_EMU_ONOFF, &emu_onoff_new, EMU_ONOFF_OPTIONS, EMU_ONOFF_OPTION_COUNT,true);
	ExtraMenuSettings->addItem(oj1);
	CMenuOptionChooser* oj2 = new CMenuOptionChooser(LOCALE_EXTRAMENU_EMU_SELECT, &emu, EMU_OPTIONS, EMU_OPTION_COUNT, true);
	ExtraMenuSettings->addItem(oj2);
	CMenuOptionChooser* oj3 = new CMenuOptionChooser(LOCALE_EXTRAMENU_EMU_AUTOSTART, &emu_autostart_new, EMU_ONOFF_OPTIONS, EMU_ONOFF_OPTION_COUNT, true);
	ExtraMenuSettings->addItem(oj3);
	ExtraMenuSettings->addItem(new CMenuForwarder(LOCALE_EXTRAMENU_EMU_RESTART, true, "", this, "camdreset", CRCInput::RC_red, NEUTRINO_ICON_BUTTON_RED));
	ExtraMenuSettings->addItem(GenericMenuSeparatorLine);

	for (int i = 0; i < EMU_OPTION_COUNT; i++)
		if (EMU_list[i].installed)
			ExtraMenuSettings->addItem( new CMenuForwarder(EMU_list[i].loctxt, false, EMU_list[i].version));

	ExtraMenuSettings->exec(NULL, "");
	ExtraMenuSettings->hide();
	delete ExtraMenuSettings;

	if(!EMU_list[emu].installed) {
		string m = " " + string(EMU_list[emu].procname) + " is not installed ";
		ShowHintUTF(LOCALE_MESSAGEBOX_INFO, m.c_str(), 450, 2); // UTF-8("")
		return;
	}

	if ((emu_autostart != emu_autostart_new) || (selected != emu)) {
		E2_Config *cfg = new E2_Config(E2SETTINGS);
		if (emu_autostart_new == 0) {
			cfg->unset("config.plugins.PinkPanel.CamAutostart");
			autostart = false;
		} else {
			cfg->set("config.plugins.PinkPanel.CamAutostart", "true");
			autostart = true;
		}
		cfg->set("config.plugins.PinkPanel.CamSelect", string(EMU_list[selected].procname));
		delete cfg;
		return;
	}

	if (running && ((emu_onoff_new == 0) || (selected != emu))) {
		// stop currently running emu
		system(EMU_list[selected].stop_command);
		running = false;
		remove_startstop_scripts();
		string m = " " + string(EMU_list[selected].procname) + " is now inactive ";
		ShowHintUTF(LOCALE_MESSAGEBOX_INFO, m.c_str(), 450, 2); // UTF-8("")
		selected = emu;
		return;
	}

	if (!running && emu_onoff_new == 1) {
		// start currently selected emu
		system(EMU_list[emu].start_command);
		sleep(2);
		system("/usr/local/bin/pzapit -rz >/dev/null 2>&1");
		running = true;
		string m = " " + string(EMU_list[emu].procname) + " is now active ";
		create_startstop_scripts(emu);
		ShowHintUTF(LOCALE_MESSAGEBOX_INFO, m.c_str(), 450, 2); // UTF-8("")
	}
}

//EMU RESTARTEN
bool EMU_Menu::CamdReset()
{
	if (running) {
		string m = "Please wait for " +  string(EMU_list[selected].procname) + " to restart.";
		CHintBox * CamdResetBox = new CHintBox(LOCALE_EXTRAMENU_EMU_RESTART, m.c_str());
		CamdResetBox->paint();
		system(EMU_list[selected].stop_command);
		sleep(1);
		system(EMU_list[selected].start_command);
		sleep(2);
		system("/usr/local/bin/pzapit -rz >/dev/null 2>&1");
		CamdResetBox->hide();
		delete CamdResetBox;
	}
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

#define DOTFILE_SWAPON "/etc/.swapon"
#define DOTFILE_SWAPRAM "/etc/.swapram"
#define DOTFILE_SWAPPART "/etc/.swappart"
#define DOTFILE_SWAPFILE "/etc/.swapjn"

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

////////////////////////////// BOOTE2 Menu ANFANG ////////////////////////////////////
#define BOOTE2_OPTION_COUNT 2
const CMenuOptionChooser::keyval BOOTE2_OPTIONS[BOOTE2_OPTION_COUNT] =
{
	{ 0, LOCALE_EXTRAMENU_BOOTE2_OFF },
	{ 1, LOCALE_EXTRAMENU_BOOTE2_ON }
};

BOOTE2_Menu::BOOTE2_Menu()
{
	frameBuffer = CFrameBuffer::getInstance();
	width = 600;
	hheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight();
	mheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight();
	height = hheight+13*mheight+ 10;

	x=(((g_settings.screen_EndX- g_settings.screen_StartX)-width) / 2) + g_settings.screen_StartX;
	y=(((g_settings.screen_EndY- g_settings.screen_StartY)-height) / 2) + g_settings.screen_StartY;
}

int BOOTE2_Menu::exec(CMenuTarget* parent, const std::string & actionKey)
{
	int res = menu_return::RETURN_REPAINT;

	if (parent)
		parent->hide();

	BOOTE2Settings();

	return res;
}

void BOOTE2_Menu::hide()
{
	frameBuffer->paintBackgroundBoxRel(x,y, width,height);
}

void BOOTE2_Menu::BOOTE2Settings()
{
#define DOTFILE_BOOTE2 "/etc/.start_enigma2"
	int bootE2 = access(DOTFILE_BOOTE2, R_OK) ? 0 : 1;
	int old_bootE2=bootE2;
	//MENU AUFBAUEN
	CMenuWidget* ExtraMenuSettings = new CMenuWidget(LOCALE_EXTRAMENU_BOOTE2, "settings.raw");
	ExtraMenuSettings->addItem(GenericMenuSeparator);
	ExtraMenuSettings->addItem(GenericMenuBack);
	ExtraMenuSettings->addItem(GenericMenuSeparatorLine);
	CMenuOptionChooser* oj1 = new CMenuOptionChooser(LOCALE_EXTRAMENU_BOOTE2_SELECT, &bootE2, BOOTE2_OPTIONS, BOOTE2_OPTION_COUNT,true);
	ExtraMenuSettings->addItem( oj1 );
	ExtraMenuSettings->exec (NULL, "");
	ExtraMenuSettings->hide ();
	delete ExtraMenuSettings;
	// UEBERPRUEFEN OB SICH WAS GEAENDERT HAT
	if (old_bootE2!=bootE2)
	{
		if (bootE2==1) {
			touch(DOTFILE_BOOTE2);
			//unlink(STARTCAM);
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "Enigma2 booting is enabled. Please reboot.", 450, 2); // UTF-8("")
		} else {
			unlink(DOTFILE_BOOTE2);
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "Neutrino booting is re-enabled.", 450, 2); // UTF-8("")
		}
	}
}
////////////////////////////// BOOTE2 Menu ENDE //////////////////////////////////////

////////////////////////////// BOOTSPARK Menu ANFANG ////////////////////////////////////
#define BOOTSPARK_OPTION_COUNT 2
const CMenuOptionChooser::keyval BOOTSPARK_OPTIONS[BOOTSPARK_OPTION_COUNT] =
{
	{ 0, LOCALE_EXTRAMENU_BOOTSPARK_OFF },
	{ 1, LOCALE_EXTRAMENU_BOOTSPARK_ON }
};

BOOTSPARK_Menu::BOOTSPARK_Menu()
{
	frameBuffer = CFrameBuffer::getInstance();
	width = 600;
	hheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight();
	mheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight();
	height = hheight+13*mheight+ 10;

	x=(((g_settings.screen_EndX- g_settings.screen_StartX)-width) / 2) + g_settings.screen_StartX;
	y=(((g_settings.screen_EndY- g_settings.screen_StartY)-height) / 2) + g_settings.screen_StartY;
}

int BOOTSPARK_Menu::exec(CMenuTarget* parent, const std::string & actionKey)
{
	int res = menu_return::RETURN_REPAINT;

	if (parent)
		parent->hide();

	BOOTSPARKSettings();

	return res;
}

void BOOTSPARK_Menu::hide()
{
	frameBuffer->paintBackgroundBoxRel(x,y, width,height);
}

void BOOTSPARK_Menu::BOOTSPARKSettings()
{
#define DOTFILE_BOOTSPARK "/etc/.start_spark"
	int bootSpark = access(DOTFILE_BOOTSPARK, R_OK) ? 0 : 1;
	int old_bootSpark = bootSpark;
	//MENU AUFBAUEN
	CMenuWidget* ExtraMenuSettings = new CMenuWidget(LOCALE_EXTRAMENU_BOOTSPARK, "settings.raw");
	ExtraMenuSettings->addItem(GenericMenuSeparator);
	ExtraMenuSettings->addItem(GenericMenuBack);
	ExtraMenuSettings->addItem(GenericMenuSeparatorLine);
	CMenuOptionChooser* oj1 = new CMenuOptionChooser(LOCALE_EXTRAMENU_BOOTSPARK_SELECT, &bootSpark, BOOTSPARK_OPTIONS, BOOTSPARK_OPTION_COUNT,true);
	ExtraMenuSettings->addItem( oj1 );
	ExtraMenuSettings->exec (NULL, "");
	ExtraMenuSettings->hide ();
	delete ExtraMenuSettings;
	// UEBERPRUEFEN OB SICH WAS GEAENDERT HAT
	if (old_bootSpark!=bootSpark) {
		if (bootSpark==1) {
			touch(DOTFILE_BOOTSPARK);
			system("fw_setenv -s /etc/bootargs_orig");
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "SPARK booting is enabled. Please reboot.", 450, 2); // UTF-8("")
		} else {
			unlink(DOTFILE_BOOTSPARK);
			system("fw_setenv -s /etc/bootargs_evolux_yaffs2");
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "Evolux (YAFFS2) booting is re-enabled.", 450, 2); // UTF-8("")
		}
	}
}
////////////////////////////// BOOTSPARK Menu ENDE //////////////////////////////////////

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
#define DOTFILE_FRITZCALL "/etc/.fritzcall"
	//UEBERPRUEFEN OB FRITZCALL SCHON LAEUFT
	int fritzcall = access(DOTFILE_FRITZCALL, R_OK) ? 0 : 1;
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
			touch(DOTFILE_FRITZCALL);
			system("/var/plugins/fritzcall/fb.sh start >/dev/null 2>&1 &");
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "FRITZCALLMONITOR activated!", 450, 2); // UTF-8("")
		} else {
			//FRITZCALL BEENDEN
			unlink(DOTFILE_FRITZCALL);
			system("/var/plugins/fritzcall/fb.sh stop >/dev/null 2>&1 &");
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "FRITZCALLMONITOR deactivated!", 450, 2); // UTF-8("")
	}
}
//ENDE FRITZCALL
}
////////////////////////////// FRITZCALL Menu ENDE //////////////////////////////////////

////////////////////////////// Overclocking Menu ANFANG ////////////////////////////////////

#define OC_OPTION_COUNT 10
const CMenuOptionChooser::keyval OC_OPTIONS[OC_OPTION_COUNT] = 
{
	{ 200, NONEXISTANT_LOCALE, "200MHz" },
	{ 250, NONEXISTANT_LOCALE, "250MHz" },
	{ 300, NONEXISTANT_LOCALE, "300MHz" },
	{ 350, NONEXISTANT_LOCALE, "350MHz" },
	{ 400, NONEXISTANT_LOCALE, "400MHz" },
	{ 450, NONEXISTANT_LOCALE, "450MHz (Default)" },
	{ 500, NONEXISTANT_LOCALE, "500MHz" },
	{ 550, NONEXISTANT_LOCALE, "550MHz" },
	{ 600, NONEXISTANT_LOCALE, "600MHz" },
	{ 650, NONEXISTANT_LOCALE, "650MHz" }
};

OC_Menu::OC_Menu()
{
	frameBuffer = CFrameBuffer::getInstance();
	width = 600;
	hheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight();
	mheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight();
	height = hheight+13*mheight+ 10;

	x=(((g_settings.screen_EndX- g_settings.screen_StartX)-width) / 2) + g_settings.screen_StartX;
	y=(((g_settings.screen_EndY- g_settings.screen_StartY)-height) / 2) + g_settings.screen_StartY;
}

int OC_Menu::exec(CMenuTarget* parent, const std::string & actionKey)
{
	int res = menu_return::RETURN_REPAINT;

	if (parent)
		parent->hide();

	OCSettings();

	return res;
}

void OC_Menu::hide()
{
	frameBuffer->paintBackgroundBoxRel(x,y, width,height);
}

void OC_Menu::OCSettings()
{
	int freq = 0;

	if (FILE *pll0 = fopen("/proc/cpu_frequ/pll0_ndiv_mdiv", "r")) {
		char buffer[120];
		while(fgets(buffer, sizeof(buffer), pll0)) {
			if (1 == sscanf(buffer, "SH4 = %d MHZ", &freq))
				break;
		}
		fclose(pll0);
	}

	bool found = false;

	for (int i = 0; i < OC_OPTION_COUNT && !found; i++)
		found = (OC_OPTIONS[i].key == freq);

	if (!found) {
		ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "Unsupported frequency detected. Aborting.", 450, 2); // UTF-8("")
		return;
	}

	int old_freq = freq;

	CMenuWidget* ExtraMenuSettings = new CMenuWidget(LOCALE_EXTRAMENU_OC, "settings.raw");
	ExtraMenuSettings->addItem(GenericMenuSeparator);
	ExtraMenuSettings->addItem(GenericMenuBack);
	ExtraMenuSettings->addItem(GenericMenuSeparatorLine);
	CMenuOptionChooser* oj1 = new CMenuOptionChooser(LOCALE_EXTRAMENU_OC_SELECT, &freq, OC_OPTIONS, OC_OPTION_COUNT,true);
	ExtraMenuSettings->addItem( oj1 );
	ExtraMenuSettings->exec (NULL, "");
	ExtraMenuSettings->hide ();
	delete ExtraMenuSettings;

	if (freq != old_freq) {
		FILE *pll0 = fopen ("/proc/cpu_frequ/pll0_ndiv_mdiv", "w");
		if (!pll0) {
			ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "Modifying frequency failed. Aborting.", 450, 2); // UTF-8("")
			return;
		}
		fprintf(pll0, "%d\n", freq/10 * 256 + 3);
		fclose (pll0);
		ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "CPU frequency updated.", 450, 2); // UTF-8("")
	}

	stringstream tmp_out;
	tmp_out << freq;
	E2_Config *cfg = new E2_Config(E2SETTINGS);
	cfg->set("config.plugins.PinkPanel.oclock", tmp_out.str());
	delete cfg;
}
////////////////////////////// OC Menu ENDE //////////////////////////////////////



// E2 configuration handling functions for keeping ExtraMenu and PinkPanel in sync.

void E2_Config::lock(void){
	int lockfd;
	while ((lockfd = open(lockfile.c_str(), O_RDWR | O_EXCL | O_CREAT)) < -1)
		sleep(1);
	close(lockfd);
}

void E2_Config::unlock(void){
	unlink(lockfile.c_str());
}

E2_Config::E2_Config(const string filename) {
	name = filename;
	lockfile = "/tmp/" + name;
	for (int i = 5; i < lockfile.length(); i++)
		if (lockfile[i] == '/')
			lockfile[i] = '_';
	lock();
	ifstream e2cfg (filename.c_str());
	string line;
	while (e2cfg.good()) {
		if (getline(e2cfg, line)) {
			string::size_type delimiter_pos = line.find_first_of("=", 0);
			if (delimiter_pos > 1)
				cfg[line.substr(0, delimiter_pos)] = line.substr(delimiter_pos + 1);
		}
	}
	e2cfg.close();
}

E2_Config::~E2_Config(void) {
	if (needs_write)
		this->sync();
	unlock();
}

string E2_Config::get(string key) {
	map<string, string>::iterator i;
	i = cfg.find(key);
	if (i != cfg.end())
		return i->second;
	return "";
}

void E2_Config::set(string key, string value) {
	map<string, string>::iterator i;
	i = cfg.find(key);
	if (i == cfg.end() || value.compare(i->second)) {
		cfg[key] = value;
		needs_write = true;
	}
}

void E2_Config::unset(string key) {
	map<string, string>::iterator i;
	i = cfg.find(key);
	if (i != cfg.end()) {
		cfg.erase(i);
		needs_write = true;
	}
}

void E2_Config::unset_hierarchy(string key) {
	map<string, string>::iterator i, j;
	for (i=cfg.begin(); i != cfg.end(); i = j) {
		j = i;
		j++;
		if (!i->first.compare(0, key.length(), key)) {
			switch(i->first[key.length()]) {
				case 0:
				case '.':
					cfg.erase(i);
					needs_write = true;
					break;
				default:
					;
			}
		}
	}
}

void E2_Config::dump() {
	map<string, string>::iterator i;
	for (i=cfg.begin(); i != cfg.end(); ++i)
		cout << i->first << "=" << i->second << endl;
}

void E2_Config::sync() {
	needs_write = false;

	string tmpname = name + ".tmp";
	ofstream tmp (tmpname.c_str());
	if(tmp.is_open()) {
		map<string, string>::iterator i;
		for (i=cfg.begin(); i != cfg.end(); ++i)
			tmp << i->first << "=" << i->second << endl;
		tmp.close();
		rename(tmpname.c_str(), name.c_str());
	}
}


