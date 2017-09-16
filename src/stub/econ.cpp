#include "stub/econ.h"
#include "addr/addr.h"
#include "stub/strings.h"
#include "util/misc.h"


using const_char_ptr = const char *;

#if defined _LINUX

static constexpr uint8_t s_Buf_perteamvisuals_t_m_Sounds[] = {
	0x8b, 0x80, 0x00, 0x00, 0x00, 0x00,       // +0000  mov eax,[eax+m_Visuals]
	0x85, 0xc0,                               // +0006  test eax,eax
	0x74, 0x00,                               // +0008  jz +0x??
	0x83, 0xfb, NUM_SHOOT_SOUND_TYPES,        // +000A  cmp ebx,NUM_SHOOT_SOUND_TYPES
	0x77, 0x00,                               // +000D  ja +0x??
	0x8b, 0x84, 0x98, 0x00, 0x00, 0x00, 0x00, // +000F  mov eax,[eax+ebx*4+m_Sounds]
};

struct CExtract_perteamvisuals_t_m_Sounds : public IExtract<const_char_ptr (*)[NUM_SHOOT_SOUND_TYPES]>
{
	CExtract_perteamvisuals_t_m_Sounds() : IExtract<const_char_ptr (*)[NUM_SHOOT_SOUND_TYPES]>(sizeof(s_Buf_perteamvisuals_t_m_Sounds)) {}
	
	virtual bool GetExtractInfo(ByteBuf& buf, ByteBuf& mask) const override
	{
		buf.CopyFrom(s_Buf_perteamvisuals_t_m_Sounds);
		
		int off_CEconItemDefinition_m_Visuals;
	//	if (!Prop::FindOffset(off_CEconItemDefinition_m_Visuals, "CEconItemDefinition", "m_Visuals")) return false;
		if (!Prop::FindOffset(off_CEconItemDefinition_m_Visuals, "CEconItemDefinition", "m_Visuals")) {
			DevMsg("Extractor for perteamvisuals_t::m_Sounds: can't find prop offset for CEconItemDefinition::m_Visuals\n");
			return false;
		}
		
		buf.SetDword(0x00 + 2, (uint32_t)off_CEconItemDefinition_m_Visuals);
		
		mask[0x08 + 1] = 0x00;
		mask[0x0d + 1] = 0x00;
		mask.SetDword(0x0f + 3, ~0x000003ff);
		
		return true;
	}
	
	virtual const char *GetFuncName() const override   { return "CTFWeaponBase::GetShootSound"; }
	virtual uint32_t GetFuncOffMin() const override    { return 0x0000; }
	virtual uint32_t GetFuncOffMax() const override    { return 0x0100; } // @ +0x0056
	virtual uint32_t GetExtractOffset() const override { return 0x000f + 3; }
};

#elif defined _WINDOWS

// TODO
struct CExtract_perteamvisuals_t_m_Sounds : public IExtract<const_char_ptr (*)[NUM_SHOOT_SOUND_TYPES]>
{
	CExtract_perteamvisuals_t_m_Sounds() : IExtract<const_char_ptr (*)[NUM_SHOOT_SOUND_TYPES]>(0) {}
	
	virtual bool GetExtractInfo(ByteBuf& buf, ByteBuf& mask) const override { return false; }
	
	virtual const char *GetFuncName() const override   { return "CTFWeaponBase::GetShootSound"; }
	virtual uint32_t GetFuncOffMin() const override    { return 0xffffffff; }
	virtual uint32_t GetFuncOffMax() const override    { return 0xffffffff; }
	virtual uint32_t GetExtractOffset() const override { return 0xffffffff; }
};

#endif


using perteamvisuals_t_ptr = perteamvisuals_t *;

#if defined _LINUX

static constexpr uint8_t s_Buf_CEconItemDefinition_m_Visuals[] = {
	0xc7, 0x84, 0x83, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // +0000  mov dword ptr [ebx+eax*4+m_Visuals],0x00000000
	0x8b, 0x04, 0x85, 0x00, 0x00, 0x00, 0x00,                         // +000B  mov eax,g_TeamVisualSections[eax*4]
};

struct CExtract_CEconItemDefinition_m_Visuals : public IExtract<perteamvisuals_t_ptr (*)[NUM_VISUALS_BLOCKS]>
{
	CExtract_CEconItemDefinition_m_Visuals() : IExtract<perteamvisuals_t_ptr (*)[NUM_VISUALS_BLOCKS]>(sizeof(s_Buf_CEconItemDefinition_m_Visuals)) {}
	
	virtual bool GetExtractInfo(ByteBuf& buf, ByteBuf& mask) const override
	{
		buf.CopyFrom(s_Buf_CEconItemDefinition_m_Visuals);
		
		buf.SetDword(0x0b + 3, (uint32_t)&g_TeamVisualSections.GetRef());
		
		mask.SetDword(0x00 + 3, ~0x000003ff);
		
		return true;
	}
	
