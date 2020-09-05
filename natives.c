/* Natives ending in _ means that they have
custom code and shouldn't be used directly.*/

static AMX_NATIVE n_AddPlayerClass;
static AMX_NATIVE n_AddStaticVehicleEx;
static AMX_NATIVE n_ChangeVehicleColor;
static AMX_NATIVE n_ClearAnimations;
static AMX_NATIVE n_CreateObject;
static AMX_NATIVE n_CreatePlayer3DTextLabel;
static AMX_NATIVE n_CreatePlayerObject;
static AMX_NATIVE n_CreatePlayerTextDraw;
static AMX_NATIVE n_CreateVehicle_;
static AMX_NATIVE n_DeletePlayer3DTextLabel;
static AMX_NATIVE n_DestroyObject;
static AMX_NATIVE n_DestroyPlayerObject;
static AMX_NATIVE n_DestroyVehicle_;
static AMX_NATIVE n_DisablePlayerCheckpoint;
static AMX_NATIVE n_DisablePlayerRaceCheckpoint;
static AMX_NATIVE n_EnableStuntBonusForAll;
static AMX_NATIVE n_ForceClassSelection;
static AMX_NATIVE n_GameTextForPlayer;
static AMX_NATIVE n_GetConsoleVarAsInt;
static AMX_NATIVE n_GetPlayerFacingAngle;
static AMX_NATIVE n_GetPlayerIp;
static AMX_NATIVE n_GetPlayerKeys;
static AMX_NATIVE n_GetPlayerName;
static AMX_NATIVE n_GetPlayerPing;
static AMX_NATIVE n_GetPlayerPos;
static AMX_NATIVE n_GetPlayerScore;
static AMX_NATIVE n_GetPlayerState;
static AMX_NATIVE n_GetPlayerVehicleID;
static AMX_NATIVE n_GetPlayerVehicleSeat;
static AMX_NATIVE n_GetServerTickRate;
static AMX_NATIVE n_GetVehicleDamageStatus;
static AMX_NATIVE n_GetVehicleHealth_;
static AMX_NATIVE n_GetVehicleModel;
static AMX_NATIVE n_GetVehicleParamsEx;
static AMX_NATIVE n_GetVehiclePos;
static AMX_NATIVE n_GetVehiclePoolSize;
static AMX_NATIVE n_GetVehicleRotationQuat;
static AMX_NATIVE n_GetVehicleVelocity;
static AMX_NATIVE n_GetVehicleZAngle;
static AMX_NATIVE n_GivePlayerMoney_;
static AMX_NATIVE n_GivePlayerWeapon;
static AMX_NATIVE n_IsValidVehicle;
static AMX_NATIVE n_IsVehicleStreamedIn;
static AMX_NATIVE n_Kick_;
static AMX_NATIVE n_MoveObject;
static AMX_NATIVE n_PlayerPlaySound;
static AMX_NATIVE n_PlayerTextDrawAlignment;
static AMX_NATIVE n_PlayerTextDrawBackgroundColor;
static AMX_NATIVE n_PlayerTextDrawBoxColor;
static AMX_NATIVE n_PlayerTextDrawColor;
static AMX_NATIVE n_PlayerTextDrawDestroy;
static AMX_NATIVE n_PlayerTextDrawFont;
static AMX_NATIVE n_PlayerTextDrawHide;
static AMX_NATIVE n_PlayerTextDrawLetterSize;
static AMX_NATIVE n_PlayerTextDrawSetOutline;
static AMX_NATIVE n_PlayerTextDrawSetProportional;
static AMX_NATIVE n_PlayerTextDrawSetShadow;
static AMX_NATIVE n_PlayerTextDrawSetString;
static AMX_NATIVE n_PlayerTextDrawShow;
static AMX_NATIVE n_PlayerTextDrawTextSize;
static AMX_NATIVE n_PlayerTextDrawUseBox;
static AMX_NATIVE n_PutPlayerInVehicle_;
static AMX_NATIVE n_RemoveBuildingForPlayer;
static AMX_NATIVE n_ResetPlayerMoney_;
static AMX_NATIVE n_RepairVehicle;
static AMX_NATIVE n_RemovePlayerMapIcon;
static AMX_NATIVE n_SHA256_PassHash;
static AMX_NATIVE n_SendClientMessage;
static AMX_NATIVE n_SendClientMessageToAll;
static AMX_NATIVE n_SendRconCommand;
static AMX_NATIVE n_SetCameraBehindPlayer;
static AMX_NATIVE n_SetGameModeText;
static AMX_NATIVE n_SetPlayerCameraPos;
static AMX_NATIVE n_SetPlayerCameraLookAt;
static AMX_NATIVE n_SetPlayerColor;
static AMX_NATIVE n_SetPlayerFacingAngle;
static AMX_NATIVE n_SetPlayerHealth;
static AMX_NATIVE n_SetPlayerMapIcon;
static AMX_NATIVE n_SetPlayerName_;
static AMX_NATIVE n_SetPlayerPos_;
static AMX_NATIVE n_SetPlayerRaceCheckpoint;
static AMX_NATIVE n_SetPlayerScore;
static AMX_NATIVE n_SetPlayerSpecialAction;
static AMX_NATIVE n_SetPlayerTime;
static AMX_NATIVE n_SetPlayerWeather;
static AMX_NATIVE n_SetSpawnInfo;
static AMX_NATIVE n_SetVehicleHealth;
static AMX_NATIVE n_SetVehicleParamsEx;
static AMX_NATIVE n_SetVehicleParamsForPlayer;
static AMX_NATIVE n_SetVehiclePos;
static AMX_NATIVE n_SetVehicleToRespawn;
static AMX_NATIVE n_SetVehicleZAngle;
static AMX_NATIVE n_ShowPlayerDialog_;
static AMX_NATIVE n_SpawnPlayer_;
static AMX_NATIVE n_TextDrawAlignment;
static AMX_NATIVE n_TextDrawBoxColor;
static AMX_NATIVE n_TextDrawColor;
static AMX_NATIVE n_TextDrawCreate;
static AMX_NATIVE n_TextDrawFont;
static AMX_NATIVE n_TextDrawHideForPlayer;
static AMX_NATIVE n_TextDrawLetterSize;
static AMX_NATIVE n_TextDrawSetOutline;
static AMX_NATIVE n_TextDrawSetProportional;
static AMX_NATIVE n_TextDrawSetShadow;
static AMX_NATIVE n_TextDrawShowForPlayer;
static AMX_NATIVE n_TextDrawTextSize;
static AMX_NATIVE n_UpdateVehicleDamageStatus;
static AMX_NATIVE n_UsePlayerPedAnims;
static AMX_NATIVE n_TextDrawUseBox;
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

#define NC_DeletePlayer3DTextLabel(PLAYERID,TXTID) \
	(NC_PARS_(2)nc_params[1]=PLAYERID,nc_params[2]=TXTID,\
	n_DeletePlayer3DTextLabel(amx,nc_params))

#define NC_DestroyObject(OBJECTID) (NC_PARS_(1)nc_params[1]=OBJECTID,\
	n_DestroyObject(amx,nc_params))

#define NC_DestroyPlayerObject(PLAYERID,OBJECTID) \
	(NC_PARS_(2)nc_params[1]=PLAYERID,nc_params[2]=OBJECTID,\
	n_DestroyPlayerObject(amx,nc_params))

#define NC_DestroyVehicle __USE__veh_DestroyVehicle__

#define NC_DisablePlayerRaceCheckpoint(PLAYERID) \
	(NC_PARS_(1)nc_params[1]=PLAYERID,\
	n_DisablePlayerRaceCheckpoint(amx,nc_params))

#define NC_EnableStuntBonusForAll(FLAG) (NC_PARS_(1)nc_params[1]=FLAG,\
	n_EnableStuntBonusForAll(amx,nc_params))

#define NC_ForceClassSelection(PLAYERID) \
	(NC_PARS_(1)nc_params[1]=PLAYERID,\
	n_ForceClassSelection(amx,nc_params))

