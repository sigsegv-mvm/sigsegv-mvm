#include "mem/detour.h"
#include "addr/addr.h"
#include "mem/protect.h"
#include "mem/opcode.h"
#include "util/backtrace.h"


#if !(defined(__i386) || defined(_M_IX86))
#error Architecture must be IA32
#endif


//#define DETOUR_DEBUG 1


#if DETOUR_DEBUG
#define TRACE_ENABLE 1
#define TRACE_TERSE  1
#endif
#include "util/trace.h"


/* literally all I want from asm.h is copy_bytes */
extern "C" int copy_bytes(unsigned char *func, unsigned char *dest, int required_len);
// TODO: stop using copy_bytes and find a way to make capstone do what we need


std::list<CDetour *> CDetour::s_LoadedDetours;
std::list<CDetour *> CDetour::s_ActiveDetours;


std::map<void *, CDetouredFunc> CDetouredFunc::s_FuncMap;


bool IDetour::Load()
{
	TRACE("[this: %08x \"%s\"]", (uintptr_t)this, this->GetName());
	
	if (this->DoLoad()) {
		this->m_bLoaded = true;
		return true;
	}
	
	return false;
}

void IDetour::Unload()
{
	TRACE("[this: %08x \"%s\"]", (uintptr_t)this, this->GetName());
	
	this->Disable();
	this->DoUnload();
}


void IDetour::Toggle(bool enable)
{
	if (enable) {
		this->Enable();
	} else {
		this->Disable();
	}
}

void IDetour::Enable()
{
	TRACE("[this: %08x \"%s\"] loaded:%c active:%c", (uintptr_t)this, this->GetName(),
		(this->IsLoaded() ? 'Y' : 'N'),
		(this->IsActive() ? 'Y' : 'N'));
	
	if (this->IsLoaded() && !this->IsActive()) {
		this->DoEnable();
		this->m_bActive = true;
	}
}

void IDetour::Disable()
{
	TRACE("[this: %08x \"%s\"] loaded:%c active:%c", (uintptr_t)this, this->GetName(),
		(this->IsLoaded() ? 'Y' : 'N'),
		(this->IsActive() ? 'Y' : 'N'));
	
	if (this->IsLoaded() && this->IsActive()) {
		this->DoDisable();
		this->m_bActive = false;
	}
}


void IDetour::DoEnable()
{
	TRACE("[this: %08x \"%s\"]", (uintptr_t)this, this->GetName());
	
	ITrace *trace   = nullptr;
	CDetour *detour = nullptr;
	
	if ((trace = dynamic_cast<ITrace *>(this)) != nullptr) {
		CDetouredFunc::Find(this->GetFuncPtr()).AddTrace(trace);
	} else if ((detour = dynamic_cast<CDetour *>(this)) != nullptr) {
		CDetouredFunc::Find(this->GetFuncPtr()).AddDetour(detour);
	} else {
		/* don't know how to deal with this kind of IDetour */
		assert(false);
	}
}

void IDetour::DoDisable()
{
	TRACE("[this: %08x \"%s\"]", (uintptr_t)this, this->GetName());
	
	ITrace *trace   = nullptr;
	CDetour *detour = nullptr;
	
	if ((trace = dynamic_cast<ITrace *>(this)) != nullptr) {
		CDetouredFunc::Find(this->GetFuncPtr()).RemoveTrace(trace);
	} else if ((detour = dynamic_cast<CDetour *>(this)) != nullptr) {
		CDetouredFunc::Find(this->GetFuncPtr()).RemoveDetour(detour);
	} else {
		/* don't know how to deal with this kind of IDetour */
		assert(false);
	}
}


bool IDetour_SymNormal::DoLoad()
{
	TRACE("[this: %08x \"%s\"]", (uintptr_t)this, this->GetName());
	
	if (this->m_bFuncByName) {
		this->m_pFunc = AddrManager::GetAddr(this->m_strFuncName.c_str());
		if (this->m_pFunc == nullptr) {
			DevMsg("IDetour_SymNormal::DoLoad: \"%s\": addr lookup failed for \"%s\"\n", this->GetName(), this->m_strFuncName.c_str());
			return false;
		}
	} else {
		if (this->m_pFunc == nullptr) {
			DevMsg("IDetour_SymNormal::DoLoad: \"%s\": func ptr provided is nullptr\n", this->GetName());
			return false;
		}
	}
	
	return true;
}

void IDetour_SymNormal::DoUnload()
{
	TRACE("[this: %08x \"%s\"]", (uintptr_t)this, this->GetName());
}


