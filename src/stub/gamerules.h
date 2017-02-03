#ifndef _INCLUDE_SIGSEGV_STUB_GAMERULES_H_
#define _INCLUDE_SIGSEGV_STUB_GAMERULES_H_


#include "prop.h"
#include "link/link.h"
#include "stub/baseentity.h"


enum gamerules_roundstate_t
{
	// initialize the game, create teams
	GR_STATE_INIT = 0,

	//Before players have joined the game. Periodically checks to see if enough players are ready
	//to start a game. Also reverts to this when there are no active players
	GR_STATE_PREGAME,

	//The game is about to start, wait a bit and spawn everyone
	GR_STATE_STARTGAME,

	//All players are respawned, frozen in place
	GR_STATE_PREROUND,

	//Round is on, playing normally
	GR_STATE_RND_RUNNING,

	//Someone has won the round
	GR_STATE_TEAM_WIN,

	//Noone has won, manually restart the game, reset scores
	GR_STATE_RESTART,

	//Noone has won, restart the game
	GR_STATE_STALEMATE,

	//Game is over, showing the scoreboard etc
	GR_STATE_GAME_OVER,

	//Game is in a bonus state, transitioned to after a round ends
	GR_STATE_BONUS,

	//Game is awaiting the next wave/round of a multi round experience
	GR_STATE_BETWEEN_RNDS,

	GR_NUM_ROUND_STATES
};


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
	
	const CViewVectors *GetViewVectors() const                   { return vt_GetViewVectors(this); }
	bool ShouldCollide(int collisionGroup0, int collisionGroup1) { return vt_ShouldCollide(this, collisionGroup0, collisionGroup1); }
	
private:
	static MemberVFuncThunk<const CGameRules *, const CViewVectors *> vt_GetViewVectors;
	static MemberVFuncThunk<      CGameRules *, bool, int, int>       vt_ShouldCollide;
};

class CMultiplayRules : public CGameRules
{
public:
	VoiceCommandMenuItem_t *VoiceCommand(CBaseMultiplayerPlayer *pPlayer, int iMenu, int iItem) { return vt_VoiceCommand(this, pPlayer, iMenu, iItem); }
	
private:
	static MemberVFuncThunk<CMultiplayRules *, VoiceCommandMenuItem_t *, CBaseMultiplayerPlayer *, int, int> vt_VoiceCommand;
};

class CTeamplayRules : public CMultiplayRules {};

class CTeamplayRoundBasedRules : public CTeamplayRules
{
public:
	gamerules_roundstate_t State_Get() { return this->m_iRoundState; }
	int GetWinningTeam()               { return this->m_iWinningTeam; }
	bool IsPlayerReady(int iIndex)     { return this->m_bPlayerReady[iIndex]; }
	
	void BroadcastSound(int iTeam, const char *sound, int iAdditionalSoundFlags = 0) {        ft_BroadcastSound(this, iTeam, sound, iAdditionalSoundFlags); }
	float GetMinTimeWhenPlayerMaySpawn(CBasePlayer *pPlayer)                         { return ft_GetMinTimeWhenPlayerMaySpawn(this, pPlayer); }
	
	float GetNextRespawnWave(int iTeam, CBasePlayer *pPlayer) { return vt_GetNextRespawnWave(this, iTeam, pPlayer); }
	
private:
	DECL_SENDPROP(gamerules_roundstate_t, m_iRoundState);
	DECL_SENDPROP(int,                    m_iWinningTeam);
	DECL_SENDPROP(bool[33],               m_bPlayerReady);
	
	static MemberFuncThunk<CTeamplayRoundBasedRules *, void, int, const char *, int> ft_BroadcastSound;
	static MemberFuncThunk<CTeamplayRoundBasedRules *, float, CBasePlayer *>         ft_GetMinTimeWhenPlayerMaySpawn;
	
	static MemberVFuncThunk<CTeamplayRoundBasedRules *, float, int, CBasePlayer *> vt_GetNextRespawnWave;
};

class CTFGameRules : public CTeamplayRoundBasedRules
{
public:
	bool IsMannVsMachineMode() const { return this->m_bPlayingMannVsMachine; }
	
	bool CanUpgradeWithAttrib(CTFPlayer *player, int slot, unsigned short attr, CMannVsMachineUpgrades *upgrade) { return ft_CanUpgradeWithAttrib               (this, player, slot, attr, upgrade); }
	int GetCostForUpgrade(CMannVsMachineUpgrades *upgrade, int slot, int pclass, CTFPlayer *player)              { return ft_GetCostForUpgrade                  (this, upgrade, slot, pclass, player); }
	int GetUpgradeTier(int index)                                                                                { return ft_GetUpgradeTier                     (this, index); }
	bool IsUpgradeTierEnabled(CTFPlayer *player, int slot, int tier)                                             { return ft_IsUpgradeTierEnabled               (this, player, slot, tier); }
	void PlayerReadyStatus_UpdatePlayerState(CTFPlayer *player, bool state)                                      { return ft_PlayerReadyStatus_UpdatePlayerState(this, player, state); }
	void DistributeCurrencyAmount(int amount, CTFPlayer *player, bool b1, bool b2, bool b3)                      {        ft_DistributeCurrencyAmount           (this, amount, player, b1, b2, b3); }
	
	DECL_SENDPROP(bool, m_bPlayingMedieval);
	DECL_SENDPROP(bool, m_bPlayingMannVsMachine);
	
private:
	static MemberFuncThunk<CTFGameRules *, bool, CTFPlayer *, int, unsigned short, CMannVsMachineUpgrades *> ft_CanUpgradeWithAttrib;
	static MemberFuncThunk<CTFGameRules *, int, CMannVsMachineUpgrades *, int, int, CTFPlayer *>             ft_GetCostForUpgrade;
	static MemberFuncThunk<CTFGameRules *, int, int>                                                         ft_GetUpgradeTier;
	static MemberFuncThunk<CTFGameRules *, bool, CTFPlayer *, int, int>                                      ft_IsUpgradeTierEnabled;
	static MemberFuncThunk<CTFGameRules *, void, CTFPlayer *, bool>                                          ft_PlayerReadyStatus_UpdatePlayerState;
	static MemberFuncThunk<CTFGameRules *, void, int, CTFPlayer *, bool, bool, bool>                         ft_DistributeCurrencyAmount;
};


inline void CGameRulesProxy::NotifyNetworkStateChanged()
{
	if (s_pGameRulesProxy != nullptr) {
		s_pGameRulesProxy->NetworkStateChanged();
	}
}


extern GlobalThunk<CGameRules *> g_pGameRules;
inline CGameRules *GameRules()
{
	CGameRules *gamerules = g_pGameRules;
	if (gamerules == nullptr) {
		Warning("Warning: GameRules() == nullptr!\n");
	}
	return gamerules;
}

inline CMultiplayRules          *MultiplayRules()          { return reinterpret_cast<CMultiplayRules          *>(GameRules()); }
inline CTeamplayRules           *TeamplayGameRules()       { return reinterpret_cast<CTeamplayRules           *>(GameRules()); }
inline CTeamplayRoundBasedRules *TeamplayRoundBasedRules() { return reinterpret_cast<CTeamplayRoundBasedRules *>(GameRules()); }
inline CTFGameRules             *TFGameRules()             { return reinterpret_cast<CTFGameRules             *>(GameRules()); }


#endif
