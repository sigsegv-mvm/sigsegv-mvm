#ifndef _INCLUDE_SIGSEGV_STUB_NAV_H_
#define _INCLUDE_SIGSEGV_STUB_NAV_H_


#include "prop.h"
#include "link/link.h"
#include "stub/tf_shareddefs.h"


class CBaseObject;
class CTFBotPathCost;


enum NavErrorType {};


enum GetNavAreaFlags_t
{
	GETNAVAREA_CHECK_LOS           = 0x1,
	GETNAVAREA_ALLOW_BLOCKED_AREAS = 0x2,
	GETNAVAREA_CHECK_GROUND        = 0x4,
};


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


struct Extent
{
	Vector lo, hi;

	void Init( void )
	{
		lo.Init();
		hi.Init();
	}

	void Init( CBaseEntity *entity )
	{
		entity->CollisionProp()->WorldSpaceSurroundingBounds( &lo, &hi );
	}

	float SizeX( void ) const	{ return hi.x - lo.x; }
	float SizeY( void ) const	{ return hi.y - lo.y; }
	float SizeZ( void ) const	{ return hi.z - lo.z; }
	float Area( void ) const	{ return SizeX() * SizeY(); }

	// Increase bounds to contain the given point
	void Encompass( const Vector &pos )
	{
		for ( int i=0; i<3; ++i )
		{
			if ( pos[i] < lo[i] )
			{
				lo[i] = pos[i];
			}
			else if ( pos[i] > hi[i] )
			{
				hi[i] = pos[i];
			}
		}
	}

	// Increase bounds to contain the given extent
	void Encompass( const Extent &extent )
	{
		Encompass( extent.lo );
		Encompass( extent.hi );
	}

	// return true if 'pos' is inside of this extent
	bool Contains( const Vector &pos ) const
	{
		return (pos.x >= lo.x && pos.x <= hi.x &&
				pos.y >= lo.y && pos.y <= hi.y &&
				pos.z >= lo.z && pos.z <= hi.z);
	}
	
	// return true if this extent overlaps the given one
	bool IsOverlapping( const Extent &other ) const
	{
		return (lo.x <= other.hi.x && hi.x >= other.lo.x &&
				lo.y <= other.hi.y && hi.y >= other.lo.y &&
				lo.z <= other.hi.z && hi.z >= other.lo.z);
	}

	// return true if this extent completely contains the given one
	bool IsEncompassing( const Extent &other, float tolerance = 0.0f ) const
	{
		return (lo.x <= other.lo.x + tolerance && hi.x >= other.hi.x - tolerance &&
				lo.y <= other.lo.y + tolerance && hi.y >= other.hi.y - tolerance &&
				lo.z <= other.lo.z + tolerance && hi.z >= other.hi.z - tolerance);
	}
};


class CNavArea
{
public:
	void GetExtent(Extent *extent) const                               { ft_GetExtent            (this, extent); }
	void GetClosestPointOnArea(const Vector& pos, Vector *close) const { ft_GetClosestPointOnArea(this, &pos, close); }
	
private:
	static MemberFuncThunk<const CNavArea *, void, Extent *>                 ft_GetExtent;
	static MemberFuncThunk<const CNavArea *, void, const Vector *, Vector *> ft_GetClosestPointOnArea;
};

class CTFNavArea : public CNavArea
{
public:
	TFNavAttributeType GetTFAttributes() const { return this->m_nAttributes; }
	
private:
	DECL_EXTRACT(TFNavAttributeType, m_nAttributes);
};


class CNavMesh
{
public:
	CNavArea *GetNearestNavArea(const Vector& pos, bool anyZ = false, float maxDist = 10000.0f, bool checkLOS = false, bool checkGround = true, int team = TEAM_ANY) const { return ft_GetNearestNavArea_vec(this, pos, anyZ, maxDist, checkLOS, checkGround, team); }
	CNavArea *GetNearestNavArea(CBaseEntity *pEntity, int nFlags = GETNAVAREA_CHECK_GROUND, float maxDist = 10000.0f) const                                                { return ft_GetNearestNavArea_ent(this, pEntity, nFlags, maxDist); }
	void CollectAreasOverlappingExtent(const Extent& extent, CUtlVector<CTFNavArea *> *outVector)                                                                          {        ft_CollectAreasOverlappingExtent_CTFNavArea(this, extent, outVector); }
	
private:
	static MemberFuncThunk<const CNavMesh *, CNavArea *, const Vector&, bool, float, bool, bool, int> ft_GetNearestNavArea_vec;
	static MemberFuncThunk<const CNavMesh *, CNavArea *, CBaseEntity *, int, float>                   ft_GetNearestNavArea_ent;
	static MemberFuncThunk<CNavMesh *, void, const Extent&, CUtlVector<CTFNavArea *> *> ft_CollectAreasOverlappingExtent_CTFNavArea;
};

class CTFNavMesh : public CNavMesh
{
public:
	void CollectBuiltObjects(CUtlVector<CBaseObject *> *objects, int i1) { ft_CollectBuiltObjects(this, objects, i1); }
	
private:
	static MemberFuncThunk<CTFNavMesh *, void, CUtlVector<CBaseObject *> *, int> ft_CollectBuiltObjects;
};


extern GlobalThunk<CTFNavMesh *> TheNavMesh;


extern StaticFuncThunk<float, CNavArea *, CNavArea *, CTFBotPathCost&, float> ft_NavAreaTravelDistance_CTFBotPathCost;
template<typename CostFunctor> inline float NavAreaTravelDistance(CNavArea *startArea, CNavArea *endArea, CostFunctor& costFunc, float maxPathLength = 0.0f);
template<> inline float NavAreaTravelDistance<CTFBotPathCost>(CNavArea *startArea, CNavArea *endArea, CTFBotPathCost& costFunc, float maxPathLength)
{
	return ft_NavAreaTravelDistance_CTFBotPathCost(startArea, endArea, costFunc, maxPathLength);
}


#endif
