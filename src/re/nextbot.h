#ifndef _INCLUDE_SIGSEGV_RE_NEXTBOT_H_
#define _INCLUDE_SIGSEGV_RE_NEXTBOT_H_


#if defined _MSC_VER
#pragma message("NextBot classes have not been checked against the VC++ build!")
#endif


#include "stub/tfplayer.h"
#include "stub/nav.h"
#include "util/misc.h"


class CTFBot;
class INextBot;
class Path;
class PathFollower;
struct animevent_t;
class AI_Response;
class CBaseCombatWeapon;
class INextBotEntityFilter;
class INextBotReply;
class CNavLadder;
class NextBotCombatCharacter;
template<class T> class Action;
#define CUtlVectorAutoPurge CUtlVector

#include "../mvm-reversed/server/NextBot/NextBotKnownEntity.h"
#include "../mvm-reversed/server/NextBot/NextBotEventResponderInterface.h"
#include "../mvm-reversed/server/NextBot/NextBotContextualQueryInterface.h"
#include "../mvm-reversed/server/NextBot/NextBotComponentInterface.h"
#include "../mvm-reversed/server/NextBot/NextBotIntentionInterface.h"
#include "../mvm-reversed/server/NextBot/NextBotBodyInterface.h"
#include "../mvm-reversed/server/NextBot/NextBotLocomotionInterface.h"
#include "../mvm-reversed/server/NextBot/NextBotVisionInterface.h"
#include "../mvm-reversed/server/NextBot/NextBotInterface.h"
#include "../mvm-reversed/server/NextBot/NextBotBehavior.h"
// TODO: NextBot/Player/PlayerBody.h
// TODO: NextBot/Player/PlayerLocomotion.h
#include "../mvm-reversed/server/NextBot/NextBotManager.h"


SIZE_CHECK(CKnownEntity,               0x30); // 0x2d
SIZE_CHECK(INextBotEventResponder,     0x04);
SIZE_CHECK(IContextualQuery,           0x04);
SIZE_CHECK(INextBotComponent,          0x14);
SIZE_CHECK(IIntention,                 0x18);
SIZE_CHECK(IBody,                      0x14);
SIZE_CHECK(ILocomotion,                0x58);
SIZE_CHECK(IVision,                    0xc4);
SIZE_CHECK(INextBot,                   0x60);
SIZE_CHECK(Behavior<CTFBot>,           0x50);
SIZE_CHECK(Action<CTFBot>,             0x34); // 0x32
SIZE_CHECK(ActionResult<CTFBot>,       0x0c);
SIZE_CHECK(EventDesiredResult<CTFBot>, 0x10);
SIZE_CHECK(NextBotManager,             0x50);


/* NextBotKnownEntity.cpp */
inline CKnownEntity::~CKnownEntity() {}

/* NextBotBehavior.cpp */
template<class T> inline Action<T>::Action() :
	m_ActionParent(nullptr), m_ActionChild(nullptr),
	m_ActionWeSuspended(nullptr), m_ActionSuspendedUs(nullptr),
	m_Actor(nullptr)
{
	this->m_Behavior = nullptr;
	this->m_bStarted = false;
	this->m_bSuspended = false;
	
	memset((void *)&this->m_Result, 0x00, 0x10);
}
template<class T> inline Behavior<T> *Action<T>::GetBehavior() const { return this->m_Behavior; }
template<class T> inline T *Action<T>::GetActor() const              { return this->m_Actor; }


/* wrapper for Action<CTFBot> which can be rapidly stopped at mod unload time */
template<class T>
class IHotplugAction : public Action<CTFBot>, public AutoList<IHotplugAction<T>>
{
public:
	void Unload()
	{
		DevMsg("BEGIN IHotplugAction<%s>::Unload\n", TypeName<T>());
		
		/* this code is mostly the same as the DONE case of ApplyResult, but we
		 * don't put ourselves on the behavior's deferred deletion list */
		
		auto actor    = this->GetActor();
		auto behavior = this->GetBehavior();
		
		this->InvokeOnEnd(actor, behavior, this->m_ActionWeSuspended);
		
		if (this->m_ActionWeSuspended != nullptr) {
			ActionResult<CTFBot> result = this->m_ActionWeSuspended->InvokeOnResume(actor, behavior, this);
			this->m_ActionWeSuspended->ApplyResult(actor, behavior, result);
		}
		
		DevMsg("END   IHotplugAction<%s>::Unload\n", TypeName<T>());
	}
	
	static void UnloadAll()
	{
		/* REMOVE ME */
		Dump();
		
		
		DevMsg("BEGIN IHotplugAction<%s>::UnloadAll\n", TypeName<T>());
		
		for (auto action : AutoList<IHotplugAction<T>>::List()) {
			action->Unload();
		}
		
		while (!AutoList<IHotplugAction<T>>::List().empty()) {
			auto action = AutoList<IHotplugAction<T>>::List().front();
			delete action;
		}
		
		DevMsg("END   IHotplugAction<%s>::UnloadAll\n", TypeName<T>());
		
		
		/* REMOVE ME */
		Dump();
	}
	
	
	static void Dump();
	static void DumpRecursive(INextBotEventResponder *iner, int level);
};


CTFBot *ToTFBot(CBaseEntity *ent);


template<class T>
inline void IHotplugAction<T>::Dump()
{
	for (int i = 1; i <= 32; ++i) {
		CBasePlayer *player = UTIL_PlayerByIndex(i);
		if (player == nullptr) continue;
		
		CTFBot *bot = ToTFBot(player);
		if (bot == nullptr) continue;
		
		auto nextbot = rtti_cast<INextBot *>(bot);
		if (nextbot == nullptr) continue;
		
		DevMsg("\nBot \"%s\"\n", player->GetPlayerName());
		DumpRecursive(nextbot, 1);
	}
}

template<class T>
inline void IHotplugAction<T>::DumpRecursive(INextBotEventResponder *iner, int level)
{
	DevMsg("%*s- %08x %s\n", (level * 2) - 1, " ", (uintptr_t)iner, typeid(*iner).name());
	
	for (auto r = iner->FirstContainedResponder(); r != nullptr;
		r = iner->NextContainedResponder(r)) {
		DumpRecursive(r, level + 1);
	}
}


#endif
