#include "mod.h"
#include "stub/tfbot.h"
#include "stub/entities.h"
#include "stub/strings.h"
#include "stub/usermessages_sv.h"
#include "util/iterate.h"


namespace Mod_Debug_Robot_Pose_Parameters
{
	void PrintToChatAll(const char *str)
	{
		int msg_type = usermessages->LookupUserMessage("SayText2");
		if (msg_type == -1) return;
		
		CReliableBroadcastRecipientFilter filter;
		
		bf_write *msg = engine->UserMessageBegin(&filter, msg_type);
		if (msg == nullptr) return;
		
		msg->WriteByte(0x00);
		msg->WriteByte(0x00);
		msg->WriteString(str);
		
		engine->MessageEnd();
	}
	
	
	CON_COMMAND(sig_debug_robot_pose_parameters_recalc, "")
	{
		ForEachTFPlayer([](CTFPlayer *player){
			CMultiplayerAnimState *m_PlayerAnimState = player->m_PlayerAnimState;
			m_PlayerAnimState->OnNewModel();
		});
	}
	
	CON_COMMAND(sig_debug_robot_pose_parameters_change, "")
	{
		CTFPlayer *host = ToTFPlayer(UTIL_PlayerByIndex(1));
		if (host == nullptr) return;
		
		auto l_switch = [=](int klass, bool giant){
			host->HandleCommand_JoinTeam_NoMenus("blue");
			
			host->HandleCommand_JoinClass(g_aRawPlayerClassNames[klass]);
			
			host->SetMiniBoss(giant);
			host->SetModelScale(giant ? 1.75f : 1.00f);
			
			const char *model_path = (giant ? g_szBotBossModels[klass] : g_szBotModels[klass]);
			
			host->GetPlayerClass()->SetCustomModel(model_path);
			host->UpdateModel();
			
			host->ForceRespawn();
			
			PrintToChatAll(CFmtStr("Switched to: %s %s (\"%s\")\n", (giant ? "giant" : "normal"), g_aRawPlayerClassNames[klass], model_path));
		};
		
	//	if (FStrEq(args[1], "scout")) l_switch(TF_CLASS_SCOUT, false);
		
		static int n = 0;
		l_switch((n % 9) + 1, ((n / 9) % 2 == 1));
		++n;
	}
	
	CON_COMMAND(sig_debug_robot_pose_parameters_dump, "")
	{
	//	CTFPlayer *host = ToTFPlayer(UTIL_PlayerByIndex(1));
	//	if (host == nullptr) {
	//		Warning("host == nullptr\n");
	//		return;
	//	}
	//	
	//	host->SetModel(args[1]);
	//	
	//	CStudioHdr *pStudioHdr = host->GetModelPtr();
	//	if (pStudioHdr == nullptr) {
	//		Warning("pStudioHdr == nullptr\n");
	//		return;
	//	}
		
		MDLHandle_t handle = mdlcache->FindMDL(args[1]);
		Msg("handle = %04x\n", handle);
		return;
		
		
	//	Msg("\n");
	//	Msg("POSE PARAMETER INFO FOR: \"%s\"\n", args[1]);
	//	Msg("================================================================================\n");
	//	
	//	int nParams = pStudioHdr->GetNumPoseParameters();
	//	Msg("Number of pose parameters: %d\n", nParams);
	//	for (int i = 0; i < nParams; ++i) {
	//		const auto& param = pStudioHdr->pPoseParameter(i);
	//		Msg("[%d] \"%s\" %.2f~%.2f %.2f %08x\n", i, param.pszName(), param.start, param.end, param.loop, param.flags);
	//	}
	//	Msg("\n");
	}
	
	
	// TODO: need a list of the various robot models' pose parameter orders
	// TODO: generate list of all interesting permutations
	
	// TODO: might need a debug overlay to show pose parameters live, if the built-in one doesn't do what we need
	
	
	// turn off cosmetics because they don't all look good...
	DETOUR_DECL_MEMBER(bool, CTFWearable_CanEquip, CBaseEntity *pEntity)
	{
		return false;
	}
	
	// don't drop ammo packs on death
	DETOUR_DECL_MEMBER(bool, CTFPlayer_ShouldDropAmmoPack)
	{
		return false;
	}
	
	
	class CMod : public IMod, public IFrameUpdateListener
	{
	public:
		CMod() : IMod("Debug:Robot_Pose_Parameters")
		{
			MOD_ADD_DETOUR_MEMBER(CTFWearable_CanEquip,         "CTFWearable::CanEquip");
			MOD_ADD_DETOUR_MEMBER(CTFPlayer_ShouldDropAmmoPack, "CTFPlayer::ShouldDropAmmoPack");
		}
		
		virtual bool ShouldReceiveFrameEvents() const override { return this->IsEnabled(); }
		
		virtual void FrameUpdatePostEntityThink() override
		{
			// get rid of ragdolls
			ForEachEntityByRTTI<CTFRagdoll>([](CTFRagdoll *ragdoll){
				ragdoll->Remove();
			});
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_debug_robot_pose_parameters", "0", FCVAR_NOTIFY,
		"Debug: demonstrate screwed-up robot player model pose parameters' effect on hitboxes",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
}
