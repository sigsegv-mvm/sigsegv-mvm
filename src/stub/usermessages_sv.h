#ifndef _INCLUDE_SIGSEGV_STUB_USERMESSAGES_SV_H_
#define _INCLUDE_SIGSEGV_STUB_USERMESSAGES_SV_H_


#include "link/link.h"
#include "stub/baseplayer.h"


class CReliableBroadcastRecipientFilter : public IRecipientFilter
{
public:
	CReliableBroadcastRecipientFilter()
	{
		this->m_Recipients.clear();
		
		for (int i = 1; i <= gpGlobals->maxClients; ++i) {
			CBasePlayer *pPlayer = UTIL_PlayerByIndex(i);
			if (pPlayer == nullptr) continue;
			
			this->m_Recipients.push_back(ENTINDEX(pPlayer));
		}
	}
	virtual ~CReliableBroadcastRecipientFilter() {}
	
	virtual bool IsReliable() const override { return true; }
	virtual bool IsInitMessage() const override { return false; }
	
	virtual int GetRecipientCount() const override
	{
		return this->m_Recipients.size();
	}
	virtual int GetRecipientIndex(int slot) const override
	{
		if (slot < 0 || slot >= (int)this->m_Recipients.size()) {
			return -1;
		}
		
		return this->m_Recipients[slot];
	}
	
private:
	std::vector<int> m_Recipients;
};


class CUserMessages_Server
{
public:
	int LookupUserMessage(const char *name)   { return ft_LookupUserMessage(this, name); }
	void Register(const char *name, int size) {        ft_Register         (this, name, size); }
	
private:
	static MemberFuncThunk<CUserMessages_Server *, int, const char *>       ft_LookupUserMessage;
	static MemberFuncThunk<CUserMessages_Server *, void, const char *, int> ft_Register;
};


extern GlobalThunk<CUserMessages_Server *> usermessages_sv;


#define CUserMessages CUserMessages_Server
#define usermessages  usermessages_sv


#endif
