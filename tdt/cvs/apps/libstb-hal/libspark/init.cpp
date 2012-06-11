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
#include <time.h>
#include <poll.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <string.h>
#include <signal.h>

#define VIRTUALINPUT "/sys/devices/virtual/input"
#define DEVINPUT "/dev/input"

typedef struct {
	const char *name;
	const char *desc;
	int fd;
	time_t discovery;
} input_device_t;

static input_device_t input_device[] = {
	{ "/dev/input/nevis_ir",	"lircd",			-1, 0 },
	{ "/dev/input/tdt_rc",		"TDT RC event driver",		-1, 0 },
	{ "/dev/input/fulan_fp",	"fulan front panel buttons",	-1, 0 },
	{ "/dev/input/event0",		NULL,				-1, 0 },
	{ "/dev/input/event1",		NULL,				-1, 0 },
	{ "/dev/input/event2",		NULL,				-1, 0 },
	{ "/dev/input/event3",		NULL,				-1, 0 },
	{ "/dev/input/event4",		NULL,				-1, 0 },
	{ "/dev/input/event5",		NULL,				-1, 0 },
	{ "/dev/input/event6",		NULL,				-1, 0 },
	{ "/dev/input/event7",		NULL,				-1, 0 },
	{ NULL,				NULL,				-1, 0 }
};

static int number_of_input_devices = 0;

static int do_mknod(const char *devname, char *i) {
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
				mknod(devname, 0666 | S_IFCHR, makedev(major, minor));
			}
		}
	}
}

static void create_input_devices (void) {
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
				if (l > 1) {
					do
						buf[l--] = 0;
					while (l > 1 && buf[l] == '\n');

					for (int i = 0; i < number_of_input_devices; i++)
						if (input_device[i].desc && !strcmp(buf, input_device[i].desc)) {
							do_mknod(input_device[i].name, e->d_name);
							break;
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
				if (l > 1) {
					do
						buf[l--] = 0;
					while (l > 1 && buf[l] == '\n');

					for (int i = 0; i < number_of_input_devices; i++)
						if (input_device[i].desc && !strcmp(buf, input_device[i].desc)) {
							unlink(name);
							break;
						}
					}
				}
			}
		closedir(d);
	}
}

static pthread_t inmux_task;
static int inmux_thread_running = 0;

static void count_input_devices(void) {
	input_device_t *i = input_device;
	while (i->name)
		i++, number_of_input_devices++;
}

static void open_input_devices(void) {
	time_t now = time(NULL);
	for (int i = 0; i < number_of_input_devices; i++)
		if ((input_device[i].fd < 0) && (input_device[i].discovery + 60 < now)) {
			input_device[i].discovery = now + 60;
			input_device[i].fd = open(input_device[i].name, O_RDWR | O_NONBLOCK);
		}
}

static void close_input_devices(void) {
	for (int i = 0; i < number_of_input_devices; i++)
		if (input_device[i].fd > -1) {
			close(input_device[i].fd);
			input_device[i].fd = -1;
		}
}

static void poll_input_devices(void) {
	struct pollfd fds[number_of_input_devices];
	input_device_t inputs[number_of_input_devices];
	int nfds = 0;
	for (int i = 1; i < number_of_input_devices; i++)
		if (input_device[i].fd > -1) {
			fds[nfds].fd = input_device[i].fd;
			fds[nfds].events = POLLIN | POLLHUP | POLLERR;
			fds[nfds].revents = 0;
			inputs[nfds] = input_device[i];
			nfds++;
		}

	if (nfds == 0) {
		// Only a single input device, which happens to be our master. poll() to avoid looping too fast.
		fds[0].fd = input_device[0].fd;
		fds[0].events = POLLIN | POLLHUP | POLLERR;
		fds[0].revents = 0;
		poll(fds, 1, 60000 /* ms */);
		return;
	}

	int r = poll(fds, nfds, 60000 /* ms */);
	if (r < 0) {
		if (errno != EAGAIN)
			inmux_thread_running = 0;
		return;
	}
	for (int i = 0; i < nfds && r > 0; i++) {
		if (fds[i].revents & POLLIN) {
//fprintf(stderr, "### input from fd %d (%s)\n", fds[i].fd, inputs[i].name);
			struct input_event ev;
			while (sizeof(ev) == read(fds[i].fd, &ev, sizeof(ev)))
				write(input_device[0].fd, &ev, sizeof(ev));
			r--;
		} else if (fds[i].revents & (POLLHUP | POLLERR | POLLNVAL)) {
//fprintf(stderr, "### error on %d (%s)\n", fds[i].fd, inputs[i].name);
			close (fds[i].fd);
			fds[i].fd = -1;
			r--;
		}
	}
}

static void *inmux_thread(void *)
{
	char threadname[17];
	strncpy(threadname, __func__, sizeof(threadname));
	threadname[16] = 0;
	prctl (PR_SET_NAME, (unsigned long)&threadname);

	inmux_thread_running = 1;
	while (inmux_thread_running) {
		open_input_devices();
		poll_input_devices();
	}

	return NULL;
}

void start_inmux_thread(void)
{
	input_device[0].fd = open(input_device[0].name, O_RDWR | O_NONBLOCK); // nevis_ir. This is mandatory.
	if (input_device[0].fd < 0){
		lt_info("%s: open(%s): %m\n", __func__, input_device[0].name);
		return;
	}
	if (pthread_create(&inmux_task, 0, inmux_thread, NULL) != 0)
	{
		lt_info("%s: inmux thread pthread_create: %m\n", __func__);
		inmux_thread_running = 0;
		return;
	}
	while (!inmux_thread_running)
		usleep(1000);
	if (inmux_thread_running == 2)
		inmux_thread_running = 0;
}

void stop_inmux_thread(void)
{
	if (! inmux_thread_running)
		return;
	inmux_thread_running = 0;
	pthread_kill(inmux_task, SIGKILL);
	close_input_devices();
	pthread_join(inmux_task, NULL);
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
#ifdef EVOLUX
		count_input_devices();
		create_input_devices();
		start_inmux_thread();
#endif
	}
	initialized = true;
	lt_info("%s end\n", __FUNCTION__);
}

void shutdown_td_api()
{
	lt_info("%s, initialized = %d\n", __FUNCTION__, (int)initialized);
#ifdef EVOLUX
	if (initialized) {
		stop_input_thread();
		stop_inmux_thread();
	}
#else
	if (initialized)
		stop_input_thread();
#endif
	initialized = false;
}
