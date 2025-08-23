EXPECT_SIZE(char, 1);
EXPECT_SIZE(short, 2);
EXPECT_SIZE(int, 4);

#pragma pack(1)
struct Quaternion {
	float w, x, y, z;
};
EXPECT_SIZE(struct Quaternion, 0x10);

struct SampMatrix {
	struct vec3 right; /*unsure*/
	float _14;
	struct vec3 up; /*unsure*/
	float _24;
	struct vec3 at; /*unsure*/
	float _34;
	struct vec3 pos;
	float _44;
};
EXPECT_SIZE(struct SampMatrix, 0x40);

struct SampVehicleParamsDoorsWindows {
	char frontleft; /*-1 unset, 0 closed, 1 open*/
	char frontright; /*-1 unset, 0 closed, 1 open*/
	char backleft; /*-1 unset, 0 closed, 1 open*/
	char backright; /*-1 unset, 0 closed, 1 open*/
};
EXPECT_SIZE(struct SampVehicleParamsDoorsWindows, 0x4);

struct SampVehicleParams {
	char engine; /*0 off, 1 on*/
	char lights; /*0 off, 1 on*/
	char alarm; /*0 off, 1 on (or it was on and has finished)*/
	char doors_locked; /*0 unlocked, 1 locked*/
	char bonnet; /*0 closed, 1 open*/
	char boot; /*0 closed, 1 open*/
	char objective;
	char siren_state; /*-1 unset (off), 0 off, 1 on*/
	struct SampVehicleParamsDoorsWindows doors;
	struct SampVehicleParamsDoorsWindows windows;
};
EXPECT_SIZE(struct SampVehicleParams, 0x10);

#define DOOR_STATE_OPENED 0x1
#define DOOR_STATE_DAMAGED 0x2
#define DOOR_STATE_REMOVED 0x4

#define PANEL_STATE_DAMAGED 0x1
#define PANEL_STATE_VERYDAMAGED 0x2
#define PANEL_STATE_REMOVED 0x3

#define LIGHT_STATE_BROKEN_FRONT_LEFT 0x1
#define LIGHT_STATE_BROKEN_FRONT_RIGHT 0x4
#define LIGHT_STATE_BROKEN_BACK 0x40

#define TIRE_STATE_POPPED_BACK_RIGHT 0x1
#define TIRE_STATE_POPPED_FRONT_RIGHT 0x2
#define TIRE_STATE_POPPED_BACK_LEFT 0x4
#define TIRE_STATE_POPPED_FRONT_LEFT 0x8

struct SampVehicleDamageStatus {
	union {
		unsigned int raw;
		struct {
			unsigned char passenger;
			unsigned char driver;
			unsigned char trunk;
			unsigned char hood;
		} structured;
	} doors;
	union {
		unsigned int raw;
		struct {
			unsigned char front_left : 4;
			unsigned char front_right : 4;
			unsigned char read_left : 4;
			unsigned char read_right : 4;
			unsigned char windshield : 4;
			unsigned char front_bumper : 4;
			unsigned char rear_bumper : 4;
			unsigned char unk : 4;
		} structured;
	} panels;
	char broken_lights;
	union {
		unsigned char raw;
		struct {
			unsigned char back_right : 1;
			unsigned char front_right : 1;
			unsigned char back_left : 1;
			unsigned char front_left : 1;
			unsigned char padding : 4;
		} structured;
	} popped_tires;
};
EXPECT_SIZE(struct SampVehicleDamageStatus, 4 + 4 + 1 + 1);

struct SYNCDATA_Onfoot {
	short lrkey;
	short udkey;
	/*combine with additional_keys*/
	short partial_keys;
	struct vec3 pos;
	float quat_w;
	float quat_x;
	float quat_y;
	float quat_z;
	char player_hp;
	char player_armor;
	unsigned char weapon_id : 6;
	/*two extra bits for YES/NO keys*/
	unsigned char additional_keys : 2;
	char special_action;
	struct vec3 vel;
	struct vec3 surfing_offset;
	short surfing_vehicle_id;
	short animation_id;
	short animation_flags;
};
EXPECT_SIZE(struct SYNCDATA_Onfoot, 2 + 2 + 2 + 12 + 16 + 1 + 1 + 1 + 1 + 12 + 12 + 2 + 2 + 2);
EXPECT_SIZE(struct SYNCDATA_Onfoot, 0x44);

