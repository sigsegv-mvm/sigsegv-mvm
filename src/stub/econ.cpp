#include "stub/econ.h"


#if defined _LINUX

using const_char_ptr = const char *;

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

#endif


#if defined _LINUX

using perteamvisuals_t_ptr = perteamvisuals_t *;

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

#endif


IMPL_EXTRACT(const char *[NUM_SHOOT_SOUND_TYPES], perteamvisuals_t, m_Sounds, new CExtract_perteamvisuals_t_m_Sounds());


IMPL_EXTRACT(perteamvisuals_t *[NUM_VISUALS_BLOCKS], CEconItemDefinition, m_Visuals, new CExtract_CEconItemDefinition_m_Visuals());


MemberVFuncThunk<const CEconItemView *, int> CEconItemView::vt_GetItemDefIndex(TypeName<CEconItemView>(), "CEconItemView::GetItemDefIndex");


StaticFuncThunk<int, int, const char *, const CBaseEntity *, CUtlVector<CBaseEntity *> *, bool>     CAttributeManager::ft_AttribHookValue_int  ("CAttributeManager::AttribHookValue<int>");
StaticFuncThunk<float, float, const char *, const CBaseEntity *, CUtlVector<CBaseEntity *> *, bool> CAttributeManager::ft_AttribHookValue_float("CAttributeManager::AttribHookValue<float>");


IMPL_SENDPROP(CEconItemView, CAttributeContainer, m_Item, CEconEntity);


MemberFuncThunk<const CEconItemSchema *, CEconItemDefinition *, int>                   CEconItemSchema::ft_GetItemDefinition           ("CEconItemSchema::GetItemDefinition");
MemberFuncThunk<      CEconItemSchema *, CEconItemDefinition *, const char *>          CEconItemSchema::ft_GetItemDefinitionByName     ("CEconItemSchema::GetItemDefinitionByName");
MemberFuncThunk<      CEconItemSchema *, CEconItemAttributeDefinition *, const char *> CEconItemSchema::ft_GetAttributeDefinitionByName("CEconItemSchema::GetAttributeDefinitionByName");


static StaticFuncThunk<CTFItemSchema *> ft_GetItemSchema("GetItemSchema");
CTFItemSchema *GetItemSchema() { return ft_GetItemSchema(); }

GlobalThunk<const char *[NUM_VISUALS_BLOCKS]> g_TeamVisualSections("g_TeamVisualSections");


CTFItemDefinition *CEconItemView::GetStaticData() const
{
	return rtti_cast<CTFItemDefinition *>(GetItemSchema()->GetItemDefinition(this->GetItemDefIndex()));
}
