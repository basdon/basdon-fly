
/* vim: set filetype=c ts=8 noexpandtab: */

int natives_find();

extern AMX_NATIVE n_AddPlayerClass;
extern AMX_NATIVE n_AddStaticVehicleEx;
extern AMX_NATIVE n_ChangeVehicleColor;
extern AMX_NATIVE n_ClearAnimations;
extern AMX_NATIVE n_CreateObject;
extern AMX_NATIVE n_CreatePlayer3DTextLabel;
extern AMX_NATIVE n_CreatePlayerObject;
extern AMX_NATIVE n_CreatePlayerTextDraw;
extern AMX_NATIVE n_CreateVehicle_;
extern AMX_NATIVE n_DeletePlayer3DTextLabel;
extern AMX_NATIVE n_DestroyObject;
extern AMX_NATIVE n_DestroyPlayerObject;
extern AMX_NATIVE n_DestroyVehicle_;
extern AMX_NATIVE n_DisablePlayerCheckpoint;
extern AMX_NATIVE n_DisablePlayerRaceCheckpoint;
extern AMX_NATIVE n_EnableStuntBonusForAll;
extern AMX_NATIVE n_ForceClassSelection;
extern AMX_NATIVE n_GameTextForPlayer;
extern AMX_NATIVE n_GetConsoleVarAsInt;
extern AMX_NATIVE n_GetPlayerFacingAngle;
extern AMX_NATIVE n_GetPlayerIp;
extern AMX_NATIVE n_GetPlayerKeys;
extern AMX_NATIVE n_GetPlayerName;
extern AMX_NATIVE n_GetPlayerPing;
extern AMX_NATIVE n_GetPlayerPos;
extern AMX_NATIVE n_GetPlayerScore;
extern AMX_NATIVE n_GetPlayerState;
extern AMX_NATIVE n_GetPlayerVehicleID;
extern AMX_NATIVE n_GetPlayerVehicleSeat;
extern AMX_NATIVE n_GetServerTickRate;
extern AMX_NATIVE n_GetVehicleDamageStatus;
extern AMX_NATIVE n_GetVehicleHealth_;
extern AMX_NATIVE n_GetVehicleModel;
extern AMX_NATIVE n_GetVehicleParamsEx;
extern AMX_NATIVE n_GetVehiclePos;
extern AMX_NATIVE n_GetVehiclePoolSize;
extern AMX_NATIVE n_GetVehicleRotationQuat;
extern AMX_NATIVE n_GetVehicleVelocity;
extern AMX_NATIVE n_GetVehicleZAngle;
extern AMX_NATIVE n_GivePlayerMoney_;
extern AMX_NATIVE n_GivePlayerWeapon;
extern AMX_NATIVE n_IsValidVehicle;
extern AMX_NATIVE n_IsVehicleStreamedIn;
extern AMX_NATIVE n_Kick_;
extern AMX_NATIVE n_MoveObject;
extern AMX_NATIVE n_PlayerPlaySound;
extern AMX_NATIVE n_PlayerTextDrawAlignment;
extern AMX_NATIVE n_PlayerTextDrawBackgroundColor;
extern AMX_NATIVE n_PlayerTextDrawBoxColor;
extern AMX_NATIVE n_PlayerTextDrawColor;
extern AMX_NATIVE n_PlayerTextDrawDestroy;
extern AMX_NATIVE n_PlayerTextDrawFont;
extern AMX_NATIVE n_PlayerTextDrawHide;
extern AMX_NATIVE n_PlayerTextDrawLetterSize;
extern AMX_NATIVE n_PlayerTextDrawSetOutline;
extern AMX_NATIVE n_PlayerTextDrawSetProportional;
extern AMX_NATIVE n_PlayerTextDrawSetShadow;
extern AMX_NATIVE n_PlayerTextDrawSetString;
extern AMX_NATIVE n_PlayerTextDrawShow;
extern AMX_NATIVE n_PlayerTextDrawTextSize;
extern AMX_NATIVE n_PlayerTextDrawUseBox;
extern AMX_NATIVE n_PutPlayerInVehicle_;
extern AMX_NATIVE n_RemoveBuildingForPlayer;
extern AMX_NATIVE n_RemovePlayerMapIcon;
extern AMX_NATIVE n_RepairVehicle;
extern AMX_NATIVE n_ResetPlayerMoney_;
extern AMX_NATIVE n_SHA256_PassHash;
extern AMX_NATIVE n_SendClientMessage;
extern AMX_NATIVE n_SendClientMessageToAll;
extern AMX_NATIVE n_SendDeathMessage;
extern AMX_NATIVE n_SendRconCommand;
extern AMX_NATIVE n_SetCameraBehindPlayer;
extern AMX_NATIVE n_SetGameModeText;
extern AMX_NATIVE n_SetPlayerCameraPos;
extern AMX_NATIVE n_SetPlayerCameraLookAt;
extern AMX_NATIVE n_SetPlayerColor;
extern AMX_NATIVE n_SetPlayerFacingAngle;
extern AMX_NATIVE n_SetPlayerHealth;
extern AMX_NATIVE n_SetPlayerMapIcon;
extern AMX_NATIVE n_SetPlayerName_;
extern AMX_NATIVE n_SetPlayerPos_;
extern AMX_NATIVE n_SetPlayerRaceCheckpoint;
extern AMX_NATIVE n_SetPlayerScore;
extern AMX_NATIVE n_SetPlayerSpecialAction;
extern AMX_NATIVE n_SetPlayerTime;
extern AMX_NATIVE n_SetPlayerWeather;
extern AMX_NATIVE n_SetSpawnInfo;
extern AMX_NATIVE n_SetVehicleHealth;
extern AMX_NATIVE n_SetVehicleParamsEx;
extern AMX_NATIVE n_SetVehicleParamsForPlayer;
extern AMX_NATIVE n_SetVehiclePos;
extern AMX_NATIVE n_SetVehicleToRespawn;
extern AMX_NATIVE n_SetVehicleZAngle;
extern AMX_NATIVE n_ShowPlayerDialog_;
extern AMX_NATIVE n_SpawnPlayer_;
extern AMX_NATIVE n_TextDrawAlignment;
extern AMX_NATIVE n_TextDrawBoxColor;
extern AMX_NATIVE n_TextDrawColor;
extern AMX_NATIVE n_TextDrawCreate;
extern AMX_NATIVE n_TextDrawFont;
extern AMX_NATIVE n_TextDrawHideForPlayer;
extern AMX_NATIVE n_TextDrawLetterSize;
extern AMX_NATIVE n_TextDrawSetOutline;
extern AMX_NATIVE n_TextDrawSetProportional;
extern AMX_NATIVE n_TextDrawSetShadow;
extern AMX_NATIVE n_TextDrawShowForPlayer;
extern AMX_NATIVE n_TextDrawTextSize;
extern AMX_NATIVE n_TextDrawUseBox;
extern AMX_NATIVE n_TogglePlayerClock;
extern AMX_NATIVE n_TogglePlayerControllable;
extern AMX_NATIVE n_TogglePlayerSpectating;
extern AMX_NATIVE n_UpdateVehicleDamageStatus;
extern AMX_NATIVE n_UsePlayerPedAnims;
extern AMX_NATIVE n_bcrypt_check;
extern AMX_NATIVE n_bcrypt_get_hash;
extern AMX_NATIVE n_bcrypt_hash;
extern AMX_NATIVE n_bcrypt_is_equal;
extern AMX_NATIVE n_cache_delete;
extern AMX_NATIVE n_cache_get_row;
extern AMX_NATIVE n_cache_get_row_count;
extern AMX_NATIVE n_cache_get_row_float;
extern AMX_NATIVE n_cache_get_row_int;
extern AMX_NATIVE n_cache_insert_id;
extern AMX_NATIVE n_gpci;
extern AMX_NATIVE n_mysql_connect;
extern AMX_NATIVE n_mysql_close;
extern AMX_NATIVE n_mysql_escape_string;
extern AMX_NATIVE n_mysql_errno;
extern AMX_NATIVE n_mysql_log;
extern AMX_NATIVE n_mysql_tquery;
extern AMX_NATIVE n_mysql_query;
extern AMX_NATIVE n_mysql_unprocessed_queries;
extern AMX_NATIVE n_random;
extern AMX_NATIVE n_ssocket_connect;
extern AMX_NATIVE n_ssocket_create;
extern AMX_NATIVE n_ssocket_destroy;
extern AMX_NATIVE n_ssocket_listen;
extern AMX_NATIVE n_ssocket_send;
extern AMX_NATIVE n_tickcount;

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
extern union NCDATA nc_data;
#define nc_params nc_data.asint
#define nc_paramf nc_data.asflt
/**
Used to reinterpret cell return values as a float.
*/
extern cell tmpfloat;

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

