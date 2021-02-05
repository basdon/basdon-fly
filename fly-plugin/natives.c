/* Natives ending in _ means that they have
custom code and shouldn't be used directly.*/

static AMX_NATIVE n_AddPlayerClass;
static AMX_NATIVE n_AddStaticVehicleEx;
static AMX_NATIVE n_ChangeVehicleColor;
static AMX_NATIVE n_ClearAnimations;
static AMX_NATIVE n_CreateObject;
static AMX_NATIVE n_CreatePlayerObject;
static AMX_NATIVE n_CreateVehicle_;
static AMX_NATIVE n_DestroyObject;
static AMX_NATIVE n_DestroyPlayerObject;
static AMX_NATIVE n_DestroyVehicle_;
static AMX_NATIVE n_DisablePlayerCheckpoint;
static AMX_NATIVE n_DisablePlayerRaceCheckpoint;
static AMX_NATIVE n_ForceClassSelection;
static AMX_NATIVE n_GameTextForPlayer;
static AMX_NATIVE n_GetConsoleVarAsInt;
static AMX_NATIVE n_GetPlayerIp;
static AMX_NATIVE n_GetPlayerName;
static AMX_NATIVE n_GetPlayerPing;
static AMX_NATIVE n_GetPlayerScore;
static AMX_NATIVE n_GetServerTickRate;
static AMX_NATIVE n_GetVehicleDamageStatus;
static AMX_NATIVE n_GetVehicleHealth_;
static AMX_NATIVE n_GetVehiclePoolSize;
static AMX_NATIVE n_GivePlayerMoney_;
static AMX_NATIVE n_GivePlayerWeapon;
static AMX_NATIVE n_IsValidVehicle;
static AMX_NATIVE n_Kick_;
static AMX_NATIVE n_MoveObject;
static AMX_NATIVE n_PutPlayerInVehicle_;
static AMX_NATIVE n_RemoveBuildingForPlayer;
static AMX_NATIVE n_ResetPlayerMoney_;
static AMX_NATIVE n_RepairVehicle;
static AMX_NATIVE n_SHA256_PassHash;
static AMX_NATIVE n_SendRconCommand;
static AMX_NATIVE n_SetCameraBehindPlayer;
static AMX_NATIVE n_SetPlayerCameraPos;
static AMX_NATIVE n_SetPlayerCameraLookAt;
static AMX_NATIVE n_SetPlayerColor;
static AMX_NATIVE n_SetPlayerFacingAngle;
static AMX_NATIVE n_SetPlayerHealth;
static AMX_NATIVE n_SetPlayerName_;
static AMX_NATIVE n_SetPlayerPos_;
static AMX_NATIVE n_SetPlayerScore;
static AMX_NATIVE n_SetPlayerSpecialAction;
static AMX_NATIVE n_SetPlayerTime;
static AMX_NATIVE n_SetPlayerWeather;
static AMX_NATIVE n_SetSpawnInfo;
static AMX_NATIVE n_SetVehicleHealth;
static AMX_NATIVE n_SetVehiclePos;
static AMX_NATIVE n_SetVehicleToRespawn;
static AMX_NATIVE n_SetVehicleZAngle;
static AMX_NATIVE n_ShowPlayerDialog_;
static AMX_NATIVE n_SpawnPlayer_;
static AMX_NATIVE n_UpdateVehicleDamageStatus;
static AMX_NATIVE n_TogglePlayerClock;
static AMX_NATIVE n_TogglePlayerControllable;
static AMX_NATIVE n_TogglePlayerSpectating;
static AMX_NATIVE n_bcrypt_check;
static AMX_NATIVE n_bcrypt_get_hash;
static AMX_NATIVE n_bcrypt_hash;
static AMX_NATIVE n_bcrypt_is_equal;
static AMX_NATIVE n_cache_delete;
static AMX_NATIVE n_cache_get_row;
static AMX_NATIVE n_cache_get_row_count;
static AMX_NATIVE n_cache_get_row_float;
static AMX_NATIVE n_cache_get_row_int;
static AMX_NATIVE n_cache_insert_id;
static AMX_NATIVE n_gpci;
static AMX_NATIVE n_mysql_connect;
static AMX_NATIVE n_mysql_close;
static AMX_NATIVE n_mysql_errno;
static AMX_NATIVE n_mysql_escape_string;
static AMX_NATIVE n_mysql_log;
static AMX_NATIVE n_mysql_query;
static AMX_NATIVE n_mysql_tquery;
static AMX_NATIVE n_mysql_unprocessed_queries;
static AMX_NATIVE n_ssocket_connect;
static AMX_NATIVE n_ssocket_create;
static AMX_NATIVE n_ssocket_destroy;
static AMX_NATIVE n_ssocket_listen;
static AMX_NATIVE n_ssocket_send;
static AMX_NATIVE n_random;
static AMX_NATIVE n_tickcount;

