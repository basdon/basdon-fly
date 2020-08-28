static
int samp_pNetGame;

/**
When respawning, the player will always regain control.

Player's vehicle will not get damage when a player is not controllable.
IT EVEN GETS REPAIRED. Vehicle HP will be set to 300 when making player
uncontrollable and the vehicle hp is < 250 (maybe even when it's < 300?)
*/
static
void TogglePlayerControllable(int playerid, char controllable)
{
	rpcdata_freeform.byte[0] = controllable;
	bitstream_freeform.numberOfBitsUsed = 8;
	bitstream_freeform.ptrData = &rpcdata_freeform;
	SAMP_SendRPCToPlayer(RPC_TogglePlayerControllable, &bitstream_freeform, playerid, 2);
}

static
void SendClientMessage(int playerid, int color, char *message)
{
	struct RPCDATA_SendClientMessage data;

	data.color = color;
	data.message_length = strlen(message);
	if (data.message_length > 144) {
		/*TODO: handle messages with a length > 144 (parse the last embedded color)*/
		data.message_length = 144;
	}
	memcpy(data.message, message, data.message_length);
	bitstream_freeform.ptrData = &data;
	bitstream_freeform.numberOfBitsUsed = 32 + 32 + data.message_length * 8;
	SAMP_SendRPCToPlayer(RPC_SendClientMessage, &bitstream_freeform, playerid, 3);
}

static
void PlayerPlaySound(int playerid, int soundid)
{
	rpcdata_freeform.dword[0] = soundid;
	rpcdata_freeform.dword[1] = 0;
	rpcdata_freeform.dword[2] = 0;
	rpcdata_freeform.dword[3] = 0;
	bitstream_freeform.ptrData = &rpcdata_freeform;
	bitstream_freeform.numberOfBitsUsed = sizeof(struct RPCDATA_PlaySound) * 8;
	SAMP_SendRPCToPlayer(RPC_PlaySound, &bitstream_freeform, playerid, 2);
}
