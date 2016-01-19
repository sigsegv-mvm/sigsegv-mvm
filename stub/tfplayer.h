#ifndef _INCLUDE_SIGSEGV_STUB_TFPLAYER_H_
#define _INCLUDE_SIGSEGV_STUB_TFPLAYER_H_


#include "stub/baseanimating.h"
#include "prop/prop.h"


class CBaseCombatCharacter : public CBaseFlex {};

class CBasePlayer : public CBaseCombatCharacter
{
public:
	bool IsBot() const { return (vt_IsBot.Get(this))(this); }
	
private:
	static VFuncThunk<bool (*)(const CBasePlayer *)> vt_IsBot;
};

class CBaseMultiplayerPlayer : public CBasePlayer {};


enum
{
	TF_CLASS_UNDEFINED = 0,
	
	TF_CLASS_SCOUT,
	TF_CLASS_SNIPER,
	TF_CLASS_SOLDIER,
	TF_CLASS_DEMOMAN,
	TF_CLASS_MEDIC,
	TF_CLASS_HEAVYWEAPONS,
	TF_CLASS_PYRO,
	TF_CLASS_SPY,
	TF_CLASS_ENGINEER,
	
	TF_CLASS_COUNT,
};

class CTFPlayerClassShared
{
public:
	bool IsClass(int iClass) const { return (this->m_iClass == iClass); }
	
	int GetClassIndex() const { return this->m_iClass; }
	// TODO: accessor for m_iszClassIcon
	// TODO: accessor for m_iszCustomModel
	
private:
	PROP_STR(CTFPlayer);
	
	PROP_SENDPROP(int,      CTFPlayer, m_iClass);
	PROP_SENDPROP(string_t, CTFPlayer, m_iszClassIcon);
	PROP_SENDPROP(string_t, CTFPlayer, m_iszCustomModel);
};
class CTFPlayerClass : public CTFPlayerClassShared {};

class CTFPlayerShared
{
public:
	
	
private:
	PROP_STR(CTFPlayerShared);
	
	
};

class CTFPlayer : public CBaseMultiplayerPlayer
{
public:
	CTFPlayerClass *GetPlayerClass() { return reinterpret_cast<CTFPlayerClass *>(this); }
	const CTFPlayerClass *GetPlayerClass() const { return reinterpret_cast<const CTFPlayerClass *>(this); }
	
	bool IsPlayerClass(int iClass) const;
	
private:
	PROP_STR(CTFPlayer);
	
//	PROP_SENDPROP(CTFPlayerClass,  CTFPlayer, m_PlayerClass);
	
public:
//	PROP_SENDPROP(CTFPlayerShared, CTFPlayer, m_Shared);
};


#endif
