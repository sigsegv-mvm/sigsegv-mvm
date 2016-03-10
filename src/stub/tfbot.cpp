#include "stub/tfbot.h"


#if defined _LINUX

static constexpr uint8_t s_Buf_CTFBot_m_nMission[] = {
	0x55,                               // +0000  push ebp
	0x89, 0xe5,                         // +0001  mov ebp,esp
	0x53,                               // +0003  push ebx
	0x83, 0xec, 0x00,                   // +0004  sub esp,0xXX
	0x8b, 0x5d, 0x08,                   // +0007  mov ebx,DWORD PTR [ebp+0x8]
	0x8b, 0x45, 0x0c,                   // +000A  mov eax,DWORD PTR [ebp+0xc]
	0x80, 0x7d, 0x10, 0x00,             // +000D  cmp BYTE PTR [ebp+0x10],0x0
	0x8b, 0x93, 0x00, 0x00, 0x00, 0x00, // +0011  mov edx,DWORD PTR [ebx+0xVVVVVVVV]
};

struct CExtract_CTFBot_m_nMission : public IExtract<CTFBot::MissionType>
{
	CExtract_CTFBot_m_nMission() : IExtract<CTFBot::MissionType>(sizeof(s_Buf_CTFBot_m_nMission)) {}
	
	virtual bool GetExtractInfo(ByteBuf& buf, ByteBuf& mask) const override
	{
		buf.CopyFrom(s_Buf_CTFBot_m_nMission);
		
		mask.SetRange(0x04 + 2, 1, 0x00);
		mask.SetRange(0x11 + 2, 4, 0x00);
		
		return true;
	}
	
	virtual const char *GetFuncName() const override   { return "CTFBot::SetMission"; }
	virtual uint32_t GetFuncOffMin() const override    { return 0x0000; }
	virtual uint32_t GetFuncOffMax() const override    { return 0x0200; }
	virtual uint32_t GetExtractOffset() const override { return 0x0011 + 2; }
};

#elif defined _WINDOWS

static constexpr uint8_t s_Buf_CTFBot_m_nMission[] = {
	0x83, 0xbf, 0x00, 0x00, 0x00, 0x00, 0x02, // +0000  cmp dword ptr [edi+0xVVVVVVVV],0x2
	0x75, 0x00,                               // +0007  jnz 0xXX
	0x68, 0x00, 0x00, 0x00, 0x00,             // +0009  push offset "mission_sentry_buster"
};

struct CExtract_CTFBot_m_nMission : public IExtract<CTFBot::MissionType>
{
	CExtract_CTFBot_m_nMission() : IExtract<CTFBot::MissionType>(sizeof(s_Buf_CTFBot_m_nMission)) {}
	
	virtual bool GetExtractInfo(ByteBuf& buf, ByteBuf& mask) const override
	{
		const char *str = Scan::FindUniqueConstStr(Library::SERVER, "mission_sentry_buster");
		if (str == nullptr) return false;
		
		buf.CopyFrom(s_Buf_CTFBot_m_nMission);
		buf.SetDword(0x09 + 1, (uint32_t)str);
		
		mask.SetRange(0x00 + 2, 4, 0x00);
		mask.SetRange(0x07 + 1, 1, 0x00);
		
		return true;
	}
	
	virtual const char *GetFuncName() const override   { return "CFuncNavCost::IsApplicableTo"; }
	virtual uint32_t GetFuncOffMin() const override    { return 0x0000; }
	virtual uint32_t GetFuncOffMax() const override    { return 0x0200; }
	virtual uint32_t GetExtractOffset() const override { return 0x0000 + 2; }
};

#endif


