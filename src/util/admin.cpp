#include "util/admin.h"
#include "stub/baseplayer.h"


static IGamePlayer *GetSMPlayer(CBasePlayer *player)
{
	if (player == nullptr) return nullptr;
	return playerhelpers->GetGamePlayer(player->edict());
}


AdminId GetPlayerSMAdminID(CBasePlayer *player)
{
	IGamePlayer *smplayer = GetSMPlayer(player);
	
	/* unsure whether this is strictly necessary */
	(void)smplayer->RunAdminCacheChecks();
	
	return smplayer->GetAdminId();
}


bool PlayerIsSMAdmin(CBasePlayer *player)
{
	AdminId id = GetPlayerSMAdminID(player);
	if (id == INVALID_ADMIN_ID) return false;
	
	/* not 100% sure on whether we really want to require this or not */
//	if (adminsys->GetAdminFlags(id, Access_Effective) == ADMFLAG_NONE) return false;
	
	return true;
}

bool PlayerIsSMAdminOrBot(CBasePlayer *player)
{
	return (player->IsBot() || PlayerIsSMAdmin(player));
}


FlagBits GetPlayerSMAdminFlags(CBasePlayer *player)
{
	AdminId id = GetPlayerSMAdminID(player);
	if (id == INVALID_ADMIN_ID) return ADMFLAG_NONE;
	
	FlagBits flags = adminsys->GetAdminFlags(id, Access_Effective);
	if ((flags & ADMFLAG_ROOT) != 0) return ADMFLAG_ALL;
	
	return flags;
}


bool PlayerHasSMAdminFlag(CBasePlayer *player, AdminFlag flag)
{
	AdminId id = GetPlayerSMAdminID(player);
	if (id == INVALID_ADMIN_ID) return false;
	
	FlagBits flags = adminsys->GetAdminFlags(id, Access_Effective);
	if ((flags & ADMFLAG_ROOT) != 0) return true;
	
	return ((flags & (FlagBits)(1 << flag)) != 0);
}


bool PlayerHasSMAdminFlags_All(CBasePlayer *player, FlagBits flag_mask)
{
	AdminId id = GetPlayerSMAdminID(player);
	if (id == INVALID_ADMIN_ID) return false;
	
	FlagBits flags = adminsys->GetAdminFlags(id, Access_Effective);
	if ((flags & ADMFLAG_ROOT) != 0) return true;
	
	return ((flags & flag_mask) == flag_mask);
}

bool PlayerHasSMAdminFlags_Any(CBasePlayer *player, FlagBits flag_mask)
{
	AdminId id = GetPlayerSMAdminID(player);
	if (id == INVALID_ADMIN_ID) return false;
	
	FlagBits flags = adminsys->GetAdminFlags(id, Access_Effective);
	if ((flags & ADMFLAG_ROOT) != 0) return true;
	
	return ((flags & flag_mask) != 0);
}
