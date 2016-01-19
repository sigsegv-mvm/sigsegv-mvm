#include "mod.h"
#include "stub/stub.h"
//#include "sm/detours.h"
//#include "util/util.h"
//#include "re/nextbot.h"


/* allow some cases that would normally get translated into an empty string by TranslateWeaponEntForClass */
static const char *TranslateWeaponEntForClass_improved(const char *name, int classnum)
{
	const char *xlat = TranslateWeaponEntForClass(name, classnum);
	
	/* if TranslateWeaponEntForClass gave us an empty string, return something reasonable instead */
	if (strcmp(xlat, "") == 0) {
		/* tf_weapon_shotgun: default to tf_weapon_shotgun_primary */
		if (strcasecmp(name, "tf_weapon_shotgun") == 0) {
			return "tf_weapon_shotgun_primary";
		}
		
		/* passthru the original entity class name for these cases */
		if (strcasecmp(name, "tf_weapon_pistol")    == 0) return name;
		if (strcasecmp(name, "tf_weapon_shovel")    == 0) return name;
		if (strcasecmp(name, "tf_weapon_bottle")    == 0) return name;
		if (strcasecmp(name, "tf_weapon_parachute") == 0) return name;
		if (strcasecmp(name, "tf_weapon_revolver")  == 0) return name;
	}
	
	return xlat;
}


static RefCount rc_CTFBot_AddItem;
static int bot_classnum = TF_CLASS_UNDEFINED;
DETOUR_DECL_MEMBER(void, CTFBot_AddItem, const char *item)
{
	SCOPED_INCREMENT(rc_CTFBot_AddItem);
	bot_classnum = reinterpret_cast<CTFBot *>(this)->GetPlayerClass()->GetClassIndex();
	DETOUR_MEMBER_CALL(CTFBot_AddItem)(item);
}

DETOUR_DECL_STATIC(CBaseEntity *, CreateEntityByName, const char *className, int iForceEdictIndex)
{
	if (rc_CTFBot_AddItem.NonZero()) {
		className = TranslateWeaponEntForClass_improved(className, bot_classnum);
	}
	
	return DETOUR_STATIC_CALL(CreateEntityByName)(className, iForceEdictIndex);
}


// void CTFBot::AddItem(const char *name)
// - CBaseEntity *CItemGeneration::GenerateRandomItem(CItemSelectionCriteria *criteria, const Vector& pos, const QAngle& ang)
//   - CBaseEntity *CItemGeneration::SpawnItem(int itemdefidx, const Vector& pos, const QAngle& ang, int, int, const char *class_or_null)
//     - CBaseEntity *CreateEntityByName(const char *className, int iForceEdictIndex)

// const char *TranslateWeaponEntForClass(const char *name, int classnum)



class CMod_BotMultiClassItem : public IMod
{
public:
	CMod_BotMultiClassItem() : IMod("BotMultiClassItem")
	{
		MOD_ADD_DETOUR_MEMBER(CTFBot, AddItem);
		MOD_ADD_DETOUR_GLOBAL(CreateEntityByName);
	}
	
	void SetEnabled(bool enable)
	{
		this->ToggleAllDetours(enable);
	}
};
static CMod_BotMultiClassItem s_Mod;


static ConVar cvar_enable("sigsegv_botmulticlassitem_enable", "0", FCVAR_NOTIFY,
	"Mod: remap item entity names so bots can be given multi-class weapons",
	[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
		ConVarRef var(pConVar);
		s_Mod.SetEnabled(var.GetBool());
	});
