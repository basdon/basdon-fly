#ifndef SAMP_NATIVES_IMPL

struct SampNetGame *samp_pNetGame;
struct SampPlayerPool *playerpool;
struct SampVehiclePool *vehiclepool;

static void* samp_pConsole;

static unsigned char vehicle_gear_state[MAX_VEHICLES];
static int vehicle_gear_change_time[MAX_VEHICLES];

/**
 * @param max_chars sizeof(dest)
 * @return amount of _bits_ written.
 */
static
int EncodeString(char *dest, char *source, int max_chars)
{
	TRACE;
	struct BitStream bs;

	bs.ptrData = dest;
	bs.numberOfBitsUsed = 0;
	bs.numberOfBitsAllocated = max_chars * 8;
#ifndef NO_CAST_IMM_FUNCPTR
	((void (*)(void*,char*,int,struct BitStream*,char))0x808EF80)(*(int**) 0x81AA744, source, max_chars, &bs, 0);
#endif
	return bs.numberOfBitsUsed;
}

/**additional_keys in the sync packets stores 3 keystates in 2 bits, need to unpack it to get a proper key bitmask value*/
static
int samp_combine_keys(int basekeys, int additional_keys)
{
	/*return basekeys | ((additional_keys == 3 ? 4 : additional_keys) << 16);*/

	register int thirdkey;

	thirdkey = (additional_keys << 1) & (additional_keys << 2) & 4;
	additional_keys = (additional_keys & ~((thirdkey >> 1) | (thirdkey >> 2))) | thirdkey;
	return basekeys | (additional_keys << 16);
}

/**
Result is the hexadec string representation, so out buf should be at least 65 size.
*/
static
void SAMP_SHA256(char *out_buf, char *input)
{
	TRACE;
#ifndef NO_CAST_IMM_FUNCPTR
	((void (*)(char*,char*,char*,int))0x80ED230)(input, /*salt*/"", out_buf, 65);
#endif
}

static
void SetConsoleVariableString(char *variable_name, char *value)
{
	TRACE;
#ifndef NO_CAST_IMM_FUNCPTR
	((void (*)(void*,char*,char*))0x80A0110)(samp_pConsole, variable_name, value);
#endif
}

static
char* GetConsoleVariableString(char *variable_name)
{
	TRACE;
#ifndef NO_CAST_IMM_FUNCPTR
	return ((char* (*)(void*,char*))0x80A0190)(samp_pConsole, variable_name);
#else
	return NULL;
#endif
}

static
void AddConsoleVariableString(char *variable_name, char *value)
{
	TRACE;
#ifndef NO_CAST_IMM_FUNCPTR
	/*see comment of AddServerRule*/
	((void (*)(void*,char*,int,char*,void*))0x80A08F0)(samp_pConsole, variable_name, 0, value, 0);
#endif
}

static
void AddServerRule(char *rule_name, char *value)
{
	TRACE;
#ifndef NO_CAST_IMM_FUNCPTR
	/*this invokes a function to add a "string console variable"
	 *using 4 as flags argument, which means it will be a server rule
	 *the last argument is a pointer to a procedure that will be called if the variable changes by console commands*/
	((void (*)(void*,char*,int,char*,void*))0x80A08F0)(samp_pConsole, rule_name, 4, value, 0);
#endif
}

static
void SendRconCommand(char *command)
{
	TRACE;
#ifndef NO_CAST_IMM_FUNCPTR
	((void (*)(void*,char*))0x809FBD0)(samp_pConsole, command);
#endif
}

static
int samp_GetTime()
{
	TRACE;
#ifndef NO_CAST_IMM_FUNCPTR
	return ((int (*)(void*))0x80AAF40)(samp_pNetGame);
#else
	return 0;
#endif
}

static
void SendRPC_8C(int playerid, int rpc, void *rpcdata, int size_bytes, enum PacketPriority priority, enum PacketReliability reliability, int orderingChannel)
{
	TRACE;
	struct BitStream bs;
	struct PlayerID playerID;

	bs.ptrData = rpcdata;
	bs.numberOfBitsUsed = size_bytes * 8;
/*TODO get rid of this ifndef*/
#ifndef NO_CAST_IMM_FUNCPTR
	RakServer__GetPlayerIDFromIndex(&playerID, rakServer, playerid);
	rakServer->vtable->RPC_8C(rakServer, (void*)rpc, &bs, priority, reliability, orderingChannel, playerID, /*broadcast*/ 0, /*shiftTimestamp*/ 0);
#endif
}

static
void SendRPCToPlayer(int playerid, int rpc, void *rpcdata, int size_bytes, int unk)
{
	TRACE;
	struct BitStream bs;

	bs.ptrData = rpcdata;
	bs.numberOfBitsUsed = size_bytes * 8;
	SAMP_SendRPCToPlayer(rpc, &bs, playerid, unk);
}

static
void ForceSendPlayerOnfootSyncNow(int playerid)
{
	TRACE;
	struct SampPlayer *cplayer;
	int actual_updateSyncType;
	int actual_state;

	cplayer = player[playerid];

	actual_updateSyncType = cplayer->updateSyncType;
	actual_state = cplayer->currentState;

	cplayer->updateSyncType = UPDATE_SYNC_TYPE_ONFOOT;
	cplayer->currentState = PLAYER_STATE_ONFOOT;
	/*Three times because it's an unreliable packet[citation needed] but it's important that players receive it.*/
#ifndef NO_CAST_IMM_FUNCPTR
	((void (*)(struct SampPlayer*)) 0x80C9DB0)(cplayer);
	((void (*)(struct SampPlayer*)) 0x80C9DB0)(cplayer);
	((void (*)(struct SampPlayer*)) 0x80C9DB0)(cplayer);
#endif

	cplayer->updateSyncType = actual_updateSyncType;
	cplayer->currentState = actual_state;
}

static
void convertSpawnInfoToSpawnInfo03DL(struct SpawnInfo *in, struct SpawnInfo03DL *out)
{
	TRACE;
	out->team = in->team;
	out->skin = in->skin;
	out->customSkin = 0;
	out->_spawnInfoPad5 = in->_pad5;
	out->pos = in->pos;
	memcpy(out->weapon, in->weapon, sizeof(in->weapon));
	memcpy(out->ammo, in->ammo, sizeof(in->ammo));
}

static
void SetGameModeText(char *gamemodetext)
{
	TRACE;
	SetConsoleVariableString("gamemodetext", gamemodetext);
}

static
void SetSpawnInfo(int playerid, struct SpawnInfo *spawnInfo)
{
	TRACE;
	struct RPCDATA_SetSpawnInfo03DL rpcdata03DL;
	struct RPCDATA_SetSpawnInfo037 rpcdata037;

	if (is_player_using_client_version_DL[playerid]) {
		convertSpawnInfoToSpawnInfo03DL(spawnInfo, &rpcdata03DL.spawnInfo);
		SendRPCToPlayer(playerid, RPC_SetSpawnInfo, &rpcdata03DL, sizeof(rpcdata03DL), 2);
	} else {
		rpcdata037.spawnInfo = *spawnInfo;
		SendRPCToPlayer(playerid, RPC_SetSpawnInfo, &rpcdata037, sizeof(rpcdata037), 2);
	}

	player[playerid]->spawnInfo = *spawnInfo;
	player[playerid]->hasSpawnInfo = 1; /*otherwise SAMP will ignore client Spawn packets and player will not be marked (not broadcasted) as spawned despited being spawned*/
}

static
__attribute__((unused))
void SetPlayerSkin(int playerid, int skin)
{
	TRACE;
	struct RPCDATA_SetPlayerSkin03DL rpcdata03DL;
	struct RPCDATA_SetPlayerSkin037 rpcdata037;
	struct SampPlayer *playa;
	int i;

	playa = player[playerid];
	if (!playa) {
		return;
	}

	/*this is what SAMP does*/
	if (!playa->hasSpawnInfo) {
		playa->spawnInfo.skin = skin;
		return;
	}

	rpcdata03DL.playerid = rpcdata037.playerid = playerid;
	rpcdata03DL.skin = rpcdata037.skin = skin;
	rpcdata03DL.customSkin = 0;

	for (i = playerpool->highestUsedPlayerid; i >= 0; i--) {
		if (i == playerid || (player[i] && player[i]->playerStreamedIn[playerid])) {
			if (is_player_using_client_version_DL[i]) {
				SendRPCToPlayer(i, RPC_SetPlayerSkin, &rpcdata03DL, sizeof(rpcdata03DL), 2);
			} else {
				SendRPCToPlayer(i, RPC_SetPlayerSkin, &rpcdata037, sizeof(rpcdata037), 2);
			}
		}
	}
}

