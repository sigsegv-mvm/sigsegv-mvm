#include "mod.h"


// these could be pretty useful:
// GetAllStaticProps
// GetAllStaticPropsInAABB
// GetAllStaticPropsInOBB


namespace Mod_Debug_Static_Props
{
	CON_COMMAND(sig_debug_static_props, "")
	{
		for (int i = 0; i < 4096; ++i) {
			ICollideable *collideable = staticpropmgr->GetStaticPropByIndex(i);
			if (collideable == nullptr) continue;
			
			int entindex = -1;
			IHandleEntity *ehandle = collideable->GetEntityHandle();
			if (ehandle != nullptr) {
				const CBaseHandle& handle = ehandle->GetRefEHandle();
				entindex = handle.ToInt();
			}
			
			Msg("Prop #%d: collideable @ 0x%08x, entity #%d\n", i, (uintptr_t)collideable, entindex);
			
			NDebugOverlay::Box(collideable->GetCollisionOrigin(), collideable->OBBMins(), collideable->OBBMaxs(), 0xff, 0xff, 0xff, 0x20, 3600.0f);
			NDebugOverlay::EntityTextAtPosition(collideable->GetCollisionOrigin(), 0, CFmtStrN<256>("#%d", i), 3600.0f, 0xff, 0xff, 0xff, 0xff);
		}
	}
}
