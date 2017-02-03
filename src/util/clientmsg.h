#ifndef _INCLUDE_SIGSEGV_UTIL_CLIENTMSG_H_
#define _INCLUDE_SIGSEGV_UTIL_CLIENTMSG_H_


#include "stub/baseplayer.h"


template<typename... ARGS>
//[[gnu::format(printf, 2, 3)]]
void ClientMsg(CBasePlayer *player, const char *fmt, ARGS... args)
{
	int n_clients = sv->GetNumClients();
	for (int i = 0; i < n_clients; ++i) {
		IClient *client = sv->GetClient(i);
		if (client == nullptr) continue;
		
		if (client->GetUserID() == player->GetUserID()) {
			client->ClientPrintf(fmt, std::forward<ARGS>(args)...);
		}
	}
}

template<typename... ARGS>
//[[gnu::format(printf, 1, 2)]]
void ClientMsgAll(const char *fmt, ARGS... args)
{
	int n_clients = sv->GetNumClients();
	for (int i = 0; i < n_clients; ++i) {
		IClient *client = sv->GetClient(i);
		if (client == nullptr) continue;
		
		client->ClientPrintf(fmt, std::forward<ARGS>(args)...);
	}
}


#endif
