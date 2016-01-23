#include "stub/baseentity.h"


IMPL_DATAMAP(int,    CBaseEntity, m_iEFlags);
IMPL_DATAMAP(Vector, CBaseEntity, m_vecAbsOrigin);

IMPL_SENDPROP(int,  CBaseEntity, m_iHealth,   CBasePlayer);
IMPL_SENDPROP(char, CBaseEntity, m_lifeState, CBasePlayer);
IMPL_SENDPROP(int,  CBaseEntity, m_iTeamNum,  CBaseEntity);

FuncThunk<IServerNetworkable * (*)(CBaseEntity *)> CBaseEntity::ft_GetNetworkable(      "CBaseEntity::GetNetworkable");
FuncThunk<void (*)(CBaseEntity *)>                 CBaseEntity::ft_CalcAbsolutePosition("CBaseEntity::CalcAbsolutePosition");
