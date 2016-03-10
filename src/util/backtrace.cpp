#include "util/backtrace.h"
#include "library.h"


#if defined _LINUX || defined _OSX

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


void sym_get_proc_name(unw_cursor_t *cp, char *bufp, size_t len, unw_word_t *offp)
{
	snprintf(bufp, len, "???");
	*offp = 0;
	
	static unw_word_t r_ip;
	unw_get_reg(cp, UNW_REG_IP, &r_ip);
	
	Library lib = LibMgr::WhichLibAtAddr((void *)r_ip);
	if (lib == Library::INVALID) {
		return;
	}
	
	static Symbol *best = nullptr;
	LibMgr::ForEachSym(lib,
	[](Symbol *sym)
	{
		if (r_ip > (uintptr_t)sym->address) {
			if (best == nullptr || (r_ip - (uintptr_t)best->address) > (r_ip - (uintptr_t)sym->address)) {
				best = sym;
			}
		}
	}
	);
	
	if (best != nullptr) {
		snprintf(bufp, len, "%*s", best->length, best->buffer());
		*offp = r_ip - (uintptr_t)best->address;
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
		
		if (unw_get_proc_name(&cur, f_name, sizeof(f_name), &off) == -UNW_ENOINFO) {
			sym_get_proc_name(&cur, f_name, sizeof(f_name), &off);
		}
		
		const char *demangled = try_demangle(f_name);
		DevMsg("%3d  %08x  %08x  %s+0x%x\n", f_idx, r_sp, r_ip, demangled, off);
		free((void *)demangled);
		
		++f_idx;
		
		/* sanity check to prevent unending log-filling backtraces when
		 * libunwind gets confused about where the end of the stack is */
		if (f_idx >= 1000) break;
	}
}


extern "C"
{
	[[noreturn]] void __real___assert_fail(const char *__assertion, const char *__file, unsigned int __line, const char *__function);
	
	[[noreturn]] void __wrap___assert_fail(const char *__assertion, const char *__file, unsigned int __line, const char *__function)
	{
		BACKTRACE();
		__real___assert_fail(__assertion, __file, __line, __function);
	}
	
//	[[noreturn]] void __real_abort();
//
//	[[noreturn]] void __wrap_abort()
//	{
//		BACKTRACE();
//		__real_abort();
//	}
}


#endif
