#include "stub/baseentity.h"


FuncThunk<IServerNetworkable * (*)(CBaseEntity *)> CBaseEntity::ft_GetNetworkable(      "CBaseEntity::GetNetworkable");
FuncThunk<void (*)(CBaseEntity *)>                 CBaseEntity::ft_CalcAbsolutePosition("CBaseEntity::CalcAbsolutePosition");

CProp_SendProp<CBaseEntity, char>   CBaseEntity::m_lifeState(   "CBaseEntity", "m_lifeState");
CProp_DataMap<CBaseEntity, int>     CBaseEntity::m_iEFlags(     "CBaseEntity", "m_iEFlags");
CProp_SendProp<CBaseEntity, int>    CBaseEntity::m_iTeamNum(    "CBaseEntity", "m_iTeamNum");
CProp_SendProp<CBaseEntity, Vector> CBaseEntity::m_vecAbsOrigin("CBaseEntity", "m_vecAbsOrigin");
