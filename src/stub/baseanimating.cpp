#include "stub/baseanimating.h"
#include "stub/econ.h"


IMPL_SENDPROP(int,   CBaseAnimating, m_nSkin,        CBaseAnimating);
IMPL_SENDPROP(int,   CBaseAnimating, m_nBody,        CBaseAnimating);
IMPL_SENDPROP(float, CBaseAnimating, m_flModelScale, CBaseAnimating);
IMPL_SENDPROP(float, CBaseAnimating, m_flCycle,      CBaseAnimating);

MemberFuncThunk<CBaseAnimating *, void, float, float>              CBaseAnimating::ft_SetModelScale       ("CBaseAnimating::SetModelScale");
MemberFuncThunk<CBaseAnimating *, void, float, bool>               CBaseAnimating::ft_DrawServerHitboxes  ("CBaseAnimating::DrawServerHitboxes");
MemberFuncThunk<CBaseAnimating *, void, int, int>                  CBaseAnimating::ft_SetBodygroup        ("CBaseAnimating::SetBodygroup");
MemberFuncThunk<CBaseAnimating *, int, int>                        CBaseAnimating::ft_GetBodygroup        ("CBaseAnimating::GetBodygroup");
MemberFuncThunk<CBaseAnimating *, const char *, int>               CBaseAnimating::ft_GetBodygroupName    ("CBaseAnimating::GetBodygroupName");
MemberFuncThunk<CBaseAnimating *, int, const char *>               CBaseAnimating::ft_FindBodygroupByName ("CBaseAnimating::FindBodygroupByName");
MemberFuncThunk<CBaseAnimating *, int, int>                        CBaseAnimating::ft_GetBodygroupCount   ("CBaseAnimating::GetBodygroupCount");
MemberFuncThunk<CBaseAnimating *, int>                             CBaseAnimating::ft_GetNumBodyGroups    ("CBaseAnimating::GetNumBodyGroups");
MemberFuncThunk<CBaseAnimating *, void>                            CBaseAnimating::ft_ResetSequenceInfo   ("CBaseAnimating::ResetSequenceInfo");
MemberFuncThunk<CBaseAnimating *, void, int>                       CBaseAnimating::ft_ResetSequence       ("CBaseAnimating::ResetSequence");
MemberFuncThunk<CBaseAnimating *, CStudioHdr *>                    CBaseAnimating::ft_GetModelPtr         ("CBaseAnimating::GetModelPtr");
MemberFuncThunk<CBaseAnimating *, int, CStudioHdr *, const char *> CBaseAnimating::ft_LookupPoseParameter ("CBaseAnimating::LookupPoseParameter");
MemberFuncThunk<CBaseAnimating *, float, int>                      CBaseAnimating::ft_GetPoseParameter_int("CBaseAnimating::GetPoseParameter [int]");
MemberFuncThunk<CBaseAnimating *, float, const char *>             CBaseAnimating::ft_GetPoseParameter_str("CBaseAnimating::GetPoseParameter [str]");


MemberFuncThunk<CEconEntity *, void> CEconEntity::ft_DebugDescribe("CEconEntity::DebugDescribe");

MemberVFuncThunk<CEconEntity *, CAttributeContainer *> CEconEntity::vt_GetAttributeContainer(TypeName<CEconEntity>(), "CEconEntity::GetAttributeContainer");


CEconItemView *CEconEntity::GetItem()
{
	CAttributeContainer *attr_container = this->GetAttributeContainer();
	assert(attr_container != nullptr);
	
	return attr_container->GetItem();
}
