#ifdef EVOLUX
/*
	LCD-Daemon  -   DBoxII-Project

	Copyright (C) 2001 Steffen Hehn 'McClean'
	Homepage: http://dbox.cyberphoria.org/


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

// FIXME -- there is too much unused code left, but we may need it in the future ...

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <driver/lcdd.h>
#include <global.h>
#include <neutrino.h>
#include <system/settings.h>

#include <fcntl.h>
#include <sys/timeb.h>
#include <time.h>
#include <unistd.h>
#include <poll.h>
#include <math.h>

#include <daemonc/remotecontrol.h>
#include <system/set_threadname.h>

#include <aotom_main.h>

extern CRemoteControl * g_RemoteControl; /* neutrino.cpp */

CLCD::CLCD()
{
	thrTime = 0;

	has_lcd = 1;

	fd = open("/dev/vfd", O_RDWR | O_CLOEXEC);

	if(fd < 0) {
		perror("/dev/vfd");
		has_lcd = 0;
	}

	mode = MODE_MENU_UTF8;
	servicename = "";

	int fds[2];
	if (pipe2(fds, O_CLOEXEC | O_NONBLOCK))
		return;

	time_notify_reader = fds[0];
	time_notify_writer = fds[1];

	if (pthread_create (&thrTime, NULL, TimeThread, NULL))
		perror("[lcdd]: pthread_create(TimeThread)");

}

CLCD::~CLCD()
{
	timeThreadRunning = false;
	if (thrTime) {
		write(time_notify_writer, "", 1);
		pthread_join(thrTime, NULL);
	}
	
	if(fd > 0)
		close(fd);
}

CLCD* CLCD::getInstance()
{
	static CLCD* lcdd = NULL;
	if(lcdd == NULL) {
		lcdd = new CLCD();
	}
	return lcdd;
}

void CLCD::wake_up() {
}

void* CLCD::TimeThread(void *arg)
{
        set_threadname("CLCL::TimeThread");
	CLCD *cvfd = CLCD::getInstance();
	cvfd->timeThreadRunning = true;
	char buf[10];
	cvfd->waitSec = 0;
	struct pollfd fds;
	memset(&fds, 0, sizeof(fds));
	fds.fd = cvfd->time_notify_reader;
	fds.events = POLLIN;
	while (cvfd->timeThreadRunning) {
		int res = poll(&fds, 1, cvfd->waitSec * 1000);
		if (res == 1)
			while (0 < read(fds.fd, buf, sizeof(buf)));
		if (!cvfd->showclock) {
			cvfd->waitSec = -1; // forever
			continue;
		}
		switch (res) {
		case 0: // timeout, update displayed time
			cvfd->showTime();
			continue;
		case 1: // re-schedule time display
			continue;
		default:
			cvfd->timeThreadRunning = false;
		}
	}
	close (cvfd->time_notify_reader);
	cvfd->time_notify_reader = -1;
	close (cvfd->time_notify_writer);
	cvfd->time_notify_writer = -1;
	return NULL;
}

void CLCD::setlcdparameter(int dimm, const int power)
{
	if(!has_lcd) return;

	if(dimm < 0)
		dimm = 0;
	else if(dimm > 15)
		dimm = 15;

	if(!power)
		dimm = 0;

	struct aotom_ioctl_data vData;
	vData.u.brightness.level = dimm;
	int ret = ioctl(fd, VFDBRIGHTNESS, &vData);
	if(ret < 0)
		perror("VFDBRIGHTNESS");
}

void CLCD::setlcdparameter(void)
{
	if(!has_lcd) return;
	int last_toggle_state_power = g_settings.lcd_setting[SNeutrinoSettings::LCD_POWER];
	setlcdparameter((mode == MODE_STANDBY)
		? g_settings.lcd_setting[SNeutrinoSettings::LCD_STANDBY_BRIGHTNESS]
		: g_settings.lcd_setting[SNeutrinoSettings::LCD_BRIGHTNESS], last_toggle_state_power);
}

