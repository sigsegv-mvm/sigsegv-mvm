#ifndef _INCLUDE_SIGSEGV_STUB_TF_OBJECTIVE_RESOURCE_H_
#define _INCLUDE_SIGSEGV_STUB_TF_OBJECTIVE_RESOURCE_H_


#include "stub/baseentity.h"
#include "prop.h"
#include "link/link.h"


enum MvMWaveClassFlags : unsigned int
{
	CLASSFLAG_NORMAL          = (1 << 0), // set for non-support bots and tanks
	CLASSFLAG_SUPPORT         = (1 << 1), // set for "support 1" and "support limited" bots
	CLASSFLAG_MISSION         = (1 << 2), // set for mission support bots and teleporters
	CLASSFLAG_MINIBOSS        = (1 << 3), // set for minibosses and tanks (red background)
	CLASSFLAG_CRITICAL        = (1 << 4), // set for crit bots (blue border)
	CLASSFLAG_SUPPORT_LIMITED = (1 << 5), // set for "support limited" bots
};


class CBaseTeamObjectiveResource : public CBaseEntity {};

class CTFObjectiveResource : public CBaseTeamObjectiveResource
{
public:
	int GetMvMEventPopfileType() const { return this->m_nMvMEventPopfileType; }
	
private:
	DECL_SENDPROP(int, m_nMvMEventPopfileType);
};


extern GlobalThunk<CBaseTeamObjectiveResource *> g_pObjectiveResource;
inline CBaseTeamObjectiveResource *ObjectiveResource()   { return g_pObjectiveResource; }
inline CTFObjectiveResource       *TFObjectiveResource() { return reinterpret_cast<CTFObjectiveResource *>(ObjectiveResource()); }


#endif
