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


bool IMod::InvokeLoad(char *error, size_t maxlen)
{
	return this->OnLoad(error, maxlen);
}

void IMod::InvokeUnload()
{
	this->OnUnload();
	
	for (auto pair : this->m_Detours) {
		CDetour *detour = pair.second;
		
		detour->Destroy();
	}
	this->m_Detours.clear();
}


void IMod::AddDetour(const char *name, CDetour *detour)
{
	assert(detour != nullptr);
	assert(this->m_Detours.find(name) == this->m_Detours.end());
	
	this->m_Detours[name] = detour;
}

void IMod::ToggleDetour(const char *name, bool enable)
{
	CDetour *detour = this->m_Detours.at(name);
	
	if (enable) {
		detour->EnableDetour();
	} else {
		detour->DisableDetour();
	}
}

void IMod::ToggleAllDetours(bool enable) {
	for (auto pair : this->m_Detours) {
		CDetour *detour = pair.second;
		
		if (enable) {
			detour->EnableDetour();
		} else {
			detour->DisableDetour();
		}
	}
}
