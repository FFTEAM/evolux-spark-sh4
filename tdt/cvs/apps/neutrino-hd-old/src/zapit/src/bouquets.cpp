/*
 * $Id: bouquets.cpp,v 1.101 2004/08/02 08:09:44 thegoodguy Exp $
 *
 * BouquetManager for zapit - d-box2 linux project
 *
 * (C) 2002 by Simplex    <simplex@berlios.de>,
 *             rasc       <rasc@berlios.de>,
 *             thegoodguy <thegoodguy@berlios.de>
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

#include <map>
#include <set>

#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include <zapit/bouquets.h>
#include <zapit/debug.h>
#include <zapit/getservices.h>
#include <zapit/sdt.h>
#include <zapit/settings.h>
#include <zapit/zapit.h>
#include <xmlinterface.h>

extern CBouquetManager *g_bouquetManager;

#define GET_ATTR(node, name, fmt, arg)                                  \
        do {                                                            \
                char * ptr = xmlGetAttribute(node, name);               \
                if ((ptr == NULL) || (sscanf(ptr, fmt, &arg) <= 0))     \
                        arg = 0;                                        \
        }                                                               \
        while (0)

/**** class CBouquet ********************************************************/
// -- servicetype 0 queries TV and Radio Channels
CZapitChannel* CZapitBouquet::getChannelByChannelID(const t_channel_id channel_id, const unsigned char serviceType)
{
	CZapitChannel* result = NULL;

	ZapitChannelList* channels = &tvChannels;

	switch (serviceType) {
		case ST_RESERVED: // ?
		case ST_DIGITAL_TELEVISION_SERVICE:
		case ST_NVOD_REFERENCE_SERVICE:
		case ST_NVOD_TIME_SHIFTED_SERVICE:
			channels = &tvChannels;
			break;

		case ST_DIGITAL_RADIO_SOUND_SERVICE:
			channels = &radioChannels;
			break;
	}

	unsigned int i;
	for (i = 0; (i < channels->size()) && ((*channels)[i]->getChannelID() != channel_id); i++)
	{};

	if (i<channels->size())
		result = (*channels)[i];

	if ((serviceType == ST_RESERVED) && (result == NULL)) {
		result = getChannelByChannelID(channel_id, ST_DIGITAL_RADIO_SOUND_SERVICE);
	}

	return result;
}

void CZapitBouquet::sortBouquet(void)
{
	sort(tvChannels.begin(), tvChannels.end(), CmpChannelByChName());
	sort(radioChannels.begin(), radioChannels.end(), CmpChannelByChName());
}

void CZapitBouquet::sortBouquetByNumber(void)
{
	sort(tvChannels.begin(), tvChannels.end(), CmpChannelByChNum());
	sort(radioChannels.begin(), radioChannels.end(), CmpChannelByChNum());
}

void CZapitBouquet::addService(CZapitChannel* newChannel)
{
	switch (newChannel->getServiceType())
	{
		case ST_DIGITAL_TELEVISION_SERVICE:
		case ST_NVOD_REFERENCE_SERVICE:
		case ST_NVOD_TIME_SHIFTED_SERVICE:
			tvChannels.push_back(newChannel);
			break;

		case ST_DIGITAL_RADIO_SOUND_SERVICE:
			radioChannels.push_back(newChannel);
			break;
	}
}

void CZapitBouquet::removeService(CZapitChannel* oldChannel)
{
	if (oldChannel != NULL) {
		ZapitChannelList* channels = &tvChannels;
		switch (oldChannel->getServiceType()) {
			case ST_DIGITAL_TELEVISION_SERVICE:
			case ST_NVOD_REFERENCE_SERVICE:
			case ST_NVOD_TIME_SHIFTED_SERVICE:
				channels = &tvChannels;
				break;

			case ST_DIGITAL_RADIO_SOUND_SERVICE:
				channels = &radioChannels;
				break;
		}

		(*channels).erase(remove(channels->begin(), channels->end(), oldChannel), channels->end());
	}
}

