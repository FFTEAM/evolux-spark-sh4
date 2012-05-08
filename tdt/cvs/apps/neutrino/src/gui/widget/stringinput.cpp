/*
	Neutrino-GUI  -   DBoxII-Project

	Copyright (C) 2001 Steffen Hehn 'McClean'
	Homepage: http://dbox.cyberphoria.org/

	Kommentar:

	Diese GUI wurde von Grund auf neu programmiert und sollte nun vom
	Aufbau und auch den Ausbaumoeglichkeiten gut aussehen. Neutrino basiert
	auf der Client-Server Idee, diese GUI ist also von der direkten DBox-
	Steuerung getrennt. Diese wird dann von Daemons uebernommen.


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

#include <gui/widget/stringinput.h>

#include <driver/fontrenderer.h>
#include <driver/rcinput.h>
#include <driver/screen_max.h>

#include <gui/color.h>

#include <gui/widget/buttons.h>
#include <gui/widget/icons.h>
#include <gui/widget/messagebox.h>

#include <global.h>
#include <neutrino.h>

#define ROUND_RADIUS 9

CStringInput::CStringInput(const neutrino_locale_t Name, char* Value, int Size, const neutrino_locale_t Hint_1, const neutrino_locale_t Hint_2, const char * const Valid_Chars, CChangeObserver* Observ, const char * const Icon)
{
	frameBuffer = CFrameBuffer::getInstance();
	name =  Name;
	head = NULL;
	value = Value;
	valueString = NULL;
	size =  Size;

	hint_1 = Hint_1;
	hint_2 = Hint_2;
	validchars = Valid_Chars;
	iconfile = Icon ? Icon : "";

	observ = Observ;
	init();
}

CStringInput::CStringInput(const neutrino_locale_t Name, std::string* Value, int Size, const neutrino_locale_t Hint_1, const neutrino_locale_t Hint_2, const char * const Valid_Chars, CChangeObserver* Observ, const char * const Icon)
{
        frameBuffer = CFrameBuffer::getInstance();
        name =  Name;
	head = NULL;
        value = new char[Size+1];
        strncpy(value,Value->c_str(),Size + 1);
        valueString = Value;
        size = Size;

        hint_1 = Hint_1;
        hint_2 = Hint_2;
        validchars = Valid_Chars;
        iconfile = Icon ? Icon : "";

        observ = Observ;
        init();
}

CStringInput::CStringInput(char * Head, char* Value, int Size, const neutrino_locale_t Hint_1, const neutrino_locale_t Hint_2, const char * const Valid_Chars, CChangeObserver* Observ, const char * const Icon)
{
        frameBuffer = CFrameBuffer::getInstance();
        head = strdup(Head);
        value = Value;
        valueString = NULL;
        size =  Size;

        hint_1 = Hint_1;
        hint_2 = Hint_2;
        validchars = Valid_Chars;
        iconfile = Icon ? Icon : "";

        observ = Observ;
        init();
}

CStringInput::~CStringInput() 
{
	if (valueString != NULL) 
	{
		delete[] value;
	}
	if(head) {
		free(head);
	}
}

void CStringInput::init() 
{
	width = (size*20)+40;

	if (width<420)
		width = 420;

	int neededWidth;
	if(head) {
		neededWidth = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getRenderWidth(head, true); // UTF-8
	}
	else
		neededWidth = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getRenderWidth(g_Locale->getText(name), true); // UTF-8

	if (!(iconfile.empty()))
		neededWidth += 28;
	if (neededWidth+20> width)
		width = neededWidth+20;

	hheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight();
	mheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getHeight();
	iheight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_INFO]->getHeight();

	height = hheight+ mheight+ 50;
	if (hint_1 != NONEXISTANT_LOCALE)
	{
		height += iheight;
		if (hint_2 != NONEXISTANT_LOCALE)
			height += iheight;
	}

	x = frameBuffer->getScreenX() + ((frameBuffer->getScreenWidth() - width)>>1);
	y = frameBuffer->getScreenY() + ((frameBuffer->getScreenHeight()-height)>>1);
	selected = 0;
}

void CStringInput::NormalKeyPressed(const neutrino_msg_t key)
{
	if (CRCInput::isNumeric(key))
	{
		if (!value[selected] && (len < size))
			len++;
		value[selected] = validchars[CRCInput::getNumericValue(key)];

		if (value[selected] && (selected + 1 < size))
		{
			selected++;
			paintChar(selected - 1);
		}
		
		paintChar(selected);
	}
}

void CStringInput::valueDeletePrev()	// delete value[selected - 1], decrement selected
{
	if (selected > 0)
	{
		selected--;
		for (int i = selected; i < len; i++)
			value[i] = value[i + 1];
		len--;
		for (int i = selected; i < len; i++)
			paintChar(i);
	}
}

void CStringInput::keyBackspacePressed(void)
{
	valueDeletePrev();
}

void CStringInput::keyGreenPressed(void)
{
	valueDeleteSelected();
}

void CStringInput::valueDeleteSelected(void) // delete value[selected]
{
	if (len > 0)
	{
		for (int i = selected; i < len; i++)
			value[i] = value[i + 1];
		len--;

		if (selected == len)
			selected--;
		if (selected < 0)
			selected = 0;

		for (int i = selected; i < len + 1; i++)
			paintChar(i);
	}
}

void CStringInput::keyRedPressed()
{
}

void CStringInput::keyYellowPressed()
{
	valueDeleteAll();
}

void CStringInput::valueDeleteAll() // clear all
{
	selected=0;
	for (int i = 0; i < len + 1; i++) {
		value[i] = 0;
		paintChar(i);
	}
	len = 0;
}

void CStringInput::keyBluePressed()
{
	valueInsert();
}

void CStringInput::valueInsert() // insert character at value[selected]
{
	if ((len < size) && value[selected]) {
		len++;
		for (int i = len; i > selected; i--) {
			value[i] = value[i - 1];
			paintChar(i);
		}
		value[selected] = 0;
		paintChar(selected);
	}
}

void CStringInput::keyUpPressed()
{
	valueSetStart();
}

void CStringInput::valueSetStart() // jump to start of string
{
	if (!value[selected] && len > 0)
		valueDeleteSelected();
	if (selected) {
		int old = selected;
		selected = 0;
		paintChar(old);
		paintChar(selected);
	}
}

void CStringInput::keyDownPressed()
{
	valueSetEnd();
}

void CStringInput::valueSetEnd() // jump to end of string
{
	if (!value[selected] && len > 0)
		valueDeleteSelected();
	if (len && (selected != len - 1)) {
		int old = selected;
		selected = len - 1;
		paintChar(old);
		paintChar(selected);
	}
}

void CStringInput::keyLeftPressed()
{
	valuePrev();
}

void CStringInput::valuePrev() // advance to the left
{
	if(selected > 0) {
		if (!value[selected] && len > 0) {
			valueDeleteSelected();
			return;
		}
		int old = selected;
		selected--;
		paintChar(old);
		paintChar(selected);
	}
}

void CStringInput::keyRightPressed()
{
	valueNext();
}

void CStringInput::valueNext() // advance to the right
{
	if (!len)
		return;

	if (!value[selected] && len == selected + 1)
		return;

	if (!value[selected] && len > selected + 1) {
		valueDeleteSelected();
		return;
	}
	if (selected + 1 < size) {
		int old = selected;
		selected++;

		if (selected == len && len < size)
			len++;
		paintChar(old);
		paintChar(selected);
	}
}

void CStringInput::keyMinusPressed()
{
	valueDeleteSelected();
}

void CStringInput::keyPlusPressed()
{
	valueInsert();
}

int CStringInput::exec(CMenuTarget* parent, const std::string & )
{
	neutrino_msg_t      msg;
	neutrino_msg_data_t data;
	int res = menu_return::RETURN_REPAINT;

        char oldval[size+1], dispval[size+1];
        oldval[size] = 0;
        dispval[size] = 0;
	len = strlen(value);
	selected = 0;


	if (parent)
		parent->hide();

	strncpy(oldval, value, size);

	paint();

	unsigned long long timeoutEnd = CRCInput::calcTimeoutEnd(g_settings.timing[SNeutrinoSettings::TIMING_MENU] == 0 ? 0xFFFF : g_settings.timing[SNeutrinoSettings
::TIMING_MENU]);

	uint32_t repeatkeys[] = {
		CRCInput::RC_0, CRCInput::RC_1, CRCInput::RC_2, CRCInput::RC_3,
		CRCInput::RC_4, CRCInput::RC_5, CRCInput::RC_6, CRCInput::RC_7,
		CRCInput::RC_8, CRCInput::RC_9, CRCInput::RC_nokey
	};

	uint32_t *old_repeatkeys = g_RCInput->setAllowRepeat(repeatkeys);

	bool loop=true;
	while (loop)
	{
		if ( strncmp(value, dispval, size) != 0)
		{
			CVFD::getInstance()->showMenuText(1, value, selected+1);
			strncpy(dispval, value, size);
		}

		g_RCInput->getMsgAbsoluteTimeout(&msg, &data, &timeoutEnd, true );

		if ( msg <= CRCInput::RC_MaxRC )
			timeoutEnd = CRCInput::calcTimeoutEnd(g_settings.timing[SNeutrinoSettings::TIMING_MENU] == 0 ? 0xFFFF : g_settings.timing[SNeutrinoSettings
::TIMING_MENU]);

		if (msg==CRCInput::RC_left)
		{
			keyLeftPressed();
		}
		else if (msg==CRCInput::RC_right)
		{
			keyRightPressed();
		}
		else if (CRCInput::getUnicodeValue(msg) != -1)
		{
			NormalKeyPressed(msg);
		}
		else if (msg==CRCInput::RC_backspace)
		{
			keyBackspacePressed();
		}
		else if (msg==CRCInput::RC_green)
		{
			keyGreenPressed();
		}
		else if (msg==CRCInput::RC_red)
		{
			keyRedPressed();
		}
		else if (msg==CRCInput::RC_yellow)
		{
			keyYellowPressed();
		}
		else if ( (msg==CRCInput::RC_green) && (index(validchars, '.') != NULL))
		{
			value[selected] = '.';

			if (selected + 1 < size)
			{
				selected++;
				paintChar(selected - 1);
			}
  
			paintChar(selected);
		}
		else if (msg== CRCInput::RC_blue)
		{
			keyBluePressed();
		}
		else if (msg==CRCInput::RC_up)
		{
			keyUpPressed();
		}
		else if (msg==CRCInput::RC_down)
		{
			keyDownPressed();
		} else if (msg==CRCInput::RC_plus)
		{
			keyPlusPressed();
		} else if (msg==CRCInput::RC_minus)
		{
			keyMinusPressed();
		}
		else if (msg==CRCInput::RC_ok)
		{
			loop=false;
		}
		else if ( (msg==CRCInput::RC_home) || (msg==CRCInput::RC_timeout) )
		{
			if ( ( strcmp(value, oldval) != 0) &&
			     (ShowLocalizedMessage(name, LOCALE_MESSAGEBOX_DISCARD, CMessageBox::mbrYes, CMessageBox::mbYes | CMessageBox::mbCancel) == CMessageBox::mbrCancel))
				continue;

			strncpy(value, oldval, size);
			loop=false;
		}
		else if ((msg ==CRCInput::RC_sat) || (msg == CRCInput::RC_favorites))
		{
		}
		else
		{
			int r = handleOthers( msg, data );
			if (r & (messages_return::cancel_all | messages_return::cancel_info))
			{
				res = (r & messages_return::cancel_all) ? menu_return::RETURN_EXIT_ALL : menu_return::RETURN_EXIT;
				loop = false;
			}
			else if ( r & messages_return::unhandled )
			{
				if ( CNeutrinoApp::getInstance()->handleMsg( msg, data ) & messages_return::cancel_all )
				{
					loop = false;
					res = menu_return::RETURN_EXIT_ALL;
				}
			}
		}
	}
	g_RCInput->setAllowRepeat(old_repeatkeys);

	hide();

	value[size]=0;

	if (valueString != NULL)
        {
                *valueString = value;
        }

        if ( (observ) && (msg==CRCInput::RC_ok) )
        {
                observ->changeNotify(name, value);
        }

	return res;
}

int CStringInput::handleOthers(const neutrino_msg_t msg, const neutrino_msg_data_t data)
{
	return messages_return::unhandled;
}

void CStringInput::hide()
{
	frameBuffer->paintBackgroundBoxRel(x, y, width, height + 1);
}

const char * CStringInput::getHint1(void)
{
	return g_Locale->getText(hint_1);
}

void CStringInput::paint()
{
	int iconoffset;

	//frameBuffer->paintBoxRel(x, y, width, hheight, COL_MENUHEAD_PLUS_0);
	//frameBuffer->paintBoxRel(x, y + hheight, width, height - hheight, COL_MENUCONTENT_PLUS_0);
	frameBuffer->paintBoxRel(x, y, width, hheight, COL_MENUHEAD_PLUS_0, ROUND_RADIUS, 1); //round
	frameBuffer->paintBoxRel(x, y + hheight, width, height - hheight, COL_MENUCONTENT_PLUS_0, ROUND_RADIUS, 2);//round

	if (!(iconfile.empty()))
	{
		frameBuffer->paintIcon(iconfile, x + 8, y + 5);
		iconoffset = 28;
	}
	else
		iconoffset = 0;

	g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->RenderString(x+ 10+ iconoffset, y+ hheight, width- 10- iconoffset, head ? head : g_Locale->getText(name), COL_MENUHEAD, 0, true); // UTF-8

	if (hint_1 != NONEXISTANT_LOCALE)
	{
		g_Font[SNeutrinoSettings::FONT_TYPE_MENU_INFO]->RenderString(x+ 20, y+ hheight+ mheight+ iheight+ 40, width- 20, getHint1(), COL_MENUCONTENT, 0, true); // UTF-8
		if (hint_2 != NONEXISTANT_LOCALE)
			g_Font[SNeutrinoSettings::FONT_TYPE_MENU_INFO]->RenderString(x+ 20, y+ hheight+ mheight+ iheight* 2+ 40, width- 20, g_Locale->getText(hint_2), COL_MENUCONTENT, 0, true); // UTF-8
	}

	for (int count=0;count<size;count++)
		paintChar(count);

}

void CStringInput::paintChar(int pos, const char c)
{
	const int xs = 20;
	int ys = mheight;
	int xpos = x+ 20+ pos* xs;
	int ypos = y+ hheight+ 25;

	char ch[2] = {c, 0};

	uint8_t    color;
	fb_pixel_t bgcolor;
	
	if (pos == selected)
	{
		color   = COL_MENUCONTENTSELECTED;
		bgcolor = COL_MENUCONTENTSELECTED_PLUS_0;
	}
	else
	{
		color   = COL_MENUCONTENT;
		bgcolor = COL_MENUCONTENT_PLUS_0;
	}

	frameBuffer->paintBoxRel(xpos, ypos, xs, ys, COL_MENUCONTENT_PLUS_4);
	frameBuffer->paintBoxRel(xpos+ 1, ypos+ 1, xs- 2, ys- 2, bgcolor);

	if ((pos < len) && c) {
		int xfpos = xpos + ((xs- g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->getRenderWidth(ch))>>1);
		g_Font[SNeutrinoSettings::FONT_TYPE_MENU]->RenderString(xfpos,ypos+ys, width, ch, color);
	} else {
		frameBuffer->paintLine(xpos, ypos + ys, xpos + xs, ypos, COL_MENUCONTENT_PLUS_4);
		frameBuffer->paintLine(xpos + xs, ypos + ys, xpos, ypos, COL_MENUCONTENT_PLUS_4);
	}
}

void CStringInput::paintChar(int pos)
{
	if (pos < size)
		paintChar(pos, value[pos]);
}

CStringInputSMS::CStringInputSMS(const neutrino_locale_t Name, std::string* Value, int Size, const neutrino_locale_t Hint_1, const neutrino_locale_t Hint_2, const char * const Valid_Chars, CChangeObserver* Observ, const char * const Icon)
		: CStringInput(Name, Value, Size, Hint_1, Hint_2, Valid_Chars, Observ, Icon)
{
	initSMS(Valid_Chars);
}

CStringInputSMS::CStringInputSMS(const neutrino_locale_t Name, char* Value, int Size, const neutrino_locale_t Hint_1, const neutrino_locale_t Hint_2, const char * const Valid_Chars, CChangeObserver* Observ, const char * const Icon)
   		: CStringInput(Name, Value, Size, Hint_1, Hint_2, Valid_Chars, Observ, Icon)
   {
	initSMS(Valid_Chars);
}

void CStringInputSMS::initSMS(const char * const Valid_Chars)
{
	last_digit = -1;				// no key pressed yet
	const char CharList[10][11] = { "0 -_/()<>=",	// 9 characters
					"1+.,:!?\\",
					"abc2ä",
					"def3",
					"ghi4",
					"jkl5",
					"mno6ö",
					"pqrs7ß",
					"tuv8ü",
					"wxyz9" };

	for (int i = 0; i < 10; i++)
	{
		int j = 0;
		for (int k = 0; k < (int) strlen(CharList[i]); k++)
			if (strchr(Valid_Chars, CharList[i][k]) != NULL)
				Chars[i][j++] = CharList[i][k];
		if (j == 0)
			Chars[i][j++] = ' ';	// prevent empty char lists 
		arraySizes[i] = j;
	}

	height+=260;
	y = ((500-height)>>1);
}


void CStringInputSMS::NormalKeyPressed(const neutrino_msg_t key)
{
	if(len == 0)
		len++;

	if (CRCInput::isNumeric(key))
	{
		int numericvalue = CRCInput::getNumericValue(key);
		if (last_digit != numericvalue)
		{
			if (last_digit != -1)	// there is a last key
			{
				if (selected + 1 < size)
					selected++;
				if ((selected == len) && (len < size))
					len++;
				paintChar(selected - 1);
			}
			keyCounter = 0;
		}
		else
			keyCounter = (keyCounter + 1) % arraySizes[numericvalue];
		value[selected] = Chars[numericvalue][keyCounter];
		last_digit = numericvalue;
		paintChar(selected);
	}
	else
	{
		value[selected] = (char)CRCInput::getUnicodeValue(key);
		valueToogleCase();   /* to lower, paintChar */
		valueNext(); /* last_digit = -1, move to next position */
	}
}

