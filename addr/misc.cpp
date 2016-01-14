#include "addr/addr.h"


struct CAddr__CTFPlayer_CanBeForcedToLaugh : public CAddr_Sym
{
	const char *GetName() const override { return "CTFPlayer::CanBeForcedToLaugh"; }
	const char *GetSymbol() const override { return "_ZN9CTFPlayer18CanBeForcedToLaughEv"; }
};


// MSVC function-finding heuristics:
// - funcs always start at 16-byte boundaries
// - funcs often have 0xCC padding before they begin
// - 
