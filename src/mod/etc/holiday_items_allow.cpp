#include "mod.h"
#include "stub/tf_shareddefs.h"


namespace Mod::Etc::Holiday_Items_Allow
{
	/* the only actual callers of this function are econ-related */
	DETOUR_DECL_STATIC(int, UTIL_GetHolidayForString, const char *str)
	{
		return kHoliday_None;
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Etc:Holiday_Items_Allow")
		{
			MOD_ADD_DETOUR_STATIC(UTIL_GetHolidayForString, "UTIL_GetHolidayForString");
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_etc_holiday_items_allow", "0", FCVAR_NOTIFY,
		"Mod: allow holiday-restricted loadout items regardless of the holiday state",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
