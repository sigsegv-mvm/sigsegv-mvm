#ifndef _INCLUDE_SIGSEGV_RE_PATH_H_
#define _INCLUDE_SIGSEGV_RE_PATH_H_


#if defined _WIN32
#pragma message("Path classes have not been checked against the VC++ build!")
#endif


#include "stub/stub.h"
#include "util/util.h"


class CBaseObject;
class CNavLadder;
class CFuncElevator;


/* from game/server/nav_pathfind.h */
enum RouteType
{
	DEFAULT_ROUTE,
	FASTEST_ROUTE,
	SAFEST_ROUTE,
	RETREAT_ROUTE,
};


class IPathCost
{
public:
	virtual float operator()(CNavArea *area1, CNavArea *area2, const CNavLadder *ladder, const CFuncElevator *elevator, float f1) const = 0;
};
SIZE_CHECK(IPathCost, 0x04);


class CTFBotPathCost : public IPathCost
{
public:
	CTFBotPathCost(CTFBot *actor, RouteType rtype);
	~CTFBotPathCost();
	
	virtual float operator()(CNavArea *area1, CNavArea *area2, const CNavLadder *ladder, const CFuncElevator *elevator, float f1) const override;
	
private:
	CTFBot *m_Actor;                          // +0x04
	RouteType m_iRouteType;                   // +0x08
	float m_flStepHeight;                     // +0x0c
	float m_flMaxJumpHeight;                  // +0x10
	float m_flDeathDropHeight;                // +0x14
	CUtlVector<CBaseObject *> m_EnemyObjects; // +0x18
};
SIZE_CHECK(CTFBotPathCost, 0x2c);


inline CTFBotPathCost::CTFBotPathCost(CTFBot *actor, RouteType rtype)
{
	this->m_flStepHeight      = actor->GetLocomotionInterface()->GetStepHeight();
	this->m_flMaxJumpHeight   = actor->GetLocomotionInterface()->GetMaxJumpHeight();
	this->m_flDeathDropHeight = actor->GetLocomotionInterface()->GetDeathDropHeight();
	
	if (actor->IsPlayerClass(TF_CLASS_SPY)) {
		TheNavMesh->CollectBuiltObjects(&this->m_EnemyObjects, GetEnemyTeam(actor));
	} else {
		this->m_EnemyObjects.RemoveAll();
	}
}

inline CTFBotPathCost::~CTFBotPathCost() {}


#endif