void CStringInputSMS::keyBackspacePressed()
{
	valueDeletePrev();
}

void CStringInputSMS::valueDeletePrev()
{
	last_digit = -1;
	CStringInput::valueDeletePrev();
}

void CStringInputSMS::keyGreenPressed()
{
	valueDeleteSelected();
}

void CStringInputSMS::valueDeleteSelected()
{
	last_digit = -1;
	CStringInput::valueDeleteSelected();
}

void CStringInputSMS::keyRedPressed()
{
	valueToogleCase();
}

void CStringInputSMS::valueToogleCase()		// switch between lower & uppercase
{
	if (((value[selected] | 32) >= 'a') && ((value[selected] | 32) <= 'z'))
	value[selected] ^= 32;

	last_digit = -1;
	paintChar(selected);
}

void CStringInputSMS::keyYellowPressed()
{
	valueDeleteAll();
}

void CStringInputSMS::valueDeleteAll()		// clear all
{
	last_digit = -1;
	CStringInput::valueDeleteAll();
}

void CStringInputSMS::keyBluePressed()
{
	last_digit = -1;
	CStringInput::valueInsert();
}
void CStringInputSMS::valueInsert()
{
	last_digit = -1;
	CStringInput::valueInsert();
}

void CStringInputSMS::keyUpPressed()
{
	last_digit = -1;
	CStringInput::valueSetStart();
}