void CLCD::showServicename(const std::string name, bool)
{
	if(!has_lcd) return;

	servicename = name;
	if (mode != MODE_TVRADIO)
		return;

	ShowText((char *) name.c_str());
	wake_up();
}

// from fp_control/Spark.c:

static double modJulianDate(struct tm *theTime)
{
        double date;
        int month;
        int day;
        int year;

        year  = theTime->tm_year + 1900;
        month = theTime->tm_mon + 1;
        day   = theTime->tm_mday;

        date = day - 32076 +
                1461 * (year + 4800 + (month - 14)/12)/4 +
                367 * (month - 2 - (month - 14)/12*12)/12 -
                3 * ((year + 4900 + (month - 14)/12)/100)/4;

        date += (theTime->tm_hour + 12.0)/24.0;
        date += (theTime->tm_min)/1440.0;
        date += (theTime->tm_sec)/86400.0;

        date -= 2400000.5;

        return date;
}

void CLCD::showTime(bool)
{
	int m = g_settings.lcd_setting[ (mode == MODE_STANDBY)
			? SNeutrinoSettings::LCD_STANDBY_DISPLAYMODE
			: SNeutrinoSettings::LCD_DISPLAYMODE];
	switch (m) {
		case LCD_DISPLAYMODE_TIMEONLY:
			break;
		case LCD_DISPLAYMODE_TIMEOFF:
			return;
		case LCD_DISPLAYMODE_OFF: // FIXME. May keep time from syncing on Pingulux+
			Clear();
			return;
		default:
			break;
	}

	struct tm tm;
	time_t now = time(NULL);
	localtime_r(&now, &tm);

	struct aotom_ioctl_data vData;
	double mjd = modJulianDate(&tm);
	int mjd_int = mjd;

	vData.u.time.time[0] = mjd_int >> 8;
	vData.u.time.time[1] = mjd_int & 0xff;
	vData.u.time.time[2] = tm.tm_hour;
	vData.u.time.time[3] = tm.tm_min;
	vData.u.time.time[4] = tm.tm_sec;

	if (ioctl(fd, VFDSETTIME, &vData) < 0) {
		char buf[10];
		strftime(buf, sizeof(buf), "%H%M", &tm);
		ShowText(buf, false);
	}
	waitSec = 60 - tm.tm_sec;
	if (waitSec == 0)
		waitSec = 60;
}

void CLCD::showRCLock(int duration)
{
}

void CLCD::showVolume(const char vol, const bool perform_update)
{
	char buf[10];
	snprintf(buf, sizeof(buf), "%4d", vol);
	ShowText(buf);
}

void CLCD::showPercentOver(const unsigned char perc, const bool /*perform_update*/, const MODES)
{
}

void CLCD::showMenuText(const int position, const char * txt, const int highlight, const bool utf_encoded)
{
	if(!has_lcd) return;
	if (mode != MODE_MENU_UTF8)
		return;

	ShowText((char *) txt);
	wake_up();
}

void CLCD::showAudioTrack(const std::string & artist, const std::string & title, const std::string & album)
{
	if(!has_lcd) return;
	if (mode != MODE_AUDIO) 
		return;
printf("CLCD::showAudioTrack: %s\n", title.c_str());
	ShowText((char *) title.c_str());
	wake_up();
}

void CLCD::showAudioPlayMode(AUDIOMODES m)
{
}

void CLCD::showAudioProgress(const char perc, bool isMuted)
{
}

void CLCD::setMode(const MODES m, const char * const title)
{
	if(!has_lcd) return;

	MODES lastmode = mode;

	if(mode == MODE_AUDIO)
		ShowIcon(VFD_ICON_MP3, false);
	if(strlen(title))
		ShowText((char *) title);
	mode = m;
	setlcdparameter();

	switch (m) {
	case MODE_TVRADIO:
		resume(m != lastmode);
		break;
	case MODE_AUDIO:
		break;
	case MODE_SCART:
		resume();
		break;
	case MODE_MENU_UTF8:
		pause();
		break;
	case MODE_SHUTDOWN:
		pause();
		break;
	case MODE_STANDBY:
		resume();
		break;
	}
	wake_up();
}


