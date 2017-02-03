#include "mod.h"
#include "stub/populators.h"
#include "mod/pop/kv_conditional.h"
#include "stub/usermessages_sv.h"
#include "stub/entities.h"
#include "stub/objects.h"
#include "stub/gamerules.h"
#include "util/scope.h"
#include "util/iterate.h"


namespace Mod_Pop_Wave_Extensions
{
	struct SentryGunInfo
	{
		bool use_hint = true;
		std::vector<CHandle<CTFBotHintSentrygun>> hints;
		Vector origin;
		QAngle angles;
		
		int teamnum = TF_TEAM_BLUE;
		float delay = 0.0f;
		int level   = -1;
		
		bool spawned = false;
	};
	
	struct WaveData
	{
		std::vector<std::string> explanation;
		std::vector<SentryGunInfo> sentryguns;
		std::vector<std::string> sound_loops;
		
		IntervalTimer t_wavestart;
	};
	
	
	std::map<CWave *, WaveData> waves;
	
	
	DETOUR_DECL_MEMBER(void, CWave_dtor0)
	{
		auto wave = reinterpret_cast<CWave *>(this);
		
//		DevMsg("CWave %08x: dtor0\n", (uintptr_t)wave);
		waves.erase(wave);
		
		DETOUR_MEMBER_CALL(CWave_dtor0)();
	}
	
	DETOUR_DECL_MEMBER(void, CWave_dtor2)
	{
		auto wave = reinterpret_cast<CWave *>(this);
		
//		DevMsg("CWave %08x: dtor2\n", (uintptr_t)wave);
		waves.erase(wave);
		
		DETOUR_MEMBER_CALL(CWave_dtor2)();
	}
	
	
	bool FindSentryHint(const char *name, std::vector<CHandle<CTFBotHintSentrygun>>& hints)
	{
		ForEachEntityByClassname("bot_hint_sentrygun", [&](CBaseEntity *ent){
			if (FStrEq(STRING(ent->GetEntityName()), name)) {
				auto hint = rtti_cast<CTFBotHintSentrygun *>(ent);
				if (hint != nullptr) {
					hints.emplace_back(hint);
				}
			}
		});
		
		return !hints.empty();
	}
	
	
	void Parse_Explanation(CWave *wave, KeyValues *kv)
	{
		waves[wave].explanation.clear();
		
		FOR_EACH_SUBKEY(kv, subkey) {
			waves[wave].explanation.emplace_back(subkey->GetString());
		}
	}
	
	void Parse_SentryGun(CWave *wave, KeyValues *kv)
	{
		SentryGunInfo info;
		
		FOR_EACH_SUBKEY(kv, subkey) {
			const char *name = subkey->GetName();
			
			if (FStrEq(name, "HintName")) {
				if (!FindSentryHint(subkey->GetString(), info.hints)) {
					Warning("Could not find a bot_hint_sentrygun entity named \"%s\".\n", subkey->GetString());
					return;
				}
			} else if (FStrEq(name, "TeamNum")) {
				info.teamnum = Clamp(subkey->GetInt(), (int)TF_TEAM_RED, (int)TF_TEAM_BLUE);
			//	DevMsg("TeamNum \"%s\" --> %d\n", subkey->GetString(), info.teamnum);
			} else if (FStrEq(name, "Delay")) {
				info.delay = Max(0.0f, subkey->GetFloat());
			//	DevMsg("Delay \"%s\" --> %.1f\n", subkey->GetString(), info.delay);
			} else if (FStrEq(name, "Level")) {
				info.level = Clamp(subkey->GetInt(), 1, 3);
			//	DevMsg("Level \"%s\" --> %d\n", subkey->GetString(), info.level);
			} else if (FStrEq(name, "Position")) {
				FOR_EACH_SUBKEY(subkey, subsub) {
					const char *name = subsub->GetName();
					float value      = subsub->GetFloat();
					
					if (FStrEq(name, "X")) {
						info.origin.x = value;
					} else if (FStrEq(name, "Y")) {
						info.origin.y = value;
					} else if (FStrEq(name, "Z")) {
						info.origin.z = value;
					} else if (FStrEq(name, "Pitch")) {
						info.angles.x = value;
					} else if (FStrEq(name, "Yaw")) {
						info.angles.y = value;
					} else if (FStrEq(name, "Roll")) {
						info.angles.z = value;
					} else {
						Warning("Unknown key \'%s\' in SentryGun Position sub-block.\n", name);
					}
				}
				info.use_hint = false;
			} else {
				Warning("Unknown key \'%s\' in SentryGun block.\n", name);
			}
		}
		
		bool fail = false;
		if (info.use_hint && info.hints.empty()) {
			Warning("Missing HintName key or Position block in SentryGun block.\n");
			fail = true;
		}
		if (info.level == -1) {
			Warning("Missing Level key in SentryGun block.\n");
			fail = true;
		}
		if (fail) return;
		
		DevMsg("Wave %08x: add SentryGun\n", (uintptr_t)wave);
		waves[wave].sentryguns.push_back(info);
	}
	