/* iterator for non-null-terminated symbol name strings, to avoid having to make
 * std::string copies a bunch of times for regex matching */
class CharPtrIterator : public std::iterator<std::bidirectional_iterator_tag, char>
{
public:
	CharPtrIterator() :
		m_pChar(nullptr) {}
	CharPtrIterator(char *ptr) :
		m_pChar(ptr) {}
	CharPtrIterator(const CharPtrIterator& that) = default;
	
	CharPtrIterator& operator++()   { ++this->m_pChar; return *this; }
	CharPtrIterator operator++(int) { CharPtrIterator tmp(*this); ++this->m_pChar; return tmp; }
	
	CharPtrIterator& operator--()   { --this->m_pChar; return *this; }
	CharPtrIterator operator--(int) { CharPtrIterator tmp(*this); --this->m_pChar; return tmp; }
	
	bool operator==(const CharPtrIterator& that) const { return this->m_pChar == that.m_pChar; }
	bool operator!=(const CharPtrIterator& that) const { return this->m_pChar != that.m_pChar; }
	
	char& operator*() const { return *this->m_pChar; }
	
private:
	char *m_pChar;
};


const char *IDetour_SymRegex::GetName() const
{
	if (this->IsLoaded()) {
		return this->m_strDemangled.c_str();
	} else {
		return this->m_strPattern.c_str();
	}
}


bool IDetour_SymRegex::DoLoad()
{
	TRACE("[this: %08x \"%s\"]", (uintptr_t)this, this->GetName());
	
	if (this->m_Library == Library::INVALID) {
		DevMsg("IDetour_SymRegex::DoLoad: \"%s\": invalid library\n", this->GetName());
		return false;
	}
	
	const LibInfo& info = LibMgr::GetInfo(this->m_Library);
	void *text_begin = (void *)(info.baseaddr         + info.segs.at(".text").off);
	void *text_end   = (void *)((uintptr_t)text_begin + info.segs.at(".text").len);
	
	std::regex filter(this->m_strPattern);
	std::vector<Symbol *> syms;
	LibMgr::ForEachSym(this->m_Library, [&](Symbol *sym){
		char *buf  = sym->buffer();
		size_t len = sym->length;
		
		CharPtrIterator begin(buf), end(buf + len);
		if (std::regex_search(begin, end, filter, std::regex_constants::match_any)) {
			if (sym->address >= text_begin && sym->address < text_end) {
				syms.push_back(sym);
			}
		}
	});
	
	if (syms.size() != 1) {
		DevMsg("IDetour_SymRegex::DoLoad: \"%s\": symbol lookup failed (%u matches):\n", this->GetName(), syms.size());
		for (auto sym : syms) {
			std::string name(sym->buffer(), sym->length);
			DevMsg("  %s\n", name.c_str());
		}
		return false;
	}
	
	this->m_strSymbol = std::string(syms[0]->buffer(), syms[0]->length);
	this->m_pFunc     = syms[0]->address;
	
	this->Demangle();
	
	return true;
}

void IDetour_SymRegex::DoUnload()
{
	TRACE("[this: %08x \"%s\"]", (uintptr_t)this, this->GetName());
}


void IDetour_SymRegex::Demangle()
{
#if defined _LINUX || defined _OSX
	const char *demangled = cplus_demangle(this->m_strSymbol.c_str(), DMGL_GNU_V3 | DMGL_TYPES | DMGL_ANSI | DMGL_PARAMS);
	if (demangled != nullptr) {
		this->m_strDemangled = demangled;
		free((void *)demangled);
	} else {
		this->m_strDemangled = this->m_strSymbol;
	}
#else
	this->m_strDemangled = this->m_strSymbol;
#endif
}


bool CDetour::DoLoad()
{
	if (!IDetour_SymNormal::DoLoad()) {
		return false;
	}
	
	if (!this->EnsureUniqueInnerPtrs()) {
		return false;
	}
	
	s_LoadedDetours.push_back(this);
	return true;
}

void CDetour::DoUnload()
{
	IDetour_SymNormal::DoUnload();
	
	s_LoadedDetours.erase(std::remove(s_LoadedDetours.begin(), s_LoadedDetours.end(), this), s_LoadedDetours.end());
}


void CDetour::DoEnable()
{
	IDetour::DoEnable();
	
	s_ActiveDetours.push_back(this);
}

void CDetour::DoDisable()
{
	IDetour::DoDisable();
	
	s_ActiveDetours.erase(std::remove(s_ActiveDetours.begin(), s_ActiveDetours.end(), this), s_ActiveDetours.end());
}


