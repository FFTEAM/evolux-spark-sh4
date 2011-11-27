#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/vfs.h>
#include "libnet.h"
#include <sys/wait.h>

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
#define EMU_OPTION_COUNT 6
#define EMU_ONOFF_OPTION_COUNT 2
#define EMU_RESTART_OPTION_COUNT 2
#define TUNERRESET_OPTION_COUNT 2
#define CORRECTVOLUME_OPTION_COUNT 2
#define AMOUNT_OPTION_COUNT 2
#define DISPLAYTIME_OPTION_COUNT 2
#define WWWDATE_OPTION_COUNT 2
#define SWAP_OPTION_COUNT 3
#define SWAP_ONOFF_OPTION_COUNT 2
#define CHECKFS_OPTION_COUNT 2
#define BOOTE2_OPTION_COUNT 2
/*#define EXTRAMENU_ONOFF_OPTION_COUNT 2

const CMenuOptionChooser::keyval EXTRAMENU_ONOFF_OPTIONS[EXTRAMENU_ONOFF_OPTION_COUNT] =
{
{ 0, LOCALE_EXTRAMENU_ONOFF_OFF },
{ 1, LOCALE_EXTRAMENU_ONOFF_ON },
}; */

////////////////////////////// CORRECTVOLUME Menu ANFANG ////////////////////////////////////
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
	paint();

	CORRECTVOLUMESettings();

	return res;
}

void CORRECTVOLUME_Menu::hide()
{
frameBuffer->paintBackgroundBoxRel(x,y, width,height);
}

void CORRECTVOLUME_Menu::paint()
{
printf("$Id: Volume-Menue Extended  Exp $\n");
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

////////////////////////////// TUNERRESET Menu ANFANG ////////////////////////////////////
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
	{
	parent->hide();
	}
	paint();

	TUNERRESETSettings();

	return res;
	}

void TUNERRESET_Menu::hide()
{
	frameBuffer->paintBackgroundBoxRel(x,y, width,height);
}

void TUNERRESET_Menu::paint()
{
	printf("$Id: TUNER-Menue Exp $\n");
}

void TUNERRESET_Menu::TUNERRESETSettings()
{
	int tuner=0;
	int save_value=0;
	//UEBERPRUEFEN OB RESET SCHON LAEUFT
	FILE* fd1 = fopen("/etc/.reset", "r");
	if(fd1)
	{
	tuner=1;
	fclose(fd1);
	}
	int old_tuner=tuner;
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
	int tuner=1;
	// AUSFUEHREN
	if (tuner==1)
	{
	//TUNERRESET STARTEN
	CHintBox * TunerResetBox = new CHintBox(LOCALE_EXTRAMENU_TUNERRESET_RESTART, "bitte warten, Tuner wird resettet");
TunerResetBox->paint();

	system("/usr/local/bin/pzapit -esb");
	system("sleep 2");
	system("/usr/local/bin/pzapit -lsb");
	TunerResetBox->hide();
	delete TunerResetBox;
	}
}
//ENDE TUNERRESET

////////////////////////////// TUNERRESET Menu ENDE //////////////////////////////////////


////////////////////////////// AMOUNT Menu ANFANG ////////////////////////////////////
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
	paint();

	AMOUNTSettings();

	return res;
}

void AMOUNT_Menu::hide()
{
frameBuffer->paintBackgroundBoxRel(x,y, width,height);
}

void AMOUNT_Menu::paint()
{
printf("$Id: Amount-Menue Exp $\n");
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


////////////////////////////// CheckFS Menu ANFANG ////////////////////////////////////
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
	{
	parent->hide();
	}
	paint();

	CHECKFSSettings();

	return res;
}

void CHECKFS_Menu::hide()
{
frameBuffer->paintBackgroundBoxRel(x,y, width,height);
}

void CHECKFS_Menu::paint()
{
printf("$Id: Amount-Menue Exp $\n");
}

void CHECKFS_Menu::CHECKFSSettings()
{
	int checkfs=0;
	int save_value=0;
	//UEBERPRUEFEN OB CHECKFS SCHON LAEUFT
	FILE* fd1 = fopen("/etc/.checkfs", "r");
	if(fd1)
	{
	checkfs=1;
	fclose(fd1);
	}
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
	save_value=1;
	}
	// ENDE UEBERPRUEFEN OB SICH WAS GEAENDERT HAT

	// AUSFUEHREN NUR WENN SICH WAS GEAENDERT HAT
	if (save_value==1)
	{
	if (checkfs==1)
	{
	//CHECKFS STARTEN
	system("touch /etc/.checkfs");
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "CHECKFS Activated!", 450, 2); // UTF-8("")
	}
	if (checkfs==0)
	{
	//CHECKFS BEENDEN
	system("rm /etc/.checkfs");
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "CHECKFS Deactivated!", 450, 2); // UTF-8("")
	}
}
//ENDE CHECKFS
}
////////////////////////////// CheckFS Menu ENDE //////////////////////////////////////

