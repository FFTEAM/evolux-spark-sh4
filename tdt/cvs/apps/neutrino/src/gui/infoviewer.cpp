/*
	Neutrino-GUI  -   DBoxII-Project

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
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <string>
#include <algorithm>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/vfs.h>

#include <fcntl.h>

#include <gui/scale.h>
#include <gui/infoviewer.h>

#include <gui/widget/icons.h>
#include <gui/widget/hintbox.h>
#include <gui/widget/buttons.h>

#include <daemonc/remotecontrol.h>

#include <global.h>
#include <neutrino.h>
#include <gui/customcolor.h>
#include <gui/pictureviewer.h>


#include <sys/timeb.h>
#include <time.h>
#include <sys/param.h>
#include <zapit/satconfig.h>
#include <zapit/frontend_c.h>
#include <video_cs.h>

#ifndef DUCKBOX
#define DUCKBOX
#endif

void sectionsd_getEventsServiceKey(t_channel_id serviceUniqueKey, CChannelEventList &eList, char search = 0, std::string search_text = "");
void sectionsd_getCurrentNextServiceKey(t_channel_id uniqueServiceKey, CSectionsdClient::responseGetCurrentNextInfoChannelID& current_next );

extern CRemoteControl *g_RemoteControl;	/* neutrino.cpp */
extern CPictureViewer * g_PicViewer;
extern CFrontend * frontend;
extern cVideo * videoDecoder;

#define COL_INFOBAR_BUTTONS            (COL_INFOBAR_SHADOW + 1)
#define COL_INFOBAR_BUTTONS_BACKGROUND (COL_INFOBAR_SHADOW_PLUS_1)

#define ICON_LARGE_WIDTH 26
#define ICON_SMALL_WIDTH 16
#define ICON_LARGE 30
#define ICON_SMALL 18
#define ICON_Y_1 18

#define ICON_OFFSET (2 + ICON_LARGE_WIDTH + 2 + ICON_LARGE_WIDTH + 2 + ICON_SMALL_WIDTH + 2)

#define BOTTOM_BAR_OFFSET 0
#define SHADOW_OFFSET 6
#define borderwidth 4
#define LEFT_OFFSET 5
#define ASIZE 100

// in us
#define FADE_TIME 40000

#define LCD_UPDATE_TIME_TV_MODE (60 * 1000 * 1000)

#define ROUND_RADIUS 7

int time_left_width;
int time_dot_width;
int time_width;
int time_height;
bool newfreq = true;
char old_timestr[10];
static event_id_t last_curr_id = 0, last_next_id = 0;

extern CZapitClient::SatelliteList satList;
static bool sortByDateTime (const CChannelEvent& a, const CChannelEvent& b)
{
        return a.startTime < b.startTime;
}

extern int timeshift;
extern unsigned char file_prozent;
extern std::string g_file_epg;
extern std::string g_file_epg1;
extern bool autoshift;
extern uint32_t shift_timer;

#define RED_BAR 40
#define YELLOW_BAR 70
#define GREEN_BAR 100
#define BAR_BORDER 1
#define BAR_WIDTH 72 //(68+BAR_BORDER*2)
#define BAR_HEIGHT 12 //(13 + BAR_BORDER*2)
#define TIME_BAR_HEIGHT 12
// InfoViewer: H 63 W 27
#define NUMBER_H 63
#define NUMBER_W 27
extern char act_emu_str[20];
extern std::string ext_channel_name;
int m_CA_Status;
static CScale * timescale;
static CScale * hddscale;
static CScale * varscale;
extern bool timeset;

bool logo_ok = false;

CInfoViewer::CInfoViewer ()
{
  info_CurrentNext.current_zeit.startzeit = 0;
  info_CurrentNext.current_zeit.dauer = 0;
  info_CurrentNext.flags = 0;
  Init();
}

void CInfoViewer::Init()
{
  frameBuffer = CFrameBuffer::getInstance ();

  BoxStartX = BoxStartY = BoxEndX = BoxEndY = 0;
  recordModeActive = false;
  is_visible = false;
  showButtonBar = false;
  //gotTime = g_Sectionsd->getIsTimeSet ();
  gotTime = timeset;
  CA_Status = false;
  virtual_zap_mode = false;
  chanready = 1;
  fileplay = 0;
  sigscale = new CScale(BAR_WIDTH, 8, RED_BAR, GREEN_BAR, YELLOW_BAR);
  snrscale = new CScale(BAR_WIDTH, 8, RED_BAR, GREEN_BAR, YELLOW_BAR);
  hddscale = new CScale(100, 4, 50, GREEN_BAR, 75, true);
  varscale = new CScale(100, 4, 50, GREEN_BAR, 75, true);
  timescale = new CScale(100, TIME_BAR_HEIGHT, 30, GREEN_BAR, 70, true);
}

void CInfoViewer::start ()
{
  ChanWidth = 122;
  ChanHeight = 70;

  lcdUpdateTimer = g_RCInput->addTimer (LCD_UPDATE_TIME_TV_MODE, false, true);
}

void CInfoViewer::paintTime (bool show_dot, bool firstPaint)
{
	if (!gotTime)
		return;

	int ChanNameY = BoxStartY + (ChanHeight >> 1) + 5;	//oberkante schatten?

	char timestr[10];
	struct timeb tm;

	ftime (&tm);
	strftime (timestr, sizeof(timestr), "%H:%M", localtime (&tm.time));

	if ((!firstPaint) && (strcmp (timestr, old_timestr) == 0)) {
		if (show_dot)
			frameBuffer->paintBoxRel (BoxEndX - time_width + time_left_width - LEFT_OFFSET, ChanNameY, time_dot_width, time_height / 2 + 2, COL_INFOBAR_PLUS_0);
		else
			g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_CHANNAME]->RenderString (BoxEndX - time_width + time_left_width - LEFT_OFFSET, ChanNameY + time_height, time_dot_width, ":", COL_INFOBAR);
	} else {
		if (!firstPaint)
			frameBuffer->paintBoxRel (BoxEndX - time_width - LEFT_OFFSET, ChanNameY, time_width + LEFT_OFFSET, time_height, COL_INFOBAR_PLUS_0, ROUND_RADIUS, 1);

		timestr[2] = 0;
		g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_CHANNAME]->RenderString (BoxEndX - time_width - LEFT_OFFSET, ChanNameY + time_height, time_left_width, timestr, COL_INFOBAR);
		g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_CHANNAME]->RenderString (BoxEndX - time_left_width - LEFT_OFFSET, ChanNameY + time_height, time_left_width + 10, timestr + 3, COL_INFOBAR);
		g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_CHANNAME]->RenderString (BoxEndX - time_width + time_left_width - LEFT_OFFSET, ChanNameY + time_height, time_dot_width + 10, ":", COL_INFOBAR);
		if (show_dot)
			frameBuffer->paintBoxRel (BoxEndX - time_left_width - time_dot_width - LEFT_OFFSET, ChanNameY, time_dot_width, time_height / 2 + 2, COL_INFOBAR_PLUS_0);
	}
	strcpy (old_timestr, timestr);
}

void CInfoViewer::showRecordIcon (const bool show)
{
  recordModeActive = CNeutrinoApp::getInstance ()->recordingstatus || shift_timer;
  if (recordModeActive) {
	int ChanNameX = BoxStartX + ChanWidth + 20;
	if (show) {
	  frameBuffer->paintIcon (autoshift ? "ats" : NEUTRINO_ICON_BUTTON_RED, ChanNameX, BoxStartY + 12);
	  if(!autoshift && !shift_timer) {
	  	int chanH = g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight ();
	  	frameBuffer->paintBoxRel (ChanNameX + 28 + SHADOW_OFFSET, BoxStartY + 12 + SHADOW_OFFSET, 300, 20, COL_INFOBAR_SHADOW_PLUS_0);
	  	frameBuffer->paintBoxRel (ChanNameX + 28, BoxStartY + 12, 300, 20, COL_INFOBAR_PLUS_0);
	  	g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->RenderString (ChanNameX + 30, BoxStartY + 12 + chanH, 300, ext_channel_name.c_str (), COL_INFOBAR, 0, true);
	  } else
	  	frameBuffer->paintBackgroundBoxRel (ChanNameX + 28, BoxStartY + 12, 300 + SHADOW_OFFSET, 20 + SHADOW_OFFSET);
	} else {
	  frameBuffer->paintBackgroundBoxRel (ChanNameX, BoxStartY + 10, 20, 20);
	}
  }
}

