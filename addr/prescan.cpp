#include "addr/prescan.h"


IScan *PreScan::s_WinRTTI_server = nullptr;


void PreScan::DoScans()
{
#if defined _WINDOWS
	DevMsg("PreScan::DoScans\n");
	
	s_WinRTTI_server = new CStringPrefixScan(ScanDir::FORWARD, ScanResults::ALL,
		CLibSegBounds(Library::SERVER, ".data"), 1, ".?AV");
	
//	for (auto str : s_WinRTTI_server->Matches()) {
//		DevMsg("[PreScan] 0x%08x \"%s\"\n", (uintptr_t)str, (const char *)str);
//	}
#endif
}
