#include "stub/team.h"


MemberVFuncThunk<const CTeam *, int>                CTeam::vt_GetTeamNumber(TypeName<CTeam>(), "CTeam::GetTeamNumber");
MemberVFuncThunk<      CTeam *, const char *>       CTeam::vt_GetName      (TypeName<CTeam>(), "CTeam::GetName");
MemberVFuncThunk<      CTeam *, int>                CTeam::vt_GetNumPlayers(TypeName<CTeam>(), "CTeam::GetNumPlayers");
MemberVFuncThunk<      CTeam *, CBasePlayer *, int> CTeam::vt_GetPlayer    (TypeName<CTeam>(), "CTeam::GetPlayer");


MemberFuncThunk<CTFTeam *, int, int>           CTFTeam::ft_GetNumObjects("CTFTeam::GetNumObjects");
MemberFuncThunk<CTFTeam *, CBaseObject *, int> CTFTeam::ft_GetObject    ("CTFTeam::GetObject");


MemberFuncThunk<CTFTeamManager *, bool, int>      CTFTeamManager::ft_IsValidTeam("CTFTeamManager::IsValidTeam");
MemberFuncThunk<CTFTeamManager *, CTFTeam *, int> CTFTeamManager::ft_GetTeam    ("CTFTeamManager::GetTeam");


GlobalThunk<CTFTeamManager> s_TFTeamManager("s_TFTeamManager");
