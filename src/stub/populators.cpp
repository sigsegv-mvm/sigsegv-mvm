#include "stub/populators.h"


MemberFuncThunk<CWave *, void, string_t, int, unsigned int> CWave::ft_AddClassType("CWave::AddClassType");


MemberFuncThunk<CMissionPopulator *, bool, int> CMissionPopulator::ft_UpdateMission("CMissionPopulator::UpdateMission");


MemberVFuncThunk<IPopulationSpawner *, string_t, int                   > IPopulationSpawner::vt_GetClassIcon(TypeName<IPopulationSpawner>(), "IPopulationSpawner::GetClassIcon");
MemberVFuncThunk<IPopulationSpawner *, bool, int                       > IPopulationSpawner::vt_IsMiniBoss(  TypeName<IPopulationSpawner>(), "IPopulationSpawner::IsMiniBoss");
MemberVFuncThunk<IPopulationSpawner *, bool, CTFBot::AttributeType, int> IPopulationSpawner::vt_HasAttribute(TypeName<IPopulationSpawner>(), "IPopulationSpawner::HasAttribute");
