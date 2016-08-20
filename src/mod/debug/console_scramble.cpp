#include "mod.h"


namespace Mod_Debug_Console_Scramble
{
	FILE *f = nullptr;
	SpewOutputFunc_t RealOutputFunc = nullptr;
	
	
	SpewRetval_t MyOutputFunc(SpewType_t spewType, const tchar *pMsg)
	{
		static std::mutex m;
		std::lock_guard<std::mutex> lock(m);
		
		fprintf(f, "MyOutputFunc [%15.10f] [T %5u] \"%s\"\n", Plat_FloatTime(), ThreadGetCurrentId(), pMsg);
		fflush(f);
		
		return RealOutputFunc(spewType, pMsg);
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Debug:Console_Scramble") {}
		
		virtual void OnEnable() override
		{
			f = fopen("spewlog.txt", "w");
			
			RealOutputFunc = GetSpewOutputFunc();
			SpewOutputFunc(&MyOutputFunc);
		}
		
		virtual void OnDisable() override
		{
			SpewOutputFunc(RealOutputFunc);
			
			fclose(f);
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_debug_console_scramble", "0", FCVAR_NOTIFY,
		"Debug: determine why mat_queue_mode 2 causes console output to go out of order",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
