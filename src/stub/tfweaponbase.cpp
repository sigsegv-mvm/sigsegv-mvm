#include "stub/tfweaponbase.h"


IMPL_SENDPROP(float,                         CBaseCombatWeapon, m_flNextPrimaryAttack,   CBaseCombatWeapon);
IMPL_SENDPROP(float,                         CBaseCombatWeapon, m_flNextSecondaryAttack, CBaseCombatWeapon);
IMPL_SENDPROP(float,                         CBaseCombatWeapon, m_flTimeWeaponIdle,      CBaseCombatWeapon);
IMPL_SENDPROP(int,                           CBaseCombatWeapon, m_iState,                CBaseCombatWeapon);
IMPL_SENDPROP(int,                           CBaseCombatWeapon, m_iPrimaryAmmoType,      CBaseCombatWeapon);
IMPL_SENDPROP(int,                           CBaseCombatWeapon, m_iSecondaryAmmoType,    CBaseCombatWeapon);
IMPL_SENDPROP(int,                           CBaseCombatWeapon, m_iClip1,                CBaseCombatWeapon);
IMPL_SENDPROP(int,                           CBaseCombatWeapon, m_iClip2,                CBaseCombatWeapon);
IMPL_SENDPROP(CHandle<CBaseCombatCharacter>, CBaseCombatWeapon, m_hOwner,                CBaseCombatWeapon);

MemberFuncThunk<const CBaseCombatWeapon *, bool> CBaseCombatWeapon::ft_IsMeleeWeapon("CBaseCombatWeapon::IsMeleeWeapon");

MemberVFuncThunk<const CBaseCombatWeapon *, int>  CBaseCombatWeapon::vt_GetMaxClip1(TypeName<CBaseCombatWeapon>(), "CBaseCombatWeapon::GetMaxClip1");
MemberVFuncThunk<const CBaseCombatWeapon *, int>  CBaseCombatWeapon::vt_GetMaxClip2(TypeName<CBaseCombatWeapon>(), "CBaseCombatWeapon::GetMaxClip2");
MemberVFuncThunk<      CBaseCombatWeapon *, bool> CBaseCombatWeapon::vt_HasAmmo    (TypeName<CBaseCombatWeapon>(), "CBaseCombatWeapon::HasAmmo");


IMPL_SENDPROP(float, CTFWeaponBase, m_flLastFireTime,       CTFWeaponBase);
IMPL_SENDPROP(float, CTFWeaponBase, m_flEffectBarRegenTime, CTFWeaponBase);
IMPL_SENDPROP(float, CTFWeaponBase, m_flEnergy,             CTFWeaponBase);

MemberVFuncThunk<const CTFWeaponBase *, int> CTFWeaponBase::vt_GetWeaponID(     TypeName<CTFBonesaw>(),     "CTFBonesaw::GetWeaponID");
MemberVFuncThunk<const CTFWeaponBase *, int> CTFWeaponBase::vt_GetPenetrateType(TypeName<CTFSniperRifle>(), "CTFSniperRifle::GetPenetrateType");


MemberFuncThunk<CTFSniperRifleDecap *, int> CTFSniperRifleDecap::ft_GetCount("CTFSniperRifleDecap::GetCount");


MemberVFuncThunk<CTFWeaponBaseMelee *, int>            CTFWeaponBaseMelee::vt_GetSwingRange(TypeName<CTFWeaponBaseMelee>(), "CTFWeaponBaseMelee::GetSwingRange");
MemberVFuncThunk<CTFWeaponBaseMelee *, bool, trace_t&> CTFWeaponBaseMelee::vt_DoSwingTrace (TypeName<CTFWeaponBaseMelee>(), "CTFWeaponBaseMelee::DoSwingTrace");


MemberFuncThunk<CTFKnife *, bool, CTFPlayer *> CTFKnife::ft_CanPerformBackstabAgainstTarget("CTFKnife::CanPerformBackstabAgainstTarget");
MemberFuncThunk<CTFKnife *, bool, CTFPlayer *> CTFKnife::ft_IsBehindAndFacingTarget        ("CTFKnife::IsBehindAndFacingTarget");


IMPL_SENDPROP(CHandle<CTFWearableRobotArm>, CTFRobotArm, m_hRobotArm, CTFRobotArm);


IMPL_SENDPROP(CHandle<CBaseEntity>, CWeaponMedigun, m_hHealingTarget, CWeaponMedigun);


static StaticFuncThunk<bool, int> ft_WeaponID_IsSniperRifle("WeaponID_IsSniperRifle");
bool WeaponID_IsSniperRifle(int id) { return ft_WeaponID_IsSniperRifle(id); }

static StaticFuncThunk<bool, int> ft_WeaponID_IsSniperRifleOrBow("WeaponID_IsSniperRifleOrBow");
bool WeaponID_IsSniperRifleOrBow(int id) { return ft_WeaponID_IsSniperRifleOrBow(id); }