void CInfoViewer::showTitle (const int ChanNum, const std::string & Channel, const t_satellite_position satellitePosition, const t_channel_id new_channel_id, const bool calledFromNumZap, int epgpos)
{
	last_curr_id = last_next_id = 0;
	std::string ChannelName = Channel;
	bool show_dot = true;
	bool fadeOut = false;
	int fadeValue;
	bool new_chan = false;
//printf("CInfoViewer::showTitle ************************* chan num %d name %s\n", ChanNum, Channel.c_str());
	aspectRatio = videoDecoder->getAspectRatio();

	showButtonBar = !calledFromNumZap;
	bool fadeIn = g_settings.widget_fade && (!is_visible) && showButtonBar;

	is_visible = true;
	if (!calledFromNumZap && fadeIn)
		fadeTimer = g_RCInput->addTimer (FADE_TIME, false);

	fileplay = (ChanNum == 0);
	newfreq = true;
	sigscale->reset(); snrscale->reset(); timescale->reset(); hddscale->reset(); varscale->reset();

	InfoHeightY = NUMBER_H * 9 / 8 + 2 * g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_INFO]->getHeight () + 25;
	InfoHeightY_Info = 40;

	time_height = g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_CHANNAME]->getHeight () + 5;

	char t[2];
	t[1] = 0;
	time_left_width = 0;
	for (int i = 0; i < 10; i++) {
		*t = '0' + i;
		int l = g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_CHANNAME]->getRenderWidth(t);
		if (l > time_left_width)
			time_left_width = l;
	}
	time_left_width <<= 1;

	time_dot_width = g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_CHANNAME]->getRenderWidth (":");
	time_width = time_left_width * 2 + time_dot_width;

	BoxStartX = g_settings.screen_StartX + 10;
	BoxEndX = g_settings.screen_EndX - 10;
	BoxEndY = g_settings.screen_EndY - 10;

	int BoxEndInfoY = showButtonBar ? (BoxEndY - InfoHeightY_Info) : (BoxEndY);
	BoxStartY = BoxEndInfoY - InfoHeightY;

	if (!gotTime)
		gotTime = timeset;

	if (fadeIn) {
		fadeValue = 0x10;
		frameBuffer->setBlendLevel(fadeValue, fadeValue);
	} else
		fadeValue = g_settings.gtx_alpha1;

	// kill linke seite
	frameBuffer->paintBackgroundBox (BoxStartX, BoxStartY + ChanHeight, BoxStartX + (ChanWidth / 3), BoxStartY + ChanHeight + InfoHeightY_Info + 10);
	// kill progressbar
	frameBuffer->paintBackgroundBox (BoxEndX - 120, BoxStartY, BoxEndX, BoxStartY + ChanHeight);

	int col_NumBoxText;
	int col_NumBox;
	if (virtual_zap_mode) {
		col_NumBoxText = COL_MENUHEAD;
		col_NumBox = COL_MENUHEAD_PLUS_0;
		if ((channel_id != new_channel_id) || (evtlist.empty())) {
			evtlist.clear();
			//evtlist = g_Sectionsd->getEventsServiceKey(new_channel_id & 0xFFFFFFFFFFFFULL);
			sectionsd_getEventsServiceKey(new_channel_id & 0xFFFFFFFFFFFFULL, evtlist);
			if (!evtlist.empty())
				sort(evtlist.begin(),evtlist.end(), sortByDateTime);
			new_chan = true;
		}
	} else {
		col_NumBoxText = COL_INFOBAR;
		col_NumBox = COL_INFOBAR_PLUS_0;
	}
	if (! calledFromNumZap && !(g_RemoteControl->subChannels.empty()) && (g_RemoteControl->selected_subchannel > 0))
	{
		channel_id = g_RemoteControl->subChannels[g_RemoteControl->selected_subchannel].getChannelID();
		ChannelName = g_RemoteControl->subChannels[g_RemoteControl->selected_subchannel].subservice_name;
	} else {
		channel_id = new_channel_id;
	}

	int ChanNameX = BoxStartX + ChanWidth + SHADOW_OFFSET;
	int ChanNameY = BoxStartY + (ChanHeight / 2) + 5;	//oberkante schatten?
	ChanInfoX = BoxStartX + (ChanWidth / 3);

	asize = (BoxEndX - (2*ICON_LARGE_WIDTH + 2*ICON_SMALL_WIDTH + 4*2) - 102) - ChanInfoX;
	asize = asize - (NEUTRINO_ICON_BUTTON_RED_WIDTH+6)*4;
	asize = asize / 4;

	//Shadow
	frameBuffer->paintBox (BoxEndX-20, ChanNameY + SHADOW_OFFSET, BoxEndX + SHADOW_OFFSET, BoxEndY, COL_INFOBAR_SHADOW_PLUS_0, ROUND_RADIUS, 1);
	frameBuffer->paintBox (ChanInfoX + SHADOW_OFFSET, BoxEndY -20, BoxEndX + SHADOW_OFFSET, BoxEndY + SHADOW_OFFSET, COL_INFOBAR_SHADOW_PLUS_0, ROUND_RADIUS, 2); //round

	//infobox
	frameBuffer->paintBoxRel (ChanNameX-10, ChanNameY, BoxEndX-ChanNameX+10, BoxEndInfoY-ChanNameY, COL_INFOBAR_PLUS_0, ROUND_RADIUS, 1); // round

	//number box
	frameBuffer->paintBoxRel (BoxStartX + SHADOW_OFFSET, BoxStartY + SHADOW_OFFSET, ChanWidth, ChanHeight + 4, COL_INFOBAR_SHADOW_PLUS_0, ROUND_RADIUS, 3); // round
	frameBuffer->paintBoxRel (BoxStartX, BoxStartY, ChanWidth, ChanHeight + 4, COL_INFOBAR_PLUS_0, ROUND_RADIUS, 3); // round

	paintTime (show_dot, true);
	showRecordIcon (show_dot);
	show_dot = !show_dot;

	char strChanNum[10];
	sprintf (strChanNum, "%d", ChanNum);

	int ChanNumWidth = 0;
	logo_ok = false;
	if(showButtonBar && g_settings.infobar_picon) {
	    int P_W = 65;
	    int P_H = 39;

	    if (g_settings.infobar_picon > 1) {
		P_W = 100;
		P_H = 60;
	    }
#define PIC_W P_W
#define PIC_H P_H

#define PIC_X (ChanNameX + 10)
#define PIC_Y (ChanNameY + time_height - PIC_H)
		if (g_settings.infobar_picon == 3) {
		    logo_ok = g_PicViewer->DisplayLogo(channel_id, PIC_X-127, PIC_Y-16, PIC_W, PIC_H);
		} else {
		    logo_ok = g_PicViewer->DisplayLogo(channel_id, PIC_X, PIC_Y, PIC_W, PIC_H);
		}
		ChanNumWidth = PIC_W + 10;
	}
	if (ChanNum && (!logo_ok || g_settings.infobar_picon == 3)) {
		int ChanNumHeight = g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_NUMBER]->getHeight() + 5;
		ChanNumWidth = 5 + g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_NUMBER]->getRenderWidth (strChanNum);
		g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_NUMBER]->RenderString (ChanNameX + 5, ChanNameY + ChanNumHeight, ChanNumWidth, strChanNum, col_NumBoxText);
	}
	if (logo_ok && g_settings.infobar_picon < 3) {
		ChannelName = "  "+ChannelName;
		ChannelName = strChanNum+ChannelName;
	}
	g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_CHANNAME]->RenderString (ChanNameX + 10 + ChanNumWidth, ChanNameY + time_height, BoxEndX - (ChanNameX + 20) - time_width - LEFT_OFFSET - 5 - ChanNumWidth, ChannelName, COL_INFOBAR, 0, true);	// UTF-8

	int ChanNumYPos = BoxStartY + ChanHeight;
	if ((g_settings.infobar_picon < 3 || !logo_ok) && g_settings.infobar_sat_display && satellitePosition != 0 && satellitePositions.size()) {
		sat_iterator_t sit = satellitePositions.find(satellitePosition);

		if(sit != satellitePositions.end()) {
			int satNameWidth = g_SignalFont->getRenderWidth (sit->second.name);
			if (satNameWidth > (ChanWidth - 4))
				satNameWidth = ChanWidth - 4;

			int chanH = g_SignalFont->getHeight ();
			g_SignalFont->RenderString (3 + BoxStartX + ((ChanWidth - satNameWidth) / 2), BoxStartY + chanH, satNameWidth, sit->second.name, COL_INFOBAR);
		}
		ChanNumYPos += 5;
	}

	int ChanInfoY = BoxStartY + ChanHeight + 10;
	ButtonWidth = (BoxEndX - ChanInfoX - ICON_OFFSET) >> 2;

	frameBuffer->paintBox (ChanInfoX, ChanInfoY, ChanNameX, BoxEndInfoY, COL_INFOBAR_PLUS_0);

	if (showButtonBar) {
		sec_timer_id = g_RCInput->addTimer (1*1000*1000, false);
		if (BOTTOM_BAR_OFFSET > 0)
			frameBuffer->paintBackgroundBox (ChanInfoX, BoxEndInfoY, BoxEndX, BoxEndInfoY + BOTTOM_BAR_OFFSET);

		frameBuffer->paintBox (ChanInfoX, BoxEndInfoY-2, BoxEndX, BoxEndY-20, COL_BLACK);
		
		#ifdef DUCKBOX
		if(g_settings.scale_display_type == 0) {
			{ // FIXME
				int xcnt = (BoxEndX - ChanInfoX) / 4;
				int ycnt = (BoxEndY - BoxEndInfoY - 18) / 4;
				for(int i = 0; i < xcnt; i++) {
					for(int j = 0; j < ycnt; j++)
						frameBuffer->paintBoxRel (ChanInfoX + i*4, BoxEndInfoY + j*4, 2, 2, COL_INFOBAR_PLUS_1);
				}
			}
		} else {
			frameBuffer->paintBox (ChanInfoX, BoxEndInfoY-2, BoxEndX, BoxEndY-20, COL_INFOBAR_PLUS_0);
		}
		#else
		{ // FIXME
			int xcnt = (BoxEndX - ChanInfoX) / 4;
			int ycnt = (BoxEndY - BoxEndInfoY - 18) / 4;
			for(int i = 0; i < xcnt; i++) {
				for(int j = 0; j < ycnt; j++)
					frameBuffer->paintBoxRel (ChanInfoX + i*4, BoxEndInfoY + j*4, 2, 2, COL_INFOBAR_PLUS_1);
			}
		}
		#endif
		
		frameBuffer->paintBox (ChanInfoX, BoxEndY-20, BoxEndX, BoxEndY, COL_INFOBAR_BUTTONS_BACKGROUND, ROUND_RADIUS, 2); //round

		showSNR();
		showButtons(true);
		showIcon_CA_Status(0);
		showIcon_16_9 ();
		showIcon_DD();
		showIcon_VTXT ();
		showIcon_SubT();
	}

	if (fileplay) {
		show_Data ();
	} else {
		//info_CurrentNext = getEPG (channel_id);
		sectionsd_getCurrentNextServiceKey(channel_id & 0xFFFFFFFFFFFFULL, info_CurrentNext);
		if (!evtlist.empty()) {
			if (new_chan) {
				for ( eli=evtlist.begin(); eli!=evtlist.end(); ++eli ) {
					if ((uint)eli->startTime >= info_CurrentNext.current_zeit.startzeit + info_CurrentNext.current_zeit.dauer)
						break;
				}
				if (eli == evtlist.end()) // the end is not valid, so go back
					--eli;
			}

			if (epgpos != 0) {
				info_CurrentNext.flags = 0;
				if ((epgpos > 0) && (eli != evtlist.end())) {
					++eli; // next epg
					if (eli == evtlist.end()) // the end is not valid, so go back
						--eli;
				}
				else if ((epgpos < 0) && (eli != evtlist.begin())) {
					--eli; // prev epg
				}
				info_CurrentNext.flags = CSectionsdClient::epgflags::has_current;
				info_CurrentNext.current_uniqueKey      = eli->eventID;
				info_CurrentNext.current_zeit.startzeit = eli->startTime;
				info_CurrentNext.current_zeit.dauer     = eli->duration;
				if (eli->description.empty())
					info_CurrentNext.current_name   = g_Locale->getText(LOCALE_INFOVIEWER_NOEPG);
				else
					info_CurrentNext.current_name   = eli->description;
				info_CurrentNext.current_fsk            = '\0';

				if (eli != evtlist.end()) {
					++eli;
					if (eli != evtlist.end()) {
						info_CurrentNext.flags                  = CSectionsdClient::epgflags::has_current | CSectionsdClient::epgflags::has_next;
						info_CurrentNext.next_uniqueKey         = eli->eventID;
						info_CurrentNext.next_zeit.startzeit    = eli->startTime;
						info_CurrentNext.next_zeit.dauer        = eli->duration;
						if (eli->description.empty())
							info_CurrentNext.next_name      = g_Locale->getText(LOCALE_INFOVIEWER_NOEPG);
						else
							info_CurrentNext.next_name      = eli->description;
					}
					--eli;
				}
			}
		}

		if (!(info_CurrentNext.flags & (CSectionsdClient::epgflags::has_later | CSectionsdClient::epgflags::has_current | CSectionsdClient::epgflags::not_broadcast))) {
			// nicht gefunden / noch nicht geladen
			g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_INFO]->RenderString (ChanNameX + 10, ChanInfoY + 2 * g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_INFO]->getHeight () + 5, BoxEndX - (ChanNameX + 20), g_Locale->getText (gotTime ? (showButtonBar ? LOCALE_INFOVIEWER_EPGWAIT : LOCALE_INFOVIEWER_EPGNOTLOAD) : LOCALE_INFOVIEWER_WAITTIME), COL_INFOBAR, 0, true);	// UTF-8
		} else {
			show_Data ();
		}
	}
	showLcdPercentOver ();

	if ((g_RemoteControl->current_channel_id == channel_id) && !(((info_CurrentNext.flags & CSectionsdClient::epgflags::has_next) && (info_CurrentNext.flags & (CSectionsdClient::epgflags::has_current | CSectionsdClient::epgflags::has_no_current))) || (info_CurrentNext.flags & CSectionsdClient::epgflags::not_broadcast))) {
		g_Sectionsd->setServiceChanged (channel_id & 0xFFFFFFFFFFFFULL, true);
	}

	neutrino_msg_t msg;
	neutrino_msg_data_t data;

	CNeutrinoApp *neutrino = CNeutrinoApp::getInstance ();
	if (!calledFromNumZap) {

		bool hideIt = true;
		virtual_zap_mode = false;
		unsigned long long timeoutEnd = CRCInput::calcTimeoutEnd (g_settings.timing[SNeutrinoSettings::TIMING_INFOBAR] == 0 ? 0xFFFF : g_settings.timing[SNeutrinoSettings::TIMING_INFOBAR]);

		int res = messages_return::none;
		time_t ta, tb;

		while (!(res & (messages_return::cancel_info | messages_return::cancel_all))) {
			g_RCInput->getMsgAbsoluteTimeout (&msg, &data, &timeoutEnd);
#if 0
			if (!(info_CurrentNext.flags & (CSectionsdClient::epgflags::has_current))) {
				if (difftime (time (&tb), ta) > 1.1) {
					time (&ta);
					info_CurrentNext = getEPG (channel_id);
					if ((info_CurrentNext.flags & (CSectionsdClient::epgflags::has_current))) {
						show_Data ();
						showLcdPercentOver ();
					}
				}
			}
#endif
			if (msg == CRCInput::RC_sat || msg == CRCInput::RC_favorites) {
				g_RCInput->postMsg (msg, 0);
				res = messages_return::cancel_info;
			}
			else if (msg == CRCInput::RC_help || msg == CRCInput::RC_info) {
				g_RCInput->postMsg (NeutrinoMessages::SHOW_EPG, 0);
				res = messages_return::cancel_info;
			} else if ((msg == NeutrinoMessages::EVT_TIMER) && (data == fadeTimer)) {
				if (fadeOut) { // disappear
					fadeValue -= 0x10;
					if (fadeValue <= 0x10) {
						fadeValue = g_settings.gtx_alpha1;
						g_RCInput->killTimer (fadeTimer);
						res = messages_return::cancel_info;
					} else
						frameBuffer->setBlendLevel(fadeValue, fadeValue);
				} else { // appears
					fadeValue += 0x10;
					if (fadeValue >= g_settings.gtx_alpha1) {
						fadeValue = g_settings.gtx_alpha1;
						g_RCInput->killTimer (fadeTimer);
						fadeIn = false;
						frameBuffer->setBlendLevel(g_settings.gtx_alpha1, g_settings.gtx_alpha2);
					} else
						frameBuffer->setBlendLevel(fadeValue, fadeValue);
				}
			} else if ((msg == CRCInput::RC_ok) || (msg == CRCInput::RC_home) || (msg == CRCInput::RC_timeout)) {
				if (fadeIn) {
					g_RCInput->killTimer (fadeTimer);
					fadeIn = false;
				}
				if ((!fadeOut) && g_settings.widget_fade) {
					fadeOut = true;
					fadeTimer = g_RCInput->addTimer (FADE_TIME, false);
					timeoutEnd = CRCInput::calcTimeoutEnd (1);
				} else {
#if 0
					if ((msg != CRCInput::RC_timeout) && (msg != CRCInput::RC_ok))
						if (!fileplay && !timeshift)
							g_RCInput->postMsg (msg, data);
#endif
					res = messages_return::cancel_info;
				}
			} else if ((msg == NeutrinoMessages::EVT_TIMER) && (data == sec_timer_id)) {
				showSNR ();
				paintTime (show_dot, false);
				showRecordIcon (show_dot);
				show_dot = !show_dot;

				showIcon_16_9();
			} else if ( g_settings.virtual_zap_mode && ((msg == CRCInput::RC_right) || msg == CRCInput::RC_left )) {
				virtual_zap_mode = true;
				res = messages_return::cancel_all;
				hideIt = true;
			} else if (!fileplay && !timeshift) {
				if ((msg == (neutrino_msg_t) g_settings.key_quickzap_up) || (msg == (neutrino_msg_t) g_settings.key_quickzap_down) || (msg == CRCInput::RC_0) || (msg == NeutrinoMessages::SHOW_INFOBAR)) {
					hideIt = false;
					//hideIt = (g_settings.timing[SNeutrinoSettings::TIMING_INFOBAR] == 0) ? true : false;
					g_RCInput->postMsg (msg, data);
					res = messages_return::cancel_info;
				} else if (msg == NeutrinoMessages::EVT_TIMESET) {
					// Handle anyway!
					neutrino->handleMsg (msg, data);
					g_RCInput->postMsg (NeutrinoMessages::SHOW_INFOBAR, 0);
					hideIt = false;
					res = messages_return::cancel_all;
				} else {
					if (msg == CRCInput::RC_standby) {
						g_RCInput->killTimer (sec_timer_id);
						if (fadeIn || fadeOut)
							g_RCInput->killTimer (fadeTimer);
					}
					res = neutrino->handleMsg (msg, data);
					if (res & messages_return::unhandled) {
						// raus hier und im Hauptfenster behandeln...
						g_RCInput->postMsg (msg, data);
						res = messages_return::cancel_info;
					}
				}
			}
		}

		if (hideIt)
			killTitle ();

		g_RCInput->killTimer (sec_timer_id);
		sec_timer_id = 0;
		if (fadeIn || fadeOut) {
			g_RCInput->killTimer (fadeTimer);
			frameBuffer->setBlendLevel(g_settings.gtx_alpha1, g_settings.gtx_alpha2);
		}
		if (virtual_zap_mode)
			CNeutrinoApp::getInstance()->channelList->virtual_zap_mode(msg == CRCInput::RC_right);

	}
	aspectRatio = 0;
	fileplay = 0;
}

