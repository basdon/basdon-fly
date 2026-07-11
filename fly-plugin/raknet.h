EXPECT_SIZE(char, 1);
EXPECT_SIZE(short, 2);
EXPECT_SIZE(int, 4);

#pragma pack(1)
struct BitStream {
	int numberOfBitsUsed;
	int numberOfBitsAllocated;
	int readOffset;
	void *ptrData;
	char copyData;
};

struct PlayerID {
	int binaryAddress;
	unsigned short port;
};
EXPECT_SIZE(struct PlayerID, 0x6);
STATIC_ASSERT_MEMBER_OFFSET(struct PlayerID, port, 0x4);

enum PacketPriority {
	SYSTEM_PRIORITY = 0,
	HIGH_PRIORITY = 1,
	MEDIUM_PRIORITY = 2,
	LOW_PRIORITY = 3,
};

enum PacketReliability {
	UNRELIABLE = 6, /*regular UDP*/
	UNRELIABLE_SEQUENCED = 7, /*regular UDP, but ignored if a newer sequenced packet was acted upon first*/
	RELIABLE = 8, /*gets resent until acknowledged*/
	RELIABLE_ORDERED = 9, /*gets resent until acknowledged, not acted upon until all earlier ordered messages are acted upon*/
	RELIABLE_SEQUENCED = 10, /*gets resent until acknowledged, but ignored if a newer sequenced packet was acted upon first*/
};

struct RakNetStatistics {
	int messageSendBufferPrior0;
	int messageSendBufferPrior1;
	int messageSendBufferPrior2;
	int messageSendBufferPrior3;
	int messagesSentPrior0;
	int messagesSentPrior1;
	int messagesSentPrior2;
	int messagesSentPrior3;
	int _pad20[21];
	int acknowledgementsSent;
	int acknowledgementsInSendBuffer;
	int _pad7C[3];
	int messagesResent;
	int _pad8C[4];
	int messagesOnResendQueue; /*aka messages waiting for ack*/
};

struct RakServer {
	struct RakServer_vtable *vtable;
};

struct RakServer_vtable {
	int _pad0[9];
	int (*SendBitStream)(struct RakServer *this, struct BitStream *bs, enum PacketPriority prio, enum PacketReliability rel, char orderingChannel, struct PlayerID playerID, int bBroadcast);
	int _pad28;
	int _pad2C;
	int (*Kick)(struct RakServer *this, struct PlayerID playerID);
	int _pad34[7];
	int (*GetLastPing)(struct RakServer *this, struct PlayerID playerID);
	int _pad54[14];
	int (*RPC_8C)(struct RakServer *this, char *rpc, struct BitStream *bitStream, enum PacketPriority priority, enum PacketReliability reliability, char orderingChannel, struct PlayerID playerID, int bBroadcast, int bShiftTimestamp);
	int _pad90[22];
	short (*GetIndexFromPlayerID)(struct RakServer *rakServer, struct PlayerID playerID);
	/** equal to actual signature: void (*GetPlayerIDFromIndex)(struct PlayerID *outPlayerId, struct RakServer *rakServer, short playerIndex)*/
	struct PlayerID (*GetPlayerIDFromIndex)(struct RakServer *rakServer, short playerIndex);
	int _padF0;
	void (*AddToBanList)(struct RakServer *rakServer, char *dottedIpAddr, int unkMaybeDurationMs);
	char _padF8[0x118-0xF8];
	struct RakNetStatistics *(*GetStatistics)(struct RakServer *rakServer, struct PlayerID playerID);
};
STATIC_ASSERT_MEMBER_OFFSET(struct RakServer_vtable, SendBitStream, 0x24);
STATIC_ASSERT_MEMBER_OFFSET(struct RakServer_vtable, Kick, 0x30);
STATIC_ASSERT_MEMBER_OFFSET(struct RakServer_vtable, GetLastPing, 0x50);
STATIC_ASSERT_MEMBER_OFFSET(struct RakServer_vtable, RPC_8C, 0x8C);
STATIC_ASSERT_MEMBER_OFFSET(struct RakServer_vtable, GetIndexFromPlayerID, 0xE8);
STATIC_ASSERT_MEMBER_OFFSET(struct RakServer_vtable, GetPlayerIDFromIndex, 0xEC);
STATIC_ASSERT_MEMBER_OFFSET(struct RakServer_vtable, AddToBanList, 0xF4);
STATIC_ASSERT_MEMBER_OFFSET(struct RakServer_vtable, GetStatistics, 0x118); /*TODO: revalidate*/

struct RakRPCHandlerArg {
	unsigned char *rpcdata;
	int numBits;
	struct PlayerID playerID;
};
EXPECT_SIZE(struct RakRPCHandlerArg, 0xE);

struct RakPacket {
	ushort playerid;
	struct PlayerID playerID;
	int byteLength;
	int bitLength;
	uchar *data;
};
EXPECT_SIZE(struct RakPacket, 0x14);
#pragma pack()
