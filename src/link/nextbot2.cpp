#include "link/link.h"
#include "abi.h"


#if defined __clang__


#error TODO


#elif defined __GNUC__


#warning TODO: check ALL addrs here in gamedata files!!!
class Dummy {};

#define CTOR_THUNK_Cn(name, mngl1, mngl2, str, n, ...) static MemberFuncThunk<Dummy *, void __VA_OPT__(, __VA_ARGS__)> ft_##name##_C##n(str " [C" #n "]"); extern "C" void _Z##mngl1##C##n##mngl2(void *_this __VA_OPT__(, __VA_ARGS__)) { ft_##name##_C##n(reinterpret_cast<Dummy *>(_this) __VA_OPT__(, __VA_ARGS__)); }
#define CTOR_THUNK_ALL(name, mngl1, mngl2, str, ...) CTOR_THUNK_Cn(name, mngl1, mngl2, str, 1 __VA_OPT__(, __VA_ARGS__)) CTOR_THUNK_Cn(name, mngl1, mngl2, str, 2 __VA_OPT__(, __VA_ARGS__))

#define DTOR_THUNK_Dn(name, mngl, str, n) static MemberFuncThunk<Dummy *, void> ft_##name##_D##n(str " [D" #n "]"); extern "C" void _Z##mngl##D##n##Ev(void *_this) { ft_##name##_D##n(reinterpret_cast<Dummy *>(_this)); }
#define DTOR_THUNK_ALL(name, mngl, str) DTOR_THUNK_Dn(name, mngl, str, 0) DTOR_THUNK_Dn(name, mngl, str, 1) DTOR_THUNK_Dn(name, mngl, str, 2)


#if 0
/* CKnownEntity */
// TODO: ctors (inlined)
DTOR_THUNK_ALL(CKnownEntity, N12CKnownEntity, "CKnownEntity::~CKnownEntity");

/* INextBotEventResponder */
// TODO: ctors (inlined)
DTOR_THUNK_ALL(INextBotEventResponder, N22INextBotEventResponder, "INextBotEventResponder::~INextBotEventResponder");

/* IContextualQuery */
// TODO: ctors (inlined)
DTOR_THUNK_ALL(IContextualQuery, N16IContextualQuery, "IContextualQuery::~IContextualQuery");

/* Path */
CTOR_THUNK_ALL(Path, N4Path, Ev, "Path::Path");
DTOR_THUNK_ALL(Path, N4Path,     "Path::~Path");

/* PathFollower */
CTOR_THUNK_ALL(PathFollower, N12PathFollower, Ev, "PathFollower::PathFollower");
DTOR_THUNK_ALL(PathFollower, N12PathFollower,     "PathFollower::~PathFollower");

/* ChasePath */
// TODO: ctors (inlined)
DTOR_THUNK_ALL(ChasePath, N9ChasePath, "ChasePath::~ChasePath");

/* Behavior<CTFBot> */
// TODO: ctors (inlined)
DTOR_THUNK_ALL(Behavior_CTFBot, N8BehaviorI6CTFBotE, "Behavior<CTFBot>::~Behavior");

/* Action<CTFBot> */
// TODO: ctors (inlined)
DTOR_THUNK_ALL(Action_CTFBot, N6ActionI6CTFBotE, "Action<CTFBot>::~Action");

/* NextBotManager */
CTOR_THUNK_ALL(NextBotManager, N14NextBotManager, Ev, "NextBotManager::NextBotManager");
DTOR_THUNK_ALL(NextBotManager, N14NextBotManager,     "NextBotManager::~NextBotManager");
#endif


#undef DTOR_THUNK_ALL
#undef DTOR_THUNK_Dn
#undef CTOR_THUNK_ALL
#undef CTOR_THUNK_Cn


