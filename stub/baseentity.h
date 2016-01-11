#ifndef _INCLUDE_SIGSEGV_STUB_BASEENTITY_H_
#define _INCLUDE_SIGSEGV_STUB_BASEENTITY_H_


#include "extension.h"
#include "link/link.h"
#include "prop/prop.h"


class CBaseEntity
{
public:
	/* inline */
	int entindex();
	const Vector& GetAbsOrigin() const;
	bool IsEFlagSet(int nEFlagMask) const;
	
	/* getter/setter */
	int GetTeamNumber() const { return this->m_iTeamNum; }
	bool IsAlive()            { return this->m_lifeState == LIFE_ALIVE; }
	
	/* thunk */
	IServerNetworkable *GetNetworkable() { return (*ft_GetNetworkable)(this);       }
	void CalcAbsolutePosition()          {        (*ft_CalcAbsolutePosition)(this); }
	
private:
	static FuncThunk<IServerNetworkable * (*)(CBaseEntity *)> ft_GetNetworkable;
	static FuncThunk<void (*)(CBaseEntity *)>                 ft_CalcAbsolutePosition;
	
	PROP_STR(CBaseEntity);
	
	PROP_SENDPROP(int,    CBaseEntity, m_iHealth);
	PROP_SENDPROP(char,   CBaseEntity, m_lifeState);
	PROP_DATAMAP(int,     CBaseEntity, m_iEFlags);
	PROP_SENDPROP(int,    CBaseEntity, m_iTeamNum);
	PROP_SENDPROP(Vector, CBaseEntity, m_vecAbsOrigin);
};

inline CBaseEntity *GetContainingEntity(edict_t *pent)
{
	if (pent != nullptr && pent->GetUnknown() != nullptr) {
		return pent->GetUnknown()->GetBaseEntity();
	}
	
	return nullptr;
}

inline int ENTINDEX(edict_t *pEdict)
{
	return gamehelpers->IndexOfEdict(pEdict);
}

inline int ENTINDEX(CBaseEntity *pEnt)
{
	if (pEnt == nullptr) {
		return 0;
	}
	
	return pEnt->entindex();
}

inline edict_t *INDEXENT(int iEdictNum)
{
	return engine->PEntityOfEntIndex(iEdictNum);
}

inline CBaseEntity *UTIL_EntityByIndex(int entityIndex)
{
	CBaseEntity *entity = nullptr;
	
	if (entityIndex > 0) {
		edict_t *edict = INDEXENT(entityIndex);
		if (edict != nullptr && !edict->IsFree()) {
			entity = GetContainingEntity(edict);
		}
	}
	
	return entity;
}


inline int CBaseEntity::entindex()
{
	return ENTINDEX(this->GetNetworkable()->GetEdict());
}

inline const Vector& CBaseEntity::GetAbsOrigin() const
{
	if (this->IsEFlagSet(EFL_DIRTY_ABSTRANSFORM)) {
		const_cast<CBaseEntity *>(this)->CalcAbsolutePosition();
	}
	return this->m_vecAbsOrigin;
}

inline bool CBaseEntity::IsEFlagSet(int nEFlagMask) const
{
	return (this->m_iEFlags & nEFlagMask) != 0;
}


#endif