void CInfoViewer::showSubchan ()
{
  CFrameBuffer *frameBuffer = CFrameBuffer::getInstance ();
  CNeutrinoApp *neutrino = CNeutrinoApp::getInstance ();

  std::string subChannelName;	// holds the name of the subchannel/audio channel
  int subchannel = 0;				// holds the channel index

  if (!(g_RemoteControl->subChannels.empty ())) {
	// get info for nvod/subchannel
	subchannel = g_RemoteControl->selected_subchannel;
	if (g_RemoteControl->selected_subchannel >= 0)
	  subChannelName = g_RemoteControl->subChannels[g_RemoteControl->selected_subchannel].subservice_name;
  } else if (g_RemoteControl->current_PIDs.APIDs.size () > 1 && g_settings.audiochannel_up_down_enable) {
	// get info for audio channel
	subchannel = g_RemoteControl->current_PIDs.PIDs.selected_apid;
	subChannelName = g_RemoteControl->current_PIDs.APIDs[g_RemoteControl->current_PIDs.PIDs.selected_apid].desc;
  }

  if (!(subChannelName.empty ())) {
	char text[100];
	sprintf (text, "%d - %s", subchannel, subChannelName.c_str ());

	int dx = g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_INFO]->getRenderWidth (text) + 20;
	int dy = 25;

	if (g_RemoteControl->director_mode) {
	  int w = 20 + g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getRenderWidth (g_Locale->getText (LOCALE_NVODSELECTOR_DIRECTORMODE), true) + 20;	// UTF-8
	  if (w > dx)
		dx = w;
	  dy = dy * 2;
	} else
	  dy = dy + 5;

	int x = 0, y = 0;
	if (g_settings.infobar_subchan_disp_pos == 0) {
	  // Rechts-Oben
	  x = g_settings.screen_EndX - dx - 10;
	  y = g_settings.screen_StartY + 10;
	} else if (g_settings.infobar_subchan_disp_pos == 1) {
	  // Links-Oben
	  x = g_settings.screen_StartX + 10;
	  y = g_settings.screen_StartY + 10;
	} else if (g_settings.infobar_subchan_disp_pos == 2) {
	  // Links-Unten
	  x = g_settings.screen_StartX + 10;
	  y = g_settings.screen_EndY - dy - 10;
	} else if (g_settings.infobar_subchan_disp_pos == 3) {
	  // Rechts-Unten
	  x = g_settings.screen_EndX - dx - 10;
	  y = g_settings.screen_EndY - dy - 10;
	}

#ifdef __sh__
	fb_pixel_t *pixbuf = frameBuffer->allocPixelBuffer((dx + 2 * borderwidth), (dy + 2 * borderwidth));
	int checkSize = frameBuffer->SaveScreen (x - borderwidth, y - borderwidth, dx + 2 * borderwidth, dy + 2 * borderwidth, pixbuf);
#else
	fb_pixel_t pixbuf[(dx + 2 * borderwidth) * (dy + 2 * borderwidth)];
	frameBuffer->SaveScreen (x - borderwidth, y - borderwidth, dx + 2 * borderwidth, dy + 2 * borderwidth, pixbuf);
#endif

	// clear border
	frameBuffer->paintBackgroundBoxRel (x - borderwidth, y - borderwidth, dx + 2 * borderwidth, borderwidth);
	frameBuffer->paintBackgroundBoxRel (x - borderwidth, y + dy, dx + 2 * borderwidth, borderwidth);
	frameBuffer->paintBackgroundBoxRel (x - borderwidth, y, borderwidth, dy);
	frameBuffer->paintBackgroundBoxRel (x + dx, y, borderwidth, dy);

	frameBuffer->paintBoxRel (x, y, dx, dy, COL_MENUCONTENT_PLUS_0);
	g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_INFO]->RenderString (x + 10, y + 30, dx - 20, text, COL_MENUCONTENT);

	if (g_RemoteControl->director_mode)
		::paintButton_Footer(frameBuffer, NEUTRINO_ICON_BUTTON_YELLOW, g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL],
			g_Locale->getText (LOCALE_NVODSELECTOR_DIRECTORMODE), x + 8, y + dy - 2, dx - 10);

	unsigned long long timeoutEnd = CRCInput::calcTimeoutEnd (2);
	int res = messages_return::none;

	neutrino_msg_t msg;
	neutrino_msg_data_t data;

	while (!(res & (messages_return::cancel_info | messages_return::cancel_all))) {
	  g_RCInput->getMsgAbsoluteTimeout (&msg, &data, &timeoutEnd);

	  if (msg == CRCInput::RC_timeout) {
		res = messages_return::cancel_info;
	  } else {
		res = neutrino->handleMsg (msg, data);

		if (res & messages_return::unhandled) {
		  // raus hier und im Hauptfenster behandeln...
		  g_RCInput->postMsg (msg, data);
		  res = messages_return::cancel_info;
		}
	  }
	}
