#ifndef _INCLUDE_SIGSEGV_STUB_PROJECTILES_H_
#define _INCLUDE_SIGSEGV_STUB_PROJECTILES_H_


#include "link/link.h"
#include "stub/baseanimating.h"


class CBaseProjectile : public CBaseAnimating
{
public:
	CBaseEntity *GetOriginalLauncher() const { return this->m_hOriginalLauncher; }
	
	void SetLauncher(CBaseEntity *pLauncher) {        vt_SetLauncher      (this, pLauncher); }
	int GetProjectileType() const            { return vt_GetProjectileType(this); }
	
private:
	DECL_SENDPROP(CHandle<CBaseEntity>, m_hOriginalLauncher);
	
	static MemberVFuncThunk<      CBaseProjectile *, void, CBaseEntity *> vt_SetLauncher;
	static MemberVFuncThunk<const CBaseProjectile *, int>                 vt_GetProjectileType;
};


class CBaseGrenade : public CBaseProjectile {};

class CThrownGrenade : public CBaseGrenade {};
class CBaseGrenadeConcussion : public CBaseGrenade {};
class CBaseGrenadeContact : public CBaseGrenade {};
class CBaseGrenadeTimed : public CBaseGrenade {};


class CTFBaseProjectile : public CBaseProjectile
{
private:
	DECL_SENDPROP(Vector,               m_vInitialVelocity);
	DECL_SENDPROP(CHandle<CBaseEntity>, m_hLauncher);
};


class CTFBaseRocket  : public CBaseProjectile
{
public:
	CBasePlayer *GetOwnerPlayer() const { return ft_GetOwnerPlayer(this); }
	
private:
	DECL_SENDPROP(Vector,               m_vInitialVelocity);
	DECL_SENDPROP(int,                  m_iDeflected);
	DECL_SENDPROP(CHandle<CBaseEntity>, m_hLauncher);
	
	static MemberFuncThunk<const CTFBaseRocket *, CBasePlayer *> ft_GetOwnerPlayer;
};


class CTFProjectile_Rocket : public CTFBaseRocket
{
public:
	bool IsCritical() const          { return this->m_bCritical; }
	void SetCritical(bool bCritical) { this->m_bCritical = bCritical; }
	
private:
	DECL_SENDPROP(bool, m_bCritical);
};


class CTFProjectile_Flare : public CTFBaseRocket
{
public:
	bool IsCritical() const          { return this->m_bCritical; }
	void SetCritical(bool bCritical) { this->m_bCritical = bCritical; }
	
private:
	DECL_SENDPROP(bool, m_bCritical);
};


class CTFProjectile_Arrow : public CTFBaseRocket
{
public:
	bool IsCritical() const          { return this->m_bCritical; }
	void SetCritical(bool bCritical) { this->m_bCritical = bCritical; }
	
	DECL_EXTRACT(float, m_flTimeInit);
	
private:
	DECL_SENDPROP(bool,  m_bArrowAlight);
	DECL_SENDPROP(bool,  m_bCritical);
	DECL_SENDPROP(int,   m_iProjectileType);
};


class CTFWeaponBaseGrenadeProj : public CBaseGrenade
{
public:
	bool IsCritical() const          { return this->m_bCritical; }
	void SetCritical(bool bCritical) { this->m_bCritical = bCritical; }
	
	int GetWeaponID() const { return vt_GetWeaponID(this); }
	
private:
	DECL_SENDPROP(Vector,               m_vInitialVelocity);
	DECL_SENDPROP(int,                  m_iDeflected);
	DECL_SENDPROP(CHandle<CBaseEntity>, m_hDeflectOwner);
	DECL_SENDPROP(bool,                 m_bCritical);
	
	static MemberVFuncThunk<const CTFWeaponBaseGrenadeProj *, int> vt_GetWeaponID;
};


class CTFGrenadePipebombProjectile : public CTFWeaponBaseGrenadeProj
{
private:
	DECL_SENDPROP(CHandle<CBaseEntity>, m_hLauncher);
	DECL_SENDPROP(bool,                 m_bTouched);
	DECL_SENDPROP(int,                  m_iType);
	DECL_SENDPROP(bool,                 m_bDefensiveBomb);
};


class CTFFlameRocket : public CTFBaseRocket {};

class CTFProjectile_Syringe : public CTFBaseProjectile {};
class CTFProjectile_EnergyRing : public CTFBaseProjectile {};
class CTFProjectile_EnergyBall : public CTFBaseRocket {};

class CTFProjectile_SentryRocket : public CTFProjectile_Rocket {};
class CTFProjectile_MechanicalArmOrb : public CTFProjectile_Rocket {};

class CTFProjectile_HealingBolt : public CTFProjectile_Arrow {};
class CTFProjectile_GrapplingHook : public CTFProjectile_Arrow {};

class CTFWeaponBaseMerasmusGrenade : public CTFWeaponBaseGrenadeProj {};

class CTFProjectile_Jar : public CTFGrenadePipebombProjectile {};
class CTFProjectile_JarMilk : public CTFProjectile_Jar {};
class CTFProjectile_Cleaver : public CTFProjectile_Jar {};

class CTFProjectile_Throwable : public CTFProjectile_Jar {};
class CTFProjectile_ThrowableBreadMonster : public CTFProjectile_Throwable {};
class CTFProjectile_ThrowableBrick : public CTFProjectile_Throwable {};
class CTFProjectile_ThrowableRepel : public CTFProjectile_Throwable {};

class CTFStunBall : public CTFGrenadePipebombProjectile {};
class CTFBall_Ornament : public CTFStunBall {};

class CTFProjectile_SpellFireball : public CTFProjectile_Rocket {};
class CTFProjectile_SpellBats : public CTFProjectile_Jar {};
class CTFProjectile_SpellMirv : public CTFProjectile_SpellBats {};
class CTFProjectile_SpellMeteorShower : public CTFProjectile_SpellBats {};
class CTFProjectile_SpellPumpkin : public CTFProjectile_SpellBats {};
class CTFProjectile_SpellTransposeTeleport : public CTFProjectile_SpellBats {};
class CTFProjectile_SpellSpawnBoss : public CTFProjectile_SpellBats {};
class CTFProjectile_SpellSpawnZombie : public CTFProjectile_SpellBats {};
class CTFProjectile_SpellSpawnHorde : public CTFProjectile_SpellBats {};
class CTFProjectile_SpellLightningOrb : public CTFProjectile_SpellFireball {};
class CTFProjectile_SpellKartOrb : public CTFProjectile_SpellFireball {};
class CTFProjectile_SpellKartBats : public CTFProjectile_SpellBats {};


class IBaseProjectileAutoList
{
public:
	static const CUtlVector<IBaseProjectileAutoList *>& AutoList() { return m_IBaseProjectileAutoListAutoList; }
private:
	static GlobalThunk<CUtlVector<IBaseProjectileAutoList *>> m_IBaseProjectileAutoListAutoList;
};


#endif
