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
