#include "mod.h"


namespace Mod::Util::Search_Paths
{
	void AddSearchPath(SearchPathAdd_t where, const CCommand& args)
	{
		if (args.ArgC() < 3) {
			Msg("Usage: %s <Dir> <PathID>\n", args[0]);
			return;
		}
		
		const char *pszDir    = args[1];
		const char *pszPathID = args[2];
		
		Msg("Adding dir '%s' with path ID '%s' to the search path list %s.\n",
			pszDir, pszPathID, (where == PATH_ADD_TO_HEAD ? "head" : "tail"));
		filesystem->AddSearchPath(pszDir, pszPathID, where);
	}
	
	
	CON_COMMAND(sig_util_search_path_add_to_head, "Add a filesystem search path to the head of the list.")
	{
		AddSearchPath(PATH_ADD_TO_HEAD, args);
	}
	
	CON_COMMAND(sig_util_search_path_add_to_tail, "Add a filesystem search path to the tail of the list.")
	{
		AddSearchPath(PATH_ADD_TO_TAIL, args);
	}
	
	
	CON_COMMAND(sig_util_search_path_remove, "Remove a filesystem search path from the list.")
	{
		if (args.ArgC() < 3) {
			Msg("Usage: %s <Dir> <PathID>\n", args[0]);
			return;
		}
		
		const char *pszDir    = args[1];
		const char *pszPathID = args[2];
		
		Msg("Removing dir '%s' with path ID '%s' from the search path list.\n",
			pszDir, pszPathID);
		bool bSuccess = filesystem->RemoveSearchPath(pszDir, pszPathID);
		
		if (bSuccess) {
			Msg("Succeeded.\n");
		} else {
			Msg("Failed.\n");
		}
		
		// TODO: maybe make the PathID parameter optional (and pass it as nullptr to RemoveSearchPath)
	}
}
