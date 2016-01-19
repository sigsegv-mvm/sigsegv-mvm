#include "mod.h"
#include "sm/detours.h"
#include "stub/stub.h"
#include "util/util.h"
#include "re/nextbot.h"


static RefCount rc_TeleSpawn_Update;
DETOUR_DECL_MEMBER(ActionResult<CTFBot>, CTFBotMvMEngineerTeleportSpawn_Update, CTFBot *actor, float dt)
{
	SCOPED_INCREMENT(rc_TeleSpawn_Update);
	return DETOUR_MEMBER_CALL(CTFBotMvMEngineerTeleportSpawn_Update)(actor, dt);
}

static RefCount rc_BuildSentry_Update;
DETOUR_DECL_MEMBER(ActionResult<CTFBot>, CTFBotMvMEngineerBuildSentryGun_Update, CTFBot *actor, float dt)
{
	SCOPED_INCREMENT(rc_BuildSentry_Update);
	return DETOUR_MEMBER_CALL(CTFBotMvMEngineerBuildSentryGun_Update)(actor, dt);
}

static RefCount rc_BuildTele_Update;
DETOUR_DECL_MEMBER(ActionResult<CTFBot>, CTFBotMvMEngineerBuildTeleportExit_Update, CTFBot *actor, float dt)
{
	SCOPED_INCREMENT(rc_BuildTele_Update);
	return DETOUR_MEMBER_CALL(CTFBotMvMEngineerBuildTeleportExit_Update)(actor, dt);
}

#if 0
DETOUR_DECL_MEMBER(void, CTFGameRules_PushAllPlayersAway, const Vector *v1, float f1, float f2, int i1, CUtlVector<CTFPlayer *> *vec)
{
	if (rc_TeleSpawn_Update.NonZero() || rc_BuildSentry_Update.NonZero() || rc_BuildTele_Update.NonZero()) {
		return;
	}
	
	DETOUR_MEMBER_CALL(CTFGameRules_PushAllPlayersAway)(v1, f1, f2, i1, vec);
}
#endif

DETOUR_DECL_MEMBER(void, CTFPlayer_ApplyAbsVelocityImpulse, const Vector *v1)
{
	if (rc_TeleSpawn_Update.NonZero() || rc_BuildSentry_Update.NonZero() || rc_BuildTele_Update.NonZero()) {
		return;
	}
	
	DETOUR_MEMBER_CALL(CTFPlayer_ApplyAbsVelocityImpulse)(v1);
}


class CMod_EngieNoPush : public IMod
{
public:
	CMod_EngieNoPush() : IMod("EngieNoPush")
	{
		MOD_ADD_DETOUR_MEMBER(CTFBotMvMEngineerTeleportSpawn, Update);
		MOD_ADD_DETOUR_MEMBER(CTFBotMvMEngineerBuildSentryGun, Update);
		MOD_ADD_DETOUR_MEMBER(CTFBotMvMEngineerBuildTeleportExit, Update);
		//MOD_ADD_DETOUR_MEMBER(CTFGameRules, PushAllPlayersAway);
		MOD_ADD_DETOUR_MEMBER(CTFPlayer, ApplyAbsVelocityImpulse);
	}
	
	void SetEnabled(bool enable)
	{
		this->ToggleAllDetours(enable);
	}
};
static CMod_EngieNoPush s_Mod;


static ConVar cvar_enable("sigsegv_engienopush_enable", "0", FCVAR_NOTIFY,
	"Mod: remove engiebot push force when spawning and building",
	[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
		ConVarRef var(pConVar);
		s_Mod.SetEnabled(var.GetBool());
	});
