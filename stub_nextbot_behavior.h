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


template<class T>
ActionResult<T> Continue()
{
	return {
		.transition = ActionTransition::CONTINUE,
		.action     = nullptr,
		.reason     = nullptr,
	};
}

template<class T>
ActionResult<T> ChangeTo(Action<T> *next, const char *why = nullptr)
{
	return {
		.transition = ActionTransition::CHANGE_TO,
		.action     = next,
		.reason     = why,
	};
}

template<class T>
ActionResult<T> SuspendFor(Action<T> *next, const char *why = nullptr)
{
	return {
		.transition = ActionTransition::SUSPEND_FOR,
		.action     = next,
		.reason     = why,
	};
}

template<class T>
ActionResult<T> Done(const char *why = nullptr)
{
	return {
		.transition = ActionTransition::DONE,
		.action     = nullptr,
		.reason     = why,
	};
}

template<class T>
ActionResult<T> Sustain(const char *why = nullptr)
{
	return {
		.transition = ActionTransition::SUSTAIN,
		.action     = nullptr,
		.reason     = why,
	};
}


template<class T>
EventDesiredResult<T> Continue(ResultSeverity level = ResultSeverity::LOW)
{
	return {
		.transition = ActionTransition::CONTINUE,
		.action     = nullptr,
		.reason     = nullptr,
		.severity   = level,
	};
}

template<class T>
EventDesiredResult<T> ChangeTo(Action<T> *next, const char *why = nullptr, ResultSeverity level = ResultSeverity::LOW)
{
	return {
		.transition = ActionTransition::CHANGE_TO,
		.action     = next,
		.reason     = why,
		.severity   = level,
	};
}

template<class T>
EventDesiredResult<T> SuspendFor(Action<T> *next, const char *why = nullptr, ResultSeverity level = ResultSeverity::LOW)
{
	return {
		.transition = ActionTransition::SUSPEND_FOR,
		.action     = next,
		.reason     = why,
		.severity   = level,
	};
}

template<class T>
EventDesiredResult<T> Done(const char *why = nullptr, ResultSeverity level = ResultSeverity::LOW)
{
	return {
		.transition = ActionTransition::DONE,
		.action     = nullptr,
		.reason     = why,
		.severity   = level,
	};
}

template<class T>
EventDesiredResult<T> Sustain(const char *why = nullptr, ResultSeverity level = ResultSeverity::LOW)
{
	return {
		.transition = ActionTransition::SUSTAIN,
		.action     = nullptr,
		.reason     = why,
		.severity   = level,
	};
}


#endif
