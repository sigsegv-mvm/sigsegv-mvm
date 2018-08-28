#include "mod.h"
#include "stub/misc.h"
#include "util/misc.h"
#include "util/iterate.h"

#include <regex>


/*

Manual KV file format:

"Downloads"
{
	"Whatever1"
	{
		"Map" ".*"
		// Required key (must have at least one)
		// Uses case-insensitive regex matching
		// Otherwise, add one Map key per map that this block should apply to
		
		"File" "scripts/items/mvm_upgrades_sigsegv_v8.txt"
		// Required key (must have at least one)
		// Can have arbitrarily many
		// Wildcard matching enabled (only for last component of filename, not for directories)
		
		"Precache" "no"
		// Required key (can only appear once)
		// Can specify "No" for no precache
		// Can specify "Generic" for PrecacheGeneric
		// Can specify "Model" for PrecacheModel
		// Can specify "Decal" for PrecacheDecal
		// Can specify "Sound" for PrecacheSound
	}
	
	"Whatever2"
	{
		// ...
	}
}

*/


namespace Mod_Util_Download_Manager
{
	extern ConVar cvar_kvpath;
	
	
	struct DownloadBlock
	{
		DownloadBlock(                     ) = default;
		DownloadBlock(      DownloadBlock& ) = delete;
		DownloadBlock(const DownloadBlock& ) = delete;
		DownloadBlock(      DownloadBlock&&) = default;
		DownloadBlock(const DownloadBlock&&) = delete;
		
		std::string name;
		
		std::vector<std::string> keys_map;
		std::vector<std::string> keys_precache;
		std::vector<std::string> keys_file;
		
