#ifndef _INCLUDE_SIGSEGV_STUB_BASEPLAYER_H_
#define _INCLUDE_SIGSEGV_STUB_BASEPLAYER_H_


#include "stub/baseanimating.h"


class CNavArea;
class CBaseCombatWeapon;


class CBaseCombatCharacter : public CBaseFlex
{
public:
	enum FieldOfViewCheckType { USE_FOV, DISREGARD_FOV };
	
	CBaseCombatWeapon *GetActiveWeapon() const { return this->m_hActiveWeapon; }
	
	void AddGlowEffect()                                                        {        ft_AddGlowEffect     (this); }
	void RemoveGlowEffect()                                                     {        ft_RemoveGlowEffect  (this); }
	bool IsGlowEffectActive()                                                   { return ft_IsGlowEffectActive(this); }
	bool IsAbleToSee(const CBaseEntity *entity, FieldOfViewCheckType checkFOV)  { return ft_IsAbleToSee_ent   (this, entity, checkFOV); }
	bool IsAbleToSee(CBaseCombatCharacter *pBCC, FieldOfViewCheckType checkFOV) { return ft_IsAbleToSee_bcc   (this, pBCC, checkFOV); }
	void SetBloodColor(int nBloodColor)                                         {        ft_SetBloodColor     (this, nBloodColor); }
	
	CBaseCombatWeapon *Weapon_GetSlot(int slot) const                      { return vt_Weapon_GetSlot    (this, slot); }
	bool Weapon_CanSwitchTo(CBaseCombatWeapon *pWeapon)                    { return vt_Weapon_CanSwitchTo(this, pWeapon); }
	bool Weapon_Switch(CBaseCombatWeapon *pWeapon, int viewmodelindex = 0) { return vt_Weapon_Switch     (this, pWeapon, viewmodelindex); }
	CNavArea *GetLastKnownArea() const                                     { return vt_GetLastKnownArea  (this); }
	int GiveAmmo(int iCount, int iAmmoIndex, bool bSuppressSound = false)  { return vt_GiveAmmo          (this, iCount, iAmmoIndex, bSuppressSound); }
	
private:
	DECL_SENDPROP(CHandle<CBaseCombatWeapon>, m_hActiveWeapon);
	
	static MemberFuncThunk<CBaseCombatCharacter *, void>                                               ft_AddGlowEffect;
	static MemberFuncThunk<CBaseCombatCharacter *, void>                                               ft_RemoveGlowEffect;
	static MemberFuncThunk<CBaseCombatCharacter *, bool>                                               ft_IsGlowEffectActive;
	static MemberFuncThunk<CBaseCombatCharacter *, bool, const CBaseEntity *, FieldOfViewCheckType>    ft_IsAbleToSee_ent;
	static MemberFuncThunk<CBaseCombatCharacter *, bool, CBaseCombatCharacter *, FieldOfViewCheckType> ft_IsAbleToSee_bcc;
	static MemberFuncThunk<CBaseCombatCharacter *, void, int>                                          ft_SetBloodColor;
	
	static MemberVFuncThunk<const CBaseCombatCharacter *, CBaseCombatWeapon *, int>       vt_Weapon_GetSlot;
	static MemberVFuncThunk<      CBaseCombatCharacter *, bool, CBaseCombatWeapon *>      vt_Weapon_CanSwitchTo;
	static MemberVFuncThunk<      CBaseCombatCharacter *, bool, CBaseCombatWeapon *, int> vt_Weapon_Switch;
	static MemberVFuncThunk<const CBaseCombatCharacter *, CNavArea *>                     vt_GetLastKnownArea;
	static MemberVFuncThunk<      CBaseCombatCharacter *, int, int, int, bool>            vt_GiveAmmo;
};

class CBasePlayer : public CBaseCombatCharacter
{
public:
	const char *GetPlayerName() { return this->m_szNetname.GetPtr(); }
	float MaxSpeed() const      { return this->m_flMaxSpeed; }
	
	void EyeVectors(Vector *pForward, Vector *pRight = nullptr, Vector *pUp = nullptr) { return ft_EyeVectors   (this, pForward, pRight, pUp); }
	bool GetSteamID(CSteamID *pID)                                                     { return ft_GetSteamID   (this, pID); }
	void SetPlayerName(const char *name)                                               {        ft_SetPlayerName(this, name); }
	
	bool IsBot() const                                             { return vt_IsBot               (this); }
	void CommitSuicide(bool bExplode = false, bool bForce = false) {        vt_CommitSuicide       (this, bExplode, bForce); }
	void ForceRespawn()                                            {        vt_ForceRespawn        (this); }
	Vector Weapon_ShootPosition()                                  { return vt_Weapon_ShootPosition(this); }
	float GetPlayerMaxSpeed()                                      { return vt_GetPlayerMaxSpeed   (this); }
	
	DECL_SENDPROP(int, m_nTickBase);
	
private:
	DECL_SENDPROP(float, m_flMaxSpeed);
	
	DECL_DATAMAP(char, m_szNetname);
	
	static MemberFuncThunk<CBasePlayer *, void, Vector *, Vector *, Vector *> ft_EyeVectors;
	static MemberFuncThunk<CBasePlayer *, bool, CSteamID *>                   ft_GetSteamID;
	static MemberFuncThunk<CBasePlayer *, void, const char *>                 ft_SetPlayerName;
	
	static MemberVFuncThunk<const CBasePlayer *, bool>             vt_IsBot;
	static MemberVFuncThunk<      CBasePlayer *, void, bool, bool> vt_CommitSuicide;
	static MemberVFuncThunk<      CBasePlayer *, void>             vt_ForceRespawn;
	static MemberVFuncThunk<      CBasePlayer *, Vector>           vt_Weapon_ShootPosition;
	static MemberVFuncThunk<      CBasePlayer *, float>            vt_GetPlayerMaxSpeed;
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


inline CBaseCombatCharacter *ToBaseCombatCharacter(CBaseEntity *pEntity)
{
	if (pEntity == nullptr) return nullptr;
	
	return pEntity->MyCombatCharacterPointer();
}

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