void CStringInputSMS::valueSetStart()
{
	last_digit = -1;
	CStringInput::valueSetStart();
}

void CStringInputSMS::keyDownPressed()
{
	last_digit = -1;
	CStringInput::valueSetEnd();
}

void CStringInputSMS::valueSetEnd()
{
	last_digit = -1;
	CStringInput::valueSetEnd();
}

void CStringInputSMS::keyLeftPressed()
{
	last_digit = -1;
	CStringInput::valuePrev();
}

void CStringInputSMS::valuePrev()
{
	last_digit = -1;
	CStringInput::valuePrev();
}

void CStringInputSMS::keyRightPressed()
{
	last_digit = -1;
	CStringInput::valueNext();
}

void CStringInputSMS::valueNext()
{
	last_digit = -1;
	CStringInput::valueNext();
}

const struct button_label CStringInputSMSButtons[4] =
{
	{ NEUTRINO_ICON_BUTTON_RED   , LOCALE_STRINGINPUT_CAPS  },
	{ NEUTRINO_ICON_BUTTON_GREEN , LOCALE_STRINGINPUT_CLEAR },
	{ NEUTRINO_ICON_BUTTON_YELLOW, LOCALE_STRINGINPUT_CLEAR_ALL },
	{ NEUTRINO_ICON_BUTTON_BLUE  , LOCALE_STRINGINPUT_INSERT }
};