struct SYNCDATA_Driver {
	short vehicle_id;
	/*when player is not controllable, this is actually from foot controls??*/
	short lrkey;
	/*when player is not controllable, this is actually from foot controls??*/
	/*this is steer forward/down backward/up*/
	short udkey;
	/*when player is not controllable, this is actually from foot controls??*/
	/*combine with additional_keys*/
	short partial_keys;
	float quat_w;
	float quat_x;
	float quat_y;
	float quat_z;
	struct vec3 pos;
	struct vec3 vel;
	float vehicle_hp;
	char player_hp;
	char player_armor;
	unsigned char weapon_id : 6;
	/*two extra bits for YES/NO keys*/
	unsigned char additional_keys : 2;
	char siren_state;
	char landing_gear_state; /*0 down 1 up (or transitioning)*/
	short trailer_id;
	int misc; /*data depending on vehicle model*/
};
EXPECT_SIZE(struct SYNCDATA_Driver, 2 + 2 + 2 + 2 + 16 + 12 + 12 + 4 + 1 + 1 + 1 + 1 + 1 + 2 + 4);
EXPECT_SIZE(struct SYNCDATA_Driver, 0x3F);

struct SYNCDATA_Passenger {
	short vehicle_id;
	unsigned char seat_id : 6;
	unsigned char drive_by : 2;
	unsigned char weapon_id : 6;
	/*two extra bits for YES/NO keys*/
	unsigned char additional_keys : 2;
	char player_hp;
	char player_armor;
	short lrkey;
	short udkey;
	short partial_keys;
	struct vec3 pos;
};
EXPECT_SIZE(struct SYNCDATA_Passenger, 2 + 1 + 1 + 1 + 1 + 2 + 2 + 2 + 12);
EXPECT_SIZE(struct SYNCDATA_Passenger, 0x18);

struct SampVehicle {
	struct vec3 pos;
	struct SampMatrix matrix;
	struct vec3 vel;
	int field_58[3];
	short vehicleid;
	int _pad66;
	short driverplayerid;
	char _pad6C[0x82-0x6C];
	int model;
	struct vec4 spawnPos;
	/*Primary color, set when creating the vehicle. This color will be used when respawning. See moddedColor1.*/
	int spawnColor1;
	/*Primary color, set when creating the vehicle. This color will be used when respawning. See moddedColor2.*/
	int spawnColor2;
	int respawn_delay_ms;
	int interior;
	float health;
	struct SampVehicleDamageStatus damageStatus;
	char _padB4;
	short _padB5;
	char modslots[14];
	char paintjob;
	/*Primary color after ChangeVehicleColor or TransFender garage has been used.
	When not set yet, this is -1. This gets reset to -1 when the vehicle is respawned.
	A value of -1 means the spawnColor1 will be used instead.*/
	int moddedColor1;
	/*Secondary color after ChangeVehicleColor or TransFender garage has been used.
	When not set yet, this is -1. This gets reset to -1 when the vehicle is respawned.
	A value of -1 means the spawnColor2 will be used instead.*/
	int moddedColor2;
	char numberplate[33]; /*32 + 1 pad*/
	struct SampVehicleParams params;
	char _padFF[0x101-0xFF];
	int lastRespawnTickCount; /*GetTickCount() at time of last respawn*/
	int lastSpawnTickCount; /*GetTickCount() at time of last (re)spawn*/
	char use_siren;
	char _pad10A;
};
EXPECT_SIZE(struct SampVehicle, 0x10B); /*Complete.*/
STATIC_ASSERT_MEMBER_OFFSET(struct SampVehicle, vehicleid, 0x64);
STATIC_ASSERT_MEMBER_OFFSET(struct SampVehicle, model, 0x82);
STATIC_ASSERT_MEMBER_OFFSET(struct SampVehicle, respawn_delay_ms, 0x9E);
STATIC_ASSERT_MEMBER_OFFSET(struct SampVehicle, health, 0xA6);
STATIC_ASSERT_MEMBER_OFFSET(struct SampVehicle, damageStatus, 0xAA);
STATIC_ASSERT_MEMBER_OFFSET(struct SampVehicle, paintjob, 0xC5);
STATIC_ASSERT_MEMBER_OFFSET(struct SampVehicle, params, 0xEF);
STATIC_ASSERT_MEMBER_OFFSET(struct SampVehicle, use_siren, 0x109);

