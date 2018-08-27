#include "mod.h"
#include "stub/tfbot.h"
#include "util/admin.h"
#include "util/iterate.h"


namespace Mod_Util_Add_Item
{
	/* only works on TFBots */
	CON_COMMAND(sig_add_item, "Gives the item with the specified name to the currently selected TFBot")
	{
		if (args.ArgC() < 2) {
			Warning("Expected an argument for the item name to give.\n");
			return;
		}
		if (args.ArgC() > 2) {
			Warning("Extraneous arguments given. Use quotes for multi-word item names.\n");
			return;
		}
		const char *arg_itemname = args[1];
		
		INextBot *selected = TheNextBots().GetSelectedBot();
		if (selected == nullptr) {
			Warning("No bot is currently selected. Use nb_select to select one.\n");
			return;
		}
		
		auto bot = rtti_cast<CTFBot *>(selected);
		if (bot == nullptr) {
			Warning("Selected bot is not a TFBot.\n");
			return;
		}
		
		bot->AddItem(arg_itemname);
		Msg("Gave item '%s' to bot '%s'.\n", arg_itemname, bot->GetPlayerName());
	}
	
	CON_COMMAND(sig_add_item_name, "Gives the item with the specified name to the bot(s) with the specified name")
	{
		for (int i = 0; i < args.ArgC(); ++i) {
			Msg("args[%d]: |%s|\n", i, args[i]);
		}
		
		if (args.ArgC() < 2) {
			Warning("Expected an argument for the bot name to select.\n");
			return;
		}
		if (args.ArgC() < 3) {
			Warning("Expected an argument for the item name to give.\n");
			return;
		}
		if (args.ArgC() > 3) {
			Warning("Extraneous arguments given. Use quotes for multi-word item names.\n");
			return;
		}
		const char *arg_botname  = args[1];
		const char *arg_itemname = args[2];
		
		ForEachTFPlayer([=](CTFPlayer *player){
			if (!player->IsBot())                              return;
			if (!FStrEq(player->GetPlayerName(), arg_botname)) return;
			
			static_cast<CTFBot *>(player)->AddItem(arg_itemname);
			Msg("Gave item '%s' to bot '%s'.\n", arg_itemname, player->GetPlayerName());
		});
	}
	
	CON_COMMAND(sig_add_item_all, "Gives the item with the specified name to all bots")
	{
		if (args.ArgC() < 2) {
			Warning("Expected an argument for the item name to give.\n");
			return;
		}
		if (args.ArgC() > 2) {
			Warning("Extraneous arguments given. Use quotes for multi-word item names.\n");
			return;
		}
		const char *arg_itemname = args[1];
		
		ForEachTFPlayer([=](CTFPlayer *player){
			if (!player->IsBot()) return;
			
			static_cast<CTFBot *>(player)->AddItem(arg_itemname);
			Msg("Gave item '%s' to bot '%s'.\n", arg_itemname, player->GetPlayerName());
		});
	}
}
