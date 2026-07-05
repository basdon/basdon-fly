
/*jeanine:p:i:2;p:0;a:b;y:1.88;n:HandleRpcPlayerSpawned;*/
static
void HandleRpcPlayerSpawned(struct RakRPCHandlerArg *arg)
{
	struct RPCDATA_SetWorldBounds rpcdata;
	struct SampPlayer *player;
	struct in_addr in;
	struct vec3 pos;
	ushort playerid;

	playerid = rakServerVtable->GetIndexFromPlayerID(rakServer, arg->playerID);
	if (playerid > MAX_PLAYERS) {
		return;
	}
	player = playerpool->players[playerid];
	if (!player->hasSpawnInfo) {
		/*TODO remove this (we should set default spawn info when player connects & isAllowedToSpawn should prevent this anyways)*/
		return;
	}
	if (player->spawnInfo.skin > 319) {
		/*TODO remove this (this value should never been set to an invalid value, same as above)*/
		return;
	}
	if (!player->isAllowedToSpawn) {
		/*Getting here means the player is manually sending packets to circumvent initial class selection,*/
		/*because passing this condition cannot happen with an unmodified client.*/
		if (arg->playerID.binaryAddress == 0xFFFFFFFF && arg->playerID.port == 0xFFFF) {
			/*Samp::KickPlayer*/
			((void (*)(struct Samp*,ushort))0x80AE8C0)(samp, playerid);
		} else {
			in.s_addr = arg->playerID.binaryAddress;
			rakServerVtable->AddToBanList(rakServer, inet_ntoa(in), 600000);
		}
	}

	/*SAMP here streams out all players for this player. I can't see a reason why, so I'm skipping it.*/
	/*After all, things like SetPlayerPos don't do anything related to streaming, so why would this?*/

	/*SAMP here removes all attached player objects (why?), but we don't use those so skip it.*/

	player->onfootSyncData.pos = player->spawnInfo.pos.coords;
	player->pos = player->spawnInfo.pos.coords;
	player->vehicleid = 0;
	player->_pad2911 = 1;

	/*SAMP always resets this to fists, even if the player is supposed to have brass knuckles.*/
	player->weaponIdInSlot[0] = 0;
	player->currentWeaponId = 0;

	/*GameMode::Event_OnPlayerStateChange*/
	((void (*)(void*,int,int,int))0x80A5940)(samp->pGameMode, playerid, PLAYER_STATE_SPAWNED, player->currentState);
	player->currentState = PLAYER_STATE_SPAWNED;

	/*OnPlayerSpawn happens here*/

	/*SetWorldBounds doesn't (always) work in OnPlayerConnect, so this is a good location I suppose.*/
	if (need_adjust_world_bounds[playerid]) {
		need_adjust_world_bounds[playerid] = 0;
		*(int*)&rpcdata.x_max = *(int*)&rpcdata.y_max = WORLD_XY_MAX;
		*(int*)&rpcdata.x_min = *(int*)&rpcdata.y_min = WORLD_XY_MIN;
		SendRPCToPlayer(playerid, RPC_SetWorldBounds, &rpcdata, sizeof(rpcdata), 2);
	}

	if (!ISPLAYING(playerid)) {
		return;
	}

	spawned[playerid] = 1;

	pos = player->pos;

	/*I'd assume player doesn't send sync data while the game's dead animation plays,*/
	/*so it's unlikely that there are sync packets that may arrive out of order after respawning,*/
	/*but maybe that assumption is incorrect or it might still happen for players with large ping...*/
	/*In general I don't think it's a bad idea to set the player's expected location, since this is basically a teleport.*/
	player->hasExpectedLocationAfterTeleport = 1;
	player->expectedLocationAfterTeleport = pos;
	player->expectedLocationSetAtTickCount = samp_GetTime();

	kneeboard_update_all(playerid, &pos);
	maps_stream_for_player(playerid, pos.x, pos.y, OBJ_STREAM_MODE_CLOSEST_NOW);
	money_on_player_spawn(playerid);
	nametags_update_for_player(playerid);
	spawn_on_player_spawn(playerid);
	svp_update_mapicons(playerid, pos.x, pos.y);
	missions_on_player_spawn(playerid, pos);
	survey_on_player_spawn(playerid);
	zones_update(playerid, pos);
}
/*jeanine:p:i:3;p:2;a:b;y:1.88;n:samp_incoming_init;*/
static
void samp_incoming_init()
{
	TRACE;

	mem_mkjmp(0x80B0D90, &HandleRpcPlayerSpawned);

	/*Following functions are supposed to be replaced with our code.*/
	/*Ensure calls to them result in a crash instead of hard-to-trace corruption.*/
	mem_mkjmp(0x80CCFC0, Crash); /*SampPlayer::Spawn*/
	mem_mkjmp(0x80D1580, Crash); /*SampPlayerPool::StreamOutAllPlayersForPlayer*/
}
