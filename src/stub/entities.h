#ifndef _INCLUDE_SIGSEGV_STUB_ENTITIES_H_
#define _INCLUDE_SIGSEGV_STUB_ENTITIES_H_


#include "link/link.h"
#include "prop.h"
#include "stub/baseplayer.h"
#include "stub/tf_shareddefs.h"


class CPointEntity : public CBaseEntity {};

class CPathTrack : public CPointEntity
{
public:
	CPathTrack *GetNext() { return ft_GetNext(this); }
	
private:
	static MemberFuncThunk<CPathTrack *, CPathTrack *> ft_GetNext;
};


class CItem : public CBaseAnimating {};

class CTFPowerup : public CItem
{
public:
	float GetLifeTime() { return vt_GetLifeTime(this); }
	
private:
	static MemberVFuncThunk<CTFPowerup *, float> vt_GetLifeTime;
};

class CSpellPickup : public CTFPowerup
{
public:
	DECL_DATAMAP(int, m_nTier);
};


class CTFReviveMarker : public CBaseAnimating
{
public:
	DECL_SENDPROP(CHandle<CBaseEntity>, m_hOwner);   // +0x488
	// 48c
	DECL_SENDPROP(short,                m_nRevives); // +0x490
	// 494 dword 0
	// 498 byte 0
	// 499 byte
	// 49a byte 0
	// 49b?
	// 49c float 0.0f
	// 4a0 byte, probably: have we landed on the ground
};


class CTFMedigunShield : public CBaseAnimating {};


class CEconWearable : public CEconEntity {};
class CTFWearable : public CEconWearable {};
class CTFPowerupBottle : public CTFWearable {};


class CBaseTFBotHintEntity : public CPointEntity {};
class CTFBotHintSentrygun : public CBaseTFBotHintEntity {};
class CTFBotHintTeleporterExit : public CBaseTFBotHintEntity {};

class CTFBotHintEngineerNest : public CBaseTFBotHintEntity
{
public:
	DECL_SENDPROP(bool, m_bHasActiveTeleporter);
};


class ITFBotHintEntityAutoList
{
public:
	static const CUtlVector<ITFBotHintEntityAutoList *>& AutoList() { return m_ITFBotHintEntityAutoListAutoList; }
private:
	static GlobalThunk<CUtlVector<ITFBotHintEntityAutoList *>> m_ITFBotHintEntityAutoListAutoList;
};


class CBaseProp : public CBaseAnimating {};
class CBreakableProp : public CBaseProp {};
class CDynamicProp : public CBreakableProp {};
class CTFItem : public CDynamicProp {};

class CCaptureFlag : public CTFItem
{
public:
	bool IsDropped()        { return (this->m_nFlagStatus == TF_FLAGINFO_DROPPED); }
	bool IsHome()           { return (this->m_nFlagStatus == TF_FLAGINFO_NONE); }
	bool IsStolen()         { return (this->m_nFlagStatus == TF_FLAGINFO_STOLEN); }
	bool IsDisabled() const { return this->m_bDisabled; }
	
private:
	DECL_SENDPROP(bool, m_bDisabled);
	DECL_SENDPROP(int,  m_nFlagStatus);
};


class ICaptureFlagAutoList
{
public:
	static const CUtlVector<ICaptureFlagAutoList *>& AutoList() { return m_ICaptureFlagAutoListAutoList; }
private:
	static GlobalThunk<CUtlVector<ICaptureFlagAutoList *>> m_ICaptureFlagAutoListAutoList;
};


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
	DECL_EXTRACT(CHandle<CPathTrack>, m_hCurrentNode);
	DECL_EXTRACT(CUtlVector<float>,   m_NodeDists);
	DECL_EXTRACT(float,               m_flTotalDistance);
	DECL_EXTRACT(int,                 m_iCurrentNode);
};


class CBaseToggle : public CBaseEntity {};

class CBaseTrigger : public CBaseToggle
{
public:
	DECL_DATAMAP(bool, m_bDisabled);
};