template<> MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void, float       > NextBotPlayer<CTFPlayer>::vt_PressFireButton(         TypeName<NextBotPlayer<CTFPlayer>>(), "NextBotPlayer<CTFPlayer>::PressFireButton");
template<> MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void              > NextBotPlayer<CTFPlayer>::vt_ReleaseFireButton(       TypeName<NextBotPlayer<CTFPlayer>>(), "NextBotPlayer<CTFPlayer>::ReleaseFireButton");
template<> MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void, float       > NextBotPlayer<CTFPlayer>::vt_PressAltFireButton(      TypeName<NextBotPlayer<CTFPlayer>>(), "NextBotPlayer<CTFPlayer>::PressAltFireButton");
template<> MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void              > NextBotPlayer<CTFPlayer>::vt_ReleaseAltFireButton(    TypeName<NextBotPlayer<CTFPlayer>>(), "NextBotPlayer<CTFPlayer>::ReleaseAltFireButton");
template<> MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void, float       > NextBotPlayer<CTFPlayer>::vt_PressMeleeButton(        TypeName<NextBotPlayer<CTFPlayer>>(), "NextBotPlayer<CTFPlayer>::PressMeleeButton");
template<> MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void              > NextBotPlayer<CTFPlayer>::vt_ReleaseMeleeButton(      TypeName<NextBotPlayer<CTFPlayer>>(), "NextBotPlayer<CTFPlayer>::ReleaseMeleeButton");
template<> MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void, float       > NextBotPlayer<CTFPlayer>::vt_PressSpecialFireButton(  TypeName<NextBotPlayer<CTFPlayer>>(), "NextBotPlayer<CTFPlayer>::PressSpecialFireButton");
template<> MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void              > NextBotPlayer<CTFPlayer>::vt_ReleaseSpecialFireButton(TypeName<NextBotPlayer<CTFPlayer>>(), "NextBotPlayer<CTFPlayer>::ReleaseSpecialFireButton");
template<> MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void, float       > NextBotPlayer<CTFPlayer>::vt_PressUseButton(          TypeName<NextBotPlayer<CTFPlayer>>(), "NextBotPlayer<CTFPlayer>::PressUseButton");
template<> MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void              > NextBotPlayer<CTFPlayer>::vt_ReleaseUseButton(        TypeName<NextBotPlayer<CTFPlayer>>(), "NextBotPlayer<CTFPlayer>::ReleaseUseButton");
template<> MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void, float       > NextBotPlayer<CTFPlayer>::vt_PressReloadButton(       TypeName<NextBotPlayer<CTFPlayer>>(), "NextBotPlayer<CTFPlayer>::PressReloadButton");
template<> MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void              > NextBotPlayer<CTFPlayer>::vt_ReleaseReloadButton(     TypeName<NextBotPlayer<CTFPlayer>>(), "NextBotPlayer<CTFPlayer>::ReleaseReloadButton");
template<> MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void, float       > NextBotPlayer<CTFPlayer>::vt_PressForwardButton(      TypeName<NextBotPlayer<CTFPlayer>>(), "NextBotPlayer<CTFPlayer>::PressForwardButton");
template<> MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void              > NextBotPlayer<CTFPlayer>::vt_ReleaseForwardButton(    TypeName<NextBotPlayer<CTFPlayer>>(), "NextBotPlayer<CTFPlayer>::ReleaseForwardButton");
template<> MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void, float       > NextBotPlayer<CTFPlayer>::vt_PressBackwardButton(     TypeName<NextBotPlayer<CTFPlayer>>(), "NextBotPlayer<CTFPlayer>::PressBackwardButton");
template<> MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void              > NextBotPlayer<CTFPlayer>::vt_ReleaseBackwardButton(   TypeName<NextBotPlayer<CTFPlayer>>(), "NextBotPlayer<CTFPlayer>::ReleaseBackwardButton");
template<> MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void, float       > NextBotPlayer<CTFPlayer>::vt_PressLeftButton(         TypeName<NextBotPlayer<CTFPlayer>>(), "NextBotPlayer<CTFPlayer>::PressLeftButton");
template<> MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void              > NextBotPlayer<CTFPlayer>::vt_ReleaseLeftButton(       TypeName<NextBotPlayer<CTFPlayer>>(), "NextBotPlayer<CTFPlayer>::ReleaseLeftButton");
template<> MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void, float       > NextBotPlayer<CTFPlayer>::vt_PressRightButton(        TypeName<NextBotPlayer<CTFPlayer>>(), "NextBotPlayer<CTFPlayer>::PressRightButton");
template<> MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void              > NextBotPlayer<CTFPlayer>::vt_ReleaseRightButton(      TypeName<NextBotPlayer<CTFPlayer>>(), "NextBotPlayer<CTFPlayer>::ReleaseRightButton");
template<> MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void, float       > NextBotPlayer<CTFPlayer>::vt_PressJumpButton(         TypeName<NextBotPlayer<CTFPlayer>>(), "NextBotPlayer<CTFPlayer>::PressJumpButton");
template<> MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void              > NextBotPlayer<CTFPlayer>::vt_ReleaseJumpButton(       TypeName<NextBotPlayer<CTFPlayer>>(), "NextBotPlayer<CTFPlayer>::ReleaseJumpButton");
template<> MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void, float       > NextBotPlayer<CTFPlayer>::vt_PressCrouchButton(       TypeName<NextBotPlayer<CTFPlayer>>(), "NextBotPlayer<CTFPlayer>::PressCrouchButton");
template<> MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void              > NextBotPlayer<CTFPlayer>::vt_ReleaseCrouchButton(     TypeName<NextBotPlayer<CTFPlayer>>(), "NextBotPlayer<CTFPlayer>::ReleaseCrouchButton");
template<> MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void, float       > NextBotPlayer<CTFPlayer>::vt_PressWalkButton(         TypeName<NextBotPlayer<CTFPlayer>>(), "NextBotPlayer<CTFPlayer>::PressWalkButton");
template<> MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void              > NextBotPlayer<CTFPlayer>::vt_ReleaseWalkButton(       TypeName<NextBotPlayer<CTFPlayer>>(), "NextBotPlayer<CTFPlayer>::ReleaseWalkButton");
template<> MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void, float, float> NextBotPlayer<CTFPlayer>::vt_SetButtonScale(          TypeName<NextBotPlayer<CTFPlayer>>(), "NextBotPlayer<CTFPlayer>::SetButtonScale");


