#include "link/link.h"


namespace Link
{
	bool InitAll(char *error, size_t maxlen)
	{
		DevMsg("Link::InitAll BEGIN\n");
		
		for (auto link : AutoList<ILinkage>::List()) {
			if (!link->Link(error, maxlen)) {
				DevMsg("Link::InitAll FAIL\n");
				return false;
			}
		}
		
		DevMsg("Link::InitAll OK\n");
		return true;
	}
}