#define NC_SendDeathMessage(KILLER,KILLEE,REASON) (NC_PARS_(3)\
	nc_params[1]=KILLER,nc_params[2]=KILLEE,nc_params[3]=REASON,\
	n_SendDeathMessage(amx,nc_params))

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

#ifdef PRINTQUERIES
void DEBUG_NC_mysql_tquery_nocb(cell buffa);
#endif
/**
To store player name and normalized name in plugin.
*/
int natives_SetPlayerName(int playerid, char *name);
/**
Done here to do stuff, like streaming maps, anticheat?
*/
int natives_SetPlayerPos(int playerid, struct vec3 pos);
/**
SpawnPlayer kills players that are in vehicles, and spawns them with a bottle.

So this wrapper does SetPlayerPos (directly) if needed, because that will
eject a player.
*/
void natives_SpawnPlayer(int playerid);
/**
Puts a player in vehicle, updating map, zone, vehiclestuff ...

Also resets the vehicle HP to 1000.0 when it's invalid.
*/
int natives_PutPlayerInVehicle(int playerid, int vehicleid, int seat);
/**
Kicks (delayed), broadcasts message and logs.

@param reason NULL to not broadcast a message (will have empty reason in table)
@param issuer name of issuer (NULL for system kick) (does not get saved to db)
@param issuer_userid may be -1 for system kick
@return 1 if the player is kicked, 0 if they were already kicked.
*/
int natives_Kick(int playerid, char *reason, char *issuer, int issuer_userid);
