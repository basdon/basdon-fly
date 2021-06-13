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
	int _pad0[60];
	void (*GetPlayerIDFromIndex)(struct PlayerID *outPlayerID, struct RakServer *rakServer, short playerIndex);
	int _padF4[9];
	struct RakNetStatistics *(*GetStatistics)(struct RakServer *rakServer, struct PlayerID playerID);
};
STATIC_ASSERT_MEMBER_OFFSET(struct RakServer_vtable, GetPlayerIDFromIndex, 0xF0);
STATIC_ASSERT_MEMBER_OFFSET(struct RakServer_vtable, GetStatistics, 0x118);
#pragma pack()