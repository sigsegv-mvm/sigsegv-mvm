#include "stub/tfplayer.h"
#include "stub/tfweaponbase.h"
#include "stub/entities.h"
#include "stub/strings.h"
#include "util/misc.h"


MemberFuncThunk<CMultiplayerAnimState *, void> CMultiplayerAnimState::ft_OnNewModel("CMultiplayerAnimState::OnNewModel");


IMPL_SENDPROP(int,      CTFPlayerClassShared, m_iClass,         CTFPlayer);
IMPL_SENDPROP(string_t, CTFPlayerClassShared, m_iszClassIcon,   CTFPlayer);
IMPL_SENDPROP(string_t, CTFPlayerClassShared, m_iszCustomModel, CTFPlayer);

MemberFuncThunk<CTFPlayerClassShared *, void, const char *, bool> CTFPlayerClassShared::ft_SetCustomModel("CTFPlayerClassShared::SetCustomModel");


IMPL_SENDPROP(float, CTFPlayerShared, m_flCloakMeter,            CTFPlayer);
IMPL_SENDPROP(float, CTFPlayerShared, m_flEnergyDrinkMeter,      CTFPlayer);
IMPL_SENDPROP(float, CTFPlayerShared, m_flHypeMeter,             CTFPlayer);
IMPL_SENDPROP(float, CTFPlayerShared, m_flChargeMeter,           CTFPlayer);
IMPL_SENDPROP(float, CTFPlayerShared, m_flRageMeter,             CTFPlayer);
IMPL_SENDPROP(bool,  CTFPlayerShared, m_bRageDraining,           CTFPlayer);
IMPL_SENDPROP(int,   CTFPlayerShared, m_iCritMult,               CTFPlayer);
IMPL_SENDPROP(bool,  CTFPlayerShared, m_bInUpgradeZone,          CTFPlayer);
IMPL_SENDPROP(float, CTFPlayerShared, m_flStealthNoAttackExpire, CTFPlayer);
IMPL_SENDPROP(int,   CTFPlayerShared, m_nPlayerState,            CTFPlayer);

MemberFuncThunk<      CTFPlayerShared *, void, ETFCond, float, CBaseEntity * > CTFPlayerShared::ft_AddCond             ("CTFPlayerShared::AddCond");
MemberFuncThunk<      CTFPlayerShared *, void, ETFCond, bool                 > CTFPlayerShared::ft_RemoveCond          ("CTFPlayerShared::RemoveCond");
MemberFuncThunk<const CTFPlayerShared *, bool, ETFCond                       > CTFPlayerShared::ft_InCond              ("CTFPlayerShared::InCond");
MemberFuncThunk<const CTFPlayerShared *, bool                                > CTFPlayerShared::ft_IsInvulnerable      ("CTFPlayerShared::IsInvulnerable");
MemberFuncThunk<      CTFPlayerShared *, void, float, float, int, CTFPlayer *> CTFPlayerShared::ft_StunPlayer          ("CTFPlayerShared::StunPlayer");
MemberFuncThunk<      CTFPlayerShared *, void, CBitVec<192>&                 > CTFPlayerShared::ft_GetConditionsBits   ("CTFPlayerShared::GetConditionsBits");
MemberFuncThunk<      CTFPlayerShared *, float, ETFCond                      > CTFPlayerShared::ft_GetConditionDuration("CTFPlayerShared::GetConditionDuration");
MemberFuncThunk<      CTFPlayerShared *, CBaseEntity *, ETFCond              > CTFPlayerShared::ft_GetConditionProvider("CTFPlayerShared::GetConditionProvider");
MemberFuncThunk<const CTFPlayerShared *, int                                 > CTFPlayerShared::ft_GetDisguiseTeam     ("CTFPlayerShared::GetDisguiseTeam");
MemberFuncThunk<const CTFPlayerShared *, bool                                > CTFPlayerShared::ft_IsStealthed         ("CTFPlayerShared::IsStealthed");
MemberFuncThunk<const CTFPlayerShared *, float                               > CTFPlayerShared::ft_GetPercentInvisible ("CTFPlayerShared::GetPercentInvisible");


IMPL_SENDPROP(CTFPlayerShared,      CTFPlayer, m_Shared,           CTFPlayer);
IMPL_SENDPROP(float,                CTFPlayer, m_flLastDamageTime, CTFPlayer);
IMPL_SENDPROP(CHandle<CTFItem>,     CTFPlayer, m_hItem,            CTFPlayer);
IMPL_RELATIVE(CTFPlayerAnimState *, CTFPlayer, m_PlayerAnimState,  m_hItem, -0x18); // 20170116a
IMPL_SENDPROP(CTFPlayerClass,       CTFPlayer, m_PlayerClass,      CTFPlayer);
IMPL_SENDPROP(bool,                 CTFPlayer, m_bIsMiniBoss,      CTFPlayer);
IMPL_SENDPROP(int,                  CTFPlayer, m_nCurrency,        CTFPlayer);

