#include "mod.h"
#include "stub/tfplayer.h"
#include "util/iterate.h"


namespace Mod::Debug::Insane_Health
{
	CON_COMMAND_F(sig_debug_insane_health, "Debug: set HP to given value and see what happens", FCVAR_CHEAT)
	{
		if (args.ArgC() < 2) return;
		
		int64_t hp_target = strtoll(args[1], nullptr, 0);
		
		ForEachTFPlayer([=](CTFPlayer *player){
			if (!player->IsAlive()) return;
			
			float maxhp_to_add = (hp_target - player->GetMaxHealth());
			player->AddCustomAttribute("hidden maxhealth non buffed", maxhp_to_add, 3600.0f);
			
			player->SetHealth(hp_target);
			
			Msg("Set health of '%s' to %" PRIi64 "\n", player->GetPlayerName(), hp_target);
		});
	}
}
