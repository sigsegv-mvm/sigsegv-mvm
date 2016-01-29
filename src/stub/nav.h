#ifndef _INCLUDE_SIGSEGV_STUB_NAV_H_
#define _INCLUDE_SIGSEGV_STUB_NAV_H_


#include "prop.h"
#include "link/link.h"


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


class CNavArea {};

class CTFNavArea : public CNavArea
{
public:
	TFNavAttributeType GetTFAttributes() const
	{
		return this->m_nAttributes;
	}
	
private:
	DEF_EXTRACT(TFNavAttributeType, m_nAttributes);
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
