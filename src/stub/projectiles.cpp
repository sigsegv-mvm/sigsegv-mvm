#include "stub/projectiles.h"
#include "mem/extract.h"


#if defined _LINUX

static constexpr uint8_t s_Buf_CTFProjectile_Arrow_ArrowTouch[] = {
	0x55,                                           // +0000  push ebp
	0x89, 0xe5,                                     // +0001  mov ebp,esp
	0x57,                                           // +0003  push edi
	0x56,                                           // +0004  push esi
	0x53,                                           // +0005  push ebx
	0x81, 0xec, 0xbc, 0x01, 0x00, 0x00,             // +0006  sub esp,0x1bc
	0xa1, 0x30, 0x98, 0x77, 0x01,                   // +000C  mov eax,ds:gpGlobals
	0x8b, 0x5d, 0x08,                               // +0011  mov ebx,[ebp+this]
	0x8b, 0x7d, 0x0c,                               // +0014  mov edi,[ebp+arg_4]
	0xf3, 0x0f, 0x10, 0x40, 0x0c,                   // +0017  movss xmm0,dword ptr [eax+0xc]
	0xf3, 0x0f, 0x5c, 0x83, 0x10, 0x05, 0x00, 0x00, // +001C  subss xmm0,dword ptr [ebx+0xVVVVVVVV]
	0x0f, 0x2f, 0x05, 0xe4, 0x51, 0x1a, 0x01,       // +0024  comiss xmm0,ds:0xXXXXXXXX
	0x0f, 0x83, 0x9f, 0x03, 0x00, 0x00,             // +002B  jnb +0xXXXXXXXX
};

struct CExtract_CTFProjectile_Arrow_ArrowTouch : public IExtract<float *>
{
	using T = float *;
	
	CExtract_CTFProjectile_Arrow_ArrowTouch() : IExtract<T>(sizeof(s_Buf_CTFProjectile_Arrow_ArrowTouch)) {}
	
	virtual bool GetExtractInfo(ByteBuf& buf, ByteBuf& mask) const override
	{
		buf.CopyFrom(s_Buf_CTFProjectile_Arrow_ArrowTouch);
		
		buf.SetDword(0x0c + 1, (uint32_t)AddrManager::GetAddr("gpGlobals"));
		
		mask.SetRange(0x1c + 4, 4, 0x00);
		mask.SetRange(0x24 + 3, 4, 0x00);
		mask.SetRange(0x2b + 2, 4, 0x00);
		
		return true;
	}
	
	virtual const char *GetFuncName() const override   { return "CTFProjectile_Arrow::ArrowTouch"; }
	virtual uint32_t GetFuncOffMin() const override    { return 0x0000; }
	virtual uint32_t GetFuncOffMax() const override    { return 0x0000; }
	virtual uint32_t GetExtractOffset() const override { return 0x001c + 4; }
};

#elif defined _WINDOWS

using CExtract_CTFProjectile_Arrow_ArrowTouch = IExtractStub;

#endif


IMPL_SENDPROP(CHandle<CBaseEntity>, CBaseProjectile, m_hOriginalLauncher, CBaseProjectile);

MemberVFuncThunk<      CBaseProjectile *, void, CBaseEntity *> CBaseProjectile::vt_SetLauncher      (TypeName<CBaseProjectile>(), "CBaseProjectile::SetLauncher");
MemberVFuncThunk<const CBaseProjectile *, int>                 CBaseProjectile::vt_GetProjectileType(TypeName<CBaseProjectile>(), "CBaseProjectile::GetProjectileType");


IMPL_SENDPROP(Vector,               CTFBaseProjectile, m_vInitialVelocity, CTFBaseProjectile);
IMPL_SENDPROP(CHandle<CBaseEntity>, CTFBaseProjectile, m_hLauncher,        CTFBaseProjectile);


IMPL_SENDPROP(Vector,               CTFBaseRocket, m_vInitialVelocity, CTFBaseRocket);
IMPL_SENDPROP(int,                  CTFBaseRocket, m_iDeflected,       CTFBaseRocket);
IMPL_SENDPROP(CHandle<CBaseEntity>, CTFBaseRocket, m_hLauncher,        CTFBaseRocket);

MemberFuncThunk<const CTFBaseRocket *, CBasePlayer *> CTFBaseRocket::ft_GetOwnerPlayer("CTFBaseRocket::GetOwnerPlayer");


IMPL_SENDPROP(bool, CTFProjectile_Rocket, m_bCritical, CTFProjectile_Rocket);


