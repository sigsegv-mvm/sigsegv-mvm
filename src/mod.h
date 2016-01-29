#ifndef _INCLUDE_SIGSEGV_MOD_H_
#define _INCLUDE_SIGSEGV_MOD_H_


#include "sm/detours.h"
#include "mem/patch.h"


class CModManager
{
public:
	static void LoadAllMods();
	static void UnloadAllMods();
	
	static void CC_ListMods(const CCommand& cmd);
};


struct DetourInfo
{
	void *callback;
	void **trampoline;
	
	CDetour *detour;
};


class IMod : public AutoList<IMod>
{
public:
	virtual const char *GetName() const final { return this->m_pszName; }
	
	virtual bool OnLoad()   { return true; }
	virtual void OnUnload() {}
	
protected:
	IMod(const char *name) :
		m_pszName(name) {}
	virtual ~IMod() {}
	
	void AddPatch(IPatch *patch);
	void ToggleAllPatches(bool enable);
	
	void AddDetour(const char *name, void *callback, void **trampoline);
	void ToggleDetour(const char *name, bool enable);
	void ToggleAllDetours(bool enable);
	
private:
	void InvokeLoad();
	void InvokeUnload();
	
	bool Init_CheckPatches();
	bool Init_SetupDetours();
	
	const char *m_pszName;
	
	bool m_bFailed = false;
	bool m_bLoaded = false;
	
	std::vector<IPatch *> m_Patches;
	std::map<const char *, DetourInfo> m_Detours;
	
	friend class CModManager;
};


#define MOD_ADD_DETOUR_MEMBER(obj, func) \
	this->AddDetour(#obj "::" #func, GET_MEMBER_CALLBACK(obj##_##func), GET_MEMBER_TRAMPOLINE(obj##_##func))
#define MOD_ADD_DETOUR_STATIC(obj, func) \
	this->AddDetour(#obj "::" #func, GET_STATIC_CALLBACK(obj##_##func), GET_STATIC_TRAMPOLINE(obj##_##func))
#define MOD_ADD_DETOUR_GLOBAL(func) \
	this->AddDetour(#func, GET_STATIC_CALLBACK(func), GET_STATIC_TRAMPOLINE(func))


#endif
