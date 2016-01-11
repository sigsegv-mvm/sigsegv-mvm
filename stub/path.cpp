#include "stub/path.h"


FuncThunk<bool (*)(Path *, INextBot *, CBaseCombatCharacter *, CTFBotPathCost&, float, bool)> Path::ft_Compute_CTFBotPathCost("Path::Compute<CTFBotPathCost>");

FuncThunk<void (*)(PathFollower *, INextBot *)> PathFollower::ft_Update                 ("PathFollower::Update");
FuncThunk<void (*)(PathFollower *, float)>      PathFollower::ft_SetMinLookAheadDistance("PathFollower::SetMinLookAheadDistance");
