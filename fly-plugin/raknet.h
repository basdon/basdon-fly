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
	short port;
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
	int _pad0[35];
	int (*RPC_8C)(struct RakServer *this, char *rpc, struct BitStream *bitStream, enum PacketPriority priority, enum PacketReliability reliability, char orderingChannel, struct PlayerID playerID, int bBroadcast, int bShiftTimestamp);
	int _pad90[24];
	/**thiscall with one parameter, use RakServer__GetPlayerIDFromIndex in samp.asm*/
	void (*GetPlayerIDFromIndex)(struct PlayerID *outPlayerID, struct RakServer *rakServer, short playerIndex);
	int _padF4[9];
	struct RakNetStatistics *(*GetStatistics)(struct RakServer *rakServer, struct PlayerID playerID);
};
#ifndef __TINYC__ /*my tcc binaries are 64bit so ptrs will be 8 bytes instead of 4, disabling these for tcc*/
STATIC_ASSERT_MEMBER_OFFSET(struct RakServer_vtable, RPC_8C, 0x8C);
STATIC_ASSERT_MEMBER_OFFSET(struct RakServer_vtable, GetPlayerIDFromIndex, 0xF0);
STATIC_ASSERT_MEMBER_OFFSET(struct RakServer_vtable, GetStatistics, 0x118);
#endif
#pragma pack()
