/*
	Neutrino-GUI  -   DBoxII-Project

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
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gui/widget/listbox.h>
#include <gui/widget/buttons.h>

#include <global.h>
#include <neutrino.h>

#include <gui/widget/icons.h>
#define ROUND_RADIUS 9

CListBox::CListBox(const char * const Caption)
{
	frameBuffer = CFrameBuffer::getInstance();
	caption = Caption;
	liststart = 0;
	selected =  0;
	width =  400;
	height = 420;
	ButtonHeight = 25;
	modified = false;
	theight     = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight();
	fheight     = g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST]->getHeight();
	listmaxshow = (height-theight-0)/fheight;
	height = theight+0+listmaxshow*fheight; // recalc height

	x=frameBuffer->getScreenX() + ((frameBuffer->getScreenWidth() - width) / 2);
	y=frameBuffer->getScreenY() + ((frameBuffer->getScreenHeight() - height) / 2);
}

void CListBox::setModified(void)
{
	modified = true;
}

void CListBox::paint()
{
	liststart = (selected/listmaxshow)*listmaxshow;

	for(unsigned int count=0;count<listmaxshow;count++)
	{
		paintItem(count);
	}

	int ypos = y+ theight;
	int sb = fheight* listmaxshow;
	frameBuffer->paintBoxRel(x+ width- 15,ypos, 15, sb,  COL_MENUCONTENT_PLUS_1);

	int sbc= ((getItemCount()- 1)/ listmaxshow)+ 1;
	float sbh= (sb- 4)/ sbc;
	int sbs= (selected/listmaxshow);

	frameBuffer->paintBoxRel(x+ width- 13, ypos+ 2+ int(sbs* sbh) , 11, int(sbh),  COL_MENUCONTENT_PLUS_3);
}

void CListBox::paintHead()
{
	//frameBuffer->paintBoxRel(x,y, width,theight+0, COL_MENUHEAD_PLUS_0);
	frameBuffer->paintBoxRel(x,y, width,theight+0, COL_MENUHEAD_PLUS_0, ROUND_RADIUS, 1);//round
	g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->RenderString(x+10,y+theight+0, width, caption.c_str() , COL_MENUHEAD, 0, true);
}

void CListBox::paintFoot()
{
	int ButtonWidth = width / 4;
	//frameBuffer->paintBoxRel(x,y+height, width,ButtonHeight, COL_MENUHEAD_PLUS_0);
	frameBuffer->paintBoxRel(x,y+height, width,ButtonHeight, COL_MENUHEAD_PLUS_0, ROUND_RADIUS, 2);//round
	frameBuffer->paintHLine(x, x+width,  y, COL_INFOBAR_SHADOW_PLUS_0);

	::paintButton_Footer(frameBuffer, NEUTRINO_ICON_BUTTON_OKAY, g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL],
		"edit", x+width- 4* ButtonWidth, y+height+24 - 2, width);
	::paintButton_Footer(frameBuffer, NEUTRINO_ICON_BUTTON_GREEN, g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL],
		"add", x+width- 3* ButtonWidth, y+height+24 - 2, width);
	::paintButton_Footer(frameBuffer, NEUTRINO_ICON_BUTTON_RED, g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL],
		"remove", x+width- 2* ButtonWidth, y+height+24 - 2, width);
	::paintButton_Footer(frameBuffer, NEUTRINO_ICON_BUTTON_HOME, g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL],
		"ready", x+width- 1* ButtonWidth, y+height+24 - 2, width);
}

void CListBox::paintItem(int pos)
{
	paintItem(liststart+pos, pos, (liststart+pos==selected) );
}

void CListBox::hide()
{
	frameBuffer->paintBackgroundBoxRel(x,y, width,height+ButtonHeight);
}

unsigned int	CListBox::getItemCount()
{
	return 10;
}

int CListBox::getItemHeight()
{
	return fheight;
}

void CListBox::paintItem(unsigned int itemNr, int paintNr, bool selected)
{
	int ypos = y+ theight + paintNr*getItemHeight();

	uint8_t    color;
	fb_pixel_t bgcolor;
	
	if (selected)
	{
		color   = COL_MENUCONTENTSELECTED;
		bgcolor = COL_MENUCONTENTSELECTED_PLUS_0;
	}
	else
	{
		color   = COL_MENUCONTENT;
		bgcolor = COL_MENUCONTENT_PLUS_0;
	}

	frameBuffer->paintBoxRel(x,ypos, width- 15, getItemHeight(), bgcolor);
	g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST]->RenderString(x + 10, ypos+ fheight, width-20, "demo", color);
}

int CListBox::exec(CMenuTarget* parent, const std::string & actionKey)
{
	neutrino_msg_t      msg;
	neutrino_msg_data_t data;

	int res = menu_return::RETURN_REPAINT;
	selected=0;

	if (parent)
	{
		parent->hide();
	}

	paintHead();
	paint();
	paintFoot();

	bool loop=true;
	modified = false;
	while (loop)
	{
		g_RCInput->getMsg(&msg, &data, g_settings.timing[SNeutrinoSettings::TIMING_EPG]);

		if (( msg == (neutrino_msg_t)g_settings.key_channelList_cancel) ||
				( msg == CRCInput::RC_home))
		{
			loop = false;
		}
		else if (msg == CRCInput::RC_up || (int) msg == g_settings.key_channelList_pageup)
		{
			if(getItemCount()!=0) {
				int step = 0;
				int prev_selected = selected;

				step = ((int) msg == g_settings.key_channelList_pageup) ? listmaxshow : 1;  // browse or step 1
				selected -= step;
				if((prev_selected-step) < 0)            // because of uint
					selected = getItemCount() - 1;

				paintItem(prev_selected - liststart);
				unsigned int oldliststart = liststart;
				liststart = (selected/listmaxshow)*listmaxshow;

				if(oldliststart!=liststart)
					paint();
				else
					paintItem(selected - liststart);
			}
		}
		else if (msg == CRCInput::RC_down || (int) msg == g_settings.key_channelList_pagedown)
		{
			if(getItemCount()!=0) {
				unsigned int step = 0;
				int prev_selected = selected;

				step = ((int) msg == g_settings.key_channelList_pagedown) ? listmaxshow : 1;  // browse or step 1
				selected += step;

				if(selected >= getItemCount()) {
					if (((getItemCount() / listmaxshow) + 1) * listmaxshow == getItemCount() + listmaxshow) // last page has full entries
						selected = 0;
					else
						selected = ((step == listmaxshow) && (selected < (((getItemCount() / listmaxshow) + 1) * listmaxshow))) ? (getItemCount() - 1) : 0;
				}

				paintItem(prev_selected - liststart);
				unsigned int oldliststart = liststart;
				liststart = (selected/listmaxshow)*listmaxshow;
				if(oldliststart!=liststart)
					paint();
				else
					paintItem(selected - liststart);
			}
		}
		else if( msg ==CRCInput::RC_ok)
		{
			onOkKeyPressed();
		}
		else if ( msg ==CRCInput::RC_red)
		{
			onRedKeyPressed();
		}
		else if ( msg ==CRCInput::RC_green)
		{
			onGreenKeyPressed();
		}
		else if ( msg ==CRCInput::RC_yellow)
		{
			onYellowKeyPressed();
		}
		else if ( msg ==CRCInput::RC_blue)
		{
			onBlueKeyPressed();
		}
		else if ((msg ==CRCInput::RC_sat) || (msg == CRCInput::RC_favorites))
		{
		}
		else
		{
			CNeutrinoApp::getInstance()->handleMsg( msg, data );
			// kein canceling...
		}
	}

	hide();
	return res;
}
