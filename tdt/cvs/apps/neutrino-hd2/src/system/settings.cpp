/*

        $Id: settings.cpp,v 1.39 2012/03/21 16:32:41 mohousch Exp $

	Neutrino-GUI  -   DBoxII-Project

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

#include <cstring>
#include <system/settings.h>

#include <zapit/settings.h>
#include <zapit/satconfig.h>

#include <config.h>
#include <global.h>
#include <neutrino.h>

#include <gui/widget/icons.h>

#include <zapit/getservices.h>
#include <zapit/satconfig.h>

#include <zapit/frontend_c.h>


#define get_set CNeutrinoApp::getInstance()->getScanSettings()

const int default_timing[TIMING_SETTING_COUNT] =
{
	0,
	60,
	240,
	6,
	60,
	3
};

const neutrino_locale_t timing_setting_name[TIMING_SETTING_COUNT] =
{
	LOCALE_TIMING_MENU,
	LOCALE_TIMING_CHANLIST,
	LOCALE_TIMING_EPG,
	LOCALE_TIMING_INFOBAR,
	LOCALE_TIMING_FILEBROWSER,
	LOCALE_TIMING_NUMERICZAP
};

// scan settings
CScanSettings::CScanSettings(void)
	: configfile('\t')
{
	//delivery_system = DVB_S;
	satNameNoDiseqc[0] = 0;
}

void CScanSettings::useDefaults( /*const delivery_system_t _delivery_system*/ )
{
	printf("[neutrino] CScanSettings::useDefaults\n");
	
	//delivery_system = _delivery_system;
	bouquetMode     = CZapitClient::BM_UPDATEBOUQUETS;
	scanType = CZapitClient::ST_ALL;
	diseqcMode      = NO_DISEQC;
	diseqcRepeat    = 0;
	TP_mod = 3;
	TP_fec = 3;

	//DVB-T
	TP_band = 0;
	TP_HP = 2;
	TP_LP = 1;
	TP_const = 1;
	TP_trans = 1;
	TP_guard = 3;
	TP_hierarchy = 0;

	/*
	switch (delivery_system)
	{
		case DVB_C:
			strcpy(satNameNoDiseqc, "none");
			break;
			
		case DVB_S:
			strcpy(satNameNoDiseqc, "none");
			break;
			
		case DVB_T:
			strcpy(satNameNoDiseqc, "");
			break;
	}
	*/
	strcpy(satNameNoDiseqc, "none");
}

bool CScanSettings::loadSettings(const char * const fileName/*, const delivery_system_t _delivery_system*/ )
{
	printf("[neutrino] CScanSettings::loadSettings\n");
	
	/* use default */
	//useDefaults(_delivery_system);
	
	/* if scan.conf not exists load default */
	if(!configfile.loadConfig(fileName))
		//return false;
		useDefaults( /*_delivery_system*/ );

	/* check delivery system */
	#if 0
	if (configfile.getInt32("delivery_system", -1) != delivery_system)
	{
		// configfile is not for this delivery system
		configfile.clear();
		//return false;
		useDefaults(_delivery_system);
	}
	#endif

	diseqcMode = configfile.getInt32("diseqcMode"  , diseqcMode);
	diseqcRepeat = configfile.getInt32("diseqcRepeat", diseqcRepeat);

	bouquetMode = (CZapitClient::bouquetMode) configfile.getInt32("bouquetMode" , bouquetMode);
	scanType=(CZapitClient::scanType) configfile.getInt32("scanType", scanType);
	strcpy(satNameNoDiseqc, configfile.getString("satNameNoDiseqc", satNameNoDiseqc).c_str());

	scan_mode = configfile.getInt32("scan_mode", 1); // NIT (0) or fast (1)
	
	TP_fec = configfile.getInt32("TP_fec", 1);
	TP_pol = configfile.getInt32("TP_pol", 0);
	TP_mod = configfile.getInt32("TP_mod", 3);
	strcpy(TP_freq, configfile.getString("TP_freq", "10100000").c_str());
	strcpy(TP_rate, configfile.getString("TP_rate", "27500000").c_str());
#if HAVE_DVB_API_VERSION >= 3
	if(TP_fec == 4) 
		TP_fec = 5;
#endif

	//DVB-T
	TP_band = configfile.getInt32("TP_band", 0);
	TP_HP = configfile.getInt32("TP_HP", 2);
	TP_LP = configfile.getInt32("TP_LP", 1);
	TP_const = configfile.getInt32("TP_const", 1);
	TP_trans = configfile.getInt32("TP_trans", 1);
	TP_guard = configfile.getInt32("TP_guard", 3);
	TP_hierarchy = configfile.getInt32("TP_hierarchy", 0);
		
	scanSectionsd = configfile.getInt32("scanSectionsd", 0);

	return true;
}

