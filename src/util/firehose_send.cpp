#include "util/firehose_send.h"


const addrinfo *FirehoseSend::GetAddrHints() const
{
	static const addrinfo hints = {
		0,           // .ai_flags
		AF_INET6,    // .ai_family
		SOCK_STREAM, // .ai_socktype
		IPPROTO_TCP, // .ai_protocol
	};
	return &hints;
}


void FirehoseSend::Send(size_t len, const uint8_t *src)
{
	if (this->m_Socket == INVALID_SOCKET || len == 0 || src == nullptr) {
		return;
	}
	
	{
		std::lock_guard<std::mutex> guard(this->m_DataMutex);
		this->m_Data.insert(this->m_Data.end(), src, src + len);
	}
	this->m_Notify.notify_all();
}


bool FirehoseSend::Connect()
{
	for (addrinfo *addr = this->GetAddrs(); addr != nullptr; addr = addr->ai_next) {
		if (connect(this->m_Socket, addr->ai_addr, addr->ai_addrlen) == 0) {
			DevMsg("FirehoseSend::Connect: connected\n");
			return true;
		}
		
#if defined _WINDOWS
		if (WSAGetLastError() == WSAEISCONN) {
#else
		if (errno == EISCONN) {
#endif
			DevMsg("FirehoseSend::Connect: already connected\n");
			return true;
		} else {
			Warning("FirehoseSend::Connect: connect() failed: %s\n", this->GetErrorMessage());
		}
	}
	
	return false;
}


void FirehoseSend::CreateThread()
{
	this->m_Thread = new std::thread(&FirehoseSend::ThreadLoop, this);
	
//	DevMsg("[Main] created thread\n");
}

void FirehoseSend::JoinThread()
{
	if (this->m_Thread != nullptr) {
//		DevMsg("[Main] acquiring m_NotifyMutex\n");
		{
			std::lock_guard<std::mutex> guard(this->m_NotifyMutex);
//			DevMsg("[Main] acquired m_NotifyMutex\n");
			this->m_bDone = true;
		}
//		DevMsg("[Main] notifying m_Notify\n");
		this->m_Notify.notify_all();
		
//		DevMsg("[Main] joining thread\n");
		this->m_Thread->join();
//		DevMsg("[Main] thread joined\n");
		
		delete this->m_Thread;
		this->m_Thread = nullptr;
	}
}


void FirehoseSend::ThreadLoop()
{
	if (this->m_Socket == INVALID_SOCKET) {
		return;
	}
	
//	DevMsg("[Thread] start\n");
	
	while (true) {
//		DevMsg("[Thread] acquiring m_NotifyMutex\n");
		std::unique_lock<std::mutex> lock(this->m_NotifyMutex);
//		DevMsg("[Thread] waiting for m_Notify\n");
		this->m_Notify.wait(lock);
//		DevMsg("[Thread] notified\n");
		
		bool done = this->m_bDone;
		lock.unlock();
		
		if (done) {
//			DevMsg("[Thread] got done signal\n");
			break;
		}
		
//		DevMsg("[Thread] starting work\n");
		while (true) {
			{
//				DevMsg("[Thread] acquiring m_DataMutex\n");
				std::lock_guard<std::mutex> guard(this->m_DataMutex);
//				DevMsg("[Thread] acquired m_DataMatrix\n");
				if (this->m_Data.empty()) {
//					DevMsg("[Thread] no work to do\n");
					break;
				}
				
				this->m_SendBuf.resize(this->m_Data.size());
				std::copy(this->m_Data.begin(), this->m_Data.end(), this->m_SendBuf.begin());
				this->m_Data.clear();
			}
			
//			DevMsg("[Thread] sending buffer with %u bytes\n", len);
			this->InternalSend(this->m_SendBuf.size(), this->m_SendBuf.data());
//			DevMsg("[Thread] buffer send complete\n");
			
			this->m_SendBuf.clear();
		}
//		DevMsg("[Thread] done with work\n");
	}
	
//	DevMsg("[Thread] end\n");
}


void FirehoseSend::InternalSend(size_t len, const uint8_t *src)
{
	if (!this->Connect()) {
		return;
	}
	
#if defined _WINDOWS
	int result = send(this->m_Socket, reinterpret_cast<const char *>(src), len, 0);
#else
	ssize_t result = send(this->m_Socket, src, len, MSG_NOSIGNAL);
#endif
	if (result == SOCKET_ERROR) {
		Warning("FirehoseSend::InternalSend: send() failed: %s\n", this->GetErrorMessage());
		return;
	}
	
	size_t bytes = result;
	if (bytes != len) {
		Warning("FirehoseSend::InternalSend: only sent %u/%u bytes\n", bytes, len);
	} else {
		DevMsg("FirehoseSend::InternalSend: success\n");
	}
}


FirehoseSend *sender = nullptr;

CON_COMMAND(firehose_send_test_begin, "")
{
	if (sender != nullptr) {
		return;
	}
	
	sender = new FirehoseSend("jgpc", 30001);
}

CON_COMMAND(firehose_send_test_end, "")
{
	delete sender;
	sender = nullptr;
}


CON_COMMAND(firehose_send_test_send, "")
{
	if (args.ArgC() < 2) {
		Warning("Need payload size argument\n");
		return;
	}
	
	if (sender == nullptr) {
		return;
	}
	
	int len = std::stoi(args[1], nullptr, 0);
	
	uint8_t *buf = new uint8_t[len];
	for (int i = 0; i < len; ++i) {
		buf[i] = (uint8_t)i;
	}
	
	sender->Send(len, buf);
	
	delete[] buf;
}
