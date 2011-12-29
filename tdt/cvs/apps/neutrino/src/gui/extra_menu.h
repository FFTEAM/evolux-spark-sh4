/*
Neutrino-GUI - DBoxII-Project

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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef __extramenu__
#define __extramenu__

#include <string>
#include <vector>

#include <driver/framebuffer.h>
#include <system/lastchannel.h>
#include <system/setting_helpers.h>

using namespace std;

class CORRECTVOLUME_Menu : public CMenuTarget
{
	private:

	CFrameBuffer *frameBuffer;
	int x;
	int y;
	int width;
	int height;
	int hheight,mheight; // head/menu font height

	void paint();


	public:

	CORRECTVOLUME_Menu();

	void hide();
	int exec(CMenuTarget* parent, const std::string & actionKey);
	void CORRECTVOLUMESettings();

};

class TUNERRESET_Menu : public CMenuTarget
{
	private:

	CFrameBuffer *frameBuffer;
	int x;
	int y;
	int width;
	int height;
	int hheight,mheight; // head/menu font height

	void paint();


	public:

	TUNERRESET_Menu();
	bool TunerReset();
	void hide();
	int exec(CMenuTarget* parent, const std::string & actionKey);
	void TUNERRESETSettings();

};

class EMU_Menu : public CMenuTarget
{
	private:

	CFrameBuffer *frameBuffer;
	int x;
	int y;
	int width;
	int height;
	int hheight,mheight; // head/menu font height

	void paint();
	bool showemuversions();


	public:

	EMU_Menu();
	bool CamdReset();
	void hide();
	int exec(CMenuTarget* parent, const std::string & actionKey);
	void EMU_Menu_Settings();

};

class AMOUNT_Menu : public CMenuTarget
{
	private:

	CFrameBuffer *frameBuffer;
	int x;
	int y;
	int width;
	int height;
	int hheight,mheight; // head/menu font height

	void paint();


	public:

	AMOUNT_Menu();

	void hide();
	int exec(CMenuTarget* parent, const std::string & actionKey);
	void AMOUNTSettings();

};

class CHECKFS_Menu : public CMenuTarget
{
	private:

	CFrameBuffer *frameBuffer;
	int x;
	int y;
	int width;
	int height;
	int hheight,mheight; // head/menu font height

	void paint();


	public:

	CHECKFS_Menu();

	void hide();
	int exec(CMenuTarget* parent, const std::string & actionKey);
	void CHECKFSSettings();

};

class DISPLAYTIME_Menu : public CMenuTarget
{
	private:

	CFrameBuffer *frameBuffer;
	int x;
	int y;
	int width;
	int height;
	int hheight,mheight; // head/menu font height

	void paint();


	public:

	DISPLAYTIME_Menu();

	void hide();
	int exec(CMenuTarget* parent, const std::string & actionKey);
	void DISPLAYTIMESettings();

};

class WWWDATE_Menu : public CMenuTarget
{
	private:

	CFrameBuffer *frameBuffer;
	int x;
	int y;
	int width;
	int height;
	int hheight,mheight; // head/menu font height

	void paint();


	public:

	WWWDATE_Menu();

	void hide();
	int exec(CMenuTarget* parent, const std::string & actionKey);
	void WWWDATESettings();

};

class SWAP_Menu : public CMenuTarget
{
	private:

	CFrameBuffer *frameBuffer;
	int x;
	int y;
	int width;
	int height;
	int hheight,mheight; // head/menu font height

	void paint();


	public:

	SWAP_Menu();
	void hide();
	int exec(CMenuTarget* parent, const std::string & actionKey);
	void SWAP_Menu_Settings();

};

class BOOTE2_Menu : public CMenuTarget
{
	private:

	CFrameBuffer *frameBuffer;
	int x;
	int y;
	int width;
	int height;
	int hheight,mheight; // head/menu font height

	void paint();


	public:

	BOOTE2_Menu();

	void hide();
	int exec(CMenuTarget* parent, const std::string & actionKey);
	void BOOTE2Settings();

};

class BOOTSPARK_Menu : public CMenuTarget
{
	private:

	CFrameBuffer *frameBuffer;
	int x;
	int y;
	int width;
	int height;
	int hheight,mheight; // head/menu font height

	void paint();


	public:

	BOOTSPARK_Menu();

	void hide();
	int exec(CMenuTarget* parent, const std::string & actionKey);
	void BOOTSPARKSettings();

};

class FSCK_Menu : public CMenuTarget
{
	private:

	CFrameBuffer *frameBuffer;
	int x;
	int y;
	int width;
	int height;
	int hheight,mheight; // head/menu font height

	void paint();


	public:

	FSCK_Menu();

	void hide();
	int exec(CMenuTarget* parent, const std::string & actionKey);
	void FSCKSettings();

};

class STMFB_Menu : public CMenuTarget
{
	private:

	CFrameBuffer *frameBuffer;
	int x;
	int y;
	int width;
	int height;
	int hheight,mheight; // head/menu font height

	void paint();


	public:

	STMFB_Menu();

	void hide();
	int exec(CMenuTarget* parent, const std::string & actionKey);
	void STMFBSettings();

};

class FRITZCALL_Menu : public CMenuTarget
{
	private:

	CFrameBuffer *frameBuffer;
	int x;
	int y;
	int width;
	int height;
	int hheight,mheight; // head/menu font height

	void paint();


	public:

	FRITZCALL_Menu();

	void hide();
	int exec(CMenuTarget* parent, const std::string & actionKey);
	void FRITZCALLSettings();

};
#endif //__emumenu__
