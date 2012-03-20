/*
 * GraphLCD driver library
 *
 * port.c  -  parallel port class with low level routines
 *
 * This file is released under the GNU General Public License. Refer
 * to the COPYING file distributed with this package.
 *
 * (c) 2004 Andreas Regel <andreas.regel AT powarman.de>
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <termios.h>
#ifndef __sh__
#include <sys/io.h>
#endif
#include <sys/ioctl.h>
#include <linux/ppdev.h>
#include <linux/parport.h>

#ifdef __sh__

/* konfetti: those are currently only dummy. not tested, not really
 * thinking on it. it needs revise for the params etc. also we need
 * to revise the assembler code below. but for now its ok because
 * I (we) only want the peal lcd to be running ;)
 */

#define __NR_ioperm 101
#define __NR_iopl 110

static inline int ioperm(int port, unsigned long int from, unsigned long int num)
{
	return syscall(__NR_ioperm, port, from, num);
}

static inline int iopl(int level)
{
	return syscall(__NR_iopl, level);
}

#endif

#include "port.h"

namespace GLCD
{

static inline int port_in(int port)
{
    unsigned char value;
#ifndef __sh__
    __asm__ volatile ("inb %1,%0"
                      : "=a" (value)
                      : "d" ((unsigned short) port));
#endif
    return value;
}

static inline void port_out(unsigned short int port, unsigned char val)
{
#ifndef __sh__
    __asm__ volatile ("outb %0,%1\n"
                      :
                      : "a" (val), "d" (port));
#endif
}

cParallelPort::cParallelPort()
:   fd(-1),
    port(0),
    usePPDev(false)
{
}

cParallelPort::~cParallelPort()
{
}

int cParallelPort::Open(int portIO)
{
    usePPDev = false;
    port = portIO;

    if (port < 0x400)
    {
        if (ioperm(port, 3, 255) == -1)
        {
            syslog(LOG_ERR, "glcd drivers: ERROR ioperm(0x%X) failed! Err:%s (cParallelPort::Open)\n",
                   port, strerror(errno));
            return -1;
        }
    }
    else
    {
        if (iopl(3) == -1)
        {
            syslog(LOG_ERR, "glcd drivers: ERROR iopl failed! Err:%s (cParallelPort::Init)\n",
                   strerror(errno));
            return -1;
        }
    }
    return 0;
}

int cParallelPort::Open(const char * device)
{
    usePPDev = true;

    fd = open(device, O_RDWR);
    if (fd == -1)
    {
        syslog(LOG_ERR, "glcd drivers: ERROR cannot open %s. Err:%s (cParallelPort::Init)\n",
               device, strerror(errno));
        return -1;
    }

    if (ioctl(fd, PPCLAIM, NULL) == -1)
    {
        syslog(LOG_ERR, "glcd drivers: ERROR cannot claim %s. Err:%s (cParallelPort::Init)\n",
               device, strerror(errno));
        close(fd);
        return -1;
    }

    int mode = PARPORT_MODE_PCSPP;
    if (ioctl(fd, PPSETMODE, &mode) == -1)
    {
        syslog(LOG_ERR, "glcd drivers: ERROR cannot setmode %s. Err:%s (cParallelPort::Init)\n",
               device, strerror(errno));
        close(fd);
        return -1;
    }

    return 0;
}

int cParallelPort::Close()
{
    if (usePPDev)
    {
        if (fd != -1)
        {
            ioctl(fd, PPRELEASE);
            close(fd);
            fd = -1;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        if (port < 0x400)
        {
            if (ioperm(port, 3, 0) == -1)
            {
                return -1;
            }
        }
        else
        {
            if (iopl(0) == -1)
            {
                return -1;
            }
        }
    }
    return 0;
}

void cParallelPort::Claim()
{
    if (usePPDev)
        ioctl(fd, PPCLAIM);
}

void cParallelPort::Release()
{
    if (usePPDev)
        ioctl(fd, PPRELEASE);
}

void cParallelPort::SetDirection(int direction)
{
    if (usePPDev)
    {
        if (ioctl(fd, PPDATADIR, &direction) == -1)
        {
            perror("ioctl(PPDATADIR)");
            //exit(1);
        }
    }
    else
    {
        if (direction == kForward)
            port_out(port + 2, port_in(port + 2) & 0xdf);
        else
            port_out(port + 2, port_in(port + 2) | 0x20);
    }
}

unsigned char cParallelPort::ReadControl()
{
    unsigned char value;

    if (usePPDev)
    {
        if (ioctl(fd, PPRCONTROL, &value) == -1)
        {
            perror("ioctl(PPRCONTROL)");
            //exit(1);
        }
    }
    else
    {
        value = port_in(port + 2);
    }

    return value;
}

void cParallelPort::WriteControl(unsigned char value)
{
    if (usePPDev)
    {
        if (ioctl(fd, PPWCONTROL, &value) == -1)
        {
            perror("ioctl(PPWCONTROL)");
            //exit(1);
        }
    }
    else
    {
        port_out(port + 2, value);
    }
}

unsigned char cParallelPort::ReadStatus()
{
    unsigned char value;

    if (usePPDev)
    {
        if (ioctl(fd, PPRSTATUS, &value) == -1)
        {
            perror("ioctl(PPRSTATUS)");
            //exit(1);
        }
    }
    else
    {
        value = port_in(port + 1);
    }

    return value;
}

unsigned char cParallelPort::ReadData()
{
    unsigned char data;

    if (usePPDev)
    {
        if (ioctl(fd, PPRDATA, &data) == -1)
        {
            perror("ioctl(PPRDATA)");
            //exit(1);
        }
    }
    else
    {
        data = port_in(port);
    }

    return data;
}

void cParallelPort::WriteData(unsigned char data)
{
    if (usePPDev)
    {
        if (ioctl(fd, PPWDATA, &data) == -1)
        {
            perror("ioctl(PPWDATA)");
            //exit(1);
        }
    }
    else
    {
        port_out(port, data);
    }
}



cSerialPort::cSerialPort()
:   fd(-1)
{
}

cSerialPort::~cSerialPort()
{
}

int cSerialPort::Open(const char * device)
{
    struct termios options;

    fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)
    {
        printf("error opening port\n");
        return -1;
    }
    //fcntl(fd, F_SETFL, FNDELAY);
    fcntl(fd, F_SETFL, 0);

    tcgetattr(fd, &options);

    cfsetispeed(&options, B921600);
    cfsetospeed(&options, B921600);

    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    options.c_cflag &= ~CRTSCTS;

    options.c_cflag |= (CLOCAL | CREAD);

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    options.c_iflag &= ~(IXON | IXOFF | IXANY);

    options.c_oflag &= ~OPOST;

    tcsetattr(fd, TCSANOW, &options);

    return 0;
}

int cSerialPort::Close()
{
    if (fd == -1)
        return -1;
    close(fd);
    return 0;
}

int cSerialPort::ReadData(unsigned char * data)
{
    if (fd == -1)
        return 0;
    return read(fd, data, 1);
}

void cSerialPort::WriteData(unsigned char data)
{
    WriteData(&data, 1);
}

void cSerialPort::WriteData(unsigned char * data, unsigned short length)
{
    if (fd == -1)
        return;
    write(fd, data, length);
}

} // end of namespace
