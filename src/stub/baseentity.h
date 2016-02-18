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
	IServerNetworkable *GetNetworkable() { return this->m_Network.GetPtr(); }
	const char *GetClassname() const     { return STRING((string_t)this->m_iClassname); }
	string_t GetEntityName() const       { return this->m_iName; }
	int GetTeamNumber() const            { return this->m_iTeamNum; }
	int GetMaxHealth() const             { return this->m_iMaxHealth; }
	int GetHealth() const                { return this->m_iHealth; }
	bool IsAlive() const                 { return (this->m_lifeState == LIFE_ALIVE); }
	CBaseEntity *GetGroundEntity() const { return this->m_hGroundEntity; }
	
	/* thunk */
	void CalcAbsolutePosition()                       {        ft_CalcAbsolutePosition(this); }
	bool ClassMatches(const char *pszClassOrWildcard) { return ft_ClassMatches        (this, pszClassOrWildcard); }
	
	/* hack */
	bool IsPlayer() const;
	
private:
	DECL_DATAMAP(IServerNetworkable, m_Network);
	DECL_DATAMAP(string_t,           m_iClassname);
	DECL_DATAMAP(string_t,           m_iName);
	DECL_DATAMAP(int,                m_iEFlags);
	DECL_DATAMAP(Vector,             m_vecAbsOrigin);
	
	DECL_SENDPROP(int,                  m_iTeamNum);
	DECL_SENDPROP(int,                  m_iMaxHealth);
	DECL_SENDPROP(int,                  m_iHealth);
	DECL_SENDPROP(char,                 m_lifeState);
	DECL_SENDPROP(CHandle<CBaseEntity>, m_hGroundEntity);
	
	static MemberFuncThunk<CBaseEntity *, void              > ft_CalcAbsolutePosition;
	static MemberFuncThunk<CBaseEntity *, bool, const char *> ft_ClassMatches;
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