IMPL_SENDPROP(bool, CTFProjectile_Flare, m_bCritical, CTFProjectile_Flare);


IMPL_EXTRACT (float, CTFProjectile_Arrow, m_flTimeInit,      new CExtract_CTFProjectile_Arrow_ArrowTouch());
IMPL_SENDPROP(bool,  CTFProjectile_Arrow, m_bArrowAlight,    CTFProjectile_Arrow);
IMPL_SENDPROP(bool,  CTFProjectile_Arrow, m_bCritical,       CTFProjectile_Arrow);
IMPL_SENDPROP(int,   CTFProjectile_Arrow, m_iProjectileType, CTFProjectile_Arrow);


IMPL_SENDPROP(Vector,               CTFWeaponBaseGrenadeProj, m_vInitialVelocity, CTFWeaponBaseGrenadeProj);
IMPL_SENDPROP(int,                  CTFWeaponBaseGrenadeProj, m_iDeflected,       CTFWeaponBaseGrenadeProj);
IMPL_SENDPROP(CHandle<CBaseEntity>, CTFWeaponBaseGrenadeProj, m_hDeflectOwner,    CTFWeaponBaseGrenadeProj);
IMPL_SENDPROP(bool,                 CTFWeaponBaseGrenadeProj, m_bCritical,        CTFWeaponBaseGrenadeProj);

MemberVFuncThunk<const CTFWeaponBaseGrenadeProj *, int> CTFWeaponBaseGrenadeProj::vt_GetWeaponID(TypeName<CTFWeaponBaseGrenadeProj>(), "CTFWeaponBaseGrenadeProj::GetWeaponID");


IMPL_SENDPROP(CHandle<CBaseEntity>, CTFGrenadePipebombProjectile, m_hLauncher,      CTFGrenadePipebombProjectile);
IMPL_SENDPROP(bool,                 CTFGrenadePipebombProjectile, m_bTouched,       CTFGrenadePipebombProjectile);
IMPL_SENDPROP(int,                  CTFGrenadePipebombProjectile, m_iType,          CTFGrenadePipebombProjectile);
IMPL_SENDPROP(bool,                 CTFGrenadePipebombProjectile, m_bDefensiveBomb, CTFGrenadePipebombProjectile);


GlobalThunk<CUtlVector<IBaseProjectileAutoList *>> IBaseProjectileAutoList::m_IBaseProjectileAutoListAutoList("IBaseProjectileAutoList::m_IBaseProjectileAutoListAutoList");


#if 0

enum
{
	TF_AMMO_DUMMY     = 0,
	TF_AMMO_PRIMARY   = 1,
	TF_AMMO_SECONDARY = 2,
	TF_AMMO_METAL     = 3,
	TF_AMMO_GRENADES1 = 4,
	TF_AMMO_GRENADES2 = 5,
	TF_AMMO_GRENADES3 = 6,
	
	TF_AMMO_COUNT,
};

const char *GetAmmoName(int)


// from game/shared/imovehelper.h... do we just include that? should we?
enum
{
	WL_NotInWater=0,
	WL_Feet,
	WL_Waist,
	WL_Eyes
};


// from game/shared/weapon_parse.h... do we include that already??
enum WeaponSound_t
{
	EMPTY           =  0,
	SINGLE          =  1,
	SINGLE_NPC      =  2,
	WPN_DOUBLE      =  3,
	DOUBLE_NPC      =  4,
	BURST           =  5,
	RELOAD          =  6,
	RELOAD_NPC      =  7,
	MELEE_MISS      =  8,
	MELEE_HIT       =  9,
	MELEE_HIT_WORLD = 10,
	SPECIAL1        = 11,
	SPECIAL2        = 12,
	SPECIAL3        = 13,
	TAUNT           = 14,
	DEPLOY          = 15,
	
	NUM_SHOOT_SOUND_TYPES,
};


// from game/shared/shareddefs.h... do we include that already??
enum PLAYER_ANIM
{
	PLAYER_IDLE,
	PLAYER_WALK,
	PLAYER_JUMP,
	PLAYER_SUPERJUMP,
	PLAYER_DIE,
	PLAYER_ATTACK1,
	PLAYER_IN_VEHICLE,

	// TF Player animations
	PLAYER_RELOAD,
	PLAYER_START_AIMING,
	PLAYER_LEAVE_AIMING,
};


// enum Collision_Group_t
// from public/const.h


#endif
