#include "mod.h"
#include "util/backtrace.h"


namespace Mod_Debug_TF_Nav_Mesh_Trace
{
	// FireGameEvent
	// OnServerActivate
	// OnRoundRestart
	// CollectAmbushAreas
	// CollectAndMarkSpawnROomExits
	// CollectBuiltObjects
	// CollectControlPointAreas
	// CollectSpawnRoomThresholdAreas
	// ComputeBlockedAreas
	// ComputeIncursionDistances 1
	// ComputeIncursionDistances 2
	// ComputeInvasionAreas
	// DecorateMesh
	// OnBlockedAreasChanged
	// OnObjectChanged
	// RecomputeInternalData
	// RemoveAllMeshDecoration
	// ResetMeshAttributes
	// MarkSpawnRoomArea
	// RecomputeBlockers
	// ScheduleRecompute
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Debug:TF_Nav_Mesh_Trace")
		{
			
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_debug_tf_nav_mesh_trace", "0", FCVAR_NOTIFY,
		"Debug: trace TF Nav Mesh calls",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
