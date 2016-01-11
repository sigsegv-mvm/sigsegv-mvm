#include "stub/baseentity.h"


FuncThunk<IServerNetworkable * (*)(CBaseEntity *)> CBaseEntity::ft_GetNetworkable(      "CBaseEntity::GetNetworkable");
FuncThunk<void (*)(CBaseEntity *)>                 CBaseEntity::ft_CalcAbsolutePosition("CBaseEntity::CalcAbsolutePosition");