#ifdef __sh__
	frameBuffer->RestoreScreen (x - borderwidth, y - borderwidth, dx + 2 * borderwidth, dy + 2 * borderwidth, pixbuf, checkSize);
	delete pixbuf;
#else
	frameBuffer->RestoreScreen (x - borderwidth, y - borderwidth, dx + 2 * borderwidth, dy + 2 * borderwidth, pixbuf);
#endif
  } else {
	g_RCInput->postMsg (NeutrinoMessages::SHOW_INFOBAR, 0);
  }
}

void CInfoViewer::showIcon_16_9 () {
	aspectRatio = videoDecoder->getAspectRatio();
	frameBuffer->paintIcon ((aspectRatio) ? "16_9" : "16_9_gray", BoxEndX - (2*ICON_LARGE_WIDTH + 2*ICON_SMALL_WIDTH + 4*2), BoxEndY - ICON_Y_1);
}

extern "C" void tuxtxt_start(int tpid);
extern "C" int  tuxtxt_stop();

void CInfoViewer::showIcon_VTXT () const
{
  frameBuffer->paintIcon ((g_RemoteControl->current_PIDs.PIDs.vtxtpid != 0) ? "vtxt" : "vtxt_gray", BoxEndX - (2*ICON_SMALL_WIDTH + 2*2), BoxEndY - ICON_Y_1);
}

