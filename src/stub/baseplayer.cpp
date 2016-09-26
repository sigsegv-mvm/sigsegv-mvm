#include "stub/baseplayer.h"
#include "stub/tfplayer.h"


IMPL_SENDPROP(CHandle<CBaseCombatWeapon>, CBaseCombatCharacter, m_hActiveWeapon, CBaseCombatCharacter);

MemberFuncThunk<CBaseCombatCharacter *, void>                                                                     CBaseCombatCharacter::ft_AddGlowEffect     ("CBaseCombatCharacter::AddGlowEffect");
MemberFuncThunk<CBaseCombatCharacter *, void>                                                                     CBaseCombatCharacter::ft_RemoveGlowEffect  ("CBaseCombatCharacter::RemoveGlowEffect");
MemberFuncThunk<CBaseCombatCharacter *, bool>                                                                     CBaseCombatCharacter::ft_IsGlowEffectActive("CBaseCombatCharacter::IsGlowEffectActive");
MemberFuncThunk<CBaseCombatCharacter *, bool, const CBaseEntity *, CBaseCombatCharacter::FieldOfViewCheckType>    CBaseCombatCharacter::ft_IsAbleToSee_ent   ("CBaseCombatCharacter::IsAbleToSee [CBaseEntity *]");
MemberFuncThunk<CBaseCombatCharacter *, bool, CBaseCombatCharacter *, CBaseCombatCharacter::FieldOfViewCheckType> CBaseCombatCharacter::ft_IsAbleToSee_bcc   ("CBaseCombatCharacter::IsAbleToSee [CBaseCombatCharacter *]");
MemberFuncThunk<CBaseCombatCharacter *, void, int>                                                                CBaseCombatCharacter::ft_SetBloodColor     ("CBaseCombatCharacter::SetBloodColor");

MemberVFuncThunk<const CBaseCombatCharacter *, CBaseCombatWeapon *, int>       CBaseCombatCharacter::vt_Weapon_GetSlot    (TypeName<CBaseCombatCharacter>(), "CBaseCombatCharacter::Weapon_GetSlot");
MemberVFuncThunk<      CBaseCombatCharacter *, bool, CBaseCombatWeapon *>      CBaseCombatCharacter::vt_Weapon_CanSwitchTo(TypeName<CBaseCombatCharacter>(), "CBaseCombatCharacter::Weapon_CanSwitchTo");
MemberVFuncThunk<      CBaseCombatCharacter *, bool, CBaseCombatWeapon *, int> CBaseCombatCharacter::vt_Weapon_Switch     (TypeName<CBaseCombatCharacter>(), "CBaseCombatCharacter::Weapon_Switch");
MemberVFuncThunk<const CBaseCombatCharacter *, CNavArea *>                     CBaseCombatCharacter::vt_GetLastKnownArea  (TypeName<CBaseCombatCharacter>(), "CBaseCombatCharacter::GetLastKnownArea");
MemberVFuncThunk<      CBaseCombatCharacter *, int, int, int, bool>            CBaseCombatCharacter::vt_GiveAmmo          (TypeName<CBaseCombatCharacter>(), "CBaseCombatCharacter::GiveAmmo");


IMPL_DATAMAP(char[32], CBasePlayer, m_szNetname);

IMPL_SENDPROP(int,   CBasePlayer, m_nTickBase,  CBasePlayer);
IMPL_SENDPROP(float, CBasePlayer, m_flMaxSpeed, CBasePlayer);

MemberFuncThunk<CBasePlayer *, void, Vector *, Vector *, Vector *> CBasePlayer::ft_EyeVectors   ("CBasePlayer::EyeVectors");
MemberFuncThunk<CBasePlayer *, bool, CSteamID *>                   CBasePlayer::ft_GetSteamID   ("CBasePlayer::GetSteamID");
MemberFuncThunk<CBasePlayer *, void, const char *>                 CBasePlayer::ft_SetPlayerName("CBasePlayer::SetPlayerName");

MemberVFuncThunk<const CBasePlayer *, bool>             CBasePlayer::vt_IsBot               (TypeName<CBasePlayer>(), "CBasePlayer::IsBot");
MemberVFuncThunk<      CBasePlayer *, void, bool, bool> CBasePlayer::vt_CommitSuicide       (TypeName<CBasePlayer>(), "CBasePlayer::CommitSuicide");
MemberVFuncThunk<      CBasePlayer *, void>             CBasePlayer::vt_ForceRespawn        (TypeName<CTFPlayer>(),   "CTFPlayer::ForceRespawn");
MemberVFuncThunk<      CBasePlayer *, Vector>           CBasePlayer::vt_Weapon_ShootPosition(TypeName<CBasePlayer>(), "CBasePlayer::Weapon_ShootPosition");
MemberVFuncThunk<      CBasePlayer *, float>            CBasePlayer::vt_GetPlayerMaxSpeed   (TypeName<CBasePlayer>(), "CBasePlayer::GetPlayerMaxSpeed");


MemberVFuncThunk<CBaseMultiplayerPlayer *, bool, int, const char *, char *, size_t, IRecipientFilter *> CBaseMultiplayerPlayer::vt_SpeakConceptIfAllowed(TypeName<CTFPlayer>(), "CTFPlayer::SpeakConceptIfAllowed");
