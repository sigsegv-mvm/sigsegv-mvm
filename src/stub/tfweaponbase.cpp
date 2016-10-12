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

MemberVFuncThunk<const CBaseCombatWeapon *, int> CBaseCombatWeapon::vt_GetMaxClip1(TypeName<CBaseCombatWeapon>(), "CBaseCombatWeapon::GetMaxClip1");
MemberVFuncThunk<const CBaseCombatWeapon *, int> CBaseCombatWeapon::vt_GetMaxClip2(TypeName<CBaseCombatWeapon>(), "CBaseCombatWeapon::GetMaxClip2");


MemberVFuncThunk<const CTFWeaponBase *, int> CTFWeaponBase::vt_GetWeaponID(     TypeName<CTFBonesaw>(),     "CTFBonesaw::GetWeaponID");
MemberVFuncThunk<const CTFWeaponBase *, int> CTFWeaponBase::vt_GetPenetrateType(TypeName<CTFSniperRifle>(), "CTFSniperRifle::GetPenetrateType");


MemberVFuncThunk<CTFWeaponBaseMelee *, int> CTFWeaponBaseMelee::vt_GetSwingRange(TypeName<CTFWeaponBaseMelee>(), "CTFWeaponBaseMelee::GetSwingRange");


MemberFuncThunk<CTFSniperRifleDecap *, int> CTFSniperRifleDecap::ft_GetCount("CTFSniperRifleDecap::GetCount");


IMPL_SENDPROP(CHandle<CTFWearableRobotArm>, CTFRobotArm, m_hRobotArm, CTFRobotArm);


IMPL_SENDPROP(CHandle<CBaseEntity>, CWeaponMedigun, m_hHealingTarget, CWeaponMedigun);


static StaticFuncThunk<bool, int> ft_WeaponID_IsSniperRifle("WeaponID_IsSniperRifle");
bool WeaponID_IsSniperRifle(int id) { return ft_WeaponID_IsSniperRifle(id); }

static StaticFuncThunk<bool, int> ft_WeaponID_IsSniperRifleOrBow("WeaponID_IsSniperRifleOrBow");
bool WeaponID_IsSniperRifleOrBow(int id) { return ft_WeaponID_IsSniperRifleOrBow(id); }
