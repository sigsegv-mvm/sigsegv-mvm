#include "stub/tfplayer.h"
#include "stub/tfweaponbase.h"


IMPL_SENDPROP(CHandle<CBaseCombatWeapon>, CBaseCombatCharacter, m_hActiveWeapon, CBaseCombatCharacter);


IMPL_DATAMAP(char, CBasePlayer, m_szNetname);

MemberFuncThunk<CBasePlayer *, void, Vector *, Vector *, Vector *> CBasePlayer::ft_EyeVectors("CBasePlayer::EyeVectors");

MemberVFuncThunk<const CBasePlayer *, bool>             CBasePlayer::vt_IsBot(        TypeName<CBasePlayer>(), "CBasePlayer::IsBot");
MemberVFuncThunk<      CBasePlayer *, void, bool, bool> CBasePlayer::vt_CommitSuicide(TypeName<CBasePlayer>(), "CBasePlayer::CommitSuicide");
MemberVFuncThunk<      CBasePlayer *, void>             CBasePlayer::vt_ForceRespawn( TypeName<CTFPlayer>(),   "CTFPlayer::ForceRespawn");


MemberVFuncThunk<CBaseMultiplayerPlayer *, bool, int, const char *, char *, size_t, IRecipientFilter *> CBaseMultiplayerPlayer::vt_SpeakConceptIfAllowed(TypeName<CTFPlayer>(), "CTFPlayer::SpeakConceptIfAllowed");


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

MemberFuncThunk<CTFPlayer *, void, int, int> CTFPlayer::ft_StartBuildingObjectOfType("CTFPlayer::StartBuildingObjectOfType");


bool CTFPlayer::IsPlayerClass(int iClass) const
{
	const CTFPlayerClass *pClass = this->GetPlayerClass();
	if (pClass == nullptr) return false;
	
	return pClass->IsClass(iClass);
}


CTFWeaponBase *CTFPlayer::GetActiveTFWeapon() const
{
	return rtti_cast<CTFWeaponBase *>(this->GetActiveWeapon());
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


static StaticFuncThunk<ETFCond, const char *> ft_GetTFConditionFromName("GetTFConditionFromName");
ETFCond GetTFConditionFromName(const char *name) { return ft_GetTFConditionFromName(name); }

static StaticFuncThunk<const char *, ETFCond> ft_GetTFConditionName("GetTFConditionName");
const char *GetTFConditionName(ETFCond cond) { return ft_GetTFConditionName(cond); }
