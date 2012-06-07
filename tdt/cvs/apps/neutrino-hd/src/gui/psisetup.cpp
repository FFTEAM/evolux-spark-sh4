/*
	(C)2012 by martii

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

#include <gui/psisetup.h>

#include <driver/fontrenderer.h>
#include <driver/rcinput.h>

#include <gui/color.h>

#include <gui/widget/messagebox.h>
#include <widget/icons.h>
#include <driver/screen_max.h>

#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include <global.h>
#include <neutrino.h>

#include <linux/stmfb.h>

#define PSI_SCALE_COUNT 5
static
  CPSISetup::PSI_list
  psi_list[PSI_SCALE_COUNT] = {
#define PSI_CONTRAST 0
  {"/proc/stb/video/plane/psi_contrast", LOCALE_VIDEOMENU_PSI_CONTRAST, true}
#define PSI_SATURATION 1
  , {"/proc/stb/video/plane/psi_saturation", LOCALE_VIDEOMENU_PSI_SATURATION}
#define PSI_BRIGHTNESS 2
  , {"/proc/stb/video/plane/psi_brightness", LOCALE_VIDEOMENU_PSI_BRIGHTNESS}
#define PSI_TINT 3
  , {"/proc/stb/video/plane/psi_tint", LOCALE_VIDEOMENU_PSI_TINT}
#define PSI_RESET 4
  , {"/dev/null", LOCALE_VIDEOMENU_PSI_RESET}
};

#define SLIDERWIDTH 200
#define SLIDERHEIGHT 15
#define LOCGAP 5
#define ROUND_RADIUS 9

CPSISetup::CPSISetup (const neutrino_locale_t Name)
{

  frameBuffer = CFrameBuffer::getInstance ();
  name = Name;
  selected = 0;

  for (int i = 0; i < PSI_RESET; i++)
    psi_list[i].scale = new CProgressBar (true);

  psi_list[PSI_CONTRAST].value = g_settings.psi_contrast;
  psi_list[PSI_SATURATION].value = g_settings.psi_saturation;
  psi_list[PSI_BRIGHTNESS].value = g_settings.psi_brightness;
  psi_list[PSI_TINT].value = g_settings.psi_tint;

  for (int i = 0; i < PSI_RESET; i++)
    writeProcPSI (i);

  needsBlit = true;
}

#if 0
unsigned char
CPSISetup::readProcPSI (int i)
{
// Broken, don't try this, read(2) will block.
  int fn, v = 128;
  if ((i > -1) && (i < PSI_RESET)
      && ((fn = open (psi_list[i].procfilename, O_RDONLY) > -1)))
    {
      char buf[10];
      ssize_t len = read (fn, buf, sizeof (buf));
      close (fn);
      if (len > -1)
	{
	  buf[len] = 0;
	  int v = atoi (buf);
	  if (v & ~0xff)
	    v = 128;
	}
    }
  return (unsigned char) v;
}
#endif

void
CPSISetup::writeProcPSI ()
{
  for (int i = 0; i < PSI_RESET; i++)
    writeProcPSI (i);
}

void
CPSISetup::writeProcPSI (int i)
{
  int fn;
  if (i < 0 || i > PSI_SCALE_COUNT - 2)
	return;

  fn = open (psi_list[i].procfilename, O_WRONLY);
  if (fn > -1)
    {
      char buf[10];
      ssize_t len = snprintf (buf, sizeof (buf), "%d", psi_list[i].value);
      if (len < sizeof(buf))
	if (0 > write (fn, buf, len))
    	  fprintf(stderr, "%s: write(%s): %s\n", __func__, psi_list[i].procfilename, strerror(errno));
		
      close (fn);
    }
  else
    fprintf(stderr, "%s: open(%s): %s\n", __func__, psi_list[i].procfilename, strerror(errno));
}

void CPSISetup::blankScreen() {
  for (int i = 0; i < PSI_RESET; i++) {
	// psi_list[i].value_old = psi_list[i].value;
	psi_list[i].value = 0;
	writeProcPSI (i);
	// psi_list[i].value = psi_list[i].value_old;
  }
}

int
CPSISetup::exec (CMenuTarget * parent, const std::string &)
{
  neutrino_msg_t msg;
  neutrino_msg_data_t data;

  locWidth = 0;
  for (int i = 0; i < PSI_RESET; i++)
    {
      int w = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getRenderWidth (g_Locale->getText (psi_list[i].loc), true) + 3;	// UTF-8
      if (w > locWidth)
	locWidth = w;
    }
  locHeight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight ();
  if (locHeight < SLIDERHEIGHT)
    locHeight = SLIDERHEIGHT + 2;


  sliderOffset = (locHeight - SLIDERHEIGHT) >> 1;

  //            [ SLIDERWIDTH ][5][locwidth    ]
  // [locHeight][XXXXXXXXXXXXX]   [XXXXXXXXXXXX]
  // [locHeight][XXXXXXXXXXXXX]   [XXXXXXXXXXXX]
  // [locHeight][XXXXXXXXXXXXX]   [XXXXXXXXXXXX]
  // [locHeight][XXXXXXXXXXXXX]   [XXXXXXXXXXXX]
  // [locHeight]                  [XXXXXXXXXXXX]

  dx = SLIDERWIDTH + LOCGAP + locWidth;
  dy = PSI_SCALE_COUNT * locHeight + (PSI_SCALE_COUNT - 1) * 2;

  x =
    frameBuffer->getScreenX () + ((frameBuffer->getScreenWidth () - dx) >> 1);
  y =
    frameBuffer->getScreenY () +
    ((frameBuffer->getScreenHeight () - dy) >> 1);

  int res = menu_return::RETURN_REPAINT;
  if (parent)
    parent->hide ();

  for (int i = 0; i < PSI_SCALE_COUNT; i++)
    {
      //psi_list[i].value = psi_list[i].value_old = readProcPSI(i);
      psi_list[i].value = psi_list[i].value_old = 128;
      psi_list[i].x = x;
      psi_list[i].y = y + locHeight * i + i * 2;
      psi_list[i].xBox = psi_list[i].x + SLIDERWIDTH + LOCGAP;
      psi_list[i].yBox = psi_list[i].y;
      psi_list[i].xLoc = psi_list[i].x + SLIDERWIDTH + LOCGAP + 2;
      psi_list[i].yLoc = psi_list[i].y + locHeight - 1;
    }
  //psi_list[PSI_RESET].xLoc = (x - g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getRenderWidth (g_Locale->getText (psi_list[PSI_RESET].loc), true) >> 1);
  psi_list[PSI_RESET].xLoc = x + 20;
  psi_list[PSI_RESET].xBox = x;

  for (int i = 0; i < PSI_RESET; i++)
    psi_list[i].scale->reset ();

  psi_list[PSI_CONTRAST].value = g_settings.psi_contrast;
  psi_list[PSI_SATURATION].value = g_settings.psi_saturation;
  psi_list[PSI_BRIGHTNESS].value = g_settings.psi_brightness;
  psi_list[PSI_TINT].value = g_settings.psi_tint;

  for (int i = 0; i < PSI_RESET; i++)
    psi_list[i].value_old = psi_list[i].value;

  paint();

  unsigned long long timeoutEnd = CRCInput::calcTimeoutEnd(g_settings.timing[SNeutrinoSettings::TIMING_MENU] ? g_settings.timing[SNeutrinoSettings::TIMING_MENU] : 0xffff);
  bool loop = true;
  while (loop)
    {
      if(needsBlit) {
	frameBuffer->blit();
	needsBlit = false;
     }
      g_RCInput->getMsgAbsoluteTimeout(&msg, &data, &timeoutEnd, true);
      if ( msg <= CRCInput::RC_MaxRC )
	timeoutEnd = CRCInput::calcTimeoutEnd(g_settings.timing[SNeutrinoSettings::TIMING_MENU] ? g_settings.timing[SNeutrinoSettings::TIMING_MENU] : 0xffff);
      switch (msg)
	{
	case CRCInput::RC_down:
	  if (selected < PSI_RESET)
	    {
	      psi_list[selected].selected = false;
	      paintSlider (selected);
	      selected++;
	      psi_list[selected].selected = true;
	      paintSlider (selected);
	    }
	  break;
	case CRCInput::RC_up:
	  if (selected > 0)
	    {
	      psi_list[selected].selected = false;
	      paintSlider (selected);
	      selected--;
	      psi_list[selected].selected = true;
	      paintSlider (selected);
	    }
	  break;
	case CRCInput::RC_right:
	  if (selected < PSI_RESET && psi_list[selected].value < 255)
	    {
	      int val = psi_list[selected].value + g_settings.psi_step;
	      psi_list[selected].value = (val > 255) ? 255 : val;
	      paintSlider (selected);
	      writeProcPSI (selected);
	    }
	  break;
	case CRCInput::RC_left:
	  if (selected < PSI_RESET && psi_list[selected].value > 0)
	    {
	      int val = psi_list[selected].value - g_settings.psi_step;
	      psi_list[selected].value = (val < 0) ? 0 : val;
	      paintSlider (selected);
	      writeProcPSI (selected);
	    }
	  break;
	case CRCInput::RC_home:	// exit -> revert changes
	  int i;
	  for (i = 0;
	       (i < PSI_RESET)
	       && (psi_list[i].value == psi_list[i].value_old); i++);

	  if (i < PSI_RESET)
	    if (ShowLocalizedMessage
		(name, LOCALE_MESSAGEBOX_ACCEPT, CMessageBox::mbrYes,
		 CMessageBox::mbYes | CMessageBox::mbCancel) ==
		CMessageBox::mbrCancel)
	      for (i = 0; i < PSI_RESET; i++)
		{
		  psi_list[i].value = psi_list[i].value_old;
		  writeProcPSI (i);
		}
	case CRCInput::RC_ok:
          loop = false;
	  if (selected != PSI_RESET)
	    {
	      g_settings.psi_contrast = psi_list[PSI_CONTRAST].value;
	      g_settings.psi_saturation = psi_list[PSI_SATURATION].value;
	      g_settings.psi_brightness = psi_list[PSI_BRIGHTNESS].value;
	      g_settings.psi_tint = psi_list[PSI_TINT].value;
	      break;
	    }
	case CRCInput::RC_red:
	  for (int i = 0; i < PSI_RESET; i++)
	    {
	      psi_list[i].value = 128;
	      writeProcPSI (i);
	    }
	  for (i = 0; i < PSI_RESET; i++)
	    paintSlider (i);
	  break;
	default:
	  ;
	}
    }

  hide ();

  return res;
}

void
CPSISetup::hide ()
{
  frameBuffer->paintBackgroundBoxRel (x, y, dx, dy);
  frameBuffer->blit();
}

void
CPSISetup::paint ()
{
  for (int i = 0; i < PSI_SCALE_COUNT; i++)
    paintSlider (i);
}

void
CPSISetup::paintSlider (int i)	// UTF-8
{
  needsBlit = true;
  int fg_col = frameBuffer->realcolor[(((((int)psi_list[i].selected ? COL_MENUHEAD : COL_MENUCONTENT) + 2) | 7) - 2)];

  if (i < PSI_RESET)
    {
      psi_list[i].scale->paintProgressBar (psi_list[i].x, psi_list[i].y + sliderOffset, SLIDERWIDTH, SLIDERHEIGHT, psi_list[i].value, 255, 0, 0, COL_INFOBAR_SHADOW_PLUS_1, 0, "", 0);
      g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->RenderString (psi_list[i].xLoc, psi_list[i].yLoc, locWidth, g_Locale->getText(psi_list[i].loc), 0, 0, true, fg_col);
    }
  else
    {
      int fh = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight();
      g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->RenderString (psi_list[i].x + 2 + fh + fh/8, psi_list[i].yLoc, dx - 2 - fh, g_Locale->getText(psi_list[i].loc), 0, 0, true, fg_col);
      frameBuffer->paintIcon (NEUTRINO_ICON_BUTTON_RED, psi_list[i].x + 2, psi_list[i].yLoc - fh + fh/8, 0, (6 * fh)/8);
    }
}

CPSISetupNotifier::CPSISetupNotifier (class CPSISetup *p) {
	psisetup = p;
}

bool
CPSISetupNotifier::changeNotify (const neutrino_locale_t OptionName, void *Data)
{
  for (int i = 0; i < PSI_RESET; i++)
    if (OptionName == psi_list[i].loc)
      {
	psi_list[i].value = *((int *) Data);
	psisetup->writeProcPSI (i);
	return true;
      }
  return false;
}
