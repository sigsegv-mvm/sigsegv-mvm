#ifndef _INCLUDE_SIGSEGV_MEM_DETOUR_H_
#define _INCLUDE_SIGSEGV_MEM_DETOUR_H_


#include "abi.h"
#include "library.h"


class IDetour
{
public:
	virtual ~IDetour() {}
	
	virtual const char *GetName() const = 0;
	
	bool Load();
	void Unload();
	
	bool IsLoaded() const { return this->m_bLoaded; }
	bool IsActive() const { return this->m_bActive; }
	
	void Toggle(bool enable);
	void Enable();
	void Disable();
	
protected:
	IDetour() {}
	
	virtual bool DoLoad() = 0;
	virtual void DoUnload() = 0;
	
	virtual void DoEnable() = 0;
	virtual void DoDisable() = 0;
	
private:
	bool m_bLoaded = false;
	bool m_bActive = false;
};


class CDetour : public IDetour
{
public:
	/* by pointer */
	CDetour(const char *name, void *func_ptr, void *callback, void **inner_ptr) :
		m_strName(name), m_pCallback(callback), m_pInner(inner_ptr), m_bFuncByName(false), m_pFunc(func_ptr) {}
	/* by addr name */
	CDetour(const char *name, const char *func_name, void *callback, void **inner_ptr) :
		m_strName(name), m_pCallback(callback), m_pInner(inner_ptr), m_bFuncByName(true), m_strFuncName(func_name) {}
	
	virtual const char *GetName() const override { return this->m_strName.c_str(); }
	
private:
	virtual bool DoLoad() override;
	virtual void DoUnload() override;
	
	virtual void DoEnable() override;
	virtual void DoDisable() override;
	
	bool EnsureUniqueInnerPtrs();
	
	std::string m_strName;
	void *m_pCallback;
	void **m_pInner;
	
	bool m_bFuncByName = false;
	std::string m_strFuncName;
	void *m_pFunc = nullptr;
	
	static std::list<CDetour *> s_LoadedDetours;
	static std::list<CDetour *> s_ActiveDetours;
	
	friend class CDetouredFunc;
};


class IDetourRegexSymbol : public IDetour
{
public:
	virtual const char *GetName() const override;
	
	virtual void *GetFuncPtr() const { return this->m_pFunc; }
	
protected:
	IDetourRegexSymbol(Library lib, const char *pattern) :
		m_Library(lib), m_strPattern(pattern) {}
	
private:
	virtual bool DoLoad() override;
	virtual void DoUnload() override;
	
	void Demangle();
	
	Library m_Library;
	std::string m_strPattern;
	
	std::string m_strSymbol;
	std::string m_strDemangled;
	void *m_pFunc = nullptr;
};


class IDetourTrace : public IDetourRegexSymbol
{
public:
	virtual void DoEnable() override;
	virtual void DoDisable() override;
	
	virtual void TracePre() = 0;
	virtual void TracePost() = 0;
	
protected:
	IDetourTrace(Library lib, const char *pattern) :
		IDetourRegexSymbol(lib, pattern) {}
};


class CFuncTrace : public IDetourTrace
{
public:
	CFuncTrace(Library lib, const char *pattern) :
		IDetourTrace(lib, pattern) {}
	
	virtual void TracePre() override;
	virtual void TracePost() override;
};


class CFuncBacktrace : public IDetourTrace
{
public:
	CFuncBacktrace(Library lib, const char *pattern) :
		IDetourTrace(lib, pattern) {}
	
	virtual void TracePre() override;
	virtual void TracePost() override;
};


class CDetouredFunc
{
public:
	CDetouredFunc(void *func_ptr);
	~CDetouredFunc();
	
	static CDetouredFunc& Find(void *func_ptr);
	
	void AddDetour(CDetour *detour);
	void RemoveDetour(CDetour *detour);
	
	void AddTrace(IDetourTrace *trace);
	void RemoveTrace(IDetourTrace *trace);
	
private:
	void RemoveAllDetours();
	
	void CreateWrapper();
	void DestroyWrapper();
	
	void CreateTrampoline();
	void DestroyTrampoline();
	
	void StorePrologue();
	bool IsPrologueValid() { return !this->m_Prologue.empty(); }
	
	void Reconfigure();
	
	void InstallJump(void *target);
	void UninstallJump();
	
	void FuncPre();
	void FuncPost();
	
	void *m_pFunc;
	
	std::vector<CDetour *> m_Detours;
	std::vector<IDetourTrace *> m_Traces;
	
	std::vector<uint8_t> m_Prologue;
	
	void *m_pWrapper    = nullptr;
	void *m_pTrampoline = nullptr;
	
	void *m_pWrapperPre   = reinterpret_cast<void *>(&WrapperPre);
	void *m_pWrapperPost  = reinterpret_cast<void *>(&WrapperPost);
	void *m_pWrapperInner = nullptr;
	
	static void WrapperPre(void *func_ptr, uint32_t *eip);
	static void WrapperPost(void *func_ptr, uint32_t *eip);
	/* TODO: should be thread_local, but our statically linked libstdc++ thinks
	 * it has recent glibc, and it may not, which breaks
	 * __cxa_thread_atexit_impl */
	static /*thread_local*/ std::vector<uint32_t> s_SaveEIP;
	
	static std::map<void *, CDetouredFunc> s_FuncMap;
};


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

#define GET_MEMBER_CALLBACK(name) GetAddrOfMemberFunc(&Detour_##name::name)
#define GET_MEMBER_INNERPTR(name) reinterpret_cast<void **>(&Detour_##name::name##_Actual)

#define GET_STATIC_CALLBACK(name) reinterpret_cast<void *>(&Detour_##name)
#define GET_STATIC_INNERPTR(name) reinterpret_cast<void **>(&Actual_##name)


// TODO: have an "exclusive" version of CDetour, which is identical, but when it's enabled/disabled,
// make it ensure that it's the only active detour for that function
// (also check whenever trying to enable/disable other detours at that func)


// PHASE 3:
// implement the whole set of pseudo-detour fixtures


// there will be one "master detour" that is actually called directly
// it will call into the chain of regular detours
// before and after that, it will do lightweight pre- and post- tasks


// IDetour: anything that is detour-ish, which a mod can enable/disable

// CDetour: regular detour
// lowest priority

// RefCount thing:
// increments/decrements RefCount when the master detour is active

// pre-notify:
// read-only detour variant that just informs you of the parameters before the call

// post-notify:
// read-only detour variant that just informs you of the parameters/retval after the call

// pre-hook:
// just a callback that happens before the call, no info given

// post-hook:
// just a callback that happens after the call, no info given


#endif
