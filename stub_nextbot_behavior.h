#ifndef _INCLUDE_SIGSEGV_STUB_NEXTBOT_BEHAVIOR_H_
#define _INCLUDE_SIGSEGV_STUB_NEXTBOT_BEHAVIOR_H_


template<class T> class Action;
template<class T> class Behavior;


enum class ActionTransition : int
{
	CONTINUE    = 0,
	CHANGE_TO   = 1,
	SUSPEND_FOR = 2,
	DONE        = 3,
	SUSTAIN     = 4,
};


enum class ResultSeverity : int
{
	/* this is 100% guesswork */
	LOW      = 1,
	MEDIUM   = 2,
	CRITICAL = 3,
};


template<class T>
struct ActionResult
{
	ActionTransition transition;
	Action<T> *action;
	const char *reason;
};


template<class T>
struct EventDesiredResult : public ActionResult<T>
{
	ResultSeverity severity;
};


#endif
