#include "modmanager.h"


void CModManager::LoadAllMods()
{
	DevMsg("CModManager::LoadAllMods\n");
	
	for (auto mod : AutoList<IMod>::List()) {
		mod->InvokeLoad();
	}
}

void CModManager::UnloadAllMods()
{
	DevMsg("CModManager::UnloadAllMods\n");
	
	for (auto mod : AutoList<IMod>::List()) {
		mod->InvokeUnload();
	}
}


static ConCommand ccmd_modlist("sigsegv_modlist", &CModManager::CC_ListMods,
	"List mods and show their status", FCVAR_NONE);
void CModManager::CC_ListMods(const CCommand& cmd)
{
	size_t name_len = 0;
	for (auto mod : AutoList<IMod>::List()) {
		name_len = Max(name_len, strlen(mod->GetName()));
	}
	
	for (auto mod : AutoList<IMod>::List()) {
		const char *status;
		
		if (mod->m_bLoaded) {
			status = "OK";
		} else {
			if (mod->m_bFailed) {
				status = "FAILED";
			} else {
				status = "INACTIVE";
			}
		}
		
		Msg("%-*s  %s\n", name_len, mod->GetName(), status);
	}
}
