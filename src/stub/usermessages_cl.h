#ifndef _INCLUDE_SIGSEGV_STUB_USERMESSAGES_CL_H_
#define _INCLUDE_SIGSEGV_STUB_USERMESSAGES_CL_H_


#include "link/link.h"


using pfnUserMsgHook = void (*)(bf_read& msg);


class CUserMessage
{
public:
	int size;
	const char *name;
	CUtlVector<pfnUserMsgHook> clienthooks;
};

class CUserMessages_Client
{
public:
	int LookupUserMessage(const char *name);
	void UnHookMessage(const char *name, pfnUserMsgHook hook);
	
	void Register(const char *name, int size)               {        ft_Register         (this, name, size); }
	void HookMessage(const char *name, pfnUserMsgHook hook) {        ft_HookMessage      (this, name, hook); }
	
private:
	CUtlDict<CUserMessage *, int> m_UserMessages;
	
	static MemberFuncThunk<CUserMessages_Client *, void, const char *, int>            ft_Register;
	static MemberFuncThunk<CUserMessages_Client *, void, const char *, pfnUserMsgHook> ft_HookMessage;
};

inline int CUserMessages_Client::LookupUserMessage(const char *name)
{
	int idx = m_UserMessages.Find(name);
	if (idx == m_UserMessages.InvalidIndex()) {
		return -1;
	}
	return idx;
}

inline void CUserMessages_Client::UnHookMessage(const char *name, pfnUserMsgHook hook)
{
	int idx = m_UserMessages.Find(name);
	if (idx != m_UserMessages.InvalidIndex()) {
		CUserMessage *msg = m_UserMessages[idx];
		
		while ((idx = msg->clienthooks.Find(hook)) != msg->clienthooks.InvalidIndex()) {
			msg->clienthooks.FastRemove(idx);
		}
	}
}


extern GlobalThunk<CUserMessages_Client *> usermessages_cl;


#undef CUserMessages
#undef usermessages
#define CUserMessages CUserMessages_Client
#define usermessages  usermessages_cl


#endif
