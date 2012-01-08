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

#include <global.h>
#include <neutrino.h>

#define PSI_SCALE_COUNT 4
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
};

#define SLIDERWIDTH 200
#define SLIDERHEIGHT 15
#define LOCGAP 5
#define SLIDER_DELTA 0x04
#define ROUND_RADIUS 9

CPSISetup::CPSISetup (const neutrino_locale_t Name)
{

  frameBuffer = CFrameBuffer::getInstance ();
  name = Name;
  selected = 0;

  for (int i = 0; i < PSI_SCALE_COUNT; i++)
    psi_list[i].scale =
      new CScale (SLIDERWIDTH, SLIDERHEIGHT, 50, 100, 80, true);

  psi_list[PSI_CONTRAST].value = g_settings.psi_contrast;
  psi_list[PSI_SATURATION].value = g_settings.psi_saturation;
  psi_list[PSI_BRIGHTNESS].value = g_settings.psi_brightness;
  psi_list[PSI_TINT].value = g_settings.psi_tint;

  for (int i = 0; i < PSI_SCALE_COUNT; i++)
    writeProcPSI (i);
}

#if 0
unsigned char
CPSISetup::readProcPSI (int i)
{
// Broken, don't try this, read(2) will block.
  int fn, v = 128;
  if ((i > -1) && (i < PSI_SCALE_COUNT)
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
CPSISetup::writeProcPSI (int i)
{
  int fn;
  if ((i > -1) && (i < PSI_SCALE_COUNT)
      && ((fn = open (psi_list[i].procfilename, O_WRONLY) > -1)))
    {
      char buf[10];
      ssize_t len = snprintf (buf, sizeof (buf), "%d", psi_list[i].value);
      if (len > 0)
	write (fn, buf, len);
      close (fn);
    }
}

int
CPSISetup::exec (CMenuTarget * parent, const std::string &)
{
  neutrino_msg_t msg;
  neutrino_msg_data_t data;

  fb_pixel_t *pixbuf = NULL;

  // [ SLIDERWIDTH ][5][2 + width + 2 ]

  locWidth = 0;
  for (int i = 0; i < PSI_SCALE_COUNT; i++)
    {
      int w = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getRenderWidth (g_Locale->getText (psi_list[i].loc), true) + 3;	// UTF-8
      if (w > locWidth)
	locWidth = w;
    }
  locHeight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight ();
  if (locHeight < SLIDERHEIGHT)
    locHeight = SLIDERHEIGHT + 2;


  sliderOffset = (locHeight - SLIDERHEIGHT) >> 1;

  //            [ SLIDERWIDTH ][5][lowidth     ]
  // [locHeight][XXXXXXXXXXXXX]   [XXXXXXXXXXXX]
  // [locHeight][XXXXXXXXXXXXX]   [XXXXXXXXXXXX]
  // [locHeight][XXXXXXXXXXXXX]   [XXXXXXXXXXXX]
  // [locHeight][XXXXXXXXXXXXX]   [XXXXXXXXXXXX]

  dx = SLIDERWIDTH + LOCGAP + locWidth;
  dy = PSI_SCALE_COUNT * locHeight + (PSI_SCALE_COUNT - 1) * 2;

#ifdef __sh__
  int checkSize;
  pixbuf = frameBuffer->allocPixelBuffer (dx, dy);
  if (pixbuf != NULL)
    checkSize = frameBuffer->SaveScreen (x, y, dx, dy, pixbuf);
#else
  pixbuf = new fb_pixel_t[dx * dy];
  if (pixbuf != NULL)
    frameBuffer->SaveScreen (x, y, dx, dy, pixbuf);
#endif

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
      psi_list[i].scale->reset ();
      psi_list[i].x = x;
      psi_list[i].y = y + locHeight * i + i * 2;
      psi_list[i].xBox = psi_list[i].x + SLIDERWIDTH + LOCGAP;
      psi_list[i].yBox = psi_list[i].y;
      psi_list[i].xLoc = psi_list[i].x + SLIDERWIDTH + LOCGAP + 2;
      psi_list[i].yLoc = psi_list[i].y + locHeight - 1;
    }

  psi_list[PSI_CONTRAST].value = g_settings.psi_contrast;
  psi_list[PSI_SATURATION].value = g_settings.psi_saturation;
  psi_list[PSI_BRIGHTNESS].value = g_settings.psi_brightness;
  psi_list[PSI_TINT].value = g_settings.psi_tint;

  for (int i = 0; i < PSI_SCALE_COUNT; i++)
    psi_list[i].value_old = psi_list[i].value;

  paint ();

  bool loop = true;
  while (loop)
    {
      g_RCInput->getMsg (&msg, &data, 100);
      switch (msg)
	{
	case CRCInput::RC_down:
	  if (selected < PSI_SCALE_COUNT - 1)
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
	  if (psi_list[selected].value < 255)
	    {
	      int val = psi_list[selected].value + SLIDER_DELTA;
	      psi_list[selected].value = (val > 255) ? 255 : val;
	      paintSlider (selected);
	      writeProcPSI (selected);
	    }
	  break;
	case CRCInput::RC_left:
	  if (psi_list[selected].value > 0)
	    {
	      int val = psi_list[selected].value - SLIDER_DELTA;
	      psi_list[selected].value = (val < 0) ? 0 : val;
	      paintSlider (selected);
	      writeProcPSI (selected);
	    }
	  break;
	case CRCInput::RC_home:	// exit -> revert changes
	  int i;
	  for (i = 0;
	       (i < PSI_SCALE_COUNT)
	       && (psi_list[i].value == psi_list[i].value_old); i++);

	  if (i < PSI_SCALE_COUNT)
	    if (ShowLocalizedMessage
		(name, LOCALE_MESSAGEBOX_ACCEPT, CMessageBox::mbrYes,
		 CMessageBox::mbYes | CMessageBox::mbCancel) ==
		CMessageBox::mbrCancel)
	      for (i = 0; i < PSI_SCALE_COUNT; i++)
		{
		  psi_list[i].value = psi_list[i].value_old;
		  writeProcPSI (i);

		}
	case CRCInput::RC_ok:
	  g_settings.psi_contrast = psi_list[PSI_CONTRAST].value;
	  g_settings.psi_saturation = psi_list[PSI_SATURATION].value;
	  g_settings.psi_brightness = psi_list[PSI_BRIGHTNESS].value;
	  g_settings.psi_tint = psi_list[PSI_TINT].value;
	  loop = false;
	  break;
	default:
	  ;
	}
    }

  hide ();

  if (pixbuf)
    {
#ifdef __sh__
      frameBuffer->RestoreScreen (x, y, dx, dy, pixbuf, checkSize);
#else
      frameBuffer->RestoreScreen (x, y, dx, dy, pixbuf);
#endif
      delete[]pixbuf;
    }

  return res;
}

void
CPSISetup::hide ()
{
  frameBuffer->paintBackgroundBoxRel (x, y, dx, dy);
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
  psi_list[i].scale->paint (psi_list[i].x, psi_list[i].y + sliderOffset,
			    (psi_list[i].value * 100) >> 8);
  int col_bg =
    psi_list[i].selected ? COL_MENUCONTENTSELECTED_PLUS_0 :
    COL_MENUCONTENT_PLUS_0;
  int col_fg =
    psi_list[i].selected ? COL_MENUCONTENTSELECTED : COL_MENUCONTENT;
  frameBuffer->paintBoxRel (psi_list[i].xBox, psi_list[i].yBox, locWidth,
			    locHeight, col_bg, ROUND_RADIUS, 3);
  g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->RenderString (psi_list[i].xLoc,
							   psi_list[i].yLoc,
							   locWidth,
							   g_Locale->getText
							   (psi_list[i].loc),
							   col_fg, 0, true);
}
