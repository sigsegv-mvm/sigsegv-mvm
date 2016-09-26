#ifndef _INCLUDE_SIGSEGV_STUB_BASEENTITY_H_
#define _INCLUDE_SIGSEGV_STUB_BASEENTITY_H_


#include "link/link.h"
#include "prop.h"


class CBaseCombatCharacter;


class CServerNetworkProperty : public IServerNetworkable {};


class CBaseEntity : public IServerEntity
{
public:
	/* inline */
	int entindex();
	const Vector& GetAbsOrigin() const;
	const QAngle& GetAbsAngles() const;
//	const Vector& GetAbsVelocity() const;
	bool IsEFlagSet(int nEFlagMask) const;
	const matrix3x4_t& EntityToWorldTransform() const;
	
	/* getter/setter */
	IServerNetworkable *GetNetworkable() const  { return &this->m_Network; }
	CServerNetworkProperty *NetworkProp() const { return &this->m_Network; }
	const char *GetClassname() const            { return STRING((string_t)this->m_iClassname); }
	string_t GetEntityName() const              { return this->m_iName; }
	void SetName(string_t newName)              { this->m_iName = newName; }
	ICollideable *GetCollideable() const        { return &this->m_Collision; }
	CCollisionProperty *CollisionProp() const   { return &this->m_Collision; }
	int GetTeamNumber() const                   { return this->m_iTeamNum; }
	int GetMaxHealth() const                    { return this->m_iMaxHealth; }
	int GetHealth() const                       { return this->m_iHealth; }
	bool IsAlive() const                        { return (this->m_lifeState == LIFE_ALIVE); }
	CBaseEntity *GetGroundEntity() const        { return this->m_hGroundEntity; }
	CBaseEntity *GetOwnerEntity() const         { return this->m_hOwnerEntity; }
	IPhysicsObject *VPhysicsGetObject() const   { return this->m_pPhysicsObject; }
	int GetFlags() const                        { return this->m_fFlags; }
	int GetCollisionGroup() const               { return this->m_CollisionGroup; }
	SolidType_t GetSolid() const                { return this->CollisionProp()->GetSolid(); }
	model_t *GetModel() const                   { return const_cast<model_t *>(modelinfo->GetModel(this->GetModelIndex())); }
	bool IsTransparent() const                  { return (this->m_nRenderMode != kRenderNormal); }
	MoveType_t GetMoveType() const              { return (MoveType_t)(unsigned char)this->m_MoveType; }
	
	/* thunk */
	void Remove()                                                                            {        ft_Remove                   (this); }
	void CalcAbsolutePosition()                                                              {        ft_CalcAbsolutePosition     (this); }
	bool ClassMatches(const char *pszClassOrWildcard)                                        { return ft_ClassMatches             (this, pszClassOrWildcard); }
	void SetAbsOrigin(const Vector& absOrigin)                                               {        ft_SetAbsOrigin             (this, absOrigin); }
	void SetAbsAngles(const QAngle& absAngles)                                               {        ft_SetAbsAngles             (this, absAngles); }
	void EmitSound(const char *soundname, float soundtime = 0.0f, float *duration = nullptr) {        ft_EmitSound                (this, soundname, soundtime, duration); }
	float GetNextThink(const char *szContext)                                                { return ft_GetNextThink             (this, szContext); }
	bool IsBSPModel() const                                                                  { return ft_IsBSPModel               (this); }
	Vector EyePosition()                                                                     { return vt_EyePosition              (this); }
	const QAngle& EyeAngles()                                                                { return vt_EyeAngles                (this); }
	void SetOwnerEntity(CBaseEntity *pOwner)                                                 {        vt_SetOwnerEntity           (this, pOwner); }
	void Spawn()                                                                             {        vt_Spawn                    (this); }
	void GetVelocity(Vector *vVelocity, AngularImpulse *vAngVelocity = nullptr)              {        vt_GetVelocity              (this, vVelocity, vAngVelocity); }
	const Vector& WorldSpaceCenter() const                                                   { return vt_WorldSpaceCenter         (this); }
	bool IsCombatItem() const                                                                { return vt_IsCombatItem             (this); }
	int GetModelIndex() const                                                                { return vt_GetModelIndex            (this); }
	CBaseCombatCharacter *MyCombatCharacterPointer()                                         { return vt_MyCombatCharacterPointer (this); }
	bool ShouldCollide(int collisionGroup, int contentsMask) const                           { return vt_ShouldCollide            (this, collisionGroup, contentsMask); }
	void DrawDebugGeometryOverlays()                                                         {        vt_DrawDebugGeometryOverlays(this); }
	
	/* hack */
	bool IsCombatCharacter() { return (this->MyCombatCharacterPointer() != nullptr); }
	bool IsPlayer() const;
	bool IsBaseObject() const;
	
