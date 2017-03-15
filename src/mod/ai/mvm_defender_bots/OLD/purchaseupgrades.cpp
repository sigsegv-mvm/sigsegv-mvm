#include "mod/ai/mvm_defender_bots/purchaseupgrades.h"
#include "mod/ai/mvm_defender_bots/gotoupgradestation.h"
#include "stub/gamerules.h"


// sizeof: 0x15c
struct CMannVsMachineUpgrades
{
	char m_szAttribute[0x40]; // +0x000
	char m_szIcon[MAX_PATH];  // +0x040
	float m_flIncrement;      // +0x144
	float m_flCap;            // +0x148
	int m_nCost;              // +0x14c
	int m_iUIGroup;           // +0x150
	int m_iQuality;           // +0x154
	int m_iTier;              // +0x158
};


/* TODO: move this into a stub file */
class CMannVsMachineUpgradeManager;
extern GlobalThunk<CMannVsMachineUpgradeManager> g_MannVsMachineUpgrades;

class CMannVsMachineUpgradeManager
{
public:
	static CUtlVector<CMannVsMachineUpgrades>& Upgrades()
	{
		CMannVsMachineUpgradeManager& instance = g_MannVsMachineUpgrades;
		return instance.m_Upgrades;
	}
	
private:
	uint8_t pad_00[0x0c];
	CUtlVector<CMannVsMachineUpgrades> m_Upgrades;
	CUtlMap<const char *, int> m_UpgradeMap;
};
SIZE_CHECK(CMannVsMachineUpgradeManager, 0x3c);

GlobalThunk<CMannVsMachineUpgradeManager> g_MannVsMachineUpgrades("g_MannVsMachineUpgrades");


namespace Mod_AI_MvM_Defender_Bots
{
	CMannVsMachineUpgrades *GetUpgradeByIndex(int index)
	{
//		DevMsg("GetUpgradeByIndex: index = %d, CMannVsMachineUpgradeManager::Upgrades().Count() = %d\n",
//			index, CMannVsMachineUpgradeManager::Upgrades().Count());
		
		assert(index >= 0 && index < CMannVsMachineUpgradeManager::Upgrades().Count());
		return &CMannVsMachineUpgradeManager::Upgrades()[index];
	}
	
	
	void ForEachValidUpgrade(CTFPlayer *player, int slot, const std::function<void(int)>& functor)
	{
		CUtlVector<CMannVsMachineUpgrades>& upgrades = CMannVsMachineUpgradeManager::Upgrades();
		
		FOR_EACH_VEC(upgrades, i) {
			if (upgrades[i].m_iUIGroup == 1 && slot != -1) continue;
			
			auto attr = GetItemSchema()->GetAttributeDefinitionByName(upgrades[i].m_szAttribute);
			if (attr == nullptr) continue;
			
			if (!TFGameRules()->CanUpgradeWithAttrib(player, slot, attr->GetIndex(), &upgrades[i])) continue;
			
			functor(i);
		}
	}
	
	
	int GetUpgradePriority(const UpgradeInfo& info)
	{
		CMannVsMachineUpgrades *upgrade = GetUpgradeByIndex(info.index);
		
		if (info.pclass == TF_CLASS_SNIPER) {
			if (info.slot == TF_LOADOUT_SLOT_PRIMARY &&
				V_stricmp(upgrade->m_szAttribute, "explosive sniper shot") == 0) {
				return 100;
			}
		}
		
		if (info.pclass == TF_CLASS_MEDIC) {
			if (info.slot == TF_LOADOUT_SLOT_SECONDARY &&
				V_stricmp(upgrade->m_szAttribute, "generate rage on heal") == 0) {
				return 100;
			}
		}
		
		if (info.pclass == TF_CLASS_SOLDIER) {
			if (info.slot == TF_LOADOUT_SLOT_PRIMARY &&
				V_stricmp(upgrade->m_szAttribute, "rocket specialist") == 0) {
				return 100;
			}
		}
		
		if (info.pclass == TF_CLASS_SPY) {
			if (info.slot == TF_LOADOUT_SLOT_MELEE &&
				V_stricmp(upgrade->m_szAttribute, "armor piercing") == 0) {
				return 100;
			}
		}
		
		if (info.pclass == TF_CLASS_HEAVYWEAPONS) {
			if (info.slot == TF_LOADOUT_SLOT_PRIMARY &&
				V_stricmp(upgrade->m_szAttribute, "attack projectiles") == 0) {
				return 100;
			}
		}
		
		if (info.pclass == TF_CLASS_SCOUT) {
			if (info.slot == TF_LOADOUT_SLOT_SECONDARY &&
				V_stricmp(upgrade->m_szAttribute, "applies snare effect") == 0) {
				return 100;
			}
		}
		
		/* low priority for canteen upgrades */
		if (info.slot == TF_LOADOUT_SLOT_ACTION) {
			return -10;
		}
		
		/* default priority */
		return 0;
	}
	
