#include <stdio.h>

#include "init_lib.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "lirmp_input.h"
#include "pwrmngr.h"

#include "lt_debug.h"
#define lt_debug(args...) _lt_debug(TRIPLE_DEBUG_INIT, NULL, args)
#define lt_info(args...) _lt_info(TRIPLE_DEBUG_INIT, NULL, args)

#ifdef EVOLUX
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>
#include <linux/input.h>

#define DEVINPUT "/dev/input"
#define VIRTUALINPUT "/sys/devices/virtual/input"

static int do_mknod(char *devname, char *i) {
	char name[255];
	int dev = -1;
	// I've no idea how the event device number is actually calculated. Just loop.  --martii

	for (int j = 0; j < 99 && dev < 0; j++) {
		snprintf(name, sizeof(name), VIRTUALINPUT "/%s/event%d/dev", i, j);
		dev = open (name, O_RDONLY);
	}

	if (dev > -1) {
		char buf[255];
		int l = read(dev, buf, sizeof(buf) - 1);
		close(dev);
		if (l > -1) {
			buf[l] = 0;
			int major, minor;
			if (2 == sscanf(buf, "%d:%d", &major, &minor)) {
				snprintf(name, sizeof(name), DEVINPUT "/%s", devname);
				unlink(name);
				mknod(name, 0666 | S_IFCHR, makedev(major, minor));
			}
		}
	}
}

static int create_input_devices (void) {
	DIR *d = opendir (VIRTUALINPUT);
	if (d) {
		struct dirent *e;
		while ((e = readdir(d))) {
			char name[255];
			if (e->d_name[0] == '.')
				continue;
			snprintf(name, sizeof(name), VIRTUALINPUT "/%s/name", e->d_name);
			int n = open(name, O_RDONLY);
			if (n > -1) {
				char buf[255];
				int l = read(n, buf, sizeof(buf) - 1);
				close(n);
				if (l > -1) {
					buf[l] = 0;
					if (!strcmp(buf, "fulan front panel buttons\n"))
						do_mknod("fulan_fp", e->d_name);
					else if (!strcmp(buf, "TDT RC event driver\n"))
						do_mknod("tdt_rc", e->d_name);
					else if (!strcmp(buf, "lircd\n")){
						do_mknod("nevis_ir", e->d_name);
					}
				}
			}
		}
		closedir(d);
	}
	// remove any event* files left that point to our "well-known" inputs
	d = opendir (DEVINPUT);
	if (d) {
		struct dirent *e;
		while ((e = readdir(d))) {
			char name[255];
			if (e->d_name[0] == '.')
				continue;
			if (strncmp(e->d_name, "event", 5))
				continue;
			snprintf(name, sizeof(name), DEVINPUT "/%s", e->d_name);
			int n = open(name, O_RDONLY);
			if (n > -1) {
				char buf[255];
				int l = ioctl(n, EVIOCGNAME(sizeof(buf)), buf);
				close(n);
				if (l > 0) {
					buf[l] = 0;
					if(!strcmp(buf, "fulan front panel buttons")
					|| !strcmp(buf, "TDT RC event driver")
					|| !strcmp(buf, "lircd"))
						unlink(name);
				}
			}
		}
		closedir(d);
	}
}

#endif

static bool initialized = false;

void init_td_api()
{
	if (!initialized)
		lt_debug_init();
	lt_info("%s begin, initialized=%d, debug=0x%02x\n", __FUNCTION__, (int)initialized, debuglevel);
	if (!initialized)
	{
		cCpuFreqManager f;
		f.SetCpuFreq(0);	/* CPUFREQ == 0 is the trigger for leaving standby */
		/* hack: if lircd pidfile is present, don't start input thread */
		if (access("/var/run/lirc/lircd.pid", R_OK))
			start_input_thread();
		else
			lt_info("%s: lircd pidfile present, not starting input thread\n", __func__);
	}
#ifdef EVOLUX
	create_input_devices();
#endif
	initialized = true;
	lt_info("%s end\n", __FUNCTION__);
}

void shutdown_td_api()
{
	lt_info("%s, initialized = %d\n", __FUNCTION__, (int)initialized);
	if (initialized)
		stop_input_thread();
	initialized = false;
}
