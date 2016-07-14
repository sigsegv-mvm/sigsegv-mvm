#include "mod.h"
#include "re/nextbot.h"
#include "stub/tfbot_behavior.h"


class CTFBotTacticalMonitor : public Action<CTFBot> {};


namespace Mod_Debug_OnKilled_Before_MainAction_Start
{
	ConVar cvar_fix_onstart("sig_debug_onkilled_before_mainaction_start_fix_onstart", "0", FCVAR_NOTIFY,
		"Debug: check for non-alive-ness in CTFBotMainAction::OnStart");
	DETOUR_DECL_MEMBER(ActionResult<CTFBot>, CTFBotMainAction_OnStart, CTFBot *actor, Action<CTFBot> *action)
	{
		if (cvar_fix_onstart.GetBool() && !actor->IsAlive()) {
			DevMsg("\n\n\n");
			DevMsg("=========================================================================\n");
			DevMsg("CTFBotMainAction::OnStart: caught a dead bot!\n");
			DevMsg("=========================================================================\n");
			DevMsg("\n\n\n");
			return EventDesiredResult<CTFBot>::ChangeTo(CTFBotDead::New(), "I died!");
		}
		
		return DETOUR_MEMBER_CALL(CTFBotMainAction_OnStart)(actor, action);
	}
	
	ConVar cvar_fix_update("sig_debug_onkilled_before_mainaction_start_fix_update", "0", FCVAR_NOTIFY,
		"Debug: check for non-alive-ness in CTFBotMainAction::Update");
	DETOUR_DECL_MEMBER(ActionResult<CTFBot>, CTFBotMainAction_Update, CTFBot *actor, float dt)
	{
		if (cvar_fix_update.GetBool() && !actor->IsAlive()) {
			DevMsg("\n\n\n");
			DevMsg("=========================================================================\n");
			DevMsg("CTFBotMainAction::Update: caught a dead bot!\n");
			DevMsg("=========================================================================\n");
			DevMsg("\n\n\n");
			return EventDesiredResult<CTFBot>::ChangeTo(CTFBotDead::New(), "I died!");
		}
		
		return DETOUR_MEMBER_CALL(CTFBotMainAction_Update)(actor, dt);
	}
	
	ConVar cvar_override("sig_debug_onkilled_before_mainaction_start_override", "0", FCVAR_NOTIFY,
		"Debug: try overriding OnKilled in a child action of CTFBotMainAction to see what happens");
	DETOUR_DECL_MEMBER(EventDesiredResult<CTFBot>, Action_CTFBot_OnKilled, CTFBot *actor, const CTakeDamageInfo& info)
	{
		auto action = reinterpret_cast<Action<CTFBot> *>(this);
		if (rtti_cast<CTFBotTacticalMonitor *>(action) != nullptr) {
			if (RandomFloat(0.0f, 1.0f) < 0.5f) {
				return EventDesiredResult<CTFBot>::SuspendFor(CTFBotSeekAndDestroy::New(), "Doing something stupid");
			} else {
				return EventDesiredResult<CTFBot>::Sustain();
			}
		}
		
		return DETOUR_MEMBER_CALL(Action_CTFBot_OnKilled)(actor, info);
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Debug:OnKilled_Before_MainAction_Start")
		{
			MOD_ADD_DETOUR_MEMBER(CTFBotMainAction_OnStart, "CTFBotMainAction::OnStart");
			MOD_ADD_DETOUR_MEMBER(CTFBotMainAction_Update,  "CTFBotMainAction::Update");
			MOD_ADD_DETOUR_MEMBER(Action_CTFBot_OnKilled,   "Action<CTFBot>::OnKilled");
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_debug_onkilled_before_mainaction_start", "0", FCVAR_NOTIFY,
		"Debug: investigate post-20160707a edge cases of bots not switching to spectator",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
