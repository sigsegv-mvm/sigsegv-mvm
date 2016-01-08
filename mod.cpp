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
			DevMsg("IMod::Init_CheckPatches: \"%s\" FAIL in Init\n", this->GetName());
			return false;
		}
		if (!patch->Check(error, maxlen)) {
			DevMsg("IMod::Init_CheckPatches: \"%s\" FAIL in Check\n", this->GetName());
			return false;
		}
		
		DevMsg("IMod::Init_CheckPatches: \"%s\" OK\n", this->GetName());
	}
	
	return true;
}

bool IMod::Init_SetupDetours(char *error, size_t maxlen)
{
	for (auto& pair : this->m_Detours) {
		const char *name = pair.first;
		DetourInfo& info = pair.second;
		
		CDetour *detour = CDetourManager::CreateDetour(info.callback, info.trampoline, name);
		if (detour == nullptr) {
			DevMsg("IMod::Init_SetupDetours: \"%s\" FAIL\n", this->GetName());
			snprintf(error, maxlen, "CDetourManager::CreateDetour failed for %s", name);
			return false;
		}
		
		info.detour = detour;
		DevMsg("IMod::Init_SetupDetours: \"%s\" OK 0x%08x\n", this->GetName(), detour);
	}
	
	return true;
}

bool IMod::InvokeLoad(char *error, size_t maxlen)
{
	DevMsg("IMod::InvokeLoad: \"%s\"\n", this->GetName());
	
	return this->OnLoad(error, maxlen);
}

void IMod::InvokeUnload()
{
	DevMsg("IMod::InvokeUnload: \"%s\"\n", this->GetName());
	
	this->OnUnload();
	
	for (auto patch : this->m_Patches) {
		patch->UnApply();
		delete patch;
	}
	this->m_Patches.clear();
	
	for (auto& pair : this->m_Detours) {
		DetourInfo& info = pair.second;
		
		info.detour->DisableDetour();
		info.detour->Destroy();
	}
	this->m_Detours.clear();
}


void IMod::AddPatch(IPatch *patch)
{
	DevMsg("IMod::AddPatch: \"%s\" \"%s\" off:0x%08x len:0x%08x\n", this->GetName(),
		patch->GetFuncName(), patch->GetFuncOffset(), patch->GetLength());
	
	this->m_Patches.push_back(patch);
}

void IMod::ToggleAllPatches(bool enable)
{
	DevMsg("IMod::ToggleAllPatches: \"%s\" %s\n", this->GetName(),
		(enable ? "ON" : "OFF"));
	
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
	DevMsg("IMod::AddDetour: \"%s\" \"%s\"\n", this->GetName(), name);
	
	assert(this->m_Detours.find(name) == this->m_Detours.end());
	
	this->m_Detours[name] = {
		callback,
		trampoline,
		nullptr,
	};
}

void IMod::ToggleDetour(const char *name, bool enable)
{
	DevMsg("IMod::ToggleDetour: \"%s\" \"%s\" %s\n", this->GetName(), name,
		(enable ? "ON" : "OFF"));
	
	DetourInfo& info = this->m_Detours.at(name);
	
	if (enable) {
		info.detour->EnableDetour();
	} else {
		info.detour->DisableDetour();
	}
}

void IMod::ToggleAllDetours(bool enable)
{
	DevMsg("IMod::ToggleAllDetours: \"%s\" %s\n", this->GetName(),
		(enable ? "ON" : "OFF"));
	
	for (auto& pair : this->m_Detours) {
		DetourInfo& info = pair.second;
		
		if (enable) {
			info.detour->EnableDetour();
		} else {
			info.detour->DisableDetour();
		}
	}
}
