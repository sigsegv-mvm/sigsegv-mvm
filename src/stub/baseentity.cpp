#include "stub/baseentity.h"
#include "stub/tfplayer.h"


IMPL_DATAMAP(IServerNetworkable, CBaseEntity, m_Network);
IMPL_DATAMAP(string_t,           CBaseEntity, m_iClassname);
IMPL_DATAMAP(string_t,           CBaseEntity, m_iName);
IMPL_DATAMAP(int,                CBaseEntity, m_iEFlags);
IMPL_DATAMAP(Vector,             CBaseEntity, m_vecAbsOrigin);

IMPL_SENDPROP(int,                  CBaseEntity, m_iTeamNum,      CBaseEntity);
IMPL_SENDPROP(int,                  CBaseEntity, m_iMaxHealth,    CBaseObject);
IMPL_SENDPROP(int,                  CBaseEntity, m_iHealth,       CBasePlayer);
IMPL_SENDPROP(char,                 CBaseEntity, m_lifeState,     CBasePlayer);
IMPL_SENDPROP(CHandle<CBaseEntity>, CBaseEntity, m_hGroundEntity, CBasePlayer);

MemberFuncThunk<CBaseEntity *, void              > CBaseEntity::ft_CalcAbsolutePosition("CBaseEntity::CalcAbsolutePosition");
MemberFuncThunk<CBaseEntity *, bool, const char *> CBaseEntity::ft_ClassMatches(        "CBaseEntity::ClassMatches");


bool CBaseEntity::IsPlayer() const
{
	return (rtti_cast<const CBasePlayer *>(this) != nullptr);
}
