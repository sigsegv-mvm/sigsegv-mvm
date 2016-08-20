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

/* this is really an ugly ugly thing, but I don't have the patience to do it
 * in a more reliable way at the moment; plus I doubt the TF team has touched
 * CTFTankBoss a single time since Two Cities, so we're safe anyway */

static constexpr uint8_t s_Buf_CTFTankBoss_m_pBodyInterface[] = {
	0x55,                               // +0000  push ebp
	0x89, 0xe5,                         // +0001  mov ebp,esp
	0x8b, 0x45, 0x08,                   // +0003  mov eax,[ebp+this]
	0x5d,                               // +0006  pop ebp
	0x8b, 0x80, 0xd0, 0x09, 0x00, 0x00, // +0007  mov eax,[eax+0xVVVVVVVV]
	0xc3,                               // +000D  ret
};

template<typename T, ptrdiff_t ADJ>
struct IExtract_CTFTankBoss_m_pBodyInterface : public IExtract<T>
{
	IExtract_CTFTankBoss_m_pBodyInterface() : IExtract<T>(sizeof(s_Buf_CTFTankBoss_m_pBodyInterface)) {}
	
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
	virtual T AdjustValue(T val) const override        { return reinterpret_cast<T>((uintptr_t)val + ADJ); }
};

struct CExtract_CTFTankBoss_m_hCurrentNode :
	public IExtract_CTFTankBoss_m_pBodyInterface<CHandle<CPathTrack> *, 0x000c> {};

struct CExtract_CTFTankBoss_m_NodeDists :
	public IExtract_CTFTankBoss_m_pBodyInterface<CUtlVector<float> *, 0x0010> {};

struct CExtract_CTFTankBoss_m_flTotalDistance :
	public IExtract_CTFTankBoss_m_pBodyInterface<float *, 0x0024> {};

struct CExtract_CTFTankBoss_m_iCurrentNode :
	public IExtract_CTFTankBoss_m_pBodyInterface<int *, 0x0028> {};

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


IMPL_SENDPROP(bool, CCurrencyPack, m_bDistributed, CCurrencyPack);
IMPL_EXTRACT (int,  CCurrencyPack, m_nAmount, new CExtract_CCurrencyPack_m_nAmount());


GlobalThunk<CUtlVector<ICurrencyPackAutoList *>> ICurrencyPackAutoList::m_ICurrencyPackAutoListAutoList("ICurrencyPackAutoList::m_ICurrencyPackAutoListAutoList");


MemberVFuncThunk<CTFBaseBoss *, void> CTFBaseBoss::vt_UpdateCollisionBounds(TypeName<CTFBaseBoss>(), "CTFBaseBoss::UpdateCollisionBounds");


IMPL_EXTRACT(CHandle<CPathTrack>, CTFTankBoss, m_hCurrentNode,    new CExtract_CTFTankBoss_m_hCurrentNode());
IMPL_EXTRACT(CUtlVector<float>,   CTFTankBoss, m_NodeDists,       new CExtract_CTFTankBoss_m_NodeDists());
IMPL_EXTRACT(float,               CTFTankBoss, m_flTotalDistance, new CExtract_CTFTankBoss_m_flTotalDistance());
IMPL_EXTRACT(int,                 CTFTankBoss, m_iCurrentNode,    new CExtract_CTFTankBoss_m_iCurrentNode());


MemberFuncThunk<const CFuncNavCost *, bool, const char *> CFuncNavCost::ft_HasTag("CFuncNavCost::HasTag");


GlobalThunk<CUtlVector<ICaptureZoneAutoList *>> ICaptureZoneAutoList::m_ICaptureZoneAutoListAutoList("ICaptureZoneAutoList::m_ICaptureZoneAutoListAutoList");


IMPL_EXTRACT(CTeamControlPointMaster::ControlPointMap, CTeamControlPointMaster, m_ControlPoints, new CExtract_CTeamControlPointMaster_m_ControlPoints());

GlobalThunk<CUtlVector<CHandle<CTeamControlPointMaster>>> g_hControlPointMasters("g_hControlPointMasters");


GlobalThunk<CUtlVector<ITFFlameEntityAutoList *>> ITFFlameEntityAutoList::m_ITFFlameEntityAutoListAutoList("ITFFlameEntityAutoList::m_ITFFlameEntityAutoListAutoList");
