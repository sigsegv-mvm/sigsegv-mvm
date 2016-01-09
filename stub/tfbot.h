#ifndef _INCLUDE_SIGSEGV_STUB_TFBOT_H_
#define _INCLUDE_SIGSEGV_STUB_TFBOT_H_


#include "extension.h"
#include "prop.h"
#include "stub/baseentity.h"


class CTFBot : public CBaseEntity
{
public:
	int GetMission() const
	{
		return m_nMission.Get(this);
	}
	
private:
	static CProp_Extract<CTFBot, int> m_nMission;
};


#endif