#warning TODO REMOVE THESE SOON!!!
extern "C" void _ZN12PathFollowerC1Ev(void *_this);
extern "C" void _ZN12PathFollowerD1Ev(void *_this);
extern "C" void _ZN6ActionI6CTFBotED2Ev(void *_this);
static MemberFuncThunk<Dummy *, void> ft_PathFollower_ctor_C1("PathFollower::PathFollower [C1]");
static MemberFuncThunk<Dummy *, void> ft_PathFollower_dtor_D1("PathFollower::~PathFollower [D1]");
static MemberFuncThunk<Dummy *, void> ft_Action_dtor_D2("Action<CTFBot>::~Action [D2]");
extern "C" void _ZN12PathFollowerC1Ev(void *_this) { ft_PathFollower_ctor_C1(reinterpret_cast<Dummy *>(_this)); }
extern "C" void _ZN12PathFollowerD1Ev(void *_this) { ft_PathFollower_dtor_D1(reinterpret_cast<Dummy *>(_this)); }
extern "C" void _ZN6ActionI6CTFBotED2Ev(void *_this) { ft_Action_dtor_D2(reinterpret_cast<Dummy *>(_this)); }


/* the definition of the vtable/RTTI symbols can't be in a translation unit that
 * includes the actual class definition */

#define THUNK_VTABLE(name, size) \
	extern "C" { uint8_t _ZTV##name[size]; } \
	static VTableThunk<sizeof(_ZTV##name)> thunk_vtable_##name(#name, _ZTV##name);

#define THUNK_TYPEINFO_BASE(name) \
	extern "C" { uint8_t _ZTI##name[sizeof(abi::__class_type_info)]; } \
	static TypeInfoThunk<sizeof(_ZTI##name)> thunk_typeinfo_##name(#name, _ZTI##name);
#define THUNK_TYPEINFO_SINGLE(name) \
	extern "C" { uint8_t _ZTI##name[sizeof(abi::__si_class_type_info)]; } \
	static TypeInfoThunk<sizeof(_ZTI##name)> thunk_typeinfo_##name(#name, _ZTI##name);
#define THUNK_TYPEINFO_VMULTI(name, nbases) \
	extern "C" { uint8_t _ZTI##name[sizeof(abi::__vmi_class_type_info) + ((nbases - 1) * sizeof(abi::__base_class_type_info))]; } \
	static TypeInfoThunk<sizeof(_ZTI##name)> thunk_typeinfo_##name(#name, _ZTI##name);


THUNK_TYPEINFO_BASE(12CKnownEntity);
THUNK_VTABLE(12CKnownEntity, 0x80);

THUNK_TYPEINFO_BASE(22INextBotEventResponder);
THUNK_VTABLE(22INextBotEventResponder, 0x100);

THUNK_TYPEINFO_BASE(16IContextualQuery);
THUNK_VTABLE(16IContextualQuery, 0x40);

THUNK_TYPEINFO_SINGLE(8INextBot);
THUNK_VTABLE(8INextBot, 0x200);


#undef THUNK_TYPEINFO_VMULTI
#undef THUNK_TYPEINFO_SINGLE
#undef THUNK_TYPEINFO_BASE
#undef THUNK_VTABLE


#elif defined _MSC_VER


#include "re/nextbot.h"
#include "re/path.h"

static MemberFuncThunk<Action<CTFBot> *, void> ft_Action_dtor_D2("Action<CTFBot>::~Action [D2]");

/* oh god... good luck declaring mangled symbols in C for MSVC++, given that
 * they use wonderful characters like "@$?"... */

/* nesting the dtor thunk inside an actual dtor is a bad idea, but it's better
 * than the horrible alternative */
template<> Action<CTFBot>::~Action() { ft_Action_dtor_D2(this); }

/* not implemented */
INextBotEventResponder::~INextBotEventResponder() { assert(false); }
IContextualQuery::~IContextualQuery()             { assert(false); }
//Path::Path()                                      { assert(false); }
//Path::~Path()                                     { assert(false); }
//PathFollower::PathFollower()                      { assert(false); }
//PathFollower::~PathFollower()                     { assert(false); }


// TODO: why didn't we have to hook up the RTTI?
// will dynamic_cast's be broken?


#endif