static
void SetPlayerColor(int playerid, int color)
{
	TRACE;
	struct RPCDATA_SetPlayerColor rpcdata;
	int i;

	player[playerid]->color = color;

	rpcdata.playerid = playerid;
	rpcdata.color = color;

	for (i = playerpool->highestUsedPlayerid; i >= 0; i--) {
		if (player[i]) {
			SendRPC_8C(i, RPC_SetPlayerColor, &rpcdata, sizeof(rpcdata), HIGH_PRIORITY, RELIABLE_ORDERED, 2);
		}
	}
}

static
void SetPlayerMapIcon(int playerid, char icon_id, struct vec3 *pos, char icon, int color, char style)
{
	TRACE;
	struct RPCDATA_SetPlayerMapIcon rpcdata;
	struct BitStream bs;

	rpcdata.icon_id = icon_id;
	rpcdata.pos = *pos;
	rpcdata.icon = icon;
	rpcdata.color = color;
	rpcdata.style = style;
	bs.ptrData = &rpcdata;
	bs.numberOfBitsUsed = sizeof(rpcdata) * 8;
	SAMP_SendRPCToPlayer(RPC_SetPlayerMapIcon, &bs, playerid, 2);
}

static
__attribute__((unused))
void RemovePlayerMapIcon(int playerid, char icon_id)
{
	TRACE;
	struct RPCDATA_RemovePlayerMapIcon rpcdata;
	struct BitStream bs;

	rpcdata.icon_id = icon_id;
	bs.ptrData = &rpcdata;
	bs.numberOfBitsUsed = sizeof(rpcdata) * 8;
	SAMP_SendRPCToPlayer(RPC_RemovePlayerMapIcon, &bs, playerid, 2);
}

static
void Create3DTextLabel(
	int playerid, int label_id, int color, struct vec3 *pos, float draw_distance, char test_los,
	int attached_player_id, int attached_vehicle_id, char *encoded_text_data, int text_bitlength)
{
	TRACE;
	struct {
		struct RPCDATA_Create3DTextLabelBase rpcdata;
		char textdata[1]; /*arbitrary size*/
	} *data;
	struct BitStream bs;
	int text_bytelength;

	text_bytelength = (text_bitlength + 7) / 8;
	data = alloca(sizeof(data->rpcdata) + text_bytelength);
	data->rpcdata.label_id = label_id;
	data->rpcdata.color = color;
	data->rpcdata.pos = *pos;
	data->rpcdata.draw_distance = draw_distance;
	data->rpcdata.test_los = test_los;
	data->rpcdata.attached_player_id = attached_player_id;
	data->rpcdata.attached_vehicle_id = attached_vehicle_id;
	memcpy(data->textdata, encoded_text_data, text_bytelength);
	bs.ptrData = data;
	bs.numberOfBitsUsed = sizeof(data->rpcdata) * 8 + text_bitlength;
	SAMP_SendRPCToPlayer(RPC_Create3DTextLabel, &bs, playerid, 2);
}

static
void Delete3DTextLabel(int playerid, int label_id)
{
	TRACE;
	struct RPCDATA_Delete3DTextLabel rpcdata;
	struct BitStream bs;

	rpcdata.label_id = label_id;
	bs.ptrData = &rpcdata;
	bs.numberOfBitsUsed = sizeof(rpcdata) * 8;
	SAMP_SendRPCToPlayer(RPC_Delete3DTextLabel, &bs, playerid, 2);
}

/**
 * Does not apply until the vehicle is respawned for players that has the vehicle streamed in.
 */
static
__attribute__((unused))
void SetVehicleSiren(int vehicleid, char siren)
{
	TRACE;
	struct SampVehicle *vehicle;

	vehicle = samp_pNetGame->vehiclePool->vehicles[vehicleid];
	if (vehicle) {
		vehicle->use_siren = siren;
	}
}

static
__attribute__((unused))
int IsPlayerStreamedIn(int forplayerid, int playerid)
{
	TRACE;
	return playeronlineflag[playerid] && player[forplayerid]->playerStreamedIn[playerid];
}

static
__attribute__((unused)) /*unused in prod builds*/
void DisableRemoteVehicleCollisions(int playerid, char disable)
{
	TRACE;
	struct BitStream bs;

	disable <<= 7;
	bs.ptrData = &disable;
	bs.numberOfBitsUsed = 1;
	SAMP_SendRPCToPlayer(RPC_DisableRemoteVehicleCollisions, &bs, playerid, 2);
}

static
void SetPlayerRaceCheckpointNoDir(int playerid, int type, struct vec3 *pos, float radius)
{
	TRACE;
	struct RPCDATA_SetRaceCheckpoint rpcdata;
	struct BitStream bs;

	rpcdata.type = type;
	rpcdata.pos = *pos;
	rpcdata.radius = radius;
	bs.ptrData = &rpcdata;
	bs.numberOfBitsUsed = sizeof(rpcdata) * 8;
	SAMP_SendRPCToPlayer(RPC_SetRaceCheckpoint, &bs, playerid, 2);
}

static
void DisablePlayerRaceCheckpoint(int playerid)
{
	TRACE;
	struct BitStream bs;

	bs.numberOfBitsUsed = 0;
	SAMP_SendRPCToPlayer(RPC_DisableRaceCheckpoint, &bs, playerid, 2);
}

/**
 * Use natives_SpawnPlayer.
 */
static
void SpawnPlayer(int playerid)
{
	TRACE;
	struct RPCDATA_RequestSpawn rpcdata;
	struct BitStream bs;

	rpcdata.type = 2;
	bs.ptrData = &rpcdata;
	bs.numberOfBitsUsed = sizeof(rpcdata) * 8;
	SAMP_SendRPCToPlayer(RPC_RequestSpawn, &bs, playerid, 2);
}

/**
Only use this if the distance from the player's current position is very small.
Otherwise, use natives_SetPlayerPos.
*/
static
void SetPlayerPosRaw(int playerid, struct vec3 *pos)
{
	TRACE;
	struct BitStream bs;

	bs.ptrData = pos;
	bs.numberOfBitsUsed = sizeof(*pos) * 8;
	SAMP_SendRPCToPlayer(RPC_SetPlayerPos, &bs, playerid, 2);
}
EXPECT_SIZE(struct RPCDATA_SetPlayerPos, sizeof(struct vec3));

static
void SetPlayerFacingAngle(int playerid, float angle)
{
	TRACE;
	struct BitStream bs;

	bs.ptrData = &angle;
	bs.numberOfBitsUsed = sizeof(angle) * 8;
	SAMP_SendRPCToPlayer(RPC_SetPlayerFacingAngle, &bs, playerid, 2);
}
EXPECT_SIZE(struct RPCDATA_SetPlayerFacingAngle, sizeof(float));

static
void SetPlayerHealth(int playerid, float health)
{
	TRACE;
	SendRPC_8C(playerid, RPC_SetHealth, &health, sizeof(float), HIGH_PRIORITY, RELIABLE_ORDERED, 2);
}
STATIC_ASSERT(sizeof(struct RPCDATA_SetHealth) == sizeof(float));

/**use money_give instead*/
static
void GivePlayerMoneyRaw(int playerid, int amount)
{
	TRACE;
	if (playerpool->created[playerid]) {
		playerpool->playerMoney[playerid] += amount;
		SendRPC_8C(playerid, RPC_MoneyGive, &amount, sizeof(int), HIGH_PRIORITY, RELIABLE_ORDERED, 2);
	}
}
STATIC_ASSERT(sizeof(struct RPCDATA_MoneyGive) == sizeof(int));

/**use money_set instead*/
static
void SetPlayerMoneyRaw(int playerid, int amount)
{
	TRACE;
	if (playerpool->created[playerid]) {
		playerpool->playerMoney[playerid] = amount;
		SendRPC_8C(playerid, RPC_MoneyReset, NULL, 0, HIGH_PRIORITY, RELIABLE_ORDERED, 2);
		SendRPC_8C(playerid, RPC_MoneyGive, &amount, sizeof(int), HIGH_PRIORITY, RELIABLE_ORDERED, 2);
	}
}
STATIC_ASSERT(sizeof(struct RPCDATA_MoneyGive) == sizeof(int));

static
void SetPlayerScore(int playerid, int score)
{
	TRACE;
	/*this does not send RPCs, scoreboard stuff is just updated regularly*/
	playerpool->playerScore[playerid] = score;
}

static
int GetPlayerScore(int playerid)
{
	TRACE;
	return playerpool->playerScore[playerid];
}

