#include "mod.h"
#include "detours.h"
#include "stub_gamerules.h"
#include "stub_nextbot_behavior.h"
#include "stub_misc.h"
#include "util.h"


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


#define MOD_CLASS CMod_EngieNoPush
#define MOD_NAME "EngieNoPush"

class MOD_CLASS : public IMod
{
public:
	MOD_CLASS() : IMod(MOD_NAME) {}
	
	virtual bool OnLoad(char *error, size_t maxlen) override
	{
		MOD_DETOUR_CREATE_MEMBER(CTFBotMvMEngineerTeleportSpawn, Update);
		MOD_DETOUR_CREATE_MEMBER(CTFBotMvMEngineerBuildSentryGun, Update);
		MOD_DETOUR_CREATE_MEMBER(CTFBotMvMEngineerBuildTeleportExit, Update);
		MOD_DETOUR_CREATE_MEMBER(CTFGameRules, PushAllPlayersAway);
		
		return true;
	}
	
	virtual void OnUnload() override
	{
		MOD_DETOUR_DESTROY_MEMBER(CTFGameRules, PushAllPlayersAway);
		MOD_DETOUR_DESTROY_MEMBER(CTFBotMvMEngineerBuildTeleportExit, Update);
		MOD_DETOUR_DESTROY_MEMBER(CTFBotMvMEngineerBuildSentryGun, Update);
		MOD_DETOUR_DESTROY_MEMBER(CTFBotMvMEngineerTeleportSpawn, Update);
	}
	
	void SetEnabled(bool enable)
	{
		MOD_DETOUR_TOGGLE_MEMBER(enable, CTFBotMvMEngineerTeleportSpawn, Update);
		MOD_DETOUR_TOGGLE_MEMBER(enable, CTFBotMvMEngineerBuildSentryGun, Update);
		MOD_DETOUR_TOGGLE_MEMBER(enable, CTFBotMvMEngineerBuildTeleportExit, Update);
		MOD_DETOUR_TOGGLE_MEMBER(enable, CTFGameRules, PushAllPlayersAway);
		
		this->m_bEnabled = enable;
	}
	
private:
	bool m_bEnabled = false;
};
static MOD_CLASS s_Mod;


static ConVar cvar_enable("sigsegv_engienopush_enable", "0", FCVAR_NOTIFY,
	"Mod: remove engiebot push force when spawning and building",
	[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
		ConVarRef var(pConVar);
		s_Mod.SetEnabled(var.GetBool());
	});
