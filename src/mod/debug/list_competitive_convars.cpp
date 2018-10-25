#include "mod.h"


namespace Mod::Debug::List_Competitive_ConVars
{
	constexpr ptrdiff_t OFF_ConVar_m_nFlags      = 0x14;
	constexpr ptrdiff_t OFF_ConVar_m_pParent     = 0x1c;
	constexpr ptrdiff_t OFF_ConVar_m_bHasCompMin = 0x44;
	constexpr ptrdiff_t OFF_ConVar_m_fCompMin    = 0x48;
	constexpr ptrdiff_t OFF_ConVar_m_bHasCompMax = 0x4c;
	constexpr ptrdiff_t OFF_ConVar_m_fCompMax    = 0x50;
	
	
	const ConVar *ConVar_GetParent(const ConVar *cvar)
	{
		return *reinterpret_cast<const ConVar **>((uintptr_t)cvar + OFF_ConVar_m_pParent);
	}
	
	
	int ConVar_GetFlags(const ConVar *cvar)
	{
		cvar = ConVar_GetParent(cvar);
		
		return *reinterpret_cast<const int *>((uintptr_t)cvar + OFF_ConVar_m_nFlags);
	}
	
	
	/* roughly analogous to the real thing: bool ConVar::GetCompMin(float&) */
	bool ConVar_GetCompMin(const ConVar *cvar, float& min)
	{
		cvar = ConVar_GetParent(cvar);
		
		min = *reinterpret_cast<const float *>((uintptr_t)cvar + OFF_ConVar_m_fCompMin);
		return *reinterpret_cast<const bool *>((uintptr_t)cvar + OFF_ConVar_m_bHasCompMin);
	}
	
	/* roughly analogous to the real thing: bool ConVar::GetCompMax(float&) */
	bool ConVar_GetCompMax(const ConVar *cvar, float& max)
	{
		cvar = ConVar_GetParent(cvar);
		
		max = *reinterpret_cast<const float *>((uintptr_t)cvar + OFF_ConVar_m_fCompMax);
		return *reinterpret_cast<const bool *>((uintptr_t)cvar + OFF_ConVar_m_bHasCompMax);
	}
	
	
	CON_COMMAND(sig_debug_list_competitive_convars, "")
	{
		std::vector<std::string> messages;
		
		ICvar::Iterator it(g_pCVar);
		for (it.SetFirst(); it.IsValid(); it.Next()) {
			ConCommandBase *cmd = it.Get();
			
			if (cmd->IsCommand()) continue;
			auto cvar = static_cast<ConVar *>(cmd);
			
			/* check for competitive min/max limits */
			float cmin = NAN;
			if (ConVar_GetCompMin(cvar, cmin)) {
				messages.emplace_back(CFmtStrN<256>("ConVar \"%s\" has a competitive minimum: %f\n", cvar->GetName(), cmin));
			}
			float cmax = NAN;
			if (ConVar_GetCompMax(cvar, cmax)) {
				messages.emplace_back(CFmtStrN<256>("ConVar \"%s\" has a competitive maximum: %f\n", cvar->GetName(), cmax));
			}
			
			/* check for new flags not documented in iconvar.h */
			std::vector<int> unknown_flags;
			if (cvar->IsFlagSet(1 << 15)) unknown_flags.push_back(15);
			if (cvar->IsFlagSet(1 << 18)) unknown_flags.push_back(18);
			if (cvar->IsFlagSet(1 << 19)) unknown_flags.push_back(19);
			if (cvar->IsFlagSet(1 << 26)) unknown_flags.push_back(26);
			if (cvar->IsFlagSet(1 << 27)) unknown_flags.push_back(27);
			if (cvar->IsFlagSet(1 << 31)) unknown_flags.push_back(31);
			if (!unknown_flags.empty()) {
				std::string msg(CFmtStrN<256>("ConVar \"%s\" has unknown flags:", cvar->GetName()));
				for (auto flag : unknown_flags) {
					msg += " " + std::to_string(flag);
				}
				msg += "\n";
				
				messages.push_back(msg);
			}
			
			constexpr int FLAG_MASK = 0x4e096;
			
			/* replicate ForceCompetitiveConvars logic */
			if ((ConVar_GetFlags(cvar) & FLAG_MASK) == 0) {
				bool is_exception = false;
				for (auto name : { "r_decal_cullsize" }) {
					if (strcmp(cvar->GetName(), name) == 0) {
						is_exception = true;
						break;
					}
				}
				
				if (!is_exception) {
					messages.emplace_back(CFmtStrN<256>("ConVar \"%s\" will be reset based on its flags (%08x)\n",
						cvar->GetName(), ConVar_GetFlags(cvar)));
				} else {
					messages.emplace_back(CFmtStrN<256>("ConVar \"%s\" would be reset based on its flags (%08x) but it has an exception\n",
						cvar->GetName(), ConVar_GetFlags(cvar)));
				}
			}
		}
		
		std::stable_sort(messages.begin(), messages.end());
		
		for (auto msg : messages) {
			Msg("%s", msg.c_str());
		}
	}
}
