#include "stub/projectiles.h"


IMPL_SENDPROP(bool, CTFWeaponBaseGrenadeProj, m_bCritical, CTFWeaponBaseGrenadeProj);


GlobalThunk<CUtlVector<IBaseProjectileAutoList *>> IBaseProjectileAutoList::m_IBaseProjectileAutoListAutoList("IBaseProjectileAutoList::m_IBaseProjectileAutoListAutoList");
