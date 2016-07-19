#include "stub/baseentity.h"
#include "stub/baseplayer.h"
#include "stub/objects.h"


IMPL_DATAMAP(CServerNetworkProperty, CBaseEntity, m_Network);
IMPL_DATAMAP(string_t,               CBaseEntity, m_iClassname);
IMPL_DATAMAP(string_t,               CBaseEntity, m_iName);
IMPL_DATAMAP(int,                    CBaseEntity, m_iEFlags);
IMPL_DATAMAP(Vector,                 CBaseEntity, m_vecAbsOrigin);
IMPL_DATAMAP(QAngle,                 CBaseEntity, m_angAbsRotation);
IMPL_DATAMAP(Vector,                 CBaseEntity, m_vecAbsVelocity);
IMPL_DATAMAP(IPhysicsObject*,        CBaseEntity, m_pPhysicsObject);
IMPL_DATAMAP(matrix3x4_t,            CBaseEntity, m_rgflCoordinateFrame);
IMPL_DATAMAP(int,                    CBaseEntity, m_nNextThinkTick);

IMPL_SENDPROP(CCollisionProperty,   CBaseEntity, m_Collision,     CBaseEntity);
IMPL_SENDPROP(int,                  CBaseEntity, m_iTeamNum,      CBaseEntity);
IMPL_SENDPROP(int,                  CBaseEntity, m_iMaxHealth,    CBaseObject);
IMPL_SENDPROP(int,                  CBaseEntity, m_iHealth,       CBasePlayer);
IMPL_SENDPROP(char,                 CBaseEntity, m_lifeState,     CBasePlayer);
IMPL_SENDPROP(CHandle<CBaseEntity>, CBaseEntity, m_hGroundEntity, CBasePlayer);
IMPL_SENDPROP(CHandle<CBaseEntity>, CBaseEntity, m_hOwnerEntity,  CBaseEntity);
IMPL_SENDPROP(int,                  CBaseEntity, m_fFlags,        CBasePlayer);

MemberFuncThunk<CBaseEntity *, void                              > CBaseEntity::ft_Remove              ("CBaseEntity::Remove");
MemberFuncThunk<CBaseEntity *, void                              > CBaseEntity::ft_CalcAbsolutePosition("CBaseEntity::CalcAbsolutePosition");
MemberFuncThunk<CBaseEntity *, bool, const char *                > CBaseEntity::ft_ClassMatches        ("CBaseEntity::ClassMatches");
MemberFuncThunk<CBaseEntity *, void, const Vector&               > CBaseEntity::ft_SetAbsOrigin        ("CBaseEntity::SetAbsOrigin");
MemberFuncThunk<CBaseEntity *, void, const QAngle&               > CBaseEntity::ft_SetAbsAngles        ("CBaseEntity::SetAbsAngles");
MemberFuncThunk<CBaseEntity *, void, const char *, float, float *> CBaseEntity::ft_EmitSound           ("CBaseEntity::EmitSound [const char *, float, float *]");
MemberFuncThunk<CBaseEntity *, float, const char *               > CBaseEntity::ft_GetNextThink        ("CBaseEntity::GetNextThink");
MemberFuncThunk<CBaseEntity *, void, const Vector&, Vector *     > CBaseEntity::ft_EntityToWorldSpace  ("CBaseEntity::EntityToWorldSpace");

MemberVFuncThunk<      CBaseEntity *, Vector                          > CBaseEntity::vt_EyePosition             (TypeName<CBaseEntity>(), "CBaseEntity::EyePosition");
MemberVFuncThunk<      CBaseEntity *, const QAngle&                   > CBaseEntity::vt_EyeAngles               (TypeName<CBaseEntity>(), "CBaseEntity::EyeAngles");
MemberVFuncThunk<      CBaseEntity *, void, CBaseEntity *             > CBaseEntity::vt_SetOwnerEntity          (TypeName<CBaseEntity>(), "CBaseEntity::SetOwnerEntity");
MemberVFuncThunk<      CBaseEntity *, void                            > CBaseEntity::vt_Spawn                   (TypeName<CBaseEntity>(), "CBaseEntity::Spawn");
MemberVFuncThunk<      CBaseEntity *, void, Vector *, AngularImpulse *> CBaseEntity::vt_GetVelocity             (TypeName<CBaseEntity>(), "CBaseEntity::GetVelocity");
MemberVFuncThunk<const CBaseEntity *, const Vector&                   > CBaseEntity::vt_WorldSpaceCenter        (TypeName<CBaseEntity>(), "CBaseEntity::WorldSpaceCenter");
MemberVFuncThunk<const CBaseEntity *, bool                            > CBaseEntity::vt_IsCombatItem            (TypeName<CBaseEntity>(), "CBaseEntity::IsCombatItem");
MemberVFuncThunk<const CBaseEntity *, int                             > CBaseEntity::vt_GetModelIndex           (TypeName<CBaseEntity>(), "CBaseEntity::GetModelIndex");
MemberVFuncThunk<      CBaseEntity *, CBaseCombatCharacter *          > CBaseEntity::vt_MyCombatCharacterPointer(TypeName<CBaseEntity>(), "CBaseEntity::MyCombatCharacterPointer");


bool CBaseEntity::IsPlayer() const
{
	return (rtti_cast<const CBasePlayer *>(this) != nullptr);
}

bool CBaseEntity::IsBaseObject() const
{
	return (rtti_cast<const CBaseObject *>(this) != nullptr);
}


static StaticFuncThunk<CBaseEntity *, const char *, int> ft_CreateEntityByName("CreateEntityByName");
CBaseEntity *CreateEntityByName(const char *className, int iForceEdictIndex)
{
	return ft_CreateEntityByName(className, iForceEdictIndex);
}
