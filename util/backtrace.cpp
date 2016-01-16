#include "util/backtrace.h"


#if defined _LINUX || defined _OSX

#include <execinfo.h>


void print_backtrace()
{
	constexpr int MAX_ENTRIES = 0x100;
	
	uintptr_t entries[MAX_ENTRIES];
	memset(entries, 0x00, sizeof(entries));
	
	int num_entries = backtrace((void **)entries, MAX_ENTRIES);
	char **symbols = backtrace_symbols((void *const *)entries, num_entries);
	
	/* skip entry 0, which will point to this function */
	for (int i = 1; i < num_entries; ++i) {
		DevMsg("  %3d  %s\n", i, symbols[i]);
	}
	
	free(symbols);
}

#endif
