#include "mod.h"


namespace Mod_Debug_Ammo_Counts
{
	CON_COMMAND(sig_debug_ammo_counts, "")
	{
		if (args.ArgC() != 2) return;
		
		int entindex = atoi(args[1]);
		
		CBasePlayer *player = UTIL_PlayerByIndex(entindex);
		if (player == nullptr) {
			Msg("No player exists with entindex %d.\n", entindex);
			return;
		}
		
		// TODO
	}
}
