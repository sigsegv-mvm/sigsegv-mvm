#ifndef _INCLUDE_SIGSEGV_MOD_H_
#define _INCLUDE_SIGSEGV_MOD_H_


#include "common.h"
#include "sdk/detours.h"
#include "patch.h"


struct DetourInfo
{
	void *callback;
	void **trampoline;
	
	CDetour *detour;
};


class IMod
{
public:
	virtual const char *GetName() const final { return this->m_pszName; }
	
	virtual bool OnLoad(char *error, size_t maxlen) { return true; }
	virtual void OnUnload() {}
	
protected:
	IMod(const char *name);
	virtual ~IMod();
	
	void AddPatch(IPatch *patch);
	void ToggleAllPatches(bool enable);
	
	void AddDetour(const char *name, void *callback, void **trampoline);
	void ToggleDetour(const char *name, bool enable);
	void ToggleAllDetours(bool enable);
	
private:
	bool Init_CheckPatches(char *error, size_t maxlen);
	bool Init_SetupDetours(char *error, size_t maxlen);
	
	bool InvokeLoad(char *error, size_t maxlen);
	void InvokeUnload();
	
	const char *m_pszName;
	
	std::vector<IPatch *> m_Patches;
	std::map<const char *, DetourInfo> m_Detours;
	
	friend class CModManager;
};


#define MOD_ADD_DETOUR_MEMBER(obj, func) \
	this->AddDetour(#obj "::" #func, GET_MEMBER_CALLBACK(obj##_##func), GET_MEMBER_TRAMPOLINE(obj##_##func))


#endif
