#include "mod.h"


// CPopulationManager::JumpToWave calls CPopulationManager::Initialize
// and then calls CTeamplayRoundBasedRules::State_Enter_PREROUND
// which also calls CPopulationManager::Initialize

// tf_mvm_popfile calls CPopulationManager::SetPopulationFilename, and then
// calls CPopulationManager::ResetMap; both of these call
// CPopulationManager::JumpToWave


// #1
// CPopulationManager::SetPopulationFilename
// CPopulationManager::ResetMap
// CPopulationManager::JumpToWave
// CPopulationManager::Initialize
// CPopulationManager::Parse

// #2
// 


namespace Mod_Debug_PopMgr_MultiParse
{
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Debug:PopMgr_MultiParse")
		{
			for (auto pattern : {
				"tf_mvm_popfile",
				"CPopulationManager\\d+SetPopulationFilenameE",
				"CPopulationManager\\d+ResetMapE",
				"CPopulationManager\\d+JumpToWaveE",
				"CPopulationManager\\d+InitializeE",
				"CPopulationManager\\d+ParseE",
				"CTeamplayRoundBasedRules\\d+State_Enter_PREROUNDE",
				"CTeamplayRoundBasedRules\\d+RoundRespawnE",
				"CTFGameRules\\d+RoundRespawnE",
				"CTFGameRules\\d+SetupOnRoundStartE",
			}) {
				this->AddDetour(new CFuncTrace(Library::SERVER, pattern));
			}
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_debug_popmgr_multiparse", "0", FCVAR_NOTIFY,
		"Debug: figure out why tf_mvm_popfile parses the popfile about 4 times",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
