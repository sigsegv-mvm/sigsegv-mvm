#include "stub/baseentity.h"
#include "stub/tfplayer.h"
#include "stub/objects.h"


IMPL_DATAMAP(CServerNetworkProperty, CBaseEntity, m_Network);
IMPL_DATAMAP(string_t,               CBaseEntity, m_iClassname);
IMPL_DATAMAP(string_t,               CBaseEntity, m_iName);
IMPL_DATAMAP(int,                    CBaseEntity, m_iEFlags);
IMPL_DATAMAP(Vector,                 CBaseEntity, m_vecAbsOrigin);
IMPL_DATAMAP(QAngle,                 CBaseEntity, m_angAbsRotation);

IMPL_SENDPROP(CCollisionProperty,   CBaseEntity, m_Collision,     CBaseEntity);
IMPL_SENDPROP(int,                  CBaseEntity, m_iTeamNum,      CBaseEntity);
IMPL_SENDPROP(int,                  CBaseEntity, m_iMaxHealth,    CBaseObject);
IMPL_SENDPROP(int,                  CBaseEntity, m_iHealth,       CBasePlayer);
IMPL_SENDPROP(char,                 CBaseEntity, m_lifeState,     CBasePlayer);
IMPL_SENDPROP(CHandle<CBaseEntity>, CBaseEntity, m_hGroundEntity, CBasePlayer);
IMPL_SENDPROP(CHandle<CBaseEntity>, CBaseEntity, m_hOwnerEntity,  CBaseEntity);

MemberFuncThunk<CBaseEntity *, void                              > CBaseEntity::ft_Remove              ("CBaseEntity::Remove");
MemberFuncThunk<CBaseEntity *, void                              > CBaseEntity::ft_CalcAbsolutePosition("CBaseEntity::CalcAbsolutePosition");
MemberFuncThunk<CBaseEntity *, bool, const char *                > CBaseEntity::ft_ClassMatches        ("CBaseEntity::ClassMatches");
MemberFuncThunk<CBaseEntity *, void, const Vector&               > CBaseEntity::ft_SetAbsOrigin        ("CBaseEntity::SetAbsOrigin");
MemberFuncThunk<CBaseEntity *, void, const QAngle&               > CBaseEntity::ft_SetAbsAngles        ("CBaseEntity::SetAbsAngles");
MemberFuncThunk<CBaseEntity *, void, const char *, float, float *> CBaseEntity::ft_EmitSound           ("CBaseEntity::EmitSound");

MemberVFuncThunk<CBaseEntity *, Vector             > CBaseEntity::vt_EyePosition   (TypeName<CBaseEntity>(), "CBaseEntity::EyePosition");
MemberVFuncThunk<CBaseEntity *, const QAngle&      > CBaseEntity::vt_EyeAngles     (TypeName<CBaseEntity>(), "CBaseEntity::EyeAngles");
MemberVFuncThunk<CBaseEntity *, void, CBaseEntity *> CBaseEntity::vt_SetOwnerEntity(TypeName<CBaseEntity>(), "CBaseEntity::SetOwnerEntity");
MemberVFuncThunk<CBaseEntity *, void               > CBaseEntity::vt_Spawn         (TypeName<CBaseEntity>(), "CBaseEntity::Spawn");


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
