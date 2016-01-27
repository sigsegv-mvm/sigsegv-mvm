#include "mod.h"
#include "sm/detours.h"
#include "stub/stub.h"
#include "util/util.h"
#include "re/nextbot.h"


namespace Mod_EngieNoPush
{
	RefCount rc_TeleSpawn_Update;
	DETOUR_DECL_MEMBER(ActionResult<CTFBot>, CTFBotMvMEngineerTeleportSpawn_Update, CTFBot *actor, float dt)
	{
		SCOPED_INCREMENT(rc_TeleSpawn_Update);
		return DETOUR_MEMBER_CALL(CTFBotMvMEngineerTeleportSpawn_Update)(actor, dt);
	}
	
	RefCount rc_BuildSentry_Update;
	DETOUR_DECL_MEMBER(ActionResult<CTFBot>, CTFBotMvMEngineerBuildSentryGun_Update, CTFBot *actor, float dt)
	{
		SCOPED_INCREMENT(rc_BuildSentry_Update);
		return DETOUR_MEMBER_CALL(CTFBotMvMEngineerBuildSentryGun_Update)(actor, dt);
	}
	
	RefCount rc_BuildTele_Update;
	DETOUR_DECL_MEMBER(ActionResult<CTFBot>, CTFBotMvMEngineerBuildTeleportExit_Update, CTFBot *actor, float dt)
	{
		SCOPED_INCREMENT(rc_BuildTele_Update);
		return DETOUR_MEMBER_CALL(CTFBotMvMEngineerBuildTeleportExit_Update)(actor, dt);
	}
	
	/* would prefer to detour CTFGameRules::PushAllPlayersAway, but it's hard to reliably locate on Windows */
	DETOUR_DECL_MEMBER(void, CTFPlayer_ApplyAbsVelocityImpulse, const Vector *v1)
	{
		if (rc_TeleSpawn_Update > 0 || rc_BuildSentry_Update > 0 || rc_BuildTele_Update > 0) {
			return;
		}
		
		DETOUR_MEMBER_CALL(CTFPlayer_ApplyAbsVelocityImpulse)(v1);
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("EngieNoPush")
		{
			MOD_ADD_DETOUR_MEMBER(CTFBotMvMEngineerTeleportSpawn,     Update);
			MOD_ADD_DETOUR_MEMBER(CTFBotMvMEngineerBuildSentryGun,    Update);
			MOD_ADD_DETOUR_MEMBER(CTFBotMvMEngineerBuildTeleportExit, Update);
			MOD_ADD_DETOUR_MEMBER(CTFPlayer,                          ApplyAbsVelocityImpulse);
		}
		
		void SetEnabled(bool enable)
		{
			this->ToggleAllDetours(enable);
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sigsegv_engienopush_enable", "0", FCVAR_NOTIFY,
		"Mod: remove engiebot push force when spawning and building",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.SetEnabled(var.GetBool());
		});
}
