#ifndef _INCLUDE_SIGSEGV_STUB_BASEENTITY_H_
#define _INCLUDE_SIGSEGV_STUB_BASEENTITY_H_


#include "extension.h"
#include "link/link.h"
#include "prop.h"


class CBaseEntity
{
public:
	/* inline */
	int entindex();
	const Vector& GetAbsOrigin() const;
	bool IsEFlagSet(int nEFlagMask) const;
	
	/* thunk */
	IServerNetworkable *GetNetworkable() { return (*ft_GetNetworkable)(this);       }
	bool IsAlive()                       { return (*ft_IsAlive)(this);              }
	void CalcAbsolutePosition()          {        (*ft_CalcAbsolutePosition)(this); }
	
private:
	static FuncThunk<IServerNetworkable * (*)(CBaseEntity *)> ft_GetNetworkable;
	static FuncThunk<bool (*)(CBaseEntity *)> ft_IsAlive;
	static FuncThunk<void (*)(CBaseEntity *)> ft_CalcAbsolutePosition;
	
	static CProp_DataMap<CBaseEntity, int> m_iEFlags;
	static CProp_SendProp<CBaseEntity, Vector> m_vecAbsOrigin;
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
	return this->m_vecAbsOrigin.Get(this);
}

inline bool CBaseEntity::IsEFlagSet(int nEFlagMask) const
{
	return (m_iEFlags.Get(this) & nEFlagMask) != 0;
}


#endif
