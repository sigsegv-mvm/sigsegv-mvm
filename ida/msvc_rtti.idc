// IDA script
// parse Visual C++ RTTI
// by sigsegv


#include <idc.idc>


static main()
{
	// wait for auto-analysis
	Wait();
	
	auto seg_data  = SegByName(".data");
	auto seg_rdata = SegByName(".rdata");
	
	if (seg_data == BADSEL) {
		Warning("Can't find segment '.data'\n");
		return;
	}
	if (seg_rdata == BADSEL) {
		Warning("Can't find segment '.rdata'\n");
		return;
	}
	
	auto addr = 0;
	while ((addr = FindBinary(addr, SEARCH_DOWN | SEARCH_NEXT | SEARCH_CASE, "2e 3f 41 56")) != BADADDR) {
		if (SegName(addr) != ".data") continue;
		
		addr_TD_ -= 8;
		
		Message("found '.?AV' @ %x\n", addr_TD);
	}
}