void CInfoViewer::showIcon_SubT() const
{
        bool have_sub = false;
	CZapitChannel * cc = CNeutrinoApp::getInstance()->channelList->getChannel(CNeutrinoApp::getInstance()->channelList->getActiveChannelNumber());
	if(cc && cc->getSubtitleCount())
		have_sub = true;

        frameBuffer->paintIcon(have_sub ? "subt" : "subt_gray", BoxEndX - (ICON_SMALL_WIDTH + 2), BoxEndY - ICON_Y_1);
}

void CInfoViewer::showFailure ()
{
  ShowHintUTF (LOCALE_MESSAGEBOX_ERROR, g_Locale->getText (LOCALE_INFOVIEWER_NOTAVAILABLE), 430);	// UTF-8
}

void CInfoViewer::showMotorMoving (int duration)
{
  char text[256];
  char buffer[10];

  sprintf (buffer, "%d", duration);
  strcpy (text, g_Locale->getText (LOCALE_INFOVIEWER_MOTOR_MOVING));
  strcat (text, " (");
  strcat (text, buffer);
  strcat (text, " s)");

  ShowHintUTF (LOCALE_MESSAGEBOX_INFO, text, g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getRenderWidth (text, true) + 10, duration);	// UTF-8
}

int CInfoViewer::handleMsg (const neutrino_msg_t msg, neutrino_msg_data_t data)
{

 if ((msg == NeutrinoMessages::EVT_CURRENTNEXT_EPG) || (msg == NeutrinoMessages::EVT_NEXTPROGRAM)) {
//printf("CInfoViewer::handleMsg: NeutrinoMessages::EVT_CURRENTNEXT_EPG data %llx current %llx\n", *(t_channel_id *) data, channel_id & 0xFFFFFFFFFFFFULL);
	if ((*(t_channel_id *) data) == (channel_id & 0xFFFFFFFFFFFFULL)) {
	  getEPG (*(t_channel_id *) data, info_CurrentNext);
	  if (is_visible)
		show_Data (true);
	  showLcdPercentOver ();
	}
	return messages_return::handled;
  } else if (msg == NeutrinoMessages::EVT_TIMER) {
	if (data == fadeTimer) {
	  // hierher kann das event nur dann kommen, wenn ein anderes Fenster im Vordergrund ist!
	  g_RCInput->killTimer (fadeTimer);
	  frameBuffer->setBlendLevel(g_settings.gtx_alpha1, g_settings.gtx_alpha2);

	  return messages_return::handled;
	} else if (data == lcdUpdateTimer) {
//printf("CInfoViewer::handleMsg: lcdUpdateTimer\n");
		if ( is_visible )
			show_Data( true );
	  showLcdPercentOver ();
	  return messages_return::handled;
	} else if (data == sec_timer_id) {
		showSNR ();
	  return messages_return::handled;
	}
  } else if (msg == NeutrinoMessages::EVT_RECORDMODE) {
	recordModeActive = data;
	if(is_visible) showRecordIcon(true);
  } else if (msg == NeutrinoMessages::EVT_ZAP_GOTAPIDS) {
	if ((*(t_channel_id *) data) == channel_id) {
	  if (is_visible && showButtonBar) {
		showIcon_DD ();
		showButtons ();
	  }
	}
	return messages_return::handled;
  } else if (msg == NeutrinoMessages::EVT_ZAP_GOTPIDS) {
	if ((*(t_channel_id *) data) == channel_id) {
	  if (is_visible && showButtonBar) {
		showIcon_VTXT ();
		showIcon_SubT();
		showIcon_CA_Status (0);
		// we need to give EMU a second to get proper decoding status when zapping
		usleep(1220000);
		showIcon_CA_Status (0); 	  }
	}
	return messages_return::handled;
  } else if (msg == NeutrinoMessages::EVT_ZAP_GOT_SUBSERVICES) {
	if ((*(t_channel_id *) data) == channel_id) {
	  if (is_visible && showButtonBar)
		showButtons ();
	}
	return messages_return::handled;
  } else if (msg == NeutrinoMessages::EVT_ZAP_SUB_COMPLETE) {
	chanready = 1;
	showSNR ();
	//if ((*(t_channel_id *)data) == channel_id)
	{
	  if (is_visible && showButtonBar && (!g_RemoteControl->are_subchannels))
		show_Data (true);
	}
	showLcdPercentOver ();
	return messages_return::handled;
  } else if (msg == NeutrinoMessages::EVT_ZAP_SUB_FAILED) {
	chanready = 1;
	showSNR ();
	// show failure..!
	CVFD::getInstance ()->showServicename ("(" + g_RemoteControl->getCurrentChannelName () + ')');
	printf ("zap failed!\n");
	showFailure ();
	CVFD::getInstance ()->showPercentOver (255);
	return messages_return::handled;
  } else if (msg == NeutrinoMessages::EVT_ZAP_FAILED) {
	chanready = 1;
	showSNR ();
	if ((*(t_channel_id *) data) == channel_id) {
	  // show failure..!
	  CVFD::getInstance ()->showServicename ("(" + g_RemoteControl->getCurrentChannelName () + ')');
	  printf ("zap failed!\n");
	  showFailure ();
	  CVFD::getInstance ()->showPercentOver (255);
	}
	return messages_return::handled;
  } else if (msg == NeutrinoMessages::EVT_ZAP_MOTOR) {
	chanready = 0;
	showMotorMoving (data);
	return messages_return::handled;
  } else if (msg == NeutrinoMessages::EVT_MODECHANGED) {
	aspectRatio = data;
	if (is_visible && showButtonBar)
	  showIcon_16_9 ();

	return messages_return::handled;
  } else if (msg == NeutrinoMessages::EVT_TIMESET) {
	gotTime = true;
	return messages_return::handled;
  } else if (msg == NeutrinoMessages::EVT_ZAP_CA_CLEAR) {
	Set_CA_Status (false);
	return messages_return::handled;
  } else if (msg == NeutrinoMessages::EVT_ZAP_CA_LOCK) {
	Set_CA_Status (true);
	return messages_return::handled;
  } else if (msg == NeutrinoMessages::EVT_ZAP_CA_FTA) {
	Set_CA_Status (false);
	return messages_return::handled;
  } else if (msg == NeutrinoMessages::EVT_ZAP_CA_ID) {
	chanready = 1;
	Set_CA_Status (data);
	showSNR ();
	return messages_return::handled;
  }

  return messages_return::unhandled;
}

