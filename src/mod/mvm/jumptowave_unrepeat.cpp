#include "mod.h"
#include "util/scope.h"


namespace Mod_MvM_JumpToWave_UnRepeat
{
	RefCount rc_tf_mvm_popfile;
	int resetmap_called;
	DETOUR_DECL_STATIC(void, tf_mvm_popfile, const CCommand& args)
	{
		SCOPED_INCREMENT(rc_tf_mvm_popfile);
		resetmap_called = 0;
		
		DETOUR_STATIC_CALL(tf_mvm_popfile)(args);
	}
	
	DETOUR_DECL_MEMBER(void, CPopulationManager_ResetMap)
	{
		if (rc_tf_mvm_popfile > 0) {
			if (resetmap_called++) return;
		}
		
		DETOUR_MEMBER_CALL(CPopulationManager_ResetMap)();
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("MvM:JumpToWave_UnRepeat")
		{
			MOD_ADD_DETOUR_STATIC(tf_mvm_popfile,              "tf_mvm_popfile");
			MOD_ADD_DETOUR_MEMBER(CPopulationManager_ResetMap, "CPopulationManager::ResetMap");
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_mvm_jumptowave_unrepeat", "0", FCVAR_NOTIFY,
		"Mod: eliminate unnecessary duplicate calls to CPopulationManager::JumpToWave",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}

// TODO:
// CPopulationManager::JumpToWave calls CPopulationManager::Initialize
// and then calls CTeamplayRoundBasedRules::State_Enter_PREROUND
// which also calls CPopulationManager::Initialize
