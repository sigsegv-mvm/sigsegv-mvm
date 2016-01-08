#include "stub_baseentity.h"
#include "stub_gamerules.h"
#include "stub_cmissionpopulator.h"


FuncThunk<IServerNetworkable * (*)(CBaseEntity *)> CBaseEntity::ft_CBaseEntity_GetNetworkable("CBaseEntity::GetNetworkable");

CProp_SendProp<CTFGameRules, bool> CTFGameRules::m_bPlayingMannVsMachine("CTFGameRules", "m_bPlayingMannVsMachine");

FuncThunk<bool (*)(CMissionPopulator *, int)> CMissionPopulator::ft_UpdateMission("CMissionPopulator::UpdateMission");
