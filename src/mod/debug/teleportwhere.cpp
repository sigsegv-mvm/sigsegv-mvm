#include "mod.h"
#include "stub/tfbot.h"


namespace Mod::Debug::TeleportWhere
{
	const CUtlStringList& Get_CTFBot_m_TeleportWhere(const CTFBot *bot)
	{
		// ServerLinux 20161122b: +0x2614
		return *reinterpret_cast<const CUtlStringList *>((uintptr_t)bot + 0x2614);
	}
	
	
	DETOUR_DECL_MEMBER(int, CTFBot_DrawDebugTextOverlays)
	{
		int line = 1;
		
		auto bot = reinterpret_cast<CTFBot *>(this);
		const auto& telewhere = Get_CTFBot_m_TeleportWhere(bot);
		
		bot->EntityText(line++, CFmtStr("Bot #%d: \"%s\"", ENTINDEX(bot), bot->GetPlayerName()), 0.0f, 0xff, 0xff, 0xff, 0xff);
		bot->EntityText(line++, CFmtStr("TeleportWhere: %d entries", telewhere.Count()), 0.0f, 0xff, 0xff, 0xff, 0xff);
		for (int i = 0; i < telewhere.Count(); ++i) {
			bot->EntityText(line++, CFmtStr("- %s", telewhere[i]), 0.0f, 0xff, 0xff, 0xff, 0xff);
		}
		
		return line;
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Debug:TeleportWhere")
		{
			MOD_ADD_DETOUR_MEMBER(CTFBot_DrawDebugTextOverlays, "CTFBot::DrawDebugTextOverlays");
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_debug_teleportwhere", "0", FCVAR_NOTIFY,
		"Debug: determine whether bots' TeleportWhere vectors are properly being cleared out at spawn",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue){
			s_Mod.Toggle(static_cast<ConVar *>(pConVar)->GetBool());
		});
}
