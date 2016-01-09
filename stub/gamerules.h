#ifndef _INCLUDE_SIGSEGV_STUB_GAMERULES_H_
#define _INCLUDE_SIGSEGV_STUB_GAMERULES_H_


#include "extension.h"
#include "prop.h"


class CTFGameRules
{
public:
	bool IsMannVsMachineMode() const
	{
		return m_bPlayingMannVsMachine.Get(this);
	}
	
private:
	static CProp_SendProp<CTFGameRules, bool> m_bPlayingMannVsMachine;
};


inline CTFGameRules *TFGameRules()
{
	return reinterpret_cast<CTFGameRules *>(g_pSDKTools->GetGameRules());
}


#endif