struct SpawnInfo {
	char team;
	int skin;
	char _pad5;
	struct vec4 pos;
	int weapon[3];
	int ammo[3];
};
EXPECT_SIZE(struct SpawnInfo, 0x2E); /*Complete.*/

struct SpawnInfo03DL {
	char team;
	int skin;
	int customSkin; /*keep this 0*/
	char _spawnInfoPad5;
	struct vec4 pos;
	int weapon[3];
	int ammo[3];
};
EXPECT_SIZE(struct SpawnInfo03DL, 0x32);

#define UPDATE_SYNC_TYPE_NONE 0
#define UPDATE_SYNC_TYPE_ONFOOT 1
#define UPDATE_SYNC_TYPE_INCAR 2
#define UPDATE_SYNC_TYPE_PASSENGER 3

struct SampPlayer {
	char _pad0[0x27];
	struct SYNCDATA_Driver driverSyncData;
	struct SYNCDATA_Passenger passengerSyncData;
	struct SYNCDATA_Onfoot onfootSyncData;
	char _padC2[0x155-0xC2];
	char playerStreamedIn[1000]; /*Does this get reset when player disconnects?*/
	char vehicleStreamedIn[2000];
	char _padD0D[1000];
	char textLabelCreated[1024];
	char pickupCreated[4096];
	char actorStreamedIn[1000];
	int numPlayersStreamedIn; /**Samp doesn't stream in new players if this is 200.*/
	int numVehiclesStreamedIn;
	int _pad28E5;
	int numTextLabelsStreamedIn;
	char _pad28ED[0x2915-0x28ED];
	struct vec3 pos;
	float hp;
	float armor;
	char _pad2929[0x2939-0x2929];
	float facingAngle;
	struct vec3 onfootvelocity;
	short lrkey;
	short udkey;
	int keys;
	int _pad2951;
	int isEditingObject;
	int _pad2959;
	short lastSentDialogID;
#ifdef __TINYC__
	int textdrawpool;
	int textlabelPool;
#else
	void *textdrawpool;
	void *textlabelPool;
#endif
	short playerid;
	int updateSyncType;
	int _pad296D[130];
	int _pad2B75[10];
	int isAiming;
	int hasTrailerToSync;
	int _pad2BA5;
	char currentState;
	struct vec3 checkpointPos;
	float checkpointSize;
	int isCurrentlyInCheckpoint;
	struct vec3 raceCheckpointPos;
	struct vec3 raceCheckpointNextPos;
	char raceCheckpointType;
	float raceCheckpointSize;
	int isCurrentlyInRaceCheckpoint;
	int _field2BDF;
	short weaponSkill[11];
	int _field2BF9;
	struct SpawnInfo spawnInfo;
	int hasSpawnInfo;
	char _pad2C2F;
	char fightingstyle;
	char vehicleseat;
	short vehicleid;
	int color;
	int isCheckpointEnabled;
	int isRaceCheckpointEnabled;
	int interior;
	int _field2C44[13];
	char _field2C78[13];
	char _field2C85;
	short _field2C86;
	short _field2C88;
	int _field2C8A;
	char _field2C8E;
	int _field2C8F[10];
	char _field2CB7;
	int _field2CB8;
	char _field2CBC;
	int _field2CBD;
	int lastStreamingTick;
	int _field2CC5;
	int _field2CC9;
	int _field2CCD;
	char _field2CD1;
	char _pad2CD2[8];
	int _field2CDA;
};
EXPECT_SIZE(struct SampPlayer, 0x2CDE); /*Complete.*/
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayer, driverSyncData, 0x27);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayer, passengerSyncData, 0x66);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayer, onfootSyncData, 0x7E);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayer, playerStreamedIn, 0x155);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayer, vehicleStreamedIn, 0x53D);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayer, actorStreamedIn, 0x24F5);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayer, numPlayersStreamedIn, 0x28DD);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayer, numVehiclesStreamedIn, 0x28E1);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayer, pos, 0x2915);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayer, hp, 0x2921);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayer, armor, 0x2925);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayer, facingAngle, 0x2939);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayer, lrkey, 0x2949);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayer, udkey, 0x294B);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayer, keys, 0x294D);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayer, lastSentDialogID, 0x295D);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayer, playerid, 0x2967);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayer, updateSyncType, 0x2969);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayer, currentState, 0x2BA9);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayer, raceCheckpointType, 0x2BD6);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayer, isCurrentlyInRaceCheckpoint, 0x2BDB);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayer, spawnInfo, 0x2BFD);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayer, hasSpawnInfo, 0x2C2B);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayer, fightingstyle, 0x2C30);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayer, vehicleseat, 0x2C31);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayer, vehicleid, 0x2C32);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayer, color, 0x2C34);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayer, interior, 0x2C40);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayer, _field2CDA, 0x2CDA);

