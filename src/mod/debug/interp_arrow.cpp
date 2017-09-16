#include "mod.h"
#include "stub/tfbot.h"
#include "stub/projectiles.h"
#include "util/iterate.h"


namespace Mod_Debug_Interp_Arrow
{
	[[gnu::format(printf, 1, 2)]]
	void ServerCommand(const char *format, ...)
	{
		char buf[4096];
		
		va_list va;
		va_start(va, format);
		vsnprintf(buf, sizeof(buf), format, va);
		va_end(va);
		
		engine->ServerCommand(buf);
		engine->ServerExecute();
	}
	
	
	void AddPuppetBot(const char *team, const char *playerclass, const char *name)
	{
		ServerCommand("bot -team %s -class %s -name %s\n", team, playerclass, name);
	}
	
	void TeleportBot(const char *name, const Vector& origin, const QAngle& angles)
	{
		ServerCommand("bot_teleport %s %f %f %f %f %f %f\n", name,
			origin.x, origin.y, origin.z,
			angles.x, angles.y, angles.x);
		
	//	constexpr int damage = -1000000;
	//	ServerCommand("bot_hurt -name %s -damage %d\n", name, damage);
	}
	
	void RefillBots()
	{
		ServerCommand("bot_refill\n");
	}
	
	
	CON_COMMAND(sig_debug_interp_arrow_bot_add, "")
	{
		ServerCommand("bot_kick all\n");
		AddPuppetBot("blue", "demoman", "DemoBot");
	}
	
	CON_COMMAND(sig_debug_interp_arrow_bot_prepare, "")
	{
		ForEachTFPlayer([](CTFPlayer *player){
			if (!player->IsBot()) return;
			
			reinterpret_cast<CTFBot *>(player)->AddItem("Ali Baba's Wee Booties");
			reinterpret_cast<CTFBot *>(player)->AddItem("The Splendid Screen");
			reinterpret_cast<CTFBot *>(player)->AddItem("The Eyelander");
		});
		
		ServerCommand("bot_selectweaponslot 2\n");
		ServerCommand("bot_forceattack_down 0\n");
		ServerCommand("bot_forceattack2 1\n");
		ServerCommand("bot_forceattack 0\n");
		
		TeleportBot("DemoBot",
			Vector(V_atof(args[1]), V_atof(args[2]), V_atof(args[3])),
			QAngle(V_atof(args[4]), V_atof(args[5]), V_atof(args[6])));
	}
	
	CON_COMMAND(sig_debug_interp_arrow_bot_charge, "")
	{
		
	}
}