void CInfoViewer::getEPG (const t_channel_id for_channel_id, CSectionsdClient::CurrentNextInfo &info)
{
	static CSectionsdClient::CurrentNextInfo oldinfo;

	/* to clear the oldinfo for channels without epg, call getEPG() with for_channel_id = 0 */
	if (for_channel_id == 0)
	{
		oldinfo.current_uniqueKey = 0;
		return;
	}

	sectionsd_getCurrentNextServiceKey(for_channel_id & 0xFFFFFFFFFFFFULL, info);

	/* of there is no EPG, send an event so that parental lock can work */
	if (info.current_uniqueKey == 0 && info.next_uniqueKey == 0) {
		memcpy(&oldinfo, &info, sizeof(CSectionsdClient::CurrentNextInfo));
		char *p = new char[sizeof(t_channel_id)];
		memcpy(p, &for_channel_id, sizeof(t_channel_id));
		g_RCInput->postMsg (NeutrinoMessages::EVT_NOEPG_YET, (const neutrino_msg_data_t) p, false);
		return;
	}

	if (info.current_uniqueKey != oldinfo.current_uniqueKey || info.next_uniqueKey != oldinfo.next_uniqueKey)
	{
		char *p = new char[sizeof(t_channel_id)];
		memcpy(p, &for_channel_id, sizeof(t_channel_id));
		neutrino_msg_t msg;
		if (info.flags & (CSectionsdClient::epgflags::has_current | CSectionsdClient::epgflags::has_next))
		{
			if (info.flags & CSectionsdClient::epgflags::has_current)
				msg = NeutrinoMessages::EVT_CURRENTEPG;
			else
				msg = NeutrinoMessages::EVT_NEXTEPG;
		}
		else
			msg = NeutrinoMessages::EVT_NOEPG_YET;
		g_RCInput->postMsg(msg, (const neutrino_msg_data_t)p, false); // data is pointer to allocated memory
		memcpy(&oldinfo, &info, sizeof(CSectionsdClient::CurrentNextInfo));
	}
}

#define get_set CNeutrinoApp::getInstance()->getScanSettings()

void CInfoViewer::showSNR () {
    if ((g_settings.infobar_picon && g_settings.infobar_picon < 3) || (!logo_ok && g_settings.infobar_picon == 3)) {

  char percent[10];
  uint16_t ssig, ssnr;
  int sw, snr, sig, posx, posy;
  int height, ChanNumYPos;
  int barwidth = BAR_WIDTH;

  if (is_visible && !fileplay) {
	if (newfreq && chanready) {
	  char freq[20];

	  newfreq = false;
	  CZapitClient::CCurrentServiceInfo si = g_Zapit->getCurrentServiceInfo ();
	  sprintf (freq, "%d.%d MHz %c", si.tsfrequency / 1000, si.tsfrequency % 1000, si.polarisation ? 'V' : 'H');
#if 0
//FIXME this sets default params for scan menu
	  sprintf (get_set.TP_freq, "%d", si.tsfrequency);
	  sprintf (get_set.TP_rate, "%d", si.rate);
	  get_set.TP_pol = si.polarisation;
	  get_set.TP_fec = si.fec;
#endif

	  int chanH = g_SignalFont->getHeight ();
	  int satNameWidth = g_SignalFont->getRenderWidth (freq);
	  g_SignalFont->RenderString (3 + BoxStartX + ((ChanWidth - satNameWidth) / 2), BoxStartY + 2 * chanH - 3, satNameWidth, freq, COL_INFOBAR);
	}
	ssig = frontend->getSignalStrength();
	ssnr = frontend->getSignalNoiseRatio();

	sig = (ssig & 0xFFFF) * 100 / 65535;
	snr = (ssnr & 0xFFFF) * 100 / 65535;
	height = g_SignalFont->getHeight () - 1;
	ChanNumYPos = BoxStartY + ChanHeight + 4 - 2 * height;

	if (sigscale->getPercent() != sig) {
	  posx = BoxStartX + 4;
	  posy = ChanNumYPos + 3;
	  sigscale->paint(posx, posy+4, sig);

	  sprintf (percent, "%d%%S", sig);
	  posx = posx + barwidth + 2;
	  sw = BoxStartX + ChanWidth - posx;
	  frameBuffer->paintBoxRel (posx, posy, sw, height, COL_INFOBAR_PLUS_0);
	  g_SignalFont->RenderString (posx, posy + height, sw, percent, COL_INFOBAR);
	}
	if (snrscale->getPercent() != snr) {
	  posx = BoxStartX + 4;
	  posy = ChanNumYPos + 3 + height - 2;

	  snrscale->paint(posx, posy+4, snr);

	  sprintf (percent, "%d%%Q", snr);
	  posx = posx + barwidth + 2;
	  sw = BoxStartX + ChanWidth - posx -4;
	  frameBuffer->paintBoxRel (posx, posy, sw, height-2, COL_INFOBAR_PLUS_0);
	  g_SignalFont->RenderString (posx, posy + height, sw, percent, COL_INFOBAR);
	}
    }
  }
  if(is_visible) {
	struct statfs s;
	int per = 0;
	if (::statfs("/var", &s) == 0) {
		per = (s.f_blocks - s.f_bfree) / (s.f_blocks/100);
	}
	varscale->paint(BoxEndX - (2*ICON_LARGE_WIDTH + 2*ICON_SMALL_WIDTH + 4*2) - 102, BoxEndY - ICON_Y_1, per);
	per = 0;
	//HD info
	if (::statfs(g_settings.network_nfs_recordingdir, &s) == 0) {
		switch(s.f_type)
		{
			case (int) 0xEF53:      /*EXT2 & EXT3*/
			case (int) 0x6969:      /*NFS*/
			case (int) 0xFF534D42:  /*CIFS*/
			case (int) 0x517B:      /*SMB*/
			case (int) 0x52654973:  /*REISERFS*/
			case (int) 0x65735546:  /*fuse for ntfs*/
			case (int) 0x58465342:  /*xfs*/
			case (int) 0x4d44:      /*msdos*/
				per = (s.f_blocks - s.f_bfree) / (s.f_blocks/100);
			break;
			default:
				fprintf( stderr,"%s Unknow File system type: %i\n",g_settings.network_nfs_recordingdir ,s.f_type);
			break;
		}
	}

	hddscale->paint(BoxEndX - (2*ICON_LARGE_WIDTH + 2*ICON_SMALL_WIDTH + 4*2) - 102, BoxEndY - ICON_Y_1 + 10, per);
  }
}

