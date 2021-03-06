/*
 * $Id: zapit.h,v 1.68 2004/10/27 16:08:40 lucgas Exp $
 */

#ifndef __zapit_h__
#define __zapit_h__

#include <OpenThreads/Thread>
#include <configfile.h>
#include <eventserver.h>
#include <connection/basicserver.h>
#include <xmltree/xmlinterface.h>
#include <ca_cs.h>

#include "client/zapitclient.h"

#include <zapit/channel.h>
#include <zapit/bouquets.h>
#include <zapit/frontend_c.h>

#define PAL	0
#define NTSC	1
#define AUDIO_CONFIG_FILE "/var/tuxbox/config/zapit/audio.conf"
#ifdef EVOLUX
#define VOLUME_CONFIG_FILE CONFIGDIR "/zapit/audiovolume.conf"
#endif

typedef std::map<t_channel_id, audio_map_set_t> audio_map_t;
typedef audio_map_t::iterator audio_map_iterator_t;
typedef std::map<transponder_id_t, time_t> sdt_tp_map_t;

class CZapitSdtMonitor : public OpenThreads::Thread
{
	private:
		bool started;
		bool sdt_wakeup;

		sdt_tp_map_t sdt_tp;

		void run();

	public:
		CZapitSdtMonitor();
		~CZapitSdtMonitor();
		bool Start();
		bool Stop();
		void Wakeup();
};

class CZapit : public OpenThreads::Thread
{
	private:
		enum {
			TV_MODE = 0x01,
			RADIO_MODE = 0x02,
			RECORD_MODE = 0x04
		};

		bool started;
		bool event_mode;
		bool firstzap;
		bool playing;
		bool list_changed;

		int audio_mode;
		int def_audio_mode;
		int aspectratio;
		int mode43;
		unsigned int volume_left;
		unsigned int volume_right;
		unsigned int def_volume_left;
		unsigned int def_volume_right;

		int currentMode;
		bool playbackStopForced;
		diseqc_t diseqcType;

		int video_mode;
		Zapit_config config;
		CConfigFile configfile;
		CEventServer *eventServer;

		CZapitChannel * current_channel;
		t_channel_id live_channel_id;
		TP_params TP;

		audio_map_t audio_map;
#ifdef EVOLUX
		typedef pair<t_channel_id, int> t_chan_apid;
		map<t_chan_apid, int> volume_map;
		map<t_chan_apid, int>::iterator volume_map_it;
#endif

		bool current_is_nvod;
		bool standby;
		uint32_t  lastChannelRadio;
		uint32_t  lastChannelTV;
		int abort_zapit;
		int pmt_update_fd;

		void LoadAudioMap();
		void SaveSettings(bool write_conf, bool write_audio);
		void SaveChannelPids(CZapitChannel* channel);
		void RestoreChannelPids(CZapitChannel* channel);
		void ConfigFrontend();

		bool TuneChannel(CZapitChannel * channel, bool &transponder_change);
		bool ParsePatPmt(CZapitChannel * channel);

		bool send_data_count(int connfd, int data_count);
		void sendAPIDs(int connfd);
		void internalSendChannels(int connfd, ZapitChannelList* channels, const unsigned int first_channel_nr, bool nonames);
		void sendBouquets(int connfd, const bool emptyBouquetsToo, CZapitClient::channelsMode mode);
		void sendBouquetChannels(int connfd, const unsigned int bouquet, const CZapitClient::channelsMode mode, bool nonames);
		void sendChannels(int connfd, const CZapitClient::channelsMode mode, const CZapitClient::channelsOrder order);
		void SendConfig(int connfd);

		bool StartPlayBack(CZapitChannel *thisChannel);
		bool StopPlayBack(bool send_pmt);
		void SendPMT(bool forupdate = false);
		void SetAudioStreamType(CZapitAudioChannel::ZapitAudioChannelType audioChannelType);

		void enterStandby();
		void leaveStandby();
		unsigned int ZapTo(const unsigned int bouquet, const unsigned int pchannel);
		unsigned int ZapTo(t_channel_id channel_id, bool isSubService = false);
		unsigned int ZapTo(const unsigned int pchannel);
		void PrepareScan();

		CZapitSdtMonitor SdtMonitor;

		void run();
	protected:
		static CZapit * zapit;
		CZapit();
	public:
		~CZapit();
		static CZapit * getInstance();

		void LoadSettings();
		bool Start(Z_start_arg* ZapStart_arg);
		bool Stop();
		bool ParseCommand(CBasicMessage::Header &rmsg, int connfd);
		bool ZapIt(const t_channel_id channel_id, bool for_update = false, bool startplayback = true);
		bool ZapForRecord(const t_channel_id channel_id);
		bool ChangeAudioPid(uint8_t index);
		void SetRadioMode();
		void SetTVMode();
		void SetRecordMode(bool enable);
		int getMode();

		bool PrepareChannels();
		bool StartScan(int scan_mode);
		bool StartScanTP(TP_params * TPparams);
//		bool StartFastScan(int scan_mode, int opid);

		void addChannelToBouquet(const unsigned int bouquet, const t_channel_id channel_id);
		void SetConfig(Zapit_config * Cfg);
		void GetConfig(Zapit_config &Cfg);

		virtual void SendEvent(const unsigned int eventID, const void* eventbody = NULL, const unsigned int eventbodysize = 0);

		audio_map_set_t * GetSavedPids(const t_channel_id channel_id);

		/* inlines */
		void Abort() { abort_zapit = 1; };
		bool Recording() { return currentMode & RECORD_MODE; };
		bool makeRemainingChannelsBouquet() { return config.makeRemainingChannelsBouquet; };
#ifdef EVOLUX
		bool makeNewChannelsBouquet() { return config.makeNewChannelsBouquet; };
#endif
		bool scanSDT() { return config.scanSDT; };
		bool scanPids() { return config.scanPids; };
		void scanPids(bool enable) { config.scanPids = enable; };

		CZapitChannel * GetCurrentChannel() { return current_channel; };
		t_channel_id GetCurrentChannelID() { return live_channel_id; };
		void SetCurrentChannelID(const t_channel_id channel_id) { live_channel_id = channel_id; };
};
#endif /* __zapit_h__ */
