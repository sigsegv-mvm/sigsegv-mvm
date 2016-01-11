#ifndef _INCLUDE_SIGSEGV_STUB_NAV_H_
#define _INCLUDE_SIGSEGV_STUB_NAV_H_


#include "prop/prop.h"
#include "link/link.h"
#include "extract.h"


class CBaseObject;


enum NavErrorType {};


/* from mvm-reversed/server/tf/nav_mesh/tf_nav_area.h */
enum TFNavAttributeType : int
{
	BLOCKED                     = (1 <<  0),
	
	RED_SPAWN_ROOM              = (1 <<  1),
	BLUE_SPAWN_ROOM             = (1 <<  2),
	SPAWN_ROOM_EXIT             = (1 <<  3),
	
	AMMO                        = (1 <<  4),
	HEALTH                      = (1 <<  5),
	
	CONTROL_POINT               = (1 <<  6),
	
	BLUE_SENTRY                 = (1 <<  7),
	RED_SENTRY                  = (1 <<  8),
	
	/* bit  9: unused */
	/* bit 10: unused */
	
	BLUE_SETUP_GATE             = (1 << 11),
	RED_SETUP_GATE              = (1 << 12),
	
	BLOCKED_AFTER_POINT_CAPTURE = (1 << 13),
	BLOCKED_UNTIL_POINT_CAPTURE = (1 << 14),
	
	BLUE_ONE_WAY_DOOR           = (1 << 15),
	RED_ONE_WAY_DOOR            = (1 << 16),
	
	WITH_SECOND_POINT           = (1 << 17),
	WITH_THIRD_POINT            = (1 << 18),
	WITH_FOURTH_POINT           = (1 << 19),
	WITH_FIFTH_POINT            = (1 << 20),
	
	SNIPER_SPOT                 = (1 << 21),
	SENTRY_SPOT                 = (1 << 22),
	
	/* bit 23: unused */
	/* bit 24: unused */
	
	NO_SPAWNING                 = (1 << 25),
	RESCUE_CLOSET               = (1 << 26),
	BOMB_DROP                   = (1 << 27),
	DOOR_NEVER_BLOCKS           = (1 << 28),
	DOOR_ALWAYS_BLOCKS          = (1 << 29),
	UNBLOCKABLE                 = (1 << 30),
	
	/* bit 31: unused */
};


#if defined _LINUX

static constexpr uint8_t s_Buf_CTFNavArea_m_nAttributes[] = {
	0x55,                                                       // +0000  push ebp
	0x89, 0xe5,                                                 // +0001  mov ebp,esp
	0x8b, 0x45, 0x08,                                           // +0003  mov eax,[ebp+this]
	0x5d,                                                       // +0006  pop ebp
	0xf7, 0x80, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x06, // +0007  test DWORD PTR [eax+0xVVVVVVVV],0x06000007
	0x0f, 0x94, 0xc0,                                           // +0011  setz al
	0xc3,                                                       // +0014  ret
};

struct CExtract_CTFNavArea_m_nAttributes : public IExtract<TFNavAttributeType>
{
	CExtract_CTFNavArea_m_nAttributes() : IExtract<TFNavAttributeType>(sizeof(s_Buf_CTFNavArea_m_nAttributes)) {}
	
	virtual void GetExtractInfo(ByteBuf& buf, ByteBuf& mask) const override
	{
		buf.CopyFrom(s_Buf_CTFNavArea_m_nAttributes);
		
		mask.SetRange(0x07 + 2, 4, 0x00);
	}
	
	virtual const char *GetFuncName() const override   { return "CTFNavArea::IsValidForWanderingPopulation"; }
	virtual uint32_t GetFuncOffset() const override    { return 0x0000; }
	virtual uint32_t GetExtractOffset() const override { return 0x0007 + 2; }
};

#elif defined _WIN32



#endif


class CNavArea {};

class CTFNavArea : public CNavArea
{
public:
	TFNavAttributeType GetTFAttributes() const
	{
		return this->m_nAttributes;
	}
	
private:
	PROP_STR(CTFNavArea);
	
	PROP_EXTRACT(TFNavAttributeType, CTFNavArea, m_nAttributes, CExtract_CTFNavArea_m_nAttributes);
};


class CNavMesh {};

class CTFNavMesh : public CNavMesh
{
public:
	void CollectBuiltObjects(CUtlVector<CBaseObject *> *objects, int i1) {}
	
private:
	static FuncThunk<void (*)(CTFNavMesh *, CUtlVector<CBaseObject *> *, int)> ft_CollectBuiltObjects;
};


extern GlobalThunk<CTFNavMesh *> TheNavMesh;


#endif