bool CScanSettings::saveSettings(const char * const fileName)
{
	printf("[neutrino] CScanSettings::saveSettings\n");
	
	/* remove file */
	//remove(fileName);
	
	//test
	//printf("test: delivery_system: %d\n\n", g_info.delivery_system);
	
	//configfile.setInt32("delivery_system", delivery_system);
	
	configfile.setInt32( "diseqcMode", diseqcMode );
	configfile.setInt32( "diseqcRepeat", diseqcRepeat );
	
	configfile.setInt32( "bouquetMode", bouquetMode );
	configfile.setInt32( "scanType", scanType );
	configfile.setString( "satNameNoDiseqc", satNameNoDiseqc );
	
	configfile.setInt32("scan_mode", scan_mode);

	configfile.setInt32("TP_fec", TP_fec);
	configfile.setInt32("TP_pol", TP_pol);
	configfile.setInt32("TP_mod", TP_mod);
	configfile.setString("TP_freq", TP_freq);
	configfile.setString("TP_rate", TP_rate);

	configfile.setInt32("TP_band", TP_band);
	configfile.setInt32("TP_HP", TP_HP);
	configfile.setInt32("TP_LP", TP_LP);
	configfile.setInt32("TP_const", TP_const);
	configfile.setInt32("TP_trans", TP_trans);
	configfile.setInt32("TP_guard", TP_guard);
	configfile.setInt32("TP_hierarchy", TP_hierarchy);

	configfile.setInt32("scanSectionsd", scanSectionsd );

	if(configfile.getModifiedFlag())
	{
		/* save neu configuration */
		configfile.saveConfig(fileName);
	}

	return true;
}

/* TPSelectHandler */
CTPSelectHandler::CTPSelectHandler(int num)
{
	feindex = num;
}