#define NC_GameTextForPlayer(PLAYERID,BUF,LENGTH,STYLE) \
	(NC_PARS_(4)nc_params[1]=PLAYERID,nc_params[2]=BUF,\
	nc_params[3]=LENGTH,nc_params[4]=STYLE,\
	n_GameTextForPlayer(amx,nc_params))

#define NC_GetConsoleVarAsInt(BUF) (NC_PARS_(1)nc_params[1]=BUF,\
	n_GetConsoleVarAsInt(amx,nc_params))

#define NC_GetPlayerFacingAngle(PLAYERID,FANGLE) (NC_PARS_(2)\
	nc_params[1]=PLAYERID,nc_params[2]=FANGLE,\
	n_GetPlayerFacingAngle(amx,nc_params))

#define NC_GetPlayerIp(PLAYERID,BUF,LEN) (NC_PARS_(3)\
	nc_params[1]=PLAYERID,nc_params[2]=BUF,nc_params[3]=LEN,\
	n_GetPlayerIp(amx,nc_params))

#define NC_GetPlayerName(PLAYERID,BUF,LEN) (NC_PARS_(3)\
	nc_params[1]=PLAYERID,nc_params[2]=BUF,nc_params[3]=LEN,\
	n_GetPlayerName(amx,nc_params))

#define NC_GetPlayerPing(PLAYERID) (NC_PARS_(1)nc_params[1]=PLAYERID,\
	n_GetPlayerPing(amx,nc_params))

#define NC_GetPlayerPos(PLAYERID,FREFX,FREFY,FREFZ) (NC_PARS_(4)\
	nc_params[1]=PLAYERID,nc_params[2]=FREFX,nc_params[3]=FREFY,\
	nc_params[4]=FREFZ,\
	n_GetPlayerPos(amx,nc_params))

#define NC_GetPlayerScore(PLAYERID) (NC_PARS_(1)nc_params[1]=PLAYERID,\
	n_GetPlayerScore(amx,nc_params))

#define NC_GetPlayerState(PLAYERID) (NC_PARS_(1)nc_params[1]=PLAYERID,\
	n_GetPlayerState(amx,nc_params))

#define NC_GetPlayerVehicleID(PLAYERID) \
	(NC_PARS_(1)nc_params[1]=PLAYERID,\
	n_GetPlayerVehicleID(amx,nc_params))

#define NC_GetPlayerVehicleSeat(PLAYERID) \
	(NC_PARS_(1)nc_params[1]=PLAYERID,\
	n_GetPlayerVehicleSeat(amx,nc_params))

#define NC_GetServerTickRate() (NC_PARS_(0)\
	n_GetServerTickRate(amx,nc_params))

#define NC_GetVehicleHealth __USE__anticheat_GetVehicleHealth__

#define NC_GetVehicleModel(VEHICLEID) \
	(NC_PARS_(1)nc_params[1]=VEHICLEID,\
	n_GetVehicleModel(amx,nc_params))

#define NC_GetVehicleParamsEx(VID,ENGINE,LIGHTS,ALARM,DOORS,BONNET,BOOT,OBJ) \
	(NC_PARS_(8)nc_params[1]=VID,nc_params[2]=ENGINE,\
	nc_params[3]=LIGHTS,nc_params[4]=ALARM,nc_params[5]=DOORS,\
	nc_params[6]=BONNET,nc_params[7]=BOOT,nc_params[8]=OBJ,\
	n_GetVehicleParamsEx(amx,nc_params))

#define NC_GetVehiclePos(VEHICLEID,FX,FY,FZ) \
	(NC_PARS_(4)nc_params[1]=VEHICLEID,\
	nc_params[2]=FX,nc_params[3]=FY,nc_params[4]=FZ,\
	n_GetVehiclePos(amx,nc_params))

