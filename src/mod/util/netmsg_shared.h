#ifndef _INCLUDE_SIGSEGV_MOD_UTIL_NETMSG_SHARED_H_
#define _INCLUDE_SIGSEGV_MOD_UTIL_NETMSG_SHARED_H_


// 20161013a: TF2's SVC_LASTMSG is 33
constexpr int svc_SigOverlay = 34;


class SVC_SigOverlay : public INetMessage
{
public:
	SVC_SigOverlay() {}
	virtual ~SVC_SigOverlay() {}
	
	virtual void SetNetChannel(INetChannel *netchan) override { this->m_pNetChannel = netchan; }
	virtual void SetReliable(bool state) override             { this->m_bReliable   = state; }
	
	virtual bool Process() override;
	
	virtual bool ReadFromBuffer(bf_read& buffer) override;
	virtual bool WriteToBuffer(bf_read& buffer) override;
	
	virtual bool IsReliable() const override            { return this->m_bReliable; }
	virtual int GetType() const override                { return svc_SigOverlay; }
	virtual int GetGroup() const override               { return INetChannelInfo::GENERIC; }
	virtual const char *GetName() const override        { return "svc_SigOverlay"; }
	virtual INetChannel *GetNetChannel() const override { return this->m_pNetChannel; }
	
	virtual const char *ToString() const override;
	
private:
	bool m_bReliable = true;
	INetChannel *m_pNetChannel = nullptr;
};


#error TODO: register message on client side
// - in CBaseClientState::ConnectionStart (or later):
//   - construct a new SVC_whatever (on the heap, leak it)
//   - and call chan->RegisterMessage(msg)
//   - if not in ConnectionStart, chan will presumably this->m_NetChannel (?)

#error TODO: send message on server side
#error TODO: implement the remaining funcs


#endif