struct SampVehiclePool {
	char _pad0[0xD4];
	int virtualworld[2000];
	int created[2000];
	struct SampVehicle *vehicles[2000];
	int poolsize;
	/*Incomplete.*/
};
STATIC_ASSERT_MEMBER_OFFSET(struct SampVehiclePool, created, 0x2014);
STATIC_ASSERT_MEMBER_OFFSET(struct SampVehiclePool, vehicles, 0x3F54);

struct SampPlayerPool {
	int virtualworld[1000];
	int _padFA0[2003];
	int playerDrunkLevel[1000];
	int _pad3E8C[1000];
	char gpci[1000][101];
	char version[1000][25];
	int playerSlotState_[1000]; /*?unsure*/
	int created[1000];
	struct SampPlayer *players[1000];
	char names[1000][25];
	int isAdmin[1000];
	int isNpc[1000];
	/*Incomplete.*/
};
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayerPool, playerDrunkLevel, 0x2EEC);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayerPool, gpci, 0x4E2C);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayerPool, version, 0x1D8B4);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayerPool, playerSlotState_, 0x23A5C);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayerPool, created, 0x249FC);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayerPool, players, 0x2599C);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayerPool, names, 0x2693C);
STATIC_ASSERT_MEMBER_OFFSET(struct SampPlayerPool, isAdmin, 0x2CAE4);