#define n_cache_get_field_s n_cache_get_row
#define n_cache_get_field_i n_cache_get_row_int
#define n_cache_get_field_f n_cache_get_row_float

/*samp0.3 doesn't seem to check for param sizes on non-varargs functions*/
#define NO_NC_PARAM_SIZE

#ifdef NO_NC_PARAM_SIZE
#define NC_PARS(X)
#define NC_PARS_(X)
#else
#define NC_PARS(X) nc_params[0]=X<<2
#define NC_PARS_(X) nc_params[0]=X<<2,
#endif /*NO_NC_PARAM_SIZE*/

union NCDATA {
	cell asint[20];
	float asflt[20];
};
/**
nc_data
*/
static union NCDATA nc_data;
#define nc_params nc_data.asint
#define nc_paramf nc_data.asflt
/**
Used to reinterpret cell return values as a float.
*/
static cell tmpfloat;

#define SOCKET_TCP 1
#define SOCKET_UDP 2
#define SOCKET_INVALID_SOCKET -1

/* NC = NativeCall */

#define NC(NATIVE) NATIVE(amx,nc_params)
#define NCF(NATIVE) (tmpfloat=NATIVE(amx,nc_params),amx_ctof(tmpfloat))

#define NC_ClearAnimations(PLAYERID,FORCESYNC) \
	(NC_PARS_(2)nc_params[1]=PLAYERID,nc_params[2]=FORCESYNC,\
	n_ClearAnimations(amx,nc_params))

#define CreateVehicle __USE__veh_CreateVehicle__

#define NC_DestroyObject(OBJECTID) (NC_PARS_(1)nc_params[1]=OBJECTID,\
	n_DestroyObject(amx,nc_params))

#define NC_DestroyPlayerObject(PLAYERID,OBJECTID) \
	(NC_PARS_(2)nc_params[1]=PLAYERID,nc_params[2]=OBJECTID,\
	n_DestroyPlayerObject(amx,nc_params))

#define NC_DestroyVehicle __USE__veh_DestroyVehicle__

#define NC_DisablePlayerRaceCheckpoint(PLAYERID) \
	(NC_PARS_(1)nc_params[1]=PLAYERID,\
	n_DisablePlayerRaceCheckpoint(amx,nc_params))

#define NC_ForceClassSelection(PLAYERID) \
	(NC_PARS_(1)nc_params[1]=PLAYERID,\
	n_ForceClassSelection(amx,nc_params))

#define NC_GameTextForPlayer(PLAYERID,BUF,LENGTH,STYLE) \
	(NC_PARS_(4)nc_params[1]=PLAYERID,nc_params[2]=BUF,\
	nc_params[3]=LENGTH,nc_params[4]=STYLE,\
	n_GameTextForPlayer(amx,nc_params))

#define NC_GetConsoleVarAsInt(BUF) (NC_PARS_(1)nc_params[1]=BUF,\
	n_GetConsoleVarAsInt(amx,nc_params))

#define NC_GetPlayerIp(PLAYERID,BUF,LEN) (NC_PARS_(3)\
	nc_params[1]=PLAYERID,nc_params[2]=BUF,nc_params[3]=LEN,\
	n_GetPlayerIp(amx,nc_params))

#define NC_GetPlayerName(PLAYERID,BUF,LEN) (NC_PARS_(3)\
	nc_params[1]=PLAYERID,nc_params[2]=BUF,nc_params[3]=LEN,\
	n_GetPlayerName(amx,nc_params))

#define NC_GetPlayerPing(PLAYERID) (NC_PARS_(1)nc_params[1]=PLAYERID,\
	n_GetPlayerPing(amx,nc_params))

