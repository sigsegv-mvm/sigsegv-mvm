#include "link/link.h"


#if defined _WIN32
#define LINK_NONFATAL
#endif


namespace Link
{
	bool InitAll(char *error, size_t maxlen)
	{
		DevMsg("Link::InitAll BEGIN\n");
		
		for (auto link : AutoList<ILinkage>::List()) {
			if (!link->Link(error, maxlen)) {
#if !defined LINK_NONFATAL
				DevMsg("Link::InitAll FAIL\n");
				return false;
#endif
			}
		}
		
		DevMsg("Link::InitAll OK\n");
		return true;
	}
}