////////////////////////////// EMU choose Menu ANFANG ////////////////////////////////////

const CMenuOptionChooser::keyval EMU_OPTIONS[EMU_OPTION_COUNT] =
{
	{ 0, LOCALE_EXTRAMENU_EMU_MGCAMD },
	{ 1, LOCALE_EXTRAMENU_EMU_INCUBUS },
	{ 2, LOCALE_EXTRAMENU_EMU_CAMD3 },
	{ 3, LOCALE_EXTRAMENU_EMU_MBOX },
	{ 4, LOCALE_EXTRAMENU_EMU_OSCAM },
	{ 5, LOCALE_EXTRAMENU_EMU_SPCS }
};

const CMenuOptionChooser::keyval EMU_ONOFF_OPTIONS[EMU_ONOFF_OPTION_COUNT] =
{
	{ 0, LOCALE_EXTRAMENU_EMU_OFF },
	{ 1, LOCALE_EXTRAMENU_EMU_ON },
};

const CMenuOptionChooser::keyval EMU_RESTART_OPTIONS[EMU_RESTART_OPTION_COUNT] =
{
	{ 0, LOCALE_EXTRAMENU_EMU_RESTART_OFF },
	{ 1, LOCALE_EXTRAMENU_EMU_RESTART_ON }
}; 

EMU_Menu::EMU_Menu()
{
	frameBuffer = CFrameBuffer::getInstance();
	width = 600;
	hheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight();
	mheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight();
	height = hheight+13*mheight+ 10;

	x=(((g_settings.screen_EndX- g_settings.screen_StartX)-width) / 2) + g_settings.screen_StartX;
	y=(((g_settings.screen_EndY- g_settings.screen_StartY)-height) / 2) + g_settings.screen_StartY;
}
int EMU_Menu::exec(CMenuTarget* parent, const std::string & actionKey)
{
	int res = menu_return::RETURN_REPAINT;
	if(actionKey == "camdreset") 
	{
	this->CamdReset();
	return res;
	}
	if (parent)
	{
	parent->hide();
	}
	paint();

	EMU_Menu_Settings();

	return res;
}

void EMU_Menu::hide()
{
	frameBuffer->paintBackgroundBoxRel(x,y, width,height);
}

void EMU_Menu::paint()
{
	printf("$Id: EMU-Menue Exp $\n");
}

