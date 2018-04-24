#include "link/link.h"


static std::vector<IGameSystem *> s_RegisteredSystems;


static bool IGameSystem_IsRegistered(IGameSystem *pSys)
{
	return (std::find(s_RegisteredSystems.begin(), s_RegisteredSystems.end(), pSys) != s_RegisteredSystems.end());
}

#if 0
static bool IGameSystem_IsRegistered(const char *name)
{
	return (std::find_if(s_RegisteredSystems.begin(), s_RegisteredSystems.end(), [=](IGameSystem *pSys){
		return (pSys->Name() != nullptr && strcmp(pSys->Name(), name) == 0);
	}) != s_RegisteredSystems.end());
}
#endif


static StaticFuncThunk<void, IGameSystem *> ft_IGameSystem_Add("IGameSystem::Add");
static void IGameSystem_ActualAdd(IGameSystem *pSys) { ft_IGameSystem_Add(pSys); }

static StaticFuncThunk<void, IGameSystem *> ft_IGameSystem_Remove("IGameSystem::Remove");
static void IGameSystem_ActualRemove(IGameSystem *pSys) { ft_IGameSystem_Remove(pSys); }


static bool IGameSystem_AreFuncsLinked() { return (ft_IGameSystem_Add.IsLinked() && ft_IGameSystem_Remove.IsLinked()); }


static void IGameSystem_SafeAdd(IGameSystem *pSys)
{
	if (!IGameSystem_AreFuncsLinked()) return;
	
	if (IGameSystem_IsRegistered(pSys)) {
//		Msg("[IGameSystem::SafeAdd] Game system \"%s\" is already registered!\n", pSys->Name());
		return;
	}
	
	IGameSystem_ActualAdd(pSys);
	s_RegisteredSystems.push_back(pSys);
	
//	Msg("[IGameSystem::SafeAdd] Game system \"%s\" registered successfully.\n", pSys->Name());
}

static void IGameSystem_SafeRemove(IGameSystem *pSys)
{
	if (!IGameSystem_AreFuncsLinked()) return;
	
	if (!IGameSystem_IsRegistered(pSys)) {
//		Msg("[IGameSystem::SafeRemove] Game system \"%s\" is not registered!\n", pSys->Name());
		return;
	}
	
	IGameSystem_ActualRemove(pSys);
	std::remove(s_RegisteredSystems.begin(), s_RegisteredSystems.end(), pSys);
	
//	Msg("[IGameSystem::SafeRemove] Game system \"%s\" unregistered successfully.\n", pSys->Name());
}


void IGameSystem::Add(IGameSystem *pSys)    { IGameSystem_SafeAdd(pSys);    }
void IGameSystem::Remove(IGameSystem *pSys) { IGameSystem_SafeRemove(pSys); }


CAutoGameSystem::CAutoGameSystem(const char *name)                 : m_pszName(name) { Add(this); }
CAutoGameSystemPerFrame::CAutoGameSystemPerFrame(const char *name) : m_pszName(name) { Add(this); }


// having global/static objects derived from IGameSystem, which automatically call Remove(this) in their dtor, which
// then check an ALSO-global/static vector to see whether the game system is registered, is inherently problematic due
// to static (de)initialization ordering uncertainty: this vector may well be destructed before the game system is

// so instead, be careful and make sure to call Remove(this) in a de-initialization function; the object dtor is NOT a
// safe choice for where to put this, if the object instance will be global/static

IGameSystem::~IGameSystem()                 {}
IGameSystemPerFrame::~IGameSystemPerFrame() {}

#if 0
IGameSystem::~IGameSystem()                 { Remove(this); }
IGameSystemPerFrame::~IGameSystemPerFrame() { Remove(this); }
#endif

// NOT IMPLEMENTED
// void IGameSystem::RemoveAll(IGameSystem *pSys)


// NOT IMPLEMENTED
// const char *IGameSystem::MapName()
