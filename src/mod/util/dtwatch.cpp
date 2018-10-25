#include "mod.h"


namespace Mod::Util::DTWatch
{
	
}



// GOAL: enhanced version of dtwatchent/dtwatchvar/dtwatchclass
// allow multiple inclusion filters
// allow multiple exclusion filters
// allow specifying individual entities by name etc rather than entindex
// colored output if possible


// ShouldWatchThisProp
// determines filtering

// Sendprop_UsingDebugWatch
// must return true for ShowEncodeDeltaWatchInfo to be called

// ShowEncodeDeltaWatchInfo
// "delta entity: %i\n"
// "+ %s %s, %s, index %i, bits %i, value %s\n"

// SendTable_WritePropList
// ConDMsg("= %i bits (%i bytes)\n")


// perhaps just hook into CBaseEntity::NetworkStateChanged?
// - calls CBaseEdict::StateChanged (2 versions)
// PROBLEM: all of this is inlined usually!
