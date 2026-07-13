/*TODO: clean these up somehow (and other occurences)*/
float *stream_distance = (float*) 0x8197DD8;
/*ms delay between checks for player/vehicle/etc streaming*/
int *stream_rate = (int*) 0x8197DD4;
#define PLAYER_MARKER_UPDATE_DELAY 2500 /*SAMP: 2500*/

/*Returns uptime of the SAMP server process, despite this function also being used*/
/*for the GetTickCount() PAWN function, of which the docs explicitely say it returns*/
/*the uptime of the actual server and not the SAMP server. Maybe that was SAMP 0.2 behavior.*/
static
int samp_GetTime()
{
	TRACE;
	return ((int (*)(void*))0x80AAF40)(samp);
}

static
int IsWithinStreamDistance(struct vec3 *a, struct vec3 *b)
{
	TRACE;
	float dx, dy, dz;

	dx = a->x - b->x;
	dy = a->y - b->y;
	dz = a->z - b->z;
	return dx * dx + dy * dy + dz * dz < *stream_distance * *stream_distance;
}
/*jeanine:p:i:5;p:1;a:r;x:11.00;y:25.00;n:UpdatePlayerMarkersForPlayer;*/
/**
 * sending markers of players that don't exist for the client has no effect (of course because the
 * client also wouldn't know what color to use). sending markers for npc players does work.
 */
static
void UpdatePlayerMarkersForPlayer(struct SampPlayer *player)
{
	TRACE;
	struct BitStream bs;
	struct SampPlayer *otherplayer;
#pragma pack(1)
	struct {
		unsigned char packetId;
		int numPlayers;
		/*then for each player: playerid(16) 1/0 (1) + if 1: xyz (16+16+16)*/
		char unstructured[MAX_PLAYERS * (2 + 1 + 2 + 2 + 2)];
	} packetdata;
#pragma pack()
	int i;

	/* SAMP also checks marker streaming mode: off/streamed/global. We always do global. */

	player->markersLastStreamedAtTickCount = samp_GetTime();

	packetdata.packetId = 208;
	packetdata.numPlayers = 0;
	bs.ptrData = &packetdata;
	bs.numberOfBitsUsed = 8 + 32;
	bs.numberOfBitsAllocated = sizeof(packetdata) * 8;
	for (i = playerpool->highestUsedPlayerid; i >= 0; i--) {
		otherplayer = sampPlayer[i];
		if (otherplayer && otherplayer != player) {
			packetdata.numPlayers++;
			bitstream_write_bits(&bs, i, 16);
			if (
				otherplayer->currentState != PLAYER_STATE_NONE &&
				otherplayer->currentState != PLAYER_STATE_SPECTATING &&
				playerpool->virtualworld[i] == playerpool->virtualworld[player->playerid]
				/*SAMP also checks distance if LimitPlayerMarkerRadius was used*/
			) {
				/*NOTE: max value is +-32k, so markers for players near CATA will be wrong :(*/
				bitstream_write_one(&bs);
				bitstream_write_bits(&bs, (short) otherplayer->pos.x, 16);
				bitstream_write_bits(&bs, (short) otherplayer->pos.y, 16);
				bitstream_write_bits(&bs, (short) otherplayer->pos.z, 16);
			} else {
				bitstream_write_zero(&bs);
			}
		}
	}
	SendSyncPacket(player->playerid, &bs);
}
/*jeanine:p:i:4;p:1;a:r;x:11.00;y:-61.00;n:StreamVehiclesForPlayer;*/
static
void StreamVehiclesForPlayer(struct SampPlayer *player)
{
	TRACE;
	struct SampVehicle *vehicle;
	int i, mission_vehicleid;

	mission_vehicleid = player_mission_vehicleid[player->playerid]; /*TODO put this on the player object*/

	for (i = vehiclepool->highestUsedVehicleid; i > 0; i--) {
		if (__builtin_expect(i == mission_vehicleid, 0)) {
			/*Keep player's mission's vehicleid streamed in, so its objective will keep showing*/
			/*on their radar. If player is far enough, vehicle physics should freeze so the vehicle*/
			/*should not be at risk of falling through the map or any other funky stuff.*/
			continue;
		}
		vehicle = vehiclepool->vehicles[i];
		if (
			vehicle &&
			vehicle->_pad7A &&
			vehiclepool->virtualworld[i] == playerpool->virtualworld[player->playerid] &&
			/*TODO: lets do something smart with train vehicle ids so we can get rid of this condition*/
			vehicle->model != MODEL_FREIFLAT &&
			vehicle->model != MODEL_STREAKC &&
			IsWithinStreamDistance(&vehicle->pos, &player->pos)
		) {
			if (
				!player->vehicleStreamedIn[i] &&
				/*TODO: this could (should?) do some prioritizing: stream*/
				/*out a random (or the furthest) unoccupied vehicle so we*/
				/*can stream in a vehicle that is occupied.*/
				player->numVehiclesStreamedIn < LIMIT_VEHICLES_STREAMED_IN
			) {
				player->vehicleStreamedIn[i] = 1;
				player->numVehiclesStreamedIn++;
				StreamVehicleIn(player, vehicle);
			}
		} else {
			if (player->vehicleStreamedIn[i]) {
				player->vehicleStreamedIn[i] = 0;
				player->numVehiclesStreamedIn--;
				StreamVehicleOut(player, vehicle);
			}
		}
	}
}
/*jeanine:p:i:6;p:3;a:r;x:8.00;n:GetOtherPlayerPosition;*/
static
struct vec3 *GetOtherPlayerPositionForStreamDistanceCheck(struct SampPlayer *player)
{
	TRACE;
	struct SampVehicle *vehicle;

