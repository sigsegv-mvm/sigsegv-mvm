#ifndef _INCLUDE_SIGSEGV_UTIL_SOCKET_OLD_H_
#define _INCLUDE_SIGSEGV_UTIL_SOCKET_OLD_H_


#if 0
class FirehoseSend
{
public:
	FirehoseSend()
	{
		this->OpenSocket();
	}
	~FirehoseSend()
	{
		this->CloseSocket();
	}
	
private:
	
};
#endif

class FirehoseRecv_Old
{
public:
	FirehoseRecv_Old(uint16_t port) :
		m_nPort(port)
	{
		this->Open();
	}
	~FirehoseRecv_Old()
	{
		this->Close();
	}
	
	size_t Recv(size_t len, uint8_t *dst);
	
private:
	void Open();
	void Close();
	
	uint16_t m_nPort;
	
	int m_Socket = -1;
	
	bool m_bOpened = false;
	bool m_bReady  = false;
};


void Firehose_Send(uint16_t port, size_t len, const uint8_t *src);


#define FirehoseRecv FirehoseRecv_Old


#endif
