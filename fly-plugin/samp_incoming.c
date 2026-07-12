
/*jeanine:p:i:2;p:3;a:r;x:30.00;y:-89.00;n:HandleRpcPlayerSpawned;*/
static
void HandleRpcPlayerSpawned(struct RakRPCHandlerArg *arg)
{
	TRACE;
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
/*jeanine:p:i:20;p:3;a:r;x:30.00;y:-94.00;n:HandleRpcNpcJoin;*/
static
void HandleRpcNpcJoin(struct RakRPCHandlerArg *arg)
{
	TRACE;

	logprintf("Kicking client %s sending NpcJoin", playerid_tostring(&arg->playerID));
	rakServerVtable->Kick(rakServer, arg->playerID);
}
/*jeanine:p:i:22;p:13;a:r;x:18.00;y:-49.00;n:ReadValidateClientJoinPayload;*/
static
int ReadValidateClientJoinPayload(struct RakRPCHandlerArg *arg, ushort *outPlayerid, struct PlayerID *outPlayerID)
{
	TRACE;
	struct RPCDATA_ConnectionRejected rpcreject;
	struct RPCDATA_SendClientMessage rpcmsg;
	struct RakResult94 *result94;
	struct PlayerID playerID;
	struct BitStream bs;
	ushort playerid;
	int bytesLeft;
	uchar *pdata;
	uchar mod;
	uchar nicknameLength;
	uchar authKeyLen;
	uchar *authKey;
	uchar clientVersionStringLength;
	char *step, *kickmsg;

	kickmsg = NULL;
	step = "proc94";
	result94 = rakServerVtable->proc94(rakServer, arg->playerID);
	if (!result94 || result94->fieldC67 != 1) {
		kickmsg = "proc94 says no";
		goto rawkick;/*jeanine:r:i:14;*/
	}

	playerid = rakServerVtable->GetIndexFromPlayerID(rakServer, arg->playerID);
	*outPlayerid = playerid;

	step = "existingplayer";
	if (playerpool->players[playerid]) {
		kickmsg = "player slot already allocated";
		goto rawkick;/*jeanine:s:a:r;i:14;*/
	}

	step = "playerid";
	/*TODO: arg->playerID exists, what's the reason of this GetPlayerID(GetPlayerIndex(playerid)) dance?*/
	/*Maybe GetPlayerIndex can return an index that is above the slot count, and GetPlayerID will*/
	/*then return INVALID_PLAYER_ID?*/
	playerID = rakServerVtable->GetPlayerIDFromIndex(rakServer, playerid);
	*outPlayerID = playerID;
	rakPlayerID[playerid] = playerID;
	if (playerID.binaryAddress != arg->playerID.binaryAddress || playerID.port != arg->playerID.port) {
		/*Can't log in one message because playerid_tostring uses a static buffer.*/
		logprintf("ClientJoin: %d playerID a %s", playerid, playerid_tostring(&arg->playerID));
		logprintf("ClientJoin: %d playerID b %s", playerid, playerid_tostring(&playerID));
	}
	if (playerID.binaryAddress == 0xFFFFFFFF && playerID.port == 0xFFFF) {
		kickmsg = "bad address";
		goto rawkick;/*jeanine:s:a:r;i:14;*/
	}

	pdata = arg->rpcdata;

	step = "version";
	bytesLeft = arg->numBits >> 3;
	if (bytesLeft < 4) {
		goto rawkick;/*jeanine:s:a:r;i:14;*/
	}
	player_netgame_version[playerid] = *((int*) pdata);
	if (player_netgame_version[playerid] == 4062) {
		is_player_using_client_version_DL[playerid] = 1;
	} else if (player_netgame_version[playerid] == 4057) {
		is_player_using_client_version_DL[playerid] = 0;
	} else {
		kickmsg = "server supports 0.3.7 and 0.3.DL";
		rpcreject.reason = CONNECTION_REJECTED_REASON_BAD_VERSION;
		goto rejectkick;/*jeanine:s:a:r;i:15;*/
	}
	pdata += 4;
	bytesLeft -= 4;

	/*Version is now checked so from here on we can send RPCs instead of only rawkick.*/
	/*(Even though version check also sends ConnectionRejected already)*/

	step = "playerindex";
	if (playerid > MAX_PLAYERS) {
		kickmsg = "this condition should not be possible, you win a cookie";
		rpcreject.reason = CONNECTION_REJECTED_REASON_SERVER_FULL;
		goto rejectkick;/*jeanine:r:i:15;*/
	}

	step = "proc104";
	if (!rakServerVtable->proc104(rakServer, arg->playerID)) {
		kickmsg = "proc104 says no";
		rpcreject.reason = CONNECTION_REJECTED_REASON_SERVER_FULL;
		goto rejectkick;/*jeanine:s:a:r;i:15;*/
	}

	step = "mod";
	mod = pdata[0];
	if (mod != samp->mod) {
		rpcreject.reason = CONNECTION_REJECTED_REASON_BAD_MOD;
		goto rejectkick;/*jeanine:s:a:r;i:15;*/
	}
	pdata += 1;
	bytesLeft -= 1;

	step = "nickname";
	if (bytesLeft < 2) {
		rpcreject.reason = CONNECTION_REJECTED_REASON_BAD_NICKNAME;
		goto rejectkick;/*jeanine:s:a:r;i:15;*/
	}
	nicknameLength = pdata[0];
	if (nicknameLength < 2 || nicknameLength > 20) {
		rpcreject.reason = CONNECTION_REJECTED_REASON_BAD_NICKNAME;
		goto rejectkick;/*jeanine:s:a:r;i:15;*/
	}
	memcpy(playerpool->names[playerid], pdata + 1, nicknameLength);
	playerpool->names[playerid][nicknameLength] = 0;
	pdata += 1 + nicknameLength;
	bytesLeft -= 1 + nicknameLength;

	/*TODO: Here check if name is acceptable and not in use already.*/

	step = "challengeresponse";
	if (((*(int*) pdata) ^ player_netgame_version[playerid]) != *(int*) 0x81AA8A8) {
		kickmsg = "bad challenge response";
		rpcreject.reason = CONNECTION_REJECTED_REASON_BAD_VERSION;
		goto rejectkick;/*jeanine:s:a:r;i:15;*/
	}
	pdata += 4;
	bytesLeft -= 4;

	step = "authkey";
	if (bytesLeft < 2) {
		goto rawkick;
	}
	authKeyLen = pdata[0];
	if (!authKeyLen) {
		goto rawkick;
	}
	authKey = pdata + 1;
	pdata += 1 + authKeyLen;
	bytesLeft -= 1 + authKeyLen;

	/*fields from hereon are optional*/
	if (bytesLeft) {
		clientVersionStringLength = *pdata;
		pdata++;
		bytesLeft--;
		if (clientVersionStringLength > 24 || bytesLeft < clientVersionStringLength) {
			goto rawkick;
		}
		memcpy(playerpool->version[playerid], pdata, clientVersionStringLength);
		playerpool->version[playerid][clientVersionStringLength] = 0;

		if (bytesLeft == clientVersionStringLength + 4) {
			/*allegedly SAMP client sends another challenge response at the end,*/
			/*while unofficial clients don't*/
			/*TODO: verify and log more*/
		}
	} else {
		memcpy(playerpool->version[playerid], "unknown\0", 8);
	}

	/*This will set playerpool->gpci*/
	if (!ClientJoinReadAuthKey(authKey, authKeyLen, playerpool->gpci[playerid])) {
		kickmsg = "bad auth";
		goto rawkick;
	}

	return 1;
/*jeanine:p:i:14;p:22;a:r;x:7.00;y:12.00;n:ReadValidateClientJoinPayload;*/
rawkick:
	if (kickmsg) {
		rpcmsg.color = COL_SAMP_GREEN;
		rpcmsg.message_length = sprintf(rpcmsg.message, "%s", kickmsg);
		bs.ptrData = &rpcmsg;
		bs.numberOfBitsUsed = sizeof(4 + 4 + rpcmsg.message_length) * 8;
		rakServerVtable->RPC_90(rakServer, (void*) RPC_SendClientMessage, &bs, HIGH_PRIORITY, UNRELIABLE, 0, playerID, 0, 0);
	}
	logprintf("Invalid client connecting from %s (%s)", playerid_tostring(&playerID), step);
	rakServerVtable->Kick(rakServer, playerID);
	return 0;
/*jeanine:p:i:15;p:22;a:r;x:7.00;y:17.00;n:ReadValidateClientJoinPayload;*/
rejectkick:
	if (kickmsg) {
		rpcmsg.color = COL_SAMP_GREEN;
		rpcmsg.message_length = sprintf(rpcmsg.message, "%s", kickmsg);
		bs.ptrData = &rpcmsg;
		bs.numberOfBitsUsed = (4 + 4 + rpcmsg.message_length) * 8;
		rakServerVtable->RPC_90(rakServer, (void*) RPC_SendClientMessage, &bs, HIGH_PRIORITY, UNRELIABLE, 0, playerID, 0, 0);
	}
	bs.ptrData = &rpcreject;
	bs.numberOfBitsUsed = sizeof(rpcreject) * 8;
	rakServerVtable->RPC_90(rakServer, (void*) RPC_ConnectionRejected, &bs, HIGH_PRIORITY, UNRELIABLE, 0, playerID, 0, 0);
	rakServerVtable->Kick(rakServer, playerID);
	return 0;
/*jeanine:p:i:21;p:22;a:b;y:1.00;*/
}
/*jeanine:p:i:13;p:3;a:r;x:31.00;y:-227.00;n:HandleRpcClientJoin;*/
static
void HandleRpcClientJoin(struct RakRPCHandlerArg *arg)
{
	TRACE;
	union {
		struct RPCDATA_PlayerJoin base;
		/*above struct only has 1 byte for player name buffer, add 24 extra bytes*/
		char _ensuresize[sizeof(struct RPCDATA_PlayerJoin) + 24];
	} playerjoinrpc;
	struct PlayerID playerID;
	struct SampPlayer *player;
	struct BitStream bs;
	ushort playerid;
	int i, now;

	/*This will write playerpool->version, playerpool->gpci, playerpool->name*/
	if (!ReadValidateClientJoinPayload(arg, &playerid, &playerID)) {/*jeanine:r:i:22;*/
		return;
	}

	now = samp_GetTime();
	/*operator new(uint) TODO: does this zero?*/
	player = ((struct SampPlayer* (*)(uint)) 0x804AFFC)(0x2CDE);
	/*below here is code of SampPlayer::ctor (which is normally called from SampPlayerPool::AddPlayer)*/
	memset(player, 0, sizeof(*player));
	player->cameraObjectTarget = INVALID_OBJECT_ID;
	player->cameraVehicleTarget = INVALID_VEHICLE_ID;
	player->cameraPlayerTarget = INVALID_PLAYER_ID;
	player->cameraActorTarget = INVALID_ACTOR_ID;
	player->targetPlayerFromWeaponsUpdatePacket = INVALID_PLAYER_ID;
	player->targetActorFromWeaponsUpdatePacket = INVALID_ACTOR_ID;
	player->playerid = playerid;
	/*TODO: assign player's color here*/
	player->currentState = PLAYER_STATE_NONE; /*is already 0 by memset, but I want this explicitly*/
	player->fightingstyle = 4;
	player->lastStreamingTick = now;
	player->markersLastStreamedAtTickCount = now;
	player->_field2C8A = now;
	for (i = 0; i < 11; i++) {
		player->weaponSkill[i] = 999;
	}
	player->worldTime = 720.0f; /*hour*60+minute; 720 = 12:00*/
	player->_pad2911 = 1;
	/*skipping allocating textdrawpool/textlabelpool/pool2CDA*/
	/*below here is code of SampPlayerPool::AddPlayer*/
	playerjoinrpc.base.playerid = playerid;
	playerjoinrpc.base.color = player->color;
	playerjoinrpc.base.npc = 0;
	playerjoinrpc.base.namelen = sprintf(playerjoinrpc.base.name, "%s", playerpool->names[playerid]);
	bs.ptrData = &playerjoinrpc.base;
	bs.numberOfBitsUsed = (sizeof(struct RPCDATA_PlayerJoin) - 1 + playerjoinrpc.base.namelen) * 8;
	for (i = playerpool->highestUsedPlayerid; i >= 0; i--) {
		if (playerpool->players[i]) {
			SendRPC_bs(i, RPC_PlayerJoin, &bs);
		}
	}
	playerpool->players[playerid] = player;
	playerpool->created[playerid] = 1;
	playerpool->isNpc[playerid] = 0;
	playerpool->isAdmin[playerid] = 0;
	playerpool->playerScore[playerid] = 0;
	playerpool->playerMoney[playerid] = 0;
	playerpool->playerDrunkLevel[playerid] = 0;
	playerpool->rakResult94[playerid] = rakServerVtable->proc94(rakServer, playerID);

	logprintf(
		"[join] %s (%u) has joined the server (%s)",
		playerpool->names[playerid],
		playerid,
		playerid_tostring(&playerID)
	);

	SendGameTimeUpdate(playerid);

	/*OnPlayerConnect happens here*/
	i = playerid;
	B_OnPlayerConnect(NULL, &i - 3); /*param2[3] needs to be the playerid*/

	playerpool->numPlayers++;
	/*TODO: figure out what this is*/
	/*This MUST be called before adjusting playerpool->highestUsedPlayerid because it uses that field*/
	/*SampPlayerPool::Something*/
	((void (*)(struct SampPlayerPool*)) 0x80D0580)(playerpool);
	/*instead of SampPlayerPool::RecalculateHighestUsedPlayerid() loop*/
	if (playerid > playerpool->highestUsedPlayerid) {
		playerpool->highestUsedPlayerid = playerid;
	}
	/*end of SampPlayerPool::AddPlayer*/

	playerpool->rakResult94[playerid]->fieldC68 = 1;

	/*below here is code of Samp::ProcessPlayerJoin*/

	/*Samp::ProcessPlayerJoin has isRestarting checks but we don't restart, so skip that.*/
	/*Here SampObjectPool::InitForPlayer sends all created objects, we don't use this.*/
	/*Here all created Menus have their syncedToPlayer flag set to 0 for this playerid, we don't use that.*/

	/*Samp::SendGameInitRpc*/
	((void (*)(struct Samp*,ushort)) 0x80ADE60)(samp, playerid);

	playerjoinrpc.base.npc = 0;
	bs.ptrData = &playerjoinrpc.base;
	for (i = playerpool->highestUsedPlayerid; i >= 0; i--) {
		if (sampPlayer[i] && i != playerid) {
			playerjoinrpc.base.playerid = i;
			playerjoinrpc.base.color = sampPlayer[i]->color;
			playerjoinrpc.base.namelen = sprintf(playerjoinrpc.base.name, "%s", playerpool->names[i]);
			bs.numberOfBitsUsed = (sizeof(struct RPCDATA_PlayerJoin) - 1 + playerjoinrpc.base.namelen) * 8;
			SendRPC_bs(playerid, RPC_PlayerJoin, &bs);
		}
	}

	/*Here we can use all the functions that SAMP wiki says do not work reliably in OnPlayerConnect,*/
	/*because now GameInit has been sent so they should work reliably at this point.*/

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
	dev_print_unoccupiedvehiclesync(packet->playerid, vehicleid, syncdata);
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

	mem_mkjmp(0x80B0030, &HandleRpcNpcJoin);/*jeanine:r:i:20;*/
	mem_mkjmp(0x80B46D0, &HandleRpcClientJoin);/*jeanine:r:i:13;*/
	/*Our impl jumps to code in the original handler, to deal with all the auth_key stuff.*/
	/*These two jumps below are the exit paths of that auth_key code, to return to our handler.*/
	mem_mkjmp(0x80B56BE, &ClientJoinReadAuthKeyRejected);
	mem_mkjmp(0x80B54D9, &ClientJoinReadAuthKeyAccepted);
	mem_mkjmp(0x80D0660, crash__this_codepath_should_be_unreachable); /*SampPlayerPool::AddPlayer*/
	mem_mkjmp(0x80CDD40, crash__this_codepath_should_be_unreachable); /*SampPlayer::ctor*/
	mem_mkjmp(0x80AE3E0, crash__this_codepath_should_be_unreachable); /*Samp::ProcessPlayerJoin*/
	mem_mkjmp(0x80D0FD0, crash__this_codepath_should_be_unreachable); /*SampPlayerPool::SendExistingPlayersToConnectingPlayer*/

	mem_mkjmp(0x80B0D90, &HandleRpcPlayerSpawned);/*jeanine:r:i:2;*/
	mem_mkjmp(0x80CCFC0, crash__this_codepath_should_be_unreachable); /*SampPlayer::Spawn*/
	mem_mkjmp(0x80D1580, crash__this_codepath_should_be_unreachable); /*SampPlayerPool::StreamOutAllPlayersForPlayer*/

	mem_redirectjmp(0x80AEE42, HandlePacketUnoccupiedSync); /*Samp::HandlePacketUnoccupiedSync*//*jeanine:r:i:8;*/
	mem_mkjmp(0x80ACD90, crash__this_codepath_should_be_unreachable); /*Samp::HandlePacketUnoccupiedSync*/
	mem_mkjmp(0x80C95C0, crash__this_codepath_should_be_unreachable); /*SampPlayer::StoreUnoccupiedVehicleSyncData*/
	mem_mkjmp(0x814B3B0, crash__this_codepath_should_be_unreachable); /*SampVehicle::HasDriver*/
	mem_mkjmp(0x814D290, crash__this_codepath_should_be_unreachable); /*SampVehicle::FindClosestPlayerId*/
}
