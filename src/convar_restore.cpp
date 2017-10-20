#include "convar_restore.h"


class CEmptyConVar : public ConVar {};


namespace ConVar_Restore
{
	ConCommand ccmd_save("sig_cvar_save", &Save, "Save the values of this mod's ConVars");
	ConCommand ccmd_load("sig_cvar_load", &Load, "Load the previously saved ConVar values of this mod");
	
	
	std::list<ConVar *>     s_ConVars;
	std::list<ConCommand *> s_ConCmds;
	
	
	void Register(ConCommandBase *pCommand)
	{
		/* ignore s_EmptyConVar */
		if (dynamic_cast<CEmptyConVar *>(pCommand) != nullptr) return;
		
		if (pCommand->IsCommand()) {
		//	DevMsg("ConVar_Restore::Register: cmd \"%s\" @ %08x\n", cmd->GetName(), (uintptr_t)cmd);
			s_ConCmds.push_back(static_cast<ConCommand *>(pCommand));
		} else {
		//	DevMsg("ConVar_Restore::Register: var \"%s\" @ %08x\n", var->GetName(), (uintptr_t)var);
			s_ConVars.push_back(static_cast<ConVar *>(pCommand));
		}
	}
	
	
	void Save()
	{
	//	DevMsg("ConVar_Restore::Save\n");
		
		auto kv = new KeyValues("SigsegvConVars");
		kv->UsesEscapeSequences(true);
		
		for (auto var : s_ConVars) {
		//	DevMsg("  %s\n", var->GetName());
			
			if (kv->FindKey(var->GetName()) != nullptr) {
				Warning("ConVar_Restore::Save: ConVar \"%s\" is a duplicate\n", var->GetName());
				continue;
			}
			if (var->IsFlagSet(FCVAR_NEVER_AS_STRING)) {
				Warning("ConVar_Restore::Save: ConVar \"%s\" has unsupported flag FCVAR_NEVER_AS_STRING\n", var->GetName());
				continue;
			}
			
			if (strcmp(var->GetString(), var->GetDefault()) == 0) {
		//		DevMsg("    default: skip\n");
				continue;
			}
			
			kv->SetString(var->GetName(), var->GetString());
			
		//	auto subkey = new KeyValues(var->GetName());
		//	subkey->SetString("value", var->GetString());
		//	kv->AddSubKey(subkey);
		}
		
		if (!kv->SaveToFile(filesystem, "sigsegv_convars.txt", nullptr, true, true, false)) {
			Warning("ConVar_Restore::Save: Could not save KeyValues to \"sigsegv_convars.txt\".\n");
		}
		
		kv->deleteThis();
	}
	
	void Load()
	{
	//	DevMsg("ConVar_Restore::Load\n");
		
		auto kv = new KeyValues("SigsegvConVars");
		kv->UsesEscapeSequences(true);
		
		if (kv->LoadFromFile(filesystem, "sigsegv_convars.txt")) {
			FOR_EACH_VALUE(kv, subkey) {
				const char *name  = subkey->GetName();
				const char *value = subkey->GetString();
				
				ConCommandBase *base = icvar->FindCommandBase(name);
				if (base == nullptr) {
					Warning("ConVar_Restore::Load: ConVar \"%s\" doesn't exist\n", name);
					continue;
				}
				if (base->IsCommand()) {
					Warning("ConVar_Restore::Load: ConVar \"%s\" is actually a ConCommand\n", name);
					continue;
				}
				if (base->IsFlagSet(FCVAR_NEVER_AS_STRING)) {
					Warning("ConVar_Restore::Load: ConVar \"%s\" has unsupported flag FCVAR_NEVER_AS_STRING\n", name);
					continue;
				}
				
				auto var = static_cast<ConVar *>(base);
				var->SetValue(value);
			}
		} else {
			Warning("ConVar_Restore::Load: Could not load KeyValues from \"sigsegv_convars.txt\".\n");
		}
		
		kv->deleteThis();
	}
}
