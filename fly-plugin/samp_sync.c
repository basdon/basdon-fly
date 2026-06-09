/*TODO: clean these up somehow (and other occurences)*/
float *stream_distance = (float*) 0x8197DD8;
/*ms delay between checks for player/vehicle/etc streaming*/
int *stream_rate = (int*) 0x8197DD4;
#define PLAYER_MARKER_UPDATE_DELAY 2500 /*SAMP: 2500*/

static
int samp_GetTime()
{
	TRACE;
	return ((int (*)(void*))0x80AAF40)(samp_pNetGame);
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
/*jeanine:p:i:5;p:1;a:r;x:11.00;y:21.00;n:UpdatePlayerMarkersForPlayer;*/
static
/**
 * sending markers of players that don't exist for the client has no effect (of course because the
 * client also wouldn't know what color to use). sending markers for npc players does work.
 */
void UpdatePlayerMarkersForPlayer(struct SampPlayer *playa)
{
	TRACE;
	struct BitStream bs;
	struct PlayerID playerID;
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

	playa->markersLastStreamedAtTickCount = samp_GetTime();

	packetdata.packetId = 208;
	packetdata.numPlayers = 0;
	bs.ptrData = &packetdata;
	bs.numberOfBitsUsed = 8 + 32;
	bs.numberOfBitsAllocated = sizeof(packetdata) * 8;
	for (i = playerpool->highestUsedPlayerid; i >= 0; i--) {
		otherplayer = player[i];
		if (otherplayer && otherplayer != playa) {
			packetdata.numPlayers++;
			bitstream_write_bits(&bs, i, 16);
			if (
				otherplayer->currentState != PLAYER_STATE_NONE &&
				otherplayer->currentState != PLAYER_STATE_SPECTATING &&
				playerpool->virtualworld[i] == playerpool->virtualworld[playa->playerid]
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
	RakServer__GetPlayerIDFromIndex(&playerID, rakServer, playa->playerid);
	rakServer->vtable->SendBitStream(rakServer, &bs, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 1, playerID, 0);
}
/*jeanine:p:i:4;p:1;a:r;x:11.00;y:-44.00;n:StreamVehiclesForPlayer;*/
static
void StreamVehiclesForPlayer(struct SampPlayer *player)
{
	TRACE;
	struct SampVehicle *vehicle;
	int i;

	for (i = vehiclepool->highestUsedVehicleid; i > 0; i--) {
		vehicle = vehiclepool->vehicles[i];
		if (
			vehicle &&
			vehicle->_pad7A &&
			vehiclepool->virtualworld[i] == playerpool->virtualworld[player->playerid] &&
			vehicle->model != MODEL_FREIFLAT &&
			vehicle->model != MODEL_STREAKC &&
			IsWithinStreamDistance(&vehicle->pos, &player->pos)
		) {
			if (!player->vehicleStreamedIn[i]) {
				/*SampPlayer::StreamInVehicle*/
				((void (*)(struct SampPlayer*,int,int))0x80C9CA0)(player, i, /*ignore streaming distance*/ 1);
			}
		} else {
			if (player->vehicleStreamedIn[i]) {
				/*SampPlayer::StreamOutVehicle*/
				((void (*)(struct SampPlayer*,int))0x80C9BF0)(player, i);
			}
		}
	}
}
/*jeanine:p:i:6;p:3;a:r;x:2.00;n:GetOtherPlayerPosition;*/
static
struct vec3 *GetOtherPlayerPosition(struct SampPlayer *player)
{
	TRACE;
	struct SampVehicle *vehicle;

	/* not entirely sure why SAMP does this, I can't think of */
	/* a vehicle where the position of a passenger would be that */
	/* different from the vehicle's position.. */
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
int StreamPlayersForPlayer(struct SampPlayer *playa)
{
	TRACE;
	struct SampPlayer *otherplayer;
	int i, didStreamOutSomeone;

	didStreamOutSomeone = 0;
	for (i = playerpool->highestUsedPlayerid; i >= 0; i--) {
		otherplayer = player[i];
		if (
			otherplayer &&
			otherplayer->currentState != PLAYER_STATE_NONE &&
			otherplayer->currentState != PLAYER_STATE_SPECTATING &&
			playerpool->virtualworld[i] == playerpool->virtualworld[playa->playerid] &&
			IsWithinStreamDistance(GetOtherPlayerPosition(otherplayer), &playa->pos)/*jeanine:r:i:6;*/
		) {
			if (!playa->playerStreamedIn[i]) {
				/*SampPlayer::StreamInPlayer*/
				((void (*)(struct SampPlayer*,int,int))0x80CAF00)(playa, i, /*ignore streaming distance*/ 1);
			}
		} else {
			if (playa->playerStreamedIn[i]) {
				/*SampPlayer::StreamOutPlayer*/
				((void (*)(struct SampPlayer*,int))0x80CAFC0)(playa, i);
				didStreamOutSomeone = 1;
			}
		}
	}
	return didStreamOutSomeone;
}
/*jeanine:p:i:1;p:2;a:r;x:3.75;n:DoStreamingAfterPlayerLocationUpdate;*/
static
void DoStreamingAfterPlayerLocationUpdate(struct SampPlayer *player, float x, float y, float z, char processStreamingImmediately)
{
	TRACE;
	int tickCount, didStreamOutPlayer;

	player->pos.x = x;
	player->pos.y = y;
	player->pos.z = z;

	tickCount = samp_GetTime();

	if (player->hasExpectedLocationAfterTeleport) {
		if (
			IsWithinStreamDistance(&player->pos, &player->expectedLocationAfterTeleport) ||
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

	mem_mkjmp(0x80CBCB0, &DoStreamingAfterPlayerLocationUpdate);/*jeanine:r:i:1;*/
}
