#ifndef _INCLUDE_SIGSEGV_STUB_BASEANIMATING_H_
#define _INCLUDE_SIGSEGV_STUB_BASEANIMATING_H_


#include "stub/baseentity.h"


class CBaseAnimating : public CBaseEntity
{
public:
	float GetModelScale() const { return this->m_flModelScale; }
	
	void SetModelScale(float scale, float change_duration = 0.0f)          {        ft_SetModelScale      (this, scale, change_duration); }
	void DrawServerHitboxes(float duration = 0.0f, bool monocolor = false) {        ft_DrawServerHitboxes (this, duration, monocolor); }
	void SetBodygroup(int iGroup, int iValue)                              {        ft_SetBodygroup       (this, iGroup, iValue); }
	int GetBodygroup(int iGroup)                                           { return ft_GetBodygroup       (this, iGroup); }
	const char *GetBodygroupName(int iGroup)                               { return ft_GetBodygroupName   (this, iGroup); }
	int FindBodygroupByName(const char *name)                              { return ft_FindBodygroupByName(this, name); }
	int GetBodygroupCount(int iGroup)                                      { return ft_GetBodygroupCount  (this, iGroup); }
	int GetNumBodyGroups()                                                 { return ft_GetNumBodyGroups   (this); }
	
	DECL_SENDPROP(int,   m_nSkin);
	DECL_SENDPROP(int,   m_nBody);
	
private:
	DECL_SENDPROP(float, m_flModelScale);
	
	static MemberFuncThunk<CBaseAnimating *, void, float, float> ft_SetModelScale;
	static MemberFuncThunk<CBaseAnimating *, void, float, bool>  ft_DrawServerHitboxes;
	static MemberFuncThunk<CBaseAnimating *, void, int, int>     ft_SetBodygroup;
	static MemberFuncThunk<CBaseAnimating *, int, int>           ft_GetBodygroup;
	static MemberFuncThunk<CBaseAnimating *, const char *, int>  ft_GetBodygroupName;
	static MemberFuncThunk<CBaseAnimating *, int, const char *>  ft_FindBodygroupByName;
	static MemberFuncThunk<CBaseAnimating *, int, int>           ft_GetBodygroupCount;
	static MemberFuncThunk<CBaseAnimating *, int>                ft_GetNumBodyGroups;
};

class CBaseAnimatingOverlay : public CBaseAnimating {};
class CBaseFlex : public CBaseAnimatingOverlay {};

class CEconEntity : public CBaseAnimating {};


#endif
