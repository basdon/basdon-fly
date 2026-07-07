#define LIMIT_VEHICLES_STREAMED_IN 700 /*copied from SAMP server*/
#define LIMIT_PLAYERS_STREAMED_IN 200 /*copied from SAMP server*/

static void samp_core_init();
/*doesn't check or set streaming state/limits, does trigger OnPlayerStreamIn effects*/
static void StreamPlayerIn(struct SampPlayer *player, struct SampPlayer *subjectplayer);
/*doesn't check or set streaming state/limits, does trigger OnPlayerStreamOut effects*/
static void StreamPlayerOut(struct SampPlayer *player, ushort subjectplayeridx);
/*doesn't check or set streaming state/limits, does trigger OnVehicleStreamIn effects*/
static void StreamVehicleIn(struct SampPlayer *player, struct SampVehicle *vehicle);
/*doesn't check or set streaming state/limits, does trigger OnVehicleStreamOut effects*/
static void StreamVehicleOut(struct SampPlayer *player, struct SampVehicle *vehicle);

#ifdef SAMP_CORE_IMPL
/*jeanine:p:i:4;p:0;a:b;y:1.88;n:StreamPlayerIn;*/
static
void StreamPlayerIn(struct SampPlayer *player, struct SampPlayer *subject)
{
	TRACE;
#pragma pack(push,1)
	union {
		struct RPCDATA_WorldPlayerAdd03DL rpcdata03DL;
		struct RPCDATA_WorldPlayerAdd037 rpcdata037;
		struct {
			int _pad0;
			struct RPCDATA_WorldPlayerAdd037 rpcdata037;
		} shifted;
	} data;
	EXPECT_SIZE(data, sizeof(struct RPCDATA_WorldPlayerAdd03DL));
#pragma pack(pop)
	struct RPCDATA_WorldPlayerAdd037 *writeptr;
	struct BitStream bs;

	bs.ptrData = &data;
	bs.numberOfBitsUsed = sizeof(struct RPCDATA_WorldPlayerAdd037);
	writeptr = &data.rpcdata037;
	writeptr->playerid = subject->playerid;
	writeptr->team = subject->spawnInfo.team;
	writeptr->skin = subject->spawnInfo.skin;
	if (is_player_using_client_version_DL[player->playerid]) {
		writeptr = &data.shifted.rpcdata037;
		/* This field is used for "custom skin".*/
		/*Brunoo16's packet list says to keep this 0 if it's not a default skin,*/
		/* but if I make either skin field 0, it always shows a CJ skin..*/
		writeptr->skin = subject->spawnInfo.skin;
		bs.numberOfBitsUsed = sizeof(struct RPCDATA_WorldPlayerAdd03DL);
	}
	writeptr->pos = subject->pos;
	writeptr->facingAngle = subject->facingAngle;
	writeptr->color = subject->color;
	memcpy(&writeptr->weaponSkill, subject->weaponSkill, sizeof(subject->weaponSkill));
	STATIC_ASSERT(sizeof(writeptr->weaponSkill) == sizeof(subject->weaponSkill));
	rakServerVtable->RPC_8C(rakServer, (void*) RPC_WorldPlayerAdd, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 2, rakPlayerID[player->playerid], 0, 0);

	/*SAMP here also sends RPC_SetPlayerAttachedObject for each slot but we currently don't use player attached objects*/

	/*OnPlayerStreamIn happens here*/
}
/*jeanine:p:i:3;p:4;a:b;y:1.88;n:StreamPlayerOut;*/
static
void StreamPlayerOut(struct SampPlayer *player, ushort subjectplayeridx)
{
	TRACE;
	struct RPCDATA_WorldPlayerRemove rpcdata;

	rpcdata.playerid = subjectplayeridx;
	SendRPC(player->playerid, RPC_WorldPlayerRemove, &rpcdata, sizeof(rpcdata) * 8);

	/*OnPlayerStreamOut happens here*/
}
/*jeanine:p:i:9;p:3;a:b;y:1.88;n:StreamVehicleIn;*/
static
void StreamVehicleIn(struct SampPlayer *player, struct SampVehicle *vehicle)
{
	TRACE;
	struct RPCDATA_CreateVehicle rpcCreate;
	struct RPCDATA_SetVehicleParamsEx rpcParams;
	ushort playerid = player->playerid;

	rpcCreate.vehicleid = vehicle->vehicleid;
	rpcCreate.model = vehicle->model;
	rpcCreate.pos.coords = vehicle->pos;
	if (
		/*For trains, the rotation defines the direction on the tracks*/
		/*(clockwise/counterclockwise), so this has to be the spawn rotation.*/
		vehicle->model != MODEL_FREIGHT && vehicle->model != MODEL_STREAK &&
		/*And atan only works with nonzero values. (skip FPU for a quicker check)*/
		*(int*) &vehicle->matrix.up.y & 0x7FFFFFFF && *(int*) &vehicle->matrix.up.x & 0x7FFFFFFF
	) {
		rpcCreate.pos.r = CalcVehicleStreamInAngle(vehicle->matrix.up.x, vehicle->matrix.up.y);
	} else {
		rpcCreate.pos.r = vehicle->spawnPos.r;
	}
	rpcCreate.vehicleid = vehicle->vehicleid;
	rpcCreate.spawnColor1 = vehicle->spawnColor1;
	rpcCreate.spawnColor2 = vehicle->spawnColor2;
	rpcCreate.health = vehicle->health;
	rpcCreate.interior = vehicle->interior;
	rpcCreate.damagestatus = vehicle->damageStatus;
	rpcCreate.siren = vehicle->use_siren;
	memcpy(rpcCreate.modslots, vehicle->modslots, sizeof(vehicle->modslots));
	STATIC_ASSERT(sizeof(rpcCreate.modslots) == sizeof(vehicle->modslots));
	rpcCreate.paintjob = vehicle->paintjob; /*TODO: we could include paintjob/modcolors into VehicleMods*/
	rpcCreate.moddedColor1 = vehicle->moddedColor1;
	rpcCreate.moddedColor2 = vehicle->moddedColor2;
	SendRPC(playerid, RPC_CreateVehicle, &rpcCreate, sizeof(rpcCreate) * 8);

	if (~vehicle->params.raw0 | ~vehicle->params.raw1 | ~vehicle->params.raw2 | ~vehicle->params.raw3) {
		rpcParams.vehicleid = vehicle->vehicleid;
		rpcParams.params = vehicle->params;
		SendRPC(playerid, RPC_SetVehicleParamsEx, &rpcParams, sizeof(rpcParams) * 8);
	}

	/*OnVehicleStreamIn happens here*/
	missions_on_vehicle_stream_in(vehicle->vehicleid, playerid);
	veh_on_vehicle_stream_in(vehicle->vehicleid, playerid);
}
/*jeanine:p:i:7;p:9;a:b;y:1.88;n:StreamVehicleOut;*/
static
void StreamVehicleOut(struct SampPlayer *player, struct SampVehicle *vehicle)
{
	TRACE;
	struct RPCDATA_DeleteVehicle rpcDelete;
	ushort playerid = player->playerid;

	rpcDelete.vehicleid = vehicle->vehicleid;
	SendRPC(playerid, RPC_DeleteVehicle, &rpcDelete, sizeof(rpcDelete) * 8);

	/*OnVehicleStreamOut happens here*/
	veh_on_vehicle_stream_out(vehicle->vehicleid, playerid);
}
/*jeanine:p:i:10;p:5;a:r;x:3.75;n:StreamVehicleOutAfterDeletion;*/
/*hooked function so this signature cannot change*/
static
void StreamVehicleOutAfterDeletion(struct SampPlayerPool *this, ushort vehicleid)
{
	TRACE;
	struct SampPlayer *player;
	struct SampVehicle *vehicle = vehiclepool->vehicles[vehicleid];
	register int i;

	for (i = playerpool->highestUsedPlayerid; i >= 0; i--) {
		player = sampPlayer[i];
		if (player && player->vehicleStreamedIn[vehicleid]) {
			player->vehicleStreamedIn[vehicleid] = 0;
			player->numVehiclesStreamedIn--;
			StreamVehicleOut(player, vehicle);
		}
	}
}
/*jeanine:p:i:5;p:7;a:b;y:1.88;n:samp_core_init;*/
static
void samp_core_init()
{
	TRACE;

	samp = sampPlugins->GetSamp();
	samp_pConsole = sampPlugins->GetSampConsole();
	playerpool = samp->playerPool;
	vehiclepool = samp->vehiclePool;
	rakServer = samp->rakServer; /*also exposed at sampPlugins->GetRakServer()*/
	rakServerVtable = rakServer->vtable;
	rakRPC_8C = rakServerVtable->RPC_8C;

	mem_mkjmp(0x80CAC70, crash__this_codepath_should_be_unreachable); /*SampPlayer::StreamInForOtherPlayer*/
	mem_mkjmp(0x80CAF00, crash__this_codepath_should_be_unreachable); /*SampPlayer::StreamInPlayer*/
	mem_mkjmp(0x80CAFC0, crash__this_codepath_should_be_unreachable); /*SampPlayer::StreamOutPlayer*/
	mem_mkjmp(0x80D0EC0, crash__this_codepath_should_be_unreachable); /*SampPlayerPool::StreamInPlayer*/
	mem_mkjmp(0x80D0F00, crash__this_codepath_should_be_unreachable); /*SampPlayerPool::StreamOutPlayer*/
	mem_mkjmp(0x80C9CA0, crash__this_codepath_should_be_unreachable); /*SampPlayer::StreamInVehicle*/
	mem_mkjmp(0x80C9BF0, crash__this_codepath_should_be_unreachable); /*SampPlayer::StreamOutVehicle*/
	mem_mkjmp(0x814CD70, crash__this_codepath_should_be_unreachable); /*SampVehiclePool::VehicleCreateForPlayer*/
	mem_mkjmp(0x814D1A0, crash__this_codepath_should_be_unreachable); /*SampVehiclePool::VehicleDeleteForPlayer*/
	mem_mkjmp(0x80D1480, &StreamVehicleOutAfterDeletion); /*SampPlayerPool::DeleteVehicle, called from SampVehicle::dtor and SampVehicle::Respawn*//*jeanine:r:i:10;*/
}

#endif /*SAMP_CORE_IMPL*/
