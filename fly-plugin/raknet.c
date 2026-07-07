static struct RakServer *rakServer;
static struct RakServer_vtable *rakServerVtable;
static struct PlayerID rakPlayerID[MAX_PLAYERS];
static int (*rakRPC_8C)(
	struct RakServer *this,
	char *rpc,
	struct BitStream *bitStream,
	enum PacketPriority priority,
	enum PacketReliability reliability,
	char orderingChannel,
	struct PlayerID playerID,
	int bBroadcast,
	int bShiftTimestamp
);
static int (*rakSendBitStream)(
	struct RakServer *this,
	struct BitStream *bs,
	enum PacketPriority priority,
	enum PacketReliability reliability,
	char orderingChannel,
	struct PlayerID playerID,
	int bBroadcast
);

static
__attribute__((unused))
int raknet_get_messages_in_send_buffer_for_player(int playerid)
{
	TRACE;
	register struct RakNetStatistics *stats;

	stats = rakServer->vtable->GetStatistics(rakServer, rakPlayerID[playerid]);
	return stats->messageSendBufferPrior0 +
		stats->messageSendBufferPrior1 +
		stats->messageSendBufferPrior2 +
		stats->messageSendBufferPrior3;
}

static
void bitstream_write_zero(struct BitStream *bitstream)
{
	TRACE;
	((void (*)(struct BitStream*))0x804D3E0)(bitstream);
}

static
void bitstream_write_one(struct BitStream *bitstream)
{
	TRACE;
	((void (*)(struct BitStream*))0x804D370)(bitstream);
}

static
void bitstream_write_bits(struct BitStream *bitstream, int value, int num_bits)
{
	TRACE;
	((void (*)(struct BitStream*,void*,int,char))0x804D250)(bitstream, &value, num_bits, 1);
}

static
void bitstream_write_bytes(struct BitStream *bitstream, void *value, int num_bytes)
{
	TRACE;
	((void (*)(struct BitStream*,void*,int))0x804D680)(bitstream, value, num_bytes);
}

static
void bitstream_write_quaternion(struct BitStream *bitstream, float w, float x, float y, float z)
{
	TRACE;
	((void (*)(struct BitStream*,float,float,float,float))0x80CE880)(bitstream, w, x, y, z);
}

static
void bitstream_write_velocity(struct BitStream *bitstream, float x, float y, float z)
{
	TRACE;
	((void (*)(struct BitStream*,float,float,float))0x80CEA50)(bitstream, x, y, z);
}

enum SampRPC {

	/*All below here use HIGH_PRIORITY, RELIABLE, orderingchannel 0*/
	RPC_RequestClass = 0x81572DF, /*ptr to 0x80(128)*/

	/*All below here use HIGH_PRIORITY, RELIABLE, orderingchannel 3*/
	RPC_SendClientMessage = 0x815A027, /*ptr to 0x5D(93)*/
	RPC_ShowGameText = 0x815A19A, /*ptr to 0x49(73)*/

