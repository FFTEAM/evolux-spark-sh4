/*
	Neutrino-GUI  -   DBoxII-Project


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
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <unistd.h>

#include <gui/streaminfo2.h>

#include <global.h>
#include <neutrino.h>

#include <driver/fontrenderer.h>
#include <driver/rcinput.h>
#include <driver/screen_max.h>
#include <gui/color.h>
#include <gui/widget/icons.h>
#include <gui/customcolor.h>
#include <daemonc/remotecontrol.h>
#include <zapit/frontend_c.h>
#include <video_cs.h>
#include <audio_cs.h>
#include <dmx_cs.h>
#include <zapit/satconfig.h>
#include <pthread.h>
#include <string>

#include <linux/dvb/dmx.h>

extern CFrontend * frontend;
extern cVideo * videoDecoder;
extern cAudio * audioDecoder;

extern CRemoteControl *g_RemoteControl;	/* neutrino.cpp */
extern CZapitClient::SatelliteList satList;

static void* bandwidth_thread(void *arg);
static long long rate = -1;
static bool bw_thread_running = false;
static pthread_t bw_thread = 0;

CStreamInfo2::CStreamInfo2 ()
{
	frameBuffer = CFrameBuffer::getInstance ();

	font_head = SNeutrinoSettings::FONT_TYPE_MENU_TITLE;
	font_info = SNeutrinoSettings::FONT_TYPE_MENU;
	font_small = SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL;

	fgcolor = frameBuffer->realcolor[(((((int)COL_MENUCONTENTDARK) + 2) | 7) - 2)];
	fgcolor_head = frameBuffer->realcolor[(((((int)COL_MENUHEAD) + 2) | 7) - 2)];
	fgcolor |= 0xFF000000;
	fgcolor_head |= 0xFF000000;
	bgcolor = COL_INFOBAR_PLUS_0;

	hheight = g_Font[font_head]->getHeight ();
	iheight = g_Font[font_info]->getHeight ();
	sheight = g_Font[font_small]->getHeight ();

	max_width = frameBuffer->getScreenWidth(true);
	max_height = frameBuffer->getScreenHeight(true);

	width =  frameBuffer->getScreenWidth();
	height = frameBuffer->getScreenHeight();
	x = frameBuffer->getScreenX();
	y = frameBuffer->getScreenY();

	sigBox_pos = 0;
	paint_mode = 0;

	bit_s = 0;
	abit_s = 0;
}

CStreamInfo2::~CStreamInfo2 ()
{
	videoDecoder->Pig(-1, -1, -1, -1);
}

int CStreamInfo2::exec()
{
	paint(paint_mode);
	doSignalStrengthLoop();
	hide();

	return menu_return::RETURN_EXIT_ALL;
}

int CStreamInfo2::exec (CMenuTarget * parent, const std::string &)
{

	if (parent)
		parent->hide ();

	paint (paint_mode);
	doSignalStrengthLoop ();
	hide ();
	return menu_return::RETURN_EXIT_ALL;
}