static
void SetPlayerTime(int playerid, char hour, char minute)
{
	TRACE;
	struct RPCDATA_SetTime rpcdata;
	struct SampPlayer *playa;

	playa = player[playerid];
	if (playa) {
		playa->worldTime = hour * 60.0f + minute; /*TODO: can probably get rid of this, samp doesn't need to interfere with our timecyc*/
		rpcdata.hour = hour;
		rpcdata.minute = minute;
		SendRPC_8C(playerid, RPC_SetTime, &rpcdata, sizeof(rpcdata), HIGH_PRIORITY, RELIABLE_ORDERED, 2);
	}
}

static
void SetPlayerWeather(int playerid, char weather)
{
	TRACE;
	SendRPC_8C(playerid, RPC_SetWeather, &weather, sizeof(char), HIGH_PRIORITY, RELIABLE_ORDERED, 2);
}
EXPECT_SIZE(struct RPCDATA_SetWeather, sizeof(char));

static
void TogglePlayerClock(int playerid, char enabled)
{
	TRACE;
	struct SampPlayer *playa;

	playa = player[playerid];
	if (playa) {
		playa->isClockEnabled = enabled; /*TODO: should get rid of this, see SetPlayerTime*/
		SendRPC_8C(playerid, RPC_ToggleClock, &enabled, sizeof(char), HIGH_PRIORITY, RELIABLE_ORDERED, 2);
	}
}
EXPECT_SIZE(struct RPCDATA_ToggleClock, sizeof(char));

static
void TogglePlayerSpectating(int playerid, int enabled)
{
	TRACE;
	struct SampPlayer *playa;

	playa = player[playerid];
	if (playa) {
		playa->spectatingTargetKind = SPECTATING_TARGET_UNSET;
		playa->spectatingTargetId = -1;
		SendRPC_8C(playerid, RPC_ToggleSpectating, &enabled, sizeof(int), HIGH_PRIORITY, RELIABLE_ORDERED, 2);
		/*this doesn't actually affect player state somehow, only with PlayerSpectatePlayer/PlayerSpectateVehicle
		will player's state be changed to PLAYER_STATE_SPECTATING*/
	}
}
EXPECT_SIZE(struct RPCDATA_ToggleSpectating, sizeof(int));

static
void SetPlayerCameraPos(int playerid, struct vec3 *pos)
{
	TRACE;
	struct BitStream bs;

	bs.ptrData = pos;
	bs.numberOfBitsUsed = sizeof(*pos) * 8;
	SAMP_SendRPCToPlayer(RPC_SetCameraPos, &bs, playerid, 2);
}
EXPECT_SIZE(struct RPCDATA_SetCameraPos, sizeof(struct vec3));

/**
 * @param switchstyle see CAMERA_ definitions
 */
static
void SetPlayerCameraLookAt(int playerid, struct vec3 *at, char switchstyle)
{
	TRACE;
	struct RPCDATA_SetCameraLookAt rpcdata;
	struct BitStream bs;

	rpcdata.at = *at;
	rpcdata.switchstyle = switchstyle;
	bs.ptrData = &rpcdata;
	bs.numberOfBitsUsed = sizeof(rpcdata) * 8;
	SAMP_SendRPCToPlayer(RPC_SetCameraLookAt, &bs, playerid, 2);
}

static
void SetCameraBehindPlayer(int playerid)
{
	TRACE;
	struct BitStream bs;

	bs.numberOfBitsUsed = 0;
	SAMP_SendRPCToPlayer(RPC_SetCameraBehind, &bs, playerid, 2);
}

static
void InterpolateCamera(int playerid, char position, struct vec3 *from, struct vec3 *to, int timeMs)
{
	TRACE;
	volatile struct RPCDATA_InterpolateCamera rpcdata;
	unsigned char *aligned, shifted[30];
	struct BitStream bs;
	int i;

	rpcdata.from = *from;
	rpcdata.to = *to;
	rpcdata.timeMs = timeMs;
	rpcdata.switchstyle = 0; /*this has no effect, see definition of the struct*/

	aligned = 1 + (unsigned char*) &rpcdata;
	shifted[0] = position ? 0x80 : 0;
	for (i = 0; i < 29; i++) {
		shifted[i] |= (*aligned & 0xFE) >> 1;
		shifted[i + 1] = (*aligned & 1) << 7;
		aligned++;
	}

	bs.ptrData = shifted;
	bs.numberOfBitsUsed = sizeof(rpcdata) * 8 - 7;
	SAMP_SendRPCToPlayer(RPC_InterpolateCamera, &bs, playerid, 2);
}
EXPECT_SIZE(struct RPCDATA_InterpolateCamera, 30);

static
__attribute((unused))
void InterpolateCameraPos(int playerid, struct vec3 *from, struct vec3 *to, int timeMs)
{
	TRACE;
	InterpolateCamera(playerid, 1, from, to, timeMs);
}

static
__attribute((unused))
void InterpolateCameraLookAt(int playerid, struct vec3 *from, struct vec3 *to, int timeMs)
{
	TRACE;
	InterpolateCamera(playerid, 0, from, to, timeMs);
}

/**
 * @param actionid see SPECIAL_ACTION_ definitions
 */
static
void SetPlayerSpecialAction(int playerid, char actionid)
{
	TRACE;
	struct BitStream bs;

	bs.ptrData = &actionid;
	bs.numberOfBitsUsed = sizeof(actionid) * 8;
	SAMP_SendRPCToPlayer(RPC_SetSpecialAction, &bs, playerid, 2);
}
EXPECT_SIZE(struct RPCDATA_SetSpecialAction, sizeof(char));

static
void ForceClassSelection(int playerid)
{
	TRACE;
	SendRPC_8C(playerid, RPC_ForceClassSelection, NULL, 0, HIGH_PRIORITY, RELIABLE, 2);
}

static
__attribute__((unused)) /*unused in prod builds*/
void GivePlayerWeapon(int playerid, int weaponid, int ammo)
{
	TRACE;
	struct RPCDATA_GiveWeapon rpcdata;

	rpcdata.weaponid = weaponid;
	rpcdata.ammo = ammo;
	SendRPC_8C(playerid, RPC_GiveWeapon, &rpcdata, sizeof(rpcdata), HIGH_PRIORITY, RELIABLE, 2);
}

/**
When respawning, the player will always regain control.

Player's vehicle will not get damage when a player is not controllable.
IT EVEN GETS REPAIRED. Vehicle HP will be set to 300 when making player
uncontrollable and the vehicle hp is < 250 (maybe even when it's < 300?)
*/
static
void TogglePlayerControllable(int playerid, char controllable)
{
	TRACE;
	struct BitStream bs;

	bs.ptrData = &controllable;
	bs.numberOfBitsUsed = 8;
	SAMP_SendRPCToPlayer(RPC_TogglePlayerControllable, &bs, playerid, 2);
}

static
void SendClientMessageToBatch(short *playerids, int numplayerids, int color, char *message)
{
	TRACE;
	/*Using 4 packets should handle messages with a length up to 144*4.*/
	struct RPCDATA_SendClientMessage rpcdata[4];
	struct BitStream bs;
	int i, j, packetidx;
	int num_packets;
	char *msg_start;
	int msglen;
	int cut_pos, min_cut_pos;
	int pos_inc;
	char colorpart;
	int newcolor;

	msglen = strlen(message);

	/*(Probably unneeded) optimization to skip cutting and looping stuff.*/
	if (msglen < 145) {
		rpcdata[0].color = color;
		rpcdata[0].message_length = msglen;
		memcpy(rpcdata[0].message, message, msglen);
		bs.ptrData = &rpcdata[0];
		bs.numberOfBitsUsed = 32 + 32 + msglen * 8;
		for (i = 0; i < numplayerids; i++) {
			SAMP_SendRPCToPlayer(RPC_SendClientMessage, &bs, playerids[i], 3);
		}
		return;
	}

	num_packets = 0;
	msg_start = message;

	/*See /testmsgsplit command.*/
	while (msglen > 0 && num_packets < 4) {
		if (msglen > 144) {
			/*Try to split on whitespace or color boundaries.*/
			for (cut_pos = 144, min_cut_pos = 144 - 15; cut_pos > min_cut_pos; cut_pos--) {
				if (msg_start[cut_pos] == ' ') {
					pos_inc = cut_pos + 1;
					goto have_cut_pos;
				} else if (cut_pos <= msglen - 7 && msg_start[cut_pos] == '{' && msg_start[cut_pos + 7] == '}') {
					pos_inc = cut_pos;
					goto have_cut_pos;
				}
			}
			cut_pos = pos_inc = 144;
		} else {
			cut_pos = pos_inc = msglen;
		}
have_cut_pos:
		rpcdata[num_packets].color = color;
		rpcdata[num_packets].message_length = cut_pos;
		memcpy(rpcdata[num_packets].message, msg_start, cut_pos);
		num_packets++;
		/*Find the last embedded color to use for the next packet.*/
		for (i = cut_pos - 1; i >= 7; i--) {
			if (msg_start[i] == '}' && msg_start[i - 7] == '{') {
				newcolor = color & 0xFF;
				for (j = 0; j < 6; j++) {
					colorpart = msg_start[i - 6 + j];
					if ('0' <= colorpart && colorpart <= '9') {
						newcolor |= (colorpart - '0') << (28 - j * 4);
					} else if ('a' <= colorpart && colorpart <= 'f') {
						newcolor |= (colorpart - 'a' + 10) << (28 - j * 4);
					} else if ('A' <= colorpart && colorpart <= 'F') {
						newcolor |= (colorpart - 'A' + 10) << (28 - j * 4);
					} else {
						goto not_a_color;
					}
				}
				color = newcolor;
				break;
			}
not_a_color:
			;
		}
		msglen -= pos_inc;
		msg_start += pos_inc;
	}

	for (i = 0; i < numplayerids; i++) {
		for (packetidx = 0; packetidx < num_packets; packetidx++) {
			bs.ptrData = &rpcdata[packetidx];
			bs.numberOfBitsUsed = 32 + 32 + rpcdata[packetidx].message_length * 8;
			SAMP_SendRPCToPlayer(RPC_SendClientMessage, &bs, playerids[i], 3);
		}
	}
}

