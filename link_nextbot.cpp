#include "link.h"
#include "extension.h"
#include "util.h"
#include "re_nextbot.h"


#define INER INextBotEventResponder
#define AR   ActionResult<CTFBot>
#define EDR  EventDesiredResult<CTFBot>

static FuncThunk<void (*)(INextBotEventResponder *)> ft_INextBotEventResponder_dtor_D0("INextBotEventResponder::~INextBotEventResponder [D0]");
static FuncThunk<void (*)(INextBotEventResponder *)> ft_INextBotEventResponder_dtor_D2("INextBotEventResponder::~INextBotEventResponder [D2]");

static FuncThunk<void                 (*)(IContextualQuery *)>                                                                                                          ft_IContextualQuery_dtor_D0(                  "IContextualQuery::~IContextualQuery [D0]");
static FuncThunk<void                 (*)(IContextualQuery *)>                                                                                                          ft_IContextualQuery_dtor_D2(                  "IContextualQuery::~IContextualQuery [D2]");
static FuncThunk<QueryResponse        (*)(const IContextualQuery *, const INextBot *nextbot, CBaseEntity *)>                                                            ft_IContextualQuery_ShouldPickUp(             "IContextualQuery::ShouldPickUp");
static FuncThunk<QueryResponse        (*)(const IContextualQuery *, const INextBot *nextbot)>                                                                           ft_IContextualQuery_ShouldHurry(              "IContextualQuery::ShouldHurry");
static FuncThunk<QueryResponse        (*)(const IContextualQuery *, const INextBot *nextbot)>                                                                           ft_IContextualQuery_ShouldRetreat(            "IContextualQuery::ShouldRetreat");
static FuncThunk<QueryResponse        (*)(const IContextualQuery *, const INextBot *nextbot, const CKnownEntity *)>                                                     ft_IContextualQuery_ShouldAttack(             "IContextualQuery::ShouldAttack");
static FuncThunk<QueryResponse        (*)(const IContextualQuery *, const INextBot *nextbot, CBaseEntity *)>                                                            ft_IContextualQuery_IsHindrance(              "IContextualQuery::IsHindrance");
static FuncThunk<Vector               (*)(const IContextualQuery *, const INextBot *nextbot, const CBaseCombatCharacter *)>                                             ft_IContextualQuery_SelectTargetPoint(        "IContextualQuery::SelectTargetPoint");
static FuncThunk<QueryResponse        (*)(const IContextualQuery *, const INextBot *nextbot, const Vector&)>                                                            ft_IContextualQuery_IsPositionAllowed(        "IContextualQuery::IsPositionAllowed");
static FuncThunk<const CKnownEntity * (*)(const IContextualQuery *, const INextBot *nextbot, const CBaseCombatCharacter *, const CKnownEntity *, const CKnownEntity *)> ft_IContextualQuery_SelectMoreDangerousThreat("IContextualQuery::SelectMoreDangerousThreat");