struct SampNetGame {
	void *pGameMode;
	void *pFilterScripts;
	struct SampPlayerPool *playerPool;
	struct SampVehiclePool *vehiclePool;
	int _pad10;
	void *objectPool;
	int _pad18[2];
	void *textLabelPool;
	void *gangzonePool;
	void *actorPool;
	int _pad2C[5];
	struct RakServer *rakServer;
	int _pad44[5];
	char showNametags; /*Should be set before anyone connects.*/
	char _pad59;
	char _pad5A;
	char enableStuntBonus; /*EnableStuntBonusForAll()*/
	int _pad5C;
	int _pad60;
	int _pad64;
	int _pad68;
	char _pad6C;
	char usePlayerPedAnims; /*UsePlayerPedAnims()*/
	char _pad6E;
	char _pad6F;
	char _pad70;
	char _pad71;
	float nametagDrawDistance; /*SetNameTagDrawDistance()*/
	char _pad76[0x8A-0x76];
	int numAvailableSpawns; /*AddPlayerClass()*/
	/*Incomplete.*/
};
STATIC_ASSERT_MEMBER_OFFSET(struct SampNetGame, vehiclePool, 0xC);
STATIC_ASSERT_MEMBER_OFFSET(struct SampNetGame, objectPool, 0x14);
STATIC_ASSERT_MEMBER_OFFSET(struct SampNetGame, textLabelPool, 0x20);
STATIC_ASSERT_MEMBER_OFFSET(struct SampNetGame, gangzonePool, 0x24);
STATIC_ASSERT_MEMBER_OFFSET(struct SampNetGame, actorPool, 0x28);
STATIC_ASSERT_MEMBER_OFFSET(struct SampNetGame, rakServer, 0x40);
STATIC_ASSERT_MEMBER_OFFSET(struct SampNetGame, showNametags, 0x58);
STATIC_ASSERT_MEMBER_OFFSET(struct SampNetGame, enableStuntBonus, 0x5B);
STATIC_ASSERT_MEMBER_OFFSET(struct SampNetGame, usePlayerPedAnims, 0x6D);
STATIC_ASSERT_MEMBER_OFFSET(struct SampNetGame, nametagDrawDistance, 0x72);
STATIC_ASSERT_MEMBER_OFFSET(struct SampNetGame, numAvailableSpawns, 0x8A);

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

struct RPCDATA_CreateObject_Attached {
	short objectid;
	int modelid;
	float x;
	float y;
	float z;
	float rx;
	float ry;
	float rz;
	float drawdistance;
	char no_camera_col;
	short attached_object_id;
	short attached_vehicle_id;
	float attach_offset_x;
	float attach_offset_y;
	float attach_offset_z;
	float attach_rotation_x;
	float attach_rotation_y;
	float attach_rotation_z;
	char sync_rotation;
	char num_materials;
	/*material data*/
};
EXPECT_SIZE(struct RPCDATA_CreateObject_Attached, 2 + 4 + 24 + 4 + 1 + 2 + 2 + 24 + 1 + 1);

struct RPCDATA_CreateObject {
	short objectid;
	int modelid;
	float x;
	float y;
	float z;
	float rx;
	float ry;
	float rz;
	float drawdistance;
	char no_camera_col;
	short attached_object_id;
	short attached_vehicle_id;
	char num_materials;
	/*material data*/
};
EXPECT_SIZE(struct RPCDATA_CreateObject, 2 + 4 + 24 + 4 + 1 + 2 + 2 + 1);

struct RPCDATA_CreateObject_Material {
	char material_type;
	char material_index;
};
EXPECT_SIZE(struct RPCDATA_CreateObject_Material, 1 + 1);

/*
struct material_texture {
	short model_id;
	char txdname_len;
	char txdname[?];
	char texture_len;
	char texture[?];
	int material_color;
};
*/

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

struct RPCDATA_ShowActor037 {
	short actorid;
	int skin;
	float x;
	float y;
	float z;
	float angle;
	float hp;
	char invulnerable;
};
EXPECT_SIZE(struct RPCDATA_ShowActor037, 2 + 4 + 12 + 4 + 4 + 1);

struct RPCDATA_ShowActor03DL {
	short actorid;
	int skin;
	int customSkin;
	float x;
	float y;
	float z;
	float angle;
	float hp;
	char invulnerable;
};
EXPECT_SIZE(struct RPCDATA_ShowActor03DL, 2 + 4 + 4 + 12 + 4 + 4 + 1);

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

struct RPCDATA_TextDrawSetString {
	short textdrawid;
	short text_length;
	char text[800]; /*actually artibrary size*/
};
EXPECT_SIZE(struct RPCDATA_TextDrawSetString, 2 + 2 + 800);

