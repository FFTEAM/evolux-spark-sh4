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

#include <gui/channellist.h>

#include <global.h>
#include <neutrino.h>

#include <driver/fontrenderer.h>
#include <driver/screen_max.h>
#include <driver/rcinput.h>

#include <gui/color.h>
#include <gui/eventlist.h>
#include <gui/infoviewer.h>
#include <gui/widget/buttons.h>
#include <gui/widget/icons.h>
#include <gui/widget/menue.h>
#include <gui/widget/messagebox.h>

#include <system/settings.h>
#include <system/lastchannel.h>
#include "gui/filebrowser.h"

#include <gui/bouquetlist.h>
#include <daemonc/remotecontrol.h>
#include <zapit/client/zapittools.h>
#include <driver/vcrcontrol.h>
#include <gui/pictureviewer.h>
#include <zapit/bouquets.h>
#include <zapit/satconfig.h>
#include <zapit/getservices.h>

#include <zapit/frontend_c.h>

#include <gui/epgplus.h>


extern CBouquetList * bouquetList;      		/* neutrino.cpp */
extern CRemoteControl * g_RemoteControl; 		/* neutrino.cpp */
extern SMSKeyInput * c_SMSKeyInput;			// defined in neutrino
extern CPictureViewer * g_PicViewer;
extern CBouquetList   * TVbouquetList;
extern CBouquetList   * TVsatList;
extern CBouquetList   * TVfavList;
extern CBouquetList   * TVallList;
extern CBouquetList   * RADIObouquetList;
extern CBouquetList   * RADIOsatList;
extern CBouquetList   * RADIOfavList;
extern CBouquetList   * RADIOallList;

#define PIC_W 52
#define PIC_H 39


extern t_channel_id rec_channel_id;
extern t_channel_id live_channel_id;
bool pip_selected = false;
extern bool autoshift;
int info_height = 0;
bool new_mode_active = 0;

extern int FrontendCount;			// defined in zapit.cpp


extern CBouquetManager *g_bouquetManager;

void sectionsd_getChannelEvents(CChannelEventList &eList, const bool tv_mode, t_channel_id *chidlist, int clen);
void sectionsd_getEventsServiceKey(t_channel_id serviceUniqueKey, CChannelEventList &eList, char search = 0, std::string search_text = "");
void addChannelToBouquet(const unsigned int bouquet, const t_channel_id channel_id);

extern int old_b_id;

CChannelList::CChannelList(const char * const Name, bool historyMode, bool _vlist)
{
	frameBuffer = CFrameBuffer::getInstance();
	name = Name;
	selected = 0;
	liststart = 0;
	tuned=0xfffffff;
	zapProtection = NULL;
	this->historyMode = historyMode;
	vlist = _vlist;
	
	//printf("CChannelList::CChannelList: add %s : %x\n", name.c_str(), this);fflush(stdout);
}

CChannelList::~CChannelList()
{
	//printf("DELETE LIST %s : %x\n", name.c_str(), this);fflush(stdout);
	chanlist.clear();
}

void CChannelList::ClearList(void)
{
	//printf("CLEAR LIST %s : %x\n", name.c_str(), this);fflush(stdout);
	chanlist.clear();
	chanlist.resize(1);
}

void CChannelList::setSize(int newsize)
{
	chanlist.reserve(newsize);
	//chanlist.resize(newsize);
}

void CChannelList::addChannel(CZapitChannel* channel, int num)
{
	//printf("CChannelList::addChannel: %s %s fe(%d)\n", name.c_str(), channel->getName().c_str(), channel->getFeIndex());fflush(stdout);
	
	if(num)
		channel->number = num;
	
	chanlist.push_back(channel);
}

void CChannelList::putChannel(CZapitChannel* channel)
{
	int num = channel->number - 1;
	
	if(num < 0) 
	{
		//printf("CChannelList::addChannel error inserting at %d\n", num);
		return;
	}
	
	if(num >= (int) chanlist.size()) 
	{
		chanlist.resize((unsigned) num + 1);
	}
	chanlist[num] = channel;

	//printf("%s : me %x putChannel: %d: %s %x -> %x [0] %x\n", name.c_str(), this, num, channel->getName().c_str(), channel, chanlist[num], chanlist[0]);fflush(stdout);
}

void CChannelList::updateEvents(void)
{
	CChannelEventList events;

	if (displayNext) 
	{
		if (chanlist.size()) 
		{
			time_t atime = time(NULL);
			unsigned int count;
			for (count=0; count < chanlist.size(); count++)
			{
				//CChannelEventList events = g_Sectionsd->getEventsServiceKey(chanlist[liststart+count]->channel_id &0xFFFFFFFFFFFFULL);
				sectionsd_getEventsServiceKey(chanlist[count]->channel_id &0xFFFFFFFFFFFFULL, events);
				chanlist[count]->nextEvent.startTime = (long)0x7fffffff;
				
				for ( CChannelEventList::iterator e= events.begin(); e != events.end(); ++e ) 
				{
					if (((long)(e->startTime) > atime) && ((e->startTime) < (long)(chanlist[count]->nextEvent.startTime)))
					{
						chanlist[count]->nextEvent= *e;
						break; //max: FIXME no sense to continue ?
					}
				}
			}
		}
	} 
	else 
	{
		t_channel_id *p_requested_channels = NULL;
		int size_requested_channels = 0;

		if (chanlist.size()) 
		{
			size_requested_channels = chanlist.size()*sizeof(t_channel_id);
			p_requested_channels    = (t_channel_id*)malloc(size_requested_channels);
			for (uint32_t count = 0; count < chanlist.size(); count++)
			{
				p_requested_channels[count] = chanlist[count]->channel_id&0xFFFFFFFFFFFFULL;
			}

			//CChannelEventList events = g_Sectionsd->getChannelEvents((CNeutrinoApp::getInstance()->getMode()) != NeutrinoMessages::mode_radio, p_requested_channels, size_requested_channels);

			CChannelEventList events;
			sectionsd_getChannelEvents(events, (CNeutrinoApp::getInstance()->getMode()) != NeutrinoMessages::mode_radio, p_requested_channels, size_requested_channels);
			for (uint32_t count=0; count < chanlist.size(); count++) 
			{
				chanlist[count]->currentEvent = CChannelEvent();
				for ( CChannelEventList::iterator e = events.begin(); e != events.end(); ++e )
				{
					if ((chanlist[count]->channel_id&0xFFFFFFFFFFFFULL) == e->get_channel_id())
					{
						chanlist[count]->currentEvent= *e;
						break;
					}
				}
			}
			if (p_requested_channels != NULL) 
				free(p_requested_channels);
		}
	}
	events.clear();
}

struct CmpChannelBySat: public binary_function <const CZapitChannel * const, const CZapitChannel * const, bool>
{
        static bool comparetolower(const char a, const char b)
        {
		return tolower(a) < tolower(b);
        };

        bool operator() (const CZapitChannel * const c1, const CZapitChannel * const c2)
        {
		if(c1->getSatellitePosition() == c2->getSatellitePosition())
			return std::lexicographical_compare(c1->getName().begin(), c1->getName().end(), c2->getName().begin(), c2->getName().end(), comparetolower);
		else
			return c1->getSatellitePosition() < c2->getSatellitePosition();
;
	};
};

struct CmpChannelByFreq: public binary_function <const CZapitChannel * const, const CZapitChannel * const, bool>
{
        static bool comparetolower(const char a, const char b)
        {
                return tolower(a) < tolower(b);
        };

        bool operator() (const CZapitChannel * const c1, const CZapitChannel * const c2)
        {
		if(c1->getFreqId() == c2->getFreqId())
			return std::lexicographical_compare(c1->getName().begin(), c1->getName().end(), c2->getName().begin(), c2->getName().end(), comparetolower);
		else
			return c1->getFreqId() < c2->getFreqId();
;
	};
};

//TEST
struct CmpChannelByFeIndex: public binary_function <const CZapitChannel * const, const CZapitChannel * const, bool>
{
        static bool comparetolower(const char a, const char b)
        {
		return tolower(a) < tolower(b);
        };

        bool operator() (const CZapitChannel * const c1, const CZapitChannel * const c2)
        {
		if(c1->getFeIndex() == c2->getFeIndex() )
			return std::lexicographical_compare(c1->getName().begin(), c1->getName().end(), c2->getName().begin(), c2->getName().end(), comparetolower);
		else
			return c1->getFeIndex() < c2->getFeIndex();
;
	};
};
//

void CChannelList::SortAlpha(void)
{
	sort(chanlist.begin(), chanlist.end(), CmpChannelByChName());
}

void CChannelList::SortSat(void)
{
	sort(chanlist.begin(), chanlist.end(), CmpChannelBySat());
}

CZapitChannel* CChannelList::getChannel(int number)
{
	for (uint32_t i=0; i< chanlist.size();i++) 
	{
		if (chanlist[i]->number == number)
			return chanlist[i];
	}
	
	return(NULL);
}

CZapitChannel* CChannelList::getChannel(t_channel_id channel_id)
{
	for (uint32_t i=0; i< chanlist.size();i++) {
		if (chanlist[i]->channel_id == channel_id)
			return chanlist[i];
	}
	return(NULL);
}

int CChannelList::getKey(int id)
{
	return chanlist[id]->number;
}

static const std::string empty_string;

