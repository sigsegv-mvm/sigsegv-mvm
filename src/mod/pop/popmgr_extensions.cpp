#include "mod.h"


namespace Mod_Pop_PopMgr_Extensions
{
	// here's the plan:
	
	// detour CPopulationManager::Parse
	// pre: enable detour for KeyValues::LoadFromFile
	
	// detour KeyValues::LoadFromFile
	// post: disable detour for KeyValues::LoadFromFile
	// check return value; if false, don't do anything
	// post: pre-parse the subkeys
	// anything that we parse: call RemoveSubkey and then deleteThis (to hide from PopMgr's parser)
	
	
	DETOUR_DECL_MEMBER(bool, CPopulationManager_Parse)
	{
		// TODO
		return DETOUR_MEMBER_CALL(CPopulationManager_Parse)();
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Pop:PopMgr_Extensions")
		{
			MOD_ADD_DETOUR_MEMBER(CPopulationManager_Parse, "CPopulationManager::Parse");
		}
		
		void SetEnabled(bool enable)
		{
			this->ToggleAllDetours(enable);
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_pop_popmgr_extensions", "0", FCVAR_NOTIFY,
		"Mod: enable extended KV in CPopulationManager::Parse",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.SetEnabled(var.GetBool());
		});
}