struct RPCDATA_SendClientMessage {
	int color;
	int message_length;
	char message[144];
};
EXPECT_SIZE(struct RPCDATA_SendClientMessage, 4 + 4 + 144);

struct RPCDATA_PlaySound {
	int soundid;
	struct vec3 pos;
};
EXPECT_SIZE(struct RPCDATA_PlaySound, 4 + 12);

struct RPCDATA_SetWorldBounds {
	float x_max;
	float x_min;
	float y_max;
	float y_min;
};
EXPECT_SIZE(struct RPCDATA_SetWorldBounds, 4 + 4 + 4 + 4);

struct RPCDATA_ShowGameText {
	int style;
	int time;
	int message_length;
	char message[800];
};
EXPECT_SIZE(struct RPCDATA_ShowGameText, 4 + 4 + 4 + 800);

struct RPCDATA_SetCheckpoint {
	struct vec3 pos;
	float size;
};
EXPECT_SIZE(struct RPCDATA_SetCheckpoint, 12 + 4);

struct RPCDATA_SetRaceCheckpoint {
	char type;
	struct vec3 pos;
	struct vec3 pointAt;
	float radius;
};
EXPECT_SIZE(struct RPCDATA_SetRaceCheckpoint, 1 + 12 + 12 + 4);

struct RPCDATA_SetVehicleParams {
	short vehicleid;
	char objective;
	char doors_locked;
};
EXPECT_SIZE(struct RPCDATA_SetVehicleParams, 2 + 1 + 1);

struct RPCDATA_SetVehicleParamsEx {
	short vehicleid;
	struct SampVehicleParams params;
};
EXPECT_SIZE(struct RPCDATA_SetVehicleParamsEx, 2 + 0x10);

struct RPCDATA_SetPlayerMapIcon {
	char icon_id;
	struct vec3 pos;
	char icon;
	int color;
	char style;
};
EXPECT_SIZE(struct RPCDATA_SetPlayerMapIcon, 1 + 12 + 1 + 4 + 1);

struct RPCDATA_RemovePlayerMapIcon {
	char icon_id;
};
EXPECT_SIZE(struct RPCDATA_RemovePlayerMapIcon, 1);

/*Base because it lacks the text data.*/
struct RPCDATA_Create3DTextLabelBase {
	short label_id;
	int color;
	struct vec3 pos; /*Offsets when it's attached.*/
	float draw_distance;
	char test_los;
	short attached_player_id;
	short attached_vehicle_id;
	/*Huffman encoded string here (preceded by bit length of encoded string, compressed somehow...).*/
};
EXPECT_SIZE(struct RPCDATA_Create3DTextLabelBase, 2 + 4 + 12 + 4 + 1 + 2 + 2);

struct RPCDATA_Delete3DTextLabel {
	short label_id;
};
EXPECT_SIZE(struct RPCDATA_Delete3DTextLabel, 2);

struct RPCDATA_RequestSpawn {
	int type; /*??. 2 is used for SpawnPlayer(). other values may be in response to client requesting spawn?*/
};
EXPECT_SIZE(struct RPCDATA_RequestSpawn, 4);

struct RPCDATA_CreateVehicle {
	short vehicleid;
	int model;
	struct vec4 pos;
	char spawnColor1; /*Note how spawnColorX is 4 bytes in SampVehicle, but 1 byte here & moddedColorX is 1 byte / 4 bytes...!?*/
	char spawnColor2;
	float health;
	char interior;
	struct SampVehicleDamageStatus damagestatus;
	char siren;
	char modslots[14];
	char paintjob;
	int moddedColor1;
	int moddedColor2;
};
EXPECT_SIZE(struct RPCDATA_CreateVehicle, 2 + 4 + 16 + 1 + 1 + 4 + 1 + 10 + 1 + 14 + 1 + 4 + 4);

struct RPCDATA_DeleteVehicle {
	short vehicleid;
};
EXPECT_SIZE(struct RPCDATA_DeleteVehicle, 2);

