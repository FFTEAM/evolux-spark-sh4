#ifndef FREESAT_H
#define FREESAT_H

#include <stdlib.h> 
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h> 
#include <string> 

#define START   '\0' 
#define STOP    '\0' 
#define ESCAPE  '\1' 

#ifndef DATADIR
#	define DATADIR "/usr/local/share"
#endif

#ifndef FREESAT_DATA_DIRECTORY
#define FREESAT_DATA_DIRECTORY       DATADIR "/neutrino"
#endif
#define TABLE1_FILENAME FREESAT_DATA_DIRECTORY "/freesat.t1"
#define TABLE2_FILENAME FREESAT_DATA_DIRECTORY "/freesat.t2"
#define TABLE1_FILENAME_BAK "/usr/share/enigma2/freesat.t1"
#define TABLE2_FILENAME_BAK "/usr/share/enigma2/freesat.t2"

class huffTableEntry {
	friend class freesatHuffmanDecoder;
private: 
	uint32_t value; 
	uint16_t bits; 
	char next;
	huffTableEntry * nextEntry;
public: 
	huffTableEntry(unsigned int value, short bits, char next, huffTableEntry * nextEntry) : value(value), bits(bits), next(next), nextEntry(nextEntry)
	{ }
};

class freesatHuffmanDecoder 
{
private:
	huffTableEntry	*m_tables[2][256];
	bool 		loadFile(int tableid, const char *filename);
	void 		loadTables();
	bool		m_tablesLoaded;
public:
	freesatHuffmanDecoder();
	~freesatHuffmanDecoder();
	std::string decode(const unsigned char *src, size_t size);
};
#endif