void CZapitBouquet::moveService(const unsigned int oldPosition, const unsigned int newPosition, const unsigned char serviceType)
{
	ZapitChannelList* channels = &tvChannels;
	switch (serviceType) {
		case ST_DIGITAL_TELEVISION_SERVICE:
		case ST_NVOD_REFERENCE_SERVICE:
		case ST_NVOD_TIME_SHIFTED_SERVICE:
			channels = &tvChannels;
			break;

		case ST_DIGITAL_RADIO_SOUND_SERVICE:
			channels = &radioChannels;
			break;
	}
	if ((oldPosition < channels->size()) && (newPosition < channels->size())) {
		ZapitChannelList::iterator it = channels->begin();

		advance(it, oldPosition);
		CZapitChannel* tmp = *it;
		channels->erase(it);

		advance(it, newPosition - oldPosition);
		channels->insert(it, tmp);
	}
}

size_t CZapitBouquet::recModeRadioSize(const transponder_id_t transponder_id)
{
	size_t size = 0;

	for (size_t i = 0; i < radioChannels.size(); i++)
		if (transponder_id == radioChannels[i]->getTransponderId())
			size++;

	return size;
}

size_t CZapitBouquet::recModeTVSize(const transponder_id_t transponder_id)
{
	size_t size = 0;

	for (size_t i = 0; i < tvChannels.size(); i++)
		if (transponder_id == tvChannels[i]->getTransponderId())
			size++;

	return size;
}

void CBouquetManager::writeBouquetHeader(FILE * bouq_fd, uint32_t i, const char * bouquetName)
{
//printf("[bouquets] writing bouquet header: %s\n", bouquetName);
	fprintf(bouq_fd, "\t<Bouquet name=\"%s\" hidden=\"%d\" locked=\"%d\">\n",
			bouquetName, Bouquets[i]->bHidden ? 1 : 0, Bouquets[i]->bLocked ? 1 : 0);
}

void CBouquetManager::writeBouquetFooter(FILE * bouq_fd)
{
	fprintf(bouq_fd, "\t</Bouquet>\n");
}

void CBouquetManager::writeChannels(FILE * bouq_fd, ZapitChannelList &list)
{
	for(zapit_list_it_t it = list.begin(); it != list.end(); it++)
		(*it)->dumpBouquetXml(bouq_fd);
}

void CBouquetManager::writeBouquetChannels(FILE * bouq_fd, uint32_t i, bool /*bUser*/)
{
	writeChannels(bouq_fd, Bouquets[i]->tvChannels);
	writeChannels(bouq_fd, Bouquets[i]->radioChannels);
}

void CBouquetManager::writeBouquet(FILE * bouq_fd, uint32_t i)
{
	writeBouquetHeader(bouq_fd, i, convert_UTF8_To_UTF8_XML(Bouquets[i]->Name.c_str()).c_str());
	writeBouquetChannels(bouq_fd, i);
	writeBouquetFooter(bouq_fd);
}

