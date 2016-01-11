#include "stub/baseentity.h"


FuncThunk<IServerNetworkable * (*)(CBaseEntity *)> CBaseEntity::ft_GetNetworkable(      "CBaseEntity::GetNetworkable");
FuncThunk<bool (*)(CBaseEntity *)>                 CBaseEntity::ft_IsAlive(             "CBaseEntity::IsAlive");
FuncThunk<void (*)(CBaseEntity *)>                 CBaseEntity::ft_CalcAbsolutePosition("CBaseEntity::CalcAbsolutePosition");

CProp_DataMap<CBaseEntity, int>     CBaseEntity::m_iEFlags(     "CBaseEntity", "m_iEFlags");
CProp_SendProp<CBaseEntity, Vector> CBaseEntity::m_vecAbsOrigin("CBaseEntity", "m_vecAbsOrigin");
