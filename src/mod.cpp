#include "mod.h"


bool IHasPatches::LoadPatches()
{
	bool ok = true;
	
	for (auto patch : this->m_Patches) {
		if (patch->Init()) {
			if (patch->Check()) {
//				DevMsg("IHasPatches::LoadPatches: \"%s\" OK\n", this->GetName());
			} else {
				DevMsg("IHasPatches::LoadPatches: \"%s\" FAIL in Check\n", this->GetName());
				ok = false;
			}
		} else {
			DevMsg("IHasPatches::LoadPatches: \"%s\" FAIL in Init\n", this->GetName());
			ok = false;
		}
	}
	
	return ok;
}

void IHasPatches::UnloadPatches()
{
	for (auto patch : this->m_Patches) {
		patch->UnApply();
		delete patch;
	}
	
	this->m_Patches.clear();
}


void IHasPatches::AddPatch(IPatch *patch)
{
//	DevMsg("IHasPatches::AddPatch: \"%s\" \"%s\" off:[0x%05x~0x%05x] len:0x%05x\n", this->GetName(),
//		patch->GetFuncName(), patch->GetFuncOffMin(), patch->GetFuncOffMax(), patch->GetLength());
	
	assert(this->CanAddPatches());
	
	this->m_Patches.push_back(patch);
}


void IHasPatches::ToggleAllPatches(bool enable)
{
	if (!this->CanTogglePatches()) return;
	
	DevMsg("IHasPatches::ToggleAllPatches: \"%s\" %s\n", this->GetName(),
		(enable ? "ON" : "OFF"));
	
	for (auto patch : this->m_Patches) {
		if (enable) {
			patch->Apply();
		} else {
			patch->UnApply();
		}
	}
}


bool IHasDetours::LoadDetours()
{
	bool ok = true;
	
	for (auto& pair : this->m_Detours) {
		const char *name = pair.first;
		IDetour *detour  = pair.second;
		
		if (!detour->IsLoaded()) {
			if (detour->Load()) {
//				DevMsg("IHasDetours::LoadDetours: \"%s\" \"%s\" OK\n", this->GetName(), name);
			} else {
				DevMsg("IHasDetours::LoadDetours: \"%s\" \"%s\" FAIL\n", this->GetName(), name);
				ok = false;
			}
		}
	}
	
	return ok;
}

void IHasDetours::UnloadDetours()
{
	for (auto& pair : this->m_Detours) {
		IDetour *detour = pair.second;
		
		if (detour->IsLoaded()) {
			detour->Unload();
		}
		
		delete detour;
	}
	
	this->m_Detours.clear();
}


void IHasDetours::AddDetour(IDetour *detour)
{
	const char *name = detour->GetName();
	
//	DevMsg("IHasDetours::AddDetour: \"%s\" \"%s\"\n", this->GetName(), name);
	
	assert(this->CanAddDetours());
	assert(this->m_Detours.find(name) == this->m_Detours.end());
	
	this->m_Detours[name] = detour;
}


void IHasDetours::ToggleDetour(const char *name, bool enable)
{
	if (!this->CanToggleDetours()) return;
	
	DevMsg("IHasDetours::ToggleDetour: \"%s\" \"%s\" %s\n", this->GetName(), name,
		(enable ? "ON" : "OFF"));
	
	this->m_Detours.at(name)->Toggle(enable);
}


void IHasDetours::ToggleAllDetours(bool enable)
{
	if (!this->CanToggleDetours()) return;
	
	DevMsg("IHasDetours::ToggleAllDetours: \"%s\" %s\n", this->GetName(),
		(enable ? "ON" : "OFF"));
	
	for (auto& pair : this->m_Detours) {
		IDetour *detour = pair.second;
		detour->Toggle(enable);
	}
}


void IMod::InvokeLoad()
{
	DevMsg("IMod::InvokeLoad: \"%s\"\n", this->GetName());
	
	bool ok_patch  = this->LoadPatches();
	bool ok_detour = this->LoadDetours();
	
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
	
	this->UnloadDetours();
	this->UnloadPatches();
}


CModManager g_ModManager;


void CModManager::Load()
{
	DevMsg("CModManager::Load\n");
	
	for (auto mod : AutoList<IMod>::List()) {
		mod->InvokeLoad();
	}
}

void CModManager::Unload()
{
	DevMsg("CModManager::Unload\n");
	
	for (auto mod : AutoList<IMod>::List()) {
		mod->InvokeUnload();
	}
}


void CModManager::LevelInitPreEntity()
{
	// TODO
}

void CModManager::LevelInitPostEntity()
{
	// TODO
}

void CModManager::LevelShutdownPreEntity()
{
	// TODO
}

void CModManager::LevelShutdownPostEntity()
{
	// TODO
}

void CModManager::FrameUpdatePreEntityThink()
{
	for (auto listener : AutoList<IFrameUpdateListener>::List()) {
		if (listener->ShouldReceiveFrameEvents()) {
			listener->FrameUpdatePreEntityThink();
		}
	}
}

void CModManager::FrameUpdatePostEntityThink()
{
	for (auto listener : AutoList<IFrameUpdateListener>::List()) {
		if (listener->ShouldReceiveFrameEvents()) {
			listener->FrameUpdatePostEntityThink();
		}
	}
}


