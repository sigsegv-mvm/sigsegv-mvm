#ifndef _INCLUDE_SIGSEGV_STUB_TFBOT_H_
#define _INCLUDE_SIGSEGV_STUB_TFBOT_H_


#include "prop.h"
#include "stub/tfplayer.h"
#include "stub/tfweaponbase.h"
#include "util/misc.h"
#include "re/nextbot.h"


enum class ExtAttr : int
{
	ALWAYS_FIRE_WEAPON_ALT = 0,
	TARGET_STICKIES        = 1,
	
	// wishlist:
	// taunt after every kill
};


class ILocomotion;
class IBody;
class IVision;
class IIntention;


template<typename T>
class NextBotPlayer : public T
{
public:
	void PressFireButton(float duration = -1.0f)        { vt_PressFireButton         (this, duration);      }
	void ReleaseFireButton()                            { vt_ReleaseFireButton       (this);                }
	void PressAltFireButton(float duration = -1.0f)     { vt_PressAltFireButton      (this, duration);      }
	void ReleaseAltFireButton()                         { vt_ReleaseAltFireButton    (this);                }
	void PressMeleeButton(float duration = -1.0f)       { vt_PressMeleeButton        (this, duration);      }
	void ReleaseMeleeButton()                           { vt_ReleaseMeleeButton      (this);                }
	void PressSpecialFireButton(float duration = -1.0f) { vt_PressSpecialFireButton  (this, duration);      }
	void ReleaseSpecialFireButton()                     { vt_ReleaseSpecialFireButton(this);                }
	void PressUseButton(float duration = -1.0f)         { vt_PressUseButton          (this, duration);      }
	void ReleaseUseButton()                             { vt_ReleaseUseButton        (this);                }
	void PressReloadButton(float duration = -1.0f)      { vt_PressReloadButton       (this, duration);      }
	void ReleaseReloadButton()                          { vt_ReleaseReloadButton     (this);                }
	void PressForwardButton(float duration = -1.0f)     { vt_PressForwardButton      (this, duration);      }
	void ReleaseForwardButton()                         { vt_ReleaseForwardButton    (this);                }
	void PressBackwardButton(float duration = -1.0f)    { vt_PressBackwardButton     (this, duration);      }
	void ReleaseBackwardButton()                        { vt_ReleaseBackwardButton   (this);                }
	void PressLeftButton(float duration = -1.0f)        { vt_PressLeftButton         (this, duration);      }
	void ReleaseLeftButton()                            { vt_ReleaseLeftButton       (this);                }
	void PressRightButton(float duration = -1.0f)       { vt_PressRightButton        (this, duration);      }
	void ReleaseRightButton()                           { vt_ReleaseRightButton      (this);                }
	void PressJumpButton(float duration = -1.0f)        { vt_PressJumpButton         (this, duration);      }
	void ReleaseJumpButton()                            { vt_ReleaseJumpButton       (this);                }
	void PressCrouchButton(float duration = -1.0f)      { vt_PressCrouchButton       (this, duration);      }
	void ReleaseCrouchButton()                          { vt_ReleaseCrouchButton     (this);                }
	void PressWalkButton(float duration = -1.0f)        { vt_PressWalkButton         (this, duration);      }
	void ReleaseWalkButton()                            { vt_ReleaseWalkButton       (this);                }
	void SetButtonScale(float forward, float side)      { vt_SetButtonScale          (this, forward, side); }
	
private:
	static MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void, float       > vt_PressFireButton;
	static MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void              > vt_ReleaseFireButton;
	static MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void, float       > vt_PressAltFireButton;
	static MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void              > vt_ReleaseAltFireButton;
	static MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void, float       > vt_PressMeleeButton;
	static MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void              > vt_ReleaseMeleeButton;
	static MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void, float       > vt_PressSpecialFireButton;
	static MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void              > vt_ReleaseSpecialFireButton;
	static MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void, float       > vt_PressUseButton;
	static MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void              > vt_ReleaseUseButton;
	static MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void, float       > vt_PressReloadButton;
	static MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void              > vt_ReleaseReloadButton;
	static MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void, float       > vt_PressForwardButton;
	static MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void              > vt_ReleaseForwardButton;
	static MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void, float       > vt_PressBackwardButton;
	static MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void              > vt_ReleaseBackwardButton;
	static MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void, float       > vt_PressLeftButton;
	static MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void              > vt_ReleaseLeftButton;
	static MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void, float       > vt_PressRightButton;
	static MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void              > vt_ReleaseRightButton;
	static MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void, float       > vt_PressJumpButton;
	static MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void              > vt_ReleaseJumpButton;
	static MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void, float       > vt_PressCrouchButton;
	static MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void              > vt_ReleaseCrouchButton;
	static MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void, float       > vt_PressWalkButton;
	static MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void              > vt_ReleaseWalkButton;
	static MemberVFuncThunk<NextBotPlayer<CTFPlayer> *, void, float, float> vt_SetButtonScale;
};

class CTFBot : public NextBotPlayer<CTFPlayer>
{
public:
	enum AttributeType : int
	{
		ATTR_NONE                        = 0,
		
