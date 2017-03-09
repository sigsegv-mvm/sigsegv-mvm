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
	edict_t *edict() const { return this->NetworkProp()->GetEdict(); }
	int entindex() const;
	const Vector& GetAbsOrigin() const;
	const QAngle& GetAbsAngles() const;
	const Vector& GetAbsVelocity() const;
	bool IsEFlagSet(int nEFlagMask) const;
	const matrix3x4_t& EntityToWorldTransform() const;
	bool NameMatches(const char *pszNameOrWildcard);
	void SetModel(const char *szModelName);
	bool ClassMatches(const char *pszClassOrWildcard);
	
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
	void SetHealth(int amt)                     { this->m_iHealth = amt; }
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
	MoveCollide_t GetMoveCollide() const        { return (MoveCollide_t)(unsigned char)this->m_MoveCollide; }
	void SetMoveCollide(MoveCollide_t val)      { this->m_MoveCollide = val; }
	CBaseEntity *GetMoveParent()                { return this->m_hMoveParent; }
	CBaseEntity *FirstMoveChild()               { return this->m_hMoveChild; }
	CBaseEntity *NextMovePeer()                 { return this->m_hMovePeer; }
	const color32 GetRenderColor() const        { return this->m_clrRender; }
	void SetRenderColorR(byte r)                { this->m_clrRender->r = r; }
	void SetRenderColorG(byte g)                { this->m_clrRender->g = g; }
	void SetRenderColorB(byte b)                { this->m_clrRender->b = b; }
	void SetRenderColorA(byte a)                { this->m_clrRender->a = a; }
	bool IsMarkedForDeletion() const            { return ((this->m_iEFlags & EFL_KILLME) != 0); }
	
	/* thunk */
	void Remove()                                                                                                           {        ft_Remove                   (this); }
	void CalcAbsolutePosition()                                                                                             {        ft_CalcAbsolutePosition     (this); }
	void CalcAbsoluteVelocity()                                                                                             {        ft_CalcAbsoluteVelocity     (this); }
	bool NameMatchesComplex(const char *pszNameOrWildcard)                                                                  { return ft_NameMatchesComplex       (this, pszNameOrWildcard); }
	bool ClassMatchesComplex(const char *pszClassOrWildcard)                                                                { return ft_ClassMatchesComplex      (this, pszClassOrWildcard); }
	void SetAbsOrigin(const Vector& absOrigin)                                                                              {        ft_SetAbsOrigin             (this, absOrigin); }
	void SetAbsAngles(const QAngle& absAngles)                                                                              {        ft_SetAbsAngles             (this, absAngles); }
	void EmitSound(const char *soundname, float soundtime = 0.0f, float *duration = nullptr)                                {        ft_EmitSound                (this, soundname, soundtime, duration); }
	float GetNextThink(const char *szContext)                                                                               { return ft_GetNextThink             (this, szContext); }
	bool IsBSPModel() const                                                                                                 { return ft_IsBSPModel               (this); }
	void EntityText(int text_offset, const char *text, float duration, int r, int g, int b, int a)                          {        ft_EntityText               (this, text_offset, text, duration, r, g, b, a); }
	int TakeDamage(const CTakeDamageInfo& info)                                                                             { return ft_TakeDamage               (this, info); }
	void SetMoveType(MoveType_t val, MoveCollide_t moveCollide = MOVECOLLIDE_DEFAULT)                                       {        ft_SetMoveType              (this, val, moveCollide); }
	model_t *GetModel()                                                                                                     { return ft_GetModel                 (this); }
	Vector EyePosition()                                                                                                    { return vt_EyePosition              (this); }
	const QAngle& EyeAngles()                                                                                               { return vt_EyeAngles                (this); }
	void SetOwnerEntity(CBaseEntity *pOwner)                                                                                {        vt_SetOwnerEntity           (this, pOwner); }
	void Spawn()                                                                                                            {        vt_Spawn                    (this); }
	void GetVelocity(Vector *vVelocity, AngularImpulse *vAngVelocity = nullptr)                                             {        vt_GetVelocity              (this, vVelocity, vAngVelocity); }
	const Vector& WorldSpaceCenter() const                                                                                  { return vt_WorldSpaceCenter         (this); }
	bool IsCombatItem() const                                                                                               { return vt_IsCombatItem             (this); }
	void SetModelIndex(int index)                                                                                           {        vt_SetModelIndex            (this, index); }
	int GetModelIndex() const                                                                                               { return vt_GetModelIndex            (this); }
	string_t GetModelName() const                                                                                           { return vt_GetModelName             (this); }
	CBaseCombatCharacter *MyCombatCharacterPointer()                                                                        { return vt_MyCombatCharacterPointer (this); }
	bool ShouldCollide(int collisionGroup, int contentsMask) const                                                          { return vt_ShouldCollide            (this, collisionGroup, contentsMask); }
	void DrawDebugGeometryOverlays()                                                                                        {        vt_DrawDebugGeometryOverlays(this); }
	void ChangeTeam(int iTeamNum)                                                                                           {        vt_ChangeTeam               (this, iTeamNum); }
	void SetModelIndexOverride(int index, int nValue)                                                                       {        vt_SetModelIndexOverride    (this, index, nValue); }
	datamap_t *GetDataDescMap()                                                                                             { return vt_GetDataDescMap           (this); }
	bool AcceptInput(const char *szInputName, CBaseEntity *pActivator, CBaseEntity *pCaller, variant_t Value, int outputID) { return vt_AcceptInput              (this, szInputName, pActivator, pCaller, Value, outputID); }
	
	/* static */
	static int PrecacheModel(const char *name, bool bPreload = true)     { return ft_PrecacheModel      (name, bPreload); }
	static bool PrecacheSound(const char *name)                          { return ft_PrecacheSound      (name); }
	static HSOUNDSCRIPTHANDLE PrecacheScriptSound(const char *soundname) { return ft_PrecacheScriptSound(soundname); }
	
	/* hack */
	bool IsCombatCharacter() { return (this->MyCombatCharacterPointer() != nullptr); }
	bool IsPlayer() const;
	bool IsBaseObject() const;
	
	/* network vars */
	void NetworkStateChanged();
	void NetworkStateChanged(void *pVar);
	
	DECL_DATAMAP(int, m_debugOverlays);
	
	/* TODO: make me private again! */
	DECL_SENDPROP(int,    m_fFlags);
	DECL_DATAMAP(int,     m_nNextThinkTick);
	DECL_SENDPROP(char,   m_lifeState);
	DECL_SENDPROP(int[4], m_nModelIndexOverrides);
	
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
	DECL_DATAMAP(CHandle<CBaseEntity>,   m_hMoveChild);
	DECL_DATAMAP(CHandle<CBaseEntity>,   m_hMovePeer);
	DECL_DATAMAP(CHandle<CBaseEntity>,   m_hMoveParent);
	
	DECL_SENDPROP_RW(CCollisionProperty,   m_Collision);
	DECL_SENDPROP   (int,                  m_iTeamNum);
	DECL_SENDPROP   (int,                  m_iMaxHealth);
	DECL_SENDPROP   (int,                  m_iHealth);
	DECL_SENDPROP   (CHandle<CBaseEntity>, m_hGroundEntity);
	DECL_SENDPROP   (CHandle<CBaseEntity>, m_hOwnerEntity);
	DECL_SENDPROP   (int,                  m_CollisionGroup);
	DECL_SENDPROP   (unsigned char,        m_nRenderMode);
	DECL_SENDPROP   (unsigned char,        m_MoveType);
	DECL_SENDPROP   (unsigned char,        m_MoveCollide);
	DECL_SENDPROP_RW(color32,              m_clrRender);
	
	static MemberFuncThunk<      CBaseEntity *, void>                                               ft_Remove;
	static MemberFuncThunk<      CBaseEntity *, void>                                               ft_CalcAbsolutePosition;
	static MemberFuncThunk<      CBaseEntity *, void>                                               ft_CalcAbsoluteVelocity;
	static MemberFuncThunk<      CBaseEntity *, bool, const char *>                                 ft_NameMatchesComplex;
	static MemberFuncThunk<      CBaseEntity *, bool, const char *>                                 ft_ClassMatchesComplex;
	static MemberFuncThunk<      CBaseEntity *, void, const Vector&>                                ft_SetAbsOrigin;
	static MemberFuncThunk<      CBaseEntity *, void, const QAngle&>                                ft_SetAbsAngles;
	static MemberFuncThunk<      CBaseEntity *, void, const char *, float, float *>                 ft_EmitSound;
	static MemberFuncThunk<      CBaseEntity *, float, const char *>                                ft_GetNextThink;
	static MemberFuncThunk<      CBaseEntity *, void, const Vector&, Vector *>                      ft_EntityToWorldSpace;
	static MemberFuncThunk<const CBaseEntity *, bool>                                               ft_IsBSPModel;
	static MemberFuncThunk<      CBaseEntity *, void, int, const char *, float, int, int, int, int> ft_EntityText;
	static MemberFuncThunk<      CBaseEntity *, int, const CTakeDamageInfo&>                        ft_TakeDamage;
	static MemberFuncThunk<      CBaseEntity *, void, MoveType_t, MoveCollide_t>                    ft_SetMoveType;
	static MemberFuncThunk<      CBaseEntity *, model_t *>                                          ft_GetModel;
	
	static MemberVFuncThunk<      CBaseEntity *, Vector>                                                           vt_EyePosition;
	static MemberVFuncThunk<      CBaseEntity *, const QAngle&>                                                    vt_EyeAngles;
	static MemberVFuncThunk<      CBaseEntity *, void, CBaseEntity *>                                              vt_SetOwnerEntity;
	static MemberVFuncThunk<      CBaseEntity *, void>                                                             vt_Spawn;
	static MemberVFuncThunk<      CBaseEntity *, void, Vector *, AngularImpulse *>                                 vt_GetVelocity;
	static MemberVFuncThunk<const CBaseEntity *, const Vector&>                                                    vt_WorldSpaceCenter;
	static MemberVFuncThunk<const CBaseEntity *, bool>                                                             vt_IsCombatItem;
	static MemberVFuncThunk<      CBaseEntity *, void, int>                                                        vt_SetModelIndex;
	static MemberVFuncThunk<const CBaseEntity *, int>                                                              vt_GetModelIndex;
	static MemberVFuncThunk<const CBaseEntity *, string_t>                                                         vt_GetModelName;
	static MemberVFuncThunk<      CBaseEntity *, CBaseCombatCharacter *>                                           vt_MyCombatCharacterPointer;
	static MemberVFuncThunk<const CBaseEntity *, bool, int, int>                                                   vt_ShouldCollide;
	static MemberVFuncThunk<      CBaseEntity *, void>                                                             vt_DrawDebugGeometryOverlays;
	static MemberVFuncThunk<      CBaseEntity *, void, int>                                                        vt_ChangeTeam;
	static MemberVFuncThunk<      CBaseEntity *, void, int, int>                                                   vt_SetModelIndexOverride;
	static MemberVFuncThunk<      CBaseEntity *, datamap_t *>                                                      vt_GetDataDescMap;
	static MemberVFuncThunk<      CBaseEntity *, bool, const char *, CBaseEntity *, CBaseEntity *, variant_t, int> vt_AcceptInput;
	static MemberVFuncThunk<      CBaseEntity *, void, const char *>                                               vt_SetModel;
	
	static StaticFuncThunk<int, const char *, bool>          ft_PrecacheModel;
	static StaticFuncThunk<bool, const char *>               ft_PrecacheSound;
	static StaticFuncThunk<HSOUNDSCRIPTHANDLE, const char *> ft_PrecacheScriptSound;
};

