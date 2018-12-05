#include "stub/entities.h"


#if defined _LINUX

static constexpr uint8_t s_Buf_CCurrencyPack_m_nAmount[] = {
	0x55,                               // +0000  push ebp
	0x89, 0xe5,                         // +0001  mov ebp,esp
	0xf3, 0x0f, 0x10, 0x45, 0x0c,       // +0003  movss xmm0,[ebp+0xc]
	0x8b, 0x45, 0x08,                   // +0008  mov eax,[ebp+this]
	0xf3, 0x0f, 0x2c, 0xd0,             // +000B  cvttss2si edx,xmm0
	0x89, 0x90, 0xfc, 0x04, 0x00, 0x00, // +000F  mov [eax+0xVVVVVVVV],edx
	0x5d,                               // +0015  pop ebp
	0xc3,                               // +0016  ret
};

struct CExtract_CCurrencyPack_m_nAmount : public IExtract<int *>
{
	CExtract_CCurrencyPack_m_nAmount() : IExtract<int *>(sizeof(s_Buf_CCurrencyPack_m_nAmount)) {}
	
	virtual bool GetExtractInfo(ByteBuf& buf, ByteBuf& mask) const override
	{
		buf.CopyFrom(s_Buf_CCurrencyPack_m_nAmount);
		
		mask.SetRange(0x0f + 2, 4, 0x00);
		
		return true;
	}
	
	virtual const char *GetFuncName() const override   { return "CCurrencyPack::SetAmount"; }
	virtual uint32_t GetFuncOffMin() const override    { return 0x0000; }
	virtual uint32_t GetFuncOffMax() const override    { return 0x0000; }
	virtual uint32_t GetExtractOffset() const override { return 0x000f + 2; }
};

#elif defined _WINDOWS

using CExtract_CCurrencyPack_m_nAmount = IExtractStub;

#endif


#if defined _LINUX

static constexpr uint8_t s_Buf_CTeamControlPointMaster_m_ControlPoints[] = {
	0x8d, 0xb3, 0x00, 0x00, 0x00, 0x00, // +0000  lea esi,[ebx+m_ControlPoints.m_Elements.m_pMemory]
	0xe8, 0x00, 0x00, 0x00, 0x00,       // +0006  call CUtlMemory<CTeamControlPointRound *,int>::Purge(void)
	0x8d, 0x83, 0x00, 0x00, 0x00, 0x00, // +000B  lea eax,[ebx+m_ControlPoints]
	0x89, 0x04, 0x24,                   // +0011  mov [esp],eax
	0xe8, 0x00, 0x00, 0x00, 0x00,       // +0014  call CUtlRBTree<CUtlMap<int,CTeamControlPoint *,ushort,bool (*)(int const&,int const&)>::Node_t,ushort,CUtlMap<int,CTeamCOntrolPoint *,ushort,bool (*)(int const&,int const&)>::CKeyLess,CUtlMemory<CUtlRBTreeNode_t<CUtlMap<int,CTeamControlPoint *,ushort,bool (*)(int const&,int const&)>::Node_t,ushort>,ushort>>::RemoveAll(void)
	0x89, 0x34, 0x24,                   // +0019  mov esp,[esi]
	0xe8, 0x00, 0x00, 0x00, 0x00,       // +001C  call CUtlMemory<UtlRBTreeNode_t<CUtlMap<int,CTeamControlPoint *,ushort,bool (*)(int const&,int const&)>::Node_t,ushort>,ushort>::Purge(void)
	0x89, 0x34, 0x24,                   // +0021  mov esp,[esi]
	0xe8, 0x00, 0x00, 0x00, 0x00,       // +0024  call CUtlMemory<UtlRBTreeNode_t<CUtlMap<int,CTeamControlPoint *,ushort,bool (*)(int const&,int const&)>::Node_t,ushort>,ushort>::Purge(void)
};

struct CExtract_CTeamControlPointMaster_m_ControlPoints : public IExtract<CTeamControlPointMaster::ControlPointMap *>
{
	using T = CTeamControlPointMaster::ControlPointMap *;
	
	CExtract_CTeamControlPointMaster_m_ControlPoints() : IExtract<T>(sizeof(s_Buf_CTeamControlPointMaster_m_ControlPoints)) {}
	
