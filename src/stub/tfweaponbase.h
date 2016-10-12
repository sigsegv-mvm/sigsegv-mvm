#ifndef _INCLUDE_SIGSEGV_STUB_TFWEAPONBASE_H_
#define _INCLUDE_SIGSEGV_STUB_TFWEAPONBASE_H_


#include "stub/misc.h"
#include "stub/tfplayer.h"
#include "stub/entities.h"


class CBaseCombatWeapon : public CEconEntity
{
public:
	CBaseCombatCharacter *GetOwner() const { return this->m_hOwner; }
	
	bool IsMeleeWeapon() const { return ft_IsMeleeWeapon(this); }
	
	int GetMaxClip1() const { return vt_GetMaxClip1(this); }
	int GetMaxClip2() const { return vt_GetMaxClip2(this); }
	
	DECL_SENDPROP(float,                         m_flNextPrimaryAttack);
	DECL_SENDPROP(float,                         m_flNextSecondaryAttack);
	DECL_SENDPROP(float,                         m_flTimeWeaponIdle);
	DECL_SENDPROP(int,                           m_iState);
	DECL_SENDPROP(int,                           m_iPrimaryAmmoType);
	DECL_SENDPROP(int,                           m_iSecondaryAmmoType);
	DECL_SENDPROP(int,                           m_iClip1);
	DECL_SENDPROP(int,                           m_iClip2);
	
private:
	DECL_SENDPROP(CHandle<CBaseCombatCharacter>, m_hOwner);
	
	static MemberFuncThunk<const CBaseCombatWeapon *, bool> ft_IsMeleeWeapon;
	
	static MemberVFuncThunk<const CBaseCombatWeapon *, int> vt_GetMaxClip1;
	static MemberVFuncThunk<const CBaseCombatWeapon *, int> vt_GetMaxClip2;
};

class CTFWeaponBase : public CBaseCombatWeapon
{
public:
	CTFPlayer *GetTFPlayerOwner() const { return ToTFPlayer(this->GetOwner()); }
	
	int GetWeaponID() const      { return vt_GetWeaponID     (this); }
	int GetPenetrateType() const { return vt_GetPenetrateType(this); }
	
private:
	static MemberVFuncThunk<const CTFWeaponBase *, int> vt_GetWeaponID;
	static MemberVFuncThunk<const CTFWeaponBase *, int> vt_GetPenetrateType;
};

class CTFWeaponBaseMelee : public CTFWeaponBase
{
public:
	int GetSwingRange() { return vt_GetSwingRange(this); }
	
private:
	static MemberVFuncThunk<CTFWeaponBaseMelee *, int> vt_GetSwingRange;
};

class CTFWeaponBaseGun : public CTFWeaponBase {};

class CTFSniperRifle : public CTFWeaponBaseGun {};

class CTFSniperRifleClassic : public CTFSniperRifle {};

class CTFSniperRifleDecap : public CTFSniperRifle
{
public:
	int GetCount() { return ft_GetCount(this); }
	
private:
	static MemberFuncThunk<CTFSniperRifleDecap *, int> ft_GetCount;
};


class CTFBonesaw : public CTFWeaponBaseMelee {};

class CTFWrench : public CTFWeaponBaseMelee {};

class CTFRobotArm : public CTFWrench
{
public:
	/* this is a hacky mess for now */
	
	int GetPunchNumber() const            { return *reinterpret_cast<int   *>((uintptr_t)&this->m_hRobotArm + 0x04); }
	float GetLastPunchTime() const        { return *reinterpret_cast<float *>((uintptr_t)&this->m_hRobotArm + 0x08); }
	bool ShouldInflictComboDamage() const { return *reinterpret_cast<bool  *>((uintptr_t)&this->m_hRobotArm + 0x0c); }
	bool ShouldImpartMaxForce() const     { return *reinterpret_cast<bool  *>((uintptr_t)&this->m_hRobotArm + 0x0d); }
	
	// 20151007a:
	// CTFRobotArm +0x800 CHandle<CTFWearableRobotArm> m_hRobotArm
	// CTFRobotArm +0x804 int                          m_iPunchNumber
	// CTFRobotArm +0x808 float                        m_flTimeLastPunch
	// CTFRobotArm +0x80c bool                         m_bComboPunch
	// CTFRobotArm +0x80d bool                         m_bMaxForce
	
private:
	DECL_SENDPROP(CHandle<CTFWearableRobotArm>, m_hRobotArm);
};

class CTFBuffItem : public CTFWeaponBaseMelee {};

class CTFLunchBox : public CTFWeaponBase {};
class CTFLunchBox_Drink : public CTFLunchBox {};

class CWeaponMedigun : public CTFWeaponBase
{
public:
	CBaseEntity *GetHealTarget() const { return this->m_hHealingTarget; }
	
private:
	DECL_SENDPROP(CHandle<CBaseEntity>, m_hHealingTarget);
};

class CTFFlameThrower : public CTFWeaponBaseGun {};

class CTFWeaponBuilder : public CTFWeaponBase {};
class CTFWeaponSapper : public CTFWeaponBuilder {};


bool WeaponID_IsSniperRifle(int id);
bool WeaponID_IsSniperRifleOrBow(int id);


#endif
