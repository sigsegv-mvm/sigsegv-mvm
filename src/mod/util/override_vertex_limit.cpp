#include "mod.h"


namespace Mod::Util::Override_Vertex_Limit
{
	CON_COMMAND(sig_util_override_vertex_limit, "")
	{
		if (g_pMaterialSystem == nullptr) return;
		
		// TODO: call IMaterialSystem::Lock
		
		// APPROACH #1: for each render context:
		// - back up what's in their m_Vertices, m_Indices, and m_Primitives
		// - placement-new a new CMemoryStack with larger limits
		// - restore the contents into the new one
		
		// APPROACH #2: for each render context:
		// - reach into each CMemoryStack and live-update the limit values without breaking anything
		
		// TODO: call IMaterialSystem::Unlock
	}
}
