#ifndef _INCLUDE_SIGSEGV_MOD_H_
#define _INCLUDE_SIGSEGV_MOD_H_


#include "common.h"
#include "detours.h"


class IMod
{
public:
	virtual const char *GetName() const final { return this->m_pszName; }
	
	virtual bool OnLoad(char *error, size_t maxlen) = 0;
	virtual void OnUnload() = 0;
	
protected:
	IMod(const char *name);
	virtual ~IMod();
	
	void AddDetour(const char *name, CDetour *detour);
	void ToggleDetour(const char *name, bool enable);
	void ToggleAllDetours(bool enable);
	
private:
	bool InvokeLoad(char *error, size_t maxlen);
	void InvokeUnload();
	
	const char *m_pszName;
	
	std::map<const char *, CDetour *> m_Detours;
	
	friend class CModManager;
};


#define MOD_ADD_DETOUR_MEMBER(obj, func) \
	{ \
		CDetour *detour = DETOUR_CREATE_MEMBER(obj##_##func, #obj "::" #func); \
		if (detour == nullptr) { \
			snprintf(error, maxlen, "DETOUR_CREATE_MEMBER failed for %s::%s", #obj, #func); \
			return false; \
		} \
		this->AddDetour(#obj "::" #func, detour); \
	}


#endif
