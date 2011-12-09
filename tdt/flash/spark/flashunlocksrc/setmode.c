// setmtdmode.c
//
// changes write protection of /dev/mtd* devices on SPARK.
//
// Compile:
// ~/tdt/tufsbox/devkit/sh4/sh4-linux/bin/gcc -Xlinker -s -Wall -o setmtdmode setmtdmode.o
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

extern char *optarg;
extern int optind;

static void usage(char *p)
{
    fprintf(stderr,
	    "Usage: %s [ -q ] [ -l *-u ] [ number ]\n\n"
	    "Options:\n"
	    "-q	quiet mode\n"
	    "-l	lock mtd device (set to read-only)\n"
	    "-u	unlock mtd device (set to read-write), this is the default\n"
	    "number	number of mtd device, defaults to 0 (/dev/mtd0)\n\n"
	    "An exit status of zero indicates that locking/unlocking succeeded.\n",
	    p);
}

int main(int argc, char **argv)
{
#define FULAN_MTD_SET_RW 0x40044d00
#define FULAN_MTD_SET_RO 0x40044dff
    static unsigned long r = FULAN_MTD_SET_RW;
    char *devformat = "/dev/mtd%s", *devnum = "0", *un = "un";
    char ch, *devname, *progname = argv[0];
    int d, silent = 0;
    size_t devlen;

    while ((ch = getopt(argc, argv, "qlu")) != -1) {
	switch (ch) {
	case 'u':
	    r = FULAN_MTD_SET_RW, un = "un";
	    break;
	case 'l':
	    r = FULAN_MTD_SET_RO, un = "";
	    break;
	case 'q':
	    silent++;
	    break;
	default:
	    usage(progname);
	}
    }
    argc -= optind, argv += optind;

    if (*argv)
	devnum = *argv++;

    if (*argv)
	usage(progname);

    devname = alloca(devlen = strlen(devformat) + strlen(devnum));
    snprintf(devname, devlen, devformat, devnum);

    if (!silent)
	fprintf(stderr, "%slocking %s\n", un, devname);

    d = open("/dev/mtd_rwmode_set", O_RDWR);
    if (d < 0) {
	if (!silent)
	    perror("open");
	exit(-1);
    }
    if (-1 == ioctl(d, r, devname)) {
	if (!silent)
	    perror("ioctl");
	exit(-1);
    }

    close(d);
    exit(0);
}


