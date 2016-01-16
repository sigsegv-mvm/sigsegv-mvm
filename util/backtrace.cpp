#include "util/backtrace.h"


#if defined _LINUX || defined _OSX

#include <libunwind.h>

#define HAVE_DECL_BASENAME 1
#include <libiberty/demangle.h>


const char *try_demangle(const char *mangled)
{
	if (strlen(mangled) == 0) {
		return strdup("???");
	}
	
	const char *demangled = cplus_demangle(mangled, DMGL_GNU_V3 | DMGL_TYPES | DMGL_ANSI | DMGL_PARAMS);
	
	if (demangled != nullptr) {
		return demangled;
	} else {
		return strdup(mangled);
	}
}


void print_backtrace()
{
	unw_context_t ctx;
	unw_getcontext(&ctx);
	
	unw_cursor_t cur;
	unw_init_local(&cur, &ctx);
	
	DevMsg("%-3s  %-10s%-10s%s\n",
		"FRM", "ESP", "EIP", "FUNC");
	
	int f_idx = 1;
	char f_name[0x1000];
	while (unw_step(&cur) > 0) {
		unw_word_t r_ip;
		unw_get_reg(&cur, UNW_REG_IP, &r_ip);
		
		unw_word_t r_sp;
		unw_get_reg(&cur, UNW_REG_SP, &r_sp);
		
		f_name[0] = '\0';
		unw_word_t off = 0;
		unw_get_proc_name(&cur, f_name, sizeof(f_name), &off);
		
		const char *demangled = try_demangle(f_name);
		DevMsg("%3d  %08x  %08x  %s+0x%x\n", f_idx, r_sp, r_ip, demangled, off);
		free((void *)demangled);
		
		++f_idx;
	}
}

#if 0
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

#endif