void EMU_Menu::EMU_Menu_Settings()
{
	int emu=0;
	int emu_onoff=0;
	/*int stop=0; */
	int emu_restart=0;
	bool mgcamd=0;
	bool incubus=0;
	bool camd3=0;
	bool mbox=0;
	bool oscam=0;
	bool spcs=0;
	bool newcs=0;
	int save_value=0;

//UEBERPRUEFEN WELCHES EMU LAEUFT
	FILE* fd1 = fopen("/etc/.mgcamd", "r");
	if(fd1)
	{
	emu=0;
	fclose(fd1);
	mgcamd=1;
	emu_onoff=1;
	}
	FILE* fd2 = fopen("/etc/.incubus", "r");
	if(fd2)
	{
	emu=1;
	fclose(fd2);
	incubus=1;
	emu_onoff=1;
	}
	FILE* fd3 = fopen("/etc/.camd3", "r");
	if(fd3)
	{
	emu=2;
	fclose(fd3);
	camd3=1;
	emu_onoff=1;
	}
	FILE* fd4 = fopen("/etc/.mbox", "r");
	if(fd4)
	{
	emu=3;
	fclose(fd4);
	mbox=1;
	emu_onoff=1;
	}
	FILE* fd5 = fopen("/etc/.oscam", "r");
	if(fd5)
	{
	emu=4;
	fclose(fd5);
	oscam=1;
	emu_onoff=1;
	}
	FILE* fd6 = fopen("/etc/.spcs", "r");
	if(fd6)
	{
	emu=5;
	fclose(fd6);
	spcs=1;
	emu_onoff=1;
	}

	int old_emu=emu;
	int old_emu_onoff=emu_onoff;
	int old_emu_restart=emu_restart;
	char mgcamdversion[7] = "N/A";
	char incubusversion[7] = "N/A";
	char camd3version[7] = "N/A";
	char mboxversion[7] = "N/A";
	char oscamversion[7] = "N/A";
	char newcsversion[7] = "N/A";
	char spcsversion[7] = "N/A";
	FILE* fdemu = fopen("/usr/bin/versions.txt", "r");
	if(fdemu)
	{
	char buffer[120];
	while(fgets(buffer, 120, fdemu)!=NULL)
	{
	sscanf(buffer, "mgcamd=%7s", mgcamdversion);
	sscanf(buffer, "incubuscamd=%7s", incubusversion);
	sscanf(buffer, "camd3=%7s", camd3version);
	sscanf(buffer, "mbox=%7s", mboxversion);
	sscanf(buffer, "oscam=%7s", oscamversion);
	sscanf(buffer, "spcs=%7s", spcsversion);
	sscanf(buffer, "newcs=%7s", newcsversion);
	}
	fclose(fdemu);
	}

	//MENU AUFBAUEN
	CMenuWidget* ExtraMenuSettings = new CMenuWidget(LOCALE_EXTRAMENU_EMU, "settings.raw");
	ExtraMenuSettings->addItem(GenericMenuSeparator);
	ExtraMenuSettings->addItem(GenericMenuBack);
	ExtraMenuSettings->addItem(GenericMenuSeparatorLine);
	CMenuOptionChooser* oj1 = new CMenuOptionChooser(LOCALE_EXTRAMENU_EMU_ONOFF, &emu_onoff, EMU_ONOFF_OPTIONS, 		EMU_ONOFF_OPTION_COUNT,true);
	ExtraMenuSettings->addItem( oj1 );
	ExtraMenuSettings->addItem(GenericMenuSeparatorLine);
	CMenuOptionChooser* oj2 = new CMenuOptionChooser(LOCALE_EXTRAMENU_EMU_SELECT, &emu, EMU_OPTIONS, EMU_OPTION_COUNT,true);
	ExtraMenuSettings->addItem( oj2 );
	ExtraMenuSettings->addItem(new CMenuForwarder(LOCALE_EXTRAMENU_EMU_RESTART, true, "", this, "camdreset", CRCInput::RC_red, NEUTRINO_ICON_BUTTON_RED));
	ExtraMenuSettings->addItem(GenericMenuSeparatorLine);
	FILE* fdcamd3_installed = fopen("/usr/bin/camd3", "r");
	FILE* fdmgcamd_installed = fopen("/usr/bin/mgcamd", "r");
	FILE* fdmbox_installed = fopen("/usr/bin/mbox", "r");
	FILE* fdincubuscamd_installed = fopen("/usr/bin/incubusCamd", "r");
	FILE* fdoscam_installed = fopen("/usr/bin/oscam", "r");
	FILE* fdspcs_installed = fopen("/usr/bin/spcs", "r");
	FILE* fdnewcs_installed = fopen("/usr/bin/newcs", "r");
	if(fdmgcamd_installed)
	{
	ExtraMenuSettings->addItem( new CMenuForwarder(LOCALE_EXTRAMENU_EMU_MGCAMDVERSION, false, mgcamdversion));
	}
	if(fdincubuscamd_installed)
	{
	ExtraMenuSettings->addItem( new CMenuForwarder(LOCALE_EXTRAMENU_EMU_INCUBUSVERSION, false, incubusversion));
	}
	if(fdcamd3_installed)
	{
	ExtraMenuSettings->addItem( new CMenuForwarder(LOCALE_EXTRAMENU_EMU_CAMD3VERSION, false, camd3version));
	}
	if(fdmbox_installed)
	{
	ExtraMenuSettings->addItem( new CMenuForwarder(LOCALE_EXTRAMENU_EMU_MBOXVERSION, false, mboxversion));
	}
	if(fdoscam_installed)
	{
	ExtraMenuSettings->addItem( new CMenuForwarder(LOCALE_EXTRAMENU_EMU_OSCAMVERSION, false, oscamversion));
	}
	if(fdspcs_installed)
	{
	ExtraMenuSettings->addItem( new CMenuForwarder(LOCALE_EXTRAMENU_EMU_SPCSVERSION, false, spcsversion));
	}
	if(fdnewcs_installed)
	{
	ExtraMenuSettings->addItem( new CMenuForwarder(LOCALE_EXTRAMENU_EMU_NEWCSVERSION, false, newcsversion));
	}
	ExtraMenuSettings->exec (NULL, "");
	ExtraMenuSettings->hide ();
	delete ExtraMenuSettings;

	// UEBERPRUEFEN OB SICH WAS GEAENDERT HAT
	if ((old_emu!=emu) || (old_emu_onoff!=emu_onoff) || (old_emu_restart!=emu_restart))
	{
	save_value=1;
	}
	// ENDE UEBERPRUEFEN OB SICH WAS GEAENDERT HAT

	// AUSFUEHREN NUR WENN SICH WAS GEAENDERT HAT
	if (save_value==1)
	{
	//MGCAMD STARTEN
	if (emu_onoff==1)
	{
	if (emu==0)
	{
	if (camd3==1)
	{
	system("rm /etc/.camd3");
	system("kill $(pidof camd3)");
	}
	if (incubus==1)
	{
	system("rm /etc/.incubus");
	system("kill $(pidof incubusCamd)");
	}
	if (mbox==1)
	{
	system("rm /etc/.mbox");
	system("kill $(pidof mbox)");
	}
	if (oscam==1)
	{
	system("rm /etc/.oscam");
	system("kill $(pidof oscam)");
	}
	if (spcs==1)
	{
	system("rm /etc/.spcs");
	system("kill $(pidof spcs)");
	}
	system("touch /etc/.mgcamd");
	system("rm /tmp/camd.socket");
	system("/usr/bin/mgcamd /var/keys/mg_cfg &");
//	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, g_Locale->getText(LOCALE_SETTINGS_SAVED), 450, 2); // UTF-8("")
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, " MGCAMD activated!", 450, 2); // UTF-8("")
}
}
//MGCAMD BEENDEN
if (emu_onoff==0)
{
	if (emu==0)
	{
	system("rm /etc/.mgcamd");
	system("kill $(pidof mgcamd)");
	emu_onoff=0;
	//ShowHintUTF(LOCALE_MESSAGEBOX_INFO, g_Locale->getText(LOCALE_SETTINGS_SAVED), 450, 2); // UTF-8("")
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, " MGCAMD deactivated!", 450, 2); // UTF-8("")
	}
}
//ENDE MGCAMD

