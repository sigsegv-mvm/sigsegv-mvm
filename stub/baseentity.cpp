#include "stub/baseentity.h"


FuncThunk<IServerNetworkable * (*)(CBaseEntity *)> CBaseEntity::ft_GetNetworkable(      "CBaseEntity::GetNetworkable");
FuncThunk<void (*)(CBaseEntity *)>                 CBaseEntity::ft_CalcAbsolutePosition("CBaseEntity::CalcAbsolutePosition");
VFuncThunk<bool (*)(CBaseEntity *)>                CBaseEntity::vt_IsAlive(             "[VT] CBaseEntity", "CBaseEntity::IsAlive");