static FuncThunk<void             (*)(Action<CTFBot> *)>                                                                     ft_Action_dtor_D2(                            "Action<CTFBot>::~Action [D2]");
static FuncThunk<INER *           (*)(const Action<CTFBot> *)>                                                               ft_Action_INER_FirstContainedResponder(       "INER Action<CTFBot>::FirstContainedResponder");
static FuncThunk<INER *           (*)(const Action<CTFBot> *, INER *)>                                                       ft_Action_INER_NextContainedResponder(        "INER Action<CTFBot>::NextContainedResponder");
static FuncThunk<void             (*)(Action<CTFBot> *, CBaseEntity *)>                                                      ft_Action_INER_OnLeaveGround(                 "INER Action<CTFBot>::OnLeaveGround");
static FuncThunk<void             (*)(Action<CTFBot> *, CBaseEntity *)>                                                      ft_Action_INER_OnLandOnGround(                "INER Action<CTFBot>::OnLandOnGround");
static FuncThunk<void             (*)(Action<CTFBot> *, CBaseEntity *, CGameTrace *)>                                        ft_Action_INER_OnContact(                     "INER Action<CTFBot>::OnContact");
static FuncThunk<void             (*)(Action<CTFBot> *, const Path *)>                                                       ft_Action_INER_OnMoveToSuccess(               "INER Action<CTFBot>::OnMoveToSuccess");
static FuncThunk<void             (*)(Action<CTFBot> *, const Path *, INER::MoveToFailureType)>                              ft_Action_INER_OnMoveToFailure(               "INER Action<CTFBot>::OnMoveToFailure");
static FuncThunk<void             (*)(Action<CTFBot> *)>                                                                     ft_Action_INER_OnStuck(                       "INER Action<CTFBot>::OnStuck");
static FuncThunk<void             (*)(Action<CTFBot> *)>                                                                     ft_Action_INER_OnUnStuck(                     "INER Action<CTFBot>::OnUnStuck");
static FuncThunk<void             (*)(Action<CTFBot> *)>                                                                     ft_Action_INER_OnPostureChanged(              "INER Action<CTFBot>::OnPostureChanged");
static FuncThunk<void             (*)(Action<CTFBot> *, int)>                                                                ft_Action_INER_OnAnimationActivityComplete(   "INER Action<CTFBot>::OnAnimationActivityComplete");
static FuncThunk<void             (*)(Action<CTFBot> *, int)>                                                                ft_Action_INER_OnAnimationActivityInterrupted("INER Action<CTFBot>::OnAnimationActivityInterrupted");
static FuncThunk<void             (*)(Action<CTFBot> *, animevent_t *)>                                                      ft_Action_INER_OnAnimationEvent(              "INER Action<CTFBot>::OnAnimationEvent");
static FuncThunk<void             (*)(Action<CTFBot> *)>                                                                     ft_Action_INER_OnIgnite(                      "INER Action<CTFBot>::OnIgnite");
static FuncThunk<void             (*)(Action<CTFBot> *, const CTakeDamageInfo&)>                                             ft_Action_INER_OnInjured(                     "INER Action<CTFBot>::OnInjured");
static FuncThunk<void             (*)(Action<CTFBot> *, const CTakeDamageInfo&)>                                             ft_Action_INER_OnKilled(                      "INER Action<CTFBot>::OnKilled");
static FuncThunk<void             (*)(Action<CTFBot> *, CBaseCombatCharacter *, const CTakeDamageInfo&)>                     ft_Action_INER_OnOtherKilled(                 "INER Action<CTFBot>::OnOtherKilled");
static FuncThunk<void             (*)(Action<CTFBot> *, CBaseEntity *)>                                                      ft_Action_INER_OnSight(                       "INER Action<CTFBot>::OnSight");
static FuncThunk<void             (*)(Action<CTFBot> *, CBaseEntity *)>                                                      ft_Action_INER_OnLostSight(                   "INER Action<CTFBot>::OnLostSight");
static FuncThunk<void             (*)(Action<CTFBot> *, CBaseEntity *, const Vector&, KeyValues *)>                          ft_Action_INER_OnSound(                       "INER Action<CTFBot>::OnSound");
static FuncThunk<void             (*)(Action<CTFBot> *, CBaseCombatCharacter *, const char *, AI_Response *)>                ft_Action_INER_OnSpokeConcept(                "INER Action<CTFBot>::OnSpokeConcept");
static FuncThunk<void             (*)(Action<CTFBot> *, CBaseCombatCharacter *, CBaseCombatWeapon *)>                        ft_Action_INER_OnWeaponFired(                 "INER Action<CTFBot>::OnWeaponFired");
static FuncThunk<void             (*)(Action<CTFBot> *, CNavArea *, CNavArea *)>                                             ft_Action_INER_OnNavAreaChanged(              "INER Action<CTFBot>::OnNavAreaChanged");
static FuncThunk<void             (*)(Action<CTFBot> *)>                                                                     ft_Action_INER_OnModelChanged(                "INER Action<CTFBot>::OnModelChanged");
static FuncThunk<void             (*)(Action<CTFBot> *, CBaseEntity *, CBaseCombatCharacter *)>                              ft_Action_INER_OnPickUp(                      "INER Action<CTFBot>::OnPickUp");
static FuncThunk<void             (*)(Action<CTFBot> *, CBaseEntity *)>                                                      ft_Action_INER_OnDrop(                        "INER Action<CTFBot>::OnDrop");
static FuncThunk<void             (*)(Action<CTFBot> *, CBaseCombatCharacter *, int)>                                        ft_Action_INER_OnActorEmoted(                 "INER Action<CTFBot>::OnActorEmoted");
static FuncThunk<void             (*)(Action<CTFBot> *, CBaseEntity *)>                                                      ft_Action_INER_OnCommandAttack(               "INER Action<CTFBot>::OnCommandAttack");
static FuncThunk<void             (*)(Action<CTFBot> *, const Vector&, float)>                                               ft_Action_INER_OnCommandApproach_vec(         "INER Action<CTFBot>::OnCommandApproach(vec)");
static FuncThunk<void             (*)(Action<CTFBot> *, CBaseEntity *)>                                                      ft_Action_INER_OnCommandApproach_ent(         "INER Action<CTFBot>::OnCommandApproach(ent)");
static FuncThunk<void             (*)(Action<CTFBot> *, CBaseEntity *, float)>                                               ft_Action_INER_OnCommandRetreat(              "INER Action<CTFBot>::OnCommandRetreat");
static FuncThunk<void             (*)(Action<CTFBot> *, float)>                                                              ft_Action_INER_OnCommandPause(                "INER Action<CTFBot>::OnCommandPause");
static FuncThunk<void             (*)(Action<CTFBot> *)>                                                                     ft_Action_INER_OnCommandResume(               "INER Action<CTFBot>::OnCommandResume");
static FuncThunk<void             (*)(Action<CTFBot> *, const char *)>                                                       ft_Action_INER_OnCommandString(               "INER Action<CTFBot>::OnCommandString");
static FuncThunk<void             (*)(Action<CTFBot> *, CBaseEntity *)>                                                      ft_Action_INER_OnShoved(                      "INER Action<CTFBot>::OnShoved");
static FuncThunk<void             (*)(Action<CTFBot> *, CBaseEntity *)>                                                      ft_Action_INER_OnBlinded(                     "INER Action<CTFBot>::OnBlinded");
static FuncThunk<void             (*)(Action<CTFBot> *, int)>                                                                ft_Action_INER_OnTerritoryContested(          "INER Action<CTFBot>::OnTerritoryContested");
static FuncThunk<void             (*)(Action<CTFBot> *, int)>                                                                ft_Action_INER_OnTerritoryCaptured(           "INER Action<CTFBot>::OnTerritoryCaptured");
static FuncThunk<void             (*)(Action<CTFBot> *, int)>                                                                ft_Action_INER_OnTerritoryLost(               "INER Action<CTFBot>::OnTerritoryLost");
static FuncThunk<void             (*)(Action<CTFBot> *)>                                                                     ft_Action_INER_OnWin(                         "INER Action<CTFBot>::OnWin");
static FuncThunk<void             (*)(Action<CTFBot> *)>                                                                     ft_Action_INER_OnLose(                        "INER Action<CTFBot>::OnLose");
static FuncThunk<bool             (*)(const Action<CTFBot> *, const char *)>                                                 ft_Action_IsNamed(                            "Action<CTFBot>::IsNamed");
static FuncThunk<char *           (*)(const Action<CTFBot> *)>                                                               ft_Action_GetFullName(                        "Action<CTFBot>::GetFullName");
static FuncThunk<AR               (*)(Action<CTFBot> *, CTFBot *actor, Action<CTFBot> *)>                                    ft_Action_OnStart(                            "Action<CTFBot>::OnStart");
static FuncThunk<AR               (*)(Action<CTFBot> *, CTFBot *actor, float)>                                               ft_Action_Update(                             "Action<CTFBot>::Update");
static FuncThunk<void             (*)(Action<CTFBot> *, CTFBot *actor, Action<CTFBot> *)>                                    ft_Action_OnEnd(                              "Action<CTFBot>::OnEnd");
static FuncThunk<AR               (*)(Action<CTFBot> *, CTFBot *actor, Action<CTFBot> *)>                                    ft_Action_OnSuspend(                          "Action<CTFBot>::OnSuspend");
static FuncThunk<AR               (*)(Action<CTFBot> *, CTFBot *actor, Action<CTFBot> *)>                                    ft_Action_OnResume(                           "Action<CTFBot>::OnResume");
static FuncThunk<Action<CTFBot> * (*)(Action<CTFBot> *, CTFBot *actor)>                                                      ft_Action_InitialContainedAction(             "Action<CTFBot>::InitialContainedAction");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, CBaseEntity *)>                                       ft_Action_OnLeaveGround(                      "Action<CTFBot>::OnLeaveGround");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, CBaseEntity *)>                                       ft_Action_OnLandOnGround(                     "Action<CTFBot>::OnLandOnGround");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, CBaseEntity *, CGameTrace *)>                         ft_Action_OnContact(                          "Action<CTFBot>::OnContact");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, const Path *)>                                        ft_Action_OnMoveToSuccess(                    "Action<CTFBot>::OnMoveToSuccess");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, const Path *, INER::MoveToFailureType)>               ft_Action_OnMoveToFailure(                    "Action<CTFBot>::OnMoveToFailure");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor)>                                                      ft_Action_OnStuck(                            "Action<CTFBot>::OnStuck");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor)>                                                      ft_Action_OnUnStuck(                          "Action<CTFBot>::OnUnStuck");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor)>                                                      ft_Action_OnPostureChanged(                   "Action<CTFBot>::OnPostureChanged");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, int)>                                                 ft_Action_OnAnimationActivityComplete(        "Action<CTFBot>::OnAnimationActivityComplete");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, int)>                                                 ft_Action_OnAnimationActivityInterrupted(     "Action<CTFBot>::OnAnimationActivityInterrupted");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, animevent_t *)>                                       ft_Action_OnAnimationEvent(                   "Action<CTFBot>::OnAnimationEvent");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor)>                                                      ft_Action_OnIgnite(                           "Action<CTFBot>::OnIgnite");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, const CTakeDamageInfo&)>                              ft_Action_OnInjured(                          "Action<CTFBot>::OnInjured");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, const CTakeDamageInfo&)>                              ft_Action_OnKilled(                           "Action<CTFBot>::OnKilled");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, CBaseCombatCharacter *, const CTakeDamageInfo&)>      ft_Action_OnOtherKilled(                      "Action<CTFBot>::OnOtherKilled");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, CBaseEntity *)>                                       ft_Action_OnSight(                            "Action<CTFBot>::OnSight");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, CBaseEntity *)>                                       ft_Action_OnLostSight(                        "Action<CTFBot>::OnLostSight");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, CBaseEntity *, const Vector&, KeyValues *)>           ft_Action_OnSound(                            "Action<CTFBot>::OnSound");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, CBaseCombatCharacter *, const char *, AI_Response *)> ft_Action_OnSpokeConcept(                     "Action<CTFBot>::OnSpokeConcept");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, CBaseCombatCharacter *, CBaseCombatWeapon *)>         ft_Action_OnWeaponFired(                      "Action<CTFBot>::OnWeaponFired");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, CNavArea *, CNavArea *)>                              ft_Action_OnNavAreaChanged(                   "Action<CTFBot>::OnNavAreaChanged");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor)>                                                      ft_Action_OnModelChanged(                     "Action<CTFBot>::OnModelChanged");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, CBaseEntity *, CBaseCombatCharacter *)>               ft_Action_OnPickUp(                           "Action<CTFBot>::OnPickUp");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, CBaseEntity *)>                                       ft_Action_OnDrop(                             "Action<CTFBot>::OnDrop");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, CBaseCombatCharacter *, int)>                         ft_Action_OnActorEmoted(                      "Action<CTFBot>::OnActorEmoted");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, CBaseEntity *)>                                       ft_Action_OnCommandAttack(                    "Action<CTFBot>::OnCommandAttack");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, const Vector&, float)>                                ft_Action_OnCommandApproach_vec(              "Action<CTFBot>::OnCommandApproach(vec)");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, CBaseEntity *)>                                       ft_Action_OnCommandApproach_ent(              "Action<CTFBot>::OnCommandApproach(ent)");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, CBaseEntity *, float)>                                ft_Action_OnCommandRetreat(                   "Action<CTFBot>::OnCommandRetreat");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, float)>                                               ft_Action_OnCommandPause(                     "Action<CTFBot>::OnCommandPause");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor)>                                                      ft_Action_OnCommandResume(                    "Action<CTFBot>::OnCommandResume");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, const char *)>                                        ft_Action_OnCommandString(                    "Action<CTFBot>::OnCommandString");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, CBaseEntity *)>                                       ft_Action_OnShoved(                           "Action<CTFBot>::OnShoved");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, CBaseEntity *)>                                       ft_Action_OnBlinded(                          "Action<CTFBot>::OnBlinded");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, int)>                                                 ft_Action_OnTerritoryContested(               "Action<CTFBot>::OnTerritoryContested");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, int)>                                                 ft_Action_OnTerritoryCaptured(                "Action<CTFBot>::OnTerritoryCaptured");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor, int)>                                                 ft_Action_OnTerritoryLost(                    "Action<CTFBot>::OnTerritoryLost");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor)>                                                      ft_Action_OnWin(                              "Action<CTFBot>::OnWin");
static FuncThunk<EDR              (*)(Action<CTFBot> *, CTFBot *actor)>                                                      ft_Action_OnLose(                             "Action<CTFBot>::OnLose");
static FuncThunk<bool             (*)(const Action<CTFBot> *, const INextBot *)>                                             ft_Action_IsAbleToBlockMovementOf(            "Action<CTFBot>::IsAbleToBlockMovementOf");