void CLCD::setBrightness(int bright)
{
	if(!has_lcd) return;

	//g_settings.lcd_setting[SNeutrinoSettings::LCD_BRIGHTNESS] = bright;
	setlcdparameter();
}

int CLCD::getBrightness()
{
	//FIXME for old neutrino.conf
	if(g_settings.lcd_setting[SNeutrinoSettings::LCD_BRIGHTNESS] > 7)
		g_settings.lcd_setting[SNeutrinoSettings::LCD_BRIGHTNESS] = 7;

	return g_settings.lcd_setting[SNeutrinoSettings::LCD_BRIGHTNESS];
}

void CLCD::setBrightnessStandby(int bright)
{
	if(!has_lcd) return;

//	g_settings.lcd_setting[SNeutrinoSettings::LCD_STANDBY_BRIGHTNESS] = bright;
	setlcdparameter();
}

int CLCD::getBrightnessStandby()
{
	//FIXME for old neutrino.conf
	if(g_settings.lcd_setting[SNeutrinoSettings::LCD_STANDBY_BRIGHTNESS] > 7)
		g_settings.lcd_setting[SNeutrinoSettings::LCD_STANDBY_BRIGHTNESS] = 7;
	return g_settings.lcd_setting[SNeutrinoSettings::LCD_STANDBY_BRIGHTNESS];
}

void CLCD::setPower(int power)
{
}

int CLCD::getPower()
{
	return g_settings.lcd_setting[SNeutrinoSettings::LCD_POWER];
}

void CLCD::togglePower(void)
{
}

void CLCD::setMuted(bool mu)
{
}

void CLCD::resume(bool showServiceName)
{
	if(!has_lcd) return;
	showclock = true;
	waitSec = 0;
	if (showServiceName)
		ShowText(NULL);
	write(time_notify_writer, "", 1);
}

void CLCD::pause()
{
	if(!has_lcd) return;
	showclock = false;
}

void CLCD::Lock()
{
}

void CLCD::Unlock()
{
}

void CLCD::Clear()
{
	if(!has_lcd) return;
        struct vfd_ioctl_data data;
	data.start_address = 0x01;
	data.length = 0x0;
	
	int ret = ioctl(fd, VFDDISPLAYCLR, &data);
	if(ret < 0)
		perror("IOC_VFD_CLEAR_ALL");
}

void CLCD::ShowIcon(vfd_icon icon, bool show)
{
	int which;
	switch (icon) {
		case VFD_ICON_PLAY:
		//case VFD_ICON_TIMESHIFT:
			which = LED_GREEN;
			break;
		case VFD_ICON_CAM1:
		case VFD_ICON_RECORD:
			which = LED_RED;
			break;
		default:
			return;
	}
	struct aotom_ioctl_data vData;
	vData.u.led.led_nr = which;
	vData.u.led.on = show ? LOG_ON : LOG_OFF;
	ioctl(fd, VFDSETLED, &vData);
}

void CLCD::ShowText(const char * str, bool rescheduleTime)
{
	if (!str && servicename.length() > 0)
		str = (char *) servicename.c_str();

	int m = g_settings.lcd_setting[ (mode == MODE_STANDBY)
			? SNeutrinoSettings::LCD_STANDBY_DISPLAYMODE
			: SNeutrinoSettings::LCD_DISPLAYMODE];
	switch (m) {
		case LCD_DISPLAYMODE_TIMEONLY:
			if (!rescheduleTime)
				break;
			return;
		case LCD_DISPLAYMODE_TIMEOFF:
			if (rescheduleTime)
				break;
			return;
		case LCD_DISPLAYMODE_OFF:
			Clear();
			return;
		default:
			break;
	}

        printf("CLCD::ShowText: [%s]\n", str);

	if (str) {
		std::string s = std::string(str);

		size_t start = s.find_first_not_of (" \t\n");
		if (start != std::string::npos) {
			size_t end = s.find_last_not_of (" \t\n");
			s = s.substr(start, end - start + 1);
		}
		if (s.length() < 1)
			waitSec = 0;
		else if (write(fd , s.c_str(), s.length()) < 0)
			perror("write to vfd failed");
		waitSec = 8;
	} else
		waitSec = 0;

	if (rescheduleTime && (time_notify_writer > -1)) {
		write(time_notify_writer, "", 1);
	}
}

