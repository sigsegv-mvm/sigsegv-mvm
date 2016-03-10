#include "mem/detour.h"
#include "addr/addr.h"
#include "mem/protect.h"


#if !(defined(__i386) || defined(_M_IX86))
#error Architecture must be IA32
#endif


//#define DETOUR_DEBUG 1


#if DETOUR_DEBUG
#define TRACE_ENABLE 1
#define TRACE_TERSE  1
#endif
#include "util/trace.h"


class JmpRelImm32
{
public:
	static constexpr size_t Size() { return 5; }
	
	/* is this atomic? no
	 * do we care?     no */
	static void Write(void *buf, uint32_t target)
	{
		uint32_t reltgt = target - ((uintptr_t)buf + Size());
		
		*(uint8_t  *)((uintptr_t)buf + 0x00) = 0xe9;
		*(uint32_t *)((uintptr_t)buf + 0x01) = reltgt;
		
		TRACE("[%08x -> %08x]: %02x %02x %02x %02x %02x",
			(uintptr_t)buf, target,
			((uint8_t *)buf)[0],
			((uint8_t *)buf)[1],
			((uint8_t *)buf)[2],
			((uint8_t *)buf)[3],
			((uint8_t *)buf)[4]);
	}
	
	/* pad out the empty space with NOP's to avoid confusing the disassembler */
	static void WritePadded(void *buf, uint32_t target, size_t len)
	{
		Write(buf, target);
		
		for (size_t i = Size(); i < len; ++i) {
			*(uint8_t *)((uintptr_t)buf + i) = 0x90;
		}
	}
};

class CallAbsMem32
{
public:
	static constexpr size_t Size() { return 6; }
	
	static void Write(void *buf, uint32_t target)
	{
		*(uint8_t  *)((uintptr_t)buf + 0x00) = 0xff;
		*(uint8_t  *)((uintptr_t)buf + 0x01) = 0x15;
		*(uint32_t *)((uintptr_t)buf + 0x02) = target;
		
		TRACE("[%08x -> %08x]: %02x %02x %02x %02x %02x %02x",
			(uintptr_t)buf, target,
			((uint8_t *)buf)[0],
			((uint8_t *)buf)[1],
			((uint8_t *)buf)[2],
			((uint8_t *)buf)[3],
			((uint8_t *)buf)[4],
			((uint8_t *)buf)[5]);
	}
};

class PushImm32
{
public:
	static constexpr size_t Size() { return 5; }
	
	static void Write(void *buf, uint32_t val)
	{
		*(uint8_t  *)((uintptr_t)buf + 0x00) = 0x68;
		*(uint32_t *)((uintptr_t)buf + 0x01) = val;
		
		TRACE("[%08x: %08x]: %02x %02x %02x %02x %02x",
			(uintptr_t)buf, val,
			((uint8_t *)buf)[0],
			((uint8_t *)buf)[1],
			((uint8_t *)buf)[2],
			((uint8_t *)buf)[3],
			((uint8_t *)buf)[4]);
	}
};


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


bool CDetour::DoLoad()
{
	TRACE("[this: %08x \"%s\"]", (uintptr_t)this, this->GetName());
	
	if (this->m_bFuncByName) {
		this->m_pFunc = AddrManager::GetAddr(this->m_strFuncName.c_str());
		if (this->m_pFunc == nullptr) {
			DevMsg("CDetour::DoLoad: \"%s\": addr lookup failed for \"%s\"\n", this->GetName(), this->m_strFuncName.c_str());
			return false;
		}
	} else {
		if (this->m_pFunc == nullptr) {
			DevMsg("CDetour::DoLoad: \"%s\": func ptr provided is nullptr\n", this->GetName());
			return false;
		}
	}
	
	if (!this->EnsureUniqueInnerPtrs()) {
		return false;
	}
	
	s_LoadedDetours.push_back(this);
	return true;
}

void CDetour::DoUnload()
{
	TRACE("[this: %08x \"%s\"]", (uintptr_t)this, this->GetName());
	
	s_LoadedDetours.erase(std::remove(s_LoadedDetours.begin(), s_LoadedDetours.end(), this), s_LoadedDetours.end());
}


void CDetour::DoEnable()
{
	TRACE("[this: %08x \"%s\"]", (uintptr_t)this, this->GetName());
	
	CDetouredFunc::Find(this->m_pFunc).AddDetour(this);
	s_ActiveDetours.push_back(this);
}