QueryResponse IContextualQuery::ShouldPickUp(const INextBot *nextbot, CBaseEntity *it) const                                                                                               { return (*ft_IContextualQuery_ShouldPickUp)             (this, nextbot, it); }
QueryResponse IContextualQuery::ShouldHurry(const INextBot *nextbot) const                                                                                                                 { return (*ft_IContextualQuery_ShouldHurry)              (this, nextbot); }
QueryResponse IContextualQuery::ShouldRetreat(const INextBot *nextbot) const                                                                                                               { return (*ft_IContextualQuery_ShouldRetreat)            (this, nextbot); }
QueryResponse IContextualQuery::ShouldAttack(const INextBot *nextbot, const CKnownEntity *threat) const                                                                                    { return (*ft_IContextualQuery_ShouldAttack)             (this, nextbot, threat); }
QueryResponse IContextualQuery::IsHindrance(const INextBot *nextbot, CBaseEntity *it) const                                                                                                { return (*ft_IContextualQuery_IsHindrance)              (this, nextbot, it); }
Vector IContextualQuery::SelectTargetPoint(const INextBot *nextbot, const CBaseCombatCharacter *them) const                                                                                { return (*ft_IContextualQuery_SelectTargetPoint)        (this, nextbot, them); }
QueryResponse IContextualQuery::IsPositionAllowed(const INextBot *nextbot, const Vector& v1) const                                                                                         { return (*ft_IContextualQuery_IsPositionAllowed)        (this, nextbot, v1); }
const CKnownEntity *IContextualQuery::SelectMoreDangerousThreat(const INextBot *nextbot, const CBaseCombatCharacter *them, const CKnownEntity *threat1, const CKnownEntity *threat2) const { return (*ft_IContextualQuery_SelectMoreDangerousThreat)(this, nextbot, them, threat1, threat2); }