extern std::map<transponder_id_t, transponder> select_transponders;
int CTPSelectHandler::exec(CMenuTarget* parent, const std::string &actionkey)
{
	transponder_list_t::iterator tI;
	sat_iterator_t sit;
	t_satellite_position position = 0;
	std::map<int, transponder> tmplist;
	std::map<int, transponder>::iterator tmpI;
	int i;
	char cnt[5];
	int select = -1;
	static int old_selected = 0;
	static t_satellite_position old_position = 0;

	if (parent)
		parent->hide();

	//loop throught satpos
	for(sit = satellitePositions.begin(); sit != satellitePositions.end(); sit++) 
	{
		if(!strcmp(sit->second.name.c_str(), CNeutrinoApp::getInstance()->getScanSettings().satNameNoDiseqc)) 
		{
			position = sit->first;
			break;
		}
	}

	if(old_position != position) 
	{
		old_selected = 0;
		old_position = position;
	}
	
	printf("CTPSelectHandler::exec: fe(%d) %s position(%d)\n", feindex, CNeutrinoApp::getInstance()->getScanSettings().satNameNoDiseqc, position);

        CMenuWidget * menu = new CMenuWidget(LOCALE_SCANTS_SELECT_TP, NEUTRINO_ICON_SETTINGS);
        CMenuSelectorTarget * selector = new CMenuSelectorTarget(&select);
	
	// intros
	//menu->addItem(GenericMenuSeparator);
	
	i = 0;

	for(tI = select_transponders.begin(); tI != select_transponders.end(); tI++) 
	{
		t_satellite_position satpos = GET_SATELLITEPOSITION_FROM_TRANSPONDER_ID(tI->first) & 0xFFF;
		if(GET_SATELLITEPOSITION_FROM_TRANSPONDER_ID(tI->first) & 0xF000)
			satpos = -satpos;
		
		if(satpos != position)
			continue;

		char buf[128];
		sprintf(cnt, "%d", i);
		char * f, *s, *m;
		
		switch(CFrontend::getInstance(feindex)->getInfo()->type) 
		{
			case FE_QPSK:
			{
				CFrontend::getInstance(feindex)->getDelSys(tI->second.feparams.u.qpsk.fec_inner, dvbs_get_modulation(tI->second.feparams.u.qpsk.fec_inner),  f, s, m);

				snprintf(buf, sizeof(buf), "%d %c %d %s %s %s ", tI->second.feparams.frequency/1000, tI->second.polarization ? 'V' : 'H', tI->second.feparams.u.qpsk.symbol_rate/1000, f, s, m);
			}
			break;

			case FE_QAM:
			{
				CFrontend::getInstance(feindex)->getDelSys(tI->second.feparams.u.qam.fec_inner, tI->second.feparams.u.qam.modulation, f, s, m);

				snprintf(buf, sizeof(buf), "%d %d %s %s %s ", tI->second.feparams.frequency/1000, tI->second.feparams.u.qam.symbol_rate/1000, f, s, m);
			}
			break;

			case FE_OFDM:
			{
				CFrontend::getInstance(feindex)->getDelSys(tI->second.feparams.u.ofdm.code_rate_HP, tI->second.feparams.u.ofdm.constellation, f, s, m);

				snprintf(buf, sizeof(buf), "%d %s %s %s ", tI->second.feparams.frequency/1000, f, s, m);
			}
			break;
				
			case FE_ATSC:
				break;
		}
		
		menu->addItem(new CMenuForwarderNonLocalized(buf, true, NULL, selector, cnt), old_selected == i);
		tmplist.insert(std::pair <int, transponder>(i, tI->second));
		i++;
	}

	int retval = menu->exec(NULL, "");
	delete menu;
	delete selector;

	if(select >= 0) 
	{
		old_selected = select;

		tmpI = tmplist.find(select);
		//printf("CTPSelectHandler::exec: selected TP: freq %d pol %d SR %d\n", tmpI->second.feparams.frequency, tmpI->second.polarization, tmpI->second.feparams.u.qpsk.symbol_rate);

		sprintf(get_set.TP_freq, "%d", tmpI->second.feparams.frequency);
		
		switch(CFrontend::getInstance(feindex)->getInfo()->type) 
		{
			case FE_QPSK:
				printf("CTPSelectHandler::exec: fe(%d) selected TP: freq %d pol %d SR %d fec %d\n", feindex, tmpI->second.feparams.frequency, tmpI->second.polarization, tmpI->second.feparams.u.qpsk.symbol_rate, tmpI->second.feparams.u.qpsk.fec_inner);
					
				sprintf(get_set.TP_rate, "%d", tmpI->second.feparams.u.qpsk.symbol_rate);
				get_set.TP_fec = tmpI->second.feparams.u.qpsk.fec_inner;
				get_set.TP_pol = tmpI->second.polarization;
				break;

			case FE_QAM:
				printf("CTPSelectHandler::exec: fe(%d) selected TP: freq %d SR %d fec %d mod %d\n", feindex, tmpI->second.feparams.frequency, tmpI->second.feparams.u.qpsk.symbol_rate, tmpI->second.feparams.u.qam.fec_inner, tmpI->second.feparams.u.qam.modulation);
					
				sprintf(get_set.TP_rate, "%d", tmpI->second.feparams.u.qam.symbol_rate);
				get_set.TP_fec = tmpI->second.feparams.u.qam.fec_inner;
				get_set.TP_mod = tmpI->second.feparams.u.qam.modulation;
				break;

			case FE_OFDM:
			{
				printf("CTPSelectHandler::exec: fe(%d) selected TP: freq %d band %d HP %d LP %d const %d trans %d guard %d hierarchy %d\n", feindex, tmpI->second.feparams.frequency, tmpI->second.feparams.u.ofdm.bandwidth, tmpI->second.feparams.u.ofdm.code_rate_HP, tmpI->second.feparams.u.ofdm.code_rate_LP, tmpI->second.feparams.u.ofdm.constellation, tmpI->second.feparams.u.ofdm.transmission_mode, tmpI->second.feparams.u.ofdm.guard_interval, tmpI->second.feparams.u.ofdm.hierarchy_information);
					
				get_set.TP_band = tmpI->second.feparams.u.ofdm.bandwidth;
				get_set.TP_HP = tmpI->second.feparams.u.ofdm.code_rate_HP;
				get_set.TP_LP = tmpI->second.feparams.u.ofdm.code_rate_LP;
				get_set.TP_const = tmpI->second.feparams.u.ofdm.constellation;
				get_set.TP_trans = tmpI->second.feparams.u.ofdm.transmission_mode;
				get_set.TP_guard = tmpI->second.feparams.u.ofdm.guard_interval;
				get_set.TP_hierarchy = tmpI->second.feparams.u.ofdm.hierarchy_information;
			}
			break;

			case FE_ATSC:
				break;
		}	
	}
	
	if(retval == menu_return::RETURN_EXIT_ALL)
		return menu_return::RETURN_EXIT_ALL;

	return menu_return::RETURN_REPAINT;
}


