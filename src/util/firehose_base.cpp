#include "util/firehose_base.h"


void FirehoseBase::GetAddrInfo()
{
	char port[16];
	snprintf(port, sizeof(port), "%" PRIu16, this->GetPort());
	
	int error = ::getaddrinfo(this->GetHost(), port, this->GetAddrHints(), &this->m_Addrs);
	if (error != 0) {
		Warning("FirehoseBase::GetAddrInfo: getaddrinfo() failed: %s\n", gai_strerror(error));
	}
}

void FirehoseBase::FreeAddrInfo()
{
	if (this->m_Addrs != nullptr) {
		::freeaddrinfo(this->m_Addrs);
		this->m_Addrs = nullptr;
	}
}


void FirehoseBase::OpenSocket()
{
	this->m_Socket = socket(this->GetAddrHints()->ai_family, this->GetAddrHints()->ai_socktype, this->GetAddrHints()->ai_protocol);
	if (this->m_Socket == INVALID_SOCKET) {
		Warning("FirehoseBase::OpenSocket: socket() failed: %s\n", this->GetErrorMessage());
		return;
	}
	
	linger linger = {
		1, // .l_onoff
		0, // .l_linger
	};
	if (setsockopt(this->m_Socket, SOL_SOCKET, SO_LINGER, reinterpret_cast<char *>(&linger), sizeof(linger)) != 0) {
		Warning("FirehoseBase::OpenSocket: setsockopt(SO_LINGER) failed: %s\n", this->GetErrorMessage());
	}
	
#if defined _WINDOWS
	DWORD timeout = 100;
#else
	timeval timeout = {
		0,      // .tv_sec
		100000, // .tv_usec
	};
#endif
	if (setsockopt(this->m_Socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char *>(&timeout), sizeof(timeout)) != 0) {
		Warning("FirehoseBase::OpenSocket: setsockopt(SO_RCVTIMEO) failed: %s\n", this->GetErrorMessage());
	}
	
#if !defined _WINDOWS
	int tos = IPTOS_LOWDELAY;
	if (setsockopt(this->m_Socket, IPPROTO_IP, IP_TOS, reinterpret_cast<char *>(&tos), sizeof(tos)) != 0) {
		Warning("FirehoseBase::OpenSocket: setsockopt(IP_TOS) failed: %s\n", this->GetErrorMessage());
	}
#endif
	
#if defined _WINDOWS
	DWORD no_delay = 1;
#else
	int no_delay = 1;
#endif
	if (setsockopt(this->m_Socket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char *>(&no_delay), sizeof(no_delay)) != 0) {
		Warning("FirehoseBase::OpenSocket: setsockopt(TCP_NODELAY) failed: %s\n", this->GetErrorMessage());
	}
	
	// maybe: IP_MTU_DISCOVER = 0 (linux only)
}

void FirehoseBase::CloseSocket()
{
	if (this->m_Socket != INVALID_SOCKET) {
		// TODO: WinSock error checks
		// - WSAENOTCONN
#if defined _WINDOWS
		if (shutdown(this->m_Socket, SD_BOTH) != 0) {
#else
		if (shutdown(this->m_Socket, SHUT_RDWR) != 0) {
#endif
			Warning("FirehoseBase::CloseSocket: shutdown() failed: %s\n", this->GetErrorMessage());
		}
		
#if defined _WINDOWS
		closesocket(this->m_Socket);
#else
		close(this->m_Socket);
#endif
		this->m_Socket = INVALID_SOCKET;
	}
}


const char *FirehoseBase::GetErrorMessage() const
{
#if defined _WINDOWS
	static char msg[1024];
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, WSAGetLastError(), 0, msg, sizeof(msg), nullptr);
	return msg;
#else
	return strerror(errno);
#endif
}