static
void SendClientMessageToAll(int color, char *message)
{
	TRACE;
	SendClientMessageToBatch(players, playercount, color, message);
}

static
void SendClientMessage(short playerid, int color, char *message)
{
	TRACE;
	SendClientMessageToBatch(&playerid, 1, color, message);
}

static
void PlayerPlaySound(int playerid, int soundid)
{
	TRACE;
	struct RPCDATA_PlaySound rpcdata;
	struct BitStream bs;

	rpcdata.soundid = soundid;
	*(int*)&rpcdata.pos.x = 0;
	*(int*)&rpcdata.pos.y = 0;
	*(int*)&rpcdata.pos.z = 0;
	bs.ptrData = &rpcdata;
	bs.numberOfBitsUsed = sizeof(rpcdata) * 8;
	SAMP_SendRPCToPlayer(RPC_PlaySound, &bs, playerid, 2);
}

static
int IsVehicleStreamedIn(int vehicleid, int forplayerid)
{
	TRACE;
	return player[forplayerid]->vehicleStreamedIn[vehicleid];
}

static
void GetPlayerPos(int playerid, struct vec3 *pos)
{
	TRACE;
	*pos = player[playerid]->pos;
}

static
__attribute((unused))
float GetPlayerFacingAngle(int playerid)
{
	TRACE;
	return player[playerid]->facingAngle;
}

static
__attribute((unused))
char GetPlayerCameraMode(int playerid)
{
	TRACE;
	if (player[playerid]) {
		return player[playerid]->aimSyncData.cameraMode;
	}
	return -1;
}

static
__attribute((unused))
int GetPlayerWeaponState(int playerid)
{
	TRACE;
	struct SampPlayer *playa = player[playerid];

	if (playa && playa->currentState == PLAYER_STATE_ONFOOT) {
		return playa->aimSyncData.weaponState;
	}
	/*samp returns 0 (WEAPONSTATE_NO_BULLETS) if the player is not connected, but that doesn't make sense*/
	return WEAPONSTATE_UNKNOWN;
}

static
void GetPlayerPosRot(int playerid, struct vec4 *pos)
{
	TRACE;
	pos->coords = player[playerid]->pos;
	pos->r = player[playerid]->facingAngle;
}

static
short GetPlayerVehicleSeat(int playerid)
{
	TRACE;
	return player[playerid]->vehicleseat;
}

/**
@return 0 when not in vehicle
*/
static
short GetPlayerVehicleID(int playerid)
{
	TRACE;
	return player[playerid]->vehicleid;
}

static
struct SampVehicle *GetPlayerVehicle(int playerid)
{
	TRACE;
	/*Player's vehicleid will be 0 when not in a vehicle.*/
	/*Non allocated vehicleids will always have a nullptr in SampVehiclePool::vehicles.*/
	return vehiclepool->vehicles[player[playerid]->vehicleid];
}

static
char GetPlayerState(int playerid)
{
	TRACE;
	return player[playerid]->currentState;
}

static
short GetVehicleModel(int vehicleid)
{
	TRACE;
	struct SampVehicle *vehicle;

	vehicle = samp_pNetGame->vehiclePool->vehicles[vehicleid];
	if (vehicle) {
		return vehicle->model;
	}
	return 0;
}

static
int GetPlayerInVehicleSeat(int vehicleid, int seat)
{
	TRACE;
	int n, playerid;

	for (n = playercount; n; ) {
		playerid = players[--n];
		if (player[playerid]->vehicleid == vehicleid && player[playerid]->vehicleseat == seat) {
			return playerid;
		}
	}
	return INVALID_PLAYER_ID;
}

static
int GetVehicleDriver(int vehicleid)
{
	TRACE;
	int n, playerid;

	for (n = playercount; n; ) {
		playerid = players[--n];
		if (player[playerid]->vehicleid == vehicleid && player[playerid]->vehicleseat == 0) {
			return playerid;
		}
	}
	return INVALID_PLAYER_ID;
}

/**
Use GetVehicleHealth, defined in anticheat.c
*/
static
__attribute__((unused))
float GetVehicleHealthRaw(int vehicleid)
{
	TRACE;
	struct SampVehicle *vehicle;

	vehicle = samp_pNetGame->vehiclePool->vehicles[vehicleid];
	if (vehicle) {
		return vehicle->health;
	}
	return 0;
}

#ifndef NO_CAST_IMM_FUNCPTR
#define SAMP_SetVehicleHealth(VEHICLE,HP) ((void (*)(struct SampVehicle*,float))0x814B860)(VEHICLE,HP);
#else
#define SAMP_SetVehicleHealth(VEHICLE,HP)
#endif

static
void SetVehicleHealth(int vehicleid, float hp)
{
	TRACE;
	struct SampVehicle *vehicle;

	vehicle = samp_pNetGame->vehiclePool->vehicles[vehicleid];
	if (vehicle) {
		SAMP_SetVehicleHealth(vehicle, hp);
	}
}

static
void SyncVehicleDamageStatus(struct SampVehicle *vehicle)
{
	TRACE;
	struct INOUTRPCDATA_UpdateVehicleDamageStatus rpcdata;
	struct PlayerID playerID;
	struct BitStream bs;
	int i;

	rpcdata.vehicleid = vehicle->vehicleid;
	rpcdata.panels = vehicle->damageStatus.panels.raw;
	rpcdata.doors = vehicle->damageStatus.doors.raw;
	rpcdata.broken_lights = vehicle->damageStatus.broken_lights.raw;
	rpcdata.popped_tires = vehicle->damageStatus.popped_tires.raw;

	bs.ptrData = &rpcdata;
	bs.numberOfBitsUsed = sizeof(rpcdata) * 8;

	for (i = playerpool->highestUsedPlayerid; i >= 0; i--) {
		if (player[i] && player[i]->vehicleStreamedIn[rpcdata.vehicleid]) {
#ifndef NO_CAST_IMM_FUNCPTR
			RakServer__GetPlayerIDFromIndex(&playerID, rakServer, i);
			rakServer->vtable->RPC_8C(rakServer, (void*) RPC_UpdateVehicleDamageStatus, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, /*orderingChannel*/ 2, playerID, /*broadcast*/ 0, /*shiftTimestamp*/ 0);
#endif
		}
	}
}

/*
TODO: make a GetVehicleParamsExForPlayer? Where objective is set in case they're on a mission but not in their vehicle.
*/
static
void GetVehicleParamsEx(int vehicleid, struct SampVehicleParams *params)
{
	TRACE;
	struct SampVehicle *vehicle;

	vehicle = samp_pNetGame->vehiclePool->vehicles[vehicleid];
	if (vehicle) {
		*params = vehicle->params;
	}
}

/*XXX: untested!*/
static
__attribute__((unused))
void SetVehicleParamsEx(int vehicleid, struct SampVehicleParams *params)
{
	TRACE;
	struct SampVehicle *vehicle;

	vehicle = samp_pNetGame->vehiclePool->vehicles[vehicleid];
	if (vehicle) {
#ifndef NO_CAST_IMM_FUNCPTR
		((int (*)(struct SampVehicle*,struct SampVehicleParams*))0x814C7A0)(vehicle, params);
#endif
	}
}

static
void SetVehicleParamsExForPlayer(int vehicleid, int playerid, struct SampVehicleParams *params)
{
	TRACE;
	struct RPCDATA_SetVehicleParamsEx rpcdata;
	struct BitStream bs;

	rpcdata.vehicleid = (short) vehicleid;
	rpcdata.params = *params;
	bs.ptrData = &rpcdata;
	bs.numberOfBitsUsed = sizeof(rpcdata) * 8;
	SAMP_SendRPCToPlayer(RPC_SetVehicleParamsEx, &bs, playerid, 2);
}