const std::string & CChannelList::getActiveChannelName(void) const
{
	if (selected < chanlist.size())
		return chanlist[selected]->name;
	else
		return empty_string;
}

t_satellite_position CChannelList::getActiveSatellitePosition(void) const
{
	if (selected < chanlist.size())
		return chanlist[selected]->getSatellitePosition();
	else
		return 0;
}

t_channel_id CChannelList::getActiveChannel_ChannelID(void) const
{
	if (selected < chanlist.size()) 
	{
		//printf("CChannelList::getActiveChannel_ChannelID me %x selected = %d %llx\n", (int) this, selected, chanlist[selected]->channel_id);
		return chanlist[selected]->channel_id;
	} 
	else
		return 0;
}

int CChannelList::getActiveChannelNumber(void) const
{
	return (selected + 1);
}

int CChannelList::doChannelMenu(void)
{
	int i = 0;
	int select = -1;
	static int old_selected = 0;
	int ret = menu_return::RETURN_NONE;
	signed int bouquet_id, old_bouquet_id, new_bouquet_id;
	int result;
	char cnt[5];
	t_channel_id channel_id;
	
	if( !bouquetList )
		return 0;

	CMenuWidget * menu = new CMenuWidget(LOCALE_CHANNELLIST_EDIT, NEUTRINO_ICON_SETTINGS);
	CMenuSelectorTarget * selector = new CMenuSelectorTarget(&select);

	sprintf(cnt, "%d", i);
	menu->addItem(new CMenuForwarder(LOCALE_BOUQUETEDITOR_DELETE, true, NULL, selector, cnt, CRCInput::RC_red, NEUTRINO_ICON_BUTTON_RED), old_selected == i++);

	sprintf(cnt, "%d", i);
	menu->addItem(new CMenuForwarder(LOCALE_BOUQUETEDITOR_MOVE, true, NULL, selector, cnt, CRCInput::RC_green, NEUTRINO_ICON_BUTTON_GREEN), old_selected == i++);

	sprintf(cnt, "%d", i);
	menu->addItem(new CMenuForwarder(LOCALE_EXTRA_ADD_TO_BOUQUET, true, NULL, selector, cnt, CRCInput::RC_yellow, NEUTRINO_ICON_BUTTON_YELLOW), old_selected == i++);

	sprintf(cnt, "%d", i);
	menu->addItem(new CMenuForwarder(LOCALE_FAVORITES_MENUEADD, true, NULL, selector, cnt, CRCInput::RC_blue, NEUTRINO_ICON_BUTTON_BLUE), old_selected == i++);

	ret = menu->exec(NULL, "");
	delete menu;
	delete selector;

	if(select >= 0) 
	{
		old_selected = select;
		channel_id = chanlist[selected]->channel_id;
		switch(select) 
		{
			case 0: //delete
				hide();
				result = ShowMsgUTF ( LOCALE_BOUQUETEDITOR_DELETE, "Delete channel from bouquet?", CMessageBox::mbrNo, CMessageBox::mbYes | CMessageBox::mbNo );

				if(result == CMessageBox::mbrYes) 
				{
					bouquet_id = bouquetList->getActiveBouquetNumber();
					bouquet_id = g_bouquetManager->existsBouquet(bouquetList->Bouquets[bouquet_id]->channelList->getName());
					if (bouquet_id == -1)
						return 0;
					
					if(g_bouquetManager->existsChannelInBouquet(bouquet_id, channel_id)) 
					{
						g_bouquetManager->Bouquets[bouquet_id]->removeService(channel_id);
						return 1;
					}
				}
				break;
				
			case 1: // move
				old_bouquet_id = bouquetList->getActiveBouquetNumber();
				old_bouquet_id = g_bouquetManager->existsBouquet(bouquetList->Bouquets[old_bouquet_id]->channelList->getName());

				do {
					new_bouquet_id = bouquetList->exec(false);
				} while(new_bouquet_id == -3);

				hide();
				if(new_bouquet_id < 0)
					return 0;
				new_bouquet_id = g_bouquetManager->existsBouquet(bouquetList->Bouquets[new_bouquet_id]->channelList->getName());
				if ((new_bouquet_id == -1) || (new_bouquet_id == old_bouquet_id))
					return 0;

				if(!g_bouquetManager->existsChannelInBouquet(new_bouquet_id, channel_id)) 
				{
					addChannelToBouquet(new_bouquet_id, channel_id);
				}
				
				if(g_bouquetManager->existsChannelInBouquet(old_bouquet_id, channel_id)) 
				{
					g_bouquetManager->Bouquets[old_bouquet_id]->removeService(channel_id);
				}
				return 1;

				break;
				
			case 2: // add to
				do {
					bouquet_id = bouquetList->exec(false);
				} while(bouquet_id == -3);
				
				hide();
				
				if(bouquet_id < 0)
					return 0;
				
				bouquet_id = g_bouquetManager->existsBouquet(bouquetList->Bouquets[bouquet_id]->channelList->getName());
				if (bouquet_id == -1)
					return 0;
				
				if(!g_bouquetManager->existsChannelInBouquet(bouquet_id, channel_id)) 
				{
					addChannelToBouquet(bouquet_id, channel_id);
					return 1;
				}
				break;
				
			case 3: // add to my favorites
				bouquet_id = g_bouquetManager->existsUBouquet(g_Locale->getText(LOCALE_FAVORITES_BOUQUETNAME), true);
				if(bouquet_id == -1) 
				{
					g_bouquetManager->addBouquet(g_Locale->getText(LOCALE_FAVORITES_BOUQUETNAME), true);
					bouquet_id = g_bouquetManager->existsUBouquet(g_Locale->getText(LOCALE_FAVORITES_BOUQUETNAME), true);
				}
				
				if(!g_bouquetManager->existsChannelInBouquet(bouquet_id, channel_id)) 
				{
					addChannelToBouquet(bouquet_id, channel_id);
					return 1;
				}
				
				break;
				
			default:
				break;
		}
	}
	return 0;
}

int CChannelList::exec()
{
  	displayNext = 0; // always start with current events
	int nNewChannel = show();
	
	if ( nNewChannel > -1) 
	{
#if 0
		if(this != CNeutrinoApp::getInstance ()->channelList)
			CNeutrinoApp::getInstance ()->channelList->adjustToChannelID(chanlist[nNewChannel]->channel_id, false);
		zapTo(nNewChannel);
#endif
		//channelList->zapTo(bouquetList->Bouquets[bouquetList->getActiveBouquetNumber()]->channelList->getKey(nNewChannel)-1);
		CNeutrinoApp::getInstance ()->channelList->zapTo(getKey(nNewChannel)-1);
	}

	return nNewChannel;
}

#define CHANNEL_SMSKEY_TIMEOUT 800
/* return: >= 0 to zap, -1 on cancel, -3 on list mode change, -4 list edited, -2 zap but no restore old list/chan ?? */ //TODO:add return value for pip
int CChannelList::show()
{
	neutrino_msg_t      msg;
	neutrino_msg_data_t data;
	bool actzap = 0;
	int res = -1;

	new_mode_active = 0;
	
	int  fw = g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getWidth();
	width  = w_max (((g_settings.channellist_extended)?(frameBuffer->getScreenWidth() / 20 * (fw+6)):(frameBuffer->getScreenWidth() / 20 * (fw+5))), 100);
	height = h_max ((frameBuffer->getScreenHeight() / 20 * 16), (frameBuffer->getScreenHeight() / 20 * 2));

	if (chanlist.empty()) 
	{
		return res;
	}

	// display channame in vfd	
	CVFD::getInstance()->setMode(CVFD::MODE_MENU_UTF8 );	

	buttonHeight = 7 + std::min(16, g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL]->getHeight());
	theight = g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->getHeight();

	if(theight < PIC_H) 
		theight = PIC_H;

	fheight = g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST]->getHeight();
	listmaxshow = (height - theight - buttonHeight -0)/fheight;
	height = theight + buttonHeight + listmaxshow * fheight;
	info_height = fheight + g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST_DESCR]->getHeight() + 10;
	
	x = frameBuffer->getScreenX() + (frameBuffer->getScreenWidth() - width) / 2;
	y = frameBuffer->getScreenY() + (frameBuffer->getScreenHeight() - (height+ info_height)) / 2;

	displayNext = false;

	// head
	paintHead();
	
	// update events
	updateEvents();
	
	// paint all
	paint();
	
#ifdef FB_BLIT	
	frameBuffer->blit();	//26: for itemdetaillines
