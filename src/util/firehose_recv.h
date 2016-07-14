#ifndef _INCLUDE_SIGSEGV_UTIL_FIREHOSE_RECV_H_
#define _INCLUDE_SIGSEGV_UTIL_FIREHOSE_RECV_H_


#include "util/firehose_base.h"


class FirehoseRecv : public FirehoseBase
{
public:
	FirehoseRecv(uint16_t port) :
		m_iPort(port)
	{
		this->GetAddrInfo();
		
		this->OpenSocket();
		
		if (this->Bind() && this->Listen()) {
			this->CreateThread();
		}
	}
	virtual ~FirehoseRecv()
	{
		this->CloseSocket();
		this->JoinThread();
		
		this->FreeAddrInfo();
	}
	
	size_t Recv(size_t len, uint8_t *dst, bool peek = false);
	size_t Available();
	void Skip(size_t len);
	
private:
	uint16_t m_iPort;
	
	virtual const char *GetHost() const override { return nullptr; }
	virtual uint16_t GetPort() const override    { return this->m_iPort; }
	
	virtual const addrinfo *GetAddrHints() const override;
	
	bool Bind();
	bool Listen();
	
	void CreateThread();
	void JoinThread();
	
	std::thread *m_Thread = nullptr;
	void ThreadLoop();
	
	std::atomic_flag m_DoneFlag;
	
	SOCKET m_Client = INVALID_SOCKET;
	
	void InternalAccept();
	void InternalRecv();
	
	std::mutex m_DataMutex;
	std::deque<uint8_t> m_Data;
	
	std::vector<uint8_t> m_RecvBuf;
};


#endif