class CUpgrades : public CBaseTrigger
{
public:
	const char *GetUpgradeAttributeName(int index) const { return ft_GetUpgradeAttributeName(this, index); }
	
private:
	static MemberFuncThunk<const CUpgrades *, const char *, int> ft_GetUpgradeAttributeName;
};
extern GlobalThunk<CHandle<CUpgrades>> g_hUpgradeEntity;


class CCurrencyPack : public CTFPowerup
{
public:
	bool IsDistributed() const { return this->m_bDistributed; }
	int GetAmount() const      { return this->m_nAmount; }
	
private:
	DECL_SENDPROP(bool, m_bDistributed);
	DECL_EXTRACT (int,  m_nAmount);
};


class ICurrencyPackAutoList
{
public:
	static const CUtlVector<ICurrencyPackAutoList *>& AutoList() { return m_ICurrencyPackAutoListAutoList; }
private:
	static GlobalThunk<CUtlVector<ICurrencyPackAutoList *>> m_ICurrencyPackAutoListAutoList;
};


class CCaptureZone : public CBaseTrigger {};


class ICaptureZoneAutoList
{
public:
	static const CUtlVector<ICaptureZoneAutoList *>& AutoList() { return m_ICaptureZoneAutoListAutoList; }
private:
	static GlobalThunk<CUtlVector<ICaptureZoneAutoList *>> m_ICaptureZoneAutoListAutoList;
};


class CTeamControlPoint : public CBaseAnimating
{
public:
	
};


class CTeamControlPointMaster : public CBaseEntity
{
public:
	using ControlPointMap = CUtlMap<int, CTeamControlPoint *>;
	DECL_EXTRACT(ControlPointMap, m_ControlPoints);
};
extern GlobalThunk<CUtlVector<CHandle<CTeamControlPointMaster>>> g_hControlPointMasters;


class CTFFlameEntity : public CBaseEntity {};

class ITFFlameEntityAutoList
{
public:
	static const CUtlVector<ITFFlameEntityAutoList *>& AutoList() { return m_ITFFlameEntityAutoListAutoList; }
private:
	static GlobalThunk<CUtlVector<ITFFlameEntityAutoList *>> m_ITFFlameEntityAutoListAutoList;
};


// 20151007a

// CTFPlayer::Event_Killed
// - CTFReviveMarker::Create
//   - CBaseEntity::Create
//     - ctor
//       - m_nRevives = 0
//     - CTFReviveMarker::Spawn
//       - m_iHealth = 1
//       - set model, solid, collision, move, effects, sequence, think, etc
//   - SetOwner
//     - m_hOwner = player
//     - change team to match player
//     - set bodygroup based on player class
//     - set angles to match player
//   - change team to match player
// - player->handle_0x2974 = marker

// CTFReviveMarker::ReviveThink (100 ms interval)
// - if m_hOwner == null: UTIL_Remove(this)
// - if m_hOwner not in same team: UTIL_Remove(this)
// - if m_iMaxHealth == 0:
//   - maxhealth = (float)(player->GetMaxHealth() / 2)
//     - TODO: does CTFPlayer::GetMaxHealth factor in items and stuff?
//   - if (stats = CTF_GameStats.FindPlayerStats(player)) != null:
//     - m_nRevives = (probably round stat TFSTAT_DEATHS)
//     - maxhealth += 10 * old value of m_nRevives
//   - m_iMaxHealth = (int)maxhealth
// - if on the ground and haven't been yet
//   - set the bool for that (+0x4a0)
//   - if landed in a trigger_hurt: UTIL_Remove(this)
// - if m_nRevives > 7:   DispatchParticleEffect("speech_revivecall_hard")
// - elif m_nRevives > 3: DispatchParticleEffect("speech_revivecall_medium")
// - else:                DispatchParticleEffect("speech_revivecall")
// - TODO: stuff related to "revive_player_stopped"

// CTFReviveMarker::AddMarkerHealth
// - 


#endif