void CInfoViewer::show_Data (bool calledFromEvent)
{
  char runningStart[10];
  char runningRest[20];
  char runningPercent = 0;
  static char oldrunningPercent = 255;

  char nextStart[10];
  char nextDuration[10];

  int is_nvod = false;
  int b114 = BoxEndX - 114 + 7;
  int b112 = BoxEndX - 112 + 7;

  if (fileplay) {
	int posy = BoxStartY + 12;
	runningPercent = file_prozent;
	if(runningPercent > 100)
		runningPercent = 100;

	if(!calledFromEvent || (oldrunningPercent != runningPercent)) {
		frameBuffer->paintBoxRel (b114+4, posy, 102, 18, COL_INFOBAR_SHADOW_PLUS_0);
		frameBuffer->paintBoxRel (b112+4, posy + 2, 98, 14, COL_INFOBAR_PLUS_0);
		oldrunningPercent = file_prozent;
	}
	if (runningPercent > -1)
		timescale->paint(b112+4, posy + 2, runningPercent);

	int xStart = BoxStartX + ChanWidth;
	int ChanInfoY = BoxStartY + ChanHeight + 15;	//+10
	int height = g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_INFO]->getHeight ();
	int duration1TextPos = BoxEndX - 30;

	g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_INFO]->RenderString (xStart, ChanInfoY + height, duration1TextPos - xStart - 5, g_file_epg, COL_INFOBAR, 0, true);
	ChanInfoY += height;
	g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_INFO]->RenderString (xStart, ChanInfoY + height, duration1TextPos - xStart - 5, g_file_epg1, COL_INFOBAR, 0, true);

	return;
  }
  if (is_visible) {

	if ((g_RemoteControl->current_channel_id == channel_id) && (g_RemoteControl->subChannels.size () > 0) && (!g_RemoteControl->are_subchannels)) {
	  is_nvod = true;
	  info_CurrentNext.current_zeit.startzeit = g_RemoteControl->subChannels[g_RemoteControl->selected_subchannel].startzeit;
	  info_CurrentNext.current_zeit.dauer = g_RemoteControl->subChannels[g_RemoteControl->selected_subchannel].dauer;
	} else {
	  if ((info_CurrentNext.flags & CSectionsdClient::epgflags::has_current) && (info_CurrentNext.flags & CSectionsdClient::epgflags::has_next) && (showButtonBar)) {
		if ((uint) info_CurrentNext.next_zeit.startzeit < (info_CurrentNext.current_zeit.startzeit + info_CurrentNext.current_zeit.dauer)) {
		  is_nvod = true;
		}
	  }
	}

	time_t jetzt = time (NULL);

	if (info_CurrentNext.flags & CSectionsdClient::epgflags::has_current) {
	  int seit = (jetzt - info_CurrentNext.current_zeit.startzeit + 30) / 60;
	  int rest = (info_CurrentNext.current_zeit.dauer / 60) - seit;
	  if (seit < 0) {
		runningPercent = 0;
		sprintf (runningRest, "in %d min", -seit);
	  } else {
		runningPercent = (unsigned) ((float) (jetzt - info_CurrentNext.current_zeit.startzeit) / (float) info_CurrentNext.current_zeit.dauer * 100.);
		if(runningPercent > 100)
			runningPercent = 100;
		sprintf (runningRest, "%d / %d min", seit, rest);
	  }

	  struct tm *pStartZeit = localtime (&info_CurrentNext.current_zeit.startzeit);
	  sprintf (runningStart, "%02d:%02d", pStartZeit->tm_hour, pStartZeit->tm_min);
	} else
		last_curr_id = 0;

	if (info_CurrentNext.flags & CSectionsdClient::epgflags::has_next) {
	  unsigned dauer = info_CurrentNext.next_zeit.dauer / 60;
	  sprintf (nextDuration, "%d min", dauer);
	  struct tm *pStartZeit = localtime (&info_CurrentNext.next_zeit.startzeit);
	  sprintf (nextStart, "%02d:%02d", pStartZeit->tm_hour, pStartZeit->tm_min);
	} else
		last_next_id = 0;

	int height = g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_CHANNAME]->getHeight () / 3;
	int ChanInfoY = BoxStartY + ChanHeight + 15;	//+10

	if (showButtonBar) {
	  int posy = BoxStartY + 16;
	  int height2 = 20;
	  //percent
	  if (info_CurrentNext.flags & CSectionsdClient::epgflags::has_current) {
//printf("CInfoViewer::show_Data: ************************************************* runningPercent %d\n", runningPercent);
		if(!calledFromEvent || (oldrunningPercent != runningPercent)) {
			frameBuffer->paintBoxRel (BoxEndX - 104, posy + 6, 108, 14, COL_INFOBAR_SHADOW_PLUS_0, 1, 3);
			frameBuffer->paintBoxRel (BoxEndX - 108, posy + 2, 108, 14, COL_INFOBAR_PLUS_0, 1, 3);
			oldrunningPercent = runningPercent;
		}
		if (runningPercent > -1)
			timescale->paint(BoxEndX - 102, posy + 2, runningPercent);
	  } else {
		oldrunningPercent = 255;
		frameBuffer->paintBackgroundBoxRel (BoxEndX - 108, posy, 112, height2);
	  }
	  showButtons();
	}

	height = g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_INFO]->getHeight ();
	int xStart = BoxStartX + ChanWidth;

	//frameBuffer->paintBox (ChanInfoX + 10, ChanInfoY, BoxEndX, ChanInfoY + height, COL_INFOBAR_PLUS_0);

	if ((info_CurrentNext.flags & CSectionsdClient::epgflags::not_broadcast) || ((calledFromEvent) && !(info_CurrentNext.flags & (CSectionsdClient::epgflags::has_next | CSectionsdClient::epgflags::has_current)))) {
	  // no EPG available
	  ChanInfoY += height;
	  frameBuffer->paintBox (ChanInfoX + 10, ChanInfoY, BoxEndX, ChanInfoY + height, COL_INFOBAR_PLUS_0);
	  g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_INFO]->RenderString (BoxStartX + ChanWidth + 20, ChanInfoY + height, BoxEndX - (BoxStartX + ChanWidth + 20), g_Locale->getText (gotTime ? LOCALE_INFOVIEWER_NOEPG : LOCALE_INFOVIEWER_WAITTIME), COL_INFOBAR, 0, true);	// UTF-8
	} else {
	  // irgendein EPG gefunden
	  int duration1Width = g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_INFO]->getRenderWidth (runningRest);
	  int duration1TextPos = BoxEndX - duration1Width - LEFT_OFFSET;

	  int duration2Width = g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_INFO]->getRenderWidth (nextDuration);
	  int duration2TextPos = BoxEndX - duration2Width - LEFT_OFFSET;

	  if ((info_CurrentNext.flags & CSectionsdClient::epgflags::has_next) && (!(info_CurrentNext.flags & CSectionsdClient::epgflags::has_current))) {
		// there are later events available - yet no current
		frameBuffer->paintBox (ChanInfoX + 10, ChanInfoY, BoxEndX, ChanInfoY + height, COL_INFOBAR_PLUS_0);
		g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_INFO]->RenderString (xStart, ChanInfoY + height, BoxEndX - xStart, g_Locale->getText (LOCALE_INFOVIEWER_NOCURRENT), COL_INFOBAR, 0, true);	// UTF-8

		ChanInfoY += height;

		//info next
		//frameBuffer->paintBox (ChanInfoX + 10, ChanInfoY, BoxEndX, ChanInfoY + height, COL_INFOBAR_PLUS_0);

		if(last_next_id != info_CurrentNext.next_uniqueKey) {
			frameBuffer->paintBox (ChanInfoX + 10, ChanInfoY, BoxEndX, ChanInfoY + height, COL_INFOBAR_PLUS_0);
			g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_INFO]->RenderString (ChanInfoX + 10, ChanInfoY + height, 100, nextStart, COL_INFOBAR);
			g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_INFO]->RenderString (xStart, ChanInfoY + height, duration2TextPos - xStart - 5, info_CurrentNext.next_name, COL_INFOBAR, 0, true);
			g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_INFO]->RenderString (duration2TextPos, ChanInfoY + height, duration2Width, nextDuration, COL_INFOBAR);
			last_next_id = info_CurrentNext.next_uniqueKey;
		}
	  } else {
		  if(last_curr_id != info_CurrentNext.current_uniqueKey) {
			  frameBuffer->paintBox (ChanInfoX + 10, ChanInfoY, BoxEndX, ChanInfoY + height, COL_INFOBAR_PLUS_0);
			  g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_INFO]->RenderString (ChanInfoX + 10, ChanInfoY + height, 100, runningStart, COL_INFOBAR);
			  g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_INFO]->RenderString (xStart, ChanInfoY + height, duration1TextPos - xStart - 5, info_CurrentNext.current_name, COL_INFOBAR, 0, true);

			  last_curr_id = info_CurrentNext.current_uniqueKey;
		  }
		  frameBuffer->paintBox (BoxEndX - 80, ChanInfoY, BoxEndX, ChanInfoY + height, COL_INFOBAR_PLUS_0);//FIXME duration1TextPos not really good
		  g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_INFO]->RenderString (duration1TextPos, ChanInfoY + height, duration1Width, runningRest, COL_INFOBAR);

		ChanInfoY += height;

		//info next
		//frameBuffer->paintBox (ChanInfoX + 10, ChanInfoY, BoxEndX, ChanInfoY + height, COL_INFOBAR_PLUS_0);

		if ((!is_nvod) && (info_CurrentNext.flags & CSectionsdClient::epgflags::has_next)) {
			if(last_next_id != info_CurrentNext.next_uniqueKey) {
				frameBuffer->paintBox (ChanInfoX + 10, ChanInfoY, BoxEndX, ChanInfoY + height, COL_INFOBAR_PLUS_0);
				g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_INFO]->RenderString (ChanInfoX + 10, ChanInfoY + height, 100, nextStart, COL_INFOBAR);
				g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_INFO]->RenderString (xStart, ChanInfoY + height, duration2TextPos - xStart - 5, info_CurrentNext.next_name, COL_INFOBAR, 0, true);
				g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_INFO]->RenderString (duration2TextPos, ChanInfoY + height, duration2Width, nextDuration, COL_INFOBAR);
				last_next_id = info_CurrentNext.next_uniqueKey;
			}
		} //else
			//frameBuffer->paintBox (ChanInfoX + 10, ChanInfoY, BoxEndX, ChanInfoY + height, COL_INFOBAR_PLUS_0);//why this...
	  }
	}
  }
}

