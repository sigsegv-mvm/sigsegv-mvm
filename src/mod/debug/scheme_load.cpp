#include "mod.h"


namespace Mod::Debug::Scheme_Load
{
	DETOUR_DECL_MEMBER(vgui::HScheme, ISchemeManager_LoadSchemeFromFileEx, vgui::VPANEL sizingPanel, const char *fileName, const char *tag)
	{
		// log stuff to console
		// pass thru the call
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Debug:Scheme_Load")
		{
			// detour LoadSchemeFromFileEx via the interface
		}
	};
	CMod s_Mod;
	
	#error need windows addr
	
	
	ConVar cvar_enable("sig_debug_scheme_load", "0", FCVAR_NOTIFY,
		"Debug: trace vgui scheme loads",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue){
			s_Mod.Toggle(static_cast<ConVar *>(pConVar)->GetBool());
		});
}