/* ensure that no two loaded CDetour instances have the same m_pInner value:
 * this is important because it may seem like two different function detours can
 * share the same callback; but in fact, that's completely unsafe */
bool CDetour::EnsureUniqueInnerPtrs()
{
	TRACE("[this: %08x \"%s\"] %08x", (uintptr_t)this, this->GetName(), (uintptr_t)this->m_pInner);
	
	for (auto detour : s_LoadedDetours) {
		if (detour->m_pInner == this->m_pInner) {
			Warning("Found two CDetour instances with the same inner function pointer!\n"
				"this: %08x \"%s\"\n"
				"that: %08x \"%s\"\n",
				(uintptr_t)this, this->GetName(),
				(uintptr_t)detour, detour->GetName());
			return false;
		}
	}
	
	return true;
}


void CFuncCount::TracePre()
{
	++this->m_RefCount;
}

void CFuncCount::TracePost()
{
	--this->m_RefCount;
}


void CFuncCallback::TracePre()
{
	if (this->m_pCBPre != nullptr) {
		this->m_pCBPre();
	}
}

void CFuncCallback::TracePost()
{
	if (this->m_pCBPost != nullptr) {
		this->m_pCBPost();
	}
}


void CFuncTrace::TracePre()
{
	ConColorMsg(Color(0xff, 0xff, 0x00, 0xff), "[%7d] %*s%s ", gpGlobals->tickcount, 2 * TraceLevel::Get(), "", "{");
	ConColorMsg(Color(0x00, 0xff, 0xff, 0xff), "%s\n", this->GetName());
	TraceLevel::Increment();
}

void CFuncTrace::TracePost()
{
	TraceLevel::Decrement();
	ConColorMsg(Color(0xff, 0xff, 0x00, 0xff), "[%7d] %*s%s ", gpGlobals->tickcount, 2 * TraceLevel::Get(), "", "}");
	ConColorMsg(Color(0x00, 0xff, 0xff, 0xff), "%s\n", this->GetName());
}


void CFuncBacktrace::TracePre()
{
	BACKTRACE();
}

void CFuncBacktrace::TracePost()
{
}


void CFuncVProf::TracePre()
{
	if (g_VProfCurrentProfile.IsEnabled()) {
		g_VProfCurrentProfile.EnterScope(this->m_strVProfName.c_str(), 0,
			this->m_strVProfGroup.c_str(), false, BUDGETFLAG_OTHER);
	}
}

void CFuncVProf::TracePost()
{
	if (g_VProfCurrentProfile.IsEnabled()) {
		g_VProfCurrentProfile.ExitScope();
	}
}


CDetouredFunc::CDetouredFunc(void *func_ptr) :
	m_pFunc(func_ptr)
{
	TRACE("[this: %08x] [func: %08x]", (uintptr_t)this, (uintptr_t)func_ptr);
	
	this->StorePrologue();
	this->CreateWrapper();
	this->CreateTrampoline();
}
CDetouredFunc::~CDetouredFunc()
{
	TRACE("[this: %08x] [func: %08x]", (uintptr_t)this, (uintptr_t)this->m_pFunc);
	
	this->RemoveAllDetours();
	this->DestroyWrapper();
	this->DestroyTrampoline();
}


CDetouredFunc& CDetouredFunc::Find(void *func_ptr)
{
//	TRACE("%08x", (uintptr_t)func_ptr);
	
	auto it = s_FuncMap.find(func_ptr);
	if (it == s_FuncMap.end()) {
		/* oh god C++11 why do you do this to me */
		auto result = s_FuncMap.emplace(std::piecewise_construct, std::forward_as_tuple(func_ptr), std::forward_as_tuple(func_ptr));
		it = result.first;
	}
	
	return (*it).second;
	
//	TRACE_EXIT("%s", (result.second ? "inserted" : "existing"));
}


void CDetouredFunc::AddDetour(CDetour *detour)
{
	TRACE("[this: %08x] [detour: %08x \"%s\"]", (uintptr_t)this, (uintptr_t)detour, detour->GetName());
	
	assert(std::count(this->m_Detours.begin(), this->m_Detours.end(), detour) == 0);
	this->m_Detours.push_back(detour);
	
	this->Reconfigure();
}

void CDetouredFunc::RemoveDetour(CDetour *detour)
{
	TRACE("[this: %08x] [detour: %08x \"%s\"]", (uintptr_t)this, (uintptr_t)detour, detour->GetName());
	
	assert(std::count(this->m_Detours.begin(), this->m_Detours.end(), detour) == 1);
	this->m_Detours.erase(std::remove(this->m_Detours.begin(), this->m_Detours.end(), detour), this->m_Detours.end());
	
	this->Reconfigure();
}