struct RPCDATA_SetVehicleNumberplate {
	short vehicleid;
	char text_length;
	char text[1]; /*actually arbitrary size*/
};
EXPECT_SIZE(struct RPCDATA_SetVehicleNumberplate, 2 + 1 + 1);

struct RPCDATA_SetVehiclePos {
	short vehicleid;
	struct vec3 pos;
};
EXPECT_SIZE(struct RPCDATA_SetVehiclePos, 2 + 12);

struct RPCDATA_SetPlayerPos {
	struct vec3 pos;
};
EXPECT_SIZE(struct RPCDATA_SetPlayerPos, 12);

struct RPCDATA_PlayerJoin {
	short playerid;
	int unkAlwaysOne;
	/**NPCs don't have nametags and are not shown in the TAB list (scoreboard?),
	NPCs also have different physics, they will barely fall when hanging in the sky.*/
	char npc;
	char namelen;
	char name[1]; /*actually arbitrary size with max length 24*/
};
EXPECT_SIZE(struct RPCDATA_PlayerJoin, 2 + 4 + 1 + 1 + 1);

struct RPCDATA_PlayerLeave {
	short playerid;
	char reason; /*0 timeout 1 quit 2 kick*/
};
EXPECT_SIZE(struct RPCDATA_PlayerLeave, 2 + 1);

/*See 0x80CAC70 int StreamInPlayer(struct SampPlayer *theplayer, short forplayerid)*/
struct RPCDATA_PlayerCreate {
	short playerid;
	char team;
	int skin;
	float x;
	float y;
	float z;
	float facingAngle;
	int colorRGBA;
	char fightingstyle;
};
EXPECT_SIZE(struct RPCDATA_PlayerCreate, 2 + 1 + 4 + 12 + 4 + 4 + 1);

struct RPCDATA_PutPlayerInVehicle {
	short vehicleid;
	char seat;
};
EXPECT_SIZE(struct RPCDATA_PutPlayerInVehicle, 2 + 1);

struct RPCDATA_RequestClass037 {
	char response; /*0 disallows the class. if 0, spawnInfo may be omitted*/
	struct SpawnInfo spawnInfo;
};
EXPECT_SIZE(struct RPCDATA_RequestClass037, 0x2F);

struct RPCDATA_RequestClass03DL {
	char response; /*0 disallows the class. if 0, spawnInfo may be omitted*/
	struct SpawnInfo03DL spawnInfo;
};
EXPECT_SIZE(struct RPCDATA_RequestClass03DL, 0x33);

struct RPCDATA_SetSpawnInfo037 {
	struct SpawnInfo spawnInfo;
};
EXPECT_SIZE(struct RPCDATA_SetSpawnInfo037, 0x2E);

struct RPCDATA_SetSpawnInfo03DL {
	struct SpawnInfo03DL spawnInfo;
};
EXPECT_SIZE(struct RPCDATA_SetSpawnInfo03DL, 0x32);

struct RPCDATA_WorldPlayerAdd037 {
	short playerid;
	char team;
	int skin;
	struct vec3 pos;
	float facingAngle;
	int color;
	char fightingstyle;
	short weaponSkill[11];
};
EXPECT_SIZE(struct RPCDATA_WorldPlayerAdd037, 0x32);

struct RPCDATA_WorldPlayerAdd03DL {
	short playerid;
	char team;
	int skin;
	int customSkin;
	struct vec3 pos;
	float facingAngle;
	int color;
	char fightingstyle;
	short weaponSkill[11];
};
EXPECT_SIZE(struct RPCDATA_WorldPlayerAdd03DL, 0x36);