//INCUBUS STARTEN
if (emu_onoff==1)
{
	if (emu==1)
	{
	if (camd3==1)
	{
	system("rm /etc/.camd3");
	system("kill $(pidof camd3)");
	}
	if (mgcamd==1)
	{
	system("rm /etc/.mgcamd");
	system("kill $(pidof mgcamd)");
	}
	if (mbox==1)
	{
	system("rm /etc/.mbox");
	system("kill $(pidof mbox)");
	}
	if (oscam==1)
	{
	system("rm /etc/.oscam");
	system("kill $(pidof oscam)");
	}
	if (spcs==1)
	{
	system("rm /etc/.spcs");
	system("kill $(pidof spcs)");
	}
	system("touch /etc/.incubus");
	system("rm /tmp/camd.socket");
	system("/usr/bin/incubusCamd &");
	//ShowHintUTF(LOCALE_MESSAGEBOX_INFO, g_Locale->getText(LOCALE_SETTINGS_SAVED), 450, 2); // UTF-8("")
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, " INCUBUS activated!", 450, 2); // UTF-8("")
	}
}
//INCUBUS BEENDEN
if (emu_onoff==0)
{
	if (emu==1)
	{
	system("rm /etc/.incubus");
	system("kill $(pidof incubusCamd)");
	emu_onoff=0;
	//ShowHintUTF(LOCALE_MESSAGEBOX_INFO, g_Locale->getText(LOCALE_SETTINGS_SAVED), 450, 2); // UTF-8("")
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, " INCUBUS deactivated!", 450, 2); // UTF-8("")
	}
}
//ENDE INCUBUS

//CAMD3 STARTEN
if (emu_onoff==1)
{
	if (emu==2)
	{
	if (mgcamd==1)
	{
	system("rm /etc/.mgcamd");
	system("kill $(pidof mgcamd)");
	}
	if (incubus==1)
	{
	system("rm /etc/.incubus");
	system("kill $(pidof incubusCamd)");
	}
	if (mbox==1)
	{
	system("rm /etc/.mbox");
	system("kill $(pidof mbox)");
	}
	if (oscam==1)
	{
	system("rm /etc/.oscam");
	system("kill $(pidof oscam)");
	}
	if (spcs==1)
	{
	system("rm /etc/.spcs");
	system("kill $(pidof spcs)");
	}
	system("touch /etc/.camd3");
	system("/usr/bin/camd3 /var/keys/camd3.config &");
	//ShowHintUTF(LOCALE_MESSAGEBOX_INFO, g_Locale->getText(LOCALE_SETTINGS_SAVED), 450, 2); // UTF-8("")
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, " CAMD3 activated!", 450, 2); // UTF-8("")
	}
}
//CAMD3 BEENDEN
if (emu_onoff==0)
{
	if (emu==2)
	{
	system("rm /etc/.camd3");
	system("kill $(pidof camd3)");
	emu_onoff=0;
	//ShowHintUTF(LOCALE_MESSAGEBOX_INFO, g_Locale->getText(LOCALE_SETTINGS_SAVED), 450, 2); // UTF-8("")
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, " CAMD3 deactivated!", 450, 2); // UTF-8("")
	}
}
//ENDE CAMD3

