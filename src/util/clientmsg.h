#ifndef _INCLUDE_SIGSEGV_UTIL_CLIENTMSG_H_
#define _INCLUDE_SIGSEGV_UTIL_CLIENTMSG_H_


template<typename... ARGS>
//[[gnu::format(printf, 1, 2)]]
void ClientMsg(const char *fmt, ARGS... args)
{
	int n_clients = sv->GetNumClients();
	for (int i = 0; i < n_clients; ++i) {
		IClient *client = sv->GetClient(i);
		if (client == nullptr) continue;
		
		client->ClientPrintf(fmt, std::forward<ARGS>(args)...);
	}
}


#endif