int CStreamInfo2::doSignalStrengthLoop ()
{
#define RED_BAR 40
#define YELLOW_BAR 70
#define GREEN_BAR 100
#define BAR_WIDTH 150 
#define BAR_HEIGHT 12 

	sigscale = new CScale(BAR_WIDTH, BAR_HEIGHT, RED_BAR, GREEN_BAR, YELLOW_BAR);
	snrscale = new CScale(BAR_WIDTH, BAR_HEIGHT, RED_BAR, GREEN_BAR, YELLOW_BAR);

	neutrino_msg_t msg;
	unsigned long long maxb, minb, lastb, tmp_rate;
	int cnt = 0,i=0;
	uint16_t ssig, ssnr;
	uint32_t  ber;
	char tmp_str[150];
	int offset_tmp = 0;
	int offset = g_Font[font_info]->getRenderWidth(g_Locale->getText (LOCALE_STREAMINFO_BITRATE));
	int sw = g_Font[font_info]->getRenderWidth ("99999.999");
	maxb = minb = lastb = 0;
	int mm = g_Font[font_info]->getRenderWidth ("Max");//max min lenght

	int pid = (g_RemoteControl->current_PIDs.PIDs.vpid > 0)
		? g_RemoteControl->current_PIDs.PIDs.vpid
		: g_RemoteControl->current_PIDs.APIDs[g_RemoteControl->current_PIDs.PIDs.selected_apid].pid;

	if (!bw_thread && (pid > 0))
		pthread_create(&bw_thread, 0, bandwidth_thread, (void *) &pid);

	while (1) {
		neutrino_msg_data_t data;

		unsigned long long timeoutEnd = CRCInput::calcTimeoutEnd_MS (100);
		g_RCInput->getMsgAbsoluteTimeout (&msg, &data, &timeoutEnd);

		ssig = frontend->getSignalStrength();
		ssnr = frontend->getSignalNoiseRatio();
		ber = frontend->getBitErrorRate();

		fesig[FESIG_SIG].val[FESIG_VAL_CUR] = ssig & 0xFFFF;
		fesig[FESIG_SNR].val[FESIG_VAL_CUR] = ssnr & 0xFFFF;
		fesig[FESIG_BER].val[FESIG_VAL_CUR] = ber;
		fesig[FESIG_RATE].val[FESIG_VAL_CUR] = rate;

		if(snrscale && sigscale)
			showSNR ();

		for (int i = 0; i < FESIG_MAX; i++)
			if (fesig[i].val[FESIG_VAL_CUR] > -1) {
				if (fesig[i].val[FESIG_VAL_MAX] < fesig[i].val[FESIG_VAL_CUR])
					fesig[i].val[FESIG_VAL_MAX] = fesig[i].val[FESIG_VAL_CUR];
				if (fesig[i].val[FESIG_VAL_MIN] < fesig[i].val[FESIG_VAL_CUR])
					fesig[i].val[FESIG_VAL_MIN] = fesig[i].val[FESIG_VAL_CUR];
			}

		paint_signal_fe();

		for (int i = 0; i < FESIG_MAX; i++)
			fesig[i].old = fesig[i].val[FESIG_VAL_CUR];

#if 0
		// switch paint mode
		if (msg == CRCInput::RC_red || msg == CRCInput::RC_blue || msg == CRCInput::RC_green || msg == CRCInput::RC_yellow) {
			hide ();
			if(sigscale)
				sigscale->reset();
			if(snrscale)
				snrscale->reset();

			paint_mode = !paint_mode;
			paint (paint_mode);
			continue;
		}
#endif
		// -- any key --> abort
		if (msg <= CRCInput::RC_MaxRC) {
			break;
		}
		// -- push other events
		if (msg > CRCInput::RC_MaxRC && msg != CRCInput::RC_timeout) {
			CNeutrinoApp::getInstance ()->handleMsg (msg, data);
		}
	}
	if(sigscale){
		delete sigscale;
		sigscale = NULL;
	}
	if(snrscale){
		delete snrscale;
		snrscale = NULL;
	}
	bw_thread_running = false;

	if (bw_thread && (pid > 0)) {
		pthread_join(bw_thread, NULL);
		bw_thread = 0;
	}

	return msg;
}

void CStreamInfo2::hide ()
{
  videoDecoder->Pig(-1, -1, -1, -1); // FIXME -- needed?
  frameBuffer->paintBackgroundBoxRel (0, 0, max_width, max_height);
}

void CStreamInfo2::paint_signal_fe_box(int _x, int _y, int w, int h)
{
	xd = 2 * w/9;

	g_Font[font_info]->RenderString(_x, _y+iheight, width-10, g_Locale->getText(LOCALE_STREAMINFO_SIGNAL), COL_MENUCONTENTDARK, 0, true, fgcolor);

	sigBox_x = _x;
	sigBox_y = _y+iheight+15;
	sigBox_w = w;
	sigBox_h = h-sheight*5;
	frameBuffer->paintBoxRel(sigBox_x,sigBox_y,sigBox_w+2,sigBox_h, COL_BLACK);

	sig_text_y = sigBox_y + sigBox_h + sheight;

	int maxmin_x = _x + 5;
	g_Font[font_small]->RenderString(maxmin_x, sig_text_y + sheight, 50, "max", COL_MENUCONTENTDARK, 0, true, fgcolor);
	g_Font[font_small]->RenderString(maxmin_x, sig_text_y + 3 * sheight, 50, "min", COL_MENUCONTENTDARK, 0, true, fgcolor);

	_x -= w/9;

	for (int i = 0; i < FESIG_MAX; i++) {
		fesig[i].x = _x + 5 + xd * (i + 1);
		frameBuffer->paintBoxRel(_x+xd *(i + 1), sig_text_y - 12,16,2, fesig[i].color);
		g_Font[font_small]->RenderString(_x+20+xd * (i + 1), sig_text_y, xd, fesig[i].title, COL_MENUCONTENTDARK, 0, true, fgcolor);
	}

	sigBox_pos = 0;
}