#define NC_GetPlayerScore(PLAYERID) (NC_PARS_(1)nc_params[1]=PLAYERID,\
	n_GetPlayerScore(amx,nc_params))

#define NC_GetServerTickRate() (NC_PARS_(0)\
	n_GetServerTickRate(amx,nc_params))

#define NC_GetVehicleHealth __USE__anticheat_GetVehicleHealth__

#define NC_GetVehiclePoolSize() (NC_PARS_(0)\
	n_GetVehiclePoolSize(amx,nc_params))

#define NC_GivePlayerWeapon(PLAYERID,WEAPONID,AMMO) (NC_PARS_(3)\
	nc_params[1]=PLAYERID,nc_params[2]=WEAPONID,nc_params[3]=AMMO,\
	n_GivePlayerWeapon(amx,nc_params))

#define NC_IsValidVehicle(VEHICLEID) (NC_PARS_(1)nc_params[1]=VEHICLEID,\
	n_IsValidVehicle(amx,nc_params))

#define NC_Kick(PLAYERID) __USE__natives_Kick__

#define NC_PutPlayerInVehicle __USE__natives_PutPlayerInVehicle__

#define NC_SHA256_PassHash(INPUT,SALT,DEST,MAXLEN) \
	(NC_PARS_(4)nc_params[1]=INPUT,nc_params[2]=SALT,\
	nc_params[3]=DEST,nc_params[4]=MAXLEN,\
	n_SHA256_PassHash(amx,nc_params))

#define NC_SendRconCommand(BUF) (NC_PARS_(1)nc_params[1]=BUF,\
	n_SendRconCommand(amx,nc_params))

#define NC_SetCameraBehindPlayer(PLAYERID) \
	(NC_PARS_(1)nc_params[1]=PLAYERID,\
	n_SetCameraBehindPlayer(amx,nc_params))

#define NC_SetPlayerFacingAngle(PLAYERID,FR) (NC_PARS_(2)\
	nc_params[1]=PLAYERID,nc_params[2]=FR,\
	n_SetPlayerFacingAngle(amx,nc_params))

#define NC_SetPlayerHealth(PLAYERID,FHEALTH) \
	(NC_PARS_(2)nc_params[1]=PLAYERID,nc_paramf[2]=FHEALTH,\
	n_SetPlayerHealth(amx,nc_params))

#define NC_SetPlayerPos __USE__natives_SetPlayerPos__

#define NC_SetPlayerScore(PLAYERID,SCORE) \
	(NC_PARS_(2)nc_params[1]=PLAYERID,nc_params[2]=SCORE,\
	n_SetPlayerScore(amx,nc_params))

#define NC_SetPlayerSpecialAction(PLAYERID,ACTION) \
	(NC_PARS_(2)nc_params[1]=PLAYERID,nc_params[2]=ACTION,\
	n_SetPlayerSpecialAction(amx,nc_params))

#define NC_SetPlayerTime(PLAYERID,H,M) \
	(NC_PARS_(3)nc_params[1]=PLAYERID,nc_params[2]=H,nc_params[3]=M,\
	n_SetPlayerTime(amx,nc_params))

#define NC_SetPlayerWeather(PLAYERID,WEATHER) \
	(NC_PARS_(2)nc_params[1]=PLAYERID,nc_params[2]=WEATHER,\
	n_SetPlayerWeather(amx,nc_params))

#define NC_SetVehicleHealth(VEHICLEID,HP) \
	(NC_PARS_(2)nc_params[1]=VEHICLEID,nc_paramf[2]=HP,\
	n_SetVehicleHealth(amx,nc_params))

#define NC_SetVehicleToRespawn(VEHICLEID) \
	(NC_PARS_(1)nc_params[1]=VEHICLEID,\
	n_SetVehicleToRespawn(amx,nc_params))

#define NC_SetVehicleZAngle(VEHICLEID,FROT) \
	(NC_PARS_(2)nc_params[1]=VEHICLEID,nc_paramf[2]=FROT,\
	n_SetVehicleZAngle(amx,nc_params))

#define NC_ShowPlayerDialog __USE__dialog_ShowPlayerDialog__

