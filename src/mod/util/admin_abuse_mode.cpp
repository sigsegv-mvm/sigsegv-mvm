#include "mod.h"
#include "stub/baseplayer.h"


namespace Mod_Util_Admin_Abuse_Mode
{
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Util:Admin_Abuse_Mode") {}
		
		virtual void OnEnable() override
		{
			static ConVarRef cvar_client_cmds  ("sig_util_client_cmds");
			static ConVarRef cvar_make_item    ("sig_util_make_item");
			static ConVarRef cvar_jointeam_blue("sig_mvm_jointeam_blue_allow");
			
			if (cvar_client_cmds  .IsValid()) cvar_client_cmds  .Ref_Flags() &= ~FCVAR_NOTIFY;
			if (cvar_make_item    .IsValid()) cvar_make_item    .Ref_Flags() &= ~FCVAR_NOTIFY;
			if (cvar_jointeam_blue.IsValid()) cvar_jointeam_blue.Ref_Flags() &= ~FCVAR_NOTIFY;
		}
		
		virtual void OnDisable() override
		{
			static ConVarRef cvar_client_cmds  ("sig_util_client_cmds");
			static ConVarRef cvar_make_item    ("sig_util_make_item");
			static ConVarRef cvar_jointeam_blue("sig_mvm_jointeam_blue_allow");
			
			if (cvar_client_cmds  .IsValid()) cvar_client_cmds  .Ref_Flags() |= FCVAR_NOTIFY;
			if (cvar_make_item    .IsValid()) cvar_make_item    .Ref_Flags() |= FCVAR_NOTIFY;
			if (cvar_jointeam_blue.IsValid()) cvar_jointeam_blue.Ref_Flags() &= ~FCVAR_NOTIFY;
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_util_admin_abuse_mode", "0", FCVAR_NOTIFY,
		"Utility: restrict Util:Client_Cmds and Util:Make_Item and MvM:JoinTeam_Blue_Allow functionality to specific players (NOTE: this feature was added reluctantly due to repetitive requests)",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
	
	
	/* complete list of people who demanded that this feature be added: */
	static std::set<uint64_t> admins{
		0x1100001004F61EAULL, // Hell-met
	};
	
	
	static_assert(sizeof(uint64_t) == sizeof(unsigned long long));
	
	CON_COMMAND(sig_admin_add, "")
	{
		if (args.ArgC() != 2) return;
		
		uint64_t id64 = strtoull(args[1], nullptr, 0);
		if (id64 == 0) return;
		
		CSteamID steamid(id64);
		if (!steamid.IsValid()) return;
		
		if (admins.emplace(id64).second) {
			Msg("Added %s to the admin list.\n", steamid.Render());
		} else {
			Msg("%s is already on the admin list.\n", steamid.Render());
		}
	}
	
	CON_COMMAND(sig_admin_del, "")
	{
		if (args.ArgC() != 2) return;
		
		uint64_t id64 = strtoull(args[1], nullptr, 0);
		if (id64 == 0) return;
		
		CSteamID steamid(id64);
		if (!steamid.IsValid()) return;
		
		if (admins.erase(id64) != 0) {
			Msg("Deleted %s from the admin list.\n", steamid.Render());
		} else {
			Msg("%s was not on the admin list.\n", steamid.Render());
		}
	}
	
	
	bool IsPlayerAllowed(CBasePlayer *player)
	{
		if (!s_Mod.IsEnabled()) return true;
		
		if (player->IsBot()) return true;
		
		CSteamID steamid;
		if (!player->GetSteamID(&steamid)) return 0;
		return (admins.count(steamid.ConvertToUint64()) > 0);
	}
}