	bool CompareUpgrades(const UpgradeInfo& lhs, const UpgradeInfo& rhs)
	{
		int pri_lhs = GetUpgradePriority(lhs);
		int pri_rhs = GetUpgradePriority(rhs);
		
		if (pri_lhs != pri_rhs) {
			return (pri_lhs > pri_rhs);
		}
		
		/* use random order for equal priority upgrades */
		return (lhs.random < rhs.random);
	}
	
	
	ActionResult<CTFBot> CTFBotPurchaseUpgrades::OnStart(CTFBot *actor, Action<CTFBot> *action)
	{
		if (!this->IsStandingAtUpgradeStation()) {
			return ActionResult<CTFBot>::ChangeTo(new CTFBotGoToUpgradeStation(), "Not standing at an upgrade station!");
		}
		
		this->KV_MvM_UpgradesBegin();
		this->m_ctNextUpgrade.Start(this->GetUpgradeInterval());
		
		this->CollectUpgrades();
		
		return ActionResult<CTFBot>::Continue();
	}
	
	ActionResult<CTFBot> CTFBotPurchaseUpgrades::Update(CTFBot *actor, float dt)
	{
		if (!this->IsStandingAtUpgradeStation()) {
			return ActionResult<CTFBot>::ChangeTo(new CTFBotGoToUpgradeStation(), "Not standing at an upgrade station!");
		}
		
		if (this->m_ctNextUpgrade.IsElapsed()) {
			this->m_ctNextUpgrade.Start(this->GetUpgradeInterval());
			
			const UpgradeInfo *info = ChooseUpgrade();
			if (info != nullptr) {
				this->PurchaseUpgrade(info);
			} else {
				return ActionResult<CTFBot>::Done("No more upgrades to buy.");
			}
		}
		
		return ActionResult<CTFBot>::Continue();
	}
	
	void CTFBotPurchaseUpgrades::OnEnd(CTFBot *actor, Action<CTFBot> *action)
	{
		this->KV_MvM_UpgradesDone();
	}
	
	
	void CTFBotPurchaseUpgrades::CollectUpgrades()
	{
		for (int slot : {
			-1,
			(int)TF_LOADOUT_SLOT_PRIMARY,
			(int)TF_LOADOUT_SLOT_SECONDARY,
			(int)TF_LOADOUT_SLOT_MELEE,
			(int)TF_LOADOUT_SLOT_BUILDING,
			(int)TF_LOADOUT_SLOT_PDA,
		//	(int)TF_LOADOUT_SLOT_ACTION,
		}) {
			ForEachValidUpgrade(this->GetActor(), slot, [=](int index){
				this->m_Upgrades.emplace_back(this->GetActor()->GetPlayerClass()->GetClassIndex(), slot, index);
			});
		}
		
//		DevMsg("Unsorted upgrades for bot #%d \"%s\": %u total\n",
//			ENTINDEX(this->GetActor()), this->GetActor()->GetPlayerName(), this->m_Upgrades.size());
//		DevMsg("%3s %4s %-5s\n",
//			"#", "SLOT", "INDEX");
//		int j = 0;
//		for (const auto& info : this->m_Upgrades) {
//			DevMsg("%3d %4d %-5d\n",
//				j, info.slot, info.index);
//			++j;
//		}
		
		std::sort(this->m_Upgrades.begin(), this->m_Upgrades.end(), CompareUpgrades);
		
		DevMsg("Preferred upgrades for bot #%d \"%s\"\n",
			ENTINDEX(this->GetActor()), this->GetActor()->GetPlayerName());
		DevMsg("%3s %4s %4s %5s %-64s\n",
			"#", "SLOT", "COST", "INDEX", "ATTRIBUTE");
		int i = 0;
		for (const auto& info : this->m_Upgrades) {
			DevMsg("%3d %4d %4d %5d %-64s\n",
				i, info.slot,
				TFGameRules()->GetCostForUpgrade(GetUpgradeByIndex(info.index),
					info.slot, info.pclass, this->GetActor()),
				info.index, GetUpgradeByIndex(info.index)->m_szAttribute);
			++i;
		}
	}
	
	const UpgradeInfo *CTFBotPurchaseUpgrades::ChooseUpgrade()
	{
		int currency = this->GetActor()->GetCurrency();
		
		for (const auto& info : this->m_Upgrades) {
			if (TFGameRules()->GetCostForUpgrade(GetUpgradeByIndex(info.index),
				info.slot, info.pclass, this->GetActor()) > currency) {
				DevMsg("upgrade %d/%d: cost $%d > $%d\n",
					info.slot, info.index, TFGameRules()->GetCostForUpgrade(GetUpgradeByIndex(info.index), info.slot, info.pclass, this->GetActor()), currency);
				continue;
			}
			
			int tier = TFGameRules()->GetUpgradeTier(info.index);
			if (tier != 0) {
				if (!TFGameRules()->IsUpgradeTierEnabled(this->GetActor(), info.slot, tier)) {
					DevMsg("upgrade %d/%d: tier %d isn't enabled\n",
						info.slot, info.index, tier);
					continue;
				}
			}
			
			return &info;
		}
		
		return nullptr;
	}
	
	void CTFBotPurchaseUpgrades::PurchaseUpgrade(const UpgradeInfo *info)
	{
		this->KV_MVM_Upgrade(1, info->slot, info->index);
		++this->m_nPurchasedUpgrades;
	}
}
