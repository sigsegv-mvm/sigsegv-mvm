#include "mod.h"
#include "util/override.h"


namespace Mod::Etc::CrayAirblast_ConVar_NoCheat
{
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Etc:CrayAirblast_ConVar_NoCheat") {}

		virtual void PreLoad() override
		{
			ICvar::Iterator it(g_pCVar);
			for (it.SetFirst(); it.IsValid(); it.Next()) {
				ConCommandBase *cmd = it.Get();
				
				if (cmd->IsCommand()) continue;
				auto cvar = static_cast<ConVar *>(cmd);
				
				/* we only care about "tf_airblast_cray*" convars */
				if (!StringHasPrefixCaseSensitive(cvar->GetName(), "tf_airblast_cray")) continue;
				
				/* special case: don't remove FCVAR_CHEAT from tf_airblast_cray_debug */
				if (strcmp(cvar->GetName(), "tf_airblast_cray_debug") == 0) continue;
				
				/* remove FCVAR_CHEAT, and add FCVAR_NOTIFY, 'cause why not */
				this->m_Overrides.emplace_back(cvar, FCVAR_NOTIFY, FCVAR_CHEAT);
			}
		}
		
		virtual void OnEnable() override
		{
			for (auto& override : this->m_Overrides) {
				override.Enable();
			}
		}
		
		virtual void OnDisable() override
		{
			for (auto& override : this->m_Overrides) {
				override.Disable();
			}
		}
		
	private:
		std::vector<CConVarOverride_Flags> m_Overrides;
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_etc_crayairblast_convar_nocheat", "0", FCVAR_NOTIFY,
		"Mod: remove the FCVAR_CHEAT flag from the Cray Airblast enable and tuning convars",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue){
			s_Mod.Toggle(static_cast<ConVar *>(pConVar)->GetBool());
		});
}
