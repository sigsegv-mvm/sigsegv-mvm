#ifndef _INCLUDE_SIGSEGV_STUB_OBJECTS_H_
#define _INCLUDE_SIGSEGV_STUB_OBJECTS_H_


#include "stub/baseentity.h"
#include "stub/tfplayer.h"


class CBaseObject : public CBaseCombatCharacter
{
public:
	int GetType() const           { return this->m_iObjectType; }
	int GetObjectMode() const     { return this->m_iObjectMode; }
	CTFPlayer *GetBuilder() const { return this->m_hBuilder; }
	
	void StartPlacement(CTFPlayer *pPlayer)   {        vt_StartPlacement(this, pPlayer); }
	bool StartBuilding(CBaseEntity *pBuilder) { return vt_StartBuilding (this, pBuilder); }
	
	DECL_DATAMAP(int, m_nDefaultUpgradeLevel);
	
private:
	DECL_SENDPROP(int,                m_iObjectType);
	DECL_SENDPROP(int,                m_iObjectMode);
	DECL_SENDPROP(CHandle<CTFPlayer>, m_hBuilder);
	
	static MemberVFuncThunk<CBaseObject *, void, CTFPlayer *  > vt_StartPlacement;
	static MemberVFuncThunk<CBaseObject *, bool, CBaseEntity *> vt_StartBuilding;
};


class CObjectDispenser : public CBaseObject
{
public:
	float GetDispenserRadius() { return vt_GetDispenserRadius(this); }
	
private:
	static MemberVFuncThunk<CObjectDispenser *, float> vt_GetDispenserRadius;
};

class CObjectCartDispenser : public CObjectDispenser {};


class CBaseObjectUpgrade : public CBaseObject {};
class CObjectSapper : public CBaseObjectUpgrade {};


class CObjectSentrygun : public CBaseObject {};


class CObjectTeleporter : public CBaseObject {};


#endif
