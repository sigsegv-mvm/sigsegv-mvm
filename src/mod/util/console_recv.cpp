#include "mod.h"


namespace Mod_Util_Console_Recv
{
	class SpewBase
	{
	public:
		virtual ~SpewBase() {}
		
	protected:
		SpewBase() = default;
	};
	
	class SpewNewline : public SpewBase
	{
	public:
		SpewNewline() = default;
		virtual ~SpewNewline() {}
	};
	
	class SpewType : public SpewBase
	{
	public:
		SpewType(SpewType_t type) : m_Type(type) {}
		virtual ~SpewType() {}
		
		SpewType_t GetType() const { return this->m_Type; }
		
	private:
		SpewType_t m_Type;
	};
	
	class SpewColor : public SpewBase
	{
	public:
		SpewColor(const Color& color) : m_Color(color) {}
		virtual ~SpewColor() {}
		
		const Color& GetColor() const { return this->m_Color; }
		
	private:
		Color m_Color;
	};
	
	class SpewString : public SpewBase
	{
	public:
		SpewString(const char *c1, const char *c2) : m_String(c1, c2) {}
		virtual ~SpewString() {}
		
		const char *GetString() const { return this->m_String.c_str(); }
		
	private:
		std::string m_String;
	};
	
	#if 0
	class SpewString : public SpewData
	{
	public:
		SpewString(const char *str) : SpewData(STRING)
		{
			if (str == nullptr) {
				str = "";
			}
			
			this->m_pString = new char[strlen(str) + 1];
			strcpy(this->m_pString, str);
		}
		~SpewString()
		{
			delete[] this->m_pString;
		}
		
		const char *GetString() const { return this->m_pString; }
		
	private:
		const char *m_pString;
	};
	
	class SpewColor : public SpewData
	{
	public:
		SpewColor(const Color& color) : SpewData(COLOR)
		{
			m_Color = color;
		}
		
		const Color& GetColor() const { return this->m_Color; }
		
	private:
		Color m_Color;
	};
	#endif
	
	
	static std::deque<SpewBase *> buf_spew;
	
	
	#if 0
	void GotSpew(SpewType_t type, const Color& color, const char *str)
	{
		
		
		size_t from = 0;
		size_t to   = 0;
		
		for (size_t i = 0; str[i] != '\0'; ++i) {
			if (str[i] == '\n') {
				
			}
		}
	}
	#endif
	
	void ClearBufferedSpew()
	{
		buf_spew.clear();
	}
	
	
	class CMod : public IMod, public IGameEventListener2
	{
	public:
		CMod() : IMod("Util:Console_Recv") {}
		
		// TODO: line-buffer the forwarding (probably on the server side)
		// and then add a "[server] " (or similar) prefix on the client side
		// to distinguish the forwarded console lines from the local lines
		
		virtual void OnEnable() override
		{
		//	ClearBufferedSpew();
			gameeventmanager->AddListener(this, "console_forward", false);
		}
		virtual void OnDisable() override
		{
			gameeventmanager->RemoveListener(this);
		//	ClearBufferedSpew();
		}
		
		virtual void FireGameEvent(IGameEvent *event) override
		{
			if (strcmp(event->GetName(), "console_forward") == 0) {
				auto type = (SpewType_t)Clamp(event->GetInt("type", SPEW_MESSAGE), 0, SPEW_TYPE_COUNT - 1);
				
				Color color;
				color[0] = Clamp(event->GetInt("r", 0xff), 0x00, 0xff);
				color[1] = Clamp(event->GetInt("g", 0xff), 0x00, 0xff);
				color[2] = Clamp(event->GetInt("b", 0xff), 0x00, 0xff);
				color[3] = 0xff;
				
				const char *str = event->GetString("str", "");
				
			//	GotSpew(type, color, str);
				
				(void)ColorSpewMessage(type, &color, "%s", str);
			}
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_util_console_recv", "0", FCVAR_NOTIFY,
		"Utility: console forwarding: client receive",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