/**
DriverSync
	char packet_id; (200)
	short playerid;
	short vehicleid;
	short lrkey;
	short udkey;
	short partial_keys;
		rotation: see 80CE880
		bit: 1 if w<0
		bit: 1 if x<0
		bit: 1 if y<0
		bit: 1 if z<0
		short abs(x)*65535.0f
		short abs(y)*65535.0f
		short abs(z)*7.5f
	float x;
	float y;
	float z;
		velocity compressed
	short vehicle_health;
	char health_armor; (TODO)
	char weapon_id_additional_keys;
	char siren_state : 1;
	char landing_gear_state : 1;
	char has_misc : 1;
		int misc; (if has_misc)
	char has_trailer_id: 1;
		short trailer_id (if has_trailer_id)
*/
#pragma pack()

/*this uses HIGH_PRIORITY
 *if orderingChannel is 3, reliability will be RELIABLE, else RELIABLE_ORDERED
 *most usages seem to be using orderingChannel 2*/
#define SAMP_SendRPCToPlayer(pRPC,pBS,playerid,orderingChannel) \
	((void (*)(void*,void*,struct BitStream*,short,int))0x80AC1D0)((void*)samp_pNetGame,(void*)pRPC,pBS,playerid,orderingChannel)

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
#define RPC_TogglePlayerControllable 0x815CCFA /*ptr to 0x0F(15)*/
#define RPC_SendClientMessage 0x815A027 /*ptr to 0x5D(93)*/
#define RPC_PlaySound 0x815CD0C /*ptr to 0x10(16)*/
#define RPC_ShowGameText 0x815A19A /*ptr to 0x49(73)*/
#define RPC_SetWorldBounds 0x815CCB8 /*ptr to 0x11(17)*/
#define RPC_TextDrawSetString 0x815214C /*ptr to 0x69(105)*/
#define RPC_SetRaceCheckpoint 0x815CD7A /*ptr to 0x26(38)*/
#define RPC_DisableRaceCheckpoint 0x81587C7 /*ptr to 0x27(39)*/
#define RPC_SetCheckpoint 0x81639A5 /*ptr to 0x6B(107)*/
#define RPC_DisableCheckpoint 0x815A860 /*ptr to 0x25(37) (rpc has no data)*/
#define RPC_SetVehicleParams 0x815CCFC /*ptr to 0xA1(161)*/
#define RPC_SetVehicleParamsEx 0x8166226 /*ptr to 0x18(24)*/
#define RPC_Create3DTextLabel 0x815072C /*ptr to 0x24(36)*/
#define RPC_SetPlayerMapIcon 0x8162E9F /*ptr to 0x38(56)*/
#define RPC_RemovePlayerMapIcon 0x815CD22 /*ptr to 0x90(144)*/
#define RPC_Delete3DTextLabel 0x8159960 /*ptr to 0x3A(58)*/
#define RPC_DisableRemoteVehicleCollisions 0x815CD80 /*ptr to 0xA7(167)*/
#define RPC_ShowDialog 0x815CD7C /*ptr to 0x3D(61)*/
#define RPC_RequestSpawn 0x815A61E /*ptr to 0x81(129)*/
#define RPC_CreateVehicle 0x8166228 /*ptr to 0xA4(164)*/
#define RPC_DeleteVehicle 0x816622C /*ptr to 0xA5(165)*/
#define RPC_SetVehicleNumberplate 0x816622A /*ptr to  0x7B(123)*/
#define RPC_SetVehiclePos 0x8166224 /*ptr to 0x9F(159)*/
#define RPC_SetPlayerPos 0x815CCEC /*ptr to 0xC(12)*/
#define RPC_PlayerJoin 0x815AA76 /*ptr to 0x89(137)*/
#define RPC_PlayerCreate 0x816324E /*ptr to 0x20(32)*/
#define RPC_PlayerLeave 0x815AA78 /*ptr to 0x8A(138)*/
#define RPC_PutPlayerInVehicle 0x815A058 /*ptr to 0x46(70)*/
#define RPC_RequestClass 0x81572DF /*ptr to 0x80(128)*/
#define RPC_SetSpawnInfo 0x8162624 /*ptr to 0x44(68)*/
#define RPC_WorldPlayerAdd 0x816324E /*ptr to 0x20(32)*/
