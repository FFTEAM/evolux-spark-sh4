#ifndef __gui_widget_buttons_h__
#define __gui_widget_buttons_h__

/*
 * $Header: /cvs/tuxbox/apps/tuxbox/neutrino/src/gui/widget/buttons.h,v 1.2 2004/05/25 07:44:16 thegoodguy Exp $
 *
 * (C) 2003 by thegoodguy <thegoodguy@berlios.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <driver/fontrenderer.h>
#include <driver/framebuffer.h>
#include <system/localize.h>
#include <string>

typedef struct button_label
{
	const char *      button;
	neutrino_locale_t locale;
	const char *text;
} button_label_struct;

void paintButtons(CFrameBuffer * const frameBuffer, Font * const font, const int x, const int y, const unsigned int buttonwidth, const unsigned int count, const struct button_label * const content);

void paintButton(CFrameBuffer * const frameBuffer, const char *button, Font * const font, const std::string &text, const int x, const int y, const unsigned int buttonwidth);
void paintButton(CFrameBuffer * const frameBuffer, const char *button, Font * const font, const char *text, const int x, const int y, const unsigned int buttonwidth);
void paintButton_Footer(CFrameBuffer * const frameBuffer, const char *button, Font * const font, const std::string &text, const int x, const int y, const unsigned int buttonwidth);
void paintButton_Footer(CFrameBuffer * const frameBuffer, const char *button, Font * const font, const char *text, const int x, const int y, const unsigned int buttonwidth);

#endif /* __gui_widget_buttons_h__ */
