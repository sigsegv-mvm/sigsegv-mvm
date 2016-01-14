#include "library.h"
#include "common.h"


void *LibMgr::s_pLibServer = nullptr;
void *LibMgr::s_pLibEngine = nullptr;


void LibMgr::SetPtr(Library lib, void *ptr)
{
	switch (lib) {
	case Library::SERVER:
		s_pLibServer = ptr;
		break;
	case Library::ENGINE:
		s_pLibEngine = ptr;
		break;
	default:
		assert(false);
	}
}

void *LibMgr::GetPtr(Library lib)
{
	switch (lib) {
	case Library::SERVER:
		return s_pLibServer;
	case Library::ENGINE:
		return s_pLibEngine;
	default:
		return nullptr;
	}
}
