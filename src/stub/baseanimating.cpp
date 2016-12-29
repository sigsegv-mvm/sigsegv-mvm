#include "stub/baseanimating.h"


IMPL_SENDPROP(int,   CBaseAnimating, m_nSkin,        CBaseAnimating);
IMPL_SENDPROP(int,   CBaseAnimating, m_nBody,        CBaseAnimating);
IMPL_SENDPROP(float, CBaseAnimating, m_flModelScale, CBaseAnimating);

MemberFuncThunk<CBaseAnimating *, void, float, float> CBaseAnimating::ft_SetModelScale      ("CBaseAnimating::SetModelScale");
MemberFuncThunk<CBaseAnimating *, void, float, bool>  CBaseAnimating::ft_DrawServerHitboxes ("CBaseAnimating::DrawServerHitboxes");
MemberFuncThunk<CBaseAnimating *, void, int, int>     CBaseAnimating::ft_SetBodygroup       ("CBaseAnimating::SetBodygroup");
MemberFuncThunk<CBaseAnimating *, int, int>           CBaseAnimating::ft_GetBodygroup       ("CBaseAnimating::GetBodygroup");
MemberFuncThunk<CBaseAnimating *, const char *, int>  CBaseAnimating::ft_GetBodygroupName   ("CBaseAnimating::GetBodygroupName");
MemberFuncThunk<CBaseAnimating *, int, const char *>  CBaseAnimating::ft_FindBodygroupByName("CBaseAnimating::FindBodygroupByName");
MemberFuncThunk<CBaseAnimating *, int, int>           CBaseAnimating::ft_GetBodygroupCount  ("CBaseAnimating::GetBodygroupCount");
MemberFuncThunk<CBaseAnimating *, int>                CBaseAnimating::ft_GetNumBodyGroups   ("CBaseAnimating::GetNumBodyGroups");