void CLCD::init(const char *, const char *, const char *, const char *, const char *, const char *)
{
}

void CLCD::setEPGTitle(const std::string)
{
}

#else
/*
	Routines to drive the SPARK boxes' 4 digit LED display

	(C) 2012 Stefan Seyfried

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
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <driver/lcdd.h>
#include <driver/framebuffer.h>

#include <global.h>
#include <neutrino.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
//#include <math.h>
#include <sys/stat.h>

#include "spark_led.h"

static char volume = 0;
//static char percent = 0;
static bool power = true;
static bool muted = false;
static bool showclock = true;
static time_t last_display = 0;
static char display_text[64] = { 0 };
static bool led_r = false;
static bool led_g = false;

static inline int dev_open()
{
	int fd = open("/dev/vfd", O_RDWR);
	if (fd < 0)
		fprintf(stderr, "[neutrino] spark_led: open /dev/vfd: %m\n");
	return fd;
}

static void display(const char *s, bool update_timestamp = true)
{
	int fd = dev_open();
	int len = strlen(s);
	if (fd < 0)
		return;
printf("spark_led:%s '%s'\n", __func__, s);
	write(fd, s, len);
	close(fd);
	if (update_timestamp)
	{
		last_display = time(NULL);
		/* increase timeout to ensure that everything is displayed
		 * the driver displays 5 characters per second */
		if (len > 4)
			last_display += (len - 4) / 5;
	}
}

CLCD::CLCD()
{
	/* do not show menu in neutrino... */
	has_lcd = false;
}

CLCD* CLCD::getInstance()
{
	static CLCD* lcdd = NULL;
	if (lcdd == NULL)
		lcdd = new CLCD();
	return lcdd;
}

void CLCD::wake_up()
{
}

void* CLCD::TimeThread(void *)
{
	while(1) {
		sleep(1);
		CLCD::getInstance()->showTime();
		/* hack, just if we missed the blit() somewhere
		 * this will update the framebuffer once per second */
		if (getenv("SPARK_NOBLIT") == NULL)
			CFrameBuffer::getInstance()->blit();
	}
	return NULL;
}

void CLCD::init(const char *, const char *, const char *, const char *, const char *, const char *)
{
	setMode(MODE_TVRADIO);
	if (pthread_create (&thrTime, NULL, TimeThread, NULL) != 0 ) {
		perror("[neutino] CLCD::init pthread_create(TimeThread)");
		return ;
	}
}

void CLCD::setlcdparameter(void)
{
}

void CLCD::showServicename(std::string, bool)
{
}

void CLCD::setled(int red, int green)
{
	struct aotom_ioctl_data d;
	int leds[2] = { red, green };
	int i;
	int fd = dev_open();
	if (fd < 0)
		return;

printf("spark_led:%s red:%d green:%d\n", __func__, red, green);

	for (i = 0; i < 2; i++)
	{
		if (leds[i] == -1)
			continue;
		d.u.led.led_nr = i;
		d.u.led.on = leds[i];
		if (ioctl(fd, VFDSETLED, &d) < 0)
			fprintf(stderr, "[neutrino] spark_led setled VFDSETLED: %m\n");
	}
	close(fd);
}

