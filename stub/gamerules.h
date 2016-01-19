#ifndef _INCLUDE_SIGSEGV_STUB_GAMERULES_H_
#define _INCLUDE_SIGSEGV_STUB_GAMERULES_H_


#include "extension.h"
#include "prop/prop.h"


class CTFGameRules
{
public:
	bool IsMannVsMachineMode() const { return this->m_bPlayingMannVsMachine; }
	
private:
	PROP_STR(CTFGameRules);
	
	PROP_SENDPROP(bool, CTFGameRules, m_bPlayingMannVsMachine);
};


inline CTFGameRules *TFGameRules()
{
	return reinterpret_cast<CTFGameRules *>(g_pSDKTools->GetGameRules());
}


#endif