//MBOX STARTEN
if (emu_onoff==1)
{
	if (emu==3)
	{
	if (mgcamd==1)
	{
	system("rm /etc/.mgcamd");
	system("kill $(pidof mgcamd)");
	}
	if (incubus==1)
	{
	system("rm /etc/.incubus");
	system("kill $(pidof incubusCamd)");
	}
	if (camd3==1)
	{
	system("rm /etc/.camd3");
	system("kill $(pidof camd3)");
	}
	if (oscam==1)
	{
	system("rm /etc/.oscam");
	system("kill $(pidof oscam)");
	}
	if (spcs==1)
	{
	system("rm /etc/.spcs");
	system("kill $(pidof spcs)");
	}
	system("touch /etc/.mbox");
	system("/usr/bin/mbox /var/keys/mbox.cfg &");
	//ShowHintUTF(LOCALE_MESSAGEBOX_INFO, g_Locale->getText(LOCALE_SETTINGS_SAVED), 450, 2); // UTF-8("")
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, " MBOX activated!", 450, 2); // UTF-8("")
	}
}
//MBOX BEENDEN
if (emu_onoff==0)
	{
	if (emu==3)
	{
	system("rm /etc/.mbox");
	system("kill $(pidof mbox)");
	system("rm /tmp/share.*");
	system("rm /tmp/mbox.ver");
	system("rm /tmp/*.info");
	emu_onoff=0;
	//ShowHintUTF(LOCALE_MESSAGEBOX_INFO, g_Locale->getText(LOCALE_SETTINGS_SAVED), 450, 2); // UTF-8("")
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, " MBOX deactivated!", 450, 2); // UTF-8("")
	}
}
//ENDE MBOX

//OSCAM STARTEN
if (emu_onoff==1)
{
	if (emu==4)
	{
	if (mgcamd==1)
	{
	system("rm /etc/.mgcamd");
	system("kill $(pidof mgcamd)");
	}
	if (incubus==1)
	{
	system("rm /etc/.incubus");
	system("kill $(pidof incubusCamd)");
	}
	if (camd3==1)
	{
	system("rm /etc/.camd3");
	system("kill $(pidof camd3)");
	}
	if (mbox==1)
	{
	system("rm /etc/.mbox");
	system("kill $(pidof mbox)");
	}
	if (spcs==1)
	{
	system("rm /etc/.spcs");
	system("kill $(pidof spcs)");
	}
	system("touch /etc/.oscam");
	system("/usr/bin/oscam -c /var/keys &");
	//ShowHintUTF(LOCALE_MESSAGEBOX_INFO, g_Locale->getText(LOCALE_SETTINGS_SAVED), 450, 2); // UTF-8("")
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, " OSCAM activated!", 450, 2); // UTF-8("")
	}
}
//OSCAM BEENDEN
if (emu_onoff==0)
{
	if (emu==4)
	{
	system("rm /etc/.oscam");
	system("kill $(pidof oscam)");
	emu_onoff=0;
	//ShowHintUTF(LOCALE_MESSAGEBOX_INFO, g_Locale->getText(LOCALE_SETTINGS_SAVED), 450, 2); // UTF-8("")
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, " OSCAM deactivated!", 450, 2); // UTF-8("")
	}
}
//ENDE OSCAM

