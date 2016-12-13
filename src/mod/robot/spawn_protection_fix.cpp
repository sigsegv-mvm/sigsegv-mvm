#include "mod.h"
#include "stub/tfbot.h"
#include "util/scope.h"


namespace Mod_Robot_Spawn_Protection_Fix
{
	RefCount rc_CTFBotMainAction_Update;
	DETOUR_DECL_MEMBER(ActionResult<CTFBot>, CTFBotMainAction_Update, CTFBot *actor, float dt)
	{
		SCOPED_INCREMENT(rc_CTFBotMainAction_Update);
		return DETOUR_MEMBER_CALL(CTFBotMainAction_Update)(actor, dt);
	}
	
	DETOUR_DECL_MEMBER(void, CTFPlayerShared_AddCond, ETFCond nCond, float flDuration, CBaseEntity *pProvider)
	{
		if (rc_CTFBotMainAction_Update > 0 &&
			(nCond == TF_COND_INVULNERABLE || nCond == TF_COND_INVULNERABLE_WEARINGOFF) &&
			flDuration == 0.5f && pProvider == nullptr) {
			return;
		}
		
	//	#warning REMOVE THIS DEBUG MESSAGE!
	//	DevMsg("[SpawnProtect] rc == %d; nCond == %d; flDuration %c= 0.5f; pProvider %c= nullptr\n",
	//		rc_CTFBotMainAction_Update, (int)nCond, (flDuration == 0.5f ? '=' : '!'), (pProvider == nullptr ? '=' : '!'));
		
		DETOUR_MEMBER_CALL(CTFPlayerShared_AddCond)(nCond, flDuration, pProvider);
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Robot:Spawn_Protection_Fix")
		{
			MOD_ADD_DETOUR_MEMBER(CTFBotMainAction_Update, "CTFBotMainAction::Update");
			MOD_ADD_DETOUR_MEMBER(CTFPlayerShared_AddCond, "CTFPlayerShared::AddCond");
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_robot_spawn_protection_fix", "0", FCVAR_NOTIFY,
		"Mod: make robot spawn protection only involve TF_COND_INVULNERABLE_HIDE_UNLESS_DAMAGED",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
