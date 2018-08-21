#include "mod.h"
#include "stub/tfplayer.h"
#include "stub/tfweaponbase.h"
#include "stub/econ.h"
#include "stub/misc.h"
#include "util/misc.h"
#include "util/clientmsg.h"


// TODO 20170825:
// - client message acknowledging a successful give
// - fix integer parsing issue ("1" --> some very big number)
// - help command
// - give for players other than self
// - maybe: allow referring to attributes by attribute_class
//   - have an associative array of <attr_class string> --> <list of CEconItemAttributeDefinition ptrs>
//   - pre-load the array once initially by iterating over all CEconItemAttributeDefinition's
//   - see how reasonable or bad the "multiple attr defs have the same attr class" situation is
//     - keep in mind that we can't necessarily just pick one attr def arbitrarily,
//       since the attr value will either stack or not stack with existing attr values depending on
//       whether the attr def itself matches, not the attr class
// - maybe: make a way to apply unimplemented attributes to items
//   - if we do this by adding our own attr defs to the schema, then we may need to hack the networking code for
//     CAttributeList so that the client doesn't see our custom-made ones
// - maybe: user restriction functionality (so everyone doesn't use this mod willy-nilly)


namespace Mod_Util_Admin_Abuse_Mode
{
	bool IsPlayerAllowed(CBasePlayer *player);
}


namespace Mod_Util_Make_Item
{
	const CSteamID *GetCommandClientSteamID(const char *func, CTFPlayer *player)
	{
		const CSteamID *steamid = engine->GetClientSteamID(player->edict());
		if (steamid == nullptr) {
			Msg("[sig_util_make_item] %s: GetClientSteamID returned nullptr\n", func);
			return nullptr;
		}
		if (!steamid->IsValid()) {
			Msg("[sig_util_make_item] %s: Client's Steam ID is invalid\n", func);
			return nullptr;
		}
		
		return steamid;
	}
	
	
	class CEconItemViewWrapper
	{
	public:
		CEconItemViewWrapper()
		{
			this->m_pEconItemView = CEconItemView::Create();
		}
		~CEconItemViewWrapper()
		{
			// the attr value needs to be deallocated AT SOME POINT to avoid leaks...
			// but how do we both allow for items to persist past mod unload, yet not leak memory?
		//	for (auto& attr : this->m_pEconItemView->GetAttributeList().m_Attributes) {
		//		attr.GetStaticData()->GetType()->UnloadEconAttributeValue(attr.GetValuePtr());
		//	}
			
			CEconItemView::Destroy(this->m_pEconItemView);
		}
		
		operator CEconItemView *() { return this->m_pEconItemView; }
		CEconItemView *operator*() { return this->m_pEconItemView; }
		
	private:
		CEconItemView *m_pEconItemView;
	};
	
