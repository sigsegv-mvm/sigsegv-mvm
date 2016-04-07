#ifndef _INCLUDE_SIGSEGV_STUB_GAMERULES_H_
#define _INCLUDE_SIGSEGV_STUB_GAMERULES_H_


#include "prop.h"
#include "link/link.h"
#include "stub/baseentity.h"


class CBaseMultiplayerPlayer;
class CTFPlayer;
struct VoiceCommandMenuItem_t;
class CMannVsMachineUpgrades;


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
	
	const CViewVectors *GetViewVectors() const { return vt_GetViewVectors(this); }
	
private:
	static MemberVFuncThunk<const CGameRules *, const CViewVectors *> vt_GetViewVectors;
};

class CMultiplayRules : public CGameRules
{
public:
	VoiceCommandMenuItem_t *VoiceCommand(CBaseMultiplayerPlayer *pPlayer, int iMenu, int iItem) { return vt_VoiceCommand(this, pPlayer, iMenu, iItem); }
	
private:
	static MemberVFuncThunk<CMultiplayRules *, VoiceCommandMenuItem_t *, CBaseMultiplayerPlayer *, int, int> vt_VoiceCommand;
};

class CTeamplayRules : public CMultiplayRules {};
class CTeamplayRoundBasedRules : public CTeamplayRules {};

class CTFGameRules : public CTeamplayRoundBasedRules
{
public:
	bool IsMannVsMachineMode() const { return this->m_bPlayingMannVsMachine; }
	
	bool CanUpgradeWithAttrib(CTFPlayer *player, int slot, unsigned short attr, CMannVsMachineUpgrades *upgrade) { return ft_CanUpgradeWithAttrib(this, player, slot, attr, upgrade); }
	int GetCostForUpgrade(CMannVsMachineUpgrades *upgrade, int slot, int pclass, CTFPlayer *player)              { return ft_GetCostForUpgrade   (this, upgrade, slot, pclass, player); }
	int GetUpgradeTier(int index)                                                                                { return ft_GetUpgradeTier      (this, index); }
	bool IsUpgradeTierEnabled(CTFPlayer *player, int slot, int tier)                                             { return ft_IsUpgradeTierEnabled(this, player, slot, tier); }
	
	DECL_SENDPROP(bool, m_bPlayingMedieval);
	DECL_SENDPROP(bool, m_bPlayingMannVsMachine);
	
private:
	static MemberFuncThunk<CTFGameRules *, bool, CTFPlayer *, int, unsigned short, CMannVsMachineUpgrades *> ft_CanUpgradeWithAttrib;
	static MemberFuncThunk<CTFGameRules *, int, CMannVsMachineUpgrades *, int, int, CTFPlayer *>             ft_GetCostForUpgrade;
	static MemberFuncThunk<CTFGameRules *, int, int>                                                         ft_GetUpgradeTier;
	static MemberFuncThunk<CTFGameRules *, bool, CTFPlayer *, int, int>                                      ft_IsUpgradeTierEnabled;
};


inline void CGameRulesProxy::NotifyNetworkStateChanged()
{
	if (s_pGameRulesProxy != nullptr) {
		s_pGameRulesProxy->NetworkStateChanged();
	}
}


extern GlobalThunk<CGameRules *> g_pGameRules;
inline CGameRules                *GameRules()               { return g_pGameRules; }
inline CMultiplayRules           *MultiplayRules()          { return reinterpret_cast<CMultiplayRules *>(GameRules()); }
inline CTeamplayRules            *TeamplayGameRules()       { return reinterpret_cast<CTeamplayRules *>(GameRules()); }
inline CTeamplayRoundBasedRules  *TeamplayRoundBasedRules() { return reinterpret_cast<CTeamplayRoundBasedRules *>(GameRules()); }
inline CTFGameRules              *TFGameRules()             { return reinterpret_cast<CTFGameRules *>(GameRules()); }


#endif