//SPCS STARTEN
if (emu_onoff==1)
{
	if (emu==5)
	{
	if (mgcamd==1)
	{
	system("rm /etc/.mgcamd");
	system("kill $(pidof mgcamd)");
	}
	if (incubus==1)
	{
	system("rm /etc/.incubus");
	system("kill $(pidof incubusCamd)");
	}
	if (camd3==1)
	{
	system("rm /etc/.camd3");
	system("kill $(pidof camd3)");
	}
	if (mbox==1)
	{
	system("rm /etc/.mbox");
	system("kill $(pidof mbox)");
	}
	if (oscam==1)
	{
	system("rm /etc/.oscam");
	system("kill $(pidof oscam)");
	}
	system("touch /etc/.spcs");
	system("/usr/bin/spcs -c /var/keys &");
	//ShowHintUTF(LOCALE_MESSAGEBOX_INFO, g_Locale->getText(LOCALE_SETTINGS_SAVED), 450, 2); // UTF-8("")
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, " SPCS activated!", 450, 2); // UTF-8("")
	}
}
//SPCS BEENDEN
if (emu_onoff==0)
{
	if (emu==5)
	{
	system("rm /etc/.spcs");
	system("kill $(pidof spcs)");
	emu_onoff=0;
	//ShowHintUTF(LOCALE_MESSAGEBOX_INFO, g_Locale->getText(LOCALE_SETTINGS_SAVED), 450, 2); // UTF-8("")
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, " SPCS deactivated!", 450, 2); // UTF-8("")
	}
}
//ENDE SPCS

}
}
//EMU RESTARTEN
/*if (emu_restart==1) */
bool EMU_Menu::CamdReset()
{
	int emu=0;
	int emu_onoff=0;
	bool mgcamd=0;
	bool incubus=0;
	bool camd3=0;
	bool mbox=0;
	bool oscam=0;
	bool newcs=0;
	bool spcs=0;
//UEBERPRUEFEN WELCHES EMU LAEUFT
	FILE* fd1 = fopen("/etc/.mgcamd", "r");
	if(fd1)
	{
	emu=0;
	fclose(fd1);
	mgcamd=1;
	emu_onoff=1;
	}
	FILE* fd2 = fopen("/etc/.incubus", "r");
	if(fd2)
	{
	emu=1;
	fclose(fd2);
	incubus=1;
	emu_onoff=1;
	}
	FILE* fd3 = fopen("/etc/.camd3", "r");
	if(fd3)
	{
	emu=2;
	fclose(fd3);
	camd3=1;
	emu_onoff=1;
	}
	FILE* fd4 = fopen("/etc/.mbox", "r");
	if(fd4)
	{
	emu=3;
	fclose(fd4);
	mbox=1;
	emu_onoff=1;
	}
	FILE* fd5 = fopen("/etc/.oscam", "r");
	if(fd5)
	{
	emu=4;
	fclose(fd5);
	oscam=1;
	emu_onoff=1;
	}
	FILE* fd6 = fopen("/etc/.spcs", "r");
	if(fd6)
	{
	emu=5;
	fclose(fd6);
	spcs=1;
	emu_onoff=1;
	}
	if(emu_onoff==1)
	{
	CHintBox * CamdResetBox = new CHintBox(LOCALE_EXTRAMENU_EMU_RESTART, "please wait, Camd restarting");
	CamdResetBox->paint();
	//mgc
	if (emu==0)
	{
	system("kill $(pidof mgcamd)");
	system("sleep 2");
	system("/usr/bin/mgcamd /var/keys/mg_cfg &");
	}
	//incubus
	if (emu==1)
	{
	system("kill $(pidof incubusCamd)");
	system("sleep 2");
	system("/usr/bin/incubusCamd &");
	}
	//camd3
	if (emu==2)
	{
	system("kill $(pidof camd3)");
	system("sleep 2");
	system("/usr/bin/camd3 /var/keys/camd3.config &");
	}
	//mbox
	if (emu==3)
	{
	system("kill $(pidof mbox)");
	system("sleep 2");
	system("/usr/bin/mbox &");
	}
	//oscam
	if (emu==4)
	{
	system("kill $(pidof oscam)");
	system("sleep 2");
	system("/usr/bin/oscam -c /var/keys &");
	}
	system("sleep 5");
	system("/usr/local/bin/pzapit -rz");
	CamdResetBox->hide();
	delete CamdResetBox;
	}
	//spcs
	if (emu==5)
	{
	system("kill $(pidof spcs)");
	system("sleep 2");
	system("/usr/bin/spcs -c /var/keys &");
	}
	system("sleep 5");
	system("/usr/local/bin/pzapit -rz");
	CamdResetBox->hide();
	delete CamdResetBox;
	}
}
////////////////////////////// EMU Menu ENDE //////////////////////////////////////

////////////////////////////// DISPLAYTIME Menu ANFANG ////////////////////////////////////
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
	{
	parent->hide();
	}
	paint();

	DISPLAYTIMESettings();

	return res;
}

void DISPLAYTIME_Menu::hide()
{
frameBuffer->paintBackgroundBoxRel(x,y, width,height);
}

void DISPLAYTIME_Menu::paint()
{
printf("$Id: DisplayTime-Menue Exp $\n");
}

