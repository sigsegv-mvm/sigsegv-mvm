#include "mod.h"
#include "stub/baseentity.h"
#include "stub/econ.h"


namespace Mod_Attr_CustomProjectileModel_Precache
{
	DETOUR_DECL_MEMBER(void, CTFWeaponBaseGun_GetCustomProjectileModel, CAttribute_String *attr_str)
	{
		DETOUR_MEMBER_CALL(CTFWeaponBaseGun_GetCustomProjectileModel)(attr_str);
		
		const char *model_path = nullptr;
		CopyStringAttributeValueToCharPointerOutput(attr_str, &model_path);
		if (model_path != nullptr && *model_path != '\x00') {
			CBaseEntity::PrecacheModel(model_path);
		}
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Attr:CustomProjectileModel_Precache")
		{
			MOD_ADD_DETOUR_MEMBER(CTFWeaponBaseGun_GetCustomProjectileModel, "CTFWeaponBaseGun::GetCustomProjectileModel");
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_attr_customprojectilemodel_precache", "0", FCVAR_NOTIFY,
		"Mod: do automatic model precaching of \"custom projectile model\" attr values instead of crashing the server",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
