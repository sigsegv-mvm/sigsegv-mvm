#include "stub/projectiles.h"


#if defined _LINUX

static constexpr uint8_t s_Buf_CTFProjectile_Arrow_ArrowTouch[] = {
	0x55,                                           // +0000  push ebp
	0x89, 0xe5,                                     // +0001  mov ebp,esp
	0x83, 0xec, 0x18,                               // +0003  sub esp,0x18
	0xa1, 0xf0, 0xed, 0x63, 0x01,                   // +0006  mov eax,ds:gpGlobals
	0x89, 0x5d, 0xf8,                               // +000B  mov [ebp-0x8],ebx
	0x8b, 0x5d, 0x08,                               // +000E  mov ebx,[ebp+this]
	0x89, 0x75, 0xfc,                               // +0011  mov [ebp-0x4],esi
	0x8b, 0x75, 0x0c,                               // +0014  mov esi,[ebp+arg_4]
	0xf3, 0x0f, 0x10, 0x40, 0x0c,                   // +0017  movss xmm0,dword ptr [eax+0xc]
	0xf3, 0x0f, 0x5c, 0x83, 0x10, 0x05, 0x00, 0x00, // +001C  subss xmm0,dword ptr [ebx+0xVVVVVVVV]
	0x0f, 0x2f, 0x05, 0x1c, 0xfd, 0x06, 0x01,       // +0024  comiss xmm0,ds:0xXXXXXXXX
	0x73, 0x43,                                     // +002B  jnb +0xXX
};

struct CExtract_CTFProjectile_Arrow_ArrowTouch : public IExtract<float *>
{
	using T = float *;
	
	CExtract_CTFProjectile_Arrow_ArrowTouch() : IExtract<T>(sizeof(s_Buf_CTFProjectile_Arrow_ArrowTouch)) {}
	
	virtual bool GetExtractInfo(ByteBuf& buf, ByteBuf& mask) const override
	{
		buf.CopyFrom(s_Buf_CTFProjectile_Arrow_ArrowTouch);
		
		buf.SetDword(0x06 + 1, (uint32_t)AddrManager::GetAddr("gpGlobals"));
		
		mask.SetRange(0x1c + 4, 4, 0x00);
		mask.SetRange(0x24 + 3, 4, 0x00);
		mask.SetRange(0x2b + 1, 1, 0x00);
		
		return true;
	}
	
	virtual const char *GetFuncName() const override   { return "CTFProjectile_Arrow::ArrowTouch"; }
	virtual uint32_t GetFuncOffMin() const override    { return 0x0000; }
	virtual uint32_t GetFuncOffMax() const override    { return 0x0000; }
	virtual uint32_t GetExtractOffset() const override { return 0x001c + 4; }
};

#elif defined _WINDOWS

// TODO: actually implement this
struct CExtract_CTFProjectile_Arrow_ArrowTouch : public IExtract<float *>
{
	CExtract_CTFProjectile_Arrow_ArrowTouch() : IExtract<float *>(1) {}
	
	virtual bool GetExtractInfo(ByteBuf& buf, ByteBuf& mask) const override { return false; }
	
	virtual const char *GetFuncName() const override   { return "CTFProjectile_Arrow::ArrowTouch"; }
	virtual uint32_t GetFuncOffMin() const override    { return 0x0000; }
	virtual uint32_t GetFuncOffMax() const override    { return 0x0000; }
	virtual uint32_t GetExtractOffset() const override { return 0x0000; }
};

#endif


IMPL_SENDPROP(CHandle<CBaseEntity>, CBaseProjectile, m_hOriginalLauncher, CBaseProjectile);

MemberVFuncThunk<const CBaseProjectile *, int> CBaseProjectile::vt_GetProjectileType(TypeName<CBaseProjectile>(), "CBaseProjectile::GetProjectileType");


IMPL_SENDPROP(int,    CBaseGrenade, m_iDeflected,       CBaseGrenade);
IMPL_SENDPROP(bool,   CBaseGrenade, m_bCritical,        CBaseGrenade);
IMPL_SENDPROP(Vector, CBaseGrenade, m_vInitialVelocity, CBaseGrenade);


IMPL_SENDPROP(Vector,               CTFBaseRocket, m_vInitialVelocity, CTFBaseRocket);
IMPL_SENDPROP(int,                  CTFBaseRocket, m_iDeflected,       CTFBaseRocket);
IMPL_SENDPROP(CHandle<CBaseEntity>, CTFBaseRocket, m_hLauncher,        CTFBaseRocket);

MemberFuncThunk<const CTFBaseRocket *, CBasePlayer *> CTFBaseRocket::ft_GetOwnerPlayer("CTFBaseRocket::GetOwnerPlayer");


IMPL_EXTRACT(float, CTFProjectile_Arrow, m_flTimeInit, new CExtract_CTFProjectile_Arrow_ArrowTouch());


IMPL_SENDPROP(CHandle<CBaseEntity>, CTFWeaponBaseGrenadeProj, m_hLauncher, CTFWeaponBaseGrenadeProj);

MemberVFuncThunk<const CTFWeaponBaseGrenadeProj *, int> CTFWeaponBaseGrenadeProj::vt_GetWeaponID(TypeName<CTFWeaponBaseGrenadeProj>(), "CTFWeaponBaseGrenadeProj::GetWeaponID");


GlobalThunk<CUtlVector<IBaseProjectileAutoList *>> IBaseProjectileAutoList::m_IBaseProjectileAutoListAutoList("IBaseProjectileAutoList::m_IBaseProjectileAutoListAutoList");
