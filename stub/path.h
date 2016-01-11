#ifndef _INCLUDE_SIGSEGV_STUB_PATH_H_
#define _INCLUDE_SIGSEGV_STUB_PATH_H_


#include "link/link.h"


class INextBot;
class CBaseCombatCharacter;
class CTFBotPathCost;


class Path
{
public:
	template<class PathCost> bool Compute(INextBot *nextbot, CBaseCombatCharacter *who, PathCost& cost_func, float maxPathLength, bool b1);
	
private:
	static FuncThunk<bool (*)(Path *, INextBot *, CBaseCombatCharacter *, CTFBotPathCost&, float, bool)> ft_Compute_CTFBotPathCost;
};

class PathFollower : public Path
{
public:
	void Update(INextBot *nextbot)         { (*ft_Update)                 (this, nextbot); }
	void SetMinLookAheadDistance(float f1) { (*ft_SetMinLookAheadDistance)(this, f1);      }
	
private:
	static FuncThunk<void (*)(PathFollower *, INextBot *)> ft_Update;
	static FuncThunk<void (*)(PathFollower *, float)>      ft_SetMinLookAheadDistance;
};

class ChasePath : public PathFollower
{
public:
	
	
private:
	
};


template<> inline bool Path::Compute<CTFBotPathCost>(INextBot *nextbot, CBaseCombatCharacter *who, CTFBotPathCost& cost_func, float maxPathLength, bool b1)
{ return (*ft_Compute_CTFBotPathCost)(this, nextbot, who, cost_func, maxPathLength, b1); }


#endif
