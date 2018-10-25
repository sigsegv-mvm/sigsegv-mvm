#include "mod.h"
#include "mod/pop/kv_conditional.h"
#include "stub/tfbot.h"


// update data with every call to:
// - CTFBot::OnEventChangeAttributes
//   (verify that this does get called even for the initial default set of settings when spawning)

// clear data with every call to:
// - CTFBot::~CTFBot (D0+D2)
// - CTFBot::Event_Killed
// - CTFBot::ChangeTeam(TEAM_SPECTATOR)
// - CTFBot::ForceChangeTeam(TEAM_SPECTATOR)

// we probably want maps of:
// - CHandle<CTFBot> -> their CTFBotSpawner of origin
// - CHandle<CTFBot> -> their current EventChangeAttributes_t * (in their CTFBotSpawner of origin)


#if 0
namespace Mod::Pop::ECAttr_Extensions
{
	/* maps ECAttr instances -> extra data instances */
	std::map<CTFBot::EventChangeAttributes_t *, CTFBot::ExtraData> extdata;
	
	/* maps CTFBot instances -> their current ECAttr instance */
	std::map<int, CTFBot::EventChangeAttributes_t *> ecattr_map;
	
#if 0
	/* maps CTFBot instances -> their current ECAttr name */
	std::map<CHandle<CTFBot>, std::string> ecattr_map;
#endif
	
	
#if 0
	const std::string& GetCurrentTFBotECAttrName(CTFBot *bot)
	{
		CHandle<CTFBot> handle = bot;
		
		auto it = ecattr_map.find(handle);
		if (it == ecattr_map.end()) {
			return "default";
		}
		return *it;
	}
#endif
	
	
	// ecattr_map:
	// clear in OnUnload and SetEnabled(false)
	// update every time CTFBot::OnEventChangeAttributes is called
	// 
	
	
	const CTFBot::ExtraData& CTFBot::Ext() const
	{
		static CTFBot::ExtraData empty;
		
		auto it1 = ecattr_map.find(ENTINDEX(this));
		if (it1 == ecattr_map.end()) {
			return empty;
		}
		auto ecattr = *it1;
		
		auto it2 = extdata.find(ecattr);
		if (it2 == extdata.end()) {
			return empty;
		}
		return *it2;
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Pop:ECAttr_Extensions")
		{
			
		}
		
		virtual void OnUnload() override
		{
			extdata.clear();
		}
		
		virtual void OnDisable() override
		{
			extdata.clear();
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_pop_ecattr_extensions", "0", FCVAR_NOTIFY,
		"Mod: enable extended KV in EventChangeAttributes",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue){
			s_Mod.Toggle(static_cast<ConVar *>(pConVar)->GetBool());
		});
	
	
	class CKVCond_ECAttr : public IKVCond
	{
	public:
		virtual bool operator()() override
		{
			return s_Mod.IsEnabled();
		}
	};
	CKVCond_ECAttr cond;
}
#endif
