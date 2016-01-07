#include "mod.h"
#include "modmanager.h"


IMod::IMod(const char *name)
	: m_pszName(name)
{
	CModManager::Register(this);
}

IMod::~IMod()
{
	CModManager::Unregister(this);
}


bool IMod::Init_CheckPatches(char *error, size_t maxlen)
{
	for (auto patch : this->m_Patches) {
		if (!patch->Init(error, maxlen)) {
			return false;
		}
		if (!patch->Check(error, maxlen)) {
			return false;
		}
	}
	
	return true;
}

bool IMod::Init_SetupDetours(char *error, size_t maxlen)
{
	for (auto pair : this->m_Detours) {
		const char *name = pair.first;
		DetourInfo& info = pair.second;
		
		CDetour *detour = CDetourManager::CreateDetour(info.callback, info.trampoline, name);
		if (detour == nullptr) {
			snprintf(error, maxlen, "CDetourManager::CreateDetour failed for %s", name);
			return false;
		}
		
		info.detour = detour;
	}
	
	return true;
}

bool IMod::InvokeLoad(char *error, size_t maxlen)
{
	return this->OnLoad(error, maxlen);
}

void IMod::InvokeUnload()
{
	this->OnUnload();
	
	for (auto patch : this->m_Patches) {
		patch->UnApply();
		delete patch;
	}
	this->m_Patches.clear();
	
	for (auto pair : this->m_Detours) {
		DetourInfo& info = pair.second;
		
		info.detour->DisableDetour();
		info.detour->Destroy();
	}
	this->m_Detours.clear();
}


void IMod::AddPatch(IPatch *patch)
{
	this->m_Patches.push_back(patch);
}

void IMod::ToggleAllPatches(bool enable)
{
	for (auto patch : this->m_Patches) {
		if (enable) {
			patch->Apply();
		} else {
			patch->UnApply();
		}
	}
}


void IMod::AddDetour(const char *name, void *callback, void **trampoline)
{
	assert(this->m_Detours.find(name) == this->m_Detours.end());
	
	this->m_Detours[name] = {
		callback,
		trampoline,
		nullptr,
	};
}

void IMod::ToggleDetour(const char *name, bool enable)
{
	DetourInfo& info = this->m_Detours.at(name);
	
	if (enable) {
		info.detour->EnableDetour();
	} else {
		info.detour->DisableDetour();
	}
}

void IMod::ToggleAllDetours(bool enable) {
	for (auto pair : this->m_Detours) {
		DetourInfo& info = pair.second;
		
		if (enable) {
			info.detour->EnableDetour();
		} else {
			info.detour->DisableDetour();
		}
	}
}