void DISPLAYTIME_Menu::DISPLAYTIMESettings()
{
	int displaytime=0;
	int save_value=0;
	//UEBERPRUEFEN OB DISPLAYTIME SCHON LAEUFT
	FILE* fd1 = fopen("/etc/.time", "r");
	if(fd1)
	{
	displaytime=1;
	fclose(fd1);
	}
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
	save_value=1;
	}
	// ENDE UEBERPRUEFEN OB SICH WAS GEAENDERT HAT

	// AUSFUEHREN NUR WENN SICH WAS GEAENDERT HAT
	if (save_value==1)
	{
	if (displaytime==1)
	{
	//DisplayTime STARTEN
	system("touch /etc/.time");
	system("touch /etc/.scroll");
//	system("/etc/init.d/DisplayTime.sh &");
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "DISPLAYTIME Activated!\nPlease REBOOT", 450, 2); // UTF-8("")
	}
	if (displaytime==0)
	{
	//DisplayTime BEENDEN
	system("rm /etc/.time");
	system("rm /etc/.scroll");
//	system("killall -9 DisplayTime.sh");
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "DISPLAYTIME Deactivated!\nPlease REBOOT", 450, 2); // UTF-8("")
	}
}
//ENDE DisplayTime
}
////////////////////////////// DisplayTime Menu ENDE //////////////////////////////////////

////////////////////////////// WWWDATE Menu ANFANG ////////////////////////////////////
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
	{
	parent->hide();
	}
	paint();

	WWWDATESettings();

	return res;
}

void WWWDATE_Menu::hide()
{
frameBuffer->paintBackgroundBoxRel(x,y, width,height);
}

void WWWDATE_Menu::paint()
{
printf("$Id: wwwDate-Menue Exp $\n");
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

////////////////////////////// SWAP choose Menu ANFANG ////////////////////////////////////

const CMenuOptionChooser::keyval SWAP_OPTIONS[SWAP_OPTION_COUNT] =
{
	{ 0, LOCALE_EXTRAMENU_SWAP_SWAPRAM },
	{ 1, LOCALE_EXTRAMENU_SWAP_SWAPPART },
	{ 2, LOCALE_EXTRAMENU_SWAP_SWAPFILE }
};

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
	{
	parent->hide();
	}
	paint();

	SWAP_Menu_Settings();

	return res;
}

void SWAP_Menu::hide()
{
	frameBuffer->paintBackgroundBoxRel(x,y, width,height);
}

void SWAP_Menu::paint()
{
	printf("$Id: SWAP-Menue Exp $\n");
}

void SWAP_Menu::SWAP_Menu_Settings()
{
	int swap=0;
	int swap_onoff=0;
	bool swapram=0;
	bool swappart=0;
	bool swapfile=0;
	int save_value=0;

//UEBERPRUEFEN WELCHES SWAP LAEUFT
	FILE* fd1 = fopen("/etc/.swapram", "r");
	if(fd1)
	{
	swap=0;
	fclose(fd1);
	swapram=1;
	swap_onoff=1;
	}
	FILE* fd2 = fopen("/etc/.swappart", "r");
	if(fd2)
	{
	swap=1;
	fclose(fd2);
	swappart=1;
	swap_onoff=1;
	}
	FILE* fd3 = fopen("/etc/.swapfile", "r");
	if(fd3)
	{
	swap=2;
	fclose(fd3);
	swapfile=1;
	swap_onoff=1;
	}

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
	if ((old_swap!=swap) || (old_swap_onoff!=swap_onoff))
	{
	save_value=1;
	}
	// ENDE UEBERPRUEFEN OB SICH WAS GEAENDERT HAT

	// AUSFUEHREN NUR WENN SICH WAS GEAENDERT HAT
	if (save_value==1)
	{
	//SWAPRAM STARTEN
	if (swap_onoff==1)
	{
	if (swap==0)
	{
	if (swapfile==1)
	{
	system("rm /etc/.swapfile");
	}
	if (swappart==1)
	{
	system("rm /etc/.swappart");
	}
	system("touch /etc/.swapram");
	system("touch /etc/.swapon");
	system("/etc/init.d/Swap.sh &");
	//ShowHintUTF(LOCALE_MESSAGEBOX_INFO, g_Locale->getText(LOCALE_SETTINGS_SAVED), 450, 2); // UTF-8("")
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, " SWAPRAM activated!", 450, 2); // UTF-8("")
	}
}
//SWAPRAM BEENDEN
if (swap_onoff==0)
{
	if (swap==0)
	{
	system("rm /etc/.swapram");
	system("rm /etc/.swapon");
	system("swapoff -a");
	swap_onoff=0;
	//ShowHintUTF(LOCALE_MESSAGEBOX_INFO, g_Locale->getText(LOCALE_SETTINGS_SAVED), 450, 2); // UTF-8("")
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, " SWAPRAM deactivated!", 450, 2); // UTF-8("")
	}
}
//ENDE SWAPRAM