void CStreamInfo2::paint_signal_fe()
{
	int   x_now = sigBox_pos;
	sigBox_pos = (++sigBox_pos) % sigBox_w;

	frameBuffer->paintBoxRel(sigBox_x+x_now, sigBox_y, 2, sigBox_h, COL_BLACK);
	frameBuffer->paintBoxRel(sigBox_x+sigBox_pos, sigBox_y, 1, sigBox_h, COL_WHITE);

	for (int i = 0; i < FESIG_MAX; i++)
		if(fesig[i].val[FESIG_VAL_CUR] > -1) {
			if (fesig[i].old != fesig[i].val[FESIG_VAL_CUR])
				for (int j = 0; j <= FESIG_VAL_MIN; j++)
					SignalRenderStr(fesig[i].val[j], fesig[i].x, sig_text_y + (j + 1) * sheight);

			int yd = y_signal_fe (fesig[i].val[FESIG_VAL_CUR], fesig[i].limit, sigBox_h);
			if (sigBox_pos < 2)
				frameBuffer->paintPixel(sigBox_x + x_now, sigBox_y + sigBox_h - yd, fesig[i].color);
			else
				frameBuffer->paintLine(sigBox_x + x_now - 1, sigBox_y + sigBox_h - fesig[i].yd_old,
						       sigBox_x + x_now, sigBox_y + sigBox_h - yd, fesig[i].color);
			fesig[i].yd_old = yd;
		}
}

// -- calc y from max_range and max_y
int CStreamInfo2::y_signal_fe (long long value, long long max_value, int max_y)
{
	if (!max_value)
		max_value = 1;

	int l = ((long) max_y * (long) value) / (long) max_value;
	return (l > max_y) ? max_y : l;
}

void CStreamInfo2::SignalRenderStr(long long value, int _x, int _y)
{
	char str[30];

	frameBuffer->paintBoxRel(_x, _y - sheight, xd, sheight - 1, bgcolor);
	snprintf(str, sizeof(str), "%8lld", value);
	g_Font[font_small]->RenderString(_x, _y, xd, str, COL_MENUCONTENTDARK, 0, true, fgcolor);
}

void CStreamInfo2::paint (int mode)
{

	width =  frameBuffer->getScreenWidth();
	height = frameBuffer->getScreenHeight();
	x = frameBuffer->getScreenX();
	y = frameBuffer->getScreenY();
	int ypos = y + 5;
	int xpos = x + 10;

	// -- tech Infos, small signal graph
	const char *head_string = g_Locale->getText (LOCALE_STREAMINFO_HEAD);
	CVFD::getInstance ()->setMode (CVFD::MODE_MENU_UTF8, head_string);

	// paint backround, title, etc.
	frameBuffer->paintBoxRel (0, 0, max_width, max_height, bgcolor);
	g_Font[font_head]->RenderString (xpos, ypos + hheight + 1, width, head_string, COL_MENUHEAD, 0, true, fgcolor_head);	// UTF-8
	ypos += hheight;

	paint_techinfo (xpos, ypos);

	fesig[FESIG_BER].limit = 4000;
	fesig[FESIG_SIG].limit = 0xFFFF;
	fesig[FESIG_SNR].limit = 0xFFFF;
	fesig[FESIG_RATE].limit = (g_RemoteControl->current_PIDs.PIDs.vpid > 0) ? 25000000 : 512000;

	fesig[FESIG_BER].color = COL_RED;
	fesig[FESIG_SIG].color = COL_GREEN;
	fesig[FESIG_SNR].color = COL_BLUE;
	fesig[FESIG_RATE].color = COL_YELLOW;

	fesig[FESIG_BER].title = "BER";
	fesig[FESIG_SIG].title = "SNR";
	fesig[FESIG_SNR].title = "SIG";
	fesig[FESIG_RATE].title = "Bitrate";

	for (int i = 0; i < FESIG_MAX; i++) {
		fesig[i].old = -1;
		fesig[i].val[FESIG_VAL_MAX] = 0;
		fesig[i].val[FESIG_VAL_MIN] = 0;
		fesig[i].yd_old = 0;
	}

	paint_signal_fe_box (width - 2 * width/5 - 10, ypos + iheight, 2 * width/5, 2 * height/5);
}

