#include "mod.h"
#include "modmanager.h"


void IMod::InvokeLoad()
{
	DevMsg("IMod::InvokeLoad: \"%s\"\n", this->GetName());
	
	bool ok_patch  = this->Init_CheckPatches();
	bool ok_detour = this->Init_SetupDetours();
	
	if (!ok_patch || !ok_detour) {
		this->m_bFailed = true;
		return;
	}
	
	bool ok_load = this->OnLoad();
	if (ok_load) {
		this->m_bLoaded = true;
	} else {
		this->m_bFailed = true;
	}
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


bool IMod::Init_CheckPatches()
{
	bool ok = true;
	
	for (auto patch : this->m_Patches) {
		if (patch->Init()) {
			if (patch->Check()) {
				DevMsg("IMod::Init_CheckPatches: \"%s\" OK\n", this->GetName());
			} else {
				DevMsg("IMod::Init_CheckPatches: \"%s\" FAIL in Check\n", this->GetName());
				ok = false;
			}
		} else {
			DevMsg("IMod::Init_CheckPatches: \"%s\" FAIL in Init\n", this->GetName());
			ok = false;
		}
	}
	
	return ok;
}

bool IMod::Init_SetupDetours()
{
	bool ok = true;
	
	for (auto& pair : this->m_Detours) {
		const char *name = pair.first;
		DetourInfo& info = pair.second;
		
		CDetour *detour = CDetourManager::CreateDetour(info.callback, info.trampoline, name);
		if (detour != nullptr) {
			info.detour = detour;
			DevMsg("IMod::Init_SetupDetours: \"%s\" OK 0x%08x\n", this->GetName(), detour);
		} else {
			DevMsg("IMod::Init_SetupDetours: \"%s\" FAIL\n", this->GetName());
			ok = false;
		}
	}
	
	return ok;
}


void IMod::AddPatch(IPatch *patch)
{
	DevMsg("IMod::AddPatch: \"%s\" \"%s\" off:0x%08x len:0x%08x\n", this->GetName(),
		patch->GetFuncName(), patch->GetFuncOffset(), patch->GetLength());
	assert(!this->m_bLoaded);
	
	this->m_Patches.push_back(patch);
}

void IMod::ToggleAllPatches(bool enable)
{
	if (this->m_bFailed) return;
	
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
	if (this->m_bFailed) return;
	
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
	if (this->m_bFailed) return;
	
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
