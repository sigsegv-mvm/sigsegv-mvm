#include "mod.h"
#include "sdk/detours.h"
#include "stub/stub.h"
#include "util/util.h"
#include "re_nextbot.h"


static RefCount s_rcTeleSpawn;
DETOUR_DECL_MEMBER(ActionResult<CTFBot>, CTFBotMvMEngineerTeleportSpawn_Update, CTFBot *actor, float dt)
{
	SCOPED_INCREMENT(s_rcTeleSpawn);
	return DETOUR_MEMBER_CALL(CTFBotMvMEngineerTeleportSpawn_Update)(actor, dt);
}

static RefCount s_rcBuildSentry;
DETOUR_DECL_MEMBER(ActionResult<CTFBot>, CTFBotMvMEngineerBuildSentryGun_Update, CTFBot *actor, float dt)
{
	SCOPED_INCREMENT(s_rcBuildSentry);
	return DETOUR_MEMBER_CALL(CTFBotMvMEngineerBuildSentryGun_Update)(actor, dt);
}

static RefCount s_rcBuildTele;
DETOUR_DECL_MEMBER(ActionResult<CTFBot>, CTFBotMvMEngineerBuildTeleportExit_Update, CTFBot *actor, float dt)
{
	SCOPED_INCREMENT(s_rcBuildTele);
	return DETOUR_MEMBER_CALL(CTFBotMvMEngineerBuildTeleportExit_Update)(actor, dt);
}

DETOUR_DECL_MEMBER(void, CTFGameRules_PushAllPlayersAway, const Vector *v1, float f1, float f2, int i1, CUtlVector<CTFPlayer *> *vec)
{
	if (s_rcTeleSpawn.NonZero() || s_rcBuildSentry.NonZero() || s_rcBuildTele.NonZero()) {
		return;
	}
	
	DETOUR_MEMBER_CALL(CTFGameRules_PushAllPlayersAway)(v1, f1, f2, i1, vec);
}


class CMod_EngieNoPush : public IMod
{
public:
	CMod_EngieNoPush() : IMod("EngieNoPush")
	{
		MOD_ADD_DETOUR_MEMBER(CTFBotMvMEngineerTeleportSpawn, Update);
		MOD_ADD_DETOUR_MEMBER(CTFBotMvMEngineerBuildSentryGun, Update);
		MOD_ADD_DETOUR_MEMBER(CTFBotMvMEngineerBuildTeleportExit, Update);
		MOD_ADD_DETOUR_MEMBER(CTFGameRules, PushAllPlayersAway);
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
