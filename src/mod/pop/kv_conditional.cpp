#include "mod.h"


namespace Mod_Pop_KV_Conditional
{
	DETOUR_DECL_STATIC(bool, EvaluateConditional, const char *str)
	{
		bool result = DETOUR_STATIC_CALL(EvaluateConditional)(str);
		
		if (*str == '[') ++str;
		bool bNot = (*str == '!');
		
		if (V_stristr(str, "$SIGSEGV") != nullptr) {
			return !bNot;
		}
		
		return result;
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Pop:KV_Conditional")
		{
			MOD_ADD_DETOUR_STATIC(EvaluateConditional, "EvaluateConditional");
		}
		
		bool OnLoad()
		{
			this->ToggleAllDetours(true);
			return true;
		}
	};
	CMod s_Mod;
}
