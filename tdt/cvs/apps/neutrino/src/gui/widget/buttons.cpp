/*
 * $Header: /cvs/tuxbox/apps/tuxbox/neutrino/src/gui/widget/buttons.cpp,v 1.2 2004/03/14 22:20:05 thegoodguy Exp $
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gui/widget/buttons.h>

#include <gui/color.h>

void paintButtons(CFrameBuffer * const frameBuffer, Font * const font, const CLocaleManager * const localemanager, const int x, const int y, const unsigned int buttonwidth, const unsigned int count, const struct button_label * const content)
{
#if 1
	bool keep = true;
	unsigned int bw[count], bw_sum = 0;
	for (unsigned int i = 0; i < count; i++) {
		bw[i] = font->getRenderWidth(localemanager->getText(content[i].locale), true);
		bw_sum += bw[i];
		if (bw[i] > buttonwidth)
			keep = false;
	}
	if (keep)
		for (unsigned int i = 0; i < count; i++)
			bw[i] = buttonwidth;
	else
		for (unsigned int i = 0; i < count; i++) {
			bw[i] *= buttonwidth * count;
			bw[i] /= bw_sum;
		}

	bw_sum = 0;
	for (unsigned int i = 0; i < count; i++) {
		frameBuffer->paintIcon(content[i].button, x + bw_sum, y);
		font->RenderString(x + bw_sum + 20, y + 19, bw[i] - 20, localemanager->getText(content[i].locale), COL_INFOBAR, 0, true); // UTF-8
		bw_sum += bw[i];
	}

#else
	for (unsigned int i = 0; i < count; i++)
	{
		frameBuffer->paintIcon(content[i].button, x + i * buttonwidth, y);
		font->RenderString(x + i * buttonwidth + 20, y + 19, buttonwidth - 20, localemanager->getText(content[i].locale), COL_INFOBAR, 0, true); // UTF-8
	}
#endif
}