	/*All below here use HIGH_PRIORITY, RELIABLE_ORDERED, orderingchannel 2*/
	RPC_ShowGangZone = 0x81587A1, /*ptr to 0x6C(108)*/
	RPC_HideGangZone = 0x8163BA7, /*ptr to 0x78(120)*/
	RPC_CreateObject = 0x815A7F8, /*ptr to 0x2C(44)*/
	RPC_DestroyObject = 0x815ACC6, /*ptr to 0x2F(47)*/
	RPC_RemoveBuilding = 0x815CD7E, /*ptr to 0x2B(43)*/
	RPC_MoveObject = 0x8158B64, /*ptr to 0x64(99)*/
	RPC_HideActor = 0x8157CCB, /*ptr to 0xAC(172)*/
	RPC_ShowActor = 0x8157CC9, /*ptr to 0xAB(171)*/
	RPC_ShowTextDraw = 0x815AA74, /*ptr to 0x86(134)*/
	RPC_HideTextDraw = 0x815AA72, /*ptr to 0x87(135)*/
	RPC_TogglePlayerControllable = 0x815CCFA, /*ptr to 0x0F(15)*/
	RPC_PlaySound = 0x815CD0C, /*ptr to 0x10(16)*/
	RPC_SetWorldBounds = 0x815CCB8, /*ptr to 0x11(17)*/
	RPC_TextDrawSetString = 0x815214C, /*ptr to 0x69(105)*/
	RPC_SetRaceCheckpoint = 0x815CD7A, /*ptr to 0x26(38)*/
	RPC_DisableRaceCheckpoint = 0x81587C7, /*ptr to 0x27(39)*/
	RPC_SetCheckpoint = 0x81639A5, /*ptr to 0x6B(107)*/
	RPC_DisableCheckpoint = 0x815A860, /*ptr to 0x25(37) (rpc has no data)*/
	RPC_SetVehicleParams = 0x815CCFC, /*ptr to 0xA1(161)*/
	RPC_SetVehicleParamsEx = 0x8166226, /*ptr to 0x18(24)*/
	RPC_Create3DTextLabel = 0x815072C, /*ptr to 0x24(36)*/
	RPC_SetPlayerMapIcon = 0x8162E9F, /*ptr to 0x38(56)*/
	RPC_RemovePlayerMapIcon = 0x815CD22, /*ptr to 0x90(144)*/
	RPC_Delete3DTextLabel = 0x8159960, /*ptr to 0x3A(58)*/
	RPC_DisableRemoteVehicleCollisions = 0x815CD80, /*ptr to 0xA7(167)*/
	RPC_ShowDialog = 0x815CD7C, /*ptr to 0x3D(61)*/
	RPC_RequestSpawn = 0x815A61E, /*ptr to 0x81(129)*/
	RPC_CreateVehicle = 0x8166228, /*ptr to 0xA4(164)*/
	RPC_DeleteVehicle = 0x816622C, /*ptr to 0xA5(165)*/
	RPC_SetVehicleNumberplate = 0x816622A, /*ptr to  0x7B(123)*/
	RPC_SetVehiclePos = 0x8166224, /*ptr to 0x9F(159)*/
	RPC_SetVehicleZAngle = 0x815CD0A, /*ptr to 0xA0(160)*/
	RPC_SetPlayerPos = 0x815CCEC, /*ptr to 0xC(12)*/
	RPC_SetPlayerFacingAngle = 0x815CD04, /*ptr to 0x13(19)*/
	RPC_SetHealth = 0x815CCF0, /*ptr to 0xE(14)*/
	RPC_MoneyGive = 0x815CCFE, /*ptr to 0x12(18)*/
	RPC_MoneyReset = 0x815CD00, /*ptr to 0x14(20) (rpc has no data)*/
	RPC_SetTime = 0x815A832, /*ptr to 0x1D(29)*/
	RPC_SetWeather = 0x815A00F, /*ptr to 0x98(152)*/
	RPC_ToggleClock = 0x815A875, /*ptr to 0x1E(30)*/
	RPC_ToggleSpectating = 0x815CCE6, /*ptr to 0x7C(124)*/
	RPC_SpectatePlayer = 0x815CCEA, /*ptr to 0x7E(126)*/
	RPC_SpectateVehicle = 0x815CCE8, /*ptr to 0x7F(127)*/
	RPC_PlayerJoin = 0x815AA76, /*ptr to 0x89(137)*/
	RPC_PlayerCreate = 0x816324E, /*ptr to 0x20(32)*/
	RPC_PlayerLeave = 0x815AA78, /*ptr to 0x8A(138)*/
	RPC_PutPlayerInVehicle = 0x815A058, /*ptr to 0x46(70)*/
	RPC_SetSpawnInfo = 0x8162624, /*ptr to 0x44(68)*/
	RPC_WorldPlayerAdd = 0x816324E, /*ptr to 0x20(32)*/
	RPC_WorldPlayerRemove = 0x815AA7A, /*ptr to 0xA3(163)*/
	RPC_SetPlayerName = 0x815CCC5, /*ptr to 0xB(11)*/
	RPC_SetPlayerSkin = 0x815CCE4, /*ptr to 0x99(153)*/
	RPC_SetPlayerColor = 0x8163C35, /*ptr to 0x48(72)*/
	RPC_SetCameraPos = 0x815CCF4, /*ptr to 0x9D(157)*/
	RPC_SetCameraLookAt = 0x815CCF6, /*ptr to 0x9E(158)*/
	RPC_SetCameraBehind = 0x815CCF8, /*ptr to 0xA2(162) (rpc has no data)*/
	RPC_InterpolateCamera = 0x8162B73, /*ptr to 0x52(82)*/
	RPC_SetSpecialAction = 0x815ABC5, /*ptr to 0x58(88)*/
	RPC_ForceClassSelection = 0x81587AC, /*ptr to 0x4A(74) (rpc has no data)*/
	RPC_GiveWeapon = 0x815CD08, /*ptr to 0x16(22)*/
	RPC_UpdateVehicleDamageStatus = 0x815A64D, /*ptr to 0x6A(106)*/
	RPC_ClearAnimations = 0x816322D, /*ptr to 0x57(87)*/
	RPC_ApplyAnimation = 0x815CD24, /*ptr to 0x56(86)*/
	RPC_SetVehicleHealth = 0x8166222, /*ptr to 0x93(147)*/
};

