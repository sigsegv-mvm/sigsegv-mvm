#include "stub/entities.h"


#if defined _LINUX

static constexpr uint8_t s_Buf_CCurrencyPack_m_nAmount[] = {
	0x55,                               // +0000  push ebp
	0x89, 0xe5,                         // +0001  mov ebp,esp
	0xf3, 0x0f, 0x2c, 0x55, 0x0c,       // +0003  cvttss2si edx,[ebp+0xc]
	0x8b, 0x45, 0x08,                   // +0008  mov eax,[ebp+this]
	0x89, 0x90, 0xf0, 0x04, 0x00, 0x00, // +000B  mov [eax+0xVVVVVVVV],edx
	0x5d,                               // +0011  pop ebp
	0xc3,                               // +0012  ret
};

struct CExtract_CCurrencyPack_m_nAmount : public IExtract<int *>
{
	CExtract_CCurrencyPack_m_nAmount() : IExtract<int *>(sizeof(s_Buf_CCurrencyPack_m_nAmount)) {}
	
	virtual bool GetExtractInfo(ByteBuf& buf, ByteBuf& mask) const override
	{
		buf.CopyFrom(s_Buf_CCurrencyPack_m_nAmount);
		
		mask.SetRange(0x0b + 2, 4, 0x00);
		
		return true;
	}
	
	virtual const char *GetFuncName() const override   { return "CCurrencyPack::SetAmount"; }
	virtual uint32_t GetFuncOffMin() const override    { return 0x0000; }
	virtual uint32_t GetFuncOffMax() const override    { return 0x0000; }
	virtual uint32_t GetExtractOffset() const override { return 0x000b + 2; }
};

#elif defined _WINDOWS

// TODO

#endif


#if defined _LINUX

static constexpr uint8_t s_Buf_CTFTankBoss_m_pBodyInterface[] = {
	0x55,                               // +0000  push ebp
	0x89, 0xe5,                         // +0001  mov ebp,esp
	0x8b, 0x45, 0x08,                   // +0003  mov eax,[ebp+this]
	0x5d,                               // +0006  pop ebp
	0x8b, 0x80, 0xd0, 0x09, 0x00, 0x00, // +0007  mov eax,[eax+0xVVVVVVVV]
	0xc3,                               // +000D  ret
};

struct CExtract_CTFTankBoss_m_pBodyInterface : public IExtract<IBody **>
{
	CExtract_CTFTankBoss_m_pBodyInterface() : IExtract<IBody **>(sizeof(s_Buf_CTFTankBoss_m_pBodyInterface)) {}
	
	virtual bool GetExtractInfo(ByteBuf& buf, ByteBuf& mask) const override
	{
		buf.CopyFrom(s_Buf_CTFTankBoss_m_pBodyInterface);
		
		mask.SetRange(0x07 + 2, 4, 0x00);
		
		return true;
	}
	
	virtual const char *GetFuncName() const override   { return "CTFTankBoss::GetBodyInterface"; }
	virtual uint32_t GetFuncOffMin() const override    { return 0x0000; }
	virtual uint32_t GetFuncOffMax() const override    { return 0x0000; }
	virtual uint32_t GetExtractOffset() const override { return 0x0007 + 2; }
};

#elif defined _WINDOWS

// TODO

#endif


#if defined _LINUX

static constexpr uint8_t s_Buf_CTeamControlPointMaster_m_ControlPoints[] = {
	0x55,                                     // +0000  push ebp
	0x89, 0xe5,                               // +0001  mov ebp,esp
	0x56,                                     // +0003  push esi
	0x8b, 0x45, 0x08,                         // +0004  mov eax,[ebp+this]
	0x53,                                     // +0007  push ebx
	0x8b, 0x75, 0x0c,                         // +0008  mov esi,[ebp+0xc]
	0x0f, 0xb7, 0x98, 0x7a, 0x03, 0x00, 0x00, // +000B  movzx ebx,word ptr [eax+0xVVVVVVVV]
};

struct CExtract_CTeamControlPointMaster_m_ControlPoints : public IExtract<CUtlMap<int, CTeamControlPoint *> *>
{
	using T = CUtlMap<int, CTeamControlPoint *> *;
	
	CExtract_CTeamControlPointMaster_m_ControlPoints() : IExtract<T>(sizeof(s_Buf_CTeamControlPointMaster_m_ControlPoints)) {}
	
	virtual bool GetExtractInfo(ByteBuf& buf, ByteBuf& mask) const override
	{
		buf.CopyFrom(s_Buf_CTeamControlPointMaster_m_ControlPoints);
		
		mask.SetRange(0x0b + 3, 4, 0x00);
		
		return true;
	}
	
	virtual const char *GetFuncName() const override   { return "CTeamControlPointMaster::PointLastContestedAt"; }
	virtual uint32_t GetFuncOffMin() const override    { return 0x0000; }
	virtual uint32_t GetFuncOffMax() const override    { return 0x0000; }
	virtual uint32_t GetExtractOffset() const override { return 0x000b + 3; }
	virtual T AdjustValue(T val) const override        { return reinterpret_cast<T>((uintptr_t)val - 0x12); }
};

#elif defined _WINDOWS

// TODO

#endif


