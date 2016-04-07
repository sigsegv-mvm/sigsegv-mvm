#ifndef _INCLUDE_SIGSEGV_STUB_BASEPLAYER_H_
#define _INCLUDE_SIGSEGV_STUB_BASEPLAYER_H_


#include "stub/baseanimating.h"
#include "stub/nav.h"


class CBaseCombatWeapon;


class CBaseCombatCharacter : public CBaseFlex
{
public:
	CBaseCombatWeapon *GetActiveWeapon() const { return this->m_hActiveWeapon; }
	
	CBaseCombatWeapon *Weapon_GetSlot(int slot) const { return ft_Weapon_GetSlot(this, slot); }
	
	CNavArea *GetLastKnownArea() const { return vt_GetLastKnownArea(this); }
	
private:
	DECL_SENDPROP(CHandle<CBaseCombatWeapon>, m_hActiveWeapon);
	
	static MemberFuncThunk<const CBaseCombatCharacter *, CBaseCombatWeapon *, int> ft_Weapon_GetSlot;
	
	static MemberVFuncThunk<const CBaseCombatCharacter *, CNavArea *> vt_GetLastKnownArea;
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
