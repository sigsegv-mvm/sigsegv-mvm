#ifndef _INCLUDE_SIGSEGV_UTIL_FIREHOSE_SEND_H_
#define _INCLUDE_SIGSEGV_UTIL_FIREHOSE_SEND_H_


#include "util/firehose_base.h"


class FirehoseSend : public FirehoseBase
{
public:
	FirehoseSend(const char *host, uint16_t port) :
		m_strHost(host), m_iPort(port)
	{
		this->GetAddrInfo();
		
		this->OpenSocket();
		this->CreateThread();
	}
	virtual ~FirehoseSend()
	{
		this->CloseSocket();
		this->JoinThread();
		
		this->FreeAddrInfo();
	}
	
	void Send(size_t len, const uint8_t *src);
	
private:
	std::string m_strHost;
	uint16_t m_iPort;
	
	virtual const char *GetHost() const override { return this->m_strHost.c_str(); }
	virtual uint16_t GetPort() const override    { return this->m_iPort; }
	
	virtual const addrinfo *GetAddrHints() const override;
	
	void CreateThread();
	void JoinThread();
	
	std::thread *m_Thread = nullptr;
	void ThreadLoop();
	
	std::mutex m_NotifyMutex;
	std::condition_variable m_Notify;
	bool m_bDone = false;
	
	bool Connect();
	
	void InternalSend(size_t len, const uint8_t *src);
	
	std::mutex m_DataMutex;
	std::deque<uint8_t> m_Data;
	
	std::vector<uint8_t> m_SendBuf;
};


#endif
