/*
 * $Id: cam.h,v 1.25 2003/02/09 19:22:08 thegoodguy Exp $
 *
 * (C) 2002-2003 Andreas Oberritter <obi@tuxbox.org>,
 *               thegoodguy         <thegoodguy@berlios.de>
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

#ifndef __zapit_cam_h__
#define __zapit_cam_h__

#include "ci.h"
#include <basicclient.h>
#include "types.h"
#include <OpenThreads/Mutex>

#define DEMUX_DECODE_0 1
#define DEMUX_DECODE_1 2
#define DEMUX_DECODE_2 4

#define DEMUX_SOURCE_0 0
#define DEMUX_SOURCE_1 1
#define DEMUX_SOURCE_2 2

#define LIVE_DEMUX	0
#define STREAM_DEMUX	1
#define RECORD_DEMUX	2

class CCam : public CBasicClient
{
	private:
		virtual unsigned char getVersion(void) const;
		virtual const char *getSocketName(void) const;
		int camask, demuxes[3];

	public:
		CCam();
		bool sendMessage(const char * const data, const size_t length, bool update = false);
		bool setCaPmt(CCaPmt * const caPmt, int _demux = 0, int _camask = 1, bool update = false);
		int  getCaMask(void) { return camask; };
		int  makeMask(int demux, bool add);
};

typedef std::map<t_channel_id, CCam*> cammap_t;
typedef cammap_t::iterator cammap_iterator_t;

class CCamManager
{
	public:
		enum runmode {
			PLAY,
			RECORD,
			STREAM
		};
	private:
		cammap_t		channel_map;
		OpenThreads::Mutex	mutex;
		static CCamManager *	manager;
		bool SetMode(t_channel_id id, enum runmode mode, bool enable, bool force_update = false);

	public:
		CCamManager();
		~CCamManager();
		static CCamManager * getInstance(void);
		bool Start(t_channel_id id, enum runmode mode, bool force_update = false) { return SetMode(id, mode, true, force_update); };
		bool Stop(t_channel_id id, enum runmode mode) { return SetMode(id, mode, false); };

};
#endif /* __cam_h__ */
