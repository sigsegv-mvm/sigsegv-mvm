#include "mod.h"
#include "stub/gamerules.h"
#include "util/scope.h"
#include "util/backtrace.h"


namespace Mod_Perf_MvM_Load_Popfile
{
	RefCount rc_tf_mvm_popfile;
	int tf_mvm_popfile_resets = 0;
	DETOUR_DECL_STATIC(void, tf_mvm_popfile, const CCommand& args)
	{
		SCOPED_INCREMENT(rc_tf_mvm_popfile);
		tf_mvm_popfile_resets = 0;
		
		DETOUR_STATIC_CALL(tf_mvm_popfile)(args);
	}
	
	DETOUR_DECL_MEMBER(void, CPopulationManager_ResetMap)
	{
		if (rc_tf_mvm_popfile > 0) {
			switch (++tf_mvm_popfile_resets) {
			case 1:
				Msg("++++ ResetMap #1: calling function as usual.\n");
				DETOUR_MEMBER_CALL(CPopulationManager_ResetMap)();
				return;
			
			// during the 20150716b --> 20150724a update (post-GunMettle), the devs replaced the GR_STATE_RESTART transition
			// in the tf_mvm_popfile command with an inexplicable second call to g_pPopulationManager->ResetMap();
			// this breaks the ability to load popfiles if the initial popfile load for a map fails, until you load an
			// entirely different map; so, to fix this, we essentially restore the second call to the way it was before
			case 2:
				Msg("++++ ResetMap #2: calling TFGameRules()->State_Transition(GR_STATE_RESTART) instead.\n");
				TFGameRules()->State_Transition(GR_STATE_RESTART);
				return;
			
			default:
				Msg("++++ ResetMap #%d: doing nothing whatsoever.\n", tf_mvm_popfile_resets);
				return;
			}
		}
		
		DETOUR_MEMBER_CALL(CPopulationManager_ResetMap)();
	}
	
	
	DETOUR_DECL_MEMBER(bool, CPopulationManager_Initialize)
	{
		BACKTRACE();
		return DETOUR_MEMBER_CALL(CPopulationManager_Initialize)();
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Perf:MvM_Load_Popfile")
		{
			MOD_ADD_DETOUR_STATIC(tf_mvm_popfile,              "tf_mvm_popfile");
			MOD_ADD_DETOUR_MEMBER(CPopulationManager_ResetMap, "CPopulationManager::ResetMap");
			
		//	MOD_ADD_DETOUR_MEMBER(CPopulationManager_Initialize, "CPopulationManager::Initialize");
		}
	};
	CMod s_Mod;
	
	// So the code, as it stands, fixes tf_mvm_popfile in post-zero-wave conditions.
	// And in post-zero-wave loads, the popfile is parsed just once.
	// - Look at the BinDiff and make sure we didn't cause a regression WRT votes (the subject of the change)
	
	// But in post-nonzero-wave loads, the popfile is parsed twice, due to GR_STATE_PREROUND being transitioned to twice.
	// - Check how they did this back in the early days of MvM
	
	// And as for tf_mvm_jump_to_wave, it only parses once, which is fine.
	
	// BUG: apparently, when this thing is enabled, when joining a server,
	// - the mission immediately starts
	// - it uses PvP timers
	
	
	
	
	// OLD --------
	
	// NOTE: okay, here's what the ACTUAL PROBLEM is:
	// CPopulationManager::JumpToWave, when called via tf_mvm_popfile, calls CPopulationManager::Initialize
	// and then JumpToWave also calls CTeamplayRoundBasedRules::State_Enter_PREROUND, which calls CPopulationManager::Initialize
	
	// so, we need to block one of those calls
	
	
	ConVar cvar_enable("sig_perf_mvm_load_popfile", "0", FCVAR_NOTIFY,
		"Mod: eliminate unnecessary duplication of parsing/init code during tf_mvm_popfile",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}

/*

Latest TF2 game states when MvM map loads:

INIT (right as map load begins)

PREGAME (very very early in map load)

all of the map loading stuff happens here
including the initial attempt at popfile loading

the first player can get through the MOTD screen and arrive at the class selection screen while it's still PREGAME (showing the wrong HUD stuff)

STARTGAME (when first player chooses a class)

PREROUND (almost immediately after STARTGAME)

the "actual" popfile load happens here

BETWEEN_RNDS (very soon after PREROUND)

it's now pre-wave 1 ready-up time and the HUD is correct


when executing a valid tf_mvm_popfile at this point, we get this:
- popfile parse
- BETWEEN_RNDS -> PREROUND
- popfile parse
- PREROUND -> BETWEEN_RNDS
- wave force-end, popfile parse
- BETWEEN_RNDS -> PREROUND
- popfile parse
- PREROUND -> BETWEEN_RNDS

*/
