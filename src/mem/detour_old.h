#ifndef _INCLUDE_SIGSEGV_MEM_DETOUR_H_
#define _INCLUDE_SIGSEGV_MEM_DETOUR_H_


#include "abi.h"
#include <asm/asm.h>
#include <CDetour/detourhelpers.h>


// current problems:
// - if two detours are setup for the same func, the second detour will overwrite the first
// - worse, if we enable A, enable B, then disable A, disable B, A will still be enabled!

// solution:
// keep a global list of all detours, sorted by actual func address
// have CUniqueDetour, which will refuse to enable if another detour is enabled at that address currently
// have CSharedDetour, which will maybe be less efficient but handle multi-detours safely


// func:       the actual function in the game code
// detour:     the function in the mod which overrides it
// trampoline: the trampoline used to call the original func from the detour
class IDetour
{
public:
	virtual ~IDetour()
	{
		this->Disable();
	}
	
	void Enable()
	{
		if (this->m_bInit && !this->m_bActive) {
			this->SetActive();
		}
	}
	void Disable()
	{
		if (this->m_bInit && this->m_bActive) {
			this->SetInactive();
		}
	}
	
	bool IsInitialized() const { return this->m_bInit; }
	bool IsActive() const      { return this->m_bInit && this->m_bActive; }
	
protected:
	IDetour(const char *func_name, void *detour_addr, void **trampoline_ptr) :
		m_FuncName(func_name), m_DetourAddr(detour_addr), m_TrampolinePtr(trampoline_ptr)
	{
		this->m_FuncAddr = AddrManager::GetAddr(func_name);
		if (this->m_FuncAddr == nullptr) {
			return;
		}
		
		this->m_bInit = true;
	}
	
	static std::multimap<void *, IDetour *> s_ActiveDetours;
	
private:
	
	
	void SetActive()
	{
		
		
		this->m_bActive = true;
	}
	void SetInactive()
	{
		
		
		this->m_bActive = false;
	}
	
	const char *m_FuncName;
	void *m_FuncAddr = nullptr;
	
	void *m_DetourAddr;
	
	void **m_TrampolinePtr;
	void *m_TrampolineBuf = nullptr;
	
	bool m_bInit   = false;
	bool m_bActive = false;
	
	patch_t m_RestorePatch;
};


class CUniqueDetour : public IDetour
{
public:
	CUniqueDetour() {}
	virtual ~CUniqueDetour() {}
	
private:
	
};

// TODO: definition of IDetour::s_ActiveDetours in cpp file


#define DETOUR_MEMBER_CALL(name) (this->*name##_Actual)
#define DETOUR_STATIC_CALL(name) (Actual_##name)

#define DETOUR_DECL_STATIC(ret, name, ...) \
	CDetour *detour_##name = nullptr; \
	static ret (*Actual_##name)(__VA_ARGS__) = nullptr; \
	static ret Detour_##name(__VA_ARGS__)

#define DETOUR_DECL_MEMBER(ret, name, ...) \
	static CDetour *detour_##name = nullptr; \
	class Detour_##name \
	{ \
	public: \
		ret name(__VA_ARGS__); \
		static ret (Detour_##name::* name##_Actual)(__VA_ARGS__); \
	}; \
	ret (Detour_##name::* Detour_##name::name##_Actual)(__VA_ARGS__) = nullptr; \
	ret Detour_##name::name(__VA_ARGS__)

#define GET_MEMBER_CALLBACK(name)   GetAddrOfMemberFunc(&Detour_##name::name)
#define GET_MEMBER_TRAMPOLINE(name) (void **)(&Detour_##name::name##_Actual)

#define GET_STATIC_CALLBACK(name) &Detour_##name
#define GET_STATIC_TRAMPOLINE(name) (void **)(&Actual_##name)


#endif
