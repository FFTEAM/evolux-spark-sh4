/*
Neutrino-GUI - DBoxII-Project

Copyright (C) 2001 Steffen Hehn 'McClean'
Homepage: http://dbox.cyberphoria.org/

Kommentar:

Diese GUI wurde von Grund auf neu programmiert und sollte nun vom
Aufbau und auch den Ausbaumoeglichkeiten gut aussehen. Neutrino basiert
auf der Client-Server Idee, diese GUI ist also von der direkten DBox-
Steuerung getrennt. Diese wird dann von Daemons uebernommen.


License: GPL

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef __extramenu__
#define __extramenu__

#include <string>
#include <vector>

#include <driver/framebuffer.h>
#include <system/lastchannel.h>
#include <system/setting_helpers.h>
#include <configfile.h>

#define EXTRA_SETTINGS_FILE CONFIGDIR "/extra.conf"

class CExtraMenuSetup : public CMenuWidget
{
	private:
		int width, selected;
		int showExtraMenuSetup();
	public:
		CExtraMenuSetup();
		~CExtraMenuSetup();
		int exec(CMenuTarget* parent, const std::string & actionKey);
};

class EMU_Menu : public CMenuTarget
{
	private:
		int width, selected;
		int suspended;
		int installed_count;
		int update_installed();
		int update_selected();
	public:
		struct emu_list
		{
			const char *procname;
			const char *start_command;
			const char *stop_command;
			bool installed;
			CMenuForwarderNonLocalized *cmf;
		};
		int get_installed_count();
		EMU_Menu();
		void hide();
		int exec(CMenuTarget* parent, const std::string & actionKey);
		void EMU_Menu_Settings();
		void suspend();
		void resume();

};

#ifdef ENABLE_GRAPHLCD
#include <time.h>
#include <string>
#include <semaphore.h>

#include <glcdgraphics/bitmap.h>
#include <glcdgraphics/font.h>
#include <glcddrivers/config.h>
#include <glcddrivers/driver.h>
#include <glcddrivers/drivers.h>

static const char * kDefaultConfigFile = "/etc/graphlcd.conf";

#ifndef MAX
#define MAX(a,b)(((a)<(b)) ? (b) : (a))
#endif

class nGLCD
{
	private:
		GLCD::cDriver * lcd;
		GLCD::cFont font_channel;
		GLCD::cFont font_epg;
		GLCD::cFont font_time;
		int fontsize_channel;
		int fontsize_epg;
		int fontsize_time;
		int percent_channel;
		int percent_time;
		int percent_epg;
		int percent_bar;
		int percent_space;
		GLCD::cBitmap * bitmap;
		std::string Channel;
		std::string Epg;
		std::string stagingChannel;
		std::string stagingEpg;
		int Scale;
		time_t now;
		struct tm *tm;
		bool channelLocked;
		bool doRescan;
		bool doSuspend;
		bool doStandby;
		bool doStandbyTime;
		bool doExit;
		bool doScrollChannel;
		bool doScrollEpg;
		bool doShowVolume;
		pthread_t thrGLCD;
		pthread_mutex_t mutex;
		void updateFonts();
		void Exec();
		std::string scrollChannel;
		std::string scrollEpg;
	public:
		bool fonts_initialized;
		bool doMirrorOSD;
		nGLCD();
		~nGLCD();
		void DeInit();
		static void Lock();
		static void Unlock();
		void mainLock();
		void mainUnlock();
		static void lockChannel(std::string txt);
		static void unlockChannel();
		static void* Run(void *);
		static void MirrorOSD(bool b = true);
		static void Update();
		static void Suspend();
		static void StandbyMode(bool);
		static void ShowVolume(bool);
		static void Resume();
		static void Exit();
		void Rescan();
		sem_t sem;
};

class GLCD_Menu;

class GLCD_Menu_Notifier : public CChangeObserver
{
	private:
		GLCD_Menu* parent;
	public:
		GLCD_Menu_Notifier();
		bool changeNotify(const neutrino_locale_t, void *);
};


class GLCD_Menu : public CMenuTarget
{
	private:
		int width;
		int selected;
		static int color2index(uint32_t color);
		GLCD_Menu_Notifier *notifier;
	public:
		static uint32_t index2color(int i);
		GLCD_Menu();
		void hide();
		int exec(CMenuTarget* parent, const std::string & actionKey);
		void GLCD_Menu_Settings();
};

#endif // ENABLE_GRAPHLCD

class KernelOptions_Menu : public CMenuTarget
{
	private:
		int width, selected;
		struct module {
			std::string comment;
			std::vector<std::pair <std::string,std::string> > moduleList;
			int active_orig;
			int active;
			bool installed;
		};
		std::vector<module> modules;
	public:
		KernelOptions_Menu();
		void hide();
		int exec(CMenuTarget* parent, const std::string & actionKey);
		void Settings();
};

class CEvoluxSoftwareUpdate : public CMenuTarget
{
	private:
		int width, selected;
	public:
		CEvoluxSoftwareUpdate();
		bool CheckUpdate();
		void hide();
		int exec(CMenuTarget* parent, const std::string & actionKey);
		void Settings();
};

#endif //__extramenu__
// vim:ts=4
