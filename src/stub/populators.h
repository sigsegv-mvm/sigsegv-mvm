#ifndef _INCLUDE_SIGSEGV_STUB_POPULATORS_H_
#define _INCLUDE_SIGSEGV_STUB_POPULATORS_H_


#include "link/link.h"
#include "stub/tfbot.h"


class CPopulationManager;
class IPopulationSpawner;


class IPopulator
{
public:
	void **vtable;
	IPopulationSpawner *m_Spawner;
	CPopulationManager *m_PopMgr;
};

class CRandomPlacementPopulator : public IPopulator {};
class CPeriodicSpawnPopulator : public IPopulator {};
class CWaveSpawnPopulator : public IPopulator {};

class CWave : public IPopulator
{
public:
	void AddClassType(string_t icon, int count, unsigned int flags) { ft_AddClassType(this, icon, count, flags); }
	
	CUtlVector<CWaveSpawnPopulator *> m_WaveSpawns;
	
private:
	static MemberFuncThunk<CWave *, void, string_t, int, unsigned int> ft_AddClassType;
};

class CMissionPopulator : public IPopulator
{
public:
	bool UpdateMission(int mtype) { return ft_UpdateMission(this, mtype); }
	
	int m_Objective;
	
private:
	static MemberFuncThunk<CMissionPopulator *, bool, int> ft_UpdateMission;
};


class IPopulationSpawner
{
public:
	string_t GetClassIcon(int index)                         { return vt_GetClassIcon(this, index); }
	bool IsMiniBoss(int index)                               { return vt_IsMiniBoss  (this, index); }
	bool HasAttribute(CTFBot::AttributeType attr, int index) { return vt_HasAttribute(this, attr, index); }
	
	void **vtable;
	IPopulator *m_Populator;
	
private:
	static MemberVFuncThunk<IPopulationSpawner *, string_t, int                   > vt_GetClassIcon;
	static MemberVFuncThunk<IPopulationSpawner *, bool, int                       > vt_IsMiniBoss;
	static MemberVFuncThunk<IPopulationSpawner *, bool, CTFBot::AttributeType, int> vt_HasAttribute;
};

class CMobSpawner : public IPopulationSpawner
{
public:
	int m_iCount;
	IPopulationSpawner *m_SubSpawner;
};

class CSentryGunSpawner : public IPopulationSpawner {};
class CTankSpawner : public IPopulationSpawner {};
class CTFBotSpawner : public IPopulationSpawner {};

class CSquadSpawner : public IPopulationSpawner
{
public:
	CUtlVector<IPopulationSpawner *> m_SubSpawners;
};

class CRandomChoiceSpawner : public IPopulationSpawner
{
public:
	CUtlVector<IPopulationSpawner *> m_SubSpawners;
	CUtlVector<int> m_Indexes;
};


#endif
