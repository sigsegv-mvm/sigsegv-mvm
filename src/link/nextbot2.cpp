#include "link/link.h"
#include "abi.h"


#if defined __GNUC__

class Dummy {};


static MemberFuncThunk<Dummy *, void> ft_PathFollower_ctor_C1("PathFollower::PathFollower [C1]");
static MemberFuncThunk<Dummy *, void> ft_PathFollower_dtor_D1("PathFollower::~PathFollower [D1]");

static MemberFuncThunk<Dummy *, void> ft_INextBotEventResponder_dtor_D0("INextBotEventResponder::~INextBotEventResponder [D0]");
static MemberFuncThunk<Dummy *, void> ft_INextBotEventResponder_dtor_D2("INextBotEventResponder::~INextBotEventResponder [D2]");

static MemberFuncThunk<Dummy *, void> ft_IContextualQuery_dtor_D0("IContextualQuery::~IContextualQuery [D0]");
static MemberFuncThunk<Dummy *, void> ft_IContextualQuery_dtor_D2("IContextualQuery::~IContextualQuery [D2]");

static MemberFuncThunk<Dummy *, void> ft_Action_dtor_D2("Action<CTFBot>::~Action [D2]");

extern "C"
{
	/* ctors */
	
	void _ZN12PathFollowerC1Ev(void *_this) { ft_PathFollower_ctor_C1(reinterpret_cast<Dummy *>(_this)); }
	
	
	/* dtors */
	
	void _ZN22INextBotEventResponderD0Ev(void *_this) { ft_INextBotEventResponder_dtor_D0(reinterpret_cast<Dummy *>(_this)); }
	void _ZN22INextBotEventResponderD2Ev(void *_this) { ft_INextBotEventResponder_dtor_D2(reinterpret_cast<Dummy *>(_this)); }
	
	void _ZN16IContextualQueryD0Ev(void *_this) { ft_IContextualQuery_dtor_D0(reinterpret_cast<Dummy *>(_this)); }
	void _ZN16IContextualQueryD2Ev(void *_this) { ft_IContextualQuery_dtor_D2(reinterpret_cast<Dummy *>(_this)); }
	
	void _ZN12PathFollowerD1Ev(void *_this) { ft_PathFollower_dtor_D1(reinterpret_cast<Dummy *>(_this)); }
		
	void _ZN6ActionI6CTFBotED2Ev(void *_this) { ft_Action_dtor_D2(reinterpret_cast<Dummy *>(_this)); }
	
	
	/* the definition of the vtable/RTTI symbols can't be in a translation unit
	 * that includes the actual class definition */
	
	/* typeinfo */
	
	abi::__class_type_info _ZTI22INextBotEventResponder("22INextBotEventResponder");
	abi::__class_type_info _ZTI16IContextualQuery("16IContextualQuery");
	abi::__si_class_type_info _ZTI8INextBot("8INextBot", &_ZTI22INextBotEventResponder);
	
	/* vtables */
	
	void VFuncNotImpl_INextBotEventResponder() { abort(); }
	vtable _ZTV22INextBotEventResponder = {
		0x00000000,
		&_ZTI22INextBotEventResponder,
		{
			(void *)&_ZN22INextBotEventResponderD2Ev,
			(void *)&_ZN22INextBotEventResponderD0Ev,
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
			(void *)&VFuncNotImpl_INextBotEventResponder, // TODO
		}
	};
	
	void VFuncNotImpl_IContextualQuery() { abort(); }
	vtable _ZTV16IContextualQuery = {
		0x00000000,
		&_ZTI16IContextualQuery,
		{
			(void *)&_ZN16IContextualQueryD2Ev,
			(void *)&_ZN16IContextualQueryD0Ev,
			(void *)&VFuncNotImpl_IContextualQuery, // TODO
			(void *)&VFuncNotImpl_IContextualQuery, // TODO
			(void *)&VFuncNotImpl_IContextualQuery, // TODO
			(void *)&VFuncNotImpl_IContextualQuery, // TODO
			(void *)&VFuncNotImpl_IContextualQuery, // TODO
			(void *)&VFuncNotImpl_IContextualQuery, // TODO
			(void *)&VFuncNotImpl_IContextualQuery, // TODO
			(void *)&VFuncNotImpl_IContextualQuery, // TODO
		}
	};
}

#elif defined _MSC_VER

#include "re/nextbot.h"

static MemberFuncThunk<Action<CTFBot> *, void> ft_Action_dtor_D2("Action<CTFBot>::~Action [D2]");

/* oh god... good luck declaring mangled symbols in C for MSVC++, given that
 * they use wonderful characters like "@$?"... */

/* these dtors don't actually do anything anyway */
INextBotEventResponder::~INextBotEventResponder() {}
IContextualQuery::~IContextualQuery() {}

/* nesting the dtor thunk inside an actual dtor is a bad idea, but it's better
 * than the horrible alternative */
template<> Action<CTFBot>::~Action() { ft_Action_dtor_D2(this); }


// TODO: why didn't we have to hook up the RTTI?
// will dynamic_cast's be broken?

#endif
