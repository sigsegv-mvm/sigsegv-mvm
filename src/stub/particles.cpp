//#include "stub/particles.h"
#include "link/link.h"


static StaticFuncThunk<void, const char *, ParticleAttachment_t, CBaseEntity *, const char *, bool> ft_DispatchParticleEffect1("DispatchParticleEffect [overload 1]");
void DispatchParticleEffect(const char *pszParticleName, ParticleAttachment_t iAttachType, CBaseEntity *pEntity, const char *pszAttachmentName, bool bResetAllParticlesOnEntity)
{
	ft_DispatchParticleEffect1(pszParticleName, iAttachType, pEntity, pszAttachmentName, bResetAllParticlesOnEntity);
}

static StaticFuncThunk<void, const char *, ParticleAttachment_t, CBaseEntity *, int, bool> ft_DispatchParticleEffect2("DispatchParticleEffect [overload 2]");
void DispatchParticleEffect(const char *pszParticleName, ParticleAttachment_t iAttachType, CBaseEntity *pEntity, int iAttachmentPoint, bool bResetAllParticlesOnEntity)
{
	ft_DispatchParticleEffect2(pszParticleName, iAttachType, pEntity, iAttachmentPoint, bResetAllParticlesOnEntity);
}

static StaticFuncThunk<void, const char *, Vector, QAngle, CBaseEntity *> ft_DispatchParticleEffect3("DispatchParticleEffect [overload 3]");
void DispatchParticleEffect(const char *pszParticleName, Vector vecOrigin, QAngle vecAngles, CBaseEntity *pEntity)
{
	ft_DispatchParticleEffect3(pszParticleName, vecOrigin, vecAngles, pEntity);
}

static StaticFuncThunk<void, const char *, Vector, Vector, QAngle, CBaseEntity *> ft_DispatchParticleEffect4("DispatchParticleEffect [overload 4]");
void DispatchParticleEffect(const char *pszParticleName, Vector vecOrigin, Vector vecStart, QAngle vecAngles, CBaseEntity *pEntity)
{
	ft_DispatchParticleEffect4(pszParticleName, vecOrigin, vecStart, vecAngles, pEntity);
}

static StaticFuncThunk<void, int, Vector, Vector, QAngle, CBaseEntity *> ft_DispatchParticleEffect5("DispatchParticleEffect [overload 5]");
void DispatchParticleEffect(int iEffectIndex, Vector vecOrigin, Vector vecStart, QAngle vecAngles, CBaseEntity *pEntity)
{
	ft_DispatchParticleEffect5(iEffectIndex, vecOrigin, vecStart, vecAngles, pEntity);
}

static StaticFuncThunk<void, const char *, ParticleAttachment_t, CBaseEntity *, const char *, Vector, Vector, bool, bool> ft_DispatchParticleEffect6("DispatchParticleEffect [overload 6]");
void DispatchParticleEffect(const char *pszParticleName, ParticleAttachment_t iAttachType, CBaseEntity *pEntity, const char *pszAttachmentName, Vector vecColor1, Vector vecColor2, bool bUseColors, bool bResetAllParticlesOnEntity)
{
	ft_DispatchParticleEffect6(pszParticleName, iAttachType, pEntity, pszAttachmentName, vecColor1, vecColor2, bUseColors, bResetAllParticlesOnEntity);
}

static StaticFuncThunk<void, const char *, Vector, QAngle, Vector, Vector, bool, CBaseEntity *, ParticleAttachment_t> ft_DispatchParticleEffect7("DispatchParticleEffect [overload 7]");
void DispatchParticleEffect(const char *pszParticleName, Vector vecOrigin, QAngle vecAngles, Vector vecColor1, Vector vecColor2, bool bUseColors, CBaseEntity *pEntity, ParticleAttachment_t iAttachType)
{
	ft_DispatchParticleEffect7(pszParticleName, vecOrigin, vecAngles, vecColor1, vecColor2, bUseColors, pEntity, iAttachType);
}

static StaticFuncThunk<void, CBaseEntity *> ft_StopParticleEffects("StopParticleEffects");
void StopParticleEffects(CBaseEntity *pEntity)
{
	ft_StopParticleEffects(pEntity);
}