void CStreamInfo2::paint_techinfo_line(int xpos, int ypos, const neutrino_locale_t loc, const char *txt, int xpos2, const char *txt2)
{
	if (!txt2)
		return;
	if (loc != NONEXISTANT_LOCALE)
		txt = g_Locale->getText(loc);
	if (!txt)
		return;

	char buf[100];
	snprintf(buf, sizeof(buf), "%s: ", txt);
	g_Font[font_info]->RenderString (xpos, ypos, width - xpos, txt, 0, 0, true, fgcolor);
	g_Font[font_info]->RenderString (xpos + xpos2, ypos, width - xpos - xpos2, txt2, 0, 0, true, fgcolor);
}

void CStreamInfo2::paint_techinfo(int xpos, int ypos)
{
	char buf[100];
	int xres, yres, aspectRatio, framerate;
	// paint labels
	int spaceoffset = 0;

	{
		neutrino_locale_t n_arr[] = {
			LOCALE_STREAMINFO_RESOLUTION, LOCALE_STREAMINFO_ARATIO, LOCALE_STREAMINFO_FRAMERATE,
			LOCALE_STREAMINFO_ARATIO, LOCALE_STREAMINFO_FRAMERATE,LOCALE_STREAMINFO_AUDIOTYPE,
			LOCALE_SATSETUP_SATELLITE, LOCALE_TIMERLIST_CHANNEL, NONEXISTANT_LOCALE
		};

		neutrino_locale_t *i = n_arr;
		while (*i != NONEXISTANT_LOCALE) {
			int w = g_Font[font_info]->getRenderWidth(g_Locale->getText (*i));
			if (w > spaceoffset)
				spaceoffset = w;
			i++;
		}
	}

	{
		char const *s_arr[] = {
			"Tp. Freq.", "ONid", "Sid", "TSid", "PMTpid", "Vpid", "Apid(s)", "VTXTpid", NULL
		};

		const char **i = s_arr;
		while (*i) {
			int w = g_Font[font_info]->getRenderWidth(*i);
			if (w > spaceoffset)
				spaceoffset = w;
			i++;
		}
	}

	spaceoffset += g_Font[font_info]->getRenderWidth(": ");
	ypos += iheight;

	//Video RESOLUTION
	ypos += iheight;
	videoDecoder->getPictureInfo(xres, yres, framerate);
	snprintf (buf, sizeof(buf), "%dx%d", xres, yres);
	paint_techinfo_line(xpos, ypos, LOCALE_STREAMINFO_RESOLUTION, NULL, spaceoffset, buf);

	//audio rate
	const char *ar;
	ypos += iheight;
	aspectRatio = videoDecoder->getAspectRatio();
	switch (aspectRatio) {
		case 0: ar = "4:3"; break;
		case 1: ar = "16:9"; break;
		case 2: ar = "14:9"; break;
		case 4: ar = "20:9"; break;
		default: ar = g_Locale->getText (LOCALE_STREAMINFO_ARATIO_UNKNOWN);
	}
	paint_techinfo_line(xpos, ypos, LOCALE_STREAMINFO_ARATIO, NULL, spaceoffset, ar);

	//Video FRAMERATE
	ypos += iheight;
	snprintf (buf, sizeof(buf), "%dfps", framerate);
	paint_techinfo_line(xpos, ypos, LOCALE_STREAMINFO_FRAMERATE, NULL, spaceoffset, buf);

	//AUDIOTYPE
	ypos += iheight;
#if 0
	// FIXME Our getAudioInfo in libcoolstream is a stub.
	int type, layer, freq, mode, bitrate;
	audioDecoder->getAudioInfo(type, layer, freq, bitrate, mode);
	const char *mpegmodes[4] = { "stereo", "joint_st", "dual_ch", "single_ch" };
	const char *ddmodes[8] = { "CH1/CH2", "C", "L/R", "L/C/R", "L/R/S", "L/C/R/S", "L/R/SL/SR", "L/C/R/SL/SR" };

	if(type == 0) {
		snprintf (buf, sizeof(buf), "MPEG %s (%d)", mpegmodes[mode], freq);
	} else {
		snprintf (buf,  sizeof(buf),"DD %s (%d)", ddmodes[mode], freq);
	}
#endif

	paint_techinfo_line(xpos, ypos, LOCALE_STREAMINFO_AUDIOTYPE, NULL, spaceoffset, g_RemoteControl->current_PIDs.APIDs[g_RemoteControl->current_PIDs.PIDs.selected_apid].desc);

	//satellite
	t_satellite_position satellitePosition = CNeutrinoApp::getInstance ()->channelList->getActiveSatellitePosition ();
	sat_iterator_t sit = satellitePositions.find(satellitePosition);
	if(sit != satellitePositions.end()) {
	ypos += iheight;
		paint_techinfo_line(xpos, ypos, LOCALE_SATSETUP_SATELLITE, NULL, spaceoffset, sit->second.name.c_str());
	}

	//channel
	ypos += iheight;
	CChannelList *channelList = CNeutrinoApp::getInstance ()->channelList;
	int curnum = channelList->getActiveChannelNumber();
	CZapitChannel * channel = channelList->getChannel(curnum);
	CZapitClient::CCurrentServiceInfo si = g_Zapit->getCurrentServiceInfo ();
	paint_techinfo_line(xpos, ypos, LOCALE_TIMERLIST_CHANNEL, NULL, spaceoffset, channelList->getActiveChannelName().c_str());

	//tsfrequenz
	ypos += iheight;
	char * f=NULL, *s=NULL, *m=NULL;
	if(frontend->getInfo()->type == FE_QPSK) {
		frontend->getDelSys((fe_code_rate_t)si.fec, dvbs_get_modulation((fe_code_rate_t)si.fec), f, s, m);
		snprintf (buf, sizeof(buf), "%d.%d (%c) %d %s %s %s", si.tsfrequency / 1000, si.tsfrequency % 1000,
			si.polarisation ? 'V' : 'H', si.rate / 1000,f,m,s=="DVB-S2"?"S2":"S1");
		paint_techinfo_line(xpos, ypos, NONEXISTANT_LOCALE, "Tp. Freq.", spaceoffset, buf);
	}

	//onid
	ypos+= iheight;
	snprintf(buf, sizeof(buf), "0x%04x (%i)", si.onid, si.onid);
	paint_techinfo_line(xpos, ypos, NONEXISTANT_LOCALE, "ONid", spaceoffset, buf);

	//sid
	ypos+= iheight;
	snprintf(buf, sizeof(buf), "0x%04x (%i)", si.sid, si.sid);
	paint_techinfo_line(xpos, ypos, NONEXISTANT_LOCALE, "Sid", spaceoffset, buf);

	//tsid
	ypos+= iheight;
	snprintf(buf, sizeof(buf), "0x%04x (%i)", si.tsid, si.tsid);
	paint_techinfo_line(xpos, ypos, NONEXISTANT_LOCALE, "TSid", spaceoffset, buf);
	
	//pmtpid
	ypos+= iheight;
	snprintf(buf, sizeof(buf), "0x%04x (%i)", si.pmtpid, si.pmtpid);
	paint_techinfo_line(xpos, ypos, NONEXISTANT_LOCALE, "PMTpid", spaceoffset, buf);

	//vpid
	ypos+= iheight;
	if (g_RemoteControl->current_PIDs.PIDs.vpid > 0 )
		snprintf(buf, sizeof(buf),  "0x%04x (%i)",
			g_RemoteControl->current_PIDs.PIDs.vpid, g_RemoteControl->current_PIDs.PIDs.vpid );
	else
		snprintf(buf, sizeof(buf),  "%s", g_Locale->getText(LOCALE_STREAMINFO_NOT_AVAILABLE));

	paint_techinfo_line(xpos, ypos, NONEXISTANT_LOCALE, "Vpid", spaceoffset, buf);

	//apid
	ypos+= iheight;
	g_Font[font_info]->RenderString(xpos, ypos, width*2/3-10, "Apid(s):" , COL_MENUCONTENTDARK, 0, true, fgcolor);
	if (g_RemoteControl->current_PIDs.APIDs.empty()){
		snprintf(buf, sizeof(buf), "%s", g_Locale->getText(LOCALE_STREAMINFO_NOT_AVAILABLE));
	} else {
		unsigned int sw=spaceoffset;
		for (int i= 0; (i<g_RemoteControl->current_PIDs.APIDs.size()) && (i<10); i++)
		{
			snprintf(buf, sizeof(buf), "0x%04x (%i)", g_RemoteControl->current_PIDs.APIDs[i].pid,
				g_RemoteControl->current_PIDs.APIDs[i].pid );
			g_Font[font_info]->RenderString(xpos+sw, ypos, width*2/3-10, buf, 0, 0, true,
			(i == g_RemoteControl->current_PIDs.PIDs.selected_apid) ? fgcolor_head : fgcolor);
			sw = g_Font[font_info]->getRenderWidth(buf)+sw+10;
			if (((i+1)%3 == 0) &&(g_RemoteControl->current_PIDs.APIDs.size()-1 > i)){
				// if we have lots of apids, put "intermediate" line with pids
				ypos+= iheight;
				sw=spaceoffset;
			}
		}
	}

	//vtxtpid
	ypos += iheight;
	if ( g_RemoteControl->current_PIDs.PIDs.vtxtpid == 0 )
        	snprintf((char*) buf, sizeof(buf), "%s", g_Locale->getText(LOCALE_STREAMINFO_NOT_AVAILABLE));
	else
        	snprintf((char*) buf, sizeof(buf), "0x%04x (%i)", g_RemoteControl->current_PIDs.PIDs.vtxtpid,
			g_RemoteControl->current_PIDs.PIDs.vtxtpid);
	paint_techinfo_line(xpos, ypos, NONEXISTANT_LOCALE, "VTXTpid", spaceoffset, buf);

	yypos = ypos;
}