MemberFuncThunk<CPathTrack *, CPathTrack *> CPathTrack::ft_GetNext("CPathTrack::GetNext");


MemberVFuncThunk<CTFPowerup *, float> CTFPowerup::vt_GetLifeTime(TypeName<CTFPowerup>(), "CTFPowerup::GetLifeTime");


IMPL_DATAMAP(int, CSpellPickup, m_nTier);


IMPL_SENDPROP(CHandle<CBaseEntity>, CTFReviveMarker, m_hOwner,   CTFReviveMarker);
IMPL_SENDPROP(short,                CTFReviveMarker, m_nRevives, CTFReviveMarker);


IMPL_SENDPROP(bool, CTFBotHintEngineerNest, m_bHasActiveTeleporter, CTFBotHintEngineerNest);


GlobalThunk<CUtlVector<ITFBotHintEntityAutoList *>> ITFBotHintEntityAutoList::m_ITFBotHintEntityAutoListAutoList("ITFBotHintEntityAutoList::m_ITFBotHintEntityAutoListAutoList");


IMPL_SENDPROP(bool, CCaptureFlag, m_bDisabled,   CCaptureFlag);
IMPL_SENDPROP(int,  CCaptureFlag, m_nFlagStatus, CCaptureFlag);


GlobalThunk<CUtlVector<ICaptureFlagAutoList *>> ICaptureFlagAutoList::m_ICaptureFlagAutoListAutoList("ICaptureFlagAutoList::m_ICaptureFlagAutoListAutoList");


IMPL_DATAMAP(bool, CBaseTrigger, m_bDisabled);


MemberFuncThunk<const CUpgrades *, const char *, int> CUpgrades::ft_GetUpgradeAttributeName("CUpgrades::GetUpgradeAttributeName");
GlobalThunk<CHandle<CUpgrades>> g_hUpgradeEntity("g_hUpgradeEntity");


IMPL_DATAMAP(int,      CFuncNavPrerequisite, m_task);
IMPL_DATAMAP(string_t, CFuncNavPrerequisite, m_taskEntityName);
IMPL_DATAMAP(float,    CFuncNavPrerequisite, m_taskValue);
IMPL_DATAMAP(bool,     CFuncNavPrerequisite, m_isDisabled);


IMPL_REL_BEFORE(CUtlStringList, CFilterTFBotHasTag, m_TagList, m_iszTags);
IMPL_DATAMAP   (string_t,       CFilterTFBotHasTag, m_iszTags);
IMPL_DATAMAP   (bool,           CFilterTFBotHasTag, m_bRequireAllTags);
IMPL_DATAMAP   (bool,           CFilterTFBotHasTag, m_bNegated);


IMPL_SENDPROP(bool, CCurrencyPack, m_bDistributed, CCurrencyPack);
IMPL_EXTRACT (int,  CCurrencyPack, m_nAmount, new CExtract_CCurrencyPack_m_nAmount());


GlobalThunk<CUtlVector<ICurrencyPackAutoList *>> ICurrencyPackAutoList::m_ICurrencyPackAutoListAutoList("ICurrencyPackAutoList::m_ICurrencyPackAutoListAutoList");


MemberVFuncThunk<CTFBaseBoss *, void> CTFBaseBoss::vt_UpdateCollisionBounds(TypeName<CTFBaseBoss>(), "CTFBaseBoss::UpdateCollisionBounds");


IMPL_EXTRACT (IBody *,             CTFTankBoss, m_pBodyInterface,  new CExtract_CTFTankBoss_m_pBodyInterface());
IMPL_RELATIVE(CHandle<CPathTrack>, CTFTankBoss, m_hCurrentNode,    m_pBodyInterface, 0x0c);
IMPL_RELATIVE(CUtlVector<float>,   CTFTankBoss, m_NodeDists,       m_pBodyInterface, 0x10);
IMPL_RELATIVE(float,               CTFTankBoss, m_flTotalDistance, m_pBodyInterface, 0x24);
IMPL_RELATIVE(int,                 CTFTankBoss, m_iCurrentNode,    m_pBodyInterface, 0x28);
IMPL_RELATIVE(int,                 CTFTankBoss, m_iModelIndex,     m_pBodyInterface, 0x44);


GlobalThunk<CUtlVector<ICaptureZoneAutoList *>> ICaptureZoneAutoList::m_ICaptureZoneAutoListAutoList("ICaptureZoneAutoList::m_ICaptureZoneAutoListAutoList");


IMPL_EXTRACT(CTeamControlPointMaster::ControlPointMap, CTeamControlPointMaster, m_ControlPoints, new CExtract_CTeamControlPointMaster_m_ControlPoints());

GlobalThunk<CUtlVector<CHandle<CTeamControlPointMaster>>> g_hControlPointMasters("g_hControlPointMasters");


GlobalThunk<CUtlVector<ITFFlameEntityAutoList *>> ITFFlameEntityAutoList::m_ITFFlameEntityAutoListAutoList("ITFFlameEntityAutoList::m_ITFFlameEntityAutoListAutoList");


GlobalThunk<const char *[4]> s_TankModel    ("s_TankModel");
GlobalThunk<const char *[4]> s_TankModelRome("s_TankModelRome");