void CDetouredFunc::AddTrace(ITrace *trace)
{
	TRACE("[this: %08x] [trace: %08x \"%s\"]", (uintptr_t)this, (uintptr_t)trace, trace->GetName());
	
	assert(std::count(this->m_Traces.begin(), this->m_Traces.end(), trace) == 0);
	this->m_Traces.push_back(trace);
	
	this->Reconfigure();
}

void CDetouredFunc::RemoveTrace(ITrace *trace)
{
	TRACE("[this: %08x] [trace: %08x \"%s\"]", (uintptr_t)this, (uintptr_t)trace, trace->GetName());
	
	assert(std::count(this->m_Traces.begin(), this->m_Traces.end(), trace) == 1);
	this->m_Traces.erase(std::remove(this->m_Traces.begin(), this->m_Traces.end(), trace), this->m_Traces.end());
	
	this->Reconfigure();
}


void CDetouredFunc::RemoveAllDetours()
{
	TRACE("[this: %08x]", (uintptr_t)this);
	
	this->m_Detours.clear();
	this->m_Traces.clear();
	
	this->Reconfigure();
}


extern "C"
{
	extern uint8_t Wrapper_begin;
	extern uint8_t Wrapper_end;
	extern uint8_t Wrapper_push_func_addr_1;
	extern uint8_t Wrapper_call_wrapper_pre;
	extern uint8_t Wrapper_call_actual_func;
	extern uint8_t Wrapper_push_func_addr_2;
	extern uint8_t Wrapper_call_wrapper_post;
}

void CDetouredFunc::CreateWrapper()
{
	TRACE("[this: %08x]", (uintptr_t)this);
	
#if !defined _WINDOWS
	size_t size = (&Wrapper_end - &Wrapper_begin);
	this->m_pWrapper = g_pSourcePawn->AllocatePageMemory(size);
	
	memcpy(this->m_pWrapper, &Wrapper_begin, size);
	
	auto func_addr_1  = (uintptr_t)this->m_pWrapper + (&Wrapper_push_func_addr_1  - &Wrapper_begin);
	auto wrapper_pre  = (uintptr_t)this->m_pWrapper + (&Wrapper_call_wrapper_pre  - &Wrapper_begin);
	auto actual_func  = (uintptr_t)this->m_pWrapper + (&Wrapper_call_actual_func  - &Wrapper_begin);
	auto func_addr_2  = (uintptr_t)this->m_pWrapper + (&Wrapper_push_func_addr_2  - &Wrapper_begin);
	auto wrapper_post = (uintptr_t)this->m_pWrapper + (&Wrapper_call_wrapper_post - &Wrapper_begin);
	
	PushImm32::Write(   (void *)func_addr_1,  (uint32_t)this->m_pFunc);
	CallAbsMem32::Write((void *)wrapper_pre,  (uint32_t)&this->m_pWrapperPre);
	CallAbsMem32::Write((void *)actual_func,  (uint32_t)&this->m_pWrapperInner);
	PushImm32::Write(   (void *)func_addr_2,  (uint32_t)this->m_pFunc);
	CallAbsMem32::Write((void *)wrapper_post, (uint32_t)&this->m_pWrapperPost);
#endif
}

void CDetouredFunc::DestroyWrapper()
{
	TRACE("[this: %08x]", (uintptr_t)this);
	
	if (this->m_pWrapper != nullptr) {
		g_pSourcePawn->FreePageMemory(this->m_pWrapper);
		this->m_pWrapper = nullptr;
	}
}


void CDetouredFunc::CreateTrampoline()
{
	TRACE("[this: %08x]", (uintptr_t)this);
	
	assert(this->IsPrologueValid());
	
	size_t size = this->m_Prologue.size() + JmpRelImm32::Size();
	TRACE_MSG("size = %u\n", size);
	
	this->m_pTrampoline = g_pSourcePawn->AllocatePageMemory(size);
	TRACE_MSG("trampoline @ %08x\n", (uintptr_t)this->m_pTrampoline);
	memcpy(this->m_pTrampoline, this->m_Prologue.data(), this->m_Prologue.size());
	
	void *where = (void *)((uintptr_t)this->m_pTrampoline + this->m_Prologue.size());
	uint32_t target = (uint32_t)this->m_pFunc + this->m_Prologue.size();
	JmpRelImm32::Write(where, target);
}

