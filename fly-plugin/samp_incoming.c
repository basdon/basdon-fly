
/*jeanine:p:i:2;p:3;a:r;x:30.00;y:-89.00;n:HandleRpcPlayerSpawned;*/
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
		SendRPC(playerid, RPC_SetWorldBounds, &rpcdata, sizeof(rpcdata) * 8);
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
/*jeanine:p:i:12;p:8;a:r;x:18.00;y:14.00;n:VehicleMarkOccupied;*/
static
void VehicleMarkOccupied(struct SampVehicle *vehicle, ushort playerid, float health)
{
	TRACE;

	vehicle->health = health;
	vehicle->hasBeenOccupiedSinceRespawn = 1;
	vehicle->lastOccupiedTickCount = samp_GetTime();

	/*SAMP does this check in VehicleApplySync, but that's weird since that*/
	/*function can also be called if the sync didn't update the health, so*/
	/*I'm doing it here instead.*/
	if (!health) {
		vehicle->isDead = 1;
		/*This is done unconditionally, so can this be overwritten if*/
		/*multiple people report this before the death event is sent.*/
		vehicle->playeridWhoReportedVehicleDeath = playerid;
	}
}
/*jeanine:p:i:10;p:8;a:r;x:18.00;y:-6.00;n:VehicleHasDriver;*/
static
int VehicleHasDriver(ushort vehicleid)
{
	TRACE;
	struct SampPlayer *otherplayer;
	int i;

	for (i = playerpool->highestUsedPlayerid; i >= 0; i--) {
		/*as per SAMP, this does not check if otherplayer has the vehicle streamed in*/
		if (
			(otherplayer= playerpool->players[i]) &&
			otherplayer->vehicleid == vehicleid &&
			otherplayer->currentState == PLAYER_STATE_DRIVER
		) {
			return 1;
		}
	}
	return 0;
}
/*jeanine:p:i:11;p:8;a:r;x:18.00;y:-55.00;n:IsPlayerAllowedToUnoccupiedUpdateVehicle;*/
/*Returns true if vehicle has no occupants and player is closest player to the vehicle.*/
static
ushort IsPlayerAllowedToUnoccupiedUpdateVehicle(ushort playerid, struct SampVehicle *vehicle)
{
	TRACE;
	float vx, vy, vz, dx, dy, dz, distanceSq;
	struct SampPlayer *otherplayer;
	float closestDistanceSq;
	int closestPlayerid;
	ushort vehicleid;
	int i;

	vehicleid = vehicle->vehicleid;
	vx = vehicle->pos.x;
	vy = vehicle->pos.y;
	vz = vehicle->pos.z;

	closestPlayerid = INVALID_PLAYER_ID;
	closestDistanceSq = 100000.0f * 100000.0f;
	for (i = playerpool->highestUsedPlayerid; i >= 0; i--) {
		otherplayer = playerpool->players[i];
		if (!otherplayer) {
			continue;
		}

		if (
			otherplayer->vehicleid == vehicleid &&
			/*as per SAMP, this does not check if otherplayer has the vehicle streamed in*/
			(otherplayer->currentState == PLAYER_STATE_DRIVER || otherplayer->currentState == PLAYER_STATE_PASSENGER)
		) {
			return 0; /*vehicle is occupied*/
		}

		if (
			otherplayer->currentState != PLAYER_STATE_SPECTATING &&
			otherplayer->vehicleStreamedIn[vehicleid]
		) {
			dx = vx - otherplayer->pos.x;
			dy = vy - otherplayer->pos.y;
			dz = vz - otherplayer->pos.z;
			distanceSq = dx * dx + dy * dy + dz * dz;
			if (distanceSq < closestDistanceSq) {
				closestDistanceSq = distanceSq;
				closestPlayerid = i;
			}
		}
	}

	return closestPlayerid == playerid;
}
/*jeanine:p:i:8;p:3;a:r;x:124.00;n:HandlePacketUnoccupiedSync;*/
static
void HandlePacketUnoccupiedSync(struct Samp *samp, struct RakPacket *packet)
{
	TRACE;
	struct SYNCDATA_UnoccupiedVehicle *syncdata;
	struct SampVehicle *vehicle;
	struct SampPlayer *player;
	ushort vehicleid;

	if (
		packet->playerid >= MAX_PLAYERS ||
		!(player = playerpool->players[packet->playerid]) ||
		packet->bitLength != (8 + sizeof(*syncdata) * 8) ||
		(vehicleid = (syncdata = (void*) (packet->data + 1))->vehicleid) > 1999 ||
		!player->vehicleStreamedIn[vehicleid] ||
		!(vehicle = vehiclepool->vehicles[vehicleid])
	) {
		return;
	}

	/*TODO: float checks*/

	if (syncdata->seat == 0) {
		/*Player is not in the vehicle, only allow these packets if*/
		/*vehicle is truly empty and player is the closest player to the vehicle*/
		if (!IsPlayerAllowedToUnoccupiedUpdateVehicle(packet->playerid, vehicle)) {/*jeanine:r:i:11;*/
			return;
		}
		/*OnOccupiedVehicleUpdate is called and checked here*/
	} else {
		/*Player is a passenger in the vehicle, only allow unoccupied*/
		/*update if vehicle has no driver.*/
		if (VehicleHasDriver(vehicleid)) {/*jeanine:r:i:10;*/
			return;
		}
		/*OnOccupiedVehicleUpdate is called and checked here*/
		VehicleMarkOccupied(vehicle, packet->playerid, syncdata->health);/*jeanine:r:i:12;*/
	}

#ifdef DEV
	sprintf(
		cbuf144,
		"UnoccupiedVehicleSync pid %d vid %d seat %d health %.1f "
			"vel %.1f %.1f %.1f angvel %.1f %.1f %.1f",
		packet->playerid,
		vehicleid,
		syncdata->seat,
		syncdata->health,
		syncdata->velocity.x,
		syncdata->velocity.y,
		syncdata->velocity.z,
		syncdata->angularVelocity.x,
		syncdata->angularVelocity.y,
		syncdata->angularVelocity.z
	);
	SendClientMessageToAll(-1, cbuf144);
#endif

	vehicle->lastSyncedByPlayerid = packet->playerid;
	vehicle->matrix.right = syncdata->matrixRight;
	vehicle->matrix.up = syncdata->matrixUp;
	vehicle->matrix.pos = syncdata->matrixPos;
	vehicle->pos = syncdata->matrixPos;
	/*TODO: lol why does this need to be 0 instead of INVALID_VEHICLE_ID*/
	/*TODO: why is this even here? does this mean trailers get detached?*/
	vehicle->trailerid = 0;

	player->unoccupiedVehicleSyncData = *syncdata;
	player->hasNewUnoccupiedVehicleSyncData = 1;
}
/*jeanine:p:i:3;p:0;a:b;y:1.88;n:samp_incoming_init;*/
static
void samp_incoming_init()
{
	TRACE;

	mem_mkjmp(0x80B0D90, &HandleRpcPlayerSpawned);/*jeanine:r:i:2;*/

	mem_mkjmp(0x80CCFC0, crash__this_codepath_should_be_unreachable); /*SampPlayer::Spawn*/
	mem_mkjmp(0x80D1580, crash__this_codepath_should_be_unreachable); /*SampPlayerPool::StreamOutAllPlayersForPlayer*/
	mem_redirectjmp(0x80AEE42, HandlePacketUnoccupiedSync); /*Samp::HandlePacketUnoccupiedSync*//*jeanine:r:i:8;*/
	mem_mkjmp(0x80ACD90, crash__this_codepath_should_be_unreachable); /*Samp::HandlePacketUnoccupiedSync*/
	mem_mkjmp(0x80C95C0, crash__this_codepath_should_be_unreachable); /*SampPlayer::StoreUnoccupiedVehicleSyncData*/
	mem_mkjmp(0x814B3B0, crash__this_codepath_should_be_unreachable); /*SampVehicle::HasDriver*/
	mem_mkjmp(0x814D290, crash__this_codepath_should_be_unreachable); /*SampVehicle::FindClosestPlayerId*/
}