#define NC_GetVehicleRotationQuat(VEHICLEID,FQW,FQX,FQY,FQZ) \
	(NC_PARS_(5)nc_params[1]=VEHICLEID,nc_params[2]=FQW,\
	nc_params[3]=FQX,nc_params[4]=FQY,nc_params[5]=FQZ,\
	n_GetVehicleRotationQuat(amx,nc_params))

#define NC_GetVehicleVelocity(VEHICLEID,FX,FY,FZ) \
	(NC_PARS_(4)nc_params[1]=VEHICLEID,\
	nc_params[2]=FX,nc_params[3]=FY,nc_params[4]=FZ,\
	n_GetVehicleVelocity(amx,nc_params))

#define NC_GetVehicleZAngle(VEHICLEID,FR) \
	(NC_PARS_(2)nc_params[1]=VEHICLEID,nc_params[2]=FR,\
	n_GetVehicleZAngle(amx,nc_params))

#define NC_GetVehiclePoolSize() (NC_PARS_(0)\
	n_GetVehiclePoolSize(amx,nc_params))

#define NC_GivePlayerWeapon(PLAYERID,WEAPONID,AMMO) (NC_PARS_(3)\
	nc_params[1]=PLAYERID,nc_params[2]=WEAPONID,nc_params[3]=AMMO,\
	n_GivePlayerWeapon(amx,nc_params))

#define NC_IsValidVehicle(VEHICLEID) (NC_PARS_(1)nc_params[1]=VEHICLEID,\
	n_IsValidVehicle(amx,nc_params))

#define NC_Kick(PLAYERID) __USE__natives_Kick__

#define NC_PlayerPlaySound(PLAYERID,SOUNDID,FX,FY,FZ) \
	(NC_PARS_(5)nc_params[1]=PLAYERID,nc_params[2]=SOUNDID,\
	nc_params[3]=FX,nc_params[4]=FY,nc_params[5]=FZ,\
	n_PlayerPlaySound(amx,nc_params))

/*same but at location 0, 0, 0*/
#define NC_PlayerPlaySound0(PLAYERID,SOUNDID) \
	(NC_PARS_(5)nc_params[1]=PLAYERID,nc_params[2]=SOUNDID,\
	nc_params[3]=nc_params[4]=nc_params[5]=0,\
	n_PlayerPlaySound(amx,nc_params))

#define NC_PlayerTextDrawAlignment(PLAYERID,TD,ALIGNMENT) (NC_PARS_(3)\
	nc_params[1]=PLAYERID,nc_params[2]=TD,nc_params[3]=ALIGNMENT,\
	n_PlayerTextDrawAlignment(amx,nc_params))

#define NC_PlayerTextDrawColor(PLAYERID,TD,COLOR) (NC_PARS_(3)\
	nc_params[1]=PLAYERID,nc_params[2]=TD,nc_params[3]=COLOR,\
	n_PlayerTextDrawColor(amx,nc_params))

#define NC_PlayerTextDrawFont(PLAYERID,TD,FONT) (NC_PARS_(3)\
	nc_params[1]=PLAYERID,nc_params[2]=TD,nc_params[3]=FONT,\
	n_PlayerTextDrawFont(amx,nc_params))

#define NC_PlayerTextDrawHide(PLAYERID,TD) \
	(NC_PARS_(2)nc_params[1]=PLAYERID,nc_params[2]=TD,\
	n_PlayerTextDrawHide(amx,nc_params))

#define NC_PlayerTextDrawLetterSize(PLAYERID,TD,FX,FY) (NC_PARS_(3)\
	nc_params[1]=PLAYERID,nc_params[2]=TD,\
	nc_params[3]=FX,nc_params[4]=FY\
	n_PlayerTextDrawLetterSize(amx,nc_params))

