#include "stub/nav.h"


// CTFNavArea::m_nAttributes
// * CTFNavArea::IsValidForWanderingPopulation
// * CTFNavArea::IsBlocked
// * CTFNavArea::IsSentryGunHere
//   CTFNavMesh::ResetMeshAttributes
//   CTFNavMesh::RemoveAllMeshDecoration
//   CTFNavMesh::ComputeLegalBombDropAreas
//   CTFNavMesh::CollectAndMaskSpawnRoomExits
//   TF_EditClearAllAttributes
//   GetBombInfo


FuncThunk<void (*)(CTFNavMesh *, CUtlVector<CBaseObject *> *, int)> CTFNavMesh::ft_CollectBuiltObjects("CTFNavMesh::CollectBuiltObjects");

GlobalThunk<CTFNavMesh *> TheNavMesh("TheNavMesh");
