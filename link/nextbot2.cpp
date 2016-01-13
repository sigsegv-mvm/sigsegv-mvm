#include "common.h"
#include "link/link.h"
#include "abi.h"


#if defined __GNUC__

static FuncThunk<void (*)(void *)> ft_INextBotEventResponder_dtor_D0("INextBotEventResponder::~INextBotEventResponder [D0]");
static FuncThunk<void (*)(void *)> ft_INextBotEventResponder_dtor_D2("INextBotEventResponder::~INextBotEventResponder [D2]");

static FuncThunk<void (*)(void *)> ft_IContextualQuery_dtor_D0("IContextualQuery::~IContextualQuery [D0]");
static FuncThunk<void (*)(void *)> ft_IContextualQuery_dtor_D2("IContextualQuery::~IContextualQuery [D2]");

static FuncThunk<void (*)(void *)> ft_Action_dtor_D2("Action<CTFBot>::~Action [D2]");

extern "C"
{
	/* dtors */
	
	void _ZN22INextBotEventResponderD0Ev(void *_this) { (*ft_INextBotEventResponder_dtor_D0)(_this); }
	void _ZN22INextBotEventResponderD2Ev(void *_this) { (*ft_INextBotEventResponder_dtor_D2)(_this); }
	
	void _ZN16IContextualQueryD0Ev(void *_this) { (*ft_IContextualQuery_dtor_D0)(_this); }
	void _ZN16IContextualQueryD2Ev(void *_this) { (*ft_IContextualQuery_dtor_D2)(_this); }
	
	void _ZN6ActionI6CTFBotED2Ev(void *_this) { (*ft_Action_dtor_D2)(_this); }
	
	
	/* the definition of the vtable/RTTI symbols can't be in a translation unit
	 * that includes the actual class definition */
	
	/* typeinfo */
	
	abi::__class_type_info _ZTI22INextBotEventResponder("22INextBotEventResponder");
	abi::__class_type_info _ZTI16IContextualQuery("16IIContextualQuery");
	
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

static FuncThunk<void (*)(void *)> ft_Action_dtor_D2("Action<CTFBot>::~Action [D2]");

#include "re/nextbot.h"

/* oh god... good luck declaring mangled symbols in C for MSVC++, given that
 * they use wonderful characters like "@$?"... */

/* these dtors don't actually do anything anyway */
INextBotEventResponder::~INextBotEventResponder() {}
IContextualQuery::~IContextualQuery() {}

/* nesting the dtor thunk inside an actual dtor is a bad idea, but it's better
 * than the horrible alternative */
template<> Action<CTFBot>::~Action() { (*ft_Action_dtor_D2)(this); }


// TODO: why didn't we have to hook up the RTTI?
// will dynamic_cast's be broken?

#endif
