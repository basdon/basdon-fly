EXPECT_SIZE(char, 1);
EXPECT_SIZE(short, 2);
EXPECT_SIZE(int, 4);

#pragma  pack(1)
struct BitStream {
	int numberOfBitsUsed;
	int numberOfBitsAllocated;
	int readOffset;
	void *ptrData;
	char copyData;
};

struct RPCDATA_ShowGangZone {
	short zoneid;
	float min_x;
	float min_y;
	float max_x;
	float max_y;
	int colorABGR; /*scripting func is RGBA but it's adjusted before RPC is sent*/
};
EXPECT_SIZE(struct RPCDATA_ShowGangZone, 2 + 16 + 4);

struct RPCDATA_HideGangZone {
	short zoneid;
};
EXPECT_SIZE(struct RPCDATA_HideGangZone, 2);

struct RPCDATA_SendDeathMessage {
	short killer;
	short killee;
	char weapon;
};
EXPECT_SIZE(struct RPCDATA_SendDeathMessage, 2 + 2 + 1);

struct RPCDATA_CreateObject {
	short objectid;
	int modelid;
	float x;
	float y;
	float z;
	float rotx;
	float roty;
	float rotz;
	float drawdistance;
	char no_camera_col;
	short attached_object;
	short attached_vehicle;
	float attach_offset_x;
	float attach_offset_y;
	float attach_offset_z;
	float attach_rotation_x;
	float attach_rotation_y;
	float attach_rotation_z;
	char sync_rotation;
};
EXPECT_SIZE(struct RPCDATA_CreateObject, 2 + 4 + 24 + 4 + 1 + 2 + 2 + 24 + 1);

struct RPCDATA_DestroyObject {
	short objectid;
};
EXPECT_SIZE(struct RPCDATA_DestroyObject, 2);

struct RPCDATA_RemoveBuilding {
	int model;
	float x;
	float y;
	float z;
	float radius;
};
EXPECT_SIZE(struct RPCDATA_RemoveBuilding, 4 + 12 + 4);

struct RPCDATA_MoveObject {
	short objectid;
	float from_x;
	float from_y;
	float from_z;
	float to_x;
	float to_y;
	float to_z;
	float speed;
	float to_rx;
	float to_ry;
	float to_rz;
};
EXPECT_SIZE(struct RPCDATA_MoveObject, 2 + 12 + 12 + 4 + 12);

struct RPCDATA_HideActor {
	short actorid;
};
EXPECT_SIZE(struct RPCDATA_HideActor, 2);

struct RPCDATA_ShowActor {
	short actorid;
	int modelid;
	float x;
	float y;
	float z;
	float angle;
	float health;
	char invulnerable;
};
EXPECT_SIZE(struct RPCDATA_ShowActor, 2 + 4 + 12 + 4 + 4 + 1);

struct RPCDATA_ShowTextDraw {
	short textdrawid;
	char flags;
	float font_width;
	float font_height;
	int font_color;
	float box_width;
	float box_height;
	int box_color;
	char shadow_size;
	char outline_size;
	int background_color;
	char font;
	char selectable;
	float x;
	float y;
	short preview_model;
	float preview_rot_x;
	float preview_rot_y;
	float preview_rot_z;
	float preview_zoom;
	short preview_color_1;
	short preview_color_2;
	short text_length;
	char text[1]; /*actually arbitrary size*/
};
EXPECT_SIZE(struct RPCDATA_ShowTextDraw, 2 + 1 + 8 + 4 + 8 + 4 + 1 + 1 + 4 + 1 + 1 + 8 + 2 + 12 + 4 + 4 + 2 + 1);

struct RPCDATA_HideTextDraw {
	short textdrawid;
};
EXPECT_SIZE(struct RPCDATA_HideTextDraw, 2);

static struct BitStream bitstream_freeform;
static union {
	char byte[16];
	short word[8];
	int dword[4];
} rpcdata_freeform;
#pragma pack()

#define SAMP_SendRPCToPlayer(pRPC,pBS,playerid,unk) \
	((void (*)(void*,void*,struct BitStream*,short,int))0x80AC1D0)((void*)samp_pNetGame,(void*)pRPC,pBS,playerid,unk)

#define RPC_ShowGangZone 0x81587A1 /*ptr to 0x6C(108)*/
#define RPC_HideGangZone 0x8163BA7 /*ptr to 0x78(120)*/
#define RPC_CreateObject 0x815A7F8 /*ptr to 0x2C(44)*/
#define RPC_DestroyObject 0x815ACC6 /*ptr to 0x2F(47)*/
#define RPC_RemoveBuilding 0x815CD7E /*ptr to 0x2B(43)*/
#define RPC_MoveObject 0x8158B64 /*ptr to 0x64(99)*/
#define RPC_HideActor 0x8157CCB /*ptr to 0xAC(172)*/
#define RPC_ShowActor 0x8157CC9 /*ptr to 0xAB(171)*/
#define RPC_ShowTextDraw 0x815AA74 /*ptr to 0x86(134)*/
#define RPC_HideTextDraw 0x815AA72 /*ptr to 0x87(135)*/

extern int samp_pNetGame;
