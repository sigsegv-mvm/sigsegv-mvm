#ifndef _INCLUDE_SIGSEGV_RE_NEXTBOT_H_
#define _INCLUDE_SIGSEGV_RE_NEXTBOT_H_


#if defined _MSC_VER
#pragma message("NextBot classes have not been checked against the VC++ build!")
#endif


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
template<typename T> class Action;

// TODO: uhhh, this redefinition seems... bad...
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


SIZE_CHECK(CKnownEntity,               0x0030); // 0x002d
SIZE_CHECK(INextBotEventResponder,     0x0004);
SIZE_CHECK(IContextualQuery,           0x0004);
SIZE_CHECK(INextBotComponent,          0x0014);
SIZE_CHECK(IIntention,                 0x0018);
SIZE_CHECK(IBody,                      0x0014);
SIZE_CHECK(ILocomotion,                0x0058);
SIZE_CHECK(IVision,                    0x00c4);
SIZE_CHECK(INextBot,                   0x0060);
SIZE_CHECK(Behavior<CTFBot>,           0x0050);
SIZE_CHECK(Action<CTFBot>,             0x0034); // 0x0032
SIZE_CHECK(ActionResult<CTFBot>,       0x000c);
SIZE_CHECK(EventDesiredResult<CTFBot>, 0x0010);
SIZE_CHECK(NextBotManager,             0x0050);


/* NextBotKnownEntity.cpp */
inline CKnownEntity::~CKnownEntity() {}

/* NextBotContextualQueryInterface.cpp */
inline IContextualQuery::~IContextualQuery() {}

/* NextBotEventResponder.cpp */
inline INextBotEventResponder::~INextBotEventResponder() {}

/* NextBotBehavior.cpp */
template<typename T> Action<T>::~Action() {}
template<typename T> Action<T>::Action() :
	m_ActionParent(nullptr), m_ActionChild(nullptr),
	m_ActionWeSuspended(nullptr), m_ActionSuspendedUs(nullptr),
	m_Actor(nullptr)
{
	this->m_Behavior = nullptr;
	this->m_bStarted = false;
	this->m_bSuspended = false;
	
	memset((void *)&this->m_Result, 0x00, 0x10);
}
template<typename T> Behavior<T> *Action<T>::GetBehavior() const { return this->m_Behavior; }
template<typename T> T           *Action<T>::GetActor()    const { return this->m_Actor; }


class IHotplugActionBase : public AutoList<IHotplugActionBase>
{
public:
	static void UnloadAll()
	{
		std::set<INextBot *> actors;
		
		for (auto action : AutoList<IHotplugActionBase>::List()) {
			auto actor = action->GetActorAsINextBot();
			if (actor == nullptr) continue;
			
			actors.insert(actor);
		}
		
		Msg("IHotplugActionBase::UnloadAll: calling Reset() on %zu NextBot intention interfaces\n", actors.size());
		
		for (auto actor : actors) {
			actor->GetIntentionInterface()->Reset();
		}
		
		assert(AutoList<IHotplugActionBase>::List().empty());
	}
	
protected:
	virtual INextBot *GetActorAsINextBot() const = 0;
};


/* IHotplugAction: wrapper for Action<T> which can be rapidly stopped at mod unload time */
template<typename T>
class IHotplugAction : public Action<T>, public IHotplugActionBase
{
public:
	virtual INextBot *GetActorAsINextBot() const override final { return rtti_cast<INextBot *>(this->GetActor()); }
};


/* ActionStub: provides a base for interoperating with real Action<T> objects in the game */
template<typename T>
class ActionStub : public Action<T>
{
public:
	virtual const char *GetName() const override { return nullptr; }
	
protected:
	ActionStub() = default;
};


#endif
