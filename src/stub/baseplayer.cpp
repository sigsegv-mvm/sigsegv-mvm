#include "stub/baseplayer.h"
#include "stub/tfplayer.h"


IMPL_SENDPROP(CHandle<CBaseCombatWeapon>,              CBaseCombatCharacter, m_hActiveWeapon, CBaseCombatCharacter);
IMPL_SENDPROP(CHandle<CBaseCombatWeapon>[MAX_WEAPONS], CBaseCombatCharacter, m_hMyWeapons,    CBaseCombatCharacter);

MemberFuncThunk<CBaseCombatCharacter *, void>                                                                     CBaseCombatCharacter::ft_AddGlowEffect     ("CBaseCombatCharacter::AddGlowEffect");
MemberFuncThunk<CBaseCombatCharacter *, void>                                                                     CBaseCombatCharacter::ft_RemoveGlowEffect  ("CBaseCombatCharacter::RemoveGlowEffect");
MemberFuncThunk<CBaseCombatCharacter *, bool>                                                                     CBaseCombatCharacter::ft_IsGlowEffectActive("CBaseCombatCharacter::IsGlowEffectActive");
MemberFuncThunk<CBaseCombatCharacter *, bool, const CBaseEntity *, CBaseCombatCharacter::FieldOfViewCheckType>    CBaseCombatCharacter::ft_IsAbleToSee_ent   ("CBaseCombatCharacter::IsAbleToSee [CBaseEntity *]");
MemberFuncThunk<CBaseCombatCharacter *, bool, CBaseCombatCharacter *, CBaseCombatCharacter::FieldOfViewCheckType> CBaseCombatCharacter::ft_IsAbleToSee_bcc   ("CBaseCombatCharacter::IsAbleToSee [CBaseCombatCharacter *]");
MemberFuncThunk<CBaseCombatCharacter *, void, int>                                                                CBaseCombatCharacter::ft_SetBloodColor     ("CBaseCombatCharacter::SetBloodColor");
MemberFuncThunk<CBaseCombatCharacter *, bool, CBaseCombatWeapon *>                                                CBaseCombatCharacter::ft_Weapon_Detach     ("CBaseCombatCharacter::Weapon_Detach");

MemberVFuncThunk<const CBaseCombatCharacter *, CBaseCombatWeapon *, int>       CBaseCombatCharacter::vt_Weapon_GetSlot     (TypeName<CBaseCombatCharacter>(), "CBaseCombatCharacter::Weapon_GetSlot");
MemberVFuncThunk<      CBaseCombatCharacter *, bool, CBaseCombatWeapon *>      CBaseCombatCharacter::vt_Weapon_CanSwitchTo (TypeName<CBaseCombatCharacter>(), "CBaseCombatCharacter::Weapon_CanSwitchTo");
MemberVFuncThunk<      CBaseCombatCharacter *, bool, CBaseCombatWeapon *, int> CBaseCombatCharacter::vt_Weapon_Switch      (TypeName<CBaseCombatCharacter>(), "CBaseCombatCharacter::Weapon_Switch");
MemberVFuncThunk<const CBaseCombatCharacter *, CNavArea *>                     CBaseCombatCharacter::vt_GetLastKnownArea   (TypeName<CBaseCombatCharacter>(), "CBaseCombatCharacter::GetLastKnownArea");
MemberVFuncThunk<      CBaseCombatCharacter *, void>                           CBaseCombatCharacter::vt_UpdateLastKnownArea(TypeName<CBaseCombatCharacter>(), "CBaseCombatCharacter::UpdateLastKnownArea");
MemberVFuncThunk<      CBaseCombatCharacter *, int, int, int, bool>            CBaseCombatCharacter::vt_GiveAmmo           (TypeName<CBaseCombatCharacter>(), "CBaseCombatCharacter::GiveAmmo");
MemberVFuncThunk<const CBaseCombatCharacter *, int, int>                       CBaseCombatCharacter::vt_GetAmmoCount       (TypeName<CBaseCombatCharacter>(), "CBaseCombatCharacter::GetAmmoCount");
MemberVFuncThunk<      CBaseCombatCharacter *, bool, const CTakeDamageInfo&>   CBaseCombatCharacter::vt_ShouldGib          (TypeName<CTFPlayer>(),            "CTFPlayer::ShouldGib");