/*Uses HIGH_PRIORITY, RELIABLE_ORDERED, orderingchannel 2. Nearly all RPCs use this config.*/
static
void SendRPC(ushort playerid, enum SampRPC rpc, void *rpcdata, int size_bits)
{
	static struct BitStream bs; /*static to make the function easier to inline*/

	bs.ptrData = rpcdata;
	bs.numberOfBitsUsed = size_bits;

	rakRPC_8C(rakServer, (void*) rpc, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 2, rakPlayerID[playerid], /*broadcast*/ 0, /*shiftTimestamp*/ 0);
}

/*BitStream version of SendRPC. Use this for bulk sending.*/
static
void SendRPC_bs(ushort playerid, enum SampRPC rpc, struct BitStream *bs)
{
	rakRPC_8C(rakServer, (void*) rpc, bs, HIGH_PRIORITY, RELIABLE_ORDERED, 2, rakPlayerID[playerid], /*broadcast*/ 0, /*shiftTimestamp*/ 0);
}

/*Uses HIGH_PRIORITY, RELIABLE, orderingchannel 3.*/
static
void SendRPC_unordered(ushort playerid, enum SampRPC rpc, void *rpcdata, int size_bits)
{
	static struct BitStream bs; /*static to make the function easier to inline*/

	bs.ptrData = rpcdata;
	bs.numberOfBitsUsed = size_bits;

	rakRPC_8C(rakServer, (void*) rpc, &bs, HIGH_PRIORITY, RELIABLE, 3, rakPlayerID[playerid], /*broadcast*/ 0, /*shiftTimestamp*/ 0);
}

static
void SendRPC_ex(ushort playerid, enum SampRPC rpc, void *rpcdata, int size_bytes, enum PacketPriority priority, enum PacketReliability reliability, int orderingChannel)
{
	struct BitStream bs;

	bs.ptrData = rpcdata;
	bs.numberOfBitsUsed = size_bytes * 8;
	rakRPC_8C(rakServer, (void*) rpc, &bs, priority, reliability, orderingChannel, rakPlayerID[playerid], /*broadcast*/ 0, /*shiftTimestamp*/ 0);
}

/*Uses HIGH_PRIORITY, UNRELIABLE_SEQUENCED, orderingchannel 1.*/
static
void SendSyncPacket(ushort playerid, struct BitStream *bs)
{
	rakSendBitStream(rakServer, bs, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, /*orderingchannel*/ 1, rakPlayerID[playerid], /*broadcast*/ 0);
}

/*Uses same config as SendRPC*/
static
void SendSyncPacketReliableOrdered(ushort playerid, struct BitStream *bs)
{
	rakSendBitStream(rakServer, bs, HIGH_PRIORITY, RELIABLE_ORDERED, /*orderingchannel*/ 2, rakPlayerID[playerid], /*broadcast*/ 0);
}
