#ifndef _INCLUDE_SIGSEGV_STUB_TFREVIVEMARKER_H_
#define _INCLUDE_SIGSEGV_STUB_TFREVIVEMARKER_H_


#include "link/link.h"
#include "prop.h"
#include "stub/baseanimating.h"


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