	/* Not entirely sure why SAMP does this, I can't think of */
	/* a vehicle where the position of a passenger would be that */
	/* different from the vehicle's position.. */
	/* Also it doesn't do this adjustment for the player for whom */
	/* the streaming is currently being processed... curious...*/
	if (
		player->currentState == PLAYER_STATE_PASSENGER &&
		(unsigned int) player->vehicleid < MAX_VEHICLES &&
		(vehicle = vehiclepool->vehicles[player->vehicleid])
	) {
		return &vehicle->pos;
	}
	return &player->pos;
}
/*jeanine:p:i:3;p:1;a:r;x:11.00;y:-12.00;n:StreamPlayersForPlayer;*/
/**
 * returns true if at least one player was freshly streamed out
 */
static
int StreamPlayersForPlayer(struct SampPlayer *player)
{
	TRACE;
	struct SampPlayer *otherplayer;
	int i, didStreamOutSomeone;

	didStreamOutSomeone = 0;
	for (i = playerpool->highestUsedPlayerid; i >= 0; i--) {
		otherplayer = sampPlayer[i];
		if (
			otherplayer &&
			/*TODO: lets combine all non-streaming states at the start of the enum so this can be a single GT check*/
			otherplayer->currentState != PLAYER_STATE_NONE &&
			otherplayer->currentState != PLAYER_STATE_SPECTATING &&
			playerpool->virtualworld[i] == playerpool->virtualworld[player->playerid] &&
			IsWithinStreamDistance(GetOtherPlayerPositionForStreamDistanceCheck(otherplayer), &player->pos)/*jeanine:r:i:6;*/
		) {
			if (
				!player->playerStreamedIn[i] &&
				player->numPlayersStreamedIn < LIMIT_PLAYERS_STREAMED_IN
			) {
				player->playerStreamedIn[i] = 1;
				player->numPlayersStreamedIn++;
				StreamPlayerIn(player, otherplayer);
			}
		} else {
			if (player->playerStreamedIn[i]) {
				player->playerStreamedIn[i] = 0;
				player->numPlayersStreamedIn--;
				StreamPlayerOut(player, i);
				didStreamOutSomeone = 1;
			}
		}
	}
	return didStreamOutSomeone;
}
/*jeanine:p:i:1;p:2;a:r;x:3.75;n:DoStreamingAfterPlayerLocationUpdate;*/
/*Note: also triggered by PutPlayerInVehicle, PlayerSpectateVehicle, PlayerSpectatePlayer*/
static
void DoStreamingAfterPlayerLocationUpdate(struct SampPlayer *player, struct vec3 pos, char processStreamingImmediately)
{
	TRACE;
	int tickCount, didStreamOutPlayer;

	player->pos = pos;

	tickCount = samp_GetTime();

	if (player->hasExpectedLocationAfterTeleport) {
		if (
			IsWithinStreamDistance(&pos, &player->expectedLocationAfterTeleport) ||
			tickCount - player->expectedLocationSetAtTickCount >= 5000
		) {
			player->hasExpectedLocationAfterTeleport = 0;
			goto processStreamingNow;
		}
		return;
	}

	if (!processStreamingImmediately && tickCount - player->lastStreamingTick < *stream_rate) {
		goto checkCheckpoints;
	}

processStreamingNow:

	/* SAMP doesn't set lastStreamingTick when processStreamingImmediately is true or when hasExpectedLocationAfterTeleport */
	/* was just reset, but it seems more logical to do set it. */
	player->lastStreamingTick = tickCount;

	StreamVehiclesForPlayer(player);/*jeanine:r:i:4;*/
	didStreamOutPlayer = StreamPlayersForPlayer(player);/*jeanine:r:i:3;*/

	/* Marker data includes players that are streamed in. When a player is/gets streamed in, their marker just */
	/* gets suppressed in the client I guess. But when streaming out, the marker is not immediately reshown. */
	/* Try to fix that by forcing a marker update right after a player was just streamed out. */
	if (didStreamOutPlayer || tickCount - player->markersLastStreamedAtTickCount > PLAYER_MARKER_UPDATE_DELAY) {
		UpdatePlayerMarkersForPlayer(player);/*jeanine:r:i:5;*/
	}

	/* pickup streaming is also done here, but we don't use those yet */
	/* 3D text label streaming is also done here, but we don't stream them */
	/* actor streaming is also done here, but we don't use them yet */

checkCheckpoints:
	/* here SAMP also checks if a checkpoint or race checkpoint is set, and checks positions to trigger */
	/* OnPlayerEnter/Leave(Race)Checkpoint when appropriate. We don't use them like that so skip doing that. */
	;
}
/*jeanine:p:i:2;p:0;a:b;y:1.00;n:samp_sync_init;*/
static
void samp_sync_init()
{
	TRACE;

	mem_mkjmp(0x80CBCB0, &DoStreamingAfterPlayerLocationUpdate); /*SampPlayer::StorePositionProcessStreamingAndCheckpoints*//*jeanine:r:i:1;*/
	mem_mkjmp(0x80CC020, crash__this_codepath_should_be_unreachable); /*SampPlayer::SetExpectedLocationAfterTeleport*/
}
