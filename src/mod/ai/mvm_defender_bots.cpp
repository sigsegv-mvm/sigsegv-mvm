#include "mod.h"
#include "re/nextbot.h"
#include "stub/tfbot.h"
#include "stub/gamerules.h"


// CTFBotEngineerBuild:  size 0x34,   inline
// CTFBotMedicHeal:      size 0x9058, inline
// CTFBotSniperLurk:     size 0x485c, inline
// CTFBotSpyInfiltrate:  size 0x4834, inline
// CTFBotSeekAndDestroy: has ctor


namespace Mod_AI_MvM_Defender_Bots
{
	// "easier" way to spoof this:
	// if TFGameRules()->IsMannVsMachineMode() and actor team is RED:
	//   set TFGameRules()->m_bPlayingMannVsMachine to false
	//   call into DesiredScenarioAndClassAction
	//   set TFGameRules()->m_bPlayingMannVsMachine to true
	
	// probably will need to temporarily override GameRules()->GetGameType() too
	
	
//	Action<CTFBot> *MakeNew_CTFBotSeekAndDestroy(float )
//	{
//		
//	}
	
	
	DETOUR_DECL_MEMBER(Action<CTFBot> *, CTFBotScenarioMonitor_DesiredScenarioAndClassAction, CTFBot *actor)
	{
		if (TFGameRules()->IsMannVsMachineMode() && actor->GetTeamNumber() == TF_TEAM_RED) {
			switch (actor->GetPlayerClass()->GetClassIndex()) {
			case TF_CLASS_ENGINEER:
				;
				// return new CTFBotEngineerBuild
			case TF_CLASS_MEDIC:
				;
				// return new CTFBotMedicHeal
			case TF_CLASS_SNIPER:
				;
				// only if they have a rifle, probably
				// return new CTFBotSniperLurk
			case TF_CLASS_SPY:
				;
				// return new CTFBotSpyInfiltrate
			}
			
			// return new CTFBotSeekAndDestroy(-1.0f)
		}
		
		return DETOUR_MEMBER_CALL(CTFBotScenarioMonitor_DesiredScenarioAndClassAction)(actor);
	}
	
	
	// do we need to override actor->IsAllowedToPickUpFlag to false?
	// do we need to throw in some CTFBotDefendPoint for gate defense?
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("AI:MvM_Defender_Bots")
		{
			MOD_ADD_DETOUR_MEMBER(CTFBotScenarioMonitor_DesiredScenarioAndClassAction, "CTFBotScenarioMonitor::DesiredScenarioAndClassAction");
		}
		
		void SetEnabled(bool enable)
		{
			this->ToggleAllDetours(enable);
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_ai_mvm_defender_bots", "0", FCVAR_NOTIFY,
		"Mod: make red TFBots in MvM do things other than stand around uselessly",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.SetEnabled(var.GetBool());
		});
}
