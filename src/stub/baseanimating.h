#ifndef _INCLUDE_SIGSEGV_STUB_BASEANIMATING_H_
#define _INCLUDE_SIGSEGV_STUB_BASEANIMATING_H_


#include "stub/baseentity.h"


class CBaseAnimating : public CBaseEntity
{
public:
	float GetModelScale() const { return this->m_flModelScale; }
	
private:
	DECL_SENDPROP(float, m_flModelScale);
};

class CBaseAnimatingOverlay : public CBaseAnimating {};
class CBaseFlex : public CBaseAnimatingOverlay {};

class CEconEntity : public CBaseAnimating {};


#endif