/*
XXX: this resets any player-specific doors_locked state back to the global doors_locked state.
*/
static
void SetVehicleObjectiveForPlayer(int vehicleid, int playerid, char objective)
{
	TRACE;
	struct RPCDATA_SetVehicleParams rpcdata;
	struct BitStream bs;
	struct SampVehicleParams params;

	GetVehicleParamsEx(vehicleid, &params);
	/*SetVehicleParams does not work to unset the object, SetVehicleParamsEx must be used.*/
	if (!objective) {
		params.objective = 0; /*TODO remove once we have GetVehicleParamsExForPlayer*/
		SetVehicleParamsExForPlayer(vehicleid, playerid, &params);
	} else {
		rpcdata.vehicleid = (short) vehicleid;
		rpcdata.objective = objective;
		rpcdata.doors_locked = params.doors_locked;
		if (rpcdata.doors_locked == -1) {
			/*Global params can have -1 for doors_locked (unset),
			but using -1 for non-global (non-SetVehicleParamsEx) will lock the doors.*/
			rpcdata.doors_locked = 0;
		}
		bs.ptrData = &rpcdata;
		bs.numberOfBitsUsed = sizeof(rpcdata) * 8;
		SAMP_SendRPCToPlayer(RPC_SetVehicleParams, &bs, playerid, 2);
	}
}

/*
Crashes when vehicle is not created.
*/
static
char GetVehicleEngineState(int vehicleid)
{
	TRACE;
	return samp_pNetGame->vehiclePool->vehicles[vehicleid]->params.engine;
}

/*
Crashes when vehicle is not created.

XXX: this resets any player-specific state back to the global state.
TODO: it should at least persist player specific objective state by checking the missions.
*/
static
void SetVehicleEngineState(int vehicleid, char engine)
{
	TRACE;
	struct RPCDATA_SetVehicleParamsEx rpcdata;
	struct BitStream bs;
	struct SampVehicleParams *params;
	int playerid, n;

	params = &samp_pNetGame->vehiclePool->vehicles[vehicleid]->params;
	if (params->engine == engine) {
		return;
	}

	params->engine = engine;

	rpcdata.vehicleid = (short) vehicleid;
	rpcdata.params = *params;
	bs.ptrData = &rpcdata;
	bs.numberOfBitsUsed = sizeof(rpcdata) * 8;

	for (n = playercount; n; ) {
		playerid = players[--n];
		if (IsVehicleStreamedIn(vehicleid, playerid)) {
			SAMP_SendRPCToPlayer(RPC_SetVehicleParamsEx, &bs, playerid, 2);
		}
	}
}

static
void RepairVehicleVisualDamage(struct SampVehicle *vehicle)
{
	TRACE;
	vehicle->damageStatus.doors.raw = 0;
	vehicle->damageStatus.panels.raw = 0;
	vehicle->damageStatus.broken_lights.raw = 0;
	vehicle->damageStatus.popped_tires.raw = 0;
	SyncVehicleDamageStatus(vehicle);

	/*thanks to Nati_Mage for telling me to put the driver door to damaged to fix the 'ghost door' problem with some planes*/
	if (vehicleflags[vehicle->model - VEHICLE_MODEL_MIN] & NEEDS_GHOST_DOOR_FIX) {
		/*
		    The damage update above results in clients calling the game function CAutomobile::Fix on the vehicle.
		    (Having all doors+panels+lights on 0 results in CAutomobile::Fix, and otherwise CAutomobile::mCC.)
		    Since CPlane::Fix is supposed to be used for planes instead (but it isn't), this creates a side-effect for
		    some plane models: the door will not have an open/close animation anymore and the player will just clip through.
		    This can be fixed by applying any kind of damage again (no clue why). We can go for damaging the pilot's door,
		    since plane doors don't have damage models anyways, but just using the light damage status seems simpler.
		    Note that we cannot immediately do it this way (without doing a damage update with doors+panels+lights all 0)
		    because while it will functionally fix the handling, it may not completely fix visual damage as ailerons might
		    still hang loosely if we do not let it call CAutomobile::Fix.
		*/
		vehicle->damageStatus.broken_lights.raw = 1;

		SyncVehicleDamageStatus(vehicle);

		/*
		    Totally unneeded but by putting the lights back on 0 here removes some client processing when this vehicle is
		    (re)streamed as it doesn't need to apply damage that won't be visible anyways. But it also feels more right
		    to put this damage back to 0 since we fixed the vehicle, above hack is only to fix quirks for players that have
		    the vehicle streamed in anyways.
		*/
		vehicle->damageStatus.broken_lights.raw = 0;
	}
}

/**
This is named ChangeVehicleColor in the PAWN API.

Color will be reset to spawnColor1/spawnColor2 when the vehicle respawns
(so this function has the same behavior as changing the color in TransFender garages).
See also SetVehicleColorPermanent.
*/
static
void SetVehicleColorTemporary(int vehicleid, int col1, int col2)
{
	TRACE;
	struct SampVehicle *vehicle;

	vehicle = samp_pNetGame->vehiclePool->vehicles[vehicleid];
	if (vehicle) {
#ifndef NO_CAST_IMM_FUNCPTR
		((void (*)(struct SampVehicle*,short,int,int))0x814C510)(
			vehicle,
			INVALID_PLAYER_ID, /*The player that caused this update, so gamemode/filterscripts can block it (not applicable here).*/
			col1,
			col2
		);
#endif
	}
}

/**
This does not exist in the PAWN API.

Color will persist when the vehicle respawns.
See also SetVehicleColorTemporary.
*/
static
void SetVehicleColorPermanent(int vehicleid, int col1, int col2)
{
	TRACE;
	struct SampVehicle *vehicle;

	vehicle = samp_pNetGame->vehiclePool->vehicles[vehicleid];
	if (vehicle) {
		/*Setting temporary color is needed for the color to change for the players that have the vehicle streamed in.*/
		SetVehicleColorTemporary(vehicleid, col1, col2);
		/*Change the actual spawn colors, so the vehicle respawns with these colors.*/
		vehicle->spawnColor1 = col1;
		vehicle->spawnColor2 = col2;
		/*Unset modded colors, because the spawn colors are correct.*/
		vehicle->moddedColor1 = -1;
		vehicle->moddedColor2 = -1;
	}
}

/**
This does not exist in the PAWN API.
*/
static
__attribute__((unused))
int GetVehicleColor(int vehicleid, int *col1, int *col2)
{
	TRACE;
	struct SampVehicle *vehicle;

	vehicle = samp_pNetGame->vehiclePool->vehicles[vehicleid];
	if (vehicle) {
		if (vehicle->moddedColor1 != -1) {
			*col1 = vehicle->moddedColor1;
		} else {
			*col1 = vehicle->spawnColor1;
		}
		if (vehicle->moddedColor2 != -1) {
			*col2 = vehicle->moddedColor2;
		} else {
			*col2 = vehicle->spawnColor2;
		}
		return 1;
	}
	return 0;
}

static
__attribute__((unused))
void SetVehiclePaintjob(int vehicleid, char paintjob)
{
	TRACE;
	struct SampVehicle *vehicle;

	vehicle = samp_pNetGame->vehiclePool->vehicles[vehicleid];
	if (vehicle) {
#ifndef NO_CAST_IMM_FUNCPTR
		((void (*)(struct SampVehicle*,short,int))0x814C2F0)(
			vehicle,
			INVALID_PLAYER_ID, /*The player that caused this update, so gamemode/filterscripts can block it (not applicable here).*/
			paintjob
		);
#endif
	}
}

/**
In the PAWN API, text is actually the 2nd parameter. Placing it at the end here, for code style reasons.
*/
static
void GameTextForPlayer(int playerid, int milliseconds, int style, char *text)
{
	TRACE;
	struct RPCDATA_ShowGameText rpcdata;
	struct BitStream bs;
	int len;

	len = strlen(text);
	rpcdata.style = style;
	rpcdata.time = milliseconds;
	rpcdata.message_length = len;
	memcpy(&rpcdata.message, text, len);
	bs.ptrData = &rpcdata;
	bs.numberOfBitsUsed = (4 + 4 + 4 + len) * 8;
	SAMP_SendRPCToPlayer(RPC_ShowGameText, &bs, playerid, 3);
}

static
struct SampVehicle *GetSampVehicleByID(int vehicleid)
{
	TRACE;
	return samp_pNetGame->vehiclePool->vehicles[vehicleid];
}

