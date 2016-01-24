#ifndef _INCLUDE_SIGSEGV_STUB_GAMERULES_H_
#define _INCLUDE_SIGSEGV_STUB_GAMERULES_H_


#include "prop.h"
#include "link/link.h"


class CGameRules {};
class CMultiplayRules : public CGameRules {};
class CTeamplayRules : public CMultiplayRules {};
class CTeamplayRoundBasedRules : public CTeamplayRules {};


class CTFGameRules : public CTeamplayRoundBasedRules
{
public:
	bool IsMannVsMachineMode() const { return this->m_bPlayingMannVsMachine; }
	
private:
	DEF_SENDPROP(bool, m_bPlayingMannVsMachine);
};


extern GlobalThunk<CGameRules *> g_pGameRules;
inline CGameRules   *GameRules()   { return g_pGameRules; }
inline CTFGameRules *TFGameRules() { return reinterpret_cast<CTFGameRules *>(GameRules()); }


#endif
