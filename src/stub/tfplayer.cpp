#include "stub/tfplayer.h"
#include "stub/tfweaponbase.h"


IMPL_SENDPROP(int,      CTFPlayerClassShared, m_iClass,         CTFPlayer);
IMPL_SENDPROP(string_t, CTFPlayerClassShared, m_iszClassIcon,   CTFPlayer);
IMPL_SENDPROP(string_t, CTFPlayerClassShared, m_iszCustomModel, CTFPlayer);

MemberFuncThunk<CTFPlayerClassShared *, void, const char *, bool> CTFPlayerClassShared::ft_SetCustomModel("CTFPlayerClassShared::SetCustomModel");


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

MemberFuncThunk<      CTFPlayer *, void, int, bool         > CTFPlayer::ft_ForceChangeTeam               ("CTFPlayer::ForceChangeTeam");
MemberFuncThunk<      CTFPlayer *, void, int, int          > CTFPlayer::ft_StartBuildingObjectOfType     ("CTFPlayer::StartBuildingObjectOfType");
MemberFuncThunk<const CTFPlayer *, bool, ETFFlagType *, int> CTFPlayer::ft_HasTheFlag                    ("CTFPlayer::HasTheFlag");
MemberFuncThunk<      CTFPlayer *, int, int                > CTFPlayer::ft_GetAutoTeam                   ("CTFPlayer::GetAutoTeam");
MemberFuncThunk<      CTFPlayer *, float, CTFWeaponBase ** > CTFPlayer::ft_MedicGetChargeLevel           ("CTFPlayer::MedicGetChargeLevel");
MemberFuncThunk<const CTFPlayer *, float                   > CTFPlayer::ft_TeamFortress_CalculateMaxSpeed("CTFPlayer::TeamFortress_CalculateMaxSpeed");
MemberFuncThunk<      CTFPlayer *, void                    > CTFPlayer::ft_UpdateModel                   ("CTFPlayer::UpdateModel");
MemberFuncThunk<const CTFPlayer *, CTFWeaponBase *, int    > CTFPlayer::ft_Weapon_OwnsThisID             ("CTFPlayer::Weapon_OwnsThisID");
MemberFuncThunk<      CTFPlayer *, CBaseObject *, int, int > CTFPlayer::ft_GetObjectOfType               ("CTFPlayer::GetObjectOfType");
MemberFuncThunk<      CTFPlayer *, int, int, int           > CTFPlayer::ft_GetMaxAmmo                    ("CTFPlayer::GetMaxAmmo");


StaticFuncThunk<CEconItemView *, CTFPlayer *, int, CEconEntity **> CTFPlayerSharedUtils::ft_GetEconItemViewByLoadoutSlot("CTFPlayerSharedUtils::GetEconItemViewByLoadoutSlot");


IMPL_SENDPROP(float, CTFRagdoll, m_flHeadScale,  CTFRagdoll);
IMPL_SENDPROP(float, CTFRagdoll, m_flTorsoScale, CTFRagdoll);
IMPL_SENDPROP(float, CTFRagdoll, m_flHandScale,  CTFRagdoll);


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


static StaticFuncThunk<ETFCond, const char *> ft_GetTFConditionFromName("GetTFConditionFromName");
ETFCond GetTFConditionFromName(const char *name) { return ft_GetTFConditionFromName(name); }

static StaticFuncThunk<const char *, ETFCond> ft_GetTFConditionName("GetTFConditionName");
const char *GetTFConditionName(ETFCond cond) { return ft_GetTFConditionName(cond); }
