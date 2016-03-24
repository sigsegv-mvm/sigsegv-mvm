#include "stub/projectiles.h"


IMPL_SENDPROP(CHandle<CBaseEntity>, CBaseProjectile, m_hOriginalLauncher, CBaseProjectile);

MemberVFuncThunk<const CBaseProjectile *, int> CBaseProjectile::vt_GetProjectileType(TypeName<CBaseProjectile>(), "CBaseProjectile::GetProjectileType");


IMPL_SENDPROP(Vector, CTFBaseRocket, m_vInitialVelocity, CTFBaseRocket);


IMPL_SENDPROP(bool,   CTFWeaponBaseGrenadeProj, m_bCritical,        CTFWeaponBaseGrenadeProj);
IMPL_SENDPROP(Vector, CTFWeaponBaseGrenadeProj, m_vInitialVelocity, CTFWeaponBaseGrenadeProj);

MemberVFuncThunk<const CTFWeaponBaseGrenadeProj *, int> CTFWeaponBaseGrenadeProj::vt_GetWeaponID(TypeName<CTFWeaponBaseGrenadeProj>(), "CTFWeaponBaseGrenadeProj::GetWeaponID");


GlobalThunk<CUtlVector<IBaseProjectileAutoList *>> IBaseProjectileAutoList::m_IBaseProjectileAutoListAutoList("IBaseProjectileAutoList::m_IBaseProjectileAutoListAutoList");
