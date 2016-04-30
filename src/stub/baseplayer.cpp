#include "stub/baseplayer.h"
#include "stub/tfplayer.h"


IMPL_SENDPROP(CHandle<CBaseCombatWeapon>, CBaseCombatCharacter, m_hActiveWeapon, CBaseCombatCharacter);

MemberFuncThunk<CBaseCombatCharacter *, void> CBaseCombatCharacter::ft_AddGlowEffect     ("CBaseCombatCharacter::AddGlowEffect");
MemberFuncThunk<CBaseCombatCharacter *, void> CBaseCombatCharacter::ft_RemoveGlowEffect  ("CBaseCombatCharacter::RemoveGlowEffect");
MemberFuncThunk<CBaseCombatCharacter *, bool> CBaseCombatCharacter::ft_IsGlowEffectActive("CBaseCombatCharacter::IsGlowEffectActive");

MemberVFuncThunk<const CBaseCombatCharacter *, CBaseCombatWeapon *, int>       CBaseCombatCharacter::vt_Weapon_GetSlot    (TypeName<CBaseCombatCharacter>(), "CBaseCombatCharacter::Weapon_GetSlot");
MemberVFuncThunk<      CBaseCombatCharacter *, bool, CBaseCombatWeapon *>      CBaseCombatCharacter::vt_Weapon_CanSwitchTo(TypeName<CBaseCombatCharacter>(), "CBaseCombatCharacter::Weapon_CanSwitchTo");
MemberVFuncThunk<      CBaseCombatCharacter *, bool, CBaseCombatWeapon *, int> CBaseCombatCharacter::vt_Weapon_Switch     (TypeName<CBaseCombatCharacter>(), "CBaseCombatCharacter::Weapon_Switch");
MemberVFuncThunk<const CBaseCombatCharacter *, CNavArea *>                     CBaseCombatCharacter::vt_GetLastKnownArea  (TypeName<CBaseCombatCharacter>(), "CBaseCombatCharacter::GetLastKnownArea");


IMPL_DATAMAP(char, CBasePlayer, m_szNetname);

MemberFuncThunk<CBasePlayer *, void, Vector *, Vector *, Vector *> CBasePlayer::ft_EyeVectors("CBasePlayer::EyeVectors");

MemberVFuncThunk<const CBasePlayer *, bool>             CBasePlayer::vt_IsBot(        TypeName<CBasePlayer>(), "CBasePlayer::IsBot");
MemberVFuncThunk<      CBasePlayer *, void, bool, bool> CBasePlayer::vt_CommitSuicide(TypeName<CBasePlayer>(), "CBasePlayer::CommitSuicide");
MemberVFuncThunk<      CBasePlayer *, void>             CBasePlayer::vt_ForceRespawn( TypeName<CTFPlayer>(),   "CTFPlayer::ForceRespawn");


MemberVFuncThunk<CBaseMultiplayerPlayer *, bool, int, const char *, char *, size_t, IRecipientFilter *> CBaseMultiplayerPlayer::vt_SpeakConceptIfAllowed(TypeName<CTFPlayer>(), "CTFPlayer::SpeakConceptIfAllowed");
