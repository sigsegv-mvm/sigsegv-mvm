#include "mod.h"
#include "stub/tfbot.h"
#include "util/scope.h"


namespace Mod::Bot::MultiClass_Weapon
{
	/* a less strict version of TranslateWeaponEntForClass (don't return empty strings) */
	const char *TranslateWeaponEntForClass_improved(const char *name, int classnum)
	{
		if (strcasecmp(name, "tf_weapon_shotgun") == 0) {
			switch (classnum) {
			case TF_CLASS_SOLDIER:
				return "tf_weapon_shotgun_soldier";
			case TF_CLASS_PYRO:
				return "tf_weapon_shotgun_pyro";
			case TF_CLASS_HEAVYWEAPONS:
				return "tf_weapon_shotgun_hwg";
			case TF_CLASS_ENGINEER:
				return "tf_weapon_shotgun_primary";
			default:
				return "tf_weapon_shotgun_primary";
			}
		}
		
		if (strcasecmp(name, "tf_weapon_pistol") == 0) {
			switch (classnum) {
			case TF_CLASS_SCOUT:
				return "tf_weapon_pistol_scout";
			case TF_CLASS_ENGINEER:
				return "tf_weapon_pistol";
			default:
				return "tf_weapon_pistol";
			}
		}
		
		if (strcasecmp(name, "tf_weapon_shovel") == 0 || strcasecmp(name, "tf_weapon_bottle") == 0) {
			switch (classnum) {
			case TF_CLASS_SOLDIER:
				return "tf_weapon_shovel";
			case TF_CLASS_DEMOMAN:
				return "tf_weapon_bottle";
			}
		}
		
		if (strcasecmp(name, "saxxy") == 0) {
			switch (classnum) {
			case TF_CLASS_SCOUT:
				return "tf_weapon_bat";
			case TF_CLASS_SOLDIER:
				return "tf_weapon_shovel";
			case TF_CLASS_PYRO:
				return "tf_weapon_fireaxe";
			case TF_CLASS_DEMOMAN:
				return "tf_weapon_bottle";
			case TF_CLASS_HEAVYWEAPONS:
				return "tf_weapon_fireaxe";
			case TF_CLASS_ENGINEER:
				return "tf_weapon_wrench";
			case TF_CLASS_MEDIC:
				return "tf_weapon_bonesaw";
			case TF_CLASS_SNIPER:
				return "tf_weapon_club";
			case TF_CLASS_SPY:
				return "tf_weapon_knife";
			}
		}
		
		if (strcasecmp(name, "tf_weapon_throwable") == 0) {
			switch (classnum) {
			case TF_CLASS_MEDIC:
				return "tf_weapon_throwable_primary";
			default:
				return "tf_weapon_throwable_secondary";
			}
		}
		
		if (strcasecmp(name, "tf_weapon_parachute") == 0) {
			switch (classnum) {
			case TF_CLASS_SOLDIER:
				return "tf_weapon_parachute_secondary";
			case TF_CLASS_DEMOMAN:
				return "tf_weapon_parachute_primary";
			default:
				return "tf_weapon_parachute";
			}
		}
		
		if (strcasecmp(name, "tf_weapon_revolver") == 0) {
			switch (classnum) {
			case TF_CLASS_ENGINEER:
				return "tf_weapon_revolver_secondary";
			default:
				return "tf_weapon_revolver";
			}
		}
		
		/* if not handled: return original entity name, not an empty string */
		return name;
	}
	
	
	RefCount rc_CTFBot_AddItem;
	int bot_classnum = TF_CLASS_UNDEFINED;
	DETOUR_DECL_MEMBER(void, CTFBot_AddItem, const char *item)
	{
		SCOPED_INCREMENT(rc_CTFBot_AddItem);
		bot_classnum = reinterpret_cast<CTFBot *>(this)->GetPlayerClass()->GetClassIndex();
		DETOUR_MEMBER_CALL(CTFBot_AddItem)(item);
	}
	
	DETOUR_DECL_STATIC(CBaseEntity *, CreateEntityByName, const char *className, int iForceEdictIndex)
	{
		if (rc_CTFBot_AddItem > 0) {
			className = TranslateWeaponEntForClass_improved(className, bot_classnum);
		}
		
		return DETOUR_STATIC_CALL(CreateEntityByName)(className, iForceEdictIndex);
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Bot:MultiClass_Weapon")
		{
			MOD_ADD_DETOUR_MEMBER(CTFBot_AddItem,     "CTFBot::AddItem");
			MOD_ADD_DETOUR_STATIC(CreateEntityByName, "CreateEntityByName");
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_bot_multiclass_weapon", "0", FCVAR_NOTIFY,
		"Mod: remap item entity names so bots can be given multi-class weapons",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
