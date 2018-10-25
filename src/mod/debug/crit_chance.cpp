#include "mod.h"
#include "stub/tfplayer.h"
#include "util/clientmsg.h"


namespace Mod::Debug::Crit_Chance
{
//	void Print_m_iCritMult(CTFPlayer *player)
//	{
//		
//	}
	
	// TODO: use ClientMsgAll to print out the following info:
	
	// - int   CTFPlayerShared::m_iCritMult
	// - float CTFPlayerShared::GetCritMult()
	
	// - float CBaseCombatWeapon::m_flCritTokenBucket
	// - int   CBaseCombatWeapon::n_nCritChecks
	// - int   CBaseCombatWeapon::m_nCritSeedRequests
	
	// void ClientMsgAll(const char *fmt, ARGS... args)
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Debug:Crit_Chance")
		{
			
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_debug_crit_chance", "0", FCVAR_NOTIFY,
		"Debug: determine influences on critical hit chance",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
