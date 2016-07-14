#include "mod.h"
#include "stub/tfplayer.h"


#define FL_ANIMDUCKING (1<<2)


namespace Mod_Debug_Sentry_Crouch_Aim
{
	CON_COMMAND(sig_debug_sentry_crouch_aim, "")
	{
		CTFPlayer *player = ToTFPlayer(UTIL_PlayerByIndex(1));
		if (player == nullptr) return;
		
		int flags = player->GetFlags();
		
		DevMsg("FL_ONGROUND:              %d\n", (flags & FL_ONGROUND ? 1 : 0));
		DevMsg("FL_DUCKING:               %d\n", (flags & FL_DUCKING ? 1 : 0));
		DevMsg("FL_ANIMDUCKING:           %d\n", (flags & FL_ANIMDUCKING ? 1 : 0));
		DevMsg("FL_WATERJUMP:             %d\n", (flags & FL_WATERJUMP ? 1 : 0));
		DevMsg("FL_ONTRAIN:               %d\n", (flags & FL_ONTRAIN ? 1 : 0));
		DevMsg("FL_INRAIN:                %d\n", (flags & FL_INRAIN ? 1 : 0));
		DevMsg("FL_FROZEN:                %d\n", (flags & FL_FROZEN ? 1 : 0));
		DevMsg("FL_ATCONTROLS:            %d\n", (flags & FL_ATCONTROLS ? 1 : 0));
		DevMsg("FL_CLIENT:                %d\n", (flags & FL_CLIENT ? 1 : 0));
		DevMsg("FL_FAKECLIENT:            %d\n", (flags & FL_FAKECLIENT ? 1 : 0));
		DevMsg("FL_INWATER:               %d\n", (flags & FL_INWATER ? 1 : 0));
		DevMsg("FL_FLY:                   %d\n", (flags & FL_FLY ? 1 : 0));
		DevMsg("FL_SWIM:                  %d\n", (flags & FL_SWIM ? 1 : 0));
		DevMsg("FL_CONVEYOR:              %d\n", (flags & FL_CONVEYOR ? 1 : 0));
		DevMsg("FL_NPC:                   %d\n", (flags & FL_NPC ? 1 : 0));
		DevMsg("FL_GODMODE:               %d\n", (flags & FL_GODMODE ? 1 : 0));
		DevMsg("FL_NOTARGET:              %d\n", (flags & FL_NOTARGET ? 1 : 0));
		DevMsg("FL_AIMTARGET:             %d\n", (flags & FL_AIMTARGET ? 1 : 0));
		DevMsg("FL_PARTIALGROUND:         %d\n", (flags & FL_PARTIALGROUND ? 1 : 0));
		DevMsg("FL_STATICPROP:            %d\n", (flags & FL_STATICPROP ? 1 : 0));
		DevMsg("FL_GRAPHED:               %d\n", (flags & FL_GRAPHED ? 1 : 0));
		DevMsg("FL_GRENADE:               %d\n", (flags & FL_GRENADE ? 1 : 0));
		DevMsg("FL_STEPMOVEMENT:          %d\n", (flags & FL_STEPMOVEMENT ? 1 : 0));
		DevMsg("FL_DONTTOUCH:             %d\n", (flags & FL_DONTTOUCH ? 1 : 0));
		DevMsg("FL_BASEVELOCITY:          %d\n", (flags & FL_BASEVELOCITY ? 1 : 0));
		DevMsg("FL_WORLDBRUSH:            %d\n", (flags & FL_WORLDBRUSH ? 1 : 0));
		DevMsg("FL_OBJECT:                %d\n", (flags & FL_OBJECT ? 1 : 0));
		DevMsg("FL_KILLME:                %d\n", (flags & FL_KILLME ? 1 : 0));
		DevMsg("FL_ONFIRE:                %d\n", (flags & FL_ONFIRE ? 1 : 0));
		DevMsg("FL_DISSOLVING:            %d\n", (flags & FL_DISSOLVING ? 1 : 0));
		DevMsg("FL_TRANSRAGDOLL:          %d\n", (flags & FL_TRANSRAGDOLL ? 1 : 0));
		DevMsg("FL_UNBLOCKABLE_BY_PLAYER: %d\n", (flags & FL_UNBLOCKABLE_BY_PLAYER ? 1 : 0));
		
		bool is_onground = ((flags & FL_ONGROUND) != 0);
		bool is_ducking  = ((flags & FL_DUCKING ) != 0);
		
		if ((!is_onground && is_ducking) || player->m_Shared->InCond(TF_COND_TAUNTING)) {
			DevMsg("Sentry will aim at bip_spine_2\n");
		} else {
			DevMsg("Sentry will aim at WSC\n");
		}
		
		DevMsg("Abs origin:         [ %4.0f %4.0f %4.0f ]\n",
			player->GetAbsOrigin().x, player->GetAbsOrigin().y, player->GetAbsOrigin().z);
		DevMsg("Eye position:       [ %4.0f %4.0f %4.0f ]\n",
			player->EyePosition().x, player->EyePosition().y, player->EyePosition().z);
		DevMsg("World space center: [ %4.0f %4.0f %4.0f ]\n",
			player->WorldSpaceCenter().x, player->WorldSpaceCenter().y, player->WorldSpaceCenter().z);
		
		
	}
}

/* findings:


prior to July 23 2015, sentry guns always aimed at enemies' WSC
the July 23 2015 patch was intended to fix the exploit where sentries would shoot over spies doing the new box trot taunt

now, sentry guns call CObjectSentrygun::GetEnemyAimPosition
- returns WSC by default
- if enemy is a player and either:
  - !FL_ONGROUND and FL_DUCKING
  - InCond(TF_COND_TAUNTING)
- then lookup bone "bip_spine_2" and return its bone position, if possible

so it seems that in addition to the box trot issue, they wanted to maybe address something about crouch jumping?
possibly involving players floating around with the base jumper? dunno

but anyway this ends up being somewhat irrelevant because the sentry aims for the WSC anyway under the buggy circumstance


procedure to reproduce the bug:
- sticky jump
- while in the air, start holding +duck
- continue holding +duck upon landing
- the game will put you in the standing animation for some reason even though you're crouching
- the sentry will essentially shoot between your legs


the major issue at play seems to be that the game's movement or animation systems are putting you in the wrong state
once you hit the ground, if you are holding +duck

*/
