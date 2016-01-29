#ifndef _INCLUDE_SIGSEGV_STUB_BASEENTITY_H_
#define _INCLUDE_SIGSEGV_STUB_BASEENTITY_H_


#include "link/link.h"
#include "prop.h"


class CBaseEntity
{
public:
	/* inline */
	int entindex();
	const Vector& GetAbsOrigin() const;
	bool IsEFlagSet(int nEFlagMask) const;
	
	/* getter/setter */
	int GetTeamNumber() const            { return this->m_iTeamNum; }
	int GetHealth() const                { return this->m_iHealth; }
	bool IsAlive() const                 { return (this->m_lifeState == LIFE_ALIVE); }
	CBaseEntity *GetGroundEntity() const { return this->m_hGroundEntity; }
	
	/* thunk */
	IServerNetworkable *GetNetworkable() { return (*ft_GetNetworkable)(this); }
	void CalcAbsolutePosition()          {        (*ft_CalcAbsolutePosition)(this); }
	bool IsPlayer() const                { return (vt_IsPlayer.Get(this))(this); }
	
private:
	DEF_DATAMAP(int,    m_iEFlags);
	DEF_DATAMAP(Vector, m_vecAbsOrigin);
	
	DEF_SENDPROP(int,                  m_iTeamNum);
	DEF_SENDPROP(int,                  m_iHealth);
	DEF_SENDPROP(char,                 m_lifeState);
	DEF_SENDPROP(CHandle<CBaseEntity>, m_hGroundEntity);
	
	static FuncThunk<IServerNetworkable * (*)(CBaseEntity *)> ft_GetNetworkable;
	static FuncThunk<void                 (*)(CBaseEntity *)> ft_CalcAbsolutePosition;
	
	static VFuncThunk<CBaseEntity, bool (*)(const CBaseEntity *)> vt_IsPlayer;
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