//IMPL_SENDPROP(bool,  CPlayerLocalData, m_bDucked,        CBasePlayer);
//IMPL_SENDPROP(bool,  CPlayerLocalData, m_bDucking,       CBasePlayer);
//IMPL_SENDPROP(bool,  CPlayerLocalData, m_bInDuckJump,    CBasePlayer);
//IMPL_SENDPROP(float, CPlayerLocalData, m_flDucktime,     CBasePlayer);
//IMPL_SENDPROP(float, CPlayerLocalData, m_flDuckJumpTime, CBasePlayer);


IMPL_DATAMAP(char[32],     CBasePlayer, m_szNetname);
IMPL_DATAMAP(bool,         CBasePlayer, m_bDuckToggled);
IMPL_DATAMAP(unsigned int, CBasePlayer, m_afPhysicsFlags);
IMPL_DATAMAP(int,          CBasePlayer, m_vphysicsCollisionState);

IMPL_SENDPROP(float,                              CBasePlayer, m_flDeathTime,   CBasePlayer);
IMPL_SENDPROP(int,                                CBasePlayer, m_iObserverMode, CBasePlayer);
IMPL_SENDPROP(CPlayerLocalData,                   CBasePlayer, m_Local,         CBasePlayer);
IMPL_SENDPROP(int,                                CBasePlayer, m_nTickBase,     CBasePlayer);
IMPL_SENDPROP(float,                              CBasePlayer, m_flMaxspeed,    CBasePlayer);
IMPL_SENDPROP(CUtlVector<CHandle<CEconWearable>>, CBasePlayer, m_hMyWearables,  CBasePlayer);

MemberFuncThunk<CBasePlayer *, void, Vector *, Vector *, Vector *> CBasePlayer::ft_EyeVectors    ("CBasePlayer::EyeVectors");
MemberFuncThunk<CBasePlayer *, bool, CSteamID *>                   CBasePlayer::ft_GetSteamID    ("CBasePlayer::GetSteamID");
MemberFuncThunk<CBasePlayer *, void, const char *>                 CBasePlayer::ft_SetPlayerName ("CBasePlayer::SetPlayerName");
MemberFuncThunk<CBasePlayer *, CBaseViewModel *, int, bool>        CBasePlayer::ft_GetViewModel  ("CBasePlayer::GetViewModel");
MemberFuncThunk<CBasePlayer *, void, int>                          CBasePlayer::ft_DisableButtons("CBasePlayer::DisableButtons");
MemberFuncThunk<CBasePlayer *, void, int>                          CBasePlayer::ft_EnableButtons ("CBasePlayer::EnableButtons");
MemberFuncThunk<CBasePlayer *, void, int>                          CBasePlayer::ft_ForceButtons  ("CBasePlayer::ForceButtons");
MemberFuncThunk<CBasePlayer *, void, int>                          CBasePlayer::ft_UnforceButtons("CBasePlayer::UnforceButtons");
MemberFuncThunk<CBasePlayer *, void, const QAngle&>                CBasePlayer::ft_SnapEyeAngles ("CBasePlayer::SnapEyeAngles");

MemberVFuncThunk<const CBasePlayer *, bool>                  CBasePlayer::vt_IsBot               (TypeName<CBasePlayer>(), "CBasePlayer::IsBot");
MemberVFuncThunk<      CBasePlayer *, void, bool, bool>      CBasePlayer::vt_CommitSuicide       (TypeName<CBasePlayer>(), "CBasePlayer::CommitSuicide");
MemberVFuncThunk<      CBasePlayer *, void>                  CBasePlayer::vt_ForceRespawn        (TypeName<CTFPlayer>(),   "CTFPlayer::ForceRespawn");
MemberVFuncThunk<      CBasePlayer *, Vector>                CBasePlayer::vt_Weapon_ShootPosition(TypeName<CBasePlayer>(), "CBasePlayer::Weapon_ShootPosition");
MemberVFuncThunk<      CBasePlayer *, float>                 CBasePlayer::vt_GetPlayerMaxSpeed   (TypeName<CBasePlayer>(), "CBasePlayer::GetPlayerMaxSpeed");
MemberVFuncThunk<      CBasePlayer *, void, CEconWearable *> CBasePlayer::vt_RemoveWearable      (TypeName<CBasePlayer>(), "CBasePlayer::RemoveWearable");


MemberVFuncThunk<CBaseMultiplayerPlayer *, bool, int, const char *, char *, size_t, IRecipientFilter *> CBaseMultiplayerPlayer::vt_SpeakConceptIfAllowed(TypeName<CTFPlayer>(), "CTFPlayer::SpeakConceptIfAllowed");
