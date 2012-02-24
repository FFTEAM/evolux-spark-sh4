#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <global.h>
#include <neutrino.h>
#include <pthread.h>
#include <string>

#include <sys/timeb.h>
#include <time.h>
#include <sys/param.h>
#include "infoclock.h"

CInfoClock::CInfoClock()
{
	frameBuffer      = CFrameBuffer::getInstance();

	time_height = g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_CHANNAME]->getHeight();
	int t1 = g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_CHANNAME]->getRenderWidth(widest_number);
	int t2 = g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_CHANNAME]->getRenderWidth(":");
	time_width = t1*6 + t2*2;
	thrTimer = 0;
}

CInfoClock::~CInfoClock()
{
	if(thrTimer)
		pthread_cancel(thrTimer);
	thrTimer = 0;
}

void CInfoClock::paintTime( bool show_dot)
{
        int dx = 40; // the mute icon might be placed here, see current_muted
        int x = g_settings.screen_EndX - dx;
        int y = g_settings.screen_StartY;

	char timestr[10];
	time_t tm;

	tm = time(0);
	strftime((char*) &timestr, sizeof(timestr), show_dot ? "%H:%M:%S" : "%H.%M:%S", localtime(&tm));
	x -= time_width;
	frameBuffer->paintBoxRel(x, y, time_width, time_height, 0x22000000 /* black, but mostly transparent */);
	int len = g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_CHANNAME]->getRenderWidth(timestr);
	g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_CHANNAME]->RenderString(x + ((time_width - len)>>1), y + time_height, len, timestr, COL_MENUCONTENT, 0, false, 0xffffffff /* white */);
}

void* CInfoClock::TimerProc(void *arg)
{

	bool show_dot = false;
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,0);
 	pthread_setcanceltype (PTHREAD_CANCEL_ASYNCHRONOUS,0);

	CInfoClock *InfoClock = (CInfoClock*) arg;

	InfoClock->paintTime(show_dot);
	while(1) {
		sleep(1);
		show_dot = !show_dot;
		InfoClock->paintTime(show_dot);
	}
}

void CInfoClock::StartClock()
{
	if(!thrTimer) {
		pthread_create (&thrTimer, NULL, TimerProc, (void*) this) ;
		pthread_detach(thrTimer);
	}
}

void CInfoClock::StopClock()
{
	if(thrTimer) {
        	int dx = 40; // the mute icon might be placed here, see current_muted
		int x = g_settings.screen_EndX - dx;
		int y = g_settings.screen_StartY;
		x -= time_width;
		pthread_cancel(thrTimer);
		thrTimer = 0;
		frameBuffer->paintBackgroundBoxRel(x, y, time_width, time_height);
	}
}