/**** class CBouquetManager *************************************************/
void CBouquetManager::saveBouquets(void)
{
	FILE * bouq_fd;

	bouq_fd = fopen(BOUQUETS_XML, "w");
	fprintf(bouq_fd, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<zapit>\n");
	for (unsigned int i = 0; i < Bouquets.size(); i++) {
#ifdef EVOLUX
		if (Bouquets[i] != remainChannels && Bouquets[i] != newChannels) {
#else
		if (Bouquets[i] != remainChannels) {
#endif
			DBG("save Bouquets: name %s user: %d\n", Bouquets[i]->Name.c_str(), Bouquets[i]->bUser);
			if(!Bouquets[i]->bUser) {
				writeBouquet(bouq_fd, i);
			}
		}
	}
	fprintf(bouq_fd, "</zapit>\n");
	fdatasync(fileno(bouq_fd));
	fclose(bouq_fd);
	chmod(BOUQUETS_XML, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
}

void CBouquetManager::saveUBouquets(void)
{
	FILE * ubouq_fd;

	ubouq_fd = fopen(UBOUQUETS_XML, "w");
	fprintf(ubouq_fd, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<zapit>\n");
	for (unsigned int i = 0; i < Bouquets.size(); i++) {
#ifdef EVOLUX
		if (Bouquets[i] != remainChannels && Bouquets[i] != newChannels) {
#else
		if (Bouquets[i] != remainChannels) {
#endif
			if(Bouquets[i]->bUser) {
				writeBouquet(ubouq_fd, i);
			}
		}
	}
	fprintf(ubouq_fd, "</zapit>\n");
	fdatasync(fileno(ubouq_fd));
	fclose(ubouq_fd);
	chmod(UBOUQUETS_XML, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
}

void CBouquetManager::saveBouquets(const CZapitClient::bouquetMode bouquetMode, const char * const providerName)
{
	if (bouquetMode == CZapitClient::BM_DELETEBOUQUETS) {
		INFO("removing existing bouquets");
		//unlink(BOUQUETS_XML);
		g_bouquetManager->clearAll();
	}
	if (bouquetMode == CZapitClient::BM_DONTTOUCHBOUQUETS)
		return;

	if (bouquetMode == CZapitClient::BM_CREATESATELLITEBOUQUET) {
		while (Bouquets.size() > 1) {
			BouquetList::iterator it = Bouquets.begin() + 1;
			Bouquets[0]->tvChannels.insert(Bouquets[0]->tvChannels.end(), (*it)->tvChannels.begin(), (*it)->tvChannels.end());
			Bouquets[0]->radioChannels.insert(Bouquets[0]->radioChannels.end(), (*it)->radioChannels.begin(), (*it)->radioChannels.end());
			delete (*it);
			Bouquets.erase(it);
		}
		if(Bouquets.size() > 0)
			Bouquets[0]->Name = providerName;
	}

	if ((bouquetMode == CZapitClient::BM_UPDATEBOUQUETS) || (bouquetMode == CZapitClient::BM_CREATESATELLITEBOUQUET)) {
		while (!(Bouquets.empty())) {
			CZapitBouquet* bouquet;
			int dest = g_bouquetManager->existsBouquet(Bouquets[0]->Name.c_str());
			DBG("save Bouquets: dest %d for name %s\n", dest, Bouquets[0]->Name.c_str());
			if(dest == -1) {
				bouquet = g_bouquetManager->addBouquet(Bouquets[0]->Name.c_str(), false);
				dest = g_bouquetManager->existsBouquet(Bouquets[0]->Name.c_str());
			}
			else
				bouquet = g_bouquetManager->Bouquets[dest];

			/* list from scanned exist in file */
			for(unsigned int i = 0; i < Bouquets[0]->tvChannels.size(); i++) {
				if(!(g_bouquetManager->existsChannelInBouquet(dest, Bouquets[0]->tvChannels[i]->getChannelID()))) {
					bouquet->addService(Bouquets[0]->tvChannels[i]);
					//Bouquets[0]->tvChannels[i]->pname = (char *) bouquet->Name.c_str();
					DBG("save Bouquets: adding channel %s\n", Bouquets[0]->tvChannels[i]->getName().c_str());
				}
			}
			for(unsigned int i = 0; i < Bouquets[0]->radioChannels.size(); i++) {
				if(!(g_bouquetManager->existsChannelInBouquet(dest, Bouquets[0]->radioChannels[i]->getChannelID()))) {
					bouquet->addService(Bouquets[0]->radioChannels[i]);
					//Bouquets[0]->tvChannels[i]->pname = (char *) bouquet->Name.c_str();
					DBG("save Bouquets: adding channel %s\n", Bouquets[0]->radioChannels[i]->getName().c_str());
				}
			}

			bouquet->sortBouquet();
			delete Bouquets[0];
			Bouquets.erase(Bouquets.begin());
		}
	}
}

void CBouquetManager::sortBouquets(void)
{
	sort(Bouquets.begin(), Bouquets.end(), CmpBouquetByChName());
}

void CBouquetManager::parseBouquetsXml(const char *fname, bool bUser)
{
	xmlDocPtr parser;

	parser = parseXmlFile(fname);
	if (parser == NULL)
		return;

	xmlNodePtr root = xmlDocGetRootElement(parser);
	xmlNodePtr search = root->xmlChildrenNode;
	xmlNodePtr channel_node;

	if (search) {
		t_original_network_id original_network_id;
		t_service_id service_id;
		t_transport_stream_id transport_stream_id;
		int16_t satellitePosition;
		freq_id_t freq = 0;

		INFO("reading bouquets from %s", fname);

		while ((search = xmlGetNextOccurence(search, "Bouquet")) != NULL) {
			char * name = xmlGetAttribute(search, "name");
			if(name == NULL)
				name = const_cast<char*>("Unknown");

			CZapitBouquet* newBouquet = addBouquet(name, bUser);
			char* hidden = xmlGetAttribute(search, "hidden");
			char* locked = xmlGetAttribute(search, "locked");
			newBouquet->bHidden = hidden ? (strcmp(hidden, "1") == 0) : false;
			newBouquet->bLocked = locked ? (strcmp(locked, "1") == 0) : false;
			newBouquet->bFav = (strcmp(name, "favorites") == 0);
			channel_node = search->xmlChildrenNode;
			while ((channel_node = xmlGetNextOccurence(channel_node, "S")) != NULL) {
				std::string  name2 = xmlGetAttribute(channel_node, "n");
				GET_ATTR(channel_node, "i", SCANF_SERVICE_ID_TYPE, service_id);
				GET_ATTR(channel_node, "on", SCANF_ORIGINAL_NETWORK_ID_TYPE, original_network_id);
				GET_ATTR(channel_node, "s", SCANF_SATELLITE_POSITION_TYPE, satellitePosition);
				GET_ATTR(channel_node, "t", SCANF_TRANSPORT_STREAM_ID_TYPE, transport_stream_id);
				GET_ATTR(channel_node, "frq", SCANF_SATELLITE_POSITION_TYPE, freq);
				if(freq > 20000)
					freq = freq/1000;

				CZapitChannel* chan = CServiceManager::getInstance()->FindChannel(CREATE_CHANNEL_ID64);
				if (chan != NULL) {
					DBG("%04x %04x %04x %s\n", transport_stream_id, original_network_id, service_id, xmlGetAttribute(channel_node, "n"));
#if 0
					if(bUser && (name2.length() > 1))
						chan->setName(name2);
#endif
					if(!bUser)
						chan->pname = (char *) newBouquet->Name.c_str();

					chan->bAlwaysLocked = newBouquet->bLocked;
					newBouquet->addService(chan);
				}

				channel_node = channel_node->xmlNextNode;
			}
			if(!bUser)
				newBouquet->sortBouquet();
			search = search->xmlNextNode;
		}
		INFO("total: %d bouquets", Bouquets.size());
	}
	xmlFreeDoc(parser);
}

void CBouquetManager::loadBouquets(bool ignoreBouquetFile)
{
	TIMER_START();
	clearAll();
	if (ignoreBouquetFile == false) {
		parseBouquetsXml(BOUQUETS_XML, false);
		sortBouquets();
	}

	parseBouquetsXml(UBOUQUETS_XML, true);
	renumServices();
	TIMER_STOP("[zapit] bouquet loading took");
}

void CBouquetManager::renumChannels(ZapitChannelList &list, int & counter, char * pname)
{
	for(zapit_list_it_t it = list.begin(); it != list.end(); it++) {
		if(!(*it)->number)
			(*it)->number = counter++;

		if(!(*it)->pname && pname)
			(*it)->pname = pname;

		(*it)->has_bouquet = true;
	}
}

void CBouquetManager::makeRemainingChannelsBouquet(void)
{
	ZapitChannelList unusedChannels;
	bool tomake = CZapit::getInstance()->makeRemainingChannelsBouquet();
#ifdef EVOLUX
	bool tomake_new = CZapit::getInstance()->makeNewChannelsBouquet();
#endif

	/* reset channel number and has_bouquet flag */
	CServiceManager::getInstance()->ResetChannelNumbers();

	int i = 1, j = 1;
	for (vector<CZapitBouquet*>::const_iterator it = Bouquets.begin(); it != Bouquets.end(); it++) {
		renumChannels((*it)->tvChannels, i, (*it)->bUser ? NULL : (char *) (*it)->Name.c_str());
		renumChannels((*it)->radioChannels, j, (*it)->bUser ? NULL : (char *) (*it)->Name.c_str());
	}

#ifdef EVOLUX
	if (newChannels) {
		deleteBouquet(newChannels);
		newChannels = NULL;
	}
	if(tomake_new) {
		ZapitChannelList newChannelList;
		if (CServiceManager::getInstance()->GetAllNewChannels(newChannelList)) {
			newChannels = addBouquet("extra.zapit_bouquetname_newchannels", true); // UTF-8 encoded
			sort(newChannelList.begin(), newChannelList.end(), CmpChannelByChName());

			for (ZapitChannelList::const_iterator it = newChannelList.begin(); it != newChannelList.end(); it++)
				newChannels->addService(*it);
			
			renumChannels(newChannels->tvChannels, i);
			renumChannels(newChannels->radioChannels, j);
		}
	}
#endif

#ifdef EVOLUX
	if(CServiceManager::getInstance()->GetAllUnusedChannels(unusedChannels) == false)
#else
	if(!tomake || CServiceManager::getInstance()->GetAllUnusedChannels(unusedChannels) == false)
#endif
		return;

	sort(unusedChannels.begin(), unusedChannels.end(), CmpChannelByChName());

#ifdef EVOLUX
	if (remainChannels) {
		deleteBouquet(remainChannels);
		remainChannels = NULL;
	}

	if (Bouquets.size() == 0)
		remainChannels = addBouquet("channellist.head", false); // UTF-8 encoded
	else
		remainChannels = addBouquet("extra.zapit_bouquetname_others", true); // UTF-8 encoded
#else
	// TODO: use locales
	remainChannels = addBouquet((Bouquets.size() == 0) ? "All Channels" : "Other", false); // UTF-8 encoded
#endif

	for (ZapitChannelList::const_iterator it = unusedChannels.begin(); it != unusedChannels.end(); it++) {
		remainChannels->addService(*it);
	}

	renumChannels(remainChannels->tvChannels, i);
	renumChannels(remainChannels->radioChannels, j);
#ifdef EVOLUX
	if (!tomake && Bouquets.size() > 1) {
		deleteBouquet(remainChannels);
		remainChannels = NULL;
	}
#endif
}

void CBouquetManager::renumServices()
{
	if(remainChannels)
		deleteBouquet(remainChannels);

	remainChannels = NULL;

	makeRemainingChannelsBouquet();
}

CZapitBouquet* CBouquetManager::addBouquet(const std::string & name, bool ub, bool myfav)
{
	CZapitBouquet* newBouquet = new CZapitBouquet(myfav ? "favorites" : name);
	newBouquet->bUser = ub;
	newBouquet->bFav = myfav;

//printf("CBouquetManager::addBouquet: %s, user %s\n", name.c_str(), ub ? "YES" : "NO");
	if(ub) {
		BouquetList::iterator it;
		for(it = Bouquets.begin(); it != Bouquets.end(); it++)
			if(!(*it)->bUser)
				break;
		Bouquets.insert(it, newBouquet);
	} else
		Bouquets.push_back(newBouquet);
	return newBouquet;
}

void CBouquetManager::deleteBouquet(const unsigned int id)
{
#ifdef EVOLUX
 	if (id < Bouquets.size() && Bouquets[id] != remainChannels && Bouquets[id] != newChannels)
#else
 	if (id < Bouquets.size() && Bouquets[id] != remainChannels)
#endif
		deleteBouquet(Bouquets[id]);
}

void CBouquetManager::deleteBouquet(const CZapitBouquet* bouquet)
{
	if (bouquet != NULL) {
		BouquetList::iterator it = find(Bouquets.begin(), Bouquets.end(), bouquet);

		if (it != Bouquets.end()) {
			Bouquets.erase(it);
			delete bouquet;
		}
	}
}

int str_compare_withoutspace(char const *s1, char const *s2)
{
	int cmp_result = 0;
	while(isspace(*s1)) ++s1;
	while(isspace(*s2)) ++s2;

	while(!cmp_result && *s1++ && *s2++){
		while(isspace(*s1)) ++s1;
		while(isspace(*s2)) ++s2;
		cmp_result = tolower(*s1) - tolower(*s2);
	}
	return cmp_result;
} 
// -- Find Bouquet-Name, if BQ exists   (2002-04-02 rasc)
// -- Return: Bouqet-ID (found: 0..n)  or -1 (Bouquet does not exist)
int CBouquetManager::existsBouquet(char const * const name)
{
	unsigned int i;
	for (i = 0; i < Bouquets.size(); i++) {
#ifdef EVOLUX
		if (Bouquets[i]->lName == name)
		{
			return (int)i;
		}
#endif
		if (Bouquets[i]->Name == name)
		{
			return (int)i;
		}
		else if (strcasecmp(Bouquets[i]->Name.c_str(), name)==0)
		{
			int lower1 = 0, lower2 =  0;
			int upper1 = 0 ,upper2 = 0;
			std::string str2 = name;
			size_t  pos=0;
			size_t pos2 = Bouquets[i]->Name.find("] "); 
			if(pos != std::string::npos){
				pos += pos2;
			}

			while (Bouquets[i]->Name[pos]){
				if(islower(Bouquets[i]->Name[pos])){
					++lower1;
				}
				if(isupper(Bouquets[i]->Name[pos])){
					++upper1;
				}
				if(islower(str2[pos])){
					++lower2;
				}
				if(isupper(str2[pos])){
					++upper2;
				}
				pos++;  
			}
			if ( ( upper2 && (lower1 < lower2)) || (lower2==0 && upper1==0 && upper2==lower1) ){
				Bouquets[i]->Name = str2;
			}
			return (int)i;
		}else if(!(str_compare_withoutspace(Bouquets[i]->Name.c_str(), name)) )
		{
			if(strlen(name) > Bouquets[i]->Name.length()){
				Bouquets[i]->Name = name;
			}
			return (int)i;
		}
	}
	return -1;
}

int CBouquetManager::existsUBouquet(char const * const name, bool myfav)
{
	unsigned int i;

	for (i = 0; i < Bouquets.size(); i++) {
		if(myfav) {
			if (Bouquets[i]->bFav)
				return (int)i;
		}
		//else if (Bouquets[i]->bUser && strncasecmp(Bouquets[i]->Name.c_str(), name,Bouquets[i]->Name.length())==0)
		else if (Bouquets[i]->bUser && (Bouquets[i]->Name == name))
			return (int)i;
#ifdef EVOLUX
		else if (Bouquets[i]->bUser && (Bouquets[i]->lName == name))
			return (int)i;
#endif
	}
	return -1;
}
// -- Check if channel exists in BQ   (2002-04-05 rasc)
// -- Return: True/false
bool CBouquetManager::existsChannelInBouquet( unsigned int bq_id, const t_channel_id channel_id)
{
	bool     status = false;
	CZapitChannel  *ch = NULL;

	if (bq_id <= Bouquets.size()) {
		// query TV-Channels  && Radio channels
		ch = Bouquets[bq_id]->getChannelByChannelID(channel_id, 0);
		if (ch)  status = true;
	}
	return status;
}

void CBouquetManager::moveBouquet(const unsigned int oldId, const unsigned int newId)
{
	if ((oldId < Bouquets.size()) && (newId < Bouquets.size())) {
		BouquetList::iterator it = Bouquets.begin();

		advance(it, oldId);
		CZapitBouquet* tmp = *it;
		Bouquets.erase(it);

		advance(it, newId - oldId);
		Bouquets.insert(it, tmp);
	}
}

void CBouquetManager::clearAll()
{
	for (unsigned int i =0; i < Bouquets.size(); i++)
		delete Bouquets[i];

	Bouquets.clear();
	remainChannels = NULL;
#ifdef EVOLUX
	newChannels = NULL;
#endif
}

CBouquetManager::ChannelIterator::ChannelIterator(CBouquetManager* owner, const bool TV)
{
	Owner = owner;
	tv = TV;
	if (Owner->Bouquets.size() == 0)
		c = -2;
	else {
		b = 0;
		c = -1;
		(*this)++;
	}
}

CBouquetManager::ChannelIterator CBouquetManager::ChannelIterator::operator ++(int)
{
	if (c != -2)  // we can add if it's not the end marker
	{
		c++;
		if ((unsigned int) c >= getBouquet()->size()) {
			for (b++; b < Owner->Bouquets.size(); b++)
				if (getBouquet()->size() != 0) {
					c = 0;
					goto end;
				}
			c = -2;
		}
	}
 end:
	return(*this);
}

CZapitChannel* CBouquetManager::ChannelIterator::operator *()
{
	return (*getBouquet())[c];               // returns junk if we are an end marker !!
}

CBouquetManager::ChannelIterator CBouquetManager::ChannelIterator::FindChannelNr(const unsigned int channel)
{
	c = channel;
	for (b = 0; b < Owner->Bouquets.size(); b++)
		if (getBouquet()->size() > (unsigned int)c)
			goto end;
		else
			c -= getBouquet()->size();
	c = -2;
 end:
	return (*this);
}

int CBouquetManager::ChannelIterator::getLowestChannelNumberWithChannelID(const t_channel_id channel_id)
{
	int i = 0;

	for (b = 0; b < Owner->Bouquets.size(); b++)
		for (c = 0; (unsigned int) c < getBouquet()->size(); c++, i++)
			if ((**this)->getChannelID() == channel_id)
			    return (**this)->number -1;
			    //return i;
	return -1; // not found
}

int CBouquetManager::ChannelIterator::getNrofFirstChannelofBouquet(const unsigned int bouquet_nr)
{
	if (bouquet_nr >= Owner->Bouquets.size())
		return -1;  // not found

	int i = 0;

	for (b = 0; b < bouquet_nr; b++)
		i += getBouquet()->size();

	return i;
}
