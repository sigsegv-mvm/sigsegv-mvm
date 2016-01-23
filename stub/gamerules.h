#ifndef _INCLUDE_SIGSEGV_STUB_GAMERULES_H_
#define _INCLUDE_SIGSEGV_STUB_GAMERULES_H_


#include "prop.h"


// CGameRules
// CMultiplayRules
// CTeamplayRules
// CTeamplayRoundBasedRules


class CTFGameRules
{
public:
	bool IsMannVsMachineMode() const { return this->m_bPlayingMannVsMachine; }
	
private:
	DEF_SENDPROP(bool, m_bPlayingMannVsMachine);
};


inline CTFGameRules *TFGameRules()
{
	return reinterpret_cast<CTFGameRules *>(g_pSDKTools->GetGameRules());
}


#endif
