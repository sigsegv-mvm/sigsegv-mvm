#include "stub/usermessages_sv.h"

MemberFuncThunk<CUserMessages_Server *, int, const char *>       CUserMessages_Server::ft_LookupUserMessage("CUserMessages::LookupUserMessage");
MemberFuncThunk<CUserMessages_Server *, void, const char *, int> CUserMessages_Server::ft_Register         ("CUserMessages::Register");

GlobalThunk<CUserMessages_Server *> usermessages_sv("usermessages");


#include "stub/usermessages_cl.h"

MemberFuncThunk<CUserMessages_Client *, void, const char *, int>            CUserMessages_Client::ft_Register         ("[client] CUserMessages::Register");
MemberFuncThunk<CUserMessages_Client *, void, const char *, pfnUserMsgHook> CUserMessages_Client::ft_HookMessage      ("[client] CUserMessages::HookMessage");

GlobalThunk<CUserMessages_Client *> usermessages_cl("[client] usermessages");