		ATTR_REMOVE_ON_DEATH             = (1 << 0),
		ATTR_AGGRESSIVE                  = (1 << 1),
		// 2?
		ATTR_SUPPRESS_FIRE               = (1 << 3),
		ATTR_DISABLE_DODGE               = (1 << 4),
		ATTR_BECOME_SPECTATOR_ON_DEATH   = (1 << 5),
		// 6?
		ATTR_RETAIN_BUILDINGS            = (1 << 7),
		ATTR_SPAWN_WITH_FULL_CHARGE      = (1 << 8),
		ATTR_ALWAYS_CRIT                 = (1 << 9),
		ATTR_IGNORE_ENEMIES              = (1 << 10),
		ATTR_HOLD_FIRE_UNTIL_FULL_RELOAD = (1 << 11),
		// 12?
		ATTR_ALWAYS_FIRE_WEAPON          = (1 << 13),
		ATTR_TELEPORT_TO_HINT            = (1 << 14),
		ATTR_MINI_BOSS                   = (1 << 15),
		ATTR_USE_BOSS_HEALTH_BAR         = (1 << 16),
		ATTR_IGNORE_FLAG                 = (1 << 17),
		ATTR_AUTO_JUMP                   = (1 << 18),
		ATTR_AIR_CHARGE_ONLY             = (1 << 19),
		ATTR_VACCINATOR_BULLETS          = (1 << 20),
		ATTR_VACCINATOR_BLAST            = (1 << 21),
		ATTR_VACCINATOR_FIRE             = (1 << 22),
		ATTR_BULLET_IMMUNE               = (1 << 23),
		ATTR_BLAST_IMMUNE                = (1 << 24),
		ATTR_FIRE_IMMUNE                 = (1 << 25),
	};
	
	enum MissionType : int
	{
		MISSION_NONE             = 0,
		// ?
		MISSION_DESTROY_SENTRIES = 2,
		MISSION_SNIPER           = 3,
		MISSION_SPY              = 4,
		MISSION_ENGINEER         = 5,
		MISSION_REPROGRAMMED     = 6,
	};
	
	class ExtendedAttr
	{
	public:
		ExtendedAttr& operator=(const ExtendedAttr&) = default;
		
		void Zero() { this->m_nBits = 0; }
		
		void Set(ExtAttr attr, bool on) { on ? this->TurnOn(attr) : this->TurnOff(attr); }
		void TurnOn(ExtAttr attr)       { this->m_nBits |=  (1U << (int)attr); }
		void TurnOff(ExtAttr attr)      { this->m_nBits &= ~(1U << (int)attr); }
		
		bool operator[](ExtAttr attr)       { return ((this->m_nBits & (1U << (int)attr)) != 0); }
		bool operator[](ExtAttr attr) const { return ((this->m_nBits & (1U << (int)attr)) != 0); }
		
		void Dump() const { DevMsg("CTFBot::ExtendedAttr::Dump %08x\n", this->m_nBits); }
		
	private:
		uint32_t m_nBits = 0;
	};
	SIZE_CHECK(ExtendedAttr, 0x4);
	
	MissionType GetMission() const { return this->m_nMission; }
	
	/* thunk */
	ILocomotion *GetLocomotionInterface() const                        { return ft_GetLocomotionInterface      (this); }
	IBody *GetBodyInterface() const                                    { return ft_GetBodyInterface            (this); }
	IVision *GetVisionInterface() const                                { return ft_GetVisionInterface          (this); }
	IIntention *GetIntentionInterface() const                          { return ft_GetIntentionInterface       (this); }
	float GetDesiredPathLookAheadRange() const                         { return ft_GetDesiredPathLookAheadRange(this); }
	void PushRequiredWeapon(CTFWeaponBase *weapon)                     { return ft_PushRequiredWeapon          (this, weapon); }
	void PopRequiredWeapon()                                           { return ft_PopRequiredWeapon           (this); }
	bool IsLineOfFireClear(const Vector& to) const                     { return ft_IsLineOfFireClear_vec       (this, to); }
	bool IsLineOfFireClear(CBaseEntity *to) const                      { return ft_IsLineOfFireClear_ent       (this, to); }
	bool IsLineOfFireClear(const Vector& from, const Vector& to) const { return ft_IsLineOfFireClear_vec_vec   (this, from, to); }
	bool IsLineOfFireClear(const Vector& from, CBaseEntity *to) const  { return ft_IsLineOfFireClear_vec_ent   (this, from, to); }
	
	/* custom: extended attributes */
	ExtendedAttr& ExtAttr()
	{
		CHandle<CTFBot> h_this = this;
		return s_ExtAttrs[h_this];
	}
	
private:
	DECL_EXTRACT(MissionType, m_nMission);
	
	static MemberFuncThunk<const CTFBot *, ILocomotion *                     > ft_GetLocomotionInterface;
	static MemberFuncThunk<const CTFBot *, IBody *                           > ft_GetBodyInterface;
	static MemberFuncThunk<const CTFBot *, IVision *                         > ft_GetVisionInterface;
	static MemberFuncThunk<const CTFBot *, IIntention *                      > ft_GetIntentionInterface;
	static MemberFuncThunk<const CTFBot *, float                             > ft_GetDesiredPathLookAheadRange;
	static MemberFuncThunk<      CTFBot *, void, CTFWeaponBase *             > ft_PushRequiredWeapon;
	static MemberFuncThunk<      CTFBot *, void                              > ft_PopRequiredWeapon;
	static MemberFuncThunk<const CTFBot *, bool, const Vector&               > ft_IsLineOfFireClear_vec;
	static MemberFuncThunk<const CTFBot *, bool, CBaseEntity *               > ft_IsLineOfFireClear_ent;
	static MemberFuncThunk<const CTFBot *, bool, const Vector&, const Vector&> ft_IsLineOfFireClear_vec_vec;
	static MemberFuncThunk<const CTFBot *, bool, const Vector&, CBaseEntity *> ft_IsLineOfFireClear_vec_ent;
	
	static std::map<CHandle<CTFBot>, ExtendedAttr> s_ExtAttrs;
};


// TODO: ToTFBot
// (WARNING: DON'T ASSUME THAT DYNAMIC CASTS ARE SAFE!)


#endif