void CInfoViewer::showButtons (bool start)
{
	const char *icon[4] = { NEUTRINO_ICON_BUTTON_RED, NEUTRINO_ICON_BUTTON_GREEN, NEUTRINO_ICON_BUTTON_YELLOW, NEUTRINO_ICON_BUTTON_BLUE };
	int iconwidth[4] = { NEUTRINO_ICON_BUTTON_RED_WIDTH, NEUTRINO_ICON_BUTTON_GREEN_WIDTH, NEUTRINO_ICON_BUTTON_YELLOW_WIDTH, NEUTRINO_ICON_BUTTON_BLUE_WIDTH };

	if (start)
		for (int i = 0; i < 4; i++)
			buttonName[i] = "";

	int x = ChanInfoX - asize;

	int fontheight = g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight();
	for (int i = 0; i < 4; i++) {
		x += asize + 2;
		if (start)
			frameBuffer->paintIcon(icon[i], x, BoxEndY - fontheight + fontheight/8, 0, (6 * fontheight)/8);
		const char *bn =  CNeutrinoApp::getInstance()->getUserMenuButtonName(i);
		x += 2 + iconwidth[i] + 2;
		if (strcmp(bn, buttonName[i].c_str())) {
			frameBuffer->paintBox (x, BoxEndY - fontheight , x + asize, BoxEndY, COL_INFOBAR_BUTTONS_BACKGROUND);
			buttonName[i] = std::string(bn);
			g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->RenderString(x, BoxEndY, asize, bn, COL_INFOBAR_BUTTONS, 0, true);
		}
	}
}

void CInfoViewer::showIcon_DD ()
{
	const char *dd_icon;
	if ((g_RemoteControl->current_PIDs.PIDs.selected_apid < g_RemoteControl->current_PIDs.APIDs.size())
		&& (g_RemoteControl->current_PIDs.APIDs[g_RemoteControl->current_PIDs.PIDs.selected_apid].is_ac3))
		dd_icon = "dd";
	else if (g_RemoteControl->has_ac3)
		dd_icon = "dd_avail";
	else
		dd_icon = "dd_gray";
	frameBuffer->paintIcon (dd_icon, BoxEndX - (ICON_LARGE_WIDTH + 2*ICON_SMALL_WIDTH + 3*2), BoxEndY - ICON_Y_1);
}


void CInfoViewer::killTitle ()
{
  if (is_visible) {
	is_visible = false;
#if 0
	frameBuffer->paintBackgroundBox (BoxStartX, BoxStartY, BoxEndX + SHADOW_OFFSET, BoxEndY + SHADOW_OFFSET);
#else
	// clear complete width to cope with leftovers from switching from/to 3D
	frameBuffer->paintBackgroundBoxRel (0, BoxStartY, frameBuffer->getScreenWidth(true), frameBuffer->getScreenHeight(true) - BoxStartY);
#endif
  }
}

void CInfoViewer::Set_CA_Status (int Status)
{
  CA_Status = Status;
  m_CA_Status = Status;
  if (is_visible && showButtonBar)
	showIcon_CA_Status (1);
}

void CInfoViewer::showLcdPercentOver ()
{
  if (g_settings.lcd_setting[SNeutrinoSettings::LCD_SHOW_VOLUME] != 1) {
	if (fileplay || (NeutrinoMessages::mode_ts == CNeutrinoApp::getInstance()->getMode())) {
	  CVFD::getInstance ()->showPercentOver (file_prozent);
	  return;
	}
	int runningPercent = -1;
	time_t jetzt = time (NULL);
#if 0
	if (!(info_CurrentNext.flags & CSectionsdClient::epgflags::has_current) || jetzt > (int) (info_CurrentNext.current_zeit.startzeit + info_CurrentNext.current_zeit.dauer)) {
	  info_CurrentNext = getEPG (channel_id);
	}
#endif
	if (info_CurrentNext.flags & CSectionsdClient::epgflags::has_current) {
	  if (jetzt < info_CurrentNext.current_zeit.startzeit)
		runningPercent = 0;
	  else
		runningPercent = MIN ((unsigned) ((float) (jetzt - info_CurrentNext.current_zeit.startzeit) / (float) info_CurrentNext.current_zeit.dauer * 100.), 100);
	}
	CVFD::getInstance ()->showPercentOver (runningPercent);
  }
}

int CInfoViewerHandler::exec (CMenuTarget * parent, const std::string & actionkey)
{
  int res = menu_return::RETURN_EXIT_ALL;
  CChannelList *channelList;
  CInfoViewer *i;

  if (parent) {
	parent->hide ();
  }

  i = new CInfoViewer;

  channelList = CNeutrinoApp::getInstance ()->channelList;
  i->start ();
  i->showTitle (channelList->getActiveChannelNumber (), channelList->getActiveChannelName (), channelList->getActiveSatellitePosition (), channelList->getActiveChannel_ChannelID ());	// UTF-8
  delete i;

  return res;
}

extern int pmt_caids[10];

#define COL_white 0xffffffff 	// white
#define COL_green 0xff00ff00	// green
#define COL_yellow 0xffffff00	// yellow

void CInfoViewer::showIcon_CA_Status (int notfirst)
{
	FILE *f;
	char input[256];
	char *buf;
	int py = BoxEndY - InfoHeightY_Info;
	int acaid = 0;
	int caids[] = { 0x0600, 0x0100, 0x0500, 0x1800, 0x0B00, 0x0D00, 0x0900, 0x2600, 0x4a00, 0x0E00 };
	const char *catxt[] = { "I", "S", "V", "N", "C", "CW", "NDS", "B", "D", "P" };
	if(!notfirst) {
		f = fopen("/tmp/ecm.info", "rt");
		if (f != NULL) {
			char *buf = (char*) malloc(50);
			if (buf) {
				int i = 0;
				if (fgets(buf, 50, f) != NULL) {
					while (buf[i] != '0')
						i++;
					sscanf(&buf[i], "%X", &acaid);
				}
				free(buf);
			}
			fclose(f);
		}

		int endx = BoxEndX - LEFT_OFFSET;
		int py = BoxEndY - InfoHeightY_Info;
		int px = 0;
		int space = g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_INFO]->getRenderWidth(" ");

		acaid &= 0xff00;
		if (acaid == 0x1700)
			acaid = 0x1800;

		for(int i = (int)(sizeof(caids)/sizeof(int)) - 1; i > -1 ; i--) {
			uint32_t col = COL_white;
			if (caids[i] == acaid)
				col = COL_green;
			else if (pmt_caids[i])
				col = COL_yellow;

			int w = g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_INFO]->getRenderWidth(catxt[i]);
			endx -= w;
			g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_INFO]->RenderString (endx, py + 15, w, catxt[i], 0, 0, true, col);
			endx -= space;
		}
	}
}
