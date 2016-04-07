#ifndef _INCLUDE_SIGSEGV_MOD_H_
#define _INCLUDE_SIGSEGV_MOD_H_


#include "mem/patch.h"
#include "mem/detour.h"


class IHasPatches
{
public:
	virtual ~IHasPatches() {}
	
	virtual const char *GetName() const = 0;
	
	bool LoadPatches();
	void UnloadPatches();
	
	void AddPatch(IPatch *patch);
	
	void ToggleAllPatches(bool enable);
	void EnableAllPatches()  { this->ToggleAllPatches(true); }
	void DisableAllPatches() { this->ToggleAllPatches(false); }
	
	size_t GetNumPatches() const     { return this->m_Patches.size(); }
	std::vector<IPatch *>& Patches() { return this->m_Patches; }
	
protected:
	IHasPatches() {}
	
	virtual bool CanAddPatches() const = 0;
	virtual bool CanTogglePatches() const = 0;
	
private:
	std::vector<IPatch *> m_Patches;
};


class IHasDetours
{
public:
	virtual ~IHasDetours() {}
	
	virtual const char *GetName() const = 0;
	
	bool LoadDetours();
	void UnloadDetours();
	
	void AddDetour(IDetour *detour);
	
	void ToggleDetour(const char *name, bool enable);
	void EnableDetour(const char *name)  { this->ToggleDetour(name, true); }
	void DisableDetour(const char *name) { this->ToggleDetour(name, false); }
	
	void ToggleAllDetours(bool enable);
	void EnableAllDetours()  { this->ToggleAllDetours(true); }
	void DisableAllDetours() { this->ToggleAllDetours(false); }
	
	size_t GetNumDetours() const                 { return this->m_Detours.size(); }
	std::map<const char *, IDetour *>& Detours() { return this->m_Detours; }
	
protected:
	IHasDetours() {}
	
	virtual bool CanAddDetours() const = 0;
	virtual bool CanToggleDetours() const = 0;
	
private:
	std::map<const char *, IDetour *> m_Detours;
};


class IMod : public AutoList<IMod>, public IHasPatches, public IHasDetours
{
public:
	virtual ~IMod() {}
	
	virtual const char *GetName() const override final { return this->m_pszName; }
	
	virtual bool OnLoad()   { return true; }
	virtual void OnUnload() {}
	
protected:
	IMod(const char *name) :
		m_pszName(name) {}
	
private:
	void InvokeLoad();
	void InvokeUnload();
	
	virtual bool CanAddPatches() const override    { return !this->m_bLoaded; }
	virtual bool CanTogglePatches() const override { return !this->m_bFailed; }
	
	virtual bool CanAddDetours() const override    { return !this->m_bLoaded; }
	virtual bool CanToggleDetours() const override { return !this->m_bFailed; }
	
	const char *m_pszName;
	
	bool m_bFailed = false;
	bool m_bLoaded = false;
	
	friend class CModManager;
};


class CModManager : public CBaseGameSystemPerFrame
{
public:
	void Load();
	void Unload();
	
	virtual void LevelInitPreEntity() override;
	virtual void LevelInitPostEntity() override;
	virtual void LevelShutdownPreEntity() override;
	virtual void LevelShutdownPostEntity() override;
	virtual void FrameUpdatePreEntityThink() override;
	virtual void FrameUpdatePostEntityThink() override;
	
	static void CC_ListMods(const CCommand& cmd);
};
extern CModManager g_ModManager;


class IFrameUpdateListener : public AutoList<IFrameUpdateListener>
{
public:
	virtual bool ShouldReceiveFrameEvents() const = 0;
	
	virtual void FrameUpdatePreEntityThink() {}
	virtual void FrameUpdatePostEntityThink() {}
	
protected:
	IFrameUpdateListener() {}
};


#define MOD_ADD_DETOUR_MEMBER(detour, addr) \
	this->AddDetour(new CDetour(addr, GET_MEMBER_CALLBACK(detour), GET_MEMBER_INNERPTR(detour)))
#define MOD_ADD_DETOUR_STATIC(detour, addr) \
	this->AddDetour(new CDetour(addr, GET_STATIC_CALLBACK(detour), GET_STATIC_INNERPTR(detour)))


#endif
