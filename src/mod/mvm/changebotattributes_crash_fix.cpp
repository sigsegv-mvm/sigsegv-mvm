#include "mod.h"
#include "stub/tfbot.h"
#include "util/scope.h"


namespace Mod::MvM::ChangeBotAttributes_Crash_Fix
{
	RefCount rc_CPointPopulatorInterface_InputChangeBotAttributes;
	DETOUR_DECL_MEMBER(void, CPointPopulatorInterface_InputChangeBotAttributes, inputdata_t& inputdata)
	{
		SCOPED_INCREMENT(rc_CPointPopulatorInterface_InputChangeBotAttributes);
		DETOUR_MEMBER_CALL(CPointPopulatorInterface_InputChangeBotAttributes)(inputdata);
	}
	
	DETOUR_DECL_MEMBER(CTFBot::EventChangeAttributes_t *, CTFBot_GetEventChangeAttributes)
	{
		auto player = reinterpret_cast<CBasePlayer *>(this);
		if (ToTFBot(player) == nullptr) {
			return nullptr;
		}
		
		return DETOUR_MEMBER_CALL(CTFBot_GetEventChangeAttributes)();
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("MvM:ChangeBotAttributes_Crash_Fix")
		{
			MOD_ADD_DETOUR_MEMBER(CPointPopulatorInterface_InputChangeBotAttributes, "CPointPopulatorInterface::InputChangeBotAttributes");
			MOD_ADD_DETOUR_MEMBER(CTFBot_GetEventChangeAttributes,                   "CTFBot::GetEventChangeAttributes");
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_mvm_changebotattributes_crash_fix", "0", FCVAR_NOTIFY,
		"Mod: fix crash in which CPointPopulatorInterface::InputChangeBotAttributes assumes that all blue players are TFBots",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue){
			s_Mod.Toggle(static_cast<ConVar *>(pConVar)->GetBool());
		});
}
