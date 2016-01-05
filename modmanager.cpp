#include "modmanager.h"


std::list<ModInfo *> CModManager::s_Mods;


ModInfo *CModManager::GetInfo(IMod *mod)
{
	for (auto info : s_Mods) {
		if (info->m_pMod == mod) {
			return info;
		}
	}
	
	return nullptr;
}


void CModManager::Register(IMod *mod)
{
	DevMsg("CModManager::Register(%s)\n", mod->GetName());
	
	assert(CModManager::GetInfo(mod) == nullptr);
	
	ModInfo *info = new ModInfo(mod);
	s_Mods.push_back(info);
}

void CModManager::Unregister(IMod *mod)
{
	DevMsg("CModManager::Unregister(%s)\n", mod->GetName());
	
	ModInfo *info = CModManager::GetInfo(mod);
	assert(info != nullptr);
	
	s_Mods.remove(info);
	delete info;
}


void CModManager::LoadAllMods()
{
	for (auto info : s_Mods) {
		if (!info->m_bLoaded) {
			bool ok = info->m_pMod->InvokeLoad(info->m_szError, sizeof(info->m_szError));
			
			info->m_bFailed = !ok;
			info->m_bLoaded = ok;
		}
	}
}

void CModManager::UnloadAllMods()
{
	for (auto info : s_Mods) {
		if (info->m_bLoaded) {
			info->m_pMod->InvokeUnload();
			info->m_bLoaded = false;
		}
	}
}


static ConCommand ccmd_modlist("sigsegv_modlist", &CModManager::CC_ListMods,
	"List mods and show their status", FCVAR_NONE);
void CModManager::CC_ListMods(const CCommand& cmd)
{
	size_t name_len = 0;
	for (auto info : s_Mods) {
		name_len = Max(name_len, strlen(info->m_pMod->GetName()));
	}
	
	for (auto info : s_Mods) {
		const char *status;
		const char *error = "";
		
		if (info->m_bLoaded) {
			status = "OK";
		} else {
			if (info->m_bFailed) {
				status = "FAILED: ";
				error = info->m_szError;
			} else {
				status = "INACTIVE";
			}
		}
		
		Msg("%-*s  %s%s\n", name_len, info->m_pMod->GetName(), status, error);
	}
}