#define NC_PlayerTextDrawOutline(PLAYERID,TD,OUTLINE) (NC_PARS_(3)\
	nc_params[1]=PLAYERID,nc_params[2]=TD,nc_params[3]=OUTLINE,\
	n_PlayerTextDrawOutline(amx,nc_params))

#define NC_PlayerTextDrawProportional(PLAYERID,TD,PROP) (NC_PARS_(3)\
	nc_params[1]=PLAYERID,nc_params[2]=TD,nc_params[3]=PROP,\
	n_PlayerTextDrawProportional(amx,nc_params))

#define NC_PlayerTextDrawSetShadow(PLAYERID,TD,SHADOW) (NC_PARS_(3)\
	nc_params[1]=PLAYERID,nc_params[2]=TD,nc_params[3]=SHADOW,\
	n_PlayerTextDrawSetShadow(amx,nc_params))

#define NC_PlayerTextDrawSetString(PLAYERID,TD,BUF) (NC_PARS_(3)\
	nc_params[1]=PLAYERID,nc_params[2]=TD,nc_params[3]=BUF,\
	n_PlayerTextDrawSetString(amx,nc_params))

#define NC_PlayerTextDrawShow(PLAYERID,TD) \
	(NC_PARS_(2)nc_params[1]=PLAYERID,nc_params[2]=TD,\
	n_PlayerTextDrawShow(amx,nc_params))

#define NC_PutPlayerInVehicle __USE__natives_PutPlayerInVehicle__

#define NC_RemovePlayerMapIcon(PLAYERID,MAPICONID) (NC_PARS_(2)\
	nc_params[1]=PLAYERID,nc_params[2]=MAPICONID,\
	n_RemovePlayerMapIcon(amx,nc_params))

#define NC_SHA256_PassHash(INPUT,SALT,DEST,MAXLEN) \
	(NC_PARS_(4)nc_params[1]=INPUT,nc_params[2]=SALT,\
	nc_params[3]=DEST,nc_params[4]=MAXLEN,\
	n_SHA256_PassHash(amx,nc_params))

#define NC_SendClientMessage(PLAYERID,COLOR,BUF) \
	(NC_PARS_(3)\
	nc_params[1]=PLAYERID,nc_params[2]=COLOR,nc_params[3]=BUF,\
	n_SendClientMessage(amx,nc_params))

#define NC_SendClientMessageToAll(COLOR,BUF) \
	(NC_PARS_(2)nc_params[1]=COLOR,nc_params[2]=BUF,\
	n_SendClientMessageToAll(amx,nc_params))

#define NC_SendRconCommand(BUF) (NC_PARS_(1)nc_params[1]=BUF,\
	n_SendRconCommand(amx,nc_params))

#define NC_SetCameraBehindPlayer(PLAYERID) \
	(NC_PARS_(1)nc_params[1]=PLAYERID,\
	n_SetCameraBehindPlayer(amx,nc_params))

#define NC_SetGameModeText(BUF) (NC_PARS_(1)nc_params[1]=BUF,\
	n_SetGameModeText(amx,nc_params))

#define NC_SetPlayerFacingAngle(PLAYERID,FR) (NC_PARS_(2)\
	nc_params[1]=PLAYERID,nc_params[2]=FR,\
	n_SetPlayerFacingAngle(amx,nc_params))

#define NC_SetPlayerHealth(PLAYERID,FHEALTH) \
	(NC_PARS_(2)nc_params[1]=PLAYERID,nc_paramf[2]=FHEALTH,\
	n_SetPlayerHealth(amx,nc_params))

#define NC_SetPlayerPos __USE__natives_SetPlayerPos__

#define NC_SetPlayerRaceCheckpoint(PLAYERID,TYPE,FX,FY,FZ,FNX,FNY,FNZ,FSIZE) \
	(NC_PARS_(9)nc_params[1]=PLAYERID,nc_params[2]=TYPE,\
	nc_paramf[3]=FX,nc_paramf[4]=FY,nc_paramf[5]=FZ,nc_paramf[6]=FNX,\
	nc_paramf[7]=FNY,nc_paramf[8]=FNZ,nc_paramf[9]=FSIZE,\
	n_SetPlayerRaceCheckpoint(amx,nc_params))

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

