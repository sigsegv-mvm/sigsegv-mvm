#include "link/link.h"


static std::vector<IGameSystem *> s_RegisteredSystems;


static bool IGameSystem_IsRegistered(IGameSystem *pSys)
{
	return (std::find(s_RegisteredSystems.begin(), s_RegisteredSystems.end(), pSys) != s_RegisteredSystems.end());
}

static bool IGameSystem_IsRegistered(const char *name)
{
	return (std::find_if(s_RegisteredSystems.begin(), s_RegisteredSystems.end(), [=](IGameSystem *pSys){
		return (pSys->Name() != nullptr && strcmp(pSys->Name(), name) == 0);
	}) != s_RegisteredSystems.end());
}


static StaticFuncThunk<void, IGameSystem *> ft_IGameSystem_Add("IGameSystem::Add");
static void IGameSystem_ActualAdd(IGameSystem *pSys) { ft_IGameSystem_Add(pSys); }

static StaticFuncThunk<void, IGameSystem *> ft_IGameSystem_Remove("IGameSystem::Remove");
static void IGameSystem_ActualRemove(IGameSystem *pSys) { ft_IGameSystem_Remove(pSys); }


static bool IGameSystem_AreFuncsLinked() { return (ft_IGameSystem_Add.IsLinked() && ft_IGameSystem_Remove.IsLinked()); }


static void IGameSystem_SafeAdd(IGameSystem *pSys)
{
	if (!IGameSystem_AreFuncsLinked()) return;
	
	if (IGameSystem_IsRegistered(pSys)) {
		Msg("[IGameSystem::SafeAdd] Game system \"%s\" is already registered!\n", pSys->Name());
		return;
	}
	
	IGameSystem_ActualAdd(pSys);
	s_RegisteredSystems.push_back(pSys);
	
	Msg("[IGameSystem::SafeAdd] Game system \"%s\" registered successfully.\n", pSys->Name());
}

static void IGameSystem_SafeRemove(IGameSystem *pSys)
{
	if (!IGameSystem_AreFuncsLinked()) return;
	
	if (!IGameSystem_IsRegistered(pSys)) {
		Msg("[IGameSystem::SafeRemove] Game system \"%s\" is not registered!\n", pSys->Name());
		return;
	}
	
	IGameSystem_ActualRemove(pSys);
	std::remove(s_RegisteredSystems.begin(), s_RegisteredSystems.end(), pSys);
	
	Msg("[IGameSystem::SafeRemove] Game system \"%s\" unregistered successfully.\n", pSys->Name());
}


void IGameSystem::Add(IGameSystem *pSys)    { IGameSystem_SafeAdd(pSys); }
void IGameSystem::Remove(IGameSystem *pSys) { IGameSystem_SafeRemove(pSys); }


CAutoGameSystem::CAutoGameSystem(const char *name)                 : m_pszName(name) { Add(this); }
CAutoGameSystemPerFrame::CAutoGameSystemPerFrame(const char *name) : m_pszName(name) { Add(this); }

IGameSystem::~IGameSystem()                 { Remove(this); }
IGameSystemPerFrame::~IGameSystemPerFrame() { Remove(this); }

// NOT IMPLEMENTED
// void IGameSystem::RemoveAll(IGameSystem *pSys)


// NOT IMPLEMENTED
// const char *IGameSystem::MapName()