inline CBaseEntity *GetContainingEntity(edict_t *pent)
{
	if (pent != nullptr && pent->GetUnknown() != nullptr) {
		return pent->GetUnknown()->GetBaseEntity();
	}
	
	return nullptr;
}

inline int ENTINDEX(const edict_t *pEdict)
{
	return gamehelpers->IndexOfEdict(const_cast<edict_t *>(pEdict));
}

inline int ENTINDEX(const CBaseEntity *pEnt)
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

inline bool FNullEnt(const edict_t *pent)
{
	return (pent == nullptr || ENTINDEX(pent) == 0);
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


inline int CBaseEntity::entindex() const
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

inline const Vector& CBaseEntity::GetAbsVelocity() const
{
	if (this->IsEFlagSet(EFL_DIRTY_ABSVELOCITY)) {
		const_cast<CBaseEntity *>(this)->CalcAbsoluteVelocity();
	}
	return this->m_vecAbsVelocity;
}

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

inline bool CBaseEntity::NameMatches(const char *pszNameOrWildcard)
{
	return (IDENT_STRINGS(this->m_iName, pszNameOrWildcard) || this->NameMatchesComplex(pszNameOrWildcard));
}

inline void CBaseEntity::SetModel(const char *szModelName)
{
	/* ensure that we don't accidentally run into the fatal Error statement in
	 * UTIL_SetModel */
	CBaseEntity::PrecacheModel(szModelName);
	
	/* now do the vcall */
	vt_SetModel(this, szModelName);
}

inline bool CBaseEntity::ClassMatches(const char *pszClassOrWildcard)
{
	return (IDENT_STRINGS(this->m_iClassname, pszClassOrWildcard) || this->ClassMatchesComplex(pszClassOrWildcard));
}


inline void CBaseEntity::NetworkStateChanged()
{
	gamehelpers->SetEdictStateChanged(this->GetNetworkable()->GetEdict(), 0);
}

inline void CBaseEntity::NetworkStateChanged(void *pVar)
{
	gamehelpers->SetEdictStateChanged(this->GetNetworkable()->GetEdict(), ((uintptr_t)pVar - (uintptr_t)this));
}


inline CBaseEntity *CreateEntityByName(const char *szClassname)
{
	return servertools->CreateEntityByName(szClassname);
}

inline void DispatchSpawn(CBaseEntity *pEntity)
{
	return servertools->DispatchSpawn(pEntity);
}


#endif