#define NC_SetVehicleParamsEx(VID,ENGINE,LIGHTS,ALARM,DOORS,BONNET,BOOT,OBJ) \
	(NC_PARS_(8)nc_params[1]=VID,nc_params[2]=ENGINE,\
	nc_params[3]=LIGHTS,nc_params[4]=ALARM,nc_params[5]=DOORS,\
	nc_params[6]=BONNET,nc_params[7]=BOOT,nc_params[8]=OBJ,\
	n_SetVehicleParamsEx(amx,nc_params))

#define NC_SetVehicleToRespawn(VEHICLEID) \
	(NC_PARS_(1)nc_params[1]=VEHICLEID,\
	n_SetVehicleToRespawn(amx,nc_params))

#define NC_SetVehicleZAngle(VEHICLEID,FROT) \
	(NC_PARS_(2)nc_params[1]=VEHICLEID,nc_paramf[2]=FROT,\
	n_SetVehicleZAngle(amx,nc_params))

#define NC_ShowPlayerDialog __USE__dialog_ShowPlayerDialog__

#define NC_SpawnPlayer __USE__natives_SpawnPlayer__

#define NC_TextDrawHideForPlayer(PLAYERID,TXT) \
	(NC_PARS_(2)nc_params[1]=PLAYERID,nc_params[2]=TXT,\
	n_TextDrawHideForPlayer(amx,nc_params))

#define NC_TextDrawShowForPlayer(PLAYERID,TXT) \
	(NC_PARS_(2)nc_params[1]=PLAYERID,nc_params[2]=TXT,\
	n_TextDrawShowForPlayer(amx,nc_params))

#define NC_TogglePlayerClock(PLAYERID,FLAG) \
	(NC_PARS_(2)nc_params[1]=PLAYERID,nc_params[2]=FLAG,\
	n_TogglePlayerClock(amx,nc_params))

#define NC_TogglePlayerControllable(PLAYERID,FLAG) \
	(NC_PARS_(2)nc_params[1]=PLAYERID,nc_params[2]=FLAG,\
	n_TogglePlayerControllable(amx,nc_params))

#define NC_TogglePlayerSpectating(PLAYERID,FLAG) \
	(NC_PARS_(2)nc_params[1]=PLAYERID,nc_params[2]=FLAG,\
	n_TogglePlayerSpectating(amx,nc_params))