	/* network vars */
	void NetworkStateChanged();
	void NetworkStateChanged(void *pVar);
	
	DECL_DATAMAP(int, m_debugOverlays);
	
	/* TODO: make me private again! */
	DECL_SENDPROP(int, m_fFlags);
	DECL_DATAMAP(int, m_nNextThinkTick);
	DECL_SENDPROP(char, m_lifeState);
	
private:
	DECL_DATAMAP(CServerNetworkProperty, m_Network);
	DECL_DATAMAP(string_t,               m_iClassname);
	DECL_DATAMAP(string_t,               m_iName);
	DECL_DATAMAP(int,                    m_iEFlags);
	DECL_DATAMAP(Vector,                 m_vecAbsOrigin);
	DECL_DATAMAP(QAngle,                 m_angAbsRotation);
	DECL_DATAMAP(Vector,                 m_vecAbsVelocity);
	DECL_DATAMAP(IPhysicsObject *,       m_pPhysicsObject);
	DECL_DATAMAP(matrix3x4_t,            m_rgflCoordinateFrame);
	
	DECL_SENDPROP_RW(CCollisionProperty,   m_Collision);
	DECL_SENDPROP   (int,                  m_iTeamNum);
	DECL_SENDPROP   (int,                  m_iMaxHealth);
	DECL_SENDPROP   (int,                  m_iHealth);
	DECL_SENDPROP   (CHandle<CBaseEntity>, m_hGroundEntity);
	DECL_SENDPROP   (CHandle<CBaseEntity>, m_hOwnerEntity);
	DECL_SENDPROP   (int,                  m_CollisionGroup);
	DECL_SENDPROP   (unsigned char,        m_nRenderMode);
	DECL_SENDPROP   (unsigned char,        m_MoveType);
	
	static MemberFuncThunk<      CBaseEntity *, void>                               ft_Remove;
	static MemberFuncThunk<      CBaseEntity *, void>                               ft_CalcAbsolutePosition;
	static MemberFuncThunk<      CBaseEntity *, bool, const char *>                 ft_ClassMatches;
	static MemberFuncThunk<      CBaseEntity *, void, const Vector&>                ft_SetAbsOrigin;
	static MemberFuncThunk<      CBaseEntity *, void, const QAngle&>                ft_SetAbsAngles;
	static MemberFuncThunk<      CBaseEntity *, void, const char *, float, float *> ft_EmitSound;
	static MemberFuncThunk<      CBaseEntity *, float, const char *>                ft_GetNextThink;
	static MemberFuncThunk<      CBaseEntity *, void, const Vector&, Vector *>      ft_EntityToWorldSpace;
	static MemberFuncThunk<const CBaseEntity *, bool>                               ft_IsBSPModel;
	
	static MemberVFuncThunk<      CBaseEntity *, Vector>                           vt_EyePosition;
	static MemberVFuncThunk<      CBaseEntity *, const QAngle&>                    vt_EyeAngles;
	static MemberVFuncThunk<      CBaseEntity *, void, CBaseEntity *>              vt_SetOwnerEntity;
	static MemberVFuncThunk<      CBaseEntity *, void>                             vt_Spawn;
	static MemberVFuncThunk<      CBaseEntity *, void, Vector *, AngularImpulse *> vt_GetVelocity;
	static MemberVFuncThunk<const CBaseEntity *, const Vector&>                    vt_WorldSpaceCenter;
	static MemberVFuncThunk<const CBaseEntity *, bool>                             vt_IsCombatItem;
	static MemberVFuncThunk<const CBaseEntity *, int>                              vt_GetModelIndex;
	static MemberVFuncThunk<      CBaseEntity *, CBaseCombatCharacter *>           vt_MyCombatCharacterPointer;
	static MemberVFuncThunk<const CBaseEntity *, bool, int, int>                   vt_ShouldCollide;
	static MemberVFuncThunk<      CBaseEntity *, void>                             vt_DrawDebugGeometryOverlays;
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

#if 0
inline const Vector& CBaseEntity::GetAbsVelocity() const
{
	if (this->IsEFlagSet(EFL_DIRTY_ABSVELOCITY)) {
		const_cast<CBaseEntity *>(this)->CalcAbsolutePosition();
	}
	return this->m_vecAbsVelocity;
}
#endif

inline bool CBaseEntity::IsEFlagSet(int nEFlagMask) const
{
	return (this->m_iEFlags & nEFlagMask) != 0;
}

inline const matrix3x4_t& CBaseEntity::EntityToWorldTransform() const
{
	if (this->IsEFlagSet(EFL_DIRTY_ABSTRANSFORM)) {
		const_cast<CBaseEntity *>(this)->CalcAbsolutePosition();
	}
	return this->m_rgflCoordinateFrame;
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
