#include <stdio.h>

#include "pwrmngr.h"

static const char * FILENAME = "pwrmngr.cpp";

void cCpuFreqManager::Up(void) { printf("%s:%s\n", FILENAME, __FUNCTION__); }
void cCpuFreqManager::Down(void) { printf("%s:%s\n", FILENAME, __FUNCTION__); }
void cCpuFreqManager::Reset(void) { printf("%s:%s\n", FILENAME, __FUNCTION__); }
//
bool cCpuFreqManager::SetCpuFreq(unsigned long CpuFreq) {
#ifdef __sh__
	// SPARK
	if (CpuFreq == 0)
		CpuFreq = 450000000;
	FILE *pll0 = fopen ("/proc/cpu_frequ/pll0_ndiv_mdiv", "w");
	if (pll0) {
		CpuFreq /= 1000000;
		fprintf(pll0, "%d\n", CpuFreq/10 * 256 + 3);
		fclose (pll0);
		return 0;
	}
#endif
	printf("%s:%s\n", FILENAME, __FUNCTION__); return 0;
}

bool cCpuFreqManager::SetDelta(unsigned long Delta) { printf("%s:%s\n", FILENAME, __FUNCTION__); return 0; }

unsigned long cCpuFreqManager::GetCpuFreq(void) {
#ifdef __sh__
	// SPARK
	int freq = 0;
	if (FILE *pll0 = fopen("/proc/cpu_frequ/pll0_ndiv_mdiv", "r")) {
		char buffer[120];
		while(fgets(buffer, sizeof(buffer), pll0)) {
			if (1 == sscanf(buffer, "SH4 = %d MHZ", &freq))
				break;
		}
		fclose(pll0);
		return 1000 * 1000 * (unsigned long) freq;
	}
#endif
	printf("%s:%s\n", FILENAME, __FUNCTION__); return 0;
}

unsigned long cCpuFreqManager::GetDelta(void) { printf("%s:%s\n", FILENAME, __FUNCTION__); return 0; }
//
cCpuFreqManager::cCpuFreqManager(void) { printf("%s:%s\n", FILENAME, __FUNCTION__); }



bool cPowerManager::SetState(PWR_STATE PowerState) { printf("%s:%s\n", FILENAME, __FUNCTION__); return 0; }

bool cPowerManager::Open(void) { printf("%s:%s\n", FILENAME, __FUNCTION__); return 0; }
void cPowerManager::Close(void) { printf("%s:%s\n", FILENAME, __FUNCTION__); }
//
bool cPowerManager::SetStandby(bool Active, bool Passive) { printf("%s:%s\n", FILENAME, __FUNCTION__); return 0; }
//
cPowerManager::cPowerManager(void) { printf("%s:%s\n", FILENAME, __FUNCTION__); }
cPowerManager::~cPowerManager() { printf("%s:%s\n", FILENAME, __FUNCTION__); }

