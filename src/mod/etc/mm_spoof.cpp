#include "mod.h"


#if 0
class CTFLobby /*: public CSOTFLobby, public GCSDK::CProtoBufSharedObject<CSOTFLobby, 2004>, public GCSDK::ILobby*/
{
public:
	
};


class CMatchInfo
{
public:
	// 00 CTFLobby+0xdc
	// 04 CTFLobby+0xe0
	// 08 some vcall related to CTFLobby
	// 0c 
	// 10 CTFLobby+0xd8, presumably this is an EMatchGroup enum
	// 14 
	// 18 
	// 1c 
	// 20 
	// 24 
	// 28 0
	// 2c 0
	// 30 0
	// 34 0
	// 38 0
	// 3c 0
	// 40 0
	// 44 0
	// 48 0
	// 4c 0
	// 50 CUtlString
	// 54 
	// 58 
	// 5c float
	// 60 float
	// 64 float
};


enum EMatchGroup
{
	//  0 Bootcamp
	//  1 Mannup
	//  2 Ladder6v6
	//  3 Ladder9v9
	//  4 Ladder12v12
	//  5 Casual6v6
	//  6 Casual9v9
	//  7 Casual12v12
};

/*
TF MM spoofing
main thing we need to override:
CTFGameRules::GetCurrentMatchGroup()
- it gets the CMatchInfo* @ GTFGCClientSystem()+0x3c8
- if non-null, returns the int @ +0x10
- if null, returns -1
*/
#endif


enum EMatchGroup : int
{
	GROUP_INVALID      = -1,
	GROUP_BOOTCAMP     =  0,
	GROUP_MANNUP       =  1,
	GROUP_LADDER_6V6   =  2,
	GROUP_LADDER_9V9   =  3,
	GROUP_LADDER_12V12 =  4,
	GROUP_CASUAL_6V6   =  5,
	GROUP_CASUAL_9V9   =  6,
	GROUP_CASUAL_12V12 =  7,
};


namespace Mod_Etc_MM_Spoof
{
	ConVar cvar_mode("sig_etc_mm_spoof_mode", "-1", FCVAR_NOTIFY,
		"Mod: spoof mode");
	
	DETOUR_DECL_MEMBER(EMatchGroup, CTFGameRules_GetCurrentMatchGroup)
	{
		return (EMatchGroup)cvar_mode.GetInt();
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Etc:MM_Spoof")
		{
			MOD_ADD_DETOUR_MEMBER(CTFGameRules_GetCurrentMatchGroup, "CTFGameRules::GetCurrentMatchGroup");
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_etc_mm_spoof", "0", FCVAR_NOTIFY,
		"Mod: spoof a server into using Valve's matchmaking gamemodes",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