//SWAPPART STARTEN
if (swap_onoff==1)
{
	if (swap==1)
	{
	if (swapfile==1)
	{
	system("rm /etc/.swapfile");
	}
	if (swapram==1)
	{
	system("rm /etc/.swapram");
	}
	system("touch /etc/.swappart");
	system("touch /etc/.swapon");
	system("/etc/init.d/Swap.sh &");
	//ShowHintUTF(LOCALE_MESSAGEBOX_INFO, g_Locale->getText(LOCALE_SETTINGS_SAVED), 450, 2); // UTF-8("")
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, " SWAPPART activated!", 450, 2); // UTF-8("")
	}
}
//SWAPPART BEENDEN
if (swap_onoff==0)
{
	if (swap==1)
	{
	system("rm /etc/.swappart");
	system("rm /etc/.swapon");
	system("swapoff -a");
	swap_onoff=0;
	//ShowHintUTF(LOCALE_MESSAGEBOX_INFO, g_Locale->getText(LOCALE_SETTINGS_SAVED), 450, 2); // UTF-8("")
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, " SWAPPART deactivated!", 450, 2); // UTF-8("")
	}
}
//ENDE SWAPPART

//SWAPFILE STARTEN
if (swap_onoff==1)
{
	if (swap==2)
	{
	if (swapram==1)
	{
	system("rm /etc/.swapram");
	}
	if (swappart==1)
	{
	system("rm /etc/.swappart");
	}
	system("touch /etc/.swapfile");
	system("touch /etc/.swapon");
	system("/etc/init.d/Swap.sh &");
	//ShowHintUTF(LOCALE_MESSAGEBOX_INFO, g_Locale->getText(LOCALE_SETTINGS_SAVED), 450, 2); // UTF-8("")
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, " SWAPFILE activated!", 450, 2); // UTF-8("")
	}
}
//SWAPFILE BEENDEN
if (swap_onoff==0)
{
	if (swap==2)
	{
	system("rm /etc/.swapfile");
	system("rm /etc/.swapon");
	system("swapoff /dev/loop0");
	system("losetup -d /dev/loop0");
	system("swapoff -a");
	swap_onoff=0;
	//ShowHintUTF(LOCALE_MESSAGEBOX_INFO, g_Locale->getText(LOCALE_SETTINGS_SAVED), 450, 2); // UTF-8("")
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, " SWAPFILE Deactivated!", 450, 2); // UTF-8("")
	}
}
//ENDE SWAPFILE
}
}

////////////////////////////// SWAP Menu ENDE //////////////////////////////////////

////////////////////////////// BOOTE2 Menu ANFANG ////////////////////////////////////
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
	{
	parent->hide();
	}
	paint();

	BOOTE2Settings();

	return res;
}

void BOOTE2_Menu::hide()
{
frameBuffer->paintBackgroundBoxRel(x,y, width,height);
}

void BOOTE2_Menu::paint()
{
printf("$Id: Volume-Menue Extended  Exp $\n");
}

void BOOTE2_Menu::BOOTE2Settings()
{
	int bootE2=0;
	int save_value=0;
	//UEBERPRUEFEN OB BOOTE2 SCHON LAEUFT
	FILE* fd1 = fopen("/etc/.start_enigma2", "r");
	if(fd1)
	{
	bootE2=1;
	fclose(fd1);
	}
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
	save_value=1;
	}
	// ENDE UEBERPRUEFEN OB SICH WAS GEAENDERT HAT

	// AUSFUEHREN NUR WENN SICH WAS GEAENDERT HAT
	if (save_value==1)
	{
	if (bootE2==1)
	{
	//BOOTE2 STARTEN
	system("touch /etc/.start_enigma2");
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "Enigma2 Activated, please reboot!", 450, 2); // UTF-8("")
	}
	bootE2=0;
	old_bootE2=0;
	/*if (bootE2==0)
	{
	//BOOTE2 BEENDEN
	system("rm /etc/.bootE2");
	ShowHintUTF(LOCALE_MESSAGEBOX_INFO, "BOOTE2 Deactivated!", 450, 2); // UTF-8("")
	}*/
}
//ENDE BOOTE2
}
////////////////////////////// BOOTE2 Menu ENDE //////////////////////////////////////
