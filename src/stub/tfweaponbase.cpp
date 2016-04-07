#include "stub/tfweaponbase.h"


IMPL_SENDPROP(float, CBaseCombatWeapon, m_flNextPrimaryAttack,   CBaseCombatWeapon);
IMPL_SENDPROP(float, CBaseCombatWeapon, m_flNextSecondaryAttack, CBaseCombatWeapon);
IMPL_SENDPROP(float, CBaseCombatWeapon, m_flTimeWeaponIdle,      CBaseCombatWeapon);
IMPL_SENDPROP(int,   CBaseCombatWeapon, m_iState,                CBaseCombatWeapon);
IMPL_SENDPROP(int,   CBaseCombatWeapon, m_iPrimaryAmmoType,      CBaseCombatWeapon);
IMPL_SENDPROP(int,   CBaseCombatWeapon, m_iSecondaryAmmoType,    CBaseCombatWeapon);
IMPL_SENDPROP(int,   CBaseCombatWeapon, m_iClip1,                CBaseCombatWeapon);
IMPL_SENDPROP(int,   CBaseCombatWeapon, m_iClip2,                CBaseCombatWeapon);


MemberVFuncThunk<const CTFWeaponBase *, int> CTFWeaponBase::vt_GetWeaponID(     TypeName<CTFBonesaw>(),     "CTFBonesaw::GetWeaponID");
MemberVFuncThunk<const CTFWeaponBase *, int> CTFWeaponBase::vt_GetPenetrateType(TypeName<CTFSniperRifle>(), "CTFSniperRifle::GetPenetrateType");


IMPL_SENDPROP(CHandle<CBaseEntity>, CWeaponMedigun, m_hHealingTarget, CWeaponMedigun);


static StaticFuncThunk<bool, int> ft_WeaponID_IsSniperRifle("WeaponID_IsSniperRifle");
bool WeaponID_IsSniperRifle(int id) { return ft_WeaponID_IsSniperRifle(id); }

static StaticFuncThunk<bool, int> ft_WeaponID_IsSniperRifleOrBow("WeaponID_IsSniperRifleOrBow");
bool WeaponID_IsSniperRifleOrBow(int id) { return ft_WeaponID_IsSniperRifleOrBow(id); }