#endif	

	int oldselected = selected;
	int zapOnExit = false;
	bool bShowBouquetList = false;

	// loop control
	unsigned long long timeoutEnd = CRCInput::calcTimeoutEnd(g_settings.timing[SNeutrinoSettings::TIMING_CHANLIST]);

	bool loop=true;
	while (loop) 
	{
		g_RCInput->getMsgAbsoluteTimeout(&msg, &data, &timeoutEnd );
		
		if ( msg <= CRCInput::RC_MaxRC )
			timeoutEnd = CRCInput::calcTimeoutEnd(g_settings.timing[SNeutrinoSettings::TIMING_CHANLIST]);

		if ( ( msg == CRCInput::RC_timeout ) || ( msg == (neutrino_msg_t)g_settings.key_channelList_cancel) || msg == CRCInput::RC_vfdexit) 
		{
			if(!actzap)
				selected = oldselected;
			
			loop = false;
			res = -1;
		}
		else if ((msg == CRCInput::RC_red) || (msg == CRCInput::RC_epg)) 
		{
			hide();

			if ( g_EventList->exec(chanlist[selected]->channel_id, chanlist[selected]->name) == menu_return::RETURN_EXIT_ALL) 
			{
				res = -2;
				loop = false;
			}
			
			paintHead();
			paint();
		}
		else if ( msg == CRCInput::/*RC_blue*/RC_yellow && ( bouquetList != NULL ) ) //bouquets
		{ 
			//FIXME: show bqt list
			bShowBouquetList = true;
			loop=false;
		}
		else if( msg == CRCInput::/*RC_green*/RC_blue ) //epgplus
		{
			CEPGplusHandler * tmpEPGplusHandler = NULL;
			tmpEPGplusHandler = new CEPGplusHandler();
			
			tmpEPGplusHandler->exec(NULL, "");
			
			if(tmpEPGplusHandler)
				delete tmpEPGplusHandler;
			
			loop = false;
		}
		else if (msg == CRCInput::RC_sat || msg == CRCInput::RC_favorites)
		{
			g_RCInput->postMsg (msg, 0);
			loop = false;
			res = -1;
		}
		else if ( msg == CRCInput::RC_setup || msg == CRCInput::RC_vfdmenu) 
		{
			// chan list setup (add/move)
			old_b_id = bouquetList->getActiveBouquetNumber();
			int ret = doChannelMenu();
			
			if(ret) 
			{
				res = -4;
				loop = false;
			} 
			else 
			{
				old_b_id = -1;
				paintHead();
				paint();
			}
			
		}
		else if (msg == (neutrino_msg_t) g_settings.key_list_start) 
		{
			selected=0;
			liststart = (selected/listmaxshow)*listmaxshow;
			paint();
			
			if(new_mode_active) 
			{ 
				actzap = true; 
				zapTo(selected); 
			}
		}
		else if (msg == (neutrino_msg_t) g_settings.key_list_end) 
		{
			selected=chanlist.size()-1;
			liststart = (selected/listmaxshow)*listmaxshow;
			paint();
			
			if(new_mode_active) 
			{ 
				actzap = true; 
				zapTo(selected); 
			}
		}
                else if (msg == CRCInput::RC_up || (int) msg == g_settings.key_channelList_pageup  || msg == CRCInput::RC_vfdup)
                {
                        int step = 0;
                        int prev_selected = selected;

                        step =  ((int) msg == g_settings.key_channelList_pageup) ? listmaxshow : 1;  // browse or step 1
                        selected -= step;
                        if((prev_selected-step) < 0)            // because of uint
                                selected = chanlist.size() - 1;

                        paintItem(prev_selected - liststart);
                        unsigned int oldliststart = liststart;
                        liststart = (selected/listmaxshow)*listmaxshow;
                        if(oldliststart!=liststart)
                                paint();
                        else
                                paintItem(selected - liststart);

			if(new_mode_active) 
			{ 
				actzap = true; 
				zapTo(selected); 
			}
			
                        //paintHead();
                }
                else if (msg == CRCInput::RC_down || (int) msg == g_settings.key_channelList_pagedown  || msg == CRCInput::RC_vfddown)
                {
                        unsigned int step = 0;
                        int prev_selected = selected;

                        step =  ((int) msg == g_settings.key_channelList_pagedown) ? listmaxshow : 1;  // browse or step 1
                        selected += step;

                        if(selected >= chanlist.size()) 
			{
                                if (((chanlist.size() / listmaxshow) + 1) * listmaxshow == chanlist.size() + listmaxshow) // last page has full entries
                                        selected = 0;
                                else
                                        selected = ((step == listmaxshow) && (selected < (((chanlist.size() / listmaxshow)+1) * listmaxshow))) ? (chanlist.size() - 1) : 0;
			}

                        paintItem(prev_selected - liststart);
                        unsigned int oldliststart = liststart;
                        liststart = (selected/listmaxshow)*listmaxshow;
                        if(oldliststart!=liststart)
                                paint();
                        else
                                paintItem(selected - liststart);

			if(new_mode_active) 
			{ 
				actzap = true; 
				zapTo(selected); 
			}
			
                        //paintHead();
                }
		else if ((msg == (neutrino_msg_t)g_settings.key_bouquet_up) && (bouquetList != NULL)) 
		{
			if (bouquetList->Bouquets.size() > 0) 
			{
				bool found = true;
				uint32_t nNext = (bouquetList->getActiveBouquetNumber()+1) % bouquetList->Bouquets.size();
				if(bouquetList->Bouquets[nNext]->channelList->getSize() <= 0) 
				{
					found = false;
					nNext = nNext < bouquetList->Bouquets.size()-1 ? nNext+1 : 0;
					for(uint32_t i = nNext; i < bouquetList->Bouquets.size(); i++) 
					{
						if(bouquetList->Bouquets[i]->channelList->getSize() > 0) 
						{
							found = true;
							nNext = i;
							break;
						}
					}
				}
				
				if(found) 
				{
					bouquetList->activateBouquet(nNext, false);
					res = bouquetList->showChannelList();
					loop = false;
				}
			}
		}
		else if ((msg == (neutrino_msg_t)g_settings.key_bouquet_down) && (bouquetList != NULL)) 
		{
			if (bouquetList->Bouquets.size() > 0) 
			{
				bool found = true;
				int nNext = (bouquetList->getActiveBouquetNumber()+bouquetList->Bouquets.size()-1) % bouquetList->Bouquets.size();
				if(bouquetList->Bouquets[nNext]->channelList->getSize() <= 0) 
				{
					found = false;
					nNext = nNext > 0 ? nNext-1 : bouquetList->Bouquets.size()-1;
					
					for(int i = nNext; i > 0; i--) 
					{
						if(bouquetList->Bouquets[i]->channelList->getSize() > 0) 
						{
							found = true;
							nNext = i;
							break;
						}
					}
				}
				
				if(found) 
				{
					bouquetList->activateBouquet(nNext, false);
					res = bouquetList->showChannelList();
					loop = false;
				}
			}
		}
		else if ( msg == CRCInput::RC_ok  || msg == CRCInput::RC_vfdok) 
		{
			//TEST
			#if 0
			if(pip_selected)
			{
				printf("Channellist::show: PiP testing\n");
				//pipzap = true;
				/* draw pip TV background */
				frameBuffer->paintBoxRel(360, 144, 360, 288, COL_BACKGROUND);
				//frameBuffer->paintBackgroundBoxRel(360, 144, 360, 288);
				frameBuffer->blit();
			}
			#endif
			  
			zapOnExit = true;
			
			loop=false;
		}
		else if ( msg == CRCInput::RC_spkr ) 
		{
			new_mode_active = (new_mode_active ? 0 : 1);
			paintHead();
			//TEST: repaint logo
			// refresh logo box
			frameBuffer->paintBoxRel(x + width - 90 - PIC_W, y+(theight-PIC_H)/2, PIC_W, PIC_H, COL_MENUHEAD_PLUS_0);
	
			// paint logo
			g_PicViewer->DisplayLogo(chanlist[selected]->channel_id, x + width - 90 - PIC_W, y+(theight-PIC_H)/2, PIC_W, PIC_H);
		}
		else if (CRCInput::isNumeric(msg) && ( /*this->historyMode ||*/ g_settings.sms_channel)) 
		{
			/* 
			//FIXME:disabled because when getting number bigger as the historymode size it crashes
			if (this->historyMode) 
			{ 
				//numeric zap
			      	switch (msg) 
				{
				        case CRCInput::RC_0:
						selected = 0;
						break;
						
				        case CRCInput::RC_1:
						selected = 1;
						break;
						
				        case CRCInput::RC_2:
						selected = 2;
						break;
						
				        case CRCInput::RC_3:
						selected = 3;
						break;
						
				        case CRCInput::RC_4:
						selected = 4;
						break;
						
				        case CRCInput::RC_5:
						selected = 5;
						break;
						
				        case CRCInput::RC_6:
						selected = 6;
						break;
						
				        case CRCInput::RC_7:
						selected = 7;
						break;
						
				        case CRCInput::RC_8:
						selected = 8;
						break;
						
				        case CRCInput::RC_9:
						selected = 9;
						break;
		        	}
		        	
		      		zapOnExit = true;
		      		loop = false;
    			}
			else 
			*/
			if(g_settings.sms_channel) 
			{
				uint32_t i;
				unsigned char smsKey = 0;
				c_SMSKeyInput->setTimeout(CHANNEL_SMSKEY_TIMEOUT);

				do {
					smsKey = c_SMSKeyInput->handleMsg(msg);
					//printf("SMS new key: %c\n", smsKey);
					g_RCInput->getMsg_ms(&msg, &data, CHANNEL_SMSKEY_TIMEOUT-100);
				} while ((msg >= CRCInput::RC_1) && (msg <= CRCInput::RC_9));

				if (msg == CRCInput::RC_timeout || msg == CRCInput::RC_nokey) 
				{
					for(i = selected+1; i < chanlist.size(); i++) 
					{
						char firstCharOfTitle = chanlist[i]->name.c_str()[0];
						if(tolower(firstCharOfTitle) == smsKey) 
						{
							//printf("SMS chan found was= %d selected= %d i= %d %s\n", was_sms, selected, i, chanlist[i]->channel->name.c_str());
							break;
						}
					}
					
					if(i >= chanlist.size()) 
					{
						for(i = 0; i < chanlist.size(); i++) 
						{
							char firstCharOfTitle = chanlist[i]->name.c_str()[0];
							if(tolower(firstCharOfTitle) == smsKey) 
							{
								//printf("SMS chan found was= %d selected= %d i= %d %s\n", was_sms, selected, i, chanlist[i]->channel->name.c_str());
								break;
							}
						}
					}
					
					if(i < chanlist.size()) 
					{
						int prevselected=selected;
						selected=i;

						paintItem(prevselected - liststart);
						unsigned int oldliststart = liststart;
						liststart = (selected/listmaxshow)*listmaxshow;
						if(oldliststart!=liststart) 
						{
							paint();
						} 
						else 
						{
							paintItem(selected - liststart);
						}
					}
					c_SMSKeyInput->resetOldKey();
				}
			}
		}
		else if(CRCInput::isNumeric(msg)) 
		{
			//pushback key if...
			selected = oldselected;
			g_RCInput->postMsg( msg, data );
			loop=false;
		}
		else if ( msg == CRCInput::/*RC_yellow*/RC_green ) //next
		{
			displayNext = !displayNext;
			paintHead(); 		// update button bar
			updateEvents();
			paint();
		} 
		else if ( (msg == CRCInput::RC_info) )
		{
			hide();
			g_EpgData->show(chanlist[selected]->channel_id); 
			paintHead();
			paint();
		} 
		else 
		{
			if ( CNeutrinoApp::getInstance()->handleMsg( msg, data ) & messages_return::cancel_all ) 
			{
				loop = false;
				res = - 2;
			}
		}
		
#ifdef FB_BLIT		
		frameBuffer->blit();	//26: for itemdetaillines
#endif		
	}
	
	hide();
	
	if (bShowBouquetList) 
	{
		res = bouquetList->exec(true);
		printf("CChannelList:: bouquetList->exec res %d\n", res);
	}
	
	CVFD::getInstance()->setMode(CVFD::MODE_TVRADIO);
	
	new_mode_active = 0;

	if(NeutrinoMessages::mode_ts == CNeutrinoApp::getInstance()->getMode())
		return -1;

	if(zapOnExit)
		res = selected;

	printf("CChannelList::show res %d\n", res);
			
	return(res);
}