	void Parse_SoundLoop(CWave *wave, KeyValues *kv)
	{
		if (!waves[wave].sound_loops.empty()) {
			Warning("Multiple \'SoundLoop\' blocks found in the same Wave!\n");
			return;
		}
		
		FOR_EACH_SUBKEY(kv, subkey) {
			const char *name = subkey->GetName();
			
			if (FStrEq(name, "SoundFile")) {
				waves[wave].sound_loops.push_back(subkey->GetString());
			} else {
				Warning("Unknown key \'%s\' in SoundLoop block.\n", name);
			}
		}
	}
	
	DETOUR_DECL_MEMBER(bool, CWave_Parse, KeyValues *kv)
	{
		auto wave = reinterpret_cast<CWave *>(this);
		
	//	DevMsg("CWave::Parse\n");
		
		std::vector<KeyValues *> del_kv;
		FOR_EACH_SUBKEY(kv, subkey) {
			const char *name = subkey->GetName();
			
			bool del = true;
			if (FStrEq(name, "Explanation")) {
				Parse_Explanation(wave, subkey);
			} else if (FStrEq(name, "SentryGun")) {
				Parse_SentryGun(wave, subkey);
			} else if (FStrEq(name, "SoundLoop")) {
				Parse_SoundLoop(wave, subkey);
			} else {
				del = false;
			}
			
			if (del) {
	//			DevMsg("Key \"%s\": processed, will delete\n", name);
				del_kv.push_back(subkey);
			} else {
	//			DevMsg("Key \"%s\": passthru\n", name);
			}
		}
		
		for (auto subkey : del_kv) {
	//		DevMsg("Deleting key \"%s\"\n", subkey->GetName());
			kv->RemoveSubKey(subkey);
			subkey->deleteThis();
		}
		
		return DETOUR_MEMBER_CALL(CWave_Parse)(kv);
	}
	
	
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
	
	void ParseColorsAndPrint(const char *line)
	{
		std::vector<char> output;
		
		/* always start with reset so that colors will work properly */
		output.push_back('\x01');
		
		bool in_braces = false;
		int brace_idx = 0;
		
		int i = 0;
		char c;
		while ((c = line[i]) != '\0') {
			if (in_braces) {
				if (c == '}') {
					const char *brace_str = line + brace_idx;
					int brace_len = i - brace_idx;
					
					if (V_strnicmp(brace_str, "Reset", brace_len) == 0) {
						output.push_back('\x01');
					} else if (V_strnicmp(brace_str, "Blue", brace_len) == 0) {
						output.insert(output.end(), {'\x07', '9', '9', 'c', 'c', 'f', 'f'});
					} else if (V_strnicmp(brace_str, "Red", brace_len) == 0) {
						output.insert(output.end(), {'\x07', 'f', 'f', '3', 'f', '3', 'f'});
					} else if (V_strnicmp(brace_str, "Green", brace_len) == 0) {
						output.insert(output.end(), {'\x07', '9', '9', 'f', 'f', '9', '9'});
					} else if (V_strnicmp(brace_str, "DarkGreen", brace_len) == 0) {
						output.insert(output.end(), {'\x07', '4', '0', 'f', 'f', '4', '0'});
					} else if (V_strnicmp(brace_str, "Yellow", brace_len) == 0) {
						output.insert(output.end(), {'\x07', 'f', 'f', 'b', '2', '0', '0'});
					} else if (V_strnicmp(brace_str, "Grey", brace_len) == 0) {
						output.insert(output.end(), {'\x07', 'c', 'c', 'c', 'c', 'c', 'c'});
					} else {
						/* RGB hex code */
						if (brace_len >= 6) {
							output.push_back('\x07');
							output.push_back(brace_str[0]);
							output.push_back(brace_str[1]);
							output.push_back(brace_str[2]);
							output.push_back(brace_str[3]);
							output.push_back(brace_str[4]);
							output.push_back(brace_str[5]);
						}
					}
					
					in_braces = false;
				}
			} else {
				if (c == '{') {
					in_braces = true;
					brace_idx = i + 1;
				} else {
					output.push_back(c);
				}
			}
			
			++i;
		}
		
		/* append an extra space at the end to make empty lines show up */
		output.push_back(' ');
		output.push_back('\n');
		output.push_back('\0');
		
		PrintToChatAll(output.data());
	}
	