int CStreamInfo2Handler::exec(CMenuTarget* parent, const std::string &actionkey)
{
	int res = menu_return::RETURN_EXIT_ALL;
	if (parent){
		parent->hide();
	}
	CStreamInfo2 *e = new CStreamInfo2;
	e->exec();
	delete e;
//	CZapitClient zapit;
//	zapit.Rezap();
//	system("(/usr/local/bin/pzapit -rz) &");
	return res;
}

void CStreamInfo2::showSNR ()
{
	char percent[40];
	int mheight = g_Font[font_info]->getHeight();
	int perc;

	perc = ((fesig[FESIG_SIG].val[FESIG_VAL_CUR] & 0xFFFF) * 100) >> 16;
	if(perc != fesig[FESIG_SIG].oldpercent) {
	  	int posy = yypos + (mheight/2) +50;
		int posx = x + 10;
		snprintf(percent, sizeof(percent), "%d%% %s", perc, fesig[FESIG_SIG].title);
		int sw = g_Font[font_info]->getRenderWidth (percent);
		sigscale->paint(posx - 1, posy, perc);
		posx = posx + BAR_WIDTH + 3;
		frameBuffer->paintBoxRel(posx, posy -1, sw, mheight-8, bgcolor);
		g_Font[font_info]->RenderString(posx + 2, posy + mheight-5, sw, percent, COL_MENUCONTENTDARK, 0, false, fgcolor);
		fesig[FESIG_SIG].oldpercent = perc;
	}

	perc = ((fesig[FESIG_SNR].val[FESIG_VAL_CUR] & 0xFFFF) * 100) >> 16;
	if(perc != fesig[FESIG_SNR].oldpercent) {
	  	int posy = yypos + mheight + 4;
		int posx = x + 10;
		snprintf(percent, sizeof(percent),  "%d%% %s", perc, fesig[FESIG_SNR].title);
		int sw = g_Font[font_info]->getRenderWidth (percent);
		snrscale->paint(posx - 1, posy+2, perc);
		posx = posx + BAR_WIDTH + 3;
		frameBuffer->paintBoxRel(posx, posy - 1, sw, mheight-8, bgcolor, 0, true);
		g_Font[font_info]->RenderString(posx + 2, posy + mheight-5, sw, percent, COL_MENUCONTENTDARK, 0, false, fgcolor);
		fesig[FESIG_SNR].oldpercent = perc;
	}
}