	virtual const char *GetFuncName() const override   { return "CEconItemDefinition::BInitVisualBlockFromKV"; }
	virtual uint32_t GetFuncOffMin() const override    { return 0x0000; }
	virtual uint32_t GetFuncOffMax() const override    { return 0x0100; } // @ +0x0057
	virtual uint32_t GetExtractOffset() const override { return 0x0000 + 3; }
};

#elif defined _WINDOWS

// TODO
struct CExtract_CEconItemDefinition_m_Visuals : public IExtract<perteamvisuals_t_ptr (*)[NUM_VISUALS_BLOCKS]>
{
	CExtract_CEconItemDefinition_m_Visuals() : IExtract<perteamvisuals_t_ptr (*)[NUM_VISUALS_BLOCKS]>(0) {}
	
	virtual bool GetExtractInfo(ByteBuf& buf, ByteBuf& mask) const override { return false; }
	
	virtual const char *GetFuncName() const override   { return "CEconItemDefinition::BInitVisualBlockFromKV"; }
	virtual uint32_t GetFuncOffMin() const override    { return 0xffffffff; }
	virtual uint32_t GetFuncOffMax() const override    { return 0xffffffff; }
	virtual uint32_t GetExtractOffset() const override { return 0xffffffff; }
};

#endif


#if defined _LINUX

static constexpr uint8_t s_Buf_CEconItemDefinition_EquipRegionMasks[] = {
	0xe8, 0x00, 0x00, 0x00, 0x00,       // +0000  call CEconItemSchema::GetEquipRegionBitMaskByName
	0x09, 0x83, 0x00, 0x00, 0x00, 0x00, // +0005  or [ebx+m_nEquipRegionBitMask],eax
	0x83, 0xc7, 0x01,                   // +000B  add edi,1
	0x09, 0xb3, 0x00, 0x00, 0x00, 0x00, // +000E  or [ebx+m_nEquipRegionMask],esi
};

struct CExtract_CEconItemDefinition_EquipRegionMasks : public IExtract<unsigned int *>
{
	CExtract_CEconItemDefinition_EquipRegionMasks() : IExtract<unsigned int *>(sizeof(s_Buf_CEconItemDefinition_EquipRegionMasks)) {}
	
	virtual bool GetExtractInfo(ByteBuf& buf, ByteBuf& mask) const override
	{
		buf.CopyFrom(s_Buf_CEconItemDefinition_EquipRegionMasks);
		
		mask.SetDword(0x0000 + 1, 0x00000000);
		mask.SetDword(0x0005 + 2, 0x00000000);
		mask.SetDword(0x000e + 2, 0x00000000);
		
		return true;
	}
	
	virtual const char *GetFuncName() const override { return "CEconItemDefinition::BInitFromKV"; }
	virtual uint32_t GetFuncOffMin() const override  { return 0x0000; }
	virtual uint32_t GetFuncOffMax() const override  { return 0x2000; } // @ +0x14a2
};

struct CExtract_CEconItemDefinition_m_nEquipRegionBitMask : public CExtract_CEconItemDefinition_EquipRegionMasks
{
	virtual uint32_t GetExtractOffset() const override { return 0x0005 + 2; }
};

struct CExtract_CEconItemDefinition_m_nEquipRegionMask : public CExtract_CEconItemDefinition_EquipRegionMasks
{
	virtual uint32_t GetExtractOffset() const override { return 0x000e + 2; }
};

#elif defined _WINDOWS

using CExtract_CEconItemDefinition_m_nEquipRegionBitMask = IExtractStub;
using CExtract_CEconItemDefinition_m_nEquipRegionMask    = IExtractStub;

#endif


StaticFuncThunk<int, int, const char *, const CBaseEntity *, CUtlVector<CBaseEntity *> *, bool>     CAttributeManager::ft_AttribHookValue_int  ("CAttributeManager::AttribHookValue<int>");
StaticFuncThunk<float, float, const char *, const CBaseEntity *, CUtlVector<CBaseEntity *> *, bool> CAttributeManager::ft_AttribHookValue_float("CAttributeManager::AttribHookValue<float>");


MemberFuncThunk<CAttributeList *, void, CEconItemAttribute *> CAttributeList::ft_AddAttribute("CAttributeList::AddAttribute");


IMPL_EXTRACT(const char *[NUM_SHOOT_SOUND_TYPES], perteamvisuals_t, m_Sounds, new CExtract_perteamvisuals_t_m_Sounds());


IMPL_EXTRACT(perteamvisuals_t *[NUM_VISUALS_BLOCKS], CEconItemDefinition, m_Visuals,             new CExtract_CEconItemDefinition_m_Visuals());
IMPL_EXTRACT(unsigned int,                           CEconItemDefinition, m_nEquipRegionBitMask, new CExtract_CEconItemDefinition_m_nEquipRegionBitMask());
IMPL_EXTRACT(unsigned int,                           CEconItemDefinition, m_nEquipRegionMask,    new CExtract_CEconItemDefinition_m_nEquipRegionMask());


MemberFuncThunk<const CTFItemDefinition *, int, int> CTFItemDefinition::ft_GetLoadoutSlot("CTFItemDefinition::GetLoadoutSlot");