	static std::map<CSteamID, CEconItemViewWrapper> state;
	
	
	void CC_Help(CTFPlayer *player, const CCommand& args)
	{
		// TODO
		// show usage info for all commands
		// as well as examples, probably
	}
	
	
	void CC_Clear(CTFPlayer *player, const CCommand& args)
	{
		const CSteamID *steamid = GetCommandClientSteamID("CC_Clear", player);
		if (steamid == nullptr) return;
		
		auto it = state.find(*steamid);
		if (it == state.end()) {
			ClientMsg(player, "[sig_makeitem_clear] No item is in progress; nothing to clear.\n");
			return;
		}
		
		CEconItemView *item_view = (*it).second;
		if (item_view != nullptr) {
			ClientMsg(player, "[sig_makeitem_clear] Clearing out information for unfinished item \"%s\"\n", item_view->GetStaticData()->GetName());
		}
		
		state.erase(it);
	}
	
	
	void CC_Start(CTFPlayer *player, const CCommand& args)
	{
		const CSteamID *steamid = GetCommandClientSteamID("CC_Start", player);
		if (steamid == nullptr) return;
		
		if (args.ArgC() != 2) {
			ClientMsg(player, "[sig_makeitem_start] Usage: any of the following:\n"
				"  sig_makeitem_start <item_name>      | item names that include spaces need quotes\n"
				"  sig_makeitem_start <item_def_index> | item definition indexes can be found in the item schema\n");
			return;
		}
		
		auto it = state.find(*steamid);
		if (it != state.end()) {
			CEconItemView *item_view = (*it).second;
			if (item_view != nullptr) {
				ClientMsg(player, "[sig_makeitem_start] Warning: discarding information for unfinished item \"%s\"\n", item_view->GetStaticData()->GetName());
			}
			
			state.erase(it);
		}
		
		/* attempt lookup first by item name, then by item definition index */
		auto item_def = rtti_cast<CTFItemDefinition *>(GetItemSchema()->GetItemDefinitionByName(args[1]));
		if (item_def == nullptr) {
			int idx = -1;
			if (StringToIntStrict(args[1], idx)) {
				item_def = FilterOutDefaultItemDef(rtti_cast<CTFItemDefinition *>(GetItemSchema()->GetItemDefinition(idx)));
			}
		}
		
		if (item_def == nullptr) {
			ClientMsg(player, "[sig_makeitem_start] Error: couldn't find any items in the item schema matching \"%s\"\n", args[1]);
			return;
		}
		
		/* insert new element into the map */
		CEconItemView *item_view = state[*steamid];
		item_view->Init(item_def->m_iItemDefIndex, item_def->m_iItemQuality, 9999, 0);
		
		ClientMsg(player, "[sig_makeitem_start] Started making item \"%s\"\n", item_def->GetName());
	}
	
	
	void CC_AddAttr(CTFPlayer *player, const CCommand& args)
	{
		const CSteamID *steamid = GetCommandClientSteamID("CC_AddAttr", player);
		if (steamid == nullptr) return;
		
		if (args.ArgC() != 3) {
			ClientMsg(player, "[sig_makeitem_add_attr] Usage: any of the following:\n"
				"  sig_makeitem_add_attr <attr_name> <value>      | attribute names that include spaces need quotes\n"
				"  sig_makeitem_add_attr <attr_def_index> <value> | attribute definition indexes can be found in the item schema\n");
			return;
		}
		
		auto it = state.find(*steamid);
		if (it == state.end()) {
			ClientMsg(player, "[sig_makeitem_add_attr] Error: you need to do sig_makeitem_start before you can do sig_makeitem_add_attr\n");
			return;
		}
		
		CEconItemView *item_view = (*it).second;
		
		/* attempt lookup first by attr name, then by attr definition index */
		CEconItemAttributeDefinition *attr_def = GetItemSchema()->GetAttributeDefinitionByName(args[1]);
		if (attr_def == nullptr) {
			int idx = -1;
			if (StringToIntStrict(args[1], idx)) {
				attr_def = GetItemSchema()->GetAttributeDefinition(idx);
			}
		}
		
		if (attr_def == nullptr) {
			ClientMsg(player, "[sig_makeitem_add_attr] Error: couldn't find any attributes in the item schema matching \"%s\"\n", args[1]);
			return;
		}
		
		const char *value_str = STRING(AllocPooledString(args[2]));
		
		attribute_data_union_t value;
		// TODO: maybe make a wrapper class for this or something, so that if we return early, we still do the proper
		// UnloadEconAttributeValue call to avoid leaking memory
		attr_def->GetType()->InitializeNewEconAttributeValue(&value);
		if (!attr_def->GetType()->BConvertStringToEconAttributeValue(attr_def, value_str, &value, true)) {
			ClientMsg(player, "[sig_makeitem_add_attr] Error: couldn't parse attribute value \"%s\"\n", args[2]);
			attr_def->GetType()->UnloadEconAttributeValue(&value);
			return;
		}
		CEconItemAttribute *attr = CEconItemAttribute::Create(attr_def->GetIndex());
		*attr->GetValuePtr() = value;
		item_view->GetAttributeList().AddAttribute(attr);
		CEconItemAttribute::Destroy(attr);
		
		std::string str;
		attr_def->GetType()->ConvertEconAttributeValueToString(attr_def, value, &str);
		ClientMsg(player, "[sig_makeitem_add_attr] Added attribute \"%s\" with value \"%s\"\n", attr_def->GetName(), str.c_str());
	}
	
	
	void CC_Give(CTFPlayer *player, const CCommand& args)
	{
		const CSteamID *steamid = GetCommandClientSteamID("CC_Give", player);
		if (steamid == nullptr) return;
		
		if (args.ArgC() != 1 && args.ArgC() != 2) {
			ClientMsg(player, "[sig_makeitem_give] Usage: any of the following:\n"
				"  sig_makeitem_give               | give to yourself\n"
				"  sig_makeitem_give <player_name> | give to the player with this name (names with spaces need quotes)\n"
				"  sig_makeitem_give <steam_id>    | give to the player with this Steam ID\n"
				"  sig_makeitem_give <user_id>     | give to the player with this server user ID\n");
			return;
		}
		
		// possible ways to use this command:
		// - 0 args: give to me
		// - 1 args: give to steam ID, or user ID, or player name exact match, or player name unique-partial-match
		CTFPlayer *recipient = nullptr;
		if (args.ArgC() == 1) {
			recipient = player;
		} else if (args.ArgC() == 2) {
			ClientMsg(player, "[sig_makeitem_give] UNIMPLEMENTED: 1-arg variants\n");
			return;
			
			// TODO: implement 1-arg version of this command
			
			if (recipient == nullptr) {
				ClientMsg(player, "[sig_makeitem_give] Error: couldn't find any potential recipient player matching \"%s\"\n", args[1]);
				return;
			}
		}
		
		auto it = state.find(*steamid);
		if (it == state.end()) {
			ClientMsg(player, "[sig_makeitem_give] Error: you need to do sig_makeitem_start before you can do sig_makeitem_give\n");
			return;
		}
		
		CEconItemView *item_view = (*it).second;
		
		int slot = item_view->GetStaticData()->GetLoadoutSlot(recipient->GetPlayerClass()->GetClassIndex());
		
		if (IsLoadoutSlot_Cosmetic(slot)) {
			/* equip-region-conflict-based old item removal */
			
			unsigned int mask1 = item_view->GetStaticData()->GetEquipRegionMask();
			
			for (int i = recipient->GetNumWearables() - 1; i >= 0; --i) {
				CEconWearable *wearable = recipient->GetWearable(i);
				if (wearable == nullptr) continue;
				
				unsigned int mask2 = wearable->GetAttributeContainer()->GetItem()->GetStaticData()->GetEquipRegionMask();
				
				if ((mask1 & mask2) != 0) {
					Msg("Removing conflicting wearable \"%s\" (old: %08x, new: %08x, overlap: %08x)\n",
						wearable->GetAttributeContainer()->GetItem()->GetStaticData()->GetName(), mask2, mask1, (mask1 & mask2));
					recipient->RemoveWearable(wearable);
				}
			}
		} else {
			/* slot-based old item removal */
			
			CEconEntity *old_econ_entity = nullptr;
			(void)CTFPlayerSharedUtils::GetEconItemViewByLoadoutSlot(recipient, slot, &old_econ_entity);
			
			if (old_econ_entity != nullptr) {
				if (old_econ_entity->IsBaseCombatWeapon()) {
					auto old_weapon = rtti_cast<CBaseCombatWeapon *>(old_econ_entity);
					
					Msg("Removing old weapon \"%s\" from slot %d\n", old_weapon->GetAttributeContainer()->GetItem()->GetStaticData()->GetName(), slot);
					recipient->Weapon_Detach(old_weapon);
					old_weapon->Remove();
				} else if (old_econ_entity->IsWearable()) {
					auto old_wearable = rtti_cast<CEconWearable *>(old_econ_entity);
					
					Msg("Removing old wearable \"%s\" from slot %d\n", old_wearable->GetAttributeContainer()->GetItem()->GetStaticData()->GetName(), slot);
					recipient->RemoveWearable(old_wearable);
				} else {
					Msg("Removing old entity \"%s\" from slot %d\n", old_econ_entity->GetClassname(), slot);
					old_econ_entity->Remove();
				}
			} else {
				Msg("No old entity in slot %d\n", slot);
			}
		}
		
		CBaseEntity *ent = recipient->GiveNamedItem(item_view->GetStaticData()->GetItemClass(""), 0, item_view, false);
		if (ent != nullptr) {
			auto econ_ent = rtti_cast<CEconEntity *>(ent);
			if (econ_ent != nullptr) {
				/* make the model visible for other players */
				econ_ent->m_bValidatedAttachedEntity = true;
				
				/* make any extra wearable models visible for other players */
				auto weapon = rtti_cast<CTFWeaponBase *>(econ_ent);
				if (weapon != nullptr) {
					if (weapon->m_hExtraWearable != nullptr) {
						Msg("Validating extra wearable #%d on weapon #%d \"%s\"\n", ENTINDEX(weapon->m_hExtraWearable), ENTINDEX(weapon), weapon->GetClassname());
						weapon->m_hExtraWearable->m_bValidatedAttachedEntity = true;
					}
					if (weapon->m_hExtraWearableViewModel != nullptr) {
						Msg("Validating extra wearable VM #%d on weapon #%d \"%s\"\n", ENTINDEX(weapon->m_hExtraWearableViewModel), ENTINDEX(weapon), weapon->GetClassname());
						weapon->m_hExtraWearableViewModel->m_bValidatedAttachedEntity = true;
					}
				}
				
				econ_ent->GiveTo(recipient);
			} else {
				ClientMsg(player, "GiveNamedItem returned a non-CEconEntity\n");
			}
		} else {
			ClientMsg(player, "GiveNamedItem returned nullptr\n");
		}
		
		// TODO: display success message like this:
		//   Created item "<item name>" with the following attributes:
		//   - "<attr name>" "<value>"
		//   And gave it to player "<player name>"
		
		state.erase(it);
	}
	
	
	// TODO: use an std::unordered_map so we don't have to do any V_stricmp's at all for lookups
	// (also make this change in Util:Client_Cmds)
	static const std::map<const char *, void (*)(CTFPlayer *, const CCommand&), VStricmpLess> cmds {
		{ "sig_makeitem_help",     CC_Help    },
		{ "sig_makeitem_clear",    CC_Clear   },
		{ "sig_makeitem_start",    CC_Start   },
		{ "sig_makeitem_add_attr", CC_AddAttr },
		{ "sig_makeitem_give",     CC_Give    },
	};
	
	
	DETOUR_DECL_MEMBER(bool, CTFPlayer_ClientCommand, const CCommand& args)
	{
		auto player = reinterpret_cast<CTFPlayer *>(this);
		if (player != nullptr) {
			auto it = cmds.find(args[0]);
			if (it != cmds.end()) {
				if (Mod_Util_Admin_Abuse_Mode::IsPlayerAllowed(player)) {
					auto func = (*it).second;
					(*func)(player, args);
				} else {
					ClientMsg(player, "[%s] Admin abuse mode is enabled, so you are not authorized to use this command. Sorry.\n", (*it).first);
				}
				
				return true;
			}
		}
		
		return DETOUR_MEMBER_CALL(CTFPlayer_ClientCommand)(args);
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Util:Make_Item")
		{
			MOD_ADD_DETOUR_MEMBER(CTFPlayer_ClientCommand, "CTFPlayer::ClientCommand");
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_util_make_item", "0", FCVAR_NOTIFY,
		"Utility: enable sig_makeitem_* client commands",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
	
	
#if 0 // REMOVE THIS CODE (OR MOVE IT ELSEWHERE) ===============================
	
	// offsets based on server_srv.so version 20170803a
	CON_COMMAND_F(sig_equipregion_dump, "", FCVAR_CHEAT)
	{
		auto& m_EquipRegions = *reinterpret_cast<CUtlVector<CEconItemSchema::EquipRegion> *>((uintptr_t)GetItemSchema() + 0x378);
		
		Msg("%-8s  %-8s  | %s\n", "BITMASK", "MASK", "REGION");
		for (const auto& region : m_EquipRegions)
		{
			Msg("%08x  %08x  | %s\n", (1U << region.m_nBitMask), region.m_nMask, region.m_strName.Get());
		}
		Msg("\n");
		
		Msg("%-8s  %-8s  |        %s\n", "BITMASK", "MASK", "ITEM");
		auto schema = GetItemSchema();
		for (int i = 1; i < 40000; ++i) {
			auto item_def = schema->GetItemDefinition(i);
			if (item_def == nullptr)                                  continue;
			if (strcmp(item_def->GetName("default"), "default") == 0) continue;
			
			auto& m_nEquipRegionBitMask = *reinterpret_cast<unsigned int *>((uintptr_t)item_def + 0x12c);
			auto& m_nEquipRegionMask    = *reinterpret_cast<unsigned int *>((uintptr_t)item_def + 0x130);
			
			Msg("%08x  %08x  | #%-5d %s\n", m_nEquipRegionBitMask, m_nEquipRegionMask, i, item_def->GetName());
		}
		Msg("\n");
	}
	
#endif // ======================================================================
}
