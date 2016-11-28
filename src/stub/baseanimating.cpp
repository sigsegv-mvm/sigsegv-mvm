#include "stub/baseanimating.h"


IMPL_SENDPROP(int,   CBaseAnimating, m_nSkin,        CBaseAnimating);
IMPL_SENDPROP(float, CBaseAnimating, m_flModelScale, CBaseAnimating);

MemberFuncThunk<CBaseAnimating *, void, float, float> CBaseAnimating::ft_SetModelScale     ("CBaseAnimating::SetModelScale");
MemberFuncThunk<CBaseAnimating *, void, float, bool>  CBaseAnimating::ft_DrawServerHitboxes("CBaseAnimating::DrawServerHitboxes");