IMPL_DATAMAP(short,          CEconItemView, m_iItemDefinitionIndex);
IMPL_DATAMAP(int,            CEconItemView, m_iEntityQuality);
IMPL_DATAMAP(int,            CEconItemView, m_iEntityLevel);
IMPL_DATAMAP(int64,          CEconItemView, m_iItemID);
IMPL_DATAMAP(bool,           CEconItemView, m_bInitialized);
IMPL_DATAMAP(CAttributeList, CEconItemView, m_AttributeList);
IMPL_DATAMAP(CAttributeList, CEconItemView, m_NetworkedDynamicAttributesForDemos);
IMPL_DATAMAP(bool,           CEconItemView, m_bOnlyIterateItemViewAttributes);

MemberFuncThunk<      CEconItemView *, void>                              CEconItemView::ft_ctor         ("CEconItemView::CEconItemView [C1]");
MemberFuncThunk<      CEconItemView *, void, int, int, int, unsigned int> CEconItemView::ft_Init         ("CEconItemView::Init");
MemberFuncThunk<const CEconItemView *, CTFItemDefinition *>               CEconItemView::ft_GetStaticData("CEconItemView::GetStaticData");

MemberVFuncThunk<const CEconItemView *, int> CEconItemView::vt_GetItemDefIndex(TypeName<CEconItemView>(), "CEconItemView::GetItemDefIndex");


IMPL_SENDPROP(CEconItemView, CAttributeContainer, m_Item, CEconEntity);


MemberFuncThunk<      CEconItemAttribute *, void>                           CEconItemAttribute::ft_ctor         ("CEconItemAttribute::CEconItemAttribute [C1]");
MemberFuncThunk<const CEconItemAttribute *, CEconItemAttributeDefinition *> CEconItemAttribute::ft_GetStaticData("CEconItemAttribute::GetStaticData");


MemberFuncThunk<const CEconItemSchema *, CEconItemDefinition *, int>                   CEconItemSchema::ft_GetItemDefinition           ("CEconItemSchema::GetItemDefinition");
MemberFuncThunk<const CEconItemSchema *, CEconItemDefinition *, const char *>          CEconItemSchema::ft_GetItemDefinitionByName     ("CEconItemSchema::GetItemDefinitionByName");
MemberFuncThunk<const CEconItemSchema *, CEconItemAttributeDefinition *, int>          CEconItemSchema::ft_GetAttributeDefinition      ("CEconItemSchema::GetAttributeDefinition");
MemberFuncThunk<const CEconItemSchema *, CEconItemAttributeDefinition *, const char *> CEconItemSchema::ft_GetAttributeDefinitionByName("CEconItemSchema::GetAttributeDefinitionByName");


static StaticFuncThunk<CTFItemSchema *> ft_GetItemSchema("GetItemSchema");
CTFItemSchema *GetItemSchema() { return ft_GetItemSchema(); }

static StaticFuncThunk<CItemGeneration *> ft_ItemGeneration("ItemGeneration");
CItemGeneration *ItemGeneration() { return ft_ItemGeneration(); }


GlobalThunk<const char *[NUM_VISUALS_BLOCKS]> g_TeamVisualSections("g_TeamVisualSections");


int GetNumberOfLoadoutSlots()
{
	static int iNumLoadoutSlots = []{
	//	ConColorMsg(Color(0xff, 0x00, 0xff, 0xff), "GetNumberOfLoadoutSlots: in lambda\n");
		
		const SegInfo& info_seg_server_rodata = LibMgr::GetInfo(Library::SERVER).GetSeg(Segment::RODATA);
		
		constexpr char prefix[] = "#LoadoutSlot_";
		
		for (int i = 0; i < 64; ++i) {
			const char *str = g_szLoadoutStringsForDisplay[i];
			
			if (str == nullptr || !info_seg_server_rodata.ContainsAddr(str, 1) || strncmp(str, prefix, strlen(prefix)) != 0) {
				return i;
			}
		}
		
		assert(false);
		return 0;
	}();
	
	return iNumLoadoutSlots;
}


bool IsValidLoadoutSlotNumber(int num)
{
	return (num >= 0 && num < GetNumberOfLoadoutSlots());
}

int ClampLoadoutSlotNumber(int num)
{
	return Clamp(num, 0, GetNumberOfLoadoutSlots() - 1);
}


static const char **GetLoadoutSlotNameArray()
{
	auto array = reinterpret_cast<const char **>(AddrManager::GetAddr("CTFPlayer::ModifyOrAppendCriteria::kSlotCriteriaName"));
	assert(array != nullptr);
	
	return array;
}

const char *GetLoadoutSlotName(int slot)
{
	if (!IsValidLoadoutSlotNumber(slot)) {
		return nullptr;
	}
	
	static auto array = GetLoadoutSlotNameArray();
	return (array[slot] + strlen("loadout_slot_"));
}

int GetLoadoutSlotFromName(const char *name)
{
	static auto array = GetLoadoutSlotNameArray();
	
	int slot_count = GetNumberOfLoadoutSlots();
	for (int i = 0; i < slot_count; ++i) {
		if (FStrEq((array[i] + strlen("loadout_slot_")), name)) {
			return i;
		}
	}
	
	return -1;
}
