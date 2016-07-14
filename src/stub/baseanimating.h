#ifndef _INCLUDE_SIGSEGV_STUB_BASEANIMATING_H_
#define _INCLUDE_SIGSEGV_STUB_BASEANIMATING_H_


#include "stub/baseentity.h"


class CBaseAnimating : public CBaseEntity
{
public:
	float GetModelScale() const { return this->m_flModelScale; }
	
	void DrawServerHitboxes(float duration = 0.0f, bool monocolor = false) { ft_DrawServerHitboxes(this, duration, monocolor); }
	
private:
	DECL_SENDPROP(float, m_flModelScale);
	
	static MemberFuncThunk<CBaseAnimating *, void, float, bool> ft_DrawServerHitboxes;
};

class CBaseAnimatingOverlay : public CBaseAnimating {};
class CBaseFlex : public CBaseAnimatingOverlay {};

class CEconEntity : public CBaseAnimating {};


#endif
