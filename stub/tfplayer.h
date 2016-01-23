#ifndef _INCLUDE_SIGSEGV_STUB_TFPLAYER_H_
#define _INCLUDE_SIGSEGV_STUB_TFPLAYER_H_


#include "stub/baseanimating.h"
#include "prop.h"


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
	DEF_SENDPROP(int,      m_iClass);
	DEF_SENDPROP(string_t, m_iszClassIcon);
	DEF_SENDPROP(string_t, m_iszCustomModel);
};
class CTFPlayerClass : public CTFPlayerClassShared {};

class CTFPlayerShared
{
public:
	
	
private:
	DEF_SENDPROP(float, m_flRageMeter);
	DEF_SENDPROP(bool,  m_bRageDraining);
	DEF_SENDPROP(bool,  m_bInUpgradeZone);
};

class CTFPlayer : public CBaseMultiplayerPlayer
{
public:
	CTFPlayerClass *GetPlayerClass()             { return m_PlayerClass; }
	const CTFPlayerClass *GetPlayerClass() const { return m_PlayerClass; }
	
	bool IsPlayerClass(int iClass) const;
	
	bool IsMiniBoss() const { return this->m_bIsMiniBoss; }
	
private:
	DEF_SENDPROP(CTFPlayerClass, m_PlayerClass);
	DEF_SENDPROP(bool,           m_bIsMiniBoss);
	
public:
	DEF_SENDPROP(CTFPlayerShared, m_Shared);
};


#endif