void CChannelList::hide()
{
	frameBuffer->paintBackgroundBoxRel(x, y, width + 5, height + info_height + 5);
	
#ifdef FB_BLIT	
	frameBuffer->blit();
#endif	
        clearItem2DetailsLine();	
}

bool CChannelList::showInfo(int pos, int epgpos)
{
	if((pos >= (signed int) chanlist.size()) || (pos<0))
		return false;

	CZapitChannel * chan = chanlist[pos];
	
	// channel infobar
	g_InfoViewer->showTitle(pos+1, chan->name, chan->getSatellitePosition(), chan->channel_id, true, epgpos); // UTF-8
	
	return true;
}

int CChannelList::handleMsg(const neutrino_msg_t msg, neutrino_msg_data_t data)
{
	if ( msg == NeutrinoMessages::EVT_PROGRAMLOCKSTATUS) 
	{
		// 0x100 als FSK-Status zeigt an, dass (noch) kein EPG zu einem Kanal der NICHT angezeigt
		// werden sollte (vorgesperrt) da ist
		// oder das bouquet des Kanals ist vorgesperrt

		printf("CChannelList::handleMsg: program-lock-status: %d\n", data);

		if ((g_settings.parentallock_prompt == PARENTALLOCK_PROMPT_ONSIGNAL) || (g_settings.parentallock_prompt == PARENTALLOCK_PROMPT_CHANGETOLOCKED))
		{
			if ( zapProtection != NULL )
				zapProtection->fsk = data;
			else 
			{
				// require password if either
				// CHANGETOLOCK mode and channel/bouquet is pre locked (0x100)
				// ONSIGNAL mode and fsk(data) is beyond configured value
				// if programm has already been unlocked, dont require pin
#if 1
				if ((data >= (neutrino_msg_data_t)g_settings.parentallock_lockage) &&
					 ((chanlist[selected]->last_unlocked_EPGid != g_RemoteControl->current_EPGid) || (g_RemoteControl->current_EPGid == 0)) &&
					 ((g_settings.parentallock_prompt != PARENTALLOCK_PROMPT_CHANGETOLOCKED) || (data >= 0x100)))
				{
					g_RemoteControl->stopvideo();
					
					zapProtection = new CZapProtection( g_settings.parentallock_pincode, data );
					
					if ( zapProtection->check() )
					{
						g_RemoteControl->startvideo();
						
						// remember it for the next time
						chanlist[selected]->last_unlocked_EPGid= g_RemoteControl->current_EPGid;
					}
					delete zapProtection;
					zapProtection = NULL;
				}
				else
					g_RemoteControl->startvideo();
#endif
			}
		}
		else
			g_RemoteControl->startvideo();

		return messages_return::handled;
	}
	else
		return messages_return::unhandled;
}

/* bToo default to true */
bool CChannelList::adjustToChannelID(const t_channel_id channel_id, bool bToo)
{
	unsigned int i;

	//printf("CChannelList::adjustToChannelID me %x list size %d channel_id %llx\n", (int) this, chanlist.size(), channel_id);fflush(stdout);
	for (i = 0; i < chanlist.size(); i++) 
	{
		if(chanlist[i] == NULL) 
		{
			//printf("CChannelList::adjustToChannelID REPORT BUG !! %d is NULL !!\n", i);
			continue;
		}

		if (chanlist[i]->channel_id == channel_id) 
		{
			selected = i;
			lastChList.store (selected, channel_id, false);

			tuned = i;
			if (bToo && (bouquetList != NULL)) 
			{
				//bouquetList->adjustToChannel( getActiveChannelNumber());
				//bouquetList->adjustToChannelID(channel_id);
//FIXME
				if(CNeutrinoApp::getInstance()->getMode() == NeutrinoMessages::mode_tv) 
				{
					TVbouquetList->adjustToChannelID(channel_id);
					TVsatList->adjustToChannelID(channel_id);
					TVfavList->adjustToChannelID(channel_id);
					TVallList->adjustToChannelID(channel_id);
				} 
				else if(CNeutrinoApp::getInstance()->getMode() == NeutrinoMessages::mode_radio) 
				{
					RADIObouquetList->adjustToChannelID(channel_id);
					RADIOsatList->adjustToChannelID(channel_id);
					RADIOfavList->adjustToChannelID(channel_id);
					RADIOallList->adjustToChannelID(channel_id);
				}
			}
			//printf("CChannelList::adjustToChannelID me %x to %llx bToo %s OK: %d\n", (int) this, channel_id, bToo ? "yes" : "no", i);fflush(stdout);
			return true;
		}
	}
	
	//printf("CChannelList::adjustToChannelID me %x to %llx bToo %s FAILED\n", (int) this, channel_id, bToo ? "yes" : "no");fflush(stdout);

	return false;
}

int CChannelList::hasChannel(int nChannelNr)
{
	for (uint32_t i=0;i<chanlist.size();i++) 
	{
		if (getKey(i) == nChannelNr)
			return(i);
	}
	return(-1);
}

int CChannelList::hasChannelID(t_channel_id channel_id)
{
	for (uint32_t i=0;i<chanlist.size();i++) 
	{
		if (chanlist[i]->channel_id == channel_id)
			return i;
	}
	return -1;
}

// for adjusting bouquet's channel list after numzap or quickzap
void CChannelList::setSelected( int nChannelNr)
{
	selected = nChannelNr;
}

// -- Zap to channel with channel_id
bool CChannelList::zapTo_ChannelID(const t_channel_id channel_id)
{
	printf("CChannelList::zapTo_ChannelID %llx\n", channel_id);
	
	for (unsigned int i=0; i<chanlist.size(); i++) 
	{
		if (chanlist[i]->channel_id == channel_id) 
		{
			zapTo (i);
			return true;
		}
	}
	return false;
}