static
int GetVehiclePos(int vehicleid, struct vec3 *pos)
{
	TRACE;
	struct SampVehicle *vehicle;

	vehicle = samp_pNetGame->vehiclePool->vehicles[vehicleid];
	if (vehicle) {
		*pos = vehicle->pos;
		return 1;
	}
	return 0;
}

/**
Crashes if vehicle does not exist.
*/
static
void GetVehiclePosUnsafe(int vehicleid, struct vec3 *pos)
{
	TRACE;
	*pos = samp_pNetGame->vehiclePool->vehicles[vehicleid]->pos;
}

/**
Crashes if vehicle does not exist.
*/
static
void GetVehicleVelocityUnsafe(int vehicleid, struct vec3 *vel)
{
	TRACE;
	*vel = samp_pNetGame->vehiclePool->vehicles[vehicleid]->vel;
}

static
void HideGameTextForPlayer(int playerid)
{
	TRACE;
	GameTextForPlayer(playerid, 2, 3, "_");
}

static
void CrashPlayer(int playerid)
{
	TRACE;
	struct RPCDATA_ShowGangZone rpcdata;
	struct BitStream bs;

	GameTextForPlayer(playerid, 5, 5, "Wasted~k~SWITCH_DEBUG_CAM_ON~~k~~TOGGLE_DPAD~~k~~NETWORK_TALK~~k~~SHOW_MOUSE_POINTER_TOGGLE~");

	bs.ptrData = &rpcdata;
	bs.numberOfBitsUsed = sizeof(rpcdata) * 8;
	rpcdata.zoneid = 65535;
	SAMP_SendRPCToPlayer(RPC_ShowGangZone, &bs, playerid, 2);
	rpcdata.zoneid = -2700;
	SAMP_SendRPCToPlayer(RPC_ShowGangZone, &bs, playerid, 2);
}
#endif

#ifndef SAMP_C_SKIP_MIXED_NATIVES /*This is just used to make testing easier, but should be maybe removed at some point...*/

/*-----------------------------------------------------------------------------*/

/*TODO: convert these natives_ funcs to non-amx things*/

/**
Callback for timer kicking a player, backup when OnPlayerUpdate doesn't get called.
*/
static
int kick_timer_cb(void *data)
#ifdef SAMP_NATIVES_IMPL
{
	TRACE;
	int playerid;

	playerid = PLAYER_CC_GETID(data);
	if (PLAYER_CC_CHECK(data, playerid)) {
		NC_PARS(1);
		nc_params[1] = playerid;
		NC(n_Kick_);
	}
	return 0;
}
#endif
;

/**
Kicks (delayed), broadcasts message and logs.

@param reason NULL to not broadcast a message (will have empty reason in table)
@param issuer name of issuer (NULL for system kick) (does not get saved to db)
@param issuer_userid may be -1 for system kick
@return 1 if the player is kicked, 0 if they were already kicked.
*/
static
int natives_Kick(int playerid, char *reason, char *issuer, int issuer_userid)
#ifdef SAMP_NATIVES_IMPL
{
	TRACE;
	const static char *SYSTEM_ISSUER = "system", *REASONNULL = "(NULL)";

	int intv;
	char *escapedreason;
	char msg[144];

	if (!kick_update_delay[playerid]) {
		if (issuer == NULL) {
			issuer = (char*) SYSTEM_ISSUER;
		}

		if (reason != NULL) {
			sprintf(msg,
				"%s[%d] was kicked by %s (%s)",
				pdata[playerid]->name,
				playerid,
				issuer,
				reason);
			SendClientMessageToAll(COL_WARN, msg);
		}

		GameTextForPlayer(playerid, 0x800000, 3, "~r~You've been kicked.");
		kick_update_delay[playerid] = 2;

		if (reason != NULL) {
			escapedreason = malloc(1000);
			common_mysql_escape_string(reason, escapedreason, 1000);
			reason = escapedreason;
		} else {
			escapedreason = NULL;
			reason = (char*) REASONNULL;
		}

		csprintf(buf4096,
			"INSERT INTO kck(usr,ip,stamp,issuer,reason)"
			"VALUES ("
			"IF(%d<1,NULL,%d),'%s',UNIX_TIMESTAMP(),"
			"IF(%d<1,NULL,%d),'%s')",
			userid[playerid],
			userid[playerid],
			pdata[playerid]->ip,
			issuer_userid,
			issuer_userid,
			escapedreason);
		NC_mysql_tquery_nocb(buf4096a);
		free(escapedreason);

		/*Interval should be smaller than 1000, since if it's a kick
		caused by system, player should be gone before the next
		check (since most of them run at 1Hz).*/
		intv = NC_GetPlayerPing(playerid) * 2;
		if (intv > 970) {
			intv = 970;
		}
		if (intv < 20) {
			intv = 20;
		}
		timer_set(intv, kick_timer_cb, (void*) MK_PLAYER_CC(playerid));
		return 1;
	}
	return 0;
}
#endif
;

/**
Puts a player in vehicle, updating map, zone, vehiclestuff ...

putting in driverseat: won't work if there already is a driver
putting in passengerseat: ejects existing passenger in that seat
putting in invalid passengerseat: nop with cars, works with planes (does not eject existing passenger)
warping seats/cars: Players still shows in original one,
                    keypresses are handled in original one. (can make ghost car like this).
                    A fix is in place in this function to prevent this.

Also resets the vehicle HP to 1000.0 when it's invalid.

@return 0 on failure
*/
static
int natives_PutPlayerInVehicle(int playerid, int vehicleid, int seat)
#ifdef SAMP_NATIVES_IMPL
{
	TRACE;
	register struct SampVehicle *vehicle;
	register float hp;
	struct vec3 pos;
	int oldvehicleid;
	int playerstate;

	vehicle = samp_pNetGame->vehiclePool->vehicles[vehicleid];
	if (!vehicle) {
		return 0;
	}

	if (seat == 0) {
		hp = vehicle->health;
		if (hp != hp || hp < 0.0f || hp > 1000.0f) {
			SAMP_SetVehicleHealth(vehicle, 1000.0f);
		}
	}

	GetVehiclePos(vehicleid, &pos);
	GameTextForPlayer(playerid, 0x80000, 3, "Loading objects...");
	maps_stream_for_player(playerid, pos.x, pos.y, OBJ_STREAM_MODE_CLOSEST_NOW);
	HideGameTextForPlayer(playerid);
	playerstate = GetPlayerState(playerid);
	if (playerstate == PLAYER_STATE_DRIVER || playerstate == PLAYER_STATE_PASSENGER) {
		/*Players will see the player in the original seat/vehicle when warping,
		so force send an onfoot packet first.*/
		ForceSendPlayerOnfootSyncNow(playerid);
		if (playerstate == PLAYER_STATE_DRIVER) {
			/*hook_OnDriverSync will invoke this already,
			but that's supposed to be as last-resort (for players that are vehicle warping)*/
			oldvehicleid = GetPlayerVehicleID(playerid);
			veh_on_driver_changed_vehicle_without_state_change(playerid, oldvehicleid, vehicleid);
			missions_on_driver_changed_vehicle_without_state_change(playerid, vehicleid);
		}
	}
	lastvehicle_asdriver[playerid] = vehicleid; /*So hook_OnDriverSync doesn't detect warping.*/
	svp_update_mapicons(playerid, pos.x, pos.y);
	missions_available_msptype_mask[playerid] = vehicle_msptypes[GetVehicleModel(vehicleid)];
	missions_update_missionpoint_indicators(playerid, pos.x, pos.y, pos.z);
	zones_update(playerid, pos);

	NC_PARS(3);
	nc_params[1] = playerid;
	nc_params[2] = vehicleid;
	nc_params[3] = seat;
	return NC(n_PutPlayerInVehicle_);
}
#endif
;

/**
To store player name and normalized name in plugin.
*/
static
int natives_SetPlayerName(int playerid, char *name)
#ifdef SAMP_NATIVES_IMPL
{
	TRACE;
	int res;
	char msg144[144];

	atoc(buf32, name, 32);
	NC_PARS(2);
	nc_params[1] = playerid;
	nc_params[2] = buf32a;
	res = NC(n_SetPlayerName_);
	if (res) {
		if (pdata[playerid]->name != name) {
			strcpy(pdata[playerid]->name, name);
		}
		pdata_on_name_updated(playerid);
		sprintf(msg144, "Your name has been changed to '%s'", pdata[playerid]->name);
		SendClientMessage(playerid, COL_SAMP_GREEN, msg144);
	}
	return res;
}
#endif
;