#define NC_UsePlayerPedAnims() (NC_PARS_(0)\
	n_UsePlayerPedAnims(amx,nc_params))

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
		N(CreatePlayer3DTextLabel),
		N(CreatePlayerObject),
		N(CreatePlayerTextDraw),
		N_(CreateVehicle),
		N(DeletePlayer3DTextLabel),
		N(DestroyObject),
		N(DestroyPlayerObject),
		N_(DestroyVehicle),
		N(DisablePlayerCheckpoint),
		N(DisablePlayerRaceCheckpoint),
		N(EnableStuntBonusForAll),
		N(ForceClassSelection),
		N(GameTextForPlayer),
		N(GetConsoleVarAsInt),
		N(GetPlayerIp),
		N(GetPlayerFacingAngle),
		N(GetPlayerKeys),
		N(GetPlayerName),
		N(GetPlayerPing),
		N(GetPlayerPos),
		N(GetPlayerScore),
		N(GetPlayerState),
		N(GetPlayerVehicleID),
		N(GetPlayerVehicleSeat),
		N(GetServerTickRate),
		N(GetVehicleDamageStatus),
		N_(GetVehicleHealth),
		N(GetVehicleModel),
		N(GetVehicleRotationQuat),
		N(GetVehicleParamsEx),
		N(GetVehiclePos),
		N(GetVehiclePoolSize),
		N(GetVehicleVelocity),
		N(GetVehicleZAngle),
		N_(GivePlayerMoney),
		N(GivePlayerWeapon),
		N(IsValidVehicle),
		N(IsVehicleStreamedIn),
		N_(Kick),
		N(MoveObject),
		N(PlayerPlaySound),
		N(PlayerTextDrawAlignment),
		N(PlayerTextDrawBackgroundColor),
		N(PlayerTextDrawBoxColor),
		N(PlayerTextDrawColor),
		N(PlayerTextDrawDestroy),
		N(PlayerTextDrawFont),
		N(PlayerTextDrawHide),
		N(PlayerTextDrawLetterSize),
		N(PlayerTextDrawSetOutline),
		N(PlayerTextDrawSetProportional),
		N(PlayerTextDrawSetShadow),
		N(PlayerTextDrawSetString),
		N(PlayerTextDrawShow),
		N(PlayerTextDrawTextSize),
		N(PlayerTextDrawUseBox),
		N_(PutPlayerInVehicle),
		N(RemoveBuildingForPlayer),
		N(RemovePlayerMapIcon),
		N(RepairVehicle),
		N_(ResetPlayerMoney),
		N(SHA256_PassHash),
		N(SendClientMessage),
		N(SendClientMessageToAll),
		N(SendRconCommand),
		N(SetCameraBehindPlayer),
		N(SetGameModeText),
		N(SetPlayerCameraPos),
		N(SetPlayerCameraLookAt),
		N(SetPlayerColor),
		N(SetPlayerFacingAngle),
		N(SetPlayerHealth),
		N(SetPlayerMapIcon),
		N_(SetPlayerName),
		N_(SetPlayerPos),
		N(SetPlayerRaceCheckpoint),
		N(SetPlayerScore),
		N(SetPlayerSpecialAction),
		N(SetPlayerTime),
		N(SetPlayerWeather),
		N(SetSpawnInfo),
		N(SetVehicleHealth),
		N(SetVehicleParamsEx),
		N(SetVehicleParamsForPlayer),
		N(SetVehiclePos),
		N(SetVehicleToRespawn),
		N(SetVehicleZAngle),
		N_(ShowPlayerDialog),
		N_(SpawnPlayer),
		N(TogglePlayerClock),
		N(TogglePlayerSpectating),
		N(TextDrawAlignment),
		N(TextDrawBoxColor),
		N(TextDrawColor),
		N(TextDrawCreate),
		N(TextDrawFont),
		N(TextDrawHideForPlayer),
		N(TextDrawLetterSize),
		N(TextDrawSetOutline),
		N(TextDrawSetProportional),
		N(TextDrawSetShadow),
		N(TextDrawShowForPlayer),
		N(TextDrawTextSize),
		N(TextDrawUseBox),
		N(TogglePlayerControllable),
		N(UpdateVehicleDamageStatus),
		N(UsePlayerPedAnims),
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

/**
Callback for timer kicking a player, backup when OnPlayerUpdate doesn't get called.
*/
static
int kick_timer_cb(void *data)
{
	int playerid;

	playerid = PLAYER_CC_GETID(data);
	if (PLAYER_CC_CHECK(data, playerid)) {
		NC_PARS(1);
		nc_params[1] = playerid;
		NC(n_Kick_);
	}
	return 0;
}

