#include "stub/tfplayer.h"


IMPL_DATAMAP(char, CBasePlayer, m_szNetname);

MemberVFuncThunk<const CBasePlayer *, bool> CBasePlayer::vt_IsBot(TypeName<CBasePlayer>(), "CBasePlayer::IsBot");


IMPL_SENDPROP(int,      CTFPlayerClassShared, m_iClass,         CTFPlayer);
IMPL_SENDPROP(string_t, CTFPlayerClassShared, m_iszClassIcon,   CTFPlayer);
IMPL_SENDPROP(string_t, CTFPlayerClassShared, m_iszCustomModel, CTFPlayer);


IMPL_SENDPROP(float, CTFPlayerShared, m_flRageMeter,    CTFPlayer);
IMPL_SENDPROP(bool,  CTFPlayerShared, m_bRageDraining,  CTFPlayer);
IMPL_SENDPROP(bool,  CTFPlayerShared, m_bInUpgradeZone, CTFPlayer);

MemberFuncThunk<      CTFPlayerShared *, void, ETFCond, float, CBaseEntity *> CTFPlayerShared::ft_AddCond(   "CTFPlayerShared::AddCond");
MemberFuncThunk<      CTFPlayerShared *, void, ETFCond, bool                > CTFPlayerShared::ft_RemoveCond("CTFPlayerShared::RemoveCond");
MemberFuncThunk<const CTFPlayerShared *, bool, ETFCond                      > CTFPlayerShared::ft_InCond(    "CTFPlayerShared::InCond");


IMPL_SENDPROP(CTFPlayerShared, CTFPlayer, m_Shared,      CTFPlayer);
IMPL_SENDPROP(CTFPlayerClass,  CTFPlayer, m_PlayerClass, CTFPlayer);
IMPL_SENDPROP(bool,            CTFPlayer, m_bIsMiniBoss, CTFPlayer);


bool CTFPlayer::IsPlayerClass(int iClass) const
{
	const CTFPlayerClass *pClass = this->GetPlayerClass();
	if (pClass == nullptr) return false;
	
	return pClass->IsClass(iClass);
}


CBasePlayer *UTIL_PlayerByIndex(int playerIndex)
{
	CBasePlayer *pPlayer = nullptr;
	
	if (playerIndex > 0 && playerIndex <= gpGlobals->maxClients) {
		edict_t *pPlayerEdict = INDEXENT(playerIndex);
		if (pPlayerEdict != nullptr && !pPlayerEdict->IsFree()) {
			pPlayer = reinterpret_cast<CBasePlayer *>(GetContainingEntity(pPlayerEdict));
		}
	}
	
	return pPlayer;
}