/**
Done here to do stuff, like streaming maps, anticheat?
*/
static
void natives_SetPlayerPos(int playerid, struct vec3 pos)
#ifdef SAMP_NATIVES_IMPL
{
	TRACE;
	GameTextForPlayer(playerid, 0x80000, 3, "Loading objects...");
	maps_stream_for_player(playerid, pos.x, pos.y, OBJ_STREAM_MODE_CLOSEST_NOW);
	HideGameTextForPlayer(playerid);
	svp_update_mapicons(playerid, pos.x, pos.y);
	missions_update_missionpoint_indicators(playerid, pos.x, pos.y, pos.z);
	nametags_update_for_player(playerid);
	zones_update(playerid, pos);

	SetPlayerPosRaw(playerid, &pos);
}
#endif
;

/**
SpawnPlayer kills players that are in vehicles, and spawns them with a bottle.

So this wrapper does ClearAnimations first if needed, because that will eject a player.
*/
static
void natives_SpawnPlayer(int playerid)
#ifdef SAMP_NATIVES_IMPL
{
	TRACE;
	struct SpawnInfo spawnInfo;

	/*eject player first if they're in a vehicle*/
	if (GetPlayerVehicleID(playerid)) {
		NC_ClearAnimations(playerid, 1);
	}
	spawn_get_random_spawn(playerid, &spawnInfo);
	SetSpawnInfo(playerid, &spawnInfo);
	SpawnPlayer(playerid);
}
#endif
;

/*-----------------------------------------------------------------------------*/

#ifdef SAMP_NATIVES_IMPL
static int player_keystates[MAX_PLAYERS];

static
void samp_OnPlayerUpdate(int playerid)
{
	TRACE;
	if (kick_update_delay[playerid] > 0) {
		if (--kick_update_delay[playerid] == 0) {
			NC_PARS(1);
			nc_params[1] = playerid;
			NC(n_Kick_);
		}
		player[playerid]->updateSyncType = 0; /*Reject update, equivalent to 'return 0' in OnPlayerUpdate.*/
		return;
	}
	playerstats_on_player_update(playerid);
	timecyc_on_player_update(playerid);

	lastvehicle_asdriver[playerid] = 0; /*This will be set again at the end of hook_OnDriverSync.*/

	/*When wanting to return 0, set CPlayer::updateSyncType to 0.*/
}

void hook_OnOnfootSync(int playerid)
{
	TRACE;
	struct SYNCDATA_Onfoot *data;
	int oldkeys, newkeys;

	data = &player[playerid]->onfootSyncData;

	/*keystate change*/
	oldkeys = player_keystates[playerid];
	newkeys = samp_combine_keys(data->partial_keys, data->additional_keys & 3);
	if (oldkeys != newkeys) {
		if (KEY_JUST_DOWN(KEY_FIRE) && data->weapon_id == 0) {
			copilot_handle_onfoot_fire(playerid, data->pos);
		}
		player_keystates[playerid] = newkeys;
	}

	/*Disable all onfoot weapons.*/
	/*People can still cheat in weapons, but they won't be able to shoot others.*/
	/*This doesn't give issues with syncing activating parachutes.*/
	/*Previously, only NVIS/IRVIS/CAMERA was unsynced,
	but there's no reason to sync anything so everything is now unsynced.*/
	/*Note that putting it here means that there will be a difference between
	GetPlayerKeys (which uses the sync data)and player_keystates (which is set above).*/
	data->partial_keys &= ~KEY_FIRE;

	/*TODO remove this when all OnPlayerUpdates are replaced*/
	samp_OnPlayerUpdate(playerid);

	/*When wanting to return 0, set CPlayer::updateSyncType to 0.*/
}

static char drive_udkeystate[MAX_PLAYERS];

void hook_OnDriverSync(int playerid)
{
	TRACE;
	struct SYNCDATA_Driver *data;
	struct SampVehicle *vehicle;
	/*TODO reset these keystate variables when player gets into the drive state?*/
	int oldkeys, newkeys;
	int storedlastvehicleid;
	short vehiclemodel;

	data = &player[playerid]->driverSyncData;
	vehicle = vehiclepool->vehicles[data->vehicle_id];

	if (!vehicle) {
		/*It sometimes happens that driversync packets of just deleted vehicles are sent/arrive late.*/
		player[playerid]->updateSyncType = 0;
		samp_OnPlayerUpdate(playerid); /*TODO: what to do with this...*/
		lastvehicle_asdriver[playerid] = data->vehicle_id; /*because ^ OnPlayerUpdate resets it..*/
		return;
	}

	/*Suppress secondary fire (my R key: hydra rockets, hunter/rustler/seaspar/rcbaron minigun).*/
	data->partial_keys &= ~KEY_ACTION;

	/*Suppress primary fire (my LCTRL key: hunter rockets, predator minigun).*/
	/*But allow firetruk/swatvan water cannon and hydra flares..*/
	/*Watercannons can be used to annoy players though... But allow it for now.*/
	if (data->partial_keys & KEY_FIRE) {
		vehiclemodel = vehicle->model;
		if (vehiclemodel != MODEL_FIRETRUK && vehiclemodel != MODEL_SWATVAN && vehiclemodel != MODEL_HYDRA) {
			data->partial_keys &= ~KEY_FIRE;
		}
	}

	vehicle_gear_state[data->vehicle_id] = data->landing_gear_state;

	/*keystate change*/
	oldkeys = player_keystates[playerid];
	newkeys = samp_combine_keys(data->partial_keys, data->additional_keys & 3);
	if (oldkeys != newkeys) {
		if (KEY_JUST_DOWN(KEY_SUBMISSION)) {
			vehicle_gear_change_time[data->vehicle_id] = time_timestamp();
		}
		missions_driversync_keystate_change(playerid, oldkeys, newkeys);
		veh_on_driver_key_state_change(playerid, oldkeys, newkeys);
		player_keystates[playerid] = newkeys;
	}

	/*up/down keystate change*/
	if (data->udkey == 0) {
		if (drive_udkeystate[playerid] != 0) {
			missions_driversync_udkeystate_change(playerid, data->udkey);
			drive_udkeystate[playerid] = 0;
		}
	} else if (data->udkey < 0) {
		if (drive_udkeystate[playerid] != -1) {
			missions_driversync_udkeystate_change(playerid, data->udkey);
			drive_udkeystate[playerid] = -1;
		}
	} else {
		if (drive_udkeystate[playerid] != 1) {
			missions_driversync_udkeystate_change(playerid, data->udkey);
			drive_udkeystate[playerid] = 1;
		}
	}

	/*printf("DRIVER SYNC ud %hd rl %hd lnd %hd\n", data->udkey, data->lrkey, data->landing_gear_state);*/

	storedlastvehicleid = lastvehicle_asdriver[playerid]; /*storing this because OnPlayerUpdate resets it to 0*/

	/*TODO remove this when all OnPlayerUpdates are replaced*/
	samp_OnPlayerUpdate(playerid);

	lastvehicle_asdriver[playerid] = data->vehicle_id; /*always write this because OnPlayerUpdate resets it to 0*/
	if (storedlastvehicleid != data->vehicle_id) {
		veh_on_driver_changed_vehicle_without_state_change(playerid, storedlastvehicleid, data->vehicle_id);
		missions_on_driver_changed_vehicle_without_state_change(playerid, data->vehicle_id);
		/*TODO: This is vehicle warping when reaching this (but how trustworthy is it?
		Maybe a player is still sending driversync of the old vehicle
		after PutPlayerInVehicle was called?)*/
	}

	missions_on_driversync(playerid, data);

	/*When wanting to return 0, set CPlayer::updateSyncType to 0.*/
}

void hook_OnPassengerSync(int playerid)
{
	TRACE;
	struct SYNCDATA_Passenger *data;
	int oldkeys, newkeys;

	data = &player[playerid]->passengerSyncData;

	/*keystate change*/
	oldkeys = player_keystates[playerid];
	newkeys = samp_combine_keys(data->partial_keys, data->additional_keys & 3);
	if (oldkeys != newkeys) {
		if (KEY_JUST_DOWN(KEY_JUMP)) {
			copilot_handler_passenger_brake(playerid, data->vehicle_id);
		}
		player_keystates[playerid] = newkeys;
	}

	/*TODO remove this when all OnPlayerUpdates are replaced*/
	samp_OnPlayerUpdate(playerid);

	/*When wanting to return 0, set CPlayer::updateSyncType to 0.*/
}

