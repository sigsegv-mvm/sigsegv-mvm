#include "stub/tfplayer.h"
#include "stub/tfweaponbase.h"


IMPL_SENDPROP(int,      CTFPlayerClassShared, m_iClass,         CTFPlayer);
IMPL_SENDPROP(string_t, CTFPlayerClassShared, m_iszClassIcon,   CTFPlayer);
IMPL_SENDPROP(string_t, CTFPlayerClassShared, m_iszCustomModel, CTFPlayer);


IMPL_SENDPROP(float, CTFPlayerShared, m_flRageMeter,    CTFPlayer);
IMPL_SENDPROP(bool,  CTFPlayerShared, m_bRageDraining,  CTFPlayer);
IMPL_SENDPROP(bool,  CTFPlayerShared, m_bInUpgradeZone, CTFPlayer);

MemberFuncThunk<      CTFPlayerShared *, void, ETFCond, float, CBaseEntity * > CTFPlayerShared::ft_AddCond       ("CTFPlayerShared::AddCond");
MemberFuncThunk<      CTFPlayerShared *, void, ETFCond, bool                 > CTFPlayerShared::ft_RemoveCond    ("CTFPlayerShared::RemoveCond");
MemberFuncThunk<const CTFPlayerShared *, bool, ETFCond                       > CTFPlayerShared::ft_InCond        ("CTFPlayerShared::InCond");
MemberFuncThunk<const CTFPlayerShared *, bool                                > CTFPlayerShared::ft_IsInvulnerable("CTFPlayerShared::IsInvulnerable");
MemberFuncThunk<      CTFPlayerShared *, void, float, float, int, CTFPlayer *> CTFPlayerShared::ft_StunPlayer    ("CTFPlayerShared::StunPlayer");


IMPL_SENDPROP(CTFPlayerShared, CTFPlayer, m_Shared,      CTFPlayer);
IMPL_SENDPROP(CTFPlayerClass,  CTFPlayer, m_PlayerClass, CTFPlayer);
IMPL_SENDPROP(bool,            CTFPlayer, m_bIsMiniBoss, CTFPlayer);
IMPL_SENDPROP(int,             CTFPlayer, m_nCurrency,   CTFPlayer);

MemberFuncThunk<      CTFPlayer *, void, int, bool         > CTFPlayer::ft_ForceChangeTeam          ("CTFPlayer::ForceChangeTeam");
MemberFuncThunk<      CTFPlayer *, void, int, int          > CTFPlayer::ft_StartBuildingObjectOfType("CTFPlayer::StartBuildingObjectOfType");
MemberFuncThunk<const CTFPlayer *, bool, ETFFlagType *, int> CTFPlayer::ft_HasTheFlag               ("CTFPlayer::HasTheFlag");
MemberFuncThunk<      CTFPlayer *, int, int                > CTFPlayer::ft_GetAutoTeam              ("CTFPlayer::GetAutoTeam");


StaticFuncThunk<CEconItemView *, CTFPlayer *, int, CEconEntity **> CTFPlayerSharedUtils::ft_GetEconItemViewByLoadoutSlot("CTFPlayerSharedUtils::GetEconItemViewByLoadoutSlot");


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