// forceStoreToLastChannels defaults to false
void CChannelList::zapTo(int pos, bool forceStoreToLastChannels)
{
	if (chanlist.empty()) 
	{
		DisplayErrorMessage(g_Locale->getText(LOCALE_CHANNELLIST_NONEFOUND)); // UTF-8
		return;
	}

	if ( (pos >= (signed int) chanlist.size()) || (pos< 0) ) 
	{
		pos = 0;
	}
	
	//TEST
	/* we record when we switched away from a channel, so that the parental-PIN code can
	   check for timeout. last_unlocked_time == 0 means: the PIN was not entered
	   "tuned" is the *old* channel, before zap 
	*/
	//if (tuned < chanlist.size() && chanlist[tuned]->last_unlocked_time != 0)
		//chanlist[tuned]->last_unlocked_time = time_monotonic();

	CZapitChannel * chan = chanlist[pos];
	
	printf("CChannelList::zapTo me %x %s tuned %d new %d %s -> %llx\n", (int) this, name.c_str(), tuned, pos, chan->name.c_str(), chan->channel_id);
	
	if ( pos !=(int)tuned ) 
	{
		if ((g_settings.radiotext_enable) && ((CNeutrinoApp::getInstance()->getMode()) == NeutrinoMessages::mode_radio) && (g_Radiotext))
		{
			// stop radiotext PES decoding before zapping
			g_Radiotext->radiotext_stop();
		}
		
		tuned = pos;
		g_RemoteControl->zapTo_ChannelID(chan->channel_id, chan->name, !chan->bAlwaysLocked); // UTF-8
		
		// TODO check is it possible bouquetList is NULL ?
		if (bouquetList != NULL) 
		{
			CNeutrinoApp::getInstance()->channelList->adjustToChannelID(chan->channel_id);
		}
		
		if(new_mode_active)
			selected = pos;
	}

	if(!new_mode_active) 
	{
		selected = pos;
#if 0
		/* TODO lastChList.store also called in adjustToChannelID, which is called
		   only from "whole" channel list. Why here too ? */
		lastChList.store (selected, chan->channel_id, forceStoreToLastChannels);
#endif
		/* remove recordModeActive from infobar */
		if(g_settings.auto_timeshift && !CNeutrinoApp::getInstance()->recordingstatus) 
		{
			g_InfoViewer->handleMsg(NeutrinoMessages::EVT_RECORDMODE, 0);
		}

		g_RCInput->postMsg( NeutrinoMessages::SHOW_INFOBAR, 0 );
	}
}

// -1: channellist not found
int CChannelList::numericZap(int key)
{
	neutrino_msg_t      msg;
	neutrino_msg_data_t data;

	int res = -1;

	if (chanlist.empty()) 
	{
		DisplayErrorMessage(g_Locale->getText(LOCALE_CHANNELLIST_NONEFOUND)); // UTF-8
		return res;
	}

	// -- quickzap "0" (recall) to last seen channel...
	if (key == g_settings.key_lastchannel) 
	{
		t_channel_id channel_id = lastChList.getlast(1);
		if(channel_id) 
		{
			lastChList.clear_storedelay (); // ignore store delay
			zapTo_ChannelID(channel_id);
		}
		
		return res;
	}

	if (key == g_settings.key_zaphistory) 
	{
		// recording status
		if(!autoshift && CNeutrinoApp::getInstance()->recordingstatus) 
		{
			CChannelList * orgList = bouquetList->orgChannelList;
			CChannelList * channelList = new CChannelList(g_Locale->getText(LOCALE_CHANNELLIST_CURRENT_TP), false, true);
			
			t_channel_id recid = rec_channel_id >> 16;
			
			// get channels from the same tp as recording channel
			for ( unsigned int i = 0 ; i < orgList->chanlist.size(); i++) 
			{
				if((orgList->chanlist[i]->channel_id >> 16) == recid) 
				{
					channelList->addChannel(orgList->chanlist[i]);
				}
			}

			if (channelList->getSize() != 0) 
			{
				channelList->adjustToChannelID(orgList->getActiveChannel_ChannelID(), false);
				
				this->frameBuffer->paintBackground();
#ifdef FB_BLIT
				this->frameBuffer->blit();
#endif
				res = channelList->exec();
#if 0
				int newChannel = channelList->show() ;

				if (newChannel > -1) { //FIXME handle edit/mode change ??
					orgList->zapTo_ChannelID(channelList->chanlist[newChannel]->channel_id);
				}
#endif
			}
			delete channelList;
			return res;
		}
		
		// -- zap history bouquet, similar to "0" quickzap, but shows a menue of last channels
		if (this->lastChList.size() > 1) 
		{
			CChannelList * channelList = new CChannelList(g_Locale->getText(LOCALE_CHANNELLIST_HISTORY), true, true);

			for(unsigned int i = 1 ; i < this->lastChList.size() ; ++i) 
			{
				t_channel_id channel_id = this->lastChList.getlast(i);

				if(channel_id) 
				{
					CZapitChannel * channel = getChannel(channel_id);
					if(channel) 
						channelList->addChannel(channel);
				}
			}

			if (channelList->getSize() != 0) 
			{
				this->frameBuffer->paintBackground();
#ifdef FB_BLIT
				this->frameBuffer->blit();
#endif
				//int newChannel = channelList->exec();
				res = channelList->exec();
			}
			delete channelList;
		}
		return res;
	}
	
	//TEST: PiP
	if( (key == g_settings.key_pip) || (key == g_settings.key_pip_subchannel) )
	{
		if( !CNeutrinoApp::getInstance()->pipstatus)  //not yet after choosing channel
		{
			CChannelList * orgList = bouquetList->orgChannelList;
			CChannelList * channelList = new CChannelList(g_Locale->getText(LOCALE_CHANNELLIST_CURRENT_TP), false, true);
			
			t_channel_id pipid = live_channel_id >> 16;
			
			for ( unsigned int i = 0 ; i < orgList->chanlist.size(); i++) 
			{
				if((orgList->chanlist[i]->channel_id >> 16) == pipid) 
				{
					channelList->addChannel(orgList->chanlist[i]);
				}
			}
			
			//TEST
			pip_selected = true;

			if (channelList->getSize() != 0) 
			{
				channelList->adjustToChannelID(orgList->getActiveChannel_ChannelID(), false);
				this->frameBuffer->paintBackground();
#ifdef FB_BLIT
				this->frameBuffer->blit();
#endif
				res = channelList->exec();
#if 0
				int newChannel = channelList->show() ;

				if (newChannel > -1) { //FIXME handle edit/mode change ??
					orgList->zapTo_ChannelID(channelList->chanlist[newChannel]->channel_id);
				}
#endif
			}
			delete channelList;
			return res;
		}
	}
	//

	int sx = 4 * g_Font[SNeutrinoSettings::FONT_TYPE_CHANNEL_NUM_ZAP]->getRenderWidth(widest_number) + 14;
	int sy = g_Font[SNeutrinoSettings::FONT_TYPE_CHANNEL_NUM_ZAP]->getHeight() + 6;

	int ox = frameBuffer->getScreenX() + (frameBuffer->getScreenWidth() - sx)/2;
	int oy = frameBuffer->getScreenY() + (frameBuffer->getScreenHeight() - sy)/2;
	char valstr[10];
	int chn = CRCInput::getNumericValue(key);
	int pos = 1;
	int lastchan= -1;
	bool doZap = true;
	bool showEPG = false;

	while(1) 
	{
		if (lastchan != chn) 
		{
			sprintf((char*) &valstr, "%d", chn);
			
			while(strlen(valstr)<4)
				strcat(valstr,"-");   //"_"

			frameBuffer->paintBoxRel(ox, oy, sx, sy, COL_INFOBAR_PLUS_0);

			for (int i=3; i>=0; i--) 
			{
				valstr[i+ 1]= 0;
				g_Font[SNeutrinoSettings::FONT_TYPE_CHANNEL_NUM_ZAP]->RenderString(ox+7+ i*((sx-14)>>2), oy+sy-3, sx, &valstr[i], COL_INFOBAR);
			}
			
#ifdef FB_BLIT
			frameBuffer->blit();
#endif

			//test
			showInfo(chn - 1);
			lastchan = chn;
		}

		g_RCInput->getMsg( &msg, &data, g_settings.timing[SNeutrinoSettings::TIMING_NUMERICZAP] * 10 );

		if ( msg == CRCInput::RC_timeout ) 
		{
			if ( ( chn > (int)chanlist.size() ) || (chn == 0) )
				chn = tuned + 1;
			break;
		}
		else if ( msg == CRCInput::RC_favorites || msg == CRCInput::RC_sat)
		{
		}
		else if (CRCInput::isNumeric(msg)) 
		{
			if (pos == 4) 
			{
				chn = 0;
				pos = 1;
			} 
			else 
			{
				chn *= 10;
				pos++;
			}
			chn += CRCInput::getNumericValue(msg);
		}
		else if ( msg == CRCInput::RC_ok  || msg == CRCInput::RC_vfdok) 
		{
			if ( ( chn > (signed int) chanlist.size() ) || ( chn == 0 ) ) 
			{
				chn = tuned + 1;
			}
			break;
		}
		else if ( msg == (neutrino_msg_t)g_settings.key_quickzap_down ) 
		{
			if ( chn == 1 )
				chn = chanlist.size();
			else {
				chn--;

				if (chn > (int)chanlist.size())
					chn = (int)chanlist.size();
			}
		}
		else if ( msg == (neutrino_msg_t)g_settings.key_quickzap_up ) 
		{
			chn++;

			if (chn > (int)chanlist.size())
				chn = 1;
		}
		else if ( ( msg == CRCInput::RC_home ) || ( msg == CRCInput::RC_left ) || ( msg == CRCInput::RC_right)  || (msg == CRCInput::RC_vfdexit) || (msg == CRCInput::RC_vfdleft) || (msg == CRCInput::RC_vfdright))
		{
			doZap = false;
			break;
		}
		else if ( msg == CRCInput::RC_red ) 
		{
			if ( ( chn <= (signed int) chanlist.size() ) && ( chn != 0 ) ) 
			{
				doZap = false;
				showEPG = true;
				break;
			}
		}
		else if ( CNeutrinoApp::getInstance()->handleMsg( msg, data ) & messages_return::cancel_all ) 
		{
			doZap = false;
			//res = menu_return::RETURN_EXIT_ALL;
			break;
		}
	}

	frameBuffer->paintBackgroundBoxRel(ox, oy, sx, sy);
	
#ifdef FB_BLIT
	frameBuffer->blit();
#endif

	chn--;
	
	if (chn < 0)
		chn = 0;
	
	if ( doZap ) 
	{
		if(g_settings.timing[SNeutrinoSettings::TIMING_INFOBAR] == 0)
			g_InfoViewer->killTitle();
		
		zapTo( chn );
	} 
	else 
	{
		//test
		showInfo(tuned);
		g_InfoViewer->killTitle();

		if ( showEPG )
			g_EventList->exec(chanlist[chn]->channel_id, chanlist[chn]->name);
	}
	
	return res;
}

