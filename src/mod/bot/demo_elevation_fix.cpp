#include "mod.h"
#include "stub/tfbot.h"


#if 0 // TODO: fix compile errors in this

namespace Mod::Bot::Demo_Elevation_Fix
{
	DETOUR_DECL_MEMBER(Vector, CTFBotMainAction_SelectTargetPoint, const INextBot *nextbot, const CBaseCombatCharacter *them)
	{
		// if them is a CObjectSentrygun, then do default thing
		// if nextbot's active tf weapon doesn't have ID pipebomblauncher or grenadelauncher, then do default thing
		
		
		
		
		
		// hooray for trigonometry:
		// theta = (1/2)arcsin(g*delta_x/(v^2))
		// where:
		// - theta:   aim angle above horizontal
		// - g:       acceleration due to gravity
		// - delta_x: horizontal distance from aimer to target
		// - v:       projectile's initial speed
		
		// should probably clamp the result to a reasonable range, e.g. [0deg, 45deg]
		
		// then we have to do some stupid stuff to convert the angle into a z-difference above the target at which to aim
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Bot:Demo_Elevation_Fix")
		{
			#error
			MOD_ADD_DETOUR_MEMBER(CTFBotMainAction_SelectTargetPoint, "CTFBotMainAction::SelectTargetPoint");
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_bot_demo_elevation_fix", "0", FCVAR_NOTIFY,
		"Mod: make demoman aim elevation properly compensate for projectile speed modifiers",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue){
			s_Mod.Toggle(static_cast<ConVar *>(pConVar)->GetBool());
		});
}

#endif
