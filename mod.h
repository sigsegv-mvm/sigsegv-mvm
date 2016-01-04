#ifndef _INCLUDE_SIGSEGV_MOD_H_
#define _INCLUDE_SIGSEGV_MOD_H_


class IMod
{
public:
	virtual bool OnLoad() = 0;
	virtual void OnUnload() = 0;
	
protected:
	IMod();
	virtual ~IMod();
};


extern std::set<IMod *> g_Mods;
#define FOR_EACH_MOD for (auto mod : g_Mods)


inline IMod::IMod()
{
	assert(g_Mods.find(this) == g_Mods.end());
	g_Mods.insert(this);
}

inline IMod::~IMod()
{
	g_Mods.erase(this);
}


#endif
