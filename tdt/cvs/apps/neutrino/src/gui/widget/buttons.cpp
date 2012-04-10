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

void paintButton(CFrameBuffer * const frameBuffer, const char *button, Font * const font, const std::string &text, const int x, const int y, const unsigned int buttonwidth) {
	paintButton(frameBuffer, button, font, text.c_str(), x, y, buttonwidth);
}

void paintButton(CFrameBuffer * const frameBuffer, const char *button, Font * const font, const char *text, const int _x, const int y, const unsigned int _buttonwidth)
{
	int x = _x + 5;
	int buttonwidth = _buttonwidth - 5;
	uint32_t fgcolor = frameBuffer->realcolor[(((((int)COL_INFOBAR) + 2) | 7) - 2)] | 0xFF000000;
	int height = font->getHeight();
	frameBuffer->paintIcon(button, x, y + height/8, 20, (6 * height)/8, true);
	if (text && strlen(text))
		font->RenderString(x + 20, y + height, buttonwidth - 20, text, COL_INFOBAR, 0, true, fgcolor); // UTF-8
}

void paintButton_Footer(CFrameBuffer * const frameBuffer, const char *button, Font * const font, const std::string &text, const int x, const int y, const unsigned int buttonwidth) {
	paintButton(frameBuffer, button, font, text.c_str(), x, y - font->getHeight(), buttonwidth);
}

void paintButton_Footer(CFrameBuffer * const frameBuffer, const char *button, Font * const font, const char *text, const int x, const int y, const unsigned int buttonwidth)
{
	paintButton(frameBuffer, button, font, text, x, y - font->getHeight(), buttonwidth);
}

void paintButtons(CFrameBuffer * const frameBuffer, Font * const font, const CLocaleManager * const localemanager, const int x, const int y, const unsigned int buttonwidth, const unsigned int count, const struct button_label * const content)
{
	uint32_t fgcolor = frameBuffer->realcolor[(((((int)COL_INFOBAR) + 2) | 7) - 2)] | 0xFF000000;
	bool keep = true;
	int bw[count], sp[count], bw_sum = 0, missing = 0, spare = 0;
	for (int i = 0; i < count; i++) {
		bw[i] = 20 + (content[i].locale == NONEXISTANT_LOCALE) ? 0 : font->getRenderWidth(localemanager->getText(content[i].locale), true);
		sp[i] = bw[i] - buttonwidth;
		if (sp[i] < 0) {
			spare -= sp[i];
			sp[i] = 0;
		} else {
			keep = false;
			bw[i] = buttonwidth;
		}
		bw_sum += bw[i];
		missing += sp[i];
	}
#if 0
	if (keep)
		for (unsigned int i = 0; i < count; i++)
			bw[i] = buttonwidth;
	else
#endif
	if (sp > 0) {
		int spare = count * buttonwidth - bw_sum;
		while (spare > 0 && missing > 0)
			for (int i = 0; (i < count) && (spare > 0); i++)
				if (sp[i] > 0) {
					bw[i]++;
					sp[i]--;
					missing--;
					spare--;
				}
		if (count > 1)
			while (spare > 0)
				for (int i = 0; (i < count - 1) && (spare > 0); i++) {
					bw[i]++;
					spare--;
				}
	} else
		for (int i = 0; i < count; i++) {
			bw[i] *= buttonwidth * count;
			bw[i] /= bw_sum;
		}

	int height = font->getHeight();
	bw_sum = 0;
	for (int i = 0; i < count; i++)
		paintButton(frameBuffer, content[i].button, font, (content[i].locale == NONEXISTANT_LOCALE)
			? NULL : localemanager->getText(content[i].locale),
			x + bw_sum, y, bw[i]);
}