	void ShowWaveExplanation()
	{
		/* wave will be null after game is won and in other corner cases */
		CWave *wave = g_pPopulationManager->GetCurrentWave();
		if (wave == nullptr) return;
		
		const auto& explanation = waves[wave].explanation;
		for (const auto& line : explanation) {
			ParseColorsAndPrint(line.c_str());
		}
	}
	
	
	RefCount rc_JumpToWave;
	DETOUR_DECL_MEMBER(void, CPopulationManager_JumpToWave, unsigned int wave, float f1)
	{
		SCOPED_INCREMENT(rc_JumpToWave);
	//	DevMsg("[%8.3f] JumpToWave\n", gpGlobals->curtime);
		DETOUR_MEMBER_CALL(CPopulationManager_JumpToWave)(wave, f1);
		ShowWaveExplanation();
	}
	
	DETOUR_DECL_MEMBER(void, CPopulationManager_WaveEnd, bool b1)
	{
	//	DevMsg("[%8.3f] WaveEnd\n", gpGlobals->curtime);
		DETOUR_MEMBER_CALL(CPopulationManager_WaveEnd)(b1);
		ShowWaveExplanation();
	}
	
	DETOUR_DECL_MEMBER(void, CMannVsMachineStats_RoundEvent_WaveEnd, bool success)
	{
		DETOUR_MEMBER_CALL(CMannVsMachineStats_RoundEvent_WaveEnd)(success);
		if (!success && rc_JumpToWave == 0) {
		//	DevMsg("[%8.3f] RoundEvent_WaveEnd\n", gpGlobals->curtime);
			ShowWaveExplanation();
		}
	}
	
	
	void SpawnSentryGun(const Vector& origin, const QAngle& angles, int teamnum, int level)
	{
		auto sentry = rtti_cast<CObjectSentrygun *>(CreateEntityByName("obj_sentrygun"));
		if (sentry == nullptr) {
			Warning("SpawnSentryGun: CreateEntityByName(\"obj_sentrygun\") failed\n");
			return;
		}
		
	//	DevMsg("[%8.3f] SpawnSentryGun: [hint #%d \"%s\"] [teamnum %d] [level %d]\n",
	//		gpGlobals->curtime, ENTINDEX(sg_info.hint), STRING(sg_info.hint->GetEntityName()), sg_info.teamnum, sg_info.level);
		
		sentry->SetAbsOrigin(origin);
		sentry->SetAbsAngles(angles);
		
		sentry->Spawn();
		
		sentry->ChangeTeam(teamnum);
		sentry->m_nDefaultUpgradeLevel = level - 1;
		
		sentry->InitializeMapPlacedObject();
		
		DevMsg("SpawnSentryGun: #%d, %08x, level %d, health %d, maxhealth %d\n",
			ENTINDEX(sentry), (uintptr_t)sentry, level, sentry->GetHealth(), sentry->GetMaxHealth());
	}
	
	void SpawnSentryGuns(SentryGunInfo& sg_info)
	{
		sg_info.spawned = true;
		
		if (sg_info.use_hint && sg_info.hints.empty()) {
			Warning("SpawnSentryGuns: sg_info.hints.empty()\n");
			return;
		}
		
		if (sg_info.use_hint) {
			for (const auto& hint : sg_info.hints) {
				SpawnSentryGun(hint->GetAbsOrigin(), hint->GetAbsAngles(), sg_info.teamnum, sg_info.level);
			}
		} else {
			SpawnSentryGun(sg_info.origin, sg_info.angles, sg_info.teamnum, sg_info.level);
		}
	}
	
	
	DETOUR_DECL_MEMBER(void, CWave_ActiveWaveUpdate)
	{
		DETOUR_MEMBER_CALL(CWave_ActiveWaveUpdate)();
		
		auto wave = reinterpret_cast<CWave *>(this);
		
		auto it = waves.find(wave);
		if (it != waves.end()) {
			WaveData& data = (*it).second;
			
			if (!data.t_wavestart.HasStarted()) {
				data.t_wavestart.Start();
			}
			
			for (auto& sg_info : data.sentryguns) {
				if (!sg_info.spawned && !data.t_wavestart.IsLessThen(sg_info.delay)) {
					SpawnSentryGuns(sg_info);
				}
			}
		}
	}
	
	
	std::string soundloop_active;
	
	
	void StopSoundLoop()
	{
		ConColorMsg(Color(0xff, 0x00, 0x00, 0xff), "[SoundLoop] StopSoundLoop \"%s\"\n", soundloop_active.c_str());
		
		if (TFGameRules() != nullptr) {
			TFGameRules()->BroadcastSound(SOUND_FROM_LOCAL_PLAYER, soundloop_active.c_str(), SND_STOP);
		}
		
		soundloop_active.clear();
	}
	