void CStringInputSMS::paint()
{
	CStringInput::paint();

	frameBuffer->paintIcon("numericpad", x+20+140, y+ hheight+ mheight+ iheight* 3+ 30, COL_MENUCONTENT);

	frameBuffer->paintBoxRel(x,y+height-25, width,25, COL_MENUHEAD_PLUS_0, ROUND_RADIUS, 2);
	frameBuffer->paintHLine(x, x+width,  y+height-25, COL_INFOBAR_SHADOW_PLUS_0);

	::paintButtons(frameBuffer, g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL], x + 8, y+height-25+1, (width - 16)/4, 4, CStringInputSMSButtons);
}

void CPINInput::paintChar(int pos)
{
	CStringInput::paintChar(pos, (value[pos] == ' ') ? ' ' : '*');
}

int CPINInput::exec( CMenuTarget* parent, const std::string & )
{
	neutrino_msg_t      msg;
	neutrino_msg_data_t data;

	len = strlen(value);
	selected = 0;

	int res = menu_return::RETURN_REPAINT;

	if (parent)
		parent->hide();

	for(int count=strlen(value)-1;count<size-1;count++)
		strcat(value, " ");

	paint();

	bool loop = true;

	while(loop)
	{
		g_RCInput->getMsg( &msg, &data, 300 );

		if (msg==CRCInput::RC_left)
		{
			keyLeftPressed();
		}
		else if (msg==CRCInput::RC_right)
		{
			keyRightPressed();
		}
		else if (CRCInput::isNumeric(msg))
		{
			int old_selected = selected;
			NormalKeyPressed(msg);
			if ( old_selected == ( size- 1 ) )
				loop=false;
		}
		else if ( (msg==CRCInput::RC_up) ||
				  (msg==CRCInput::RC_down) )
		{
			g_RCInput->postMsg( msg, data );
			res = menu_return::RETURN_EXIT;
			loop=false;
		}
		else if ( (msg==CRCInput::RC_home) || (msg==CRCInput::RC_timeout) || (msg==CRCInput::RC_ok) )
		{
			value[0] = 0;
			loop=false;
		}
		else
		{
			int r = handleOthers(msg, data);
			if (r & (messages_return::cancel_all | messages_return::cancel_info))
			{
				res = (r & messages_return::cancel_all) ? menu_return::RETURN_EXIT_ALL : menu_return::RETURN_EXIT;
				loop = false;
			}
			else if ( r & messages_return::unhandled )
			{
				if ( CNeutrinoApp::getInstance()->handleMsg( msg, data ) & ( messages_return::cancel_all | messages_return::cancel_info ) )
				{
					loop = false;
					res = menu_return::RETURN_EXIT_ALL;
				}
			}
		}

	}

	hide();

	value[size]=0;

	if ( (observ) && (msg==CRCInput::RC_ok) )
	{
		observ->changeNotify(name, value);
	}

	return res;
}