void CChannelList::virtual_zap_mode(bool up)
{
        neutrino_msg_t      msg;
        neutrino_msg_data_t data;

        if (chanlist.empty()) 
	{
                DisplayErrorMessage(g_Locale->getText(LOCALE_CHANNELLIST_NONEFOUND)); // UTF-8
                return;
        }

        int chn = getActiveChannelNumber() + (up ? 1 : -1);
        if (chn > (int)chanlist.size())
		chn = 1;
        if (chn == 0)
		chn = (int)chanlist.size();
        int lastchan= -1;
        bool doZap = true;
        bool showEPG = false;
	int epgpos = 0;

        while(1)
        {
                if (lastchan != chn || (epgpos != 0))
                {
                        showInfo(chn- 1, epgpos);
                        lastchan= chn;
                }
		epgpos = 0;
                g_RCInput->getMsg( &msg, &data, 15*10 ); // 15 seconds, not user changable
		
                //printf("%u %u %u\n", msg, NeutrinoMessages::EVT_TIMER, CRCInput::RC_timeout);

                if ( msg == CRCInput::RC_ok || msg == CRCInput::RC_vfdok)
                {
                        if ( ( chn > (signed int) chanlist.size() ) || ( chn == 0 ) )
                        {
                                chn = tuned + 1;
                        }
                        break;
                }
                else if ( msg == CRCInput::RC_left || msg == CRCInput::RC_vfdleft)
                {
                        if ( chn == 1 )
                                chn = chanlist.size();
                        else
                        {
                                chn--;

                                if (chn > (int)chanlist.size())
                                        chn = (int)chanlist.size();
                        }
                }
                else if ( msg == CRCInput::RC_right || msg == CRCInput::RC_vfdright)
                {
                        chn++;

                        if (chn > (int)chanlist.size())
                                chn = 1;
                }
                else if ( msg == CRCInput::RC_up || msg == CRCInput::RC_vfdup)
                {
                        epgpos = -1;
                }
                else if ( msg == CRCInput::RC_down || msg == CRCInput::RC_vfddown)
                {
                        epgpos = 1;
                }
                else if ( ( msg == CRCInput::RC_home ) || ( msg == CRCInput::RC_timeout ) || (msg == CRCInput::RC_vfdexit) )
                {
                        // Abbruch ohne Channel zu wechseln
                        doZap = false;
                        break;
                }
                else if ( msg == CRCInput::RC_red )
                {
                        // Rote Taste zeigt EPG fuer gewaehlten Kanal an
                        if ( ( chn <= (signed int) chanlist.size() ) && ( chn != 0 ) )
                        {
                                doZap = false;
                                showEPG = true;
                                break;
                        }
                }
                else if ( CNeutrinoApp::getInstance()->handleMsg( msg, data ) & messages_return::cancel_all )
                {
                        doZap = false;
                        break;
                }
        }
        
	g_InfoViewer->clearVirtualZapMode();

        chn--;
        if (chn<0)
                chn=0;
        if ( doZap )
        {
		if(g_settings.timing[SNeutrinoSettings::TIMING_INFOBAR] == 0)
			g_InfoViewer->killTitle();
                zapTo( chn );
        }
        else
        {
                showInfo(tuned);
                g_InfoViewer->killTitle();

                // Rote Taste zeigt EPG fuer gewaehlten Kanal an
                if ( showEPG )
                        g_EventList->exec(chanlist[chn]->channel_id, chanlist[chn]->name);
        }
}

void CChannelList::quickZap(int key, bool cycle)
{
        if(chanlist.size() == 0)
                return;

	if ( (key == g_settings.key_quickzap_down) || (key == CRCInput::RC_left) || (key == CRCInput::RC_vfddown))
	{
                if(selected==0)
                        selected = chanlist.size()-1;
                else
                        selected--;
        }
	else if ((key == g_settings.key_quickzap_up) || (key == CRCInput::RC_right) || (key == CRCInput::RC_vfdup))
	{
                selected = (selected+1)%chanlist.size();
        }

	printf("[neutrino] quick zap selected = %d bouquetList %x getActiveBouquetNumber %d orgChannelList %x\n", selected, bouquetList, bouquetList->getActiveBouquetNumber(), bouquetList->orgChannelList);

	if(cycle)
		bouquetList->orgChannelList->zapTo(bouquetList->Bouquets[bouquetList->getActiveBouquetNumber()]->channelList->getKey(selected)-1);
	else
        	zapTo( selected );

	g_RCInput->clearRCMsg(); //FIXME test for n.103
}

void CChannelList::paintDetails(int index)
{
	CChannelEvent *p_event;

	if (displayNext) 
	{
		p_event = &chanlist[index]->nextEvent;
	} 
	else 
	{
		p_event = &chanlist[index]->currentEvent;
	}

	// itembox refresh
	frameBuffer->paintBoxRel(x + 2, y + height + 2, width - 4, info_height - 4, COL_MENUCONTENTDARK_PLUS_0);

	if (!p_event->description.empty()) 
	{
		char cNoch[50]; // UTF-8
		char cSeit[50]; // UTF-8

		struct tm *pStartZeit = localtime(&p_event->startTime);
		unsigned 	seit = ( time(NULL) - p_event->startTime ) / 60;

		if (displayNext) 
		{
			sprintf(cNoch, "(%d min)", p_event->duration / 60);
			sprintf(cSeit, g_Locale->getText(LOCALE_CHANNELLIST_START), pStartZeit->tm_hour, pStartZeit->tm_min);
		} 
		else 
		{
			sprintf(cSeit, g_Locale->getText(LOCALE_CHANNELLIST_SINCE), pStartZeit->tm_hour, pStartZeit->tm_min);
			int noch = (p_event->startTime + p_event->duration - time(NULL)) / 60;
			if ((noch< 0) || (noch>=10000))
				noch= 0;
			sprintf(cNoch, "(%d / %d min)", seit, noch);
		}
		int seit_len = g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST_DESCR]->getRenderWidth(cSeit, true); // UTF-8
		int noch_len = g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST_NUMBER]->getRenderWidth(cNoch, true); // UTF-8

		std::string text1= p_event->description;
		std::string text2= p_event->text;

		int xstart = 10;
		if (g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST]->getRenderWidth(text1, true) > (width - 30 - seit_len) )
		{
			// zu breit, Umbruch versuchen...
			int pos;
			do 
			{
				pos = text1.find_last_of("[ -.]+");
				if ( pos!=-1 )
					text1 = text1.substr( 0, pos );
			} while ( ( pos != -1 ) && (g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST]->getRenderWidth(text1, true) > (width - 30 - seit_len) ) );

			std::string text3 = p_event->description.substr(text1.length()+ 1);

			if (!(text2.empty()))
				text3= text3+ " - ";

			xstart += g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST]->getRenderWidth(text3, true);
			g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST]->RenderString(x+ 10, y+ height+ 5+ 2* fheight, width - 30- noch_len, text3, COL_MENUCONTENTDARK, 0, true);
		}

		if (!(text2.empty())) 
		{
			while ( text2.find_first_of("[ -.+*#?=!$%&/]+") == 0 )
				text2 = text2.substr( 1 );

			text2 = text2.substr( 0, text2.find('\n') );
			g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST_DESCR]->RenderString(x+ xstart, y+ height+ 5+ 2* fheight, width- xstart- 20- noch_len, text2, COL_MENUCONTENTDARK, 0, true);
		}

		g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST]->RenderString(x+ 10, y+ height+ 5+ fheight, width - 30 - seit_len, text1, COL_MENUCONTENTDARK, 0, true);

		g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST_DESCR]->RenderString (x+ width- 10- seit_len, y+ height+ 5+    fheight   , seit_len, cSeit, COL_MENUCONTENTDARK, 0, true); // UTF-8

		g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST_NUMBER]->RenderString(x+ width- 10- noch_len, y+ height+ 5+ 2* fheight- 2, noch_len, cNoch, COL_MENUCONTENTDARK, 0, true); // UTF-8
	}
}

void CChannelList::clearItem2DetailsLine ()
{  
	  paintItem2DetailsLine(-1, 0);  
}

