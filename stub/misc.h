#ifndef _INCLUDE_SIGSEGV_STUB_MISC_H_
#define _INCLUDE_SIGSEGV_STUB_MISC_H_


enum
{
	TEAM_ANY = -2,
	
	TF_TEAM_RED  = 2,
	TF_TEAM_BLUE = 3,
	
	TF_TEAM_PVE_DEFENDERS = TF_TEAM_RED,
	TF_TEAM_PVE_INVADERS  = TF_TEAM_BLUE,
};


/* I invented this function, because this particular idiom comes up frequently;
 * I can't seem to actually find it anywhere in the 2013 SDK or the 2007 leak,
 * so I don't know if it actually is an inlined function or just a copypasta */
inline int GetEnemyTeam(CBaseEntity *ent)
{
	int enemy_team = ent->GetTeamNumber();
	
	switch (enemy_team) {
	case TF_TEAM_RED:
		enemy_team = TF_TEAM_BLUE;
		break;
	case TF_TEAM_BLUE:
		enemy_team = TF_TEAM_RED;
		break;
	}
	
	return enemy_team;
}


class CBaseObject;

class CTFBotMvMEngineerTeleportSpawn;
class CTFBotMvMEngineerBuildTeleportExit;
class CTFBotMvMEngineerBuildSentryGun;


#endif
