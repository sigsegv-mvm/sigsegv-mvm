#include "stub/nav.h"


FuncThunk<void (*)(CTFNavMesh *, CUtlVector<CBaseObject *> *, int)> CTFNavMesh::ft_CollectBuiltObjects("CTFNavMesh::CollectBuiltObjects");

GlobalThunk<CTFNavMesh *> TheNavMesh("TheNavMesh");
