#include "stub/populators.h"


#if defined _LINUX

static constexpr uint8_t s_Buf_CPopulationManager_m_RespecPoints[] = {
	0x55,                               // +0000  push ebp
	0x89, 0xe5,                         // +0001  mov ebp,esp
	0x83, 0xec, 0x58,                   // +0003  sub esp,0x58
	0x89, 0x5d, 0xf4,                   // +0006  mov [ebp-0xc],ebx
	0x8b, 0x5d, 0x08,                   // +0009  mov ebx,[ebp+this]
	0x89, 0x75, 0xf8,                   // +000C  mov [ebp-0x8],esi
	0x89, 0x7d, 0xfc,                   // +000F  mov [ebp-0x4],edi
	0x8d, 0x83, 0xe0, 0x06, 0x00, 0x00, // +0012  lea eax,[ebx+0xVVVVVVVV]
	0x89, 0x04, 0x24,                   // +0018  mov [esp],eax
	0xe8, 0x20, 0x60, 0x00, 0x00,       // +001B  call CUtlRBTree<...>::RemoveAll
};

struct CExtract_CPopulationManager_m_RespecPoints : public IExtract<CPopulationManager::SteamIDMap *>
{
	using T = CPopulationManager::SteamIDMap *;
	
	CExtract_CPopulationManager_m_RespecPoints() : IExtract<T>(sizeof(s_Buf_CPopulationManager_m_RespecPoints)) {}
	
	virtual bool GetExtractInfo(ByteBuf& buf, ByteBuf& mask) const override
	{
		buf.CopyFrom(s_Buf_CPopulationManager_m_RespecPoints);
		
		mask.SetRange(0x03 + 2, 1, 0x00);
		mask.SetRange(0x12 + 2, 4, 0x00);
		mask.SetRange(0x1b + 1, 4, 0x00);
		
		return true;
	}
	
	virtual const char *GetFuncName() const override   { return "CPopulationManager::ResetRespecPoints"; }
	virtual uint32_t GetFuncOffMin() const override    { return 0x0000; }
	virtual uint32_t GetFuncOffMax() const override    { return 0x0000; }
	virtual uint32_t GetExtractOffset() const override { return 0x0012 + 2; }
	virtual T AdjustValue(T val) const override        { return reinterpret_cast<T>((uintptr_t)val - 0x4); }
};

#elif defined _WINDOWS

// TODO

#endif


MemberFuncThunk<CPopulationManager *, bool, KeyValues *> CPopulationManager::ft_LoadMvMMission("CPopulationManager::LoadMvMMission");
MemberFuncThunk<CPopulationManager *, CWave *>           CPopulationManager::ft_GetCurrentWave("CPopulationManager::GetCurrentWave");

IMPL_EXTRACT(CPopulationManager::SteamIDMap, CPopulationManager, m_RespecPoints, new CExtract_CPopulationManager_m_RespecPoints());


GlobalThunk<CPopulationManager *> g_pPopulationManager("g_pPopulationManager");


MemberFuncThunk<CWave *, void, string_t, int, unsigned int> CWave::ft_AddClassType("CWave::AddClassType");


MemberFuncThunk<CMissionPopulator *, bool, int> CMissionPopulator::ft_UpdateMission("CMissionPopulator::UpdateMission");


MemberVFuncThunk<IPopulationSpawner *, string_t, int                   > IPopulationSpawner::vt_GetClassIcon(TypeName<IPopulationSpawner>(), "IPopulationSpawner::GetClassIcon");
MemberVFuncThunk<IPopulationSpawner *, bool, int                       > IPopulationSpawner::vt_IsMiniBoss(  TypeName<IPopulationSpawner>(), "IPopulationSpawner::IsMiniBoss");
MemberVFuncThunk<IPopulationSpawner *, bool, CTFBot::AttributeType, int> IPopulationSpawner::vt_HasAttribute(TypeName<IPopulationSpawner>(), "IPopulationSpawner::HasAttribute");
