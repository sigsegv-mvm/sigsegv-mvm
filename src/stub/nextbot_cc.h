#ifndef _INCLUDE_SIGSEGV_STUB_NEXTBOT_CC_H_
#define _INCLUDE_SIGSEGV_STUB_NEXTBOT_CC_H_


#include "link/link.h"
#include "stub/baseplayer.h"


class IBody;
class CPathTrack;


class NextBotCombatCharacter : public CBaseCombatCharacter {};


class CTFBaseBoss : public NextBotCombatCharacter
{
public:
	void UpdateCollisionBounds() { vt_UpdateCollisionBounds(this); }
	
private:
	static MemberVFuncThunk<CTFBaseBoss *, void> vt_UpdateCollisionBounds;
};

class CTFTankBoss : public CTFBaseBoss
{
public:
	DECL_EXTRACT (IBody *,             m_pBodyInterface);
	DECL_RELATIVE(CHandle<CPathTrack>, m_hCurrentNode);
	DECL_RELATIVE(CUtlVector<float>,   m_NodeDists);
	DECL_RELATIVE(float,               m_flTotalDistance);
	DECL_RELATIVE(int,                 m_iCurrentNode);
	DECL_RELATIVE(int,                 m_iModelIndex);
};


class CHalloweenBaseBoss : public NextBotCombatCharacter
{
public:
	enum HalloweenBossType : int32_t
	{
		INVALID         = 0,
		HEADLESS_HATMAN = 1,
		EYEBALL_BOSS    = 2,
		MERASMUS        = 3,
	};
	
	static CHalloweenBaseBoss *SpawnBossAtPos(HalloweenBossType type, const Vector& pos, int team, CBaseEntity *owner) { return ft_SpawnBossAtPos(type, pos, team, owner); }
	
private:
	static StaticFuncThunk<CHalloweenBaseBoss *, HalloweenBossType, const Vector&, int, CBaseEntity *> ft_SpawnBossAtPos;
};

class CHeadlessHatman : public CHalloweenBaseBoss {};
class CMerasmus       : public CHalloweenBaseBoss {};
class CEyeballBoss    : public CHalloweenBaseBoss {};


/*
class CZombie : public NextBotCombatCharacter
{
public:
	enum SkeletonType_t : int32_t
	{
		SKELETON_NORMAL = 0,
		SKELETON_KING   = 1,
		SKELETON_SMALL  = 2,
	};
	
	#warning TODO: CZombie
	
	// TODO
	
private:
	// TODO
};
*/


#endif