/**
Kicks (delayed), broadcasts message and logs.

@param reason NULL to not broadcast a message (will have empty reason in table)
@param issuer name of issuer (NULL for system kick) (does not get saved to db)
@param issuer_userid may be -1 for system kick
@return 1 if the player is kicked, 0 if they were already kicked.
*/
static
int natives_Kick(int playerid, char *reason, char *issuer, int issuer_userid)
{
	const static char *SYSTEM_ISSUER = "system", *REASONNULL = "(NULL)";

	int intv;
	char *escapedreason;

	if (!kick_update_delay[playerid]) {
		if (issuer == NULL) {
			issuer = (char*) SYSTEM_ISSUER;
		}

		if (reason != NULL) {
			csprintf(buf144,
				"%s[%d] was kicked by %s (%s)",
				pdata[playerid]->name,
				playerid,
				issuer,
				reason);
			NC_SendClientMessageToAll(COL_WARN, buf144a);
		}

		B144("~r~You've been kicked.");
		NC_GameTextForPlayer(playerid, buf144a, 0x800000, 3);
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

/**
Puts a player in vehicle, updating map, zone, vehiclestuff ...

Also resets the vehicle HP to 1000.0 when it's invalid.
*/
static
int natives_PutPlayerInVehicle(int playerid, int vehicleid, int seat)
{
	float hp;
	struct vec3 pos;

	if (seat == 0) {
		NC_PARS(2);
		nc_params[1] = vehicleid;
		nc_params[2] = buf32a;
		NC(n_GetVehicleHealth_);
		hp = *fbuf32;
		if (hp != hp || hp < 0.0f || 1000.0f < hp) {
			NC_SetVehicleHealth(vehicleid, 1000.0f);
		}
	}

	common_GetVehiclePos(vehicleid, &pos);
	maps_stream_for_player(playerid, pos);
	veh_on_player_now_driving(playerid, vehicleid, gamevehicles[vehicleid].dbvehicle);
	svp_update_mapicons(playerid, pos.x, pos.y);
	missions_update_missionpoint_indicators(playerid, pos.x, pos.y, pos.z);
	zones_update(playerid, pos);

	NC_PARS(3);
	nc_params[1] = playerid;
	nc_params[2] = vehicleid;
	nc_params[3] = seat;
	return NC(n_PutPlayerInVehicle_);
}

/**
To store player name and normalized name in plugin.
*/
static
int natives_SetPlayerName(int playerid, char *name)
{
	int res;

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
		csprintf(buf144,
			"Your name has been changed to '%s'",
			pdata[playerid]->name);
		NC_PARS(3);
		nc_params[2] = COL_SAMP_GREEN;
		nc_params[3] = buf144a;
		NC(n_SendClientMessage);
	}
	return res;
}

/**
Done here to do stuff, like streaming maps, anticheat?
*/
static
int natives_SetPlayerPos(int playerid, struct vec3 pos)
{
	/*TODO? Technically the amount of _models_ and not _objects_ determines how long
	it will take to load, because the models are loaded in a blocking way.
	Because of that, a map with 100 objects but all different models might block more
	than a 1000 object map with 1 model, but the check below is pure object count.*/
	if (maps_calculate_objects_to_create_for_player_at_position(playerid, pos) > 200) {
		B144("Loading objects...");
		NC_GameTextForPlayer(playerid, buf144a, 0x80000, 5);
	}
	maps_stream_for_player(playerid, pos);
	B144("_");
	NC_GameTextForPlayer(playerid, buf144a, 0, 5);
	svp_update_mapicons(playerid, pos.x, pos.y);
	missions_update_missionpoint_indicators(playerid, pos.x, pos.y, pos.z);
	zones_update(playerid, pos);

	NC_PARS(4);
	nc_params[1] = playerid;
	nc_paramf[2] = pos.x;
	nc_paramf[3] = pos.y;
	nc_paramf[4] = pos.z;
	return NC(n_SetPlayerPos_);
}

/**
SpawnPlayer kills players that are in vehicles, and spawns them with a bottle.

So this wrapper does SetPlayerPos (directly) if needed, because that will
eject a player.
*/
static
void natives_SpawnPlayer(int playerid)
{
	/*eject player first if they're in a vehicle*/
	if (NC_GetPlayerVehicleID(playerid)) {
		NC_GetPlayerPos(playerid, buf32a, buf64a, buf144a);
		NC(n_SetPlayerPos_);
	}
	NC_PARS(1);
	/*nc_params[1] = playerid;*/
	NC(n_SpawnPlayer_);

	spawn_prespawn(playerid);
}