	virtual bool GetExtractInfo(ByteBuf& buf, ByteBuf& mask) const override
	{
		buf.CopyFrom(s_Buf_CTeamControlPointMaster_m_ControlPoints);
		
		mask.SetRange(0x00 + 2, 2, 0x00);
		mask.SetDword(0x06 + 1,    0x00);
		mask.SetRange(0x0b + 2, 2, 0x00);
		mask.SetDword(0x14 + 1,    0x00);
		mask.SetDword(0x1c + 1,    0x00);
		mask.SetDword(0x24 + 1,    0x00);
		
		return true;
	}
	
	virtual bool Validate(const uint8_t *ptr) const override
	{
		uint32_t off_m_ControlPoints_m_Elements_m_pMemory = *(uint32_t *)(ptr + 0x00 + 2);
		uint32_t off_m_ControlPoints                      = *(uint32_t *)(ptr + 0x0b + 2);
		
		static_assert(offsetof(CTeamControlPointMaster::ControlPointMap,             m_Elements) == 0x04);
		static_assert(offsetof(CTeamControlPointMaster::ControlPointMap::m_Elements, m_pMemory ) == 0x00);
		
		if (off_m_ControlPoints_m_Elements_m_pMemory - 0x04 != off_m_ControlPoints) return false;
		
		return true;
	}
	
	virtual const char *GetFuncName() const override   { return "CTeamControlPointMaster::~CTeamControlPointMaster [D2]"; }
	virtual uint32_t GetFuncOffMin() const override    { return 0x0000; }
	virtual uint32_t GetFuncOffMax() const override    { return 0x0080; }
	virtual uint32_t GetExtractOffset() const override { return 0x000b + 2; }
};

#elif defined _WINDOWS

using CExtract_CTeamControlPointMaster_m_ControlPoints = IExtractStub;

#endif


IMPL_DATAMAP(int, CPathTrack, m_eOrientationType);

MemberFuncThunk<CPathTrack *, CPathTrack *> CPathTrack::ft_GetNext("CPathTrack::GetNext");


IMPL_DATAMAP(float,                CItem, m_flNextResetCheckTime);
IMPL_DATAMAP(bool,                 CItem, m_bActivateWhenAtRest);
IMPL_DATAMAP(Vector,               CItem, m_vOriginalSpawnOrigin);
IMPL_DATAMAP(QAngle,               CItem, m_vOriginalSpawnAngles);
IMPL_DATAMAP(IPhysicsConstraint *, CItem, m_pConstraint);


IMPL_DATAMAP(bool,     CTFPowerup, m_bDisabled);
IMPL_DATAMAP(bool,     CTFPowerup, m_bAutoMaterialize);
IMPL_DATAMAP(string_t, CTFPowerup, m_iszModel);

MemberVFuncThunk<CTFPowerup *, float> CTFPowerup::vt_GetLifeTime(TypeName<CTFPowerup>(), "CTFPowerup::GetLifeTime");


IMPL_DATAMAP(int, CSpellPickup, m_nTier);


IMPL_SENDPROP(CHandle<CBaseEntity>, CTFReviveMarker, m_hOwner,   CTFReviveMarker);
IMPL_SENDPROP(short,                CTFReviveMarker, m_nRevives, CTFReviveMarker);


MemberVFuncThunk<const IHasGenericMeter *, bool>  IHasGenericMeter::vt_ShouldUpdateMeter    (TypeName<IHasGenericMeter>(), "IHasGenericMeter::ShouldUpdateMeter");
MemberVFuncThunk<const IHasGenericMeter *, float> IHasGenericMeter::vt_GetMeterMultiplier   (TypeName<IHasGenericMeter>(), "IHasGenericMeter::GetMeterMultiplier");
MemberVFuncThunk<      IHasGenericMeter *, void>  IHasGenericMeter::vt_OnResourceMeterFilled(TypeName<IHasGenericMeter>(), "IHasGenericMeter::OnResourceMeterFilled");
MemberVFuncThunk<const IHasGenericMeter *, float> IHasGenericMeter::vt_GetChargeInterval    (TypeName<IHasGenericMeter>(), "IHasGenericMeter::GetChargeInterval");


MemberVFuncThunk<CEconWearable *, void, CBaseEntity *> CEconWearable::vt_RemoveFrom(TypeName<CEconWearable>(), "CEconWearable::RemoveFrom");
MemberVFuncThunk<CEconWearable *, void, CBasePlayer *> CEconWearable::vt_UnEquip   (TypeName<CEconWearable>(), "CEconWearable::UnEquip");