// The stuff below is based on dvbsnoop/src/dvb_api/dmx_tspidbandwidth.c
// --martii

/*
 * some definition
 */
#define TS_LEN			188
#define TS_SYNC_BYTE		0x47
#define TS_BUF_SIZE		(TS_LEN * 2048)		/* fix dmx buffer size */

static inline unsigned long timeval_to_ms(const struct timeval *tv)
{
        return (tv->tv_sec * 1000) + ((tv->tv_usec + 500) / 1000);
}

static long inline delta_time_ms (struct timeval *tv, struct timeval *last_tv)
{
        return timeval_to_ms(tv) - timeval_to_ms(last_tv);
}

static int sync_ts (u_char *buf, int len)
{
        int  i;

        // find TS sync byte...
        // SYNC ...[188 len] ...SYNC...

        for (i=0; i < len; i++)
                if (buf[i] == TS_SYNC_BYTE) {
                   if ((i+TS_LEN) < len) {
                      if (buf[i+TS_LEN] != TS_SYNC_BYTE) continue;
                   }
                   break;
                }
        return i;
}

static void* bandwidth_thread(void *arg)
{
	bw_thread_running = true;
	u_char 	 buf[TS_BUF_SIZE];
	struct pollfd  pfd;
	struct dmx_pes_filter_params flt;
	int 		 dmxfd;
	long           b;

	int pid = *((int *)arg);

	// -- open DVR device for reading
	pfd.events = POLLIN | POLLPRI;
	if((pfd.fd = open("/dev/dvb/adapter0/dvr0",O_RDONLY|O_NONBLOCK)) < 0){
    		pthread_exit(NULL);
	}

	if ((dmxfd=open("/dev/dvb/adapter0/demux0", O_RDWR)) < 0) {
		close(pfd.fd);
    		pthread_exit(NULL);
	}

	fcntl(pfd.fd, F_SETFL, O_NONBLOCK);

	ioctl (dmxfd,DMX_SET_BUFFER_SIZE, sizeof(buf));
	memset(&flt, 0, sizeof(flt));
	flt.pid = pid;
	flt.input = DMX_IN_FRONTEND;
	flt.output = DMX_OUT_TS_TAP;
	flt.pes_type = DMX_PES_OTHER;
	flt.flags = DMX_IMMEDIATE_START;
	if (ioctl(dmxfd, DMX_SET_PES_FILTER, &flt) < 0) {
		fprintf(stderr, "%s %d: DMX_SET_PES_FILTER", __func__, __LINE__);
		close(pfd.fd);
		close(dmxfd);
    		pthread_exit(NULL);
	}

	struct timeval last_tv;
	gettimeofday (&last_tv, NULL);

	while (bw_thread_running) {
		int b_len = 0;

		// -- we will poll the PID in 1 secs interval
		int timeout_initial = 1000;
		int timeout = timeout_initial;

		unsigned long long b_total = 0;

		while (timeout > 0 && bw_thread_running) {

			if ((poll(&pfd, 1, timeout)) > 0 &&  (pfd.revents & POLLIN)) {
				b_len = read(pfd.fd, buf, sizeof(buf));
				if (b_len < 0)
					pthread_exit(NULL);
				struct timeval tv;
				gettimeofday (&tv, NULL);
				int b_start = 0;
				if (b_len >= TS_LEN)
					b_start = sync_ts (buf, b_len);
				else
					b_len = 0;

				b = b_len - b_start;
				if (b < 0)
					continue;

				b_total += b;

				long d_tim_ms = delta_time_ms (&tv, &last_tv);
				timeout = timeout_initial - d_tim_ms;

				if (timeout <= 0) {
					if (d_tim_ms < 1) d_tim_ms = 1;   //  ignore usecs 
					rate = (8000 * b_total)/d_tim_ms;
					last_tv.tv_sec  =  tv.tv_sec;
					last_tv.tv_usec =  tv.tv_usec;
				}
			}
		}
	}

	if (ioctl(dmxfd, DMX_STOP) < 0)
		fprintf(stderr, "%s %d: DMX_STOP", __func__, __LINE__);

	close(dmxfd);
	close(pfd.fd);
	pthread_exit(NULL);
}