void CChannelList::paintItem2DetailsLine(int pos, int ch_index)
{
#define ConnectLineBox_Width	16

	int xpos  = x - ConnectLineBox_Width;
	int ypos1 = y + theight+0 + pos*fheight;
	int ypos2 = y + height;
	int ypos1a = ypos1 + (fheight/2)-2;
	int ypos2a = ypos2 + (info_height/2)-2;
	fb_pixel_t col1 = COL_MENUCONTENT_PLUS_6;
	fb_pixel_t col2 = COL_MENUCONTENT_PLUS_1;

	// Clear
	frameBuffer->paintBackgroundBoxRel(xpos - 10, y - 10, ConnectLineBox_Width + 10, height+info_height + 10);
#ifdef FB_BLIT
	frameBuffer->blit();
#endif

	// paint Line if detail info (and not valid list pos)
	if (pos >= 0) 
	{ 
		//pos >= 0 &&  chanlist[ch_index]->currentEvent.description != "") {
  		if(1) // FIXME why -> ? (!g_settings.channellist_extended)
		{
			int fh = fheight > 10 ? fheight - 10: 5;
			
			frameBuffer->paintBoxRel(xpos + ConnectLineBox_Width - 4, ypos1 + 5, 4, fh, col1);
			frameBuffer->paintBoxRel(xpos + ConnectLineBox_Width - 4, ypos1 + 5, 1, fh, col2);			

			frameBuffer->paintBoxRel(xpos+ConnectLineBox_Width - 4, ypos2 + 7, 4, info_height - 14, col1);
			frameBuffer->paintBoxRel(xpos+ConnectLineBox_Width - 4, ypos2 + 7, 1, info_height - 14, col2);			

			// vertical line
			frameBuffer->paintBoxRel(xpos + ConnectLineBox_Width - 16, ypos1a, 4, ypos2a - ypos1a, col1);
			frameBuffer->paintBoxRel(xpos + ConnectLineBox_Width - 16, ypos1a, 1, ypos2a - ypos1a + 4, col2);		

			// Hline Oben
			frameBuffer->paintBoxRel(xpos + ConnectLineBox_Width - 15, ypos1a, 12,4, col1);
			frameBuffer->paintBoxRel(xpos + ConnectLineBox_Width - 16, ypos1a, 12,1, col2);
		
			// Hline Unten
			frameBuffer->paintBoxRel(xpos + ConnectLineBox_Width - 15, ypos2a, 12, 4, col1);
			frameBuffer->paintBoxRel(xpos + ConnectLineBox_Width - 12, ypos2a, 8, 1, col2);

			// untere info box lines
			frameBuffer->paintBoxRel(x, ypos2, width, info_height, col1);
		}
	}
}

void CChannelList::paintItem(int pos)
{
	int ypos = y + theight + 0 + pos*fheight;
	uint8_t    color;
	fb_pixel_t bgcolor;
	bool iscurrent = true;
	unsigned int curr = liststart + pos;

	if(!autoshift && CNeutrinoApp::getInstance()->recordingstatus && curr < chanlist.size()) 
	{
		//iscurrent = (chanlist[curr]->channel_id >> 16) == (rec_channel_id >> 16);
		//if(FrontendCount == 1) // single tuner
		{
			if(chanlist[curr]->channel_id >> 16 == rec_channel_id >> 16)
				iscurrent = false;
		}
	
		printf("CChannelList::paintItem: recording %llx current %llx current = %s\n", rec_channel_id, chanlist[liststart + pos]->channel_id, iscurrent? "yes" : "no");
	}
	
	if (curr == selected) 
	{
		color   = COL_MENUCONTENTSELECTED;
		bgcolor = COL_MENUCONTENTSELECTED_PLUS_0;
		
		// itemlines	
		paintItem2DetailsLine(pos, curr);		
		
		// details
		paintDetails(curr);

		// refresh logo box
		frameBuffer->paintBoxRel(x + width - 90 - PIC_W, y + (theight-PIC_H)/2, PIC_W, PIC_H, COL_MENUHEAD_PLUS_0);
	
		// paint logo
		g_PicViewer->DisplayLogo(chanlist[selected]->channel_id, x + width - 90 - PIC_W, y+(theight-PIC_H)/2, PIC_W, PIC_H);

		// infobox
		frameBuffer->paintBoxRel(x, ypos, width- 15, fheight, bgcolor);
	} 
	else 
	{
		color = iscurrent ? COL_MENUCONTENT : COL_MENUCONTENTINACTIVE;
		bgcolor = iscurrent ? COL_MENUCONTENT_PLUS_0 : COL_MENUCONTENTINACTIVE_PLUS_0;
		
		// infobox
		frameBuffer->paintBoxRel(x, ypos, width - 15, fheight, bgcolor);
	}

	//name and description
	if(curr < chanlist.size()) 
	{
		char nameAndDescription[255];
		char tmp[10];
		CZapitChannel * chan = chanlist[curr];
		int prg_offset=0;
		int title_offset=0;
		uint8_t tcolor=(liststart + pos == selected) ? color : COL_MENUCONTENTINACTIVE;
		int xtheight=fheight-2;
		
		if(g_settings.channellist_extended)
		{
			prg_offset = 42;
			title_offset = 6;
		}

		sprintf((char*) tmp, "%d", this->historyMode ? pos : chan->number);

		CChannelEvent * p_event = NULL;

		if (displayNext) 
		{
			p_event = &chan->nextEvent;
		} 
		else 
		{
			p_event = &chan->currentEvent;
		}

		// hd/scrambled icons
		if (g_settings.channellist_ca)
		{
			// hd icon
			if(chan->isHD() ) 
				frameBuffer->paintIcon(NEUTRINO_ICON_RESOLUTION_HD, x + width - 15 - 28 -30, ypos + (fheight - 16)/2);
			
			// scrambled icon
			if(chan->scrambled) 
				frameBuffer->paintIcon(NEUTRINO_ICON_SCRAMBLED2, x + width - 15 - 28, ypos + (fheight - 16)/2);
		}

		int numpos = x + 5 + numwidth - g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST_NUMBER]->getRenderWidth(tmp);
		g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST_NUMBER]->RenderString(numpos, ypos + fheight, numwidth + 5, tmp, color, fheight);

		int l=0;
		if (this->historyMode)
			l = snprintf(nameAndDescription, sizeof(nameAndDescription), ": %d %s", chan->number, chan->name.c_str());
		else
			l = snprintf(nameAndDescription, sizeof(nameAndDescription), "%s", chan->name.c_str());

		if (!(p_event->description.empty())) 
		{
			snprintf(nameAndDescription+l, sizeof(nameAndDescription)-l," - ");
			unsigned int ch_name_len = g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST]->getRenderWidth(nameAndDescription, true);
			unsigned int ch_desc_len = g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST_DESCR]->getRenderWidth(p_event->description, true);

			if ( (width- numwidth- 60- 15- prg_offset - ch_name_len)< ch_desc_len )
				ch_desc_len = (width- numwidth- 60- 15 - 30 - ch_name_len - prg_offset); //30: hd icon width
			if (ch_desc_len< 0)
				ch_desc_len = 0;
			
			if(g_settings.channellist_extended)
			{
				if(displayNext)
				{
					struct tm *pStartZeit = localtime(&p_event->startTime);
			
					sprintf((char*) tmp, "%02d:%02d", pStartZeit->tm_hour, pStartZeit->tm_min);
					//g_Font[SNeutrinoSettings::FONT_TYPE_IMAGEINFO_SMALL]->RenderString(x+ 5+ numwidth+ 6, ypos+ xtheight, width- numwidth- 20- 15 -poffs, tmp, COL_MENUCONTENT, 0, true);
					g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST_NUMBER]->RenderString(x+ 5+ numwidth+ 6, ypos+ xtheight, width- numwidth- 20- 15 -prg_offset, tmp, tcolor, 0, true);
				}
				else
				{
					time_t jetzt=time(NULL);
					int runningPercent = 0;
					
					if (((jetzt - p_event->startTime + 30) / 60) < 0 )
					{
						runningPercent= 0;
					}
					else
					{
						runningPercent=(jetzt-p_event->startTime) * 30 / p_event->duration;
						if (runningPercent > 30)	// this would lead to negative value in paintBoxRel
							runningPercent = 30;	// later on which can be fatal...
					}
					frameBuffer->paintBoxRel(x+ 5+ numwidth+ title_offset, ypos+fheight/4, 34, fheight/2, COL_MENUCONTENT_PLUS_3, 0);//fill passive
					frameBuffer->paintBoxRel(x+ 5+ numwidth+ title_offset+2, ypos+2+fheight/4, 30, fheight/2-4, COL_MENUCONTENT_PLUS_1, 0);//frame(passive)
				
					frameBuffer->paintBoxRel(x+ 5+ numwidth+ title_offset+2, ypos+2+fheight/4, runningPercent, fheight/2-4, COL_MENUCONTENT_PLUS_3, 0);//fill(active)
				}
			}

			g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST]->RenderString(x+ 5+ numwidth+ 10+prg_offset, ypos+ fheight, width- numwidth- 40- 15-prg_offset, nameAndDescription, color, 0, true);

			if (g_settings.channellist_epgtext_align_right)
			{
				// align right
				g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST_DESCR]->RenderString(x + width - 20 - ch_desc_len - 28, ypos + fheight, ch_desc_len, p_event->description, (curr == selected)?COL_MENUCONTENTSELECTED:(!displayNext ? COL_MENUCONTENT : COL_MENUCONTENTINACTIVE) , 0, true);
			}
			else
			{
				// align left
				g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST_DESCR]->RenderString(x+ 5+ numwidth+ 10+ ch_name_len+ 5+prg_offset, ypos+ fheight, ch_desc_len, p_event->description, (curr == selected)?COL_MENUCONTENTSELECTED:(!displayNext ? COL_MENUCONTENT : COL_MENUCONTENTINACTIVE) , 0, true);
			}
		}
		else 
		{
			if(g_settings.channellist_extended)
			{
				short runningPercent=0;
				frameBuffer->paintBoxRel(x+ 5+ numwidth+ title_offset, ypos+fheight/4, 34, fheight/2, COL_MENUCONTENT_PLUS_3, 0);//fill passive
				frameBuffer->paintBoxRel(x+ 5+ numwidth+ title_offset+2, ypos+2+fheight/4, 30, fheight/2-4, COL_MENUCONTENT_PLUS_1, 0);//frame(passive)

				frameBuffer->paintBoxRel(x+ 5+ numwidth+ title_offset+2, ypos+2+fheight/4, runningPercent, fheight/2-4, COL_MENUCONTENT_PLUS_3, 0);//fill(active)
				
				frameBuffer->paintLine(x+ 5+ numwidth+ title_offset, ypos+fheight/4+1,x+ 5+ numwidth+ title_offset+ 32, ypos+fheight/4+fheight/2-3, COL_MENUCONTENT_PLUS_3);
			}
			//name
			g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST]->RenderString(x+ 5+ numwidth+ 10+prg_offset, ypos+ fheight, width- numwidth- 40- 15-prg_offset, nameAndDescription, color, 0, true); // UTF-8
		}

		// show channame/event info in vfd
