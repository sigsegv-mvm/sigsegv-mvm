#include "mod.h"
#include "factory.h"


#if defined _WINDOWS

namespace Mod_Etc_Release_Source_Mutex
{
	bool IsClient()
	{
		return (ClientFactory() != nullptr);
	}
	
	const char *GetWinErrorString()
	{
		static char msg[1024];
		::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, ::GetLastError(), 0, msg, sizeof(msg), nullptr);
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
		
		if (!::ReleaseMutex(hMutex)) {
			Msg("ReleaseMutex failed: %s\n", GetWinErrorString());
		}
		
		if (!::CloseHandle(hMutex)) {
			Msg("CloseHandle failed: %s\n", GetWinErrorString());
		}
	}
}

#endif
