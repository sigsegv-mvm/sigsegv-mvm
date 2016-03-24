#include "mod.h"
#include "re/nextbot.h"


namespace Mod_AI_NextBotEventResponder_Hooks
{
	// INER test results:
	
	// NOPE OnLeaveGround  (L4D2: ZombieBotLocomotion::UpdateGroundConstraint)
	// NOPE OnLandOnGround (L4D2: ZombieBotLocomotion::UpdateGroundConstraint)
	// (these are also set in NextBotGroundLocomotion::UpdateGroundConstraint in TF2)
	
	// OK   OnContact
	
	// OK   OnMoveToSuccess (PathFollower::CheckProgress)
	// OK   OnMoveToFailure (ChasePath::RefreshPath)
	
	// TODO OnStuck
	// TODO OnUnStuck
	
	// TODO OnPostureChanged
	// TODO OnAnimationActivityComplete
	// TODO OnAnimationActivityInterrupted
	// TODO OnAnimationEvent
	
	// OK   OnIgnite
	// OK   OnInjured
	// OK   OnKilled
	// TODO OnOtherKilled
	
	// TODO OnSight
	// TODO OnLostSight
	
	// TODO OnSound
	// TODO OnSpokeConcept
	// TODO OnWeaponFired
	
	// OK   OnNavAreaChanged
	// TODO OnModelChanged
	// TODO OnPickUp
	// TODO OnDrop
	// OK   OnActorEmoted
	
	// TODO OnCommandAttack
	// TODO OnCommandApproach
	// TODO OnCommandApproach
	// TODO OnCommandRetreat
	// TODO OnCommandPause
	// TODO OnCommandResume
	// OK   OnCommandString
	
	// TODO OnShoved
	// TODO OnBlinded
	
	// TODO OnTerritoryContested
	// TODO OnTerritoryCaptured
	// TODO OnTerritoryLost
	
	// TODO OnWin
	// TODO OnLose
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("AI:NextBotEventResponder_Hooks")
		{
			
		}
		
		void SetEnabled(bool enable)
		{
			this->ToggleAllDetours(enable);
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_ai_nextboteventresponder_hooks", "0", FCVAR_NOTIFY,
		"Mod: enable previously unfired INextBotEventResponder events",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.SetEnabled(var.GetBool());
		});
}