#define NC_SpawnPlayer __USE__natives_SpawnPlayer__

#define NC_TogglePlayerClock(PLAYERID,FLAG) \
	(NC_PARS_(2)nc_params[1]=PLAYERID,nc_params[2]=FLAG,\
	n_TogglePlayerClock(amx,nc_params))

#define NC_TogglePlayerControllable(PLAYERID,FLAG) \
	(NC_PARS_(2)nc_params[1]=PLAYERID,nc_params[2]=FLAG,\
	n_TogglePlayerControllable(amx,nc_params))

#define NC_TogglePlayerSpectating(PLAYERID,FLAG) \
	(NC_PARS_(2)nc_params[1]=PLAYERID,nc_params[2]=FLAG,\
	n_TogglePlayerSpectating(amx,nc_params))

/*bcrypt_get_hash fails when argument size does not match*/
#define NC_bcrypt_get_hash(BUF) (nc_params[0]=4,nc_params[1]=BUF,\
	n_bcrypt_get_hash(amx,nc_params))

/*bcrypt_is_equal doesn't check argument size*/
#define NC_bcrypt_is_equal() (NC_PARS_(0)\
	n_bcrypt_is_equal(amx,nc_params))

#define NC_cache_delete(ID) (NC_PARS_(2)nc_params[1]=ID,nc_params[2]=1,\
	n_cache_delete(amx,nc_params))

#define NC_cache_get_row_count() (NC_PARS_(1)nc_params[1]=1,\
	n_cache_get_row_count(amx,nc_params))

#define NC_cache_get_field_int(ROW,COL) (NC_PARS_(2)\
	nc_params[1]=ROW,nc_params[2]=COL,\
	n_cache_get_row_int(amx,nc_params))

#define NC_cache_get_field_flt(ROW,COL) (NC_PARS_(2)\
	nc_params[1]=ROW,nc_params[2]=COL,\
	NCF(n_cache_get_row_float))

#define NC_cache_get_field_str(ROW,COL,BUF) (NC_PARS_(3)\
	nc_params[1]=ROW,nc_params[2]=COL,nc_params[3]=BUF,\
	n_cache_get_row(amx,nc_params))

#define NC_cache_insert_id() (NC_PARS_(0)\
	n_cache_insert_id(amx,nc_params))

#define NC_gpci(PLAYERID,BUF,LEN) (NC_PARS_(3)\
	nc_params[1]=PLAYERID,nc_params[2]=BUF,nc_params[3]=LEN,\
	n_gpci(amx,nc_params))

#define NC_mysql_errno() (NC_PARS_(1)nc_params[1]=1,\
	n_mysql_errno(amx,nc_params))

#define NC_mysql_query(BUF) \
	(NC_PARS_(3)nc_params[1]=1,nc_params[2]=BUF,nc_params[3]=1,\
	n_mysql_query(amx,nc_params))

#define _NC_mysql_tquery_nocb(BUF) (NC_PARS_(4)nc_params[1]=1,\
	nc_params[2]=BUF,nc_params[3]=nc_params[4]=emptystringa,\
	n_mysql_tquery(amx,nc_params))

#ifdef PRINTQUERIES
#define NC_mysql_tquery_nocb(BUF) DEBUG_NC_mysql_tquery_nocb(BUF)
#else
#define NC_mysql_tquery_nocb(BUF) _NC_mysql_tquery_nocb(BUF)
#endif

#define NC_random(MAX) (NC_PARS_(1)nc_params[1]=MAX,\
	n_random(amx,nc_params))

#define NC_ssocket_connect(SOCKET,HOST,PORT) (NC_PARS_(3)\
	nc_params[1]=SOCKET,nc_params[2]=HOST,nc_params[3]=PORT,\
	n_ssocket_connect(amx,nc_params))

#define NC_ssocket_create(TYPE) (NC_PARS_(1)nc_params[1]=TYPE,\
	n_ssocket_create(amx,nc_params))

#define NC_ssocket_destroy(SOCKET) (NC_PARS_(1)nc_params[1]=SOCKET,\
	n_ssocket_destroy(amx,nc_params))

#define NC_ssocket_listen(SOCKET,PORT) (NC_PARS_(2)\
	nc_params[1]=SOCKET,nc_params[2]=PORT,\
	n_ssocket_listen(amx,nc_params))

