#include "mod.h"
#include "stub/tfbot.h"
#include "util/iterate.h"


namespace Mod_Util_Add_Item
{
	CON_COMMAND(sig_add_item, "Gives the item with the specified name to the currently selected TFBot")
	{
		if (args.ArgC() < 2) {
			Warning("Expected an argument for the item name to give.\n");
			return;
		}
		if (args.ArgC() > 2) {
			Warning("Multiple arguments given. Use quotes for multi-word item names.\n");
			return;
		}
		
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
		
		bot->AddItem(args[1]);
	}
	
	CON_COMMAND(sig_add_item_all, "Gives the item with the specified name to all bots")
	{
		if (args.ArgC() < 2) {
			Warning("Expected an argument for the item name to give.\n");
			return;
		}
		if (args.ArgC() > 2) {
			Warning("Multiple arguments given. Use quotes for multi-word item names.\n");
			return;
		}
		
		/* include puppet bots */
		ForEachTFPlayer([=](CTFPlayer *player){
			if (player->IsBot()) {
				static_cast<CTFBot *>(player)->AddItem(args[1]);
			}
		});
	}
}
