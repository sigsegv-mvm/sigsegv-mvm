#include "stub/path.h"


FuncThunk<void (*)(PathFollower *, INextBot *)> PathFollower::ft_Update                 ("PathFollower::Update");
FuncThunk<void (*)(PathFollower *, float)>      PathFollower::ft_SetMinLookAheadDistance("PathFollower::SetMinLookAheadDistance");