	void StartSoundLoop(const std::string& filename)
	{
		if (!soundloop_active.empty()) {
			StopSoundLoop();
		}
		
		ConColorMsg(Color(0x00, 0xff, 0x00, 0xff), "[SoundLoop] StartSoundLoop \"%s\"\n", filename.c_str());
		
		/* if filename is explicitly "", then don't play anything */
		if (TFGameRules() != nullptr && filename != "") {
			TFGameRules()->BroadcastSound(SOUND_FROM_LOCAL_PLAYER, filename.c_str(), SND_NOFLAGS);
			soundloop_active = filename;
		}
	}
	
	
	const char *GetRoundStateName(gamerules_roundstate_t state)
	{
		switch (state) {
		case GR_STATE_INIT:         return "INIT";
		case GR_STATE_PREGAME:      return "PREGAME";
		case GR_STATE_STARTGAME:    return "STARTGAME";
		case GR_STATE_PREROUND:     return "PREROUND";
		case GR_STATE_RND_RUNNING:  return "RND_RUNNING";
		case GR_STATE_TEAM_WIN:     return "TEAM_WIN";
		case GR_STATE_RESTART:      return "RESTART";
		case GR_STATE_STALEMATE:    return "STALEMATE";
		case GR_STATE_GAME_OVER:    return "GAME_OVER";
		case GR_STATE_BONUS:        return "BONUS";
		case GR_STATE_BETWEEN_RNDS: return "BETWEEN_RNDS";
		default:                    return "???";
		}
	}
	
	
	DETOUR_DECL_MEMBER(void, CTeamplayRoundBasedRules_State_Enter, gamerules_roundstate_t newState)
	{
		auto oldState = TeamplayRoundBasedRules()->State_Get();
		
		ConColorMsg(Color(0xff, 0x00, 0xff, 0xff),
			"[SoundLoop] CTeamplayRoundBasedRules: %s -> %s\n",
			GetRoundStateName(oldState), GetRoundStateName(newState));
		
		if (oldState != GR_STATE_RND_RUNNING && newState == GR_STATE_RND_RUNNING) {
			if (TFGameRules()->IsMannVsMachineMode()) {
				auto it = waves.find(g_pPopulationManager->GetCurrentWave());
				if (it != waves.end()) {
					WaveData& data = (*it).second;
					
					if (!data.sound_loops.empty()) {
						std::string sound_loop = *select_random(data.sound_loops);
						StartSoundLoop(sound_loop);
					}
				}
			}
		} else if (oldState == GR_STATE_RND_RUNNING && newState != GR_STATE_RND_RUNNING) {
			StopSoundLoop();
		}
		
		DETOUR_MEMBER_CALL(CTeamplayRoundBasedRules_State_Enter)(newState);
	}
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Pop:Wave_Extensions")
		{
			MOD_ADD_DETOUR_MEMBER(CWave_dtor0, "CWave::~CWave [D0]");
			MOD_ADD_DETOUR_MEMBER(CWave_dtor2, "CWave::~CWave [D2]");
			
			MOD_ADD_DETOUR_MEMBER(CWave_Parse, "CWave::Parse");
			
			MOD_ADD_DETOUR_MEMBER(CPopulationManager_JumpToWave,          "CPopulationManager::JumpToWave");
			MOD_ADD_DETOUR_MEMBER(CPopulationManager_WaveEnd,             "CPopulationManager::WaveEnd");
			MOD_ADD_DETOUR_MEMBER(CMannVsMachineStats_RoundEvent_WaveEnd, "CMannVsMachineStats::RoundEvent_WaveEnd");
			
			MOD_ADD_DETOUR_MEMBER(CWave_ActiveWaveUpdate, "CWave::ActiveWaveUpdate");
			
			MOD_ADD_DETOUR_MEMBER(CTeamplayRoundBasedRules_State_Enter, "CTeamplayRoundBasedRules::State_Enter");
		}
		
		virtual void OnUnload() override
		{
			waves.clear();
			StopSoundLoop();
		}
		
		virtual void OnDisable() override
		{
			waves.clear();
			StopSoundLoop();
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_pop_wave_extensions", "0", FCVAR_NOTIFY,
		"Mod: enable extended KV in CWave::Parse",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.Toggle(var.GetBool());
		});
	
	
	class CKVCond_Wave : public IKVCond
	{
	public:
		virtual bool operator()() override
		{
			return s_Mod.IsEnabled();
		}
	};
	CKVCond_Wave cond;
}


/* wave explanation TODO:

each time a wave starts, reset some tracking info
- keep track of which steam IDs have seen the explanation and which haven't

if a new player connects, and the wave has an explanation, and they haven't seen it,
then show it *just* to them, a few seconds after they pick a class






*/
