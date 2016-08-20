//#include "stub/igamesystem.h"
#include "link/link.h"


CAutoGameSystem::CAutoGameSystem(const char *name) :
	m_pszName(name)
{
	Add(this);
}

CAutoGameSystemPerFrame::CAutoGameSystemPerFrame(const char *name) :
	m_pszName(name)
{
	Add(this);
}


IGameSystem::~IGameSystem()
{
//	Remove(this);
}

IGameSystemPerFrame::~IGameSystemPerFrame()
{
//	Remove(this);
}


static StaticFuncThunk<void, IGameSystem *> ft_IGameSystem_Add("IGameSystem::Add");
void IGameSystem::Add(IGameSystem *pSys)
{
	ft_IGameSystem_Add(pSys);
}

static StaticFuncThunk<void, IGameSystem *> ft_IGameSystem_Remove("IGameSystem::Remove");
void IGameSystem::Remove(IGameSystem *pSys)
{
	ft_IGameSystem_Remove(pSys);
}

// NOT IMPLEMENTED
// void IGameSystem::RemoveAll(IGameSystem *pSys)


// NOT IMPLEMENTED
// const char *IGameSystem::MapName()


bool IGameSystem_IsLinked()
{
	return (ft_IGameSystem_Add.IsLinked() && ft_IGameSystem_Remove.IsLinked());
}