void hook_OnPlayerRequestClass(int playerid, int classid)
{
	TRACE;
	struct RPCDATA_RequestClass03DL rpcdata03DL;
	struct RPCDATA_RequestClass037 rpcdata037;

	class_on_player_request_class(playerid, classid);
	timecyc_on_player_request_class(playerid);

	spawn_get_random_spawn(playerid, &rpcdata037.spawnInfo);
	if (is_player_using_client_version_DL[playerid]) {
		rpcdata03DL.response = 1;
		convertSpawnInfoToSpawnInfo03DL(&rpcdata037.spawnInfo, &rpcdata03DL.spawnInfo);
		SendRPC_8C(playerid, RPC_RequestClass, &rpcdata03DL, sizeof(rpcdata03DL), HIGH_PRIORITY, RELIABLE, 0);
	} else {
		rpcdata037.response = 1;
		SendRPC_8C(playerid, RPC_RequestClass, &rpcdata037, sizeof(rpcdata037), HIGH_PRIORITY, RELIABLE, 0);
	}

	player[playerid]->spawnInfo = rpcdata037.spawnInfo;
	player[playerid]->hasSpawnInfo = 1; /*otherwise SAMP will ignore client Spawn packets and player will not be marked (not broadcasted) as spawned despited being spawned*/
}

void StreamInPlayer(struct SampPlayer *player, int forplayer)
{
	TRACE;
	if (is_player_using_client_version_DL[forplayer]) {
		struct RPCDATA_WorldPlayerAdd03DL rpcdata03DL;
		rpcdata03DL.playerid = player->playerid;
		rpcdata03DL.team = player->spawnInfo.team;
		rpcdata03DL.skin = player->spawnInfo.skin;
		rpcdata03DL.customSkin = player->spawnInfo.skin; /*Brunoo16's packet list say to keep this 0, but if I make either skin field 0, it will always show a CJ skin...*/
		rpcdata03DL.pos = player->pos;
		rpcdata03DL.facingAngle = player->facingAngle;
		rpcdata03DL.color = player->color;
		rpcdata03DL.fightingstyle = player->fightingstyle;
		memcpy(rpcdata03DL.weaponSkill, player->weaponSkill, sizeof(player->weaponSkill));
		SendRPCToPlayer(forplayer, RPC_WorldPlayerAdd, &rpcdata03DL, sizeof(rpcdata03DL), 2);
	} else {
		struct RPCDATA_WorldPlayerAdd037 rpcdata037;
		rpcdata037.playerid = player->playerid;
		rpcdata037.team = player->spawnInfo.team;
		rpcdata037.skin = player->spawnInfo.skin;
		rpcdata037.pos = player->pos;
		rpcdata037.facingAngle = player->facingAngle;
		rpcdata037.color = player->color;
		rpcdata037.fightingstyle = player->fightingstyle;
		memcpy(rpcdata037.weaponSkill, player->weaponSkill, sizeof(player->weaponSkill));
		SendRPCToPlayer(forplayer, RPC_WorldPlayerAdd, &rpcdata037, sizeof(rpcdata037), 2);
	}

	/*SAMP here also sends RPC_SetPlayerAttachedObject for each slot but we currently don't use player attached objects*/
}

static
void OnRPCUpdateVehicleDamageStatus(struct RakRPCHandlerArg *arg)
{
	TRACE;
	struct INOUTRPCDATA_UpdateVehicleDamageStatus *rpcdata;
	struct SampVehicle *vehicle;
	int playerid, vehicleid, i;
	struct PlayerID playerID;
	struct BitStream bs;
#ifdef DEV
	char msg144[144];
#endif

	if (
		samp_pNetGame->gamestate != 1 ||
		arg->numBits != sizeof(struct INOUTRPCDATA_UpdateVehicleDamageStatus) * 8
	) {
		return;
	}
	rpcdata = (void*) arg->rpcdata;
	vehicleid = rpcdata->vehicleid;
	playerid = rakServer->vtable->GetIndexFromPlayerID(rakServer, arg->playerID);
	if (
		(unsigned int) playerid >= MAX_PLAYERS ||
		!playerpool->players[playerid] ||
		(unsigned int) vehicleid >= 2000 ||
		!(vehicle = vehiclepool->vehicles[vehicleid]) ||
		vehicle->driverplayerid != playerid
	) {
		return;
	}

#ifdef DEV
	sprintf(
		msg144,
		"UpdateVehicleDamageStatus pid %d vid %d panels %08X->%08X doors %08X->%08X lights %02X->%02X tires %02X->%02X",
		playerid, vehicleid,
		vehicle->damageStatus.panels.raw, rpcdata->panels,
		vehicle->damageStatus.doors.raw, rpcdata->doors,
		vehicle->damageStatus.broken_lights.raw, rpcdata->broken_lights,
		vehicle->damageStatus.popped_tires.raw, rpcdata->popped_tires
	);
	SendClientMessageToAll(COL_SAMP_GREY, msg144);
#endif

	vehicle->damageStatus.panels.raw = rpcdata->panels;
	vehicle->damageStatus.doors.raw = rpcdata->doors;
	vehicle->damageStatus.broken_lights.raw = rpcdata->broken_lights;
	vehicle->damageStatus.popped_tires.raw = rpcdata->popped_tires;

	/*incoming rpcdata is same as outgoing*/
	bs.ptrData = rpcdata;
	bs.numberOfBitsUsed = arg->numBits;

	for (i = playerpool->highestUsedPlayerid; i >= 0; i--) {
		if (i != playerid && player[i] && player[i]->vehicleStreamedIn[vehicleid]) {
#ifndef NO_CAST_IMM_FUNCPTR
			RakServer__GetPlayerIDFromIndex(&playerID, rakServer, i);
			rakServer->vtable->RPC_8C(rakServer, (void*) RPC_UpdateVehicleDamageStatus, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, /*orderingChannel*/ 2, playerID, /*broadcast*/ 0, /*shiftTimestamp*/ 0);
#endif
		}
	}
}

static
void samp_init()
{
	TRACE;
	char *str;

	samp_pNetGame = *(struct SampNetGame**) 0x81CA4BC;
	samp_pConsole = *(int**) 0x81CA4B8;
	playerpool = samp_pNetGame->playerPool;
	vehiclepool = samp_pNetGame->vehiclePool;
	rakServer = samp_pNetGame->rakServer;

	/*Remove filtering in chat messages coming from clients.*/
	/*OnPlayerText*/
	mem_protect(0x80B0760, 0x14, PROT_READ | PROT_WRITE | PROT_EXEC);
	*(int*) 0x80B0760 = 0x90909090;
	*(unsigned char*) 0x80B0764 = 0x90;
	/*OnPlayerCommandText*/
	mem_protect(0x80B16D6, 0x14, PROT_READ | PROT_WRITE | PROT_EXEC);
	*(int*) 0x80B16D6 = 0x90909090;
	*(unsigned char*) 0x80B16DA = 0x90;

	/*color the hostname that we send to the client, because we can*/
	/*it will be colorized in the client's "Connected to " message, and in the scoreboard title*/
	str = GetConsoleVariableString("hostname");
	sprintf(cbuf144, "{1b8ae4}%s", str);
	AddConsoleVariableString("coloredhostname", cbuf144);
	mem_protect(0x80AE1D8, 4, PROT_READ | PROT_WRITE | PROT_EXEC);
	*((char**) 0x80AE1D8) = "coloredhostname";

	/*Sync data is dropped when coordinates exceed -20k/+20k, update those limits here.
	See 804B5D0 CheckSyncBounds.*/
	/*Not adding exec will result in a segfault on server shutdown. This section is read|exec by default.*/
	mem_protect(0x815070C, 0x10, PROT_READ | PROT_WRITE | PROT_EXEC);
	*((int*) 0x815070C) = WORLD_XY_MAX; /*sync bounds x/y max, default 20000.0*/
	*((int*) 0x8150710) = WORLD_XY_MIN; /*sync bounds x/y min, default -20000.0*/
	/**(int*) 0x8150714 = ; *//*sync bounds z max, default 200000.0*/
	/**(int*) 0x8150718 = ; *//*sync bounds z min, default -1000.0*/

	mem_mkjmp(0x80AC99C, &OnfootSyncHook);
	mem_mkjmp(0x80AEC4F, &DriverSyncHook);
	mem_mkjmp(0x80AEA7D, &PassengerSyncHook);
	mem_mkjmp(0x80B1712, &OnPlayerCommandTextHook);
	mem_mkjmp(0x80B2BA2, &OnDialogResponseHook);
	mem_mkjmp(0x80B09D4, &OnPlayerRequestClassHook);
	mem_mkjmp(0x80B1020, &OnRPCUpdateVehicleDamageStatus);

	/*stuff to allow both 0.3.7 and 0.3.DL clients */
	AddServerRule("artwork", "No"); /*rule that DL added, probably not needed to have but setting it anyways*/
	AddServerRule("allowed_clients", "0.3.7, 0.3.DL"); /*open.mp started setting this I guess so we roll with it too*/
	mem_mkjmp(0x80B4B89, &ClientJoinCheckVersionHook);
	mem_mkjmp(0x80D0EF2, &StreamInPlayer);

	AddServerRule("hotel", "Trivago"); /*be a little silly*/
}
#endif

#endif /*SAMP_C_SKIP_MIXED_NATIVES*/
