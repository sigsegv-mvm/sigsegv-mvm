#include "mod.h"
#include <vstdlib/coroutine.h>


namespace Mod::Debug::Console_Scramble_v1
{
	FILE *f = nullptr;
	SpewOutputFunc_t RealOutputFunc = nullptr;
	
	
	SpewRetval_t MyOutputFunc(SpewType_t spewType, const tchar *pMsg)
	{
		static std::mutex m;
		std::lock_guard<std::mutex> lock(m);
		
		fprintf(f, "MyOutputFunc [%15.10f] [T %4x] \"%s\"\n", Plat_FloatTime(), ThreadGetCurrentId(), pMsg);
		fflush(f);
		
	//	if (Coroutine_IsActive()) {
	//		size_t depth = Coroutine_GetStackDepth();
	//		fprintf(f, ">>> In coroutine! Stack depth: %lu\n", depth);
	//		fflush(f);
	//	}
		
	//	if (ThreadGetCurrentId() != 0x5914) {
	//		__asm {
	//			int 3
	//		};
	//	}
		
	//	return RealOutputFunc(spewType, pMsg);
		return SPEW_CONTINUE;
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Debug:Console_Scramble_v1") {}
		
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
	
	
	// new plan: detour all of these funcs, and synchronously print enter/exit events with thread ID to file from each
	// Msg
	// _SpewMessage
	// ...
	
	
	ConVar cvar_enable("sig_debug_console_scramble_v1", "0", FCVAR_NOTIFY,
		"Debug: determine why mat_queue_mode 2 causes console output to go out of order",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}

/* findings:

Default client spew func is Sys_SpewFunc, which calls Con_ColorPrintf (engine/console.cpp)
Con_ColorPrintf calls Con_ColorPrint, which calls
- g_pCVar->ConsoleColorPrintf
- g_pConPanel->AddToNotify

the deserialization has already happened by the time we get to Con_ColorPrint

it's in Sys_SpewFunc: inlined function AddSpewRecord(const char *) with static CUtlLinkedList<CUtlString> g_SpewHistory
no wait; actually all this does is add to a linked list for post-mortem minidump purposes (sys_minidumpspewlines);
messing with sys_minidumpspewlines does nothing

also there's GetSpew
and g_SpewMutex


ACTUAL REALIZATION:
it's vstdlib coroutines, man!


non-main thread:
>	tier0.dll!DevWarning() + 478 bytes	Unknown
 	[Frames below may be incorrect and/or missing, no symbols loaded for tier0.dll]	
 	atiumdag.dll!60c38567()	Unknown
 	atiumdag.dll!60c37a54()	Unknown
 	atiumdag.dll!60c8535a()	Unknown
 	atiumdag.dll!60c83b66()	Unknown
 	atiumdag.dll!60be4d54()	Unknown
 	d3d9.dll!_AllocateCB()	Unknown
 	ntdll.dll!_NtSetEvent()	Unknown
 	GameOverlayRenderer.dll!NotifyVRCleanup() + 13193 bytes	Unknown
 	atiu9pag.dll!OpenAdapter() + 1717 bytes	Unknown
 	d3d9.dll!CD3DDDIDX10_DrawIndexedPrimitive(class CD3DBase *,enum _D3DPRIMITIVETYPE,unsigned int,unsigned int,unsigned int,unsigned int,unsigned int)	Unknown
 	atiu9pag.dll!OpenAdapter() + 1434 bytes	Unknown
 	d3d9.dll!CD3DBase::UpdateTextures(void)	Unknown
 	d3d9.dll!CD3DHal::SetSamplerState_FP(unsigned long,enum _D3DSAMPLERSTATETYPE,unsigned long)	Unknown
 	1750d7c0()	Unknown
 	atiu9pag.dll!OpenAdapter() + 1717 bytes	Unknown
 	d3d9.dll!CD3DDDIDX10_DrawIndexedPrimitive(class CD3DBase *,enum _D3DPRIMITIVETYPE,unsigned int,unsigned int,unsigned int,unsigned int,unsigned int)	Unknown
 	d3d9.dll!CD3DBase::DrawIndexedPrimitive(enum _D3DPRIMITIVETYPE,int,unsigned int,unsigned int,unsigned int,unsigned int)	Unknown
 	shaderapidx9.dll!145ea027()	Unknown
 	shaderapidx9.dll!145f5e91()	Unknown
 	MaterialSystem.dll!0f8fa7e7()	Unknown
 	MaterialSystem.dll!0f8fa7f5()	Unknown
 	MaterialSystem.dll!0f8fa82e()	Unknown
 	atiu9pag.dll!XopOpenAdapters9() + 3039 bytes	Unknown
 	MaterialSystem.dll!0f8fa7f5()	Unknown
 	MaterialSystem.dll!0f8fc73a()	Unknown
 	MaterialSystem.dll!0f8fa5c8()	Unknown
 	MaterialSystem.dll!0f8b957e()	Unknown
 	shaderapidx9.dll!145f1605()	Unknown
 	shaderapidx9.dll!145e7586()	Unknown
 	shaderapidx9.dll!145e7b3f()	Unknown
 	shaderapidx9.dll!145f23d2()	Unknown
 	shaderapidx9.dll!145edae1()	Unknown
 	MaterialSystem.dll!0f8dfb65()	Unknown
 	MaterialSystem.dll!0f8dbeb0()	Unknown
 	MaterialSystem.dll!0f8dc7ca()	Unknown
 	MaterialSystem.dll!0f8dcf33()	Unknown
 	MaterialSystem.dll!0f8cf6e4()	Unknown
 	MaterialSystem.dll!0f8c3290()	Unknown
 	MaterialSystem.dll!0f8c7158()	Unknown
 	vstdlib.dll!Coroutine_YieldToMain() + 9984 bytes	Unknown
 	ntdll.dll!_NtClearEvent()	Unknown
 	tier0.dll!CThread::ThreadProc() + 256 bytes	Unknown
 	tier0.dll!0fe0d4f7()	Unknown
 	tier0.dll!0fe0d61f()	Unknown
 	kernel32.dll!@BaseThreadInitThunk()	Unknown
 	ntdll.dll!__RtlUserThreadStart()	Unknown
 	ntdll.dll!__RtlUserThreadStart()	Unknown


*/