#if 0
		if (curr == selected) 
		{
			if (!(chan->currentEvent.description.empty())) 
			{
#if defined (PLATFORM_CUBEREVO) || defined (PLATFORM_CUBEREVO_MINI) || defined (PLATFORM_CUBEREVO_MINI2) || defined (PLATFORM_CUBEREVO_MINI_FTA) || defined (PLATFORM_CUBEREVO_2000HD) || defined (PLATFORM_CUBEREVO_9500HD)			  
				snprintf(nameAndDescription, sizeof(nameAndDescription), "%s - %s", chan->name.c_str(), p_event->description.c_str());
#elif defined (PLATFORM_CUBEREVO_250HD) || defined (PLATFORM_GIGABLUE_800SE)
				snprintf(nameAndDescription, sizeof(nameAndDescription), "%04d", chan->number);
#endif				
			} 
			else
			{				
#if defined (PLATFORM_CUBEREVO) || defined (PLATFORM_CUBEREVO_MINI) || defined (PLATFORM_CUBEREVO_MINI2) || defined (PLATFORM_CUBEREVO_MINI_FTA) || defined (PLATFORM_CUBEREVO_2000HD) || defined (PLATFORM_CUBEREVO_9500HD)			  
				snprintf(nameAndDescription, sizeof(nameAndDescription), "%s", chan->name.c_str() );
#elif defined (PLATFORM_CUBEREVO_250HD) || defined (PLATFORM_GIGABLUE_800SE)
				snprintf(nameAndDescription, sizeof(nameAndDescription), "%04d", chan->number);
#endif
			}
			
			//CVFD::getInstance()->showMenuText(0, nameAndDescription, -1, true); // UTF-8
			CVFD::getInstance()->ShowText(nameAndDescription); // UTF-8
		}
#endif
	}
}

#define NUM_LIST_BUTTONS 4
struct button_label CChannelListButtons[NUM_LIST_BUTTONS] =
{
	{ NEUTRINO_ICON_BUTTON_RED, LOCALE_INFOVIEWER_EVENTLIST},
	{ NEUTRINO_ICON_BUTTON_GREEN, LOCALE_INFOVIEWER_NEXT},
	{ NEUTRINO_ICON_BUTTON_YELLOW, LOCALE_BOUQUETLIST_HEAD},
	{ NEUTRINO_ICON_BUTTON_BLUE, LOCALE_EPGMENU_EPGPLUS },
};

#define NUM_VLIST_BUTTONS 4
const struct button_label CChannelVListButtons[NUM_VLIST_BUTTONS] =
{
	{ NEUTRINO_ICON_BUTTON_RED, LOCALE_INFOVIEWER_EVENTLIST },
	{ NEUTRINO_ICON_BUTTON_GREEN, LOCALE_INFOVIEWER_NEXT },
	{ NEUTRINO_ICON_BUTTON_YELLOW, LOCALE_BOUQUETLIST_HEAD },
	{ NEUTRINO_ICON_BUTTON_BLUE, LOCALE_EPGMENU_EPGPLUS }
};

void CChannelList::paintHead()
{
	// head
	frameBuffer->paintBoxRel(x, y, width, theight, COL_MENUHEAD_PLUS_0, RADIUS_MID, CORNER_TOP);//round

	int ButtonWidth = (width - 20) / 4;

	// foot
	if (displayNext) 
	{
		CChannelListButtons[1].locale = LOCALE_INFOVIEWER_NOW;
	} 
	else 
	{
		CChannelListButtons[1].locale = LOCALE_INFOVIEWER_NEXT;
	}

	frameBuffer->paintBoxRel(x, y + (height - buttonHeight), width, buttonHeight - 1, COL_MENUHEAD_PLUS_0, RADIUS_MID, CORNER_BOTTOM); //round
	
	::paintButtons(frameBuffer, g_Font[SNeutrinoSettings::FONT_TYPE_INFOBAR_SMALL], g_Locale, x + 10, y + (height - buttonHeight) + 3, ButtonWidth, vlist ? NUM_VLIST_BUTTONS : NUM_LIST_BUTTONS, vlist ? CChannelVListButtons : CChannelListButtons);

	// help icon
	int icon_h_w, icon_h_h;
	frameBuffer->getIconSize(NEUTRINO_ICON_BUTTON_HELP, &icon_h_w, &icon_h_h);
	frameBuffer->paintIcon(NEUTRINO_ICON_BUTTON_HELP, x + width - 10 - icon_h_w , y + 5 );

	// setup icon
	int icon_s_w, icon_s_h;
	if (bouquetList != NULL)
	{
		frameBuffer->getIconSize(NEUTRINO_ICON_BUTTON_DBOX, &icon_s_w, &icon_s_h);
		frameBuffer->paintIcon(NEUTRINO_ICON_BUTTON_DBOX, x + width - 10 - icon_h_w - 2 - icon_s_w, y + 5); // icon for bouquet list button
	}

	// mute zap
	int icon_z_w, icon_z_h;
	frameBuffer->getIconSize(NEUTRINO_ICON_BUTTON_MUTE_ZAP_ACTIVE, &icon_z_w, &icon_z_h);
	frameBuffer->paintIcon(new_mode_active ? NEUTRINO_ICON_BUTTON_MUTE_ZAP_ACTIVE : NEUTRINO_ICON_BUTTON_MUTE_ZAP_INACTIVE, x + width - 10 - icon_h_w - 2 - icon_s_w - 2 - icon_z_w, y + 5);
	
	// paint time/date
	int timestr_len = 0;
	char timestr[18];
	
	time_t now = time(NULL);
	struct tm *tm = localtime(&now);
	
	bool gotTime = g_Sectionsd->getIsTimeSet();

	if(gotTime)
	{
		strftime(timestr, 18, "%d.%m.%Y %H:%M", tm);
		timestr_len = g_Font[SNeutrinoSettings::FONT_TYPE_EVENTLIST_ITEMLARGE]->getRenderWidth(timestr, true); // UTF-8
		
		g_Font[SNeutrinoSettings::FONT_TYPE_EVENTLIST_ITEMLARGE]->RenderString(x + width - 10 - icon_h_w - 2 - icon_s_w - 2 - icon_z_w - 2 - PIC_W - 5 - timestr_len, y + g_Font[SNeutrinoSettings::FONT_TYPE_EVENTLIST_ITEMLARGE]->getHeight() + 5, timestr_len+1, timestr, COL_MENUHEAD, 0, true); // UTF-8 // 100 is pic_w refresh box
	}
	
	//title
	g_Font[SNeutrinoSettings::FONT_TYPE_MENU_TITLE]->RenderString(x + 10, y + theight, width - 10 - icon_h_w - 2 - icon_s_w - 2 - icon_z_w - 2 - PIC_W - 5 - timestr_len, name, COL_MENUHEAD, 0, true); // UTF-8
}

void CChannelList::paint()
{
	liststart = (selected/listmaxshow)*listmaxshow;
	
	//FIXME do we need to find biggest chan number in list ?
	numwidth = g_Font[SNeutrinoSettings::FONT_TYPE_CHANNELLIST_NUMBER]->getRenderWidth("0000");
	
	//updateEvents();

	frameBuffer->paintBoxRel(x, y+theight, width, height-buttonHeight-theight, COL_MENUCONTENT_PLUS_0);

	for(unsigned int count = 0; count < listmaxshow; count++) 
	{
		paintItem(count);
	}

	int ypos = y+ theight;
	int sb = fheight* listmaxshow;
	
	frameBuffer->paintBoxRel(x+ width- 15,ypos, 15, sb,  COL_MENUCONTENT_PLUS_1);

	int sbc= ((chanlist.size()- 1)/ listmaxshow)+ 1;
	int sbs= (selected/listmaxshow);

	frameBuffer->paintBoxRel(x+ width- 13, ypos+ 2+ sbs*(sb-4)/sbc, 11, (sb-4)/sbc, COL_MENUCONTENT_PLUS_3);
}

int CChannelList::getSize() const
{
	return this->chanlist.size();
}

int CChannelList::getSelectedChannelIndex() const
{
	return this->selected;
}
