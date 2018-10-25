#include "mod.h"
#include "mod/util/netmsg_shared.h"


namespace Mod::Util::NetMsg_Client
{
	
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Util:NetMsg_Client")
		{
			
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_util_netmsg_client", "0", FCVAR_NOTIFY,
		"Utility: custom net messages: client",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
