#ifndef _INCLUDE_SIGSEGV_STUB_BASEPLAYER_H_
#define _INCLUDE_SIGSEGV_STUB_BASEPLAYER_H_


#include "stub/baseanimating.h"


class CNavArea;
class CBaseCombatWeapon;


class CBaseCombatCharacter : public CBaseFlex
{
public:
	CBaseCombatWeapon *GetActiveWeapon() const { return this->m_hActiveWeapon; }
	
	void AddGlowEffect()      {        ft_AddGlowEffect     (this); }
	void RemoveGlowEffect()   {        ft_RemoveGlowEffect  (this); }
	bool IsGlowEffectActive() { return ft_IsGlowEffectActive(this); }
	
	CBaseCombatWeapon *Weapon_GetSlot(int slot) const                      { return vt_Weapon_GetSlot    (this, slot); }
	bool Weapon_CanSwitchTo(CBaseCombatWeapon *pWeapon)                    { return vt_Weapon_CanSwitchTo(this, pWeapon); }
	bool Weapon_Switch(CBaseCombatWeapon *pWeapon, int viewmodelindex = 0) { return vt_Weapon_Switch     (this, pWeapon, viewmodelindex); }
	CNavArea *GetLastKnownArea() const                                     { return vt_GetLastKnownArea  (this); }
	
private:
	DECL_SENDPROP(CHandle<CBaseCombatWeapon>, m_hActiveWeapon);
	
	static MemberFuncThunk<CBaseCombatCharacter *, void> ft_AddGlowEffect;
	static MemberFuncThunk<CBaseCombatCharacter *, void> ft_RemoveGlowEffect;
	static MemberFuncThunk<CBaseCombatCharacter *, bool> ft_IsGlowEffectActive;
	
	static MemberVFuncThunk<const CBaseCombatCharacter *, CBaseCombatWeapon *, int>       vt_Weapon_GetSlot;
	static MemberVFuncThunk<      CBaseCombatCharacter *, bool, CBaseCombatWeapon *>      vt_Weapon_CanSwitchTo;
	static MemberVFuncThunk<      CBaseCombatCharacter *, bool, CBaseCombatWeapon *, int> vt_Weapon_Switch;
	static MemberVFuncThunk<const CBaseCombatCharacter *, CNavArea *>                     vt_GetLastKnownArea;
};

class CBasePlayer : public CBaseCombatCharacter
{
public:
	const char *GetPlayerName() { return m_szNetname.GetPtr(); }
	
	void EyeVectors(Vector *pForward, Vector *pRight = nullptr, Vector *pUp = nullptr) { return ft_EyeVectors(this, pForward, pRight, pUp); }
	
	bool IsBot() const                                             { return vt_IsBot        (this); }
	void CommitSuicide(bool bExplode = false, bool bForce = false) {        vt_CommitSuicide(this, bExplode, bForce); }
	void ForceRespawn()                                            {        vt_ForceRespawn (this); }
	
private:
	DECL_DATAMAP(char, m_szNetname);
	
	static MemberFuncThunk<CBasePlayer *, void, Vector *, Vector *, Vector *> ft_EyeVectors;
	
	static MemberVFuncThunk<const CBasePlayer *, bool>             vt_IsBot;
	static MemberVFuncThunk<      CBasePlayer *, void, bool, bool> vt_CommitSuicide;
	static MemberVFuncThunk<      CBasePlayer *, void>             vt_ForceRespawn;
};

class CBaseMultiplayerPlayer : public CBasePlayer
{
public:
	bool SpeakConceptIfAllowed(int iConcept, const char *modifiers = nullptr, char *pszOutResponseChosen = nullptr, size_t bufsize = 0, IRecipientFilter *filter = nullptr)
	{
		return vt_SpeakConceptIfAllowed(this, iConcept, modifiers, pszOutResponseChosen, bufsize, filter);
	}
	
private:
	static MemberVFuncThunk<CBaseMultiplayerPlayer *, bool, int, const char *, char *, size_t, IRecipientFilter *> vt_SpeakConceptIfAllowed;
};


inline CBasePlayer *ToBasePlayer(CBaseEntity *pEntity)
{
	if (pEntity == nullptr)   return nullptr;
	if (!pEntity->IsPlayer()) return nullptr;
	
	return rtti_cast<CBasePlayer *>(pEntity);
}

inline CBaseMultiplayerPlayer *ToBaseMultiplayerPlayer(CBaseEntity *pEntity)
{
	if (pEntity == nullptr)   return nullptr;
	if (!pEntity->IsPlayer()) return nullptr;
	
	return rtti_cast<CBaseMultiplayerPlayer *>(pEntity);
}


#endif