int CPLPINInput::handleOthers(neutrino_msg_t msg, neutrino_msg_data_t data)
{
	int res = messages_return::unhandled;

	if ( msg == NeutrinoMessages::EVT_PROGRAMLOCKSTATUS )
	{
		// trotzdem handlen
		CNeutrinoApp::getInstance()->handleMsg(msg, data);

		if (data != (neutrino_msg_data_t) fsk)
			res = messages_return::cancel_info;
	}

	return res;
}

const char * CPLPINInput::getHint1(void)
{
	if (fsk == 0x100)
	{
		hint_1 = LOCALE_PARENTALLOCK_LOCKEDCHANNEL;
		return CStringInput::getHint1();
	}
	else
	{
		sprintf(hint, g_Locale->getText(LOCALE_PARENTALLOCK_LOCKEDPROGRAM), fsk);
		return hint;
	}
}

#define borderwidth 4

int CPLPINInput::exec( CMenuTarget* parent, const std::string & )
{
#ifdef __sh__
	fb_pixel_t * pixbuf = frameBuffer->allocPixelBuffer((width+ 2* borderwidth), (height+ 2* borderwidth));
	int checkSize;
	if (pixbuf != NULL)
		checkSize = frameBuffer->SaveScreen(x- borderwidth, y- borderwidth, width+ 2* borderwidth, height+ 2* borderwidth, pixbuf);
#else
	fb_pixel_t * pixbuf = new fb_pixel_t[(width+ 2* borderwidth) * (height+ 2* borderwidth)];

	if (pixbuf != NULL)
		frameBuffer->SaveScreen(x- borderwidth, y- borderwidth, width+ 2* borderwidth, height+ 2* borderwidth, pixbuf);
#endif

	// clear border
	frameBuffer->paintBackgroundBoxRel(x- borderwidth, y- borderwidth, width+ 2* borderwidth, borderwidth);
	frameBuffer->paintBackgroundBoxRel(x- borderwidth, y+ height, width+ 2* borderwidth, borderwidth);
	frameBuffer->paintBackgroundBoxRel(x- borderwidth, y, borderwidth, height);
	frameBuffer->paintBackgroundBoxRel(x+ width, y, borderwidth, height);

	int res = CPINInput::exec ( parent, "" );

	if (pixbuf != NULL)
	{
#ifdef __sh__
		frameBuffer->RestoreScreen(x- borderwidth, y- borderwidth, width+ 2* borderwidth, height+ 2* borderwidth, pixbuf, checkSize);
#else
		frameBuffer->RestoreScreen(x- borderwidth, y- borderwidth, width+ 2* borderwidth, height+ 2* borderwidth, pixbuf);
#endif
		delete[] pixbuf;//Mismatching allocation and deallocation: pixbuf
	}

	return ( res );
}