		void (*l_precache)(const char *) = [](const char *path){ /* do nothing */ };
	};
	
	
	void LoadDownloadsFile()
	{
	//	if (!g_pSM->IsMapRunning()) return;
		
		INetworkStringTable *downloadables = networkstringtable->FindTable("downloadables");
		if (downloadables == nullptr) {
			Warning("LoadDownloadsFile: String table \"downloadables\" apparently doesn't exist!\n");
			return;
		}
		
		auto kv = new KeyValues("Downloads");
		kv->UsesEscapeSequences(true);
		
		if (kv->LoadFromFile(filesystem, cvar_kvpath.GetString())) {
			std::vector<DownloadBlock> blocks;
			
			FOR_EACH_SUBKEY(kv, kv_block) {
				DownloadBlock block;
				bool errors = false;
				
				block.name = kv_block->GetName();
				
				FOR_EACH_SUBKEY(kv_block, kv_key) {
					const char *name  = kv_key->GetName();
					const char *value = kv_key->GetString();
					
					if (FStrEq(name, "Map")) {
						block.keys_map.emplace_back(value);
					} else if (FStrEq(name, "File")) {
						block.keys_file.emplace_back(value);
					} else if (FStrEq(name, "Precache")) {
						block.keys_precache.emplace_back(value);
					} else {
						Warning("LoadDownloadsFile: Block \"%s\": Invalid key type \"%s\"\n", block.name.c_str(), name);
						errors = true;
					}
				}
				
				if (block.keys_map.empty()) {
					Warning("LoadDownloadsFile: Block \"%s\": Must have at least one Map key\n", block.name.c_str());
					errors = true;
				}
				
				if (block.keys_file.empty()) {
					Warning("LoadDownloadsFile: Block \"%s\": Must have at least one File key\n", block.name.c_str());
					errors = true;
				}
				
				if (block.keys_precache.size() != 1) {
					Warning("LoadDownloadsFile: Block \"%s\": Must have exactly one Precache key\n", block.name.c_str());
					errors = true;
				}
				
				if (FStrEq(block.keys_precache[0].c_str(), "Generic")) {
					block.l_precache = [](const char *path){ engine->PrecacheGeneric(path, true); };
				} else if (FStrEq(block.keys_precache[0].c_str(), "Model")) {
					block.l_precache = [](const char *path){ CBaseEntity::PrecacheModel(path, true); };
				} else if (FStrEq(block.keys_precache[0].c_str(), "Decal")) {
					block.l_precache = [](const char *path){ engine->PrecacheDecal(path, true); };
				} else if (FStrEq(block.keys_precache[0].c_str(), "Sound")) {
					block.l_precache = [](const char *path){ enginesound->PrecacheSound(path, true); };
				} else if (!FStrEq(block.keys_precache[0].c_str(), "No")) {
					Warning("LoadDownloadsFile: Block \"%s\": Invalid Precache value \"%s\"\n", block.name.c_str(), block.keys_precache[0].c_str());
				}
				
				if (!errors) {
					blocks.push_back(std::move(block));
				} else {
					Warning("LoadDownloadsFile: Not applying block \"%s\" due to errors\n", block.name.c_str());
				}
			}
			
		//	std::vector<std::string> map_names;
		//	ForEachMapName([&](const char *map_name){
		//		map_names.emplace_back(map_name);
		//	});
			
#ifndef _MSC_VER
#warning NEED try/catch for std::regex ctor and funcs!
#endif
			
			for (const auto& block : blocks) {
			//	DevMsg("LoadDownloadsFile: Block \"%s\"\n", block.name.c_str());
				
				/* check each Map regex pattern against the current map and see if any is applicable */
				bool match = false;
				for (const auto& map_re : block.keys_map) {
					std::regex re(map_re, std::regex::ECMAScript | std::regex::icase);
					
					if (std::regex_match(STRING(gpGlobals->mapname), re, std::regex_constants::match_any)) {
					//	DevMsg("LoadDownloadsFile:   Map \"%s\" vs \"%s\": MATCH\n", map_re.c_str(), STRING(gpGlobals->mapname));
						match = true;
						break;
					} else {
					//	DevMsg("LoadDownloadsFile:   Map \"%s\" vs \"%s\": nope\n", map_re.c_str(), STRING(gpGlobals->mapname));
					}
				}
				if (!match) continue;
				
				/* for each File wildcard pattern, find all matching files and add+precache them */
				for (const auto& file_wild : block.keys_file) {
				//	DevMsg("LoadDownloadsFile:   File \"%s\":\n", file_wild.c_str());
					
					// TODO: maybe use an explicit PathID, rather than nullptr...?
					FileFindHandle_t handle;
					for (const char *file = filesystem->FindFirstEx(file_wild.c_str(), nullptr, &handle);
						file != nullptr; file = filesystem->FindNext(handle)) {
						char path[0x400];
						V_ExtractFilePath(file_wild.c_str(), path, sizeof(path));
						V_AppendSlash(path, sizeof(path));
						V_strcat_safe(path, file);
						
						if (filesystem->FindIsDirectory(handle)) {
						//	DevMsg("LoadDownloadsFile:     Skip Directory \"%s\"\n", path);
							continue;
						}
						
						const char *ext = V_GetFileExtension(path);
						if (ext != nullptr && FStrEq(ext, "bz2")) {
						//	DevMsg("LoadDownloadsFile:     Skip Bzip2 \"%s\"\n", path);
							continue;
						}
						
					//	DevMsg("LoadDownloadsFile:     Match \"%s\"\n", path);
						
					//	DevMsg("LoadDownloadsFile:       Precache\n");
						block.l_precache(path);
						
					//	DevMsg("LoadDownloadsFile:       StringTable\n");
						bool saved_lock = engine->LockNetworkStringTables(false);
						downloadables->AddString(true, path);
						engine->LockNetworkStringTables(saved_lock);
					}
					filesystem->FindClose(handle);
				}
			}
		} else {
			Warning("LoadDownloadsFile: Could not load KeyValues from \"%s\".\n", cvar_kvpath.GetString());
		}
		
		kv->deleteThis();
	}
	
	
	DETOUR_DECL_MEMBER(void, CServerGameDLL_ServerActivate, edict_t *pEdictList, int edictList, int clientMax)
	{
		DETOUR_MEMBER_CALL(CServerGameDLL_ServerActivate)(pEdictList, edictList, clientMax);
		
		LoadDownloadsFile();
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Util:Download_Manager")
		{
			MOD_ADD_DETOUR_MEMBER(CServerGameDLL_ServerActivate, "CServerGameDLL::ServerActivate");
		}
		
		virtual void OnEnable() override
		{
			LoadDownloadsFile();
		}
	};
	CMod s_Mod;
	
	
	// TODO: 'download/' prefix fixup for custom MvM upgrade files!
	// TODO: AUTOMATIC download entry generation by scanning directories!
	
	
	static void ReloadConfigIfModEnabled()
	{
		if (s_Mod.IsEnabled()) {
			LoadDownloadsFile();
		}
	}
	
	
	// is FCVAR_NOTIFY even a valid thing for commands...?
	CON_COMMAND_F(sig_util_download_manager_reload, "Utility: reload the configuration file", FCVAR_NOTIFY)
	{
		ReloadConfigIfModEnabled();
	}
	
	
	ConVar cvar_kvpath("sig_util_download_manager_kvpath", "cfg/downloads.kv", FCVAR_NOTIFY,
		"Utility: specify the path to the configuration file",
		[](IConVar *pConVar, const char *pOldValue, float fOldValue) {
			ReloadConfigIfModEnabled();
		});
	
	
	ConVar cvar_enable("sig_util_download_manager", "0", FCVAR_NOTIFY,
		"Utility: add custom downloads to the downloadables string table and tweak some things",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
