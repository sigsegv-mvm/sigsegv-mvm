#include "mod.h"


#include "addr/standard.h"
//static CAddr_Sym addr1("CTFBotMainAction::Update", "_ZN16CTFBotMainAction6UpdateEP6CTFBotf");


#define TRACE_ENABLE 1
#include "util/trace.h"


namespace Mod_Debug_PopMgr_MultiParse
{
	//DETOUR_DECL_MEMBER()
	
	// CPopulationManager::Parse()
	
	// CPopulationManager::Initialize()
	
	// CPopulationManager::JumpToWave(unsigned int, float)
	
	// CPopulationManager::ResetMap()
	
	// CPopulationManager::SetPopulationFilename(char const*)
	
	DETOUR_DECL_STATIC(void, tf_mvm_popfile, const CCommand& args)
	{
		TRACE();
		DETOUR_STATIC_CALL(tf_mvm_popfile)(args);
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Debug:PopMgr_MultiParse")
		{
			this->AddDetour(new CFuncTrace(Library::SERVER, "_ZN18CPopulationManager5ParseEv"));
		}
		
		void SetEnabled(bool enable)
		{
			this->ToggleAllDetours(enable);
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_debug_popmgr_multiparse", "0", FCVAR_NOTIFY,
		"Debug: figure out why tf_mvm_popfile parses the popfile about 4 times",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.SetEnabled(var.GetBool());
		});
}
