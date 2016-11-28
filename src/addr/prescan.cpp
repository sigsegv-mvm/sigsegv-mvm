#include "addr/prescan.h"


//CSingleScan *PreScan::s_WinRTTI_server = nullptr;


void PreScan::DoScans()
{
#if defined _WINDOWS
	DevMsg("PreScan::DoScans\n");
	
//	s_WinRTTI_server = new CSingleScan(ScanDir::FORWARD, CLibSegBounds(Library::SERVER, Segment::DATA), 1, new CStringPrefixScanner(ScanResults::ALL, ".?AV"));
//	
//	for (auto match : s_WinRTTI_server->Matches()) {
//		DevMsg("[PreScan] 0x%08x \"%s\"\n", (uintptr_t)match, (const char *)match);
//	}
#endif
}
