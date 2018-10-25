#include "mod.h"
#include "re/nextbot.h"


namespace Mod::AI::NextBotEventResponder_Hooks
{
	// INER test results:
	
	// NOPE OnLeaveGround  (L4D2: ZombieBotLocomotion::UpdateGroundConstraint)
	// NOPE OnLandOnGround (L4D2: ZombieBotLocomotion::UpdateGroundConstraint)
	// (these are also set in NextBotGroundLocomotion::UpdateGroundConstraint in TF2)
	
	// OK   OnContact
	
	// OK   OnMoveToSuccess (PathFollower::CheckProgress)
	// OK   OnMoveToFailure (ChasePath::RefreshPath)
	
	// OK   OnStuck (ILocomotion::StuckMonitor)
	// OK   OnUnStuck (PathFollower::Update? / ILocomotion::ClearStuckStatus?)
	
	// TODO OnPostureChanged
	// TODO OnAnimationActivityComplete
	// TODO OnAnimationActivityInterrupted
	// TODO OnAnimationEvent
	
	// OK   OnIgnite
	// OK   OnInjured
	// OK   OnKilled
	// OK   OnOtherKilled
	//      - CBaseCombatCharacter::Event_Killed
	//      - NextBotCombatCharacter::Event_Killed
	
	// OK   OnSight     (IVision::UpdateKnownEntities)
	// OK   OnLostSight (IVision::UpdateKnownEntities)
	
	// NOPE OnSound
	// NOPE OnSpokeConcept
	// OK   OnWeaponFired
	//      - CTFGameStats::Event_PlayerFiredWeapon
	//      - CTFFlameThrower::FireAirBlast
	//      - CTFPlayerShared::FadeInvis
	//      - CTFBot::Touch
	
	// OK   OnNavAreaChanged
	// NOPE OnModelChanged
	// OK   OnPickUp (CTFBot::FireGameEvent "teamplay_flag_event")
	// NOPE OnDrop
	// OK   OnActorEmoted (CMultiplayRules::VoiceCommand)
	
	// TODO OnCommandAttack
	// TODO OnCommandApproach
	// TODO OnCommandApproach
	// TODO OnCommandRetreat
	// TODO OnCommandPause
	// TODO OnCommandResume
	// OK   OnCommandString
	
	// TODO OnShoved
	// TODO OnBlinded
	
	// OK   OnTerritoryContested (CTFBot::FireGameEvent "teamplay_point_startcapture")
	// OK   OnTerritoryCaptured  (CTFBot::FireGameEvent "teamplay_point_captured")
	// OK   OnTerritoryLost      (CTFBot::FireGameEvent "teamplay_point_captured")
	
	// TODO OnWin
	// TODO OnLose
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("AI:NextBotEventResponder_Hooks")
		{
			
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_ai_nextboteventresponder_hooks", "0", FCVAR_NOTIFY,
		"Mod: enable previously unfired INextBotEventResponder events",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