IMPL_DATAMAP(int,                  CTFBotHint, m_team);
IMPL_DATAMAP(CTFBotHint::HintType, CTFBotHint, m_hint);
IMPL_DATAMAP(bool,                 CTFBotHint, m_isDisabled);


IMPL_SENDPROP(bool, CTFBotHintEngineerNest, m_bHasActiveTeleporter, CTFBotHintEngineerNest);

MemberFuncThunk<const CTFBotHintEngineerNest *, bool> CTFBotHintEngineerNest::ft_IsStaleNest      ("CTFBotHintEngineerNest::IsStaleNest");
MemberFuncThunk<      CTFBotHintEngineerNest *, void> CTFBotHintEngineerNest::ft_DetonateStaleNest("CTFBotHintEngineerNest::DetonateStaleNest");


GlobalThunk<CUtlVector<ITFBotHintEntityAutoList *>> ITFBotHintEntityAutoList::m_ITFBotHintEntityAutoListAutoList("ITFBotHintEntityAutoList::m_ITFBotHintEntityAutoListAutoList");


MemberVFuncThunk<const CTFItem *, int> CTFItem::vt_GetItemID(TypeName<CTFItem>(), "CTFItem::GetItemID");


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


IMPL_REL_BEFORE(bool, CCurrencyPack, m_bTouched,     m_bPulled);      // 20151007a
IMPL_REL_BEFORE(bool, CCurrencyPack, m_bPulled,      m_bDistributed); // 20151007a
IMPL_SENDPROP  (bool, CCurrencyPack, m_bDistributed, CCurrencyPack);
IMPL_EXTRACT   (int,  CCurrencyPack, m_nAmount, new CExtract_CCurrencyPack_m_nAmount());

MemberVFuncThunk<const CCurrencyPack *, bool> CCurrencyPack::vt_AffectedByRadiusCollection(TypeName<CCurrencyPack>(), "CCurrencyPack::AffectedByRadiusCollection");


GlobalThunk<CUtlVector<ICurrencyPackAutoList *>> ICurrencyPackAutoList::m_ICurrencyPackAutoListAutoList("ICurrencyPackAutoList::m_ICurrencyPackAutoListAutoList");


IMPL_SENDPROP(bool, CCaptureZone, m_bDisabled, CCaptureZone);

MemberFuncThunk<CCaptureZone *, void, CBaseEntity *> CCaptureZone::ft_Capture("CCaptureZone::Capture");


GlobalThunk<CUtlVector<ICaptureZoneAutoList *>> ICaptureZoneAutoList::m_ICaptureZoneAutoListAutoList("ICaptureZoneAutoList::m_ICaptureZoneAutoListAutoList");


static StaticFuncThunk<bool, const CBaseEntity *, const Vector&, bool> ft_PointInRespawnRoom("PointInRespawnRoom");
bool PointInRespawnRoom(const CBaseEntity *ent, const Vector& vec, bool b1) { return ft_PointInRespawnRoom(ent, vec, b1); }


IMPL_EXTRACT(CTeamControlPointMaster::ControlPointMap, CTeamControlPointMaster, m_ControlPoints, new CExtract_CTeamControlPointMaster_m_ControlPoints());

GlobalThunk<CUtlVector<CHandle<CTeamControlPointMaster>>> g_hControlPointMasters("g_hControlPointMasters");


IMPL_DATAMAP(bool,     CTFTeamSpawn, m_bDisabled);
IMPL_DATAMAP(int,      CTFTeamSpawn, m_nSpawnMode);
IMPL_DATAMAP(string_t, CTFTeamSpawn, m_iszControlPointName);
IMPL_DATAMAP(string_t, CTFTeamSpawn, m_iszRoundBlueSpawn);
IMPL_DATAMAP(string_t, CTFTeamSpawn, m_iszRoundRedSpawn);


GlobalThunk<CUtlVector<ITFFlameEntityAutoList *>> ITFFlameEntityAutoList::m_ITFFlameEntityAutoListAutoList("ITFFlameEntityAutoList::m_ITFFlameEntityAutoListAutoList");


IMPL_DATAMAP (string_t, CSmokeStack, m_strMaterialModel);
IMPL_SENDPROP(int,      CSmokeStack, m_iMaterialModel, CSmokeStack);
