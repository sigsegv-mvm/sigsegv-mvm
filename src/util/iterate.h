#ifndef _INCLUDE_SIGSEGV_UTIL_ITERATE_H_
#define _INCLUDE_SIGSEGV_UTIL_ITERATE_H_


/* magic template wizardry to make function pointers or C++ lambda closures
 * work as functor parameters, whether they have void or bool return type */
template<typename RET>
struct IterateInternals
{
	template<typename FUNCTOR, typename... ARGS>
	static bool CallFunc(const FUNCTOR& functor, ARGS... args);
};
template<> template<typename FUNCTOR, typename... ARGS>
inline bool IterateInternals<void>::CallFunc(const FUNCTOR& func, ARGS... args)
{
	func(std::forward<ARGS>(args)...);
	return true;
}
template<> template<typename FUNCTOR, typename... ARGS>
inline bool IterateInternals<bool>::CallFunc(const FUNCTOR& func, ARGS... args)
{
	return func(std::forward<ARGS>(args)...);
}
#define CALL_FUNCTOR(...) IterateInternals<typename std::result_of<FUNCTOR(__VA_ARGS__)>::type>::CallFunc


#ifdef _INCLUDE_SIGSEGV_STUB_BASEENTITY_H_
template<typename FUNCTOR>
inline void ForEachEntity(const FUNCTOR& func)
{
	using T = CBaseEntity;
	
	for (int i = 1; i < MAX_EDICTS; ++i) {
		T *ent = UTIL_EntityByIndex(i);
		if (ent == nullptr) continue;
		
		if (!CALL_FUNCTOR(T *)(func, ent)) break;
	}
}
#endif


#ifdef _INCLUDE_SIGSEGV_STUB_BASEPLAYER_H_
template<typename FUNCTOR>
inline void ForEachPlayer(const FUNCTOR& func)
{
	using T = CBasePlayer;
	
	for (int i = 1; i <= gpGlobals->maxClients; ++i) {
		T *player = UTIL_PlayerByIndex(i);
		if (player == nullptr) continue;
		
		if (!CALL_FUNCTOR(T *)(func, player)) break;
	}
}

template<typename FUNCTOR>
inline void ForEachBot(const FUNCTOR& func)
{
	using T = CBasePlayer;
	
	for (int i = 1; i <= gpGlobals->maxClients; ++i) {
		T *player = UTIL_PlayerByIndex(i);
		if (player == nullptr) continue;
		if (!player->IsBot())  continue;
		
		if (!CALL_FUNCTOR(T *)(func, player)) break;
	}
}
#endif


#ifdef _INCLUDE_SIGSEGV_STUB_TFPLAYER_H_
template<typename FUNCTOR>
inline void ForEachTFPlayer(const FUNCTOR& func)
{
	using T = CTFPlayer;
	
	for (int i = 1; i <= gpGlobals->maxClients; ++i) {
		T *player = ToTFPlayer(UTIL_PlayerByIndex(i));
		if (player == nullptr) continue;
		
		if (!CALL_FUNCTOR(T *)(func, player)) break;
	}
}
#endif


#ifdef _INCLUDE_SIGSEGV_STUB_TFBOT_H_
template<typename FUNCTOR>
inline void ForEachTFBot(const FUNCTOR& func)
{
	using T = CTFBot;
	
	for (int i = 1; i <= gpGlobals->maxClients; ++i) {
		T *bot = ToTFBot(UTIL_PlayerByIndex(i));
		if (bot == nullptr) continue;
		
		if (!CALL_FUNCTOR(T *)(func, bot)) break;
	}
}
#endif


#undef CALL_FUNCTOR


#endif
