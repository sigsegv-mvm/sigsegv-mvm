#include "mod.h"
#include "factory.h"


#if defined _WINDOWS

namespace Mod::Etc::Release_Source_Mutex
{
	bool IsClient()
	{
		return (ClientFactory() != nullptr);
	}
	
	const char *GetWinErrorString()
	{
		static char msg[1024];
		
		msg[0] = '\0';
		::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, ::GetLastError(), 0, msg, sizeof(msg), nullptr);
		
		/* chop off any trailing newline characters */
		for (int i = strlen(msg) - 1; i >= 0; --i) {
			if (msg[i] == '\r' || msg[i] == '\n') {
				msg[i] = '\0';
			}
		}
		
		return msg;
	}
	
	
	CON_COMMAND(sig_etc_release_source_mutex, "")
	{
		if (!IsClient()) {
			Msg("Not a client instance.\n");
			return;
		}
		
		HANDLE hMutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, TEXT("hl2_singleton_mutex"));
		if (hMutex == NULL) {
			Msg("OpenMutex failed: %s\n", GetWinErrorString());
			return;
		}
		
		bool success = true;
		
		if (!::ReleaseMutex(hMutex)) {
			Msg("ReleaseMutex failed: %s\n", GetWinErrorString());
		} else {
			Msg("Success.\n");
		}
		
		if (!::CloseHandle(hMutex)) {
			Msg("CloseHandle failed: %s\n", GetWinErrorString());
		}
	}
}

#endif