MemberFuncThunk<CTFBot::SuspectedSpyInfo_t *, void> CTFBot::SuspectedSpyInfo_t::ft_Suspect             ("CTFBot::SuspectedSpyInfo_t::Suspect");
MemberFuncThunk<CTFBot::SuspectedSpyInfo_t *, bool> CTFBot::SuspectedSpyInfo_t::ft_IsCurrentlySuspected("CTFBot::SuspectedSpyInfo_t::IsCurrentlySuspected");
MemberFuncThunk<CTFBot::SuspectedSpyInfo_t *, bool> CTFBot::SuspectedSpyInfo_t::ft_TestForRealizing    ("CTFBot::SuspectedSpyInfo_t::TestForRealizing");


IMPL_EXTRACT(CTFBot::MissionType, CTFBot, m_nMission, new CExtract_CTFBot_m_nMission());

MemberFuncThunk<const CTFBot *, ILocomotion *                            > CTFBot::ft_GetLocomotionInterface      ("CTFBot::GetLocomotionInterface");
MemberFuncThunk<const CTFBot *, IBody *                                  > CTFBot::ft_GetBodyInterface            ("CTFBot::GetBodyInterface");
MemberFuncThunk<const CTFBot *, IVision *                                > CTFBot::ft_GetVisionInterface          ("CTFBot::GetVisionInterface");
MemberFuncThunk<const CTFBot *, IIntention *                             > CTFBot::ft_GetIntentionInterface       ("CTFBot::GetIntentionInterface");
MemberFuncThunk<const CTFBot *, float                                    > CTFBot::ft_GetDesiredPathLookAheadRange("CTFBot::GetDesiredPathLookAheadRange");
MemberFuncThunk<      CTFBot *, void, CTFWeaponBase *                    > CTFBot::ft_PushRequiredWeapon          ("CTFBot::PushRequiredWeapon");
MemberFuncThunk<      CTFBot *, void                                     > CTFBot::ft_PopRequiredWeapon           ("CTFBot::PopRequiredWeapon");
MemberFuncThunk<const CTFBot *, bool, const Vector&                      > CTFBot::ft_IsLineOfFireClear_vec       ("CTFBot::IsLineOfFireClear_vec");
MemberFuncThunk<const CTFBot *, bool, CBaseEntity *                      > CTFBot::ft_IsLineOfFireClear_ent       ("CTFBot::IsLineOfFireClear_ent");
MemberFuncThunk<const CTFBot *, bool, const Vector&, const Vector&       > CTFBot::ft_IsLineOfFireClear_vec_vec   ("CTFBot::IsLineOfFireClear_vec_vec");
MemberFuncThunk<const CTFBot *, bool, const Vector&, CBaseEntity *       > CTFBot::ft_IsLineOfFireClear_vec_ent   ("CTFBot::IsLineOfFireClear_vec_ent");
MemberFuncThunk<      CTFBot *, CTFBot::SuspectedSpyInfo_t *, CTFPlayer *> CTFBot::ft_IsSuspectedSpy              ("CTFBot::IsSuspectedSpy");
MemberFuncThunk<      CTFBot *, void, CTFPlayer *                        > CTFBot::ft_SuspectSpy                  ("CTFBot::SuspectSpy");
MemberFuncThunk<      CTFBot *, void, CTFPlayer *                        > CTFBot::ft_StopSuspectingSpy           ("CTFBot::StopSuspectingSpy");
MemberFuncThunk<const CTFBot *, bool, CTFPlayer *                        > CTFBot::ft_IsKnownSpy                  ("CTFBot::IsKnownSpy");
MemberFuncThunk<      CTFBot *, void, CTFPlayer *                        > CTFBot::ft_RealizeSpy                  ("CTFBot::RealizeSpy");
MemberFuncThunk<      CTFBot *, void, CTFPlayer *                        > CTFBot::ft_ForgetSpy                   ("CTFBot::ForgetSpy");

std::map<CHandle<CTFBot>, CTFBot::ExtendedAttr> CTFBot::s_ExtAttrs;