#define NC_ssocket_send(SOCKET,DATA,LEN) (NC_PARS_(3)\
	nc_params[1]=SOCKET,nc_params[2]=DATA,nc_params[3]=LEN,\
	n_ssocket_send(amx,nc_params))

#define NC_tickcount() (NC_PARS_(0)\
	n_tickcount(amx,nc_params))

/**
Loads natives.
*/
static
int natives_find()
{
#define N(X) {#X,(int*)&n_##X}
#define N_(X) {#X,(int*)&n_##X##_}
	struct NATIVE {
		char *name;
		int *var;
	};
	struct NATIVE natives[] = {
		N(AddPlayerClass),
		N(AddStaticVehicleEx),
		N(ChangeVehicleColor),
		N(ClearAnimations),
		N(CreateObject),
		N(CreatePlayerObject),
		N_(CreateVehicle),
		N(DestroyObject),
		N(DestroyPlayerObject),
		N_(DestroyVehicle),
		N(DisablePlayerCheckpoint),
		N(DisablePlayerRaceCheckpoint),
		N(ForceClassSelection),
		N(GameTextForPlayer),
		N(GetConsoleVarAsInt),
		N(GetPlayerIp),
		N(GetPlayerName),
		N(GetPlayerPing),
		N(GetPlayerScore),
		N(GetServerTickRate),
		N(GetVehicleDamageStatus),
		N_(GetVehicleHealth),
		N(GetVehiclePoolSize),
		N_(GivePlayerMoney),
		N(GivePlayerWeapon),
		N(IsValidVehicle),
		N_(Kick),
		N(MoveObject),
		N_(PutPlayerInVehicle),
		N(RemoveBuildingForPlayer),
		N(RepairVehicle),
		N_(ResetPlayerMoney),
		N(SHA256_PassHash),
		N(SendRconCommand),
		N(SetCameraBehindPlayer),
		N(SetPlayerCameraPos),
		N(SetPlayerCameraLookAt),
		N(SetPlayerColor),
		N(SetPlayerFacingAngle),
		N(SetPlayerHealth),
		N_(SetPlayerName),
		N_(SetPlayerPos),
		N(SetPlayerScore),
		N(SetPlayerSpecialAction),
		N(SetPlayerTime),
		N(SetPlayerWeather),
		N(SetSpawnInfo),
		N(SetVehicleHealth),
		N(SetVehiclePos),
		N(SetVehicleToRespawn),
		N(SetVehicleZAngle),
		N_(ShowPlayerDialog),
		N_(SpawnPlayer),
		N(TogglePlayerClock),
		N(TogglePlayerSpectating),
		N(TogglePlayerControllable),
		N(UpdateVehicleDamageStatus),
		N(bcrypt_check),
		N(bcrypt_get_hash),
		N(bcrypt_hash),
		N(bcrypt_is_equal),
		N(cache_delete),
		N(cache_get_row),
		N(cache_get_row_count),
		N(cache_get_row_float),
		N(cache_get_row_int),
		N(cache_insert_id),
		N(gpci),
		N(mysql_connect),
		N(mysql_close),
		N(mysql_errno),
		N(mysql_escape_string),
		N(mysql_log),
		N(mysql_query),
		N(mysql_tquery),
		N(mysql_unprocessed_queries),
		N(random),
		N(ssocket_connect),
		N(ssocket_create),
		N(ssocket_destroy),
		N(ssocket_listen),
		N(ssocket_send),
		N(tickcount),
	};
	struct NATIVE *n = natives + sizeof(natives)/sizeof(struct NATIVE);
	AMX_HEADER *header;
	AMX_FUNCSTUB *func;
	unsigned char *nativetable;
	int nativesize;
	int idx;

	header = (AMX_HEADER*) amx->base;
	nativetable = (unsigned char*) header + header->natives;
	nativesize = (int) header->defsize;

	while (n-- != natives) {
		if (amx_FindNative(amx, n->name, &idx) == AMX_ERR_NOTFOUND) {
			logprintf("ERR: no %s native", n->name);
			return 0;
		}
		func = (AMX_FUNCSTUB*) (nativetable + idx * nativesize);
		*n->var = func->address;
	}
	return 1;
}
