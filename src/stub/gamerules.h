#ifndef _INCLUDE_SIGSEGV_STUB_GAMERULES_H_
#define _INCLUDE_SIGSEGV_STUB_GAMERULES_H_


#include "prop.h"
#include "link/link.h"
#include "stub/baseentity.h"


class CGameRulesProxy : public CBaseEntity
{
public:
	static void NotifyNetworkStateChanged();
	
private:
	static GlobalThunk<CGameRulesProxy *> s_pGameRulesProxy;
};

class CTeamplayRoundBasedRulesProxy : public CGameRulesProxy {};
class CTFGameRulesProxy : public CTeamplayRoundBasedRulesProxy {};


class CGameRules
{
public:
	void NetworkStateChanged(void *pVar) { CGameRulesProxy::NotifyNetworkStateChanged(); }
	void NetworkStateChanged()           { CGameRulesProxy::NotifyNetworkStateChanged(); }
};

class CMultiplayRules : public CGameRules {};
class CTeamplayRules : public CMultiplayRules {};
class CTeamplayRoundBasedRules : public CTeamplayRules {};

class CTFGameRules : public CTeamplayRoundBasedRules
{
public:
	bool IsMannVsMachineMode() const { return this->m_bPlayingMannVsMachine; }
	
	DECL_SENDPROP(bool, m_bPlayingMedieval);
	
private:
	DECL_SENDPROP(bool, m_bPlayingMannVsMachine);
};


inline void CGameRulesProxy::NotifyNetworkStateChanged()
{
	if (s_pGameRulesProxy != nullptr) {
		s_pGameRulesProxy->NetworkStateChanged();
	}
}


extern GlobalThunk<CGameRules *> g_pGameRules;
inline CGameRules   *GameRules()   { return g_pGameRules; }
inline CTFGameRules *TFGameRules() { return reinterpret_cast<CTFGameRules *>(GameRules()); }


#endif