template<> INextBotEventResponder *Action<CTFBot>::FirstContainedResponder() const                                                                    { return (*ft_Action_INER_FirstContainedResponder)       (this); }
template<> INextBotEventResponder *Action<CTFBot>::NextContainedResponder(INextBotEventResponder *prev) const                                         { return (*ft_Action_INER_NextContainedResponder)        (this, prev); }
template<> void Action<CTFBot>::OnLeaveGround(CBaseEntity *ent)                                                                                       {        (*ft_Action_INER_OnLeaveGround)                 (this, ent); }
template<> void Action<CTFBot>::OnLandOnGround(CBaseEntity *ent)                                                                                      {        (*ft_Action_INER_OnLandOnGround)                (this, ent); }
template<> void Action<CTFBot>::OnContact(CBaseEntity *ent, CGameTrace *trace)                                                                        {        (*ft_Action_INER_OnContact)                     (this, ent, trace); }
template<> void Action<CTFBot>::OnMoveToSuccess(const Path *path)                                                                                     {        (*ft_Action_INER_OnMoveToSuccess)               (this, path); }
template<> void Action<CTFBot>::OnMoveToFailure(const Path *path, MoveToFailureType fail)                                                             {        (*ft_Action_INER_OnMoveToFailure)               (this, path, fail); }
template<> void Action<CTFBot>::OnStuck()                                                                                                             {        (*ft_Action_INER_OnStuck)                       (this); }
template<> void Action<CTFBot>::OnUnStuck()                                                                                                           {        (*ft_Action_INER_OnUnStuck)                     (this); }
template<> void Action<CTFBot>::OnPostureChanged()                                                                                                    {        (*ft_Action_INER_OnPostureChanged)              (this); }
template<> void Action<CTFBot>::OnAnimationActivityComplete(int i1)                                                                                   {        (*ft_Action_INER_OnAnimationActivityComplete)   (this, i1); }
template<> void Action<CTFBot>::OnAnimationActivityInterrupted(int i1)                                                                                {        (*ft_Action_INER_OnAnimationActivityInterrupted)(this, i1); }
template<> void Action<CTFBot>::OnAnimationEvent(animevent_t *a1)                                                                                     {        (*ft_Action_INER_OnAnimationEvent)              (this, a1); }
template<> void Action<CTFBot>::OnIgnite()                                                                                                            {        (*ft_Action_INER_OnIgnite)                      (this); }
template<> void Action<CTFBot>::OnInjured(const CTakeDamageInfo& info)                                                                                {        (*ft_Action_INER_OnInjured)                     (this, info); }
template<> void Action<CTFBot>::OnKilled(const CTakeDamageInfo& info)                                                                                 {        (*ft_Action_INER_OnKilled)                      (this, info); }
template<> void Action<CTFBot>::OnOtherKilled(CBaseCombatCharacter *who, const CTakeDamageInfo& info)                                                 {        (*ft_Action_INER_OnOtherKilled)                 (this, who, info); }
template<> void Action<CTFBot>::OnSight(CBaseEntity *ent)                                                                                             {        (*ft_Action_INER_OnSight)                       (this, ent); }
template<> void Action<CTFBot>::OnLostSight(CBaseEntity *ent)                                                                                         {        (*ft_Action_INER_OnLostSight)                   (this, ent); }
template<> void Action<CTFBot>::OnSound(CBaseEntity *ent, const Vector& v1, KeyValues *kv)                                                            {        (*ft_Action_INER_OnSound)                       (this, ent, v1, kv); }
template<> void Action<CTFBot>::OnSpokeConcept(CBaseCombatCharacter *who, const char *s1, AI_Response *response)                                      {        (*ft_Action_INER_OnSpokeConcept)                (this, who, s1, response); }
template<> void Action<CTFBot>::OnWeaponFired(CBaseCombatCharacter *who, CBaseCombatWeapon *weapon)                                                   {        (*ft_Action_INER_OnWeaponFired)                 (this, who, weapon); }
template<> void Action<CTFBot>::OnNavAreaChanged(CNavArea *area1, CNavArea *area2)                                                                    {        (*ft_Action_INER_OnNavAreaChanged)              (this, area1, area2); }
template<> void Action<CTFBot>::OnModelChanged()                                                                                                      {        (*ft_Action_INER_OnModelChanged)                (this); }
template<> void Action<CTFBot>::OnPickUp(CBaseEntity *ent, CBaseCombatCharacter *who)                                                                 {        (*ft_Action_INER_OnPickUp)                      (this, ent, who); }
template<> void Action<CTFBot>::OnDrop(CBaseEntity *ent)                                                                                              {        (*ft_Action_INER_OnDrop)                        (this, ent); }
template<> void Action<CTFBot>::OnActorEmoted(CBaseCombatCharacter *who, int concept)                                                                 {        (*ft_Action_INER_OnActorEmoted)                 (this, who, concept); }
template<> void Action<CTFBot>::OnCommandAttack(CBaseEntity *ent)                                                                                     {        (*ft_Action_INER_OnCommandAttack)               (this, ent); }
template<> void Action<CTFBot>::OnCommandApproach(const Vector& v1, float f1)                                                                         {        (*ft_Action_INER_OnCommandApproach_vec)         (this, v1, f1); }
template<> void Action<CTFBot>::OnCommandApproach(CBaseEntity *ent)                                                                                   {        (*ft_Action_INER_OnCommandApproach_ent)         (this, ent); }
template<> void Action<CTFBot>::OnCommandRetreat(CBaseEntity *ent, float f1)                                                                          {        (*ft_Action_INER_OnCommandRetreat)              (this, ent, f1); }
template<> void Action<CTFBot>::OnCommandPause(float f1)                                                                                              {        (*ft_Action_INER_OnCommandPause)                (this, f1); }
template<> void Action<CTFBot>::OnCommandResume()                                                                                                     {        (*ft_Action_INER_OnCommandResume)               (this); }
template<> void Action<CTFBot>::OnCommandString(const char *cmd)                                                                                      {        (*ft_Action_INER_OnCommandString)               (this, cmd); }
template<> void Action<CTFBot>::OnShoved(CBaseEntity *ent)                                                                                            {        (*ft_Action_INER_OnShoved)                      (this, ent); }
template<> void Action<CTFBot>::OnBlinded(CBaseEntity *ent)                                                                                           {        (*ft_Action_INER_OnBlinded)                     (this, ent); }
template<> void Action<CTFBot>::OnTerritoryContested(int i1)                                                                                          {        (*ft_Action_INER_OnTerritoryContested)          (this, i1); }
template<> void Action<CTFBot>::OnTerritoryCaptured(int i1)                                                                                           {        (*ft_Action_INER_OnTerritoryCaptured)           (this, i1); }
template<> void Action<CTFBot>::OnTerritoryLost(int i1)                                                                                               {        (*ft_Action_INER_OnTerritoryLost)               (this, i1); }
template<> void Action<CTFBot>::OnWin()                                                                                                               {        (*ft_Action_INER_OnWin)                         (this); }
template<> void Action<CTFBot>::OnLose()                                                                                                              {        (*ft_Action_INER_OnLose)                        (this); }
template<> bool Action<CTFBot>::IsNamed(const char *name) const                                                                                       { return (*ft_Action_IsNamed)                            (this, name); }
template<> char *Action<CTFBot>::GetFullName() const                                                                                                  { return (*ft_Action_GetFullName)                        (this); }
template<> ActionResult<CTFBot> Action<CTFBot>::OnStart(CTFBot *actor, Action<CTFBot> *action)                                                        { return (*ft_Action_OnStart)                            (this, actor, action); }
template<> ActionResult<CTFBot> Action<CTFBot>::Update(CTFBot *actor, float dt)                                                                       { return (*ft_Action_Update)                             (this, actor, dt); }
template<> void Action<CTFBot>::OnEnd(CTFBot *actor, Action<CTFBot> *action)                                                                          {        (*ft_Action_OnEnd)                              (this, actor, action); }
template<> ActionResult<CTFBot> Action<CTFBot>::OnSuspend(CTFBot *actor, Action<CTFBot> *action)                                                      { return (*ft_Action_OnSuspend)                          (this, actor, action); }
template<> ActionResult<CTFBot> Action<CTFBot>::OnResume(CTFBot *actor, Action<CTFBot> *action)                                                       { return (*ft_Action_OnResume)                           (this, actor, action); }
template<> Action<CTFBot> *Action<CTFBot>::InitialContainedAction(CTFBot *actor)                                                                      { return (*ft_Action_InitialContainedAction)             (this, actor); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnLeaveGround(CTFBot *actor, CBaseEntity *ent)                                                  { return (*ft_Action_OnLeaveGround)                      (this, actor, ent); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnLandOnGround(CTFBot *actor, CBaseEntity *ent)                                                 { return (*ft_Action_OnLandOnGround)                     (this, actor, ent); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnContact(CTFBot *actor, CBaseEntity *ent, CGameTrace *trace)                                   { return (*ft_Action_OnContact)                          (this, actor, ent, trace); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnMoveToSuccess(CTFBot *actor, const Path *path)                                                { return (*ft_Action_OnMoveToSuccess)                    (this, actor, path); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnMoveToFailure(CTFBot *actor, const Path *path, MoveToFailureType fail)                        { return (*ft_Action_OnMoveToFailure)                    (this, actor, path, fail); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnStuck(CTFBot *actor)                                                                          { return (*ft_Action_OnStuck)                            (this, actor); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnUnStuck(CTFBot *actor)                                                                        { return (*ft_Action_OnUnStuck)                          (this, actor); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnPostureChanged(CTFBot *actor)                                                                 { return (*ft_Action_OnPostureChanged)                   (this, actor); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnAnimationActivityComplete(CTFBot *actor, int i1)                                              { return (*ft_Action_OnAnimationActivityComplete)        (this, actor, i1); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnAnimationActivityInterrupted(CTFBot *actor, int i1)                                           { return (*ft_Action_OnAnimationActivityInterrupted)     (this, actor, i1); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnAnimationEvent(CTFBot *actor, animevent_t *a1)                                                { return (*ft_Action_OnAnimationEvent)                   (this, actor, a1); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnIgnite(CTFBot *actor)                                                                         { return (*ft_Action_OnIgnite)                           (this, actor); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnInjured(CTFBot *actor, const CTakeDamageInfo& info)                                           { return (*ft_Action_OnInjured)                          (this, actor, info); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnKilled(CTFBot *actor, const CTakeDamageInfo& info)                                            { return (*ft_Action_OnKilled)                           (this, actor, info); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnOtherKilled(CTFBot *actor, CBaseCombatCharacter *who, const CTakeDamageInfo& info)            { return (*ft_Action_OnOtherKilled)                      (this, actor, who, info); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnSight(CTFBot *actor, CBaseEntity *ent)                                                        { return (*ft_Action_OnSight)                            (this, actor, ent); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnLostSight(CTFBot *actor, CBaseEntity *ent)                                                    { return (*ft_Action_OnLostSight)                        (this, actor, ent); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnSound(CTFBot *actor, CBaseEntity *ent, const Vector& v1, KeyValues *kv)                       { return (*ft_Action_OnSound)                            (this, actor, ent, v1, kv); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnSpokeConcept(CTFBot *actor, CBaseCombatCharacter *who, const char *s1, AI_Response *response) { return (*ft_Action_OnSpokeConcept)                     (this, actor, who, s1, response); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnWeaponFired(CTFBot *actor, CBaseCombatCharacter *who, CBaseCombatWeapon *weapon)              { return (*ft_Action_OnWeaponFired)                      (this, actor, who, weapon); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnNavAreaChanged(CTFBot *actor, CNavArea *area1, CNavArea *area2)                               { return (*ft_Action_OnNavAreaChanged)                   (this, actor, area1, area2); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnModelChanged(CTFBot *actor)                                                                   { return (*ft_Action_OnModelChanged)                     (this, actor); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnPickUp(CTFBot *actor, CBaseEntity *ent, CBaseCombatCharacter *who)                            { return (*ft_Action_OnPickUp)                           (this, actor, ent, who); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnDrop(CTFBot *actor, CBaseEntity *ent)                                                         { return (*ft_Action_OnDrop)                             (this, actor, ent); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnActorEmoted(CTFBot *actor, CBaseCombatCharacter *who, int i1)                                 { return (*ft_Action_OnActorEmoted)                      (this, actor, who, i1); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnCommandAttack(CTFBot *actor, CBaseEntity *ent)                                                { return (*ft_Action_OnCommandAttack)                    (this, actor, ent); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnCommandApproach(CTFBot *actor, const Vector& v1, float f1)                                    { return (*ft_Action_OnCommandApproach_vec)              (this, actor, v1, f1); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnCommandApproach(CTFBot *actor, CBaseEntity *ent)                                              { return (*ft_Action_OnCommandApproach_ent)              (this, actor, ent); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnCommandRetreat(CTFBot *actor, CBaseEntity *ent, float f1)                                     { return (*ft_Action_OnCommandRetreat)                   (this, actor, ent, f1); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnCommandPause(CTFBot *actor, float f1)                                                         { return (*ft_Action_OnCommandPause)                     (this, actor, f1); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnCommandResume(CTFBot *actor)                                                                  { return (*ft_Action_OnCommandResume)                    (this, actor); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnCommandString(CTFBot *actor, const char *cmd)                                                 { return (*ft_Action_OnCommandString)                    (this, actor, cmd); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnShoved(CTFBot *actor, CBaseEntity *ent)                                                       { return (*ft_Action_OnShoved)                           (this, actor, ent); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnBlinded(CTFBot *actor, CBaseEntity *ent)                                                      { return (*ft_Action_OnBlinded)                          (this, actor, ent); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnTerritoryContested(CTFBot *actor, int i1)                                                     { return (*ft_Action_OnTerritoryContested)               (this, actor, i1); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnTerritoryCaptured(CTFBot *actor, int i1)                                                      { return (*ft_Action_OnTerritoryCaptured)                (this, actor, i1); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnTerritoryLost(CTFBot *actor, int i1)                                                          { return (*ft_Action_OnTerritoryLost)                    (this, actor, i1); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnWin(CTFBot *actor)                                                                            { return (*ft_Action_OnWin)                              (this, actor); }
template<> EventDesiredResult<CTFBot> Action<CTFBot>::OnLose(CTFBot *actor)                                                                           { return (*ft_Action_OnLose)                             (this, actor); }
template<> bool Action<CTFBot>::IsAbleToBlockMovementOf(const INextBot *nextbot) const                                                                { return (*ft_Action_IsAbleToBlockMovementOf)            (this, nextbot); }


extern "C"
{
	/* dtor */
	void _ZN22INextBotEventResponderD0Ev(INextBotEventResponder *_this) { return (*ft_INextBotEventResponder_dtor_D0)(_this); }
	void _ZN22INextBotEventResponderD2Ev(INextBotEventResponder *_this) { return (*ft_INextBotEventResponder_dtor_D2)(_this); }
	void _ZN16IContextualQueryD0Ev      (IContextualQuery *_this)       { return (*ft_IContextualQuery_dtor_D0)      (_this); }
	void _ZN16IContextualQueryD2Ev      (IContextualQuery *_this)       { return (*ft_IContextualQuery_dtor_D2)      (_this); }
	void _ZN6ActionI6CTFBotED2Ev        (Action<CTFBot> *_this)         { return (*ft_Action_dtor_D2)                (_this); }
}
