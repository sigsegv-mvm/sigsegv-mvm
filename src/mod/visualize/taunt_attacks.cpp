#include "mod.h"


//  0 TAUNTATK_NONE
//  1 TAUNTATK_PYRO_HADOUKEN
//  2 TAUNTATK_HEAVY_EAT
//  3 TAUNTATK_HEAVY_RADIAL_BUFF
//  4 TAUNTATK_HEAVY_HIGH_NOON
//  5 TAUNTATK_SCOUT_DRINK
//  6 TAUNTATK_SCOUT_GRAND_SLAM
//  7 TAUNTATK_MEDIC_INHALE
//  8 TAUNTATK_SPY_FENCING_SLASH_A
//  9 TAUNTATK_SPY_FENCING_SLASH_B
//  a TAUNTATK_SPY_FENCING_STAB
//  b TAUNTATK_RPS_KILL
//  c TAUNTATK_SNIPER_ARROW_STAB_IMPALE
//  d TAUNTATK_SNIPER_ARROW_STAB_KILL
//  e TAUNTATK_SOLDIER_GRENADE_KILL
//  f TAUNTATK_DEMOMAN_BARBARIAN_SWING
// 10 TAUNTATK_MEDIC_UBERSLICE_IMPALE
// 11 TAUNTATK_MEDIC_UBERSLICE_KILL
// 12 TAUNTATK_FLIP_LAND_PARTICLE
// 13 TAUNTATK_RPS_PARTICLE
// 14 TAUNTATK_HIGHFIVE_PARTICLE
// 15 TAUNTATK_ENGINEER_GUITAR_SMASH
// 16 TAUNTATK_ENGINEER_ARM_IMPALE
// 17 TAUNTATK_ENGINEER_ARM_KILL
// 18 TAUNTATK_ENGINEER_ARM_BLEND
// 19 TAUNTATK_SOLDIER_GRENADE_KILL_WORMSIGN
// 1a TAUNTATK_SHOW_ITEM
// 1b TAUNTATK_MEDIC_RELEASE_DOVES
// 1c TAUNTATK_PYRO_ARMAGEDDON
// 1d TAUNTATK_PYRO_SCORCHSHOT
// 1e TAUNTATK_ALLCLASS_GUITAR_RIFF
// 1f TAUNTATK_MEDIC_HEROIC_TAUNT


// video with taunt attacks: hit detection type, damage type, distances, multi target, etc

// 1d TAUNTATK_PYRO_SCORCHSHOT
// ?

// 1c TAUNTATK_PYRO_ARMAGEDDON
// multi target
// sphere detection: 100 HU radius from your abs origin

// 04 TAUNTATK_HEAVY_HIGH_NOON
// single target
// trace from eye, extending 500 HU

// 16 TAUNTATK_ENGINEER_ARM_IMPALE
// 17 TAUNTATK_ENGINEER_ARM_KILL
// 18 TAUNTATK_ENGINEER_ARM_BLEND
// 0c TAUNTATK_SNIPER_ARROW_STAB_IMPALE
// 0d TAUNTATK_SNIPER_ARROW_STAB_KILL
// 10 TAUNTATK_MEDIC_UBERSLICE_IMPALE
// 11 TAUNTATK_MEDIC_UBERSLICE_KILL
// 0f TAUNTATK_DEMOMAN_BARBARIAN_SWING
// 15 TAUNTATK_ENGINEER_GUITAR_SMASH
// single target
// trace from eye, extending 128 HU

// 0e TAUNTATK_SOLDIER_GRENADE_KILL
// detection center is from the soldier's actual hand bone position
// is radius damage with splash falloff

// 06 TAUNTATK_SCOUT_GRAND_SLAM
// 01 TAUNTATK_PYRO_HADOUKEN
// 08 TAUNTATK_SPY_FENCING_SLASH_A
// 09 TAUNTATK_SPY_FENCING_SLASH_B
// 0a TAUNTATK_SPY_FENCING_STAB
// multi target
// box detection: 48x48x48, 64 HU from your WSC in ? direction


namespace Mod_Visualize_Taunt_Attacks
{
	
	
	
	class CMod : public IMod
	{
	public:
		CMod() : IMod("Visualize:Taunt_Attacks")
		{
			
		}
		
		void SetEnabled(bool enable)
		{
			this->ToggleAllDetours(enable);
		}
	};
	CMod s_Mod;
	
	
	ConVar cvar_enable("sig_visualize_taunt_attacks", "0", FCVAR_NOTIFY,
		"Visualization: draw taunt kill detection",
		[](IConVar *pConVar, const char *pOldValue, float flOldValue) {
			ConVarRef var(pConVar);
			s_Mod.SetEnabled(var.GetBool());
		});
}