MemberFuncThunk<      CTFPlayer *, void, int, bool                 > CTFPlayer::ft_ForceChangeTeam               ("CTFPlayer::ForceChangeTeam");
MemberFuncThunk<      CTFPlayer *, void, int, int                  > CTFPlayer::ft_StartBuildingObjectOfType     ("CTFPlayer::StartBuildingObjectOfType");
MemberFuncThunk<const CTFPlayer *, bool, ETFFlagType *, int        > CTFPlayer::ft_HasTheFlag                    ("CTFPlayer::HasTheFlag");
MemberFuncThunk<      CTFPlayer *, int, int                        > CTFPlayer::ft_GetAutoTeam                   ("CTFPlayer::GetAutoTeam");
MemberFuncThunk<      CTFPlayer *, float, CTFWeaponBase **         > CTFPlayer::ft_MedicGetChargeLevel           ("CTFPlayer::MedicGetChargeLevel");
MemberFuncThunk<      CTFPlayer *, float, bool                     > CTFPlayer::ft_TeamFortress_CalculateMaxSpeed("CTFPlayer::TeamFortress_CalculateMaxSpeed");
MemberFuncThunk<      CTFPlayer *, void                            > CTFPlayer::ft_UpdateModel                   ("CTFPlayer::UpdateModel");
MemberFuncThunk<const CTFPlayer *, CTFWeaponBase *, int            > CTFPlayer::ft_Weapon_OwnsThisID             ("CTFPlayer::Weapon_OwnsThisID");
MemberFuncThunk<      CTFPlayer *, CTFWeaponBase *, int            > CTFPlayer::ft_Weapon_GetWeaponByType        ("CTFPlayer::Weapon_GetWeaponByType");
MemberFuncThunk<      CTFPlayer *, CBaseObject *, int, int         > CTFPlayer::ft_GetObjectOfType               ("CTFPlayer::GetObjectOfType");
MemberFuncThunk<      CTFPlayer *, int, int, int                   > CTFPlayer::ft_GetMaxAmmo                    ("CTFPlayer::GetMaxAmmo");
MemberFuncThunk<      CTFPlayer *, void, const char *              > CTFPlayer::ft_HandleCommand_JoinTeam        ("CTFPlayer::HandleCommand_JoinTeam");
MemberFuncThunk<      CTFPlayer *, void, const char *              > CTFPlayer::ft_HandleCommand_JoinTeam_NoMenus("CTFPlayer::HandleCommand_JoinTeam_NoMenus");
MemberFuncThunk<      CTFPlayer *, void, const char *, bool        > CTFPlayer::ft_HandleCommand_JoinClass       ("CTFPlayer::HandleCommand_JoinClass");
MemberFuncThunk<      CTFPlayer *, void, const char *, float, float> CTFPlayer::ft_AddCustomAttribute            ("CTFPlayer::AddCustomAttribute");
MemberFuncThunk<      CTFPlayer *, void, const char *              > CTFPlayer::ft_RemoveCustomAttribute         ("CTFPlayer::RemoveCustomAttribute");
MemberFuncThunk<      CTFPlayer *, void                            > CTFPlayer::ft_RemoveAllCustomAttributes     ("CTFPlayer::RemoveAllCustomAttributes");


StaticFuncThunk<CEconItemView *, CTFPlayer *, int, CEconEntity **> CTFPlayerSharedUtils::ft_GetEconItemViewByLoadoutSlot("CTFPlayerSharedUtils::GetEconItemViewByLoadoutSlot");


IMPL_SENDPROP(float, CTFRagdoll, m_flHeadScale,  CTFRagdoll);
IMPL_SENDPROP(float, CTFRagdoll, m_flTorsoScale, CTFRagdoll);
IMPL_SENDPROP(float, CTFRagdoll, m_flHandScale,  CTFRagdoll);


bool CTFPlayer::IsPlayerClass(int iClass) const
{
	const CTFPlayerClass *pClass = this->GetPlayerClass();
	if (pClass == nullptr) return false;
	
	return pClass->IsClass(iClass);
}


CTFWeaponBase *CTFPlayer::GetActiveTFWeapon() const
{
	return rtti_cast<CTFWeaponBase *>(this->GetActiveWeapon());
}


static int GetNumberOfTFConds()
{
	static int iNumTFConds =
	[]{
		ConColorMsg(Color(0xff, 0x00, 0xff, 0xff), "GetNumberOfTFConds: in lambda\n");
		
		const SegInfo& info_seg_server_rodata = LibMgr::GetInfo(Library::SERVER).GetSeg(Segment::RODATA);
		
		constexpr char prefix[] = "TF_COND_";
		
		for (int i = 0; i < 256; ++i) {
			const char *str = g_aConditionNames[i];
			
			if (str == nullptr || !info_seg_server_rodata.ContainsAddr(str, 1) || strncmp(str, prefix, strlen(prefix)) != 0) {
				return i;
			}
		}
		
		assert(false);
		return 0;
	}();
	
	return iNumTFConds;
}


bool IsValidTFConditionNumber(int num)
{
	return (num >= 0 && num < GetNumberOfTFConds());
}

ETFCond ClampTFConditionNumber(int num)
{
	return static_cast<ETFCond>(Clamp(num, 0, GetNumberOfTFConds() - 1));
}


const char *GetTFConditionName(ETFCond cond)
{
	int num = static_cast<int>(cond);
	
	if (!IsValidTFConditionNumber(num)) {
		return nullptr;
	}
	
	return g_aConditionNames[num];
}

ETFCond GetTFConditionFromName(const char *name)
{
	int cond_count = GetNumberOfTFConds();
	for (int i = 0; i < cond_count; ++i) {
		if (FStrEq(g_aConditionNames[i], name)) {
			return static_cast<ETFCond>(i);
		}
	}
	
	return TF_COND_INVALID;
}


int GetNumberOfLoadoutSlots()
{
	static int iNumLoadoutSlots =
	[]{
		ConColorMsg(Color(0xff, 0x00, 0xff, 0xff), "GetNumberOfLoadoutSlots: in lambda\n");
		
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