void CDetouredFunc::DestroyTrampoline()
{
	TRACE("[this: %08x]", (uintptr_t)this);
	
	if (this->m_pTrampoline != nullptr) {
		g_pSourcePawn->FreePageMemory(this->m_pTrampoline);
		this->m_pTrampoline = nullptr;
	}
}


void CDetouredFunc::StorePrologue()
{
	TRACE("[this: %08x]", (uintptr_t)this);
	
	assert(!this->IsPrologueValid());
	
	size_t n_bytes = copy_bytes((unsigned char *)this->m_pFunc, nullptr, JmpRelImm32::Size());
	assert(n_bytes >= JmpRelImm32::Size());
	
	this->m_Prologue.resize(n_bytes);
	memcpy(this->m_Prologue.data(), this->m_pFunc, n_bytes);
}


void CDetouredFunc::Reconfigure()
{
	TRACE("[this: %08x] with %u detour(s)", (uintptr_t)this, this->m_Detours.size());
	
	this->UninstallJump();
	
	void *jump_to = nullptr;
	
	if (!this->m_Detours.empty()) {
		CDetour *first = this->m_Detours.front();
		CDetour *last  = this->m_Detours.back();
		
		TRACE_MSG("detour[\"%s\"].inner [%08x] -> trampoline [%08x]\n",
			last->GetName(), (uintptr_t)last->m_pInner, (uintptr_t)this->m_pTrampoline);
		*last->m_pInner = this->m_pTrampoline;
		
		for (int i = this->m_Detours.size() - 2; i >= 0; --i) {
			CDetour *d1 = this->m_Detours[i];
			CDetour *d2 = this->m_Detours[i + 1];
			
			TRACE_MSG("detour[\"%s\"].inner [%08x] -> detour[\"%s\"].callback [%08x]\n",
				d1->GetName(), (uintptr_t)d1->m_pInner,
				d2->GetName(), (uintptr_t)d2->m_pCallback);
			*d1->m_pInner = d2->m_pCallback;
		}
		
		jump_to = first->m_pCallback;
		
		/*TRACE_MSG("func jump -> detour[\"%s\"].callback [%08x]\n",
			first->GetName(), (uintptr_t)first->m_pCallback);
		this->InstallJump(first->m_pCallback);*/
	}
	
#if !defined _WINDOWS
	if (!this->m_Traces.empty()) {
		if (jump_to != nullptr) {
			this->m_pWrapperInner = jump_to;
		} else {
			this->m_pWrapperInner = this->m_pTrampoline;
		}
		
		jump_to = this->m_pWrapper;
	}
#endif
	
	if (jump_to != nullptr) {
		this->InstallJump(jump_to);
	}
}


void CDetouredFunc::InstallJump(void *target)
{
	TRACE("[this: %08x] [target: %08x]", (uintptr_t)this, (uintptr_t)target);
	
	MemUnprotector prot(this->m_pFunc, this->m_Prologue.size());
	JmpRelImm32::WritePadded(this->m_pFunc, (uint32_t)target, this->m_Prologue.size());
}

void CDetouredFunc::UninstallJump()
{
	TRACE("[this: %08x]", (uintptr_t)this);
	
	assert(this->IsPrologueValid());
	
	MemUnprotector prot(this->m_pFunc, this->m_Prologue.size());
	memcpy(this->m_pFunc, this->m_Prologue.data(), this->m_Prologue.size());
}


void CDetouredFunc::FuncPre()
{
	for (auto trace : this->m_Traces) {
		trace->TracePre();
	}
}

void CDetouredFunc::FuncPost()
{
	for (auto trace : this->m_Traces) {
		trace->TracePost();
	}
}


/*thread_local*/ std::vector<uint32_t> CDetouredFunc::s_SaveEIP;


void CDetouredFunc::WrapperPre(void *func_ptr, uint32_t *eip)
{
//	DevMsg("WrapperPre [func: %08x] [eip: %08x]\n", (uintptr_t)func_ptr, (uintptr_t)eip);
	
	auto it = s_FuncMap.find(func_ptr);
	assert(it != s_FuncMap.end());
	(*it).second.FuncPre();
	
	s_SaveEIP.push_back(*eip);
}

void CDetouredFunc::WrapperPost(void *func_ptr, uint32_t *eip)
{
//	DevMsg("WrapperPost [func: %08x] [eip: %08x]\n", (uintptr_t)func_ptr, (uintptr_t)eip);
	
	assert(s_SaveEIP.size() >= 1);
	*eip = s_SaveEIP.back();
	s_SaveEIP.pop_back();
	
	auto it = s_FuncMap.find(func_ptr);
	assert(it != s_FuncMap.end());
	(*it).second.FuncPost();
}