void CDetour::DoDisable()
{
	TRACE("[this: %08x \"%s\"]", (uintptr_t)this, this->GetName());
	
	CDetouredFunc::Find(this->m_pFunc).RemoveDetour(this);
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


void CFuncTrace::TracePre()
{
	IndentMsg("TracePre:  %s\n", this->GetName());
	TraceLevel::Increment();
}

void CFuncTrace::TracePost()
{
	TraceLevel::Decrement();
	IndentMsg("TracePost: %s\n", this->GetName());
}


bool CFuncTrace::DoLoad()
{
	TRACE("[this: %08x \"%s\"]", (uintptr_t)this, this->GetName());
	
	this->m_pFunc = LibMgr::FindSym(this->m_Library, this->m_strSymbol.c_str());
	if (this->m_pFunc == nullptr) {
		DevMsg("CFuncTrace::DoLoad: \"%s\": symbol lookup failed\n", this->GetName());
		return false;
	}
	
	return true;
}

void CFuncTrace::DoUnload()
{
	TRACE("[this: %08x \"%s\"]", (uintptr_t)this, this->GetName());
}


void CFuncTrace::DoEnable()
{
	TRACE("[this: %08x \"%s\"]", (uintptr_t)this, this->GetName());
	
	CDetouredFunc::Find(this->m_pFunc).AddTrace(this);
}

void CFuncTrace::DoDisable()
{
	TRACE("[this: %08x \"%s\"]", (uintptr_t)this, this->GetName());
	
	CDetouredFunc::Find(this->m_pFunc).RemoveTrace(this);
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


void CDetouredFunc::AddTrace(CFuncTrace *trace)
{
	TRACE("[this: %08x] [trace: %08x \"%s\"]", (uintptr_t)this, (uintptr_t)trace, trace->GetName());
	
	assert(std::count(this->m_Traces.begin(), this->m_Traces.end(), trace) == 0);
	this->m_Traces.push_back(trace);
	
	this->Reconfigure();
}

void CDetouredFunc::RemoveTrace(CFuncTrace *trace)
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
	
	size_t size = (&Wrapper_end - &Wrapper_begin);
	this->m_pWrapper = g_pSM->GetScriptingEngine()->AllocatePageMemory(size);
	
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
}

void CDetouredFunc::DestroyWrapper()
{
	TRACE("[this: %08x]", (uintptr_t)this);
	
	if (this->m_pWrapper == nullptr) {
		g_pSM->GetScriptingEngine()->FreePageMemory(this->m_pWrapper);
	}
}


void CDetouredFunc::CreateTrampoline()
{
	TRACE("[this: %08x]", (uintptr_t)this);
	
	assert(this->IsPrologueValid());
	
	size_t size = this->m_Prologue.size() + JmpRelImm32::Size();
	TRACE_MSG("size = %u\n", size);
	
	this->m_pTrampoline = g_pSM->GetScriptingEngine()->AllocatePageMemory(size);
	TRACE_MSG("trampoline @ %08x\n", (uintptr_t)this->m_pTrampoline);
	memcpy(this->m_pTrampoline, this->m_Prologue.data(), this->m_Prologue.size());
	
	void *where = (void *)((uintptr_t)this->m_pTrampoline + this->m_Prologue.size());
	uint32_t target = (uint32_t)this->m_pFunc + this->m_Prologue.size();
	JmpRelImm32::Write(where, target);
}

void CDetouredFunc::DestroyTrampoline()
{
	TRACE("[this: %08x]", (uintptr_t)this);
	
	if (this->m_pTrampoline == nullptr) {
		g_pSM->GetScriptingEngine()->FreePageMemory(this->m_pTrampoline);
	}
}


void CDetouredFunc::StorePrologue()
{
	TRACE("[this: %08x]", (uintptr_t)this);
	
	assert(!this->IsPrologueValid());
	
	size_t n_bytes = copy_bytes((unsigned char *)this->m_pFunc, nullptr, JmpRelImm32::Size());
	
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
	
	if (!this->m_Traces.empty()) {
		if (jump_to != nullptr) {
			this->m_pWrapperInner = jump_to;
		} else {
			this->m_pWrapperInner = this->m_pTrampoline;
		}
		
		jump_to = this->m_pWrapper;
	}
	
	if (jump_to != nullptr) {
		this->InstallJump(jump_to);
	}
}


void CDetouredFunc::InstallJump(void *target)
{
	TRACE("[this: %08x] [target: %08x]", (uintptr_t)this, (uintptr_t)target);
	
	this->FuncEnableWrite();
	JmpRelImm32::WritePadded(this->m_pFunc, (uint32_t)target, this->m_Prologue.size());
	this->FuncDisableWrite();
}

void CDetouredFunc::UninstallJump()
{
	TRACE("[this: %08x]", (uintptr_t)this);
	
	assert(this->IsPrologueValid());
	
	this->FuncEnableWrite();
	memcpy(this->m_pFunc, this->m_Prologue.data(), this->m_Prologue.size());
	this->FuncDisableWrite();
}


void CDetouredFunc::FuncEnableWrite()
{
	MemProtect(this->m_pFunc, this->m_Prologue.size(), false);
}

void CDetouredFunc::FuncDisableWrite()
{
	MemProtect(this->m_pFunc, this->m_Prologue.size(), true);
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


thread_local std::vector<uint32_t> CDetouredFunc::s_SaveEIP;


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
