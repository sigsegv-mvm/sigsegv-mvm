#ifndef _INCLUDE_SIGSEGV_STUB_UPGRADES_H_
#define _INCLUDE_SIGSEGV_STUB_UPGRADES_H_


#include "util/misc.h"
#include "link/link.h"


struct CMannVsMachineUpgrades
{
	char m_szAttribute[0x40]; // +0x000
	char m_szIcon[MAX_PATH];  // +0x040
	float m_flIncrement;      // +0x144
	float m_flCap;            // +0x148
	int m_nCost;              // +0x14c
	int m_iUIGroup;           // +0x150
	int m_iQuality;           // +0x154
	int m_iTier;              // +0x158
};
SIZE_CHECK(CMannVsMachineUpgrades, 0x15c);


class CMannVsMachineUpgradeManager;
extern GlobalThunk<CMannVsMachineUpgradeManager> g_MannVsMachineUpgrades;

class CMannVsMachineUpgradeManager
{
public:
	static CUtlVector<CMannVsMachineUpgrades>& Upgrades()
	{
		CMannVsMachineUpgradeManager& instance = g_MannVsMachineUpgrades;
		return instance.m_Upgrades;
	}
	
private:
	uint8_t pad_00[0x0c];
	CUtlVector<CMannVsMachineUpgrades> m_Upgrades;
	CUtlMap<const char *, int> m_UpgradeMap;
};
SIZE_CHECK(CMannVsMachineUpgradeManager, 0x3c);


#endif
