#include "mod.h"
#include "sm/detours.h"
#include "stub/stub.h"
#include "util/util.h"


// TODO: make video with this:
// stock RL vs direct hit
// stock GL vs loch n load
// stock RL: RS 0/1/2/3/4


namespace Mod_Visualize_Blast_Radius
{
	
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Visualize:Blast_Radius")
		{
			
		}
		
		void SetEnabled(bool enable)
		{
			this->ToggleAllDetours(enable);
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_visualize_blast_radius", "0", FCVAR_NOTIFY,
		"Visualization: draw sphere representing blast radius",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.SetEnabled(var.GetBool());
		});
}