static ConCommand ccmd_list_mods("sig_list_mods", &CModManager::CC_ListMods,
	"List mods and show their status", FCVAR_NONE);
void CModManager::CC_ListMods(const CCommand& cmd)
{
	struct ModInfo
	{
		IMod *mod;
		std::string cat;
		std::string name;
		
		Color c_status;
		std::string status;
		
		Color c_patches;
		std::string patches;
		
		Color c_d_total;
		std::string d_total;
		
		Color c_d_failed;
		std::string d_failed;
		
		Color c_d_active;
		std::string d_active;
		
		bool operator<(const ModInfo& that) const
		{
			if (this->cat.compare(that.cat) != 0) {
				return this->cat < that.cat;
			}
			
			if (this->name.compare(that.name) != 0) {
				return this->name < that.name;
			}
			
			return this->mod < that.mod;
		}
	};
	std::set<ModInfo> mods;
	
	Color c_header(0xff, 0xff, 0xff, 0xff);
	Color c_normal(0xc0, 0xc0, 0xc0, 0xff);
	Color c_bright(0xff, 0xff, 0xff, 0xff);
	Color c_bad   (0xff, 0x40, 0x40, 0xff);
	Color c_good  (0x40, 0xff, 0x40, 0xff);
	Color c_note  (0xff, 0xff, 0x40, 0xff);
	
	constexpr auto h_cat      = "Category";
	constexpr auto h_name     = "Name";
	constexpr auto h_status   = "Status";
	constexpr auto h_patches  = "Patches";
	constexpr auto h_d_total  = "Detours";
	constexpr auto h_d_failed = "D:Fail";
	constexpr auto h_d_active = "D:Act";
	
	size_t l_cat      = strlen(h_cat);
	size_t l_name     = strlen(h_name);
	size_t l_status   = strlen(h_status);
	size_t l_patches  = strlen(h_patches);
	size_t l_d_total  = strlen(h_d_total);
	size_t l_d_failed = strlen(h_d_failed);
	size_t l_d_active = strlen(h_d_active);
	
	for (auto mod : AutoList<IMod>::List()) {
		std::string fullname(mod->GetName());
		size_t off_sep = fullname.rfind(':');
		
		ModInfo info;
		info.mod  = mod;
		info.cat  = fullname.substr(0, off_sep);
		info.name = fullname.substr(off_sep + 1);
		
		if (mod->m_bLoaded) {
			info.c_status = c_good;
			info.status   = "OK";
		} else {
			if (mod->m_bFailed) {
				info.c_status = c_bad;
				info.status   = "FAILED";
			} else {
				info.c_status = c_normal;
				info.status   = "UNLOADED";
			}
		}
		
		info.c_patches = (mod->GetNumPatches() != 0 ? c_bright : c_normal);
		info.patches   = std::to_string(mod->GetNumPatches());
		
		int n_detours_failed = 0;
		int n_detours_active = 0;
		for (const auto& pair : mod->Detours()) {
			IDetour *detour = pair.second;
			
			if (detour->IsLoaded()) {
				if (detour->IsActive()) {
					++n_detours_active;
				}
			} else {
				++n_detours_failed;
			}
		}
		
		info.c_d_total = (mod->GetNumDetours() != 0 ? c_bright : c_normal);
		info.d_total   = std::to_string(mod->GetNumDetours());
		
		info.c_d_failed = (n_detours_failed != 0 ? c_bad : c_normal);
		info.d_failed   = std::to_string(n_detours_failed);
		
		info.c_d_active = (n_detours_active != 0 ? c_note : c_normal);
		info.d_active   = std::to_string(n_detours_active);
		
		mods.insert(info);
		
		l_cat      = Max(l_cat,      info.cat.length());
		l_name     = Max(l_name,     info.name.length());
		l_status   = Max(l_status,   info.status.length());
		l_patches  = Max(l_patches,  info.patches.length());
		l_d_total  = Max(l_d_total,  info.d_total.length());
		l_d_failed = Max(l_d_failed, info.d_failed.length());
		l_d_active = Max(l_d_active, info.d_active.length());
	}
	
	ConColorMsg(c_header, "%-*s%-*s%-*s%-*s%-*s%-*s%-*s\n",
		2 + l_cat,      h_cat,
		2 + l_name,     h_name,
		2 + l_status,   h_status,
		2 + l_patches,  h_patches,
		2 + l_d_total,  h_d_total,
		2 + l_d_failed, h_d_failed,
		2 + l_d_active, h_d_active);
	
	for (const auto& info : mods) {
		ConColorMsg(c_normal,        "%-*s",   2 + l_cat,      info.cat.c_str());
		ConColorMsg(c_normal,        "%-*s",   2 + l_name,     info.name.c_str());
		ConColorMsg(info.c_status,   "%-*s",   2 + l_status,   info.status.c_str());
		ConColorMsg(info.c_patches,  "%-*s",   2 + l_patches,  info.patches.c_str());
		ConColorMsg(info.c_d_total,  "%-*s",   2 + l_d_total,  info.d_total.c_str());
		ConColorMsg(info.c_d_failed, "%-*s",   2 + l_d_failed, info.d_failed.c_str());
		ConColorMsg(info.c_d_active, "%-*s\n", 2 + l_d_active, info.d_active.c_str());
	}
}
