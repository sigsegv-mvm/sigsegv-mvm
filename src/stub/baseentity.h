#ifndef _INCLUDE_SIGSEGV_STUB_BASEENTITY_H_
#define _INCLUDE_SIGSEGV_STUB_BASEENTITY_H_


#include "link/link.h"
#include "prop.h"


class CServerNetworkProperty : public IServerNetworkable {};


class CBaseEntity
{
public:
	/* inline */
	int entindex();
	const Vector& GetAbsOrigin() const;
	const QAngle& GetAbsAngles() const;
	bool IsEFlagSet(int nEFlagMask) const;
	
	/* getter/setter */
	IServerNetworkable *GetNetworkable() const  { return this->m_Network.GetPtr(); }
	CServerNetworkProperty *NetworkProp() const { return this->m_Network.GetPtr(); }
	const char *GetClassname() const            { return STRING((string_t)this->m_iClassname); }
	string_t GetEntityName() const              { return this->m_iName; }
	void SetName(string_t newName)              { this->m_iName = newName; }
	ICollideable *GetCollideable() const        { return this->m_Collision.GetPtr(); }
	CCollisionProperty *CollisionProp() const   { return this->m_Collision.GetPtr(); }
	int GetTeamNumber() const                   { return this->m_iTeamNum; }
	int GetMaxHealth() const                    { return this->m_iMaxHealth; }
	int GetHealth() const                       { return this->m_iHealth; }
	bool IsAlive() const                        { return (this->m_lifeState == LIFE_ALIVE); }
	CBaseEntity *GetGroundEntity() const        { return this->m_hGroundEntity; }
	CBaseEntity *GetOwnerEntity() const         { return this->m_hOwnerEntity; }
	
	/* thunk */
	void Remove()                                     {        ft_Remove              (this); }
	void CalcAbsolutePosition()                       {        ft_CalcAbsolutePosition(this); }
	bool ClassMatches(const char *pszClassOrWildcard) { return ft_ClassMatches        (this, pszClassOrWildcard); }
	void SetAbsOrigin(const Vector& absOrigin)        {        ft_SetAbsOrigin        (this, absOrigin); }
	void SetAbsAngles(const QAngle& absAngles)        {        ft_SetAbsAngles        (this, absAngles); }
	Vector EyePosition()                              { return vt_EyePosition         (this); }
	const QAngle& EyeAngles()                         { return vt_EyeAngles           (this); }
	void SetOwnerEntity(CBaseEntity *pOwner)          {        vt_SetOwnerEntity      (this, pOwner); }
	void Spawn()                                      {        vt_Spawn               (this); }
	
	/* hack */
	bool IsPlayer() const;
	bool IsBaseObject() const;
	
	/* network vars */
	void NetworkStateChanged();
	void NetworkStateChanged(void *pVar);
	
private:
	DECL_DATAMAP(CServerNetworkProperty, m_Network);
	DECL_DATAMAP(string_t,               m_iClassname);
	DECL_DATAMAP(string_t,               m_iName);
	DECL_DATAMAP(int,                    m_iEFlags);
	DECL_DATAMAP(Vector,                 m_vecAbsOrigin);
	DECL_DATAMAP(QAngle,                 m_angAbsRotation);
	
	DECL_SENDPROP(CCollisionProperty,   m_Collision);
	DECL_SENDPROP(int,                  m_iTeamNum);
	DECL_SENDPROP(int,                  m_iMaxHealth);
	DECL_SENDPROP(int,                  m_iHealth);
	DECL_SENDPROP(char,                 m_lifeState);
	DECL_SENDPROP(CHandle<CBaseEntity>, m_hGroundEntity);
	DECL_SENDPROP(CHandle<CBaseEntity>, m_hOwnerEntity);
	
	static MemberFuncThunk<CBaseEntity *, void>                ft_Remove;
	static MemberFuncThunk<CBaseEntity *, void>                ft_CalcAbsolutePosition;
	static MemberFuncThunk<CBaseEntity *, bool, const char *>  ft_ClassMatches;
	static MemberFuncThunk<CBaseEntity *, void, const Vector&> ft_SetAbsOrigin;
	static MemberFuncThunk<CBaseEntity *, void, const QAngle&> ft_SetAbsAngles;
	
	static MemberVFuncThunk<CBaseEntity *, Vector>              vt_EyePosition;
	static MemberVFuncThunk<CBaseEntity *, const QAngle&>       vt_EyeAngles;
	static MemberVFuncThunk<CBaseEntity *, void, CBaseEntity *> vt_SetOwnerEntity;
	static MemberVFuncThunk<CBaseEntity *, void>                vt_Spawn;
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

inline const QAngle& CBaseEntity::GetAbsAngles() const
{
	if (this->IsEFlagSet(EFL_DIRTY_ABSTRANSFORM)) {
		const_cast<CBaseEntity *>(this)->CalcAbsolutePosition();
	}
	return this->m_angAbsRotation;
}

inline bool CBaseEntity::IsEFlagSet(int nEFlagMask) const
{
	return (this->m_iEFlags & nEFlagMask) != 0;
}


inline void CBaseEntity::NetworkStateChanged()
{
	gamehelpers->SetEdictStateChanged(this->GetNetworkable()->GetEdict(), 0);
}

inline void CBaseEntity::NetworkStateChanged(void *pVar)
{
	gamehelpers->SetEdictStateChanged(this->GetNetworkable()->GetEdict(), ((uintptr_t)pVar - (uintptr_t)this));
}


CBaseEntity *CreateEntityByName(const char *className, int iForceEdictIndex = -1);


#endif
