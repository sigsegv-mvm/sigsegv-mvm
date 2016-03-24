#include "mod.h"
#include "mod/pop/kv_conditional.h"
#include "stub/tfbot.h"


namespace Mod_Pop_ECAttr_Extensions
{
	/* maps ECAttr instances -> extra data instances */
	std::map<CTFBot::EventChangeAttributes *, CTFBot::ExtraData> extdata;
	
	/* maps CTFBot instances -> their current ECAttr instance */
	std::map<int, CTFBot::EventChangeAttributes *> ecattr_map;
	
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
		
		virtual void OnUnload()
		{
			extdata.clear();
		}
		
		void SetEnabled(bool enable)
		{
			this->ToggleAllDetours(enable);
			
			if (!enable) {
				extdata.clear();
			}
			
			this->m_bEnabled = enable;
		}
		
		bool IsEnabled() const { return this->m_bEnabled; }
		
	private:
		bool m_bEnabled = false;
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_pop_ecattr_extensions", "0", FCVAR_NOTIFY,
		"Mod: enable extended KV in EventChangeAttributes",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.SetEnabled(var.GetBool());
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