void CLCD::showTime(bool force)
{
	static bool blink = false;
	int red = -1, green = -1;

	if (mode == MODE_SHUTDOWN)
	{
		setled(1, 1);
		return;
	}

	time_t now = time(NULL);
	if (display_text[0])
	{
		display(display_text);
		display_text[0] = '\0';
	}
	else if (power && (force || (showclock && (now - last_display) > 4)))
	{
		char timestr[5];
		struct tm *t;
		static int hour = 0, minute = 0;

		t = localtime(&now);
		if (force || last_display || (hour != t->tm_hour) || (minute != t->tm_min)) {
			hour = t->tm_hour;
			minute = t->tm_min;
			sprintf(timestr, "%02d%02d", hour, minute);
			display(timestr, false);
			last_display = 0;
		}
	}

	if (led_r)
		red = blink;
	blink = !blink;
	if (led_g)
		green = blink;

	if (led_r || led_g)
		setled(red, green);
}

void CLCD::showRCLock(int)
{
}

/* update is default true, the mute code sets it to false
 * to force an update => inverted logic! */
void CLCD::showVolume(const char vol, const bool update)
{
	char s[5];
	if (vol == volume && update)
		return;

	volume = vol;
	/* char is unsigned, so vol is never < 0 */
	if (volume > 100)
		volume = 100;

	if (muted)
		strcpy(s, "mute");
	else
		sprintf(s, "%4d", volume);

	display(s);
}

void CLCD::showPercentOver(const unsigned char perc, const bool /*perform_update*/, const MODES)
{
}

void CLCD::showMenuText(const int, const char *text, const int, const bool)
{
	if (mode != MODE_MENU_UTF8)
		return;
	strncpy(display_text, text, sizeof(display_text) - 1);
	display_text[sizeof(display_text) - 1] = '\0';
}

void CLCD::showAudioTrack(const std::string &, const std::string & title, const std::string &)
{
	if (mode != MODE_AUDIO)
		return;
//	ShowText(title.c_str());
}

void CLCD::showAudioPlayMode(AUDIOMODES)
{
}

void CLCD::showAudioProgress(const char, bool)
{
}

void CLCD::setMode(const MODES m, const char * const)
{
	mode = m;
printf("spark_led:%s %d\n", __func__, (int)m);

	switch (m) {
	case MODE_TVRADIO:
		setled(0, 0);
		showclock = true;
		power = true;
		showTime();
		break;
	case MODE_SHUTDOWN:
		showclock = false;
		Clear();
		break;
	case MODE_STANDBY:
		setled(0, 1);
		showclock = true;
		showTime(true);
		break;
	default:
		showclock = true;
		showTime();
	}
printf("spark_led:%s %d end\n", __func__, (int)m);
}

void CLCD::setBrightness(int)
{
}

int CLCD::getBrightness()
{
	return 0;
}

void CLCD::setBrightnessStandby(int)
{
}

int CLCD::getBrightnessStandby()
{
	return 0;
}

void CLCD::setPower(int)
{
}

int CLCD::getPower()
{
	return 0;
}

void CLCD::togglePower(void)
{
	power = !power;
	if (!power)
		Clear();
	else
		showTime(true);
}

void CLCD::setMuted(bool mu)
{
printf("spark_led:%s %d\n", __func__, mu);
	muted = mu;
	showVolume(volume, false);
}

void CLCD::resume()
{
}

void CLCD::pause()
{
}

void CLCD::Lock()
{
}

void CLCD::Unlock()
{
}

void CLCD::Clear()
{
	int fd = dev_open();
	if (fd < 0)
		return;
	int ret = ioctl(fd, VFDDISPLAYCLR);
	if(ret < 0)
		perror("[neutrino] spark_led Clear() VFDDISPLAYCLR");
	close(fd);
printf("spark_led:%s\n", __func__);
}

void CLCD::ShowIcon(vfd_icon i, bool on)
{
	switch (i)
	{
		case VFD_ICON_CAM1:
			led_r = on;
			setled(led_r, -1); /* switch instant on / switch off if disabling */
			break;
		case VFD_ICON_PLAY:
			led_g = on;
			setled(-1, led_g);
			break;
		default:
			break;
	}
}

void CLCD::setEPGTitle(const std::string)
{
}
#endif
