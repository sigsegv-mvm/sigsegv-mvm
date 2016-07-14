#ifndef _INCLUDE_SIGSEGV_UTIL_FIREHOSE_BASE_H_
#define _INCLUDE_SIGSEGV_UTIL_FIREHOSE_BASE_H_


#if !defined _WINDOWS
typedef int SOCKET;
constexpr SOCKET INVALID_SOCKET = -1;
constexpr ssize_t SOCKET_ERROR = -1;
#endif


class FirehoseBase
{
public:
	FirehoseBase() {}
//	{
//		this->GetAddrInfo();
//		this->OpenSocket();
//	}
	virtual ~FirehoseBase() {}
//	{
//		this->CloseSocket();
//		this->FreeAddrInfo();
//	}
	
protected:
	virtual const char *GetHost() const = 0;
	virtual uint16_t GetPort() const = 0;
	
	virtual const addrinfo *GetAddrHints() const = 0;
	
	const char *GetErrorMessage() const;
	
	addrinfo *GetAddrs() const { return this->m_Addrs; }
	
	void OpenSocket();
	void CloseSocket();
	
	void GetAddrInfo();
	void FreeAddrInfo();
	
	SOCKET m_Socket = INVALID_SOCKET;
	
private:
	addrinfo *m_Addrs = nullptr;
};


#endif
