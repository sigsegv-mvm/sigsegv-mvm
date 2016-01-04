#include "extension.h"


DETOUR_DECL_MEMBER_V(D_CTFGameRules_PushAllPlayersAway, void, const Vector *v1, float f1, float f2, int i1, CUtlVector<CTFPlayer *> *vec)
{
	int offset;
	assert(g_pGameConf->GetOffset("CTFGameRules::m_bPlayingMannVsMachine", &offset));
	bool m_bPlayingMannVsMachine = *(bool *)((uintptr_t)this + offset);
	
	if (!m_bPlayingMannVsMachine) {
		DETOUR_MEMBER_CALL(D_CTFGameRules_PushAllPlayersAway)(v1, f1, f2, i1, vec);
	}
}


class CMod_EngieNoPush : public IMod
{
public:
	virtual bool OnLoad() override;
	virtual void OnUnload() override;
	
	void SetEnabled(bool enable);
	
private:
	bool m_bEnabled;
};
static CMod_EngieNoPush s_Mod;


static CDetour *detour_PushAllPlayersAway;

static ConVar cvar_enable("sigsegv_engienopush_enable", "0", FCVAR_NOTIFY,
	"Mod: remove engiebot push force when spawning and building",
	[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
		ConVarRef var(pConVar);
		s_Mod.SetEnabled(var.GetBool());
	});


bool CMod_EngieNoPush::OnLoad()
{
	detour_PushAllPlayersAway = DETOUR_CREATE_MEMBER(D_CTFGameRules_PushAllPlayersAway, "CTFGameRules::PushAllPlayersAway");
	if (detour_PushAllPlayersAway == nullptr) {
		return false;
	}
	
	return true;
}

void CMod_EngieNoPush::OnUnload()
{
	if (detour_PushAllPlayersAway != nullptr) {
		detour_PushAllPlayersAway->Destroy();
		detour_PushAllPlayersAway = nullptr;
	}
}


void CMod_EngieNoPush::SetEnabled(bool enable)
{
	if (enable) {
		detour_PushAllPlayersAway->EnableDetour();
	} else {
		detour_PushAllPlayersAway->DisableDetour();
	}
	
	this->m_bEnabled = enable;
}
