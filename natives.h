
/* vim: set filetype=c ts=8 noexpandtab: */

int natives_find(AMX *amx);

extern int n_AddPlayerClass;
extern int n_ChangeVehicleColor;
extern int n_CreatePlayerObject;
extern int n_CreatePlayerTextDraw;
extern int n_CreateVehicle_;
extern int n_DestroyPlayerObject;
extern int n_DestroyVehicle_;
extern int n_DisablePlayerRaceCheckpoint;
extern int n_GameTextForPlayer;
extern int n_GetConsoleVarAsInt;
extern int n_GetPlayerIp;
extern int n_GetPlayerName;
extern int n_GetPlayerPos;
extern int n_GetPlayerVehicleID;
extern int n_GetVehicleModel;
extern int n_GetVehiclePos;
extern int n_GetVehicleVelocity;
extern int n_GetVehicleZAngle;
extern int n_GivePlayerWeapon;
extern int n_PlayerTextDrawAlignment;
extern int n_PlayerTextDrawColor;
extern int n_PlayerTextDrawFont;
extern int n_PlayerTextDrawHide;
extern int n_PlayerTextDrawLetterSize;
extern int n_PlayerTextDrawSetOutline;
extern int n_PlayerTextDrawSetProportional;
extern int n_PlayerTextDrawSetShadow;
extern int n_PlayerTextDrawSetString;
extern int n_PlayerTextDrawShow;
extern int n_RemoveBuildingForPlayer;
extern int n_SendClientMessage;
extern int n_SendClientMessageToAll;
extern int n_SendRconCommand;
extern int n_SetCameraBehindPlayer;
extern int n_SetPlayerCameraPos;
extern int n_SetPlayerCameraLookAt;
extern int n_SetPlayerColor;
extern int n_SetPlayerFacingAngle;
extern int n_SetPlayerPos_;
extern int n_SetPlayerRaceCheckpoint;
extern int n_SetPlayerSpecialAction;
extern int n_SetVehicleToRespawn;
extern int n_ShowPlayerDialog_;
extern int n_SpawnPlayer_;
extern int n_cache_delete;
extern int n_cache_get_row;
extern int n_cache_get_row_count;
extern int n_cache_get_row_float;
extern int n_cache_get_row_int;
extern int n_gettime;
extern int n_mysql_tquery;
extern int n_mysql_query;
extern int n_printf;
extern int n_random;
extern int n_ssocket_connect;
extern int n_ssocket_create;
extern int n_ssocket_destroy;
extern int n_ssocket_listen;
extern int n_ssocket_send;
extern int n_tickcount;
extern cell *nc_params;
extern cell nc_result;

#define SOCKET_TCP 1
#define SOCKET_UDP 2
#define SOCKET_INVALID_SOCKET -1

/* NC = NativeCall */

#define NC_(NATIVE,RESULT) amx_Callback(amx,NATIVE,(cell*)RESULT,nc_params)
#define NC(NATIVE) NC_(NATIVE,&nc_result)

#define NC_AddPlayerClass(CLASSID,FX,FY,FZ,FR,W1) nc_params[0]=11;\
	nc_params[1]=CLASSID;nc_params[2]=FX;nc_params[3]=FY;nc_params[4]=FZ;\
	nc_params[5]=FR;nc_params[6]=WEAPON_CAMERA;nc_params[7]=3036;\
	nc_params[8]=nc_params[9]=nc_params[10]=nc_params[11]=0;\
	amx_Callback(amx,n_AddPlayerClass,&nc_result,nc_params)

#define NC_ChangeVehicleColor(VEHICLEID,COL1,COL2) \
	nc_params[0]=3;nc_params[1]=VEHICLEID;\
	nc_params[2]=COL1;nc_params[3]=COL2;\
	amx_Callback(amx,n_ChangeVehicleColor,&nc_result,nc_params)

#define NC_CreatePlayerObject(PLAYERID,MODELID,FX,FY,FZ,FRX,FRY,FRZ,FDISTANCE) \
	nc_params[0]=9;nc_params[1]=PLAYERID;nc_params[2]=MODELID;\
	nc_params[3]=FX;nc_params[4]=FY;nc_params[5]=FZ;nc_params[6]=FRX;\
	nc_params[7]=FRY;nc_params[8]=FRZ;nc_params[9]=FDISTANCE;\
	amx_Callback(amx,n_CreatePlayerObject,&nc_result,nc_params)

#define NC_CreatePlayerTextDraw(PLAYERID,FX,FY,BUF) nc_params[0]=4;\
	nc_params[1]=PLAYERID;nc_params[2]=FX;nc_params[3]=FY;\
	nc_params[4]=BUF;\
	amx_Callback(amx,n_CreatePlayerTextDraw,&nc_result,nc_params)

#define NC_CreateVehicle __USE__veh_NC_CreateVehicle__

#define NC_DestroyPlayerObject(PLAYERID,OBJECTID) \
	nc_params[0]=2;nc_params[1]=PLAYERID;nc_params[2]=OBJECTID;\
	amx_Callback(amx,n_DestroyPlayerObject,&nc_result,nc_params)

#define NC_DestroyVehicle __USE__veh_NC_DestroyVehicle__

#define NC_DisablePlayerRaceCheckpoint(PLAYERID) \
	nc_params[0]=1;nc_params[1]=PLAYERID;\
	amx_Callback(amx,n_DisablePlayerRaceCheckpoint,&nc_result,nc_params)

#define NC_GameTextForPlayer(PLAYERID,BUF,LENGTH,STYLE) \
	nc_params[0]=4;nc_params[1]=PLAYERID;nc_params[2]=BUF;\
	nc_params[3]=LENGTH;nc_params[4]=STYLE;\
	amx_Callback(amx,n_GameTextForPlayer,&nc_result,nc_params)

#define NC_GetConsoleVarAsInt(BUF) nc_params[0]=1;nc_params[1]=BUF;\
	amx_Callback(amx,n_GetConsoleVarAsInt,&nc_result,nc_params)

#define NC_GetPlayerIp(PLAYERID,BUF,LEN) nc_params[0]=3;\
	nc_params[1]=PLAYERID;nc_params[2]=BUF;nc_params[3]=LEN;\
	amx_Callback(amx,n_GetPlayerIp,&nc_result,nc_params)

#define NC_GetPlayerName_(PLAYERID,BUF,LEN,RESULT) nc_params[0]=3;\
	nc_params[1]=PLAYERID;nc_params[2]=BUF;nc_params[3]=LEN;\
	amx_Callback(amx,n_GetPlayerName,(cell*)RESULT,nc_params)
#define NC_GetPlayerName(PLAYERID,BUF,LEN) \
	NC_GetPlayerName_(PLAYERI,BUF,LEN,&nc_result)

#define NC_GetPlayerPos(PLAYERID,FREFX,FREFY,FREFZ) nc_params[0]=4;\
	nc_params[1]=PLAYERID;nc_params[2]=FREFX;nc_params[3]=FREFY;\
	nc_params[4]=FREFZ;\
	amx_Callback(amx,n_GetPlayerPos,&nc_result,nc_params)

#define NC_GetPlayerVehicleID_(PLAYERID,RESULT) \
	nc_params[0]=1;nc_params[1]=PLAYERID;\
	amx_Callback(amx,n_GetPlayerVehicleID,(cell*)RESULT,nc_params)
#define NC_GetPlayerVehicleID(PLAYERID) \
	NC_GetPlayerVehicleID_(PLAYERID,&nc_result)

#define NC_GetVehicleModel_(VEHICLEID,RESULT) \
	nc_params[0]=1;nc_params[1]=VEHICLEID;\
	amx_Callback(amx,n_GetVehicleModel,(cell*)RESULT,nc_params)
#define NC_GetVehicleModel(VEHICLEID) NC_GetVehicleModel_(VEHICLEID,&nc_result)

#define NC_GetVehiclePos(VEHICLEID,FX,FY,FZ) \
	nc_params[0]=4;nc_params[1]=VEHICLEID;\
	nc_params[2]=FX;nc_params[3]=FY;nc_params[4]=FZ;\
	amx_Callback(amx,n_GetVehiclePos,&nc_result,nc_params)

#define NC_GetVehicleVelocity(VEHICLEID,FX,FY,FZ) \
	nc_params[0]=4;nc_params[1]=VEHICLEID;\
	nc_params[2]=FX;nc_params[3]=FY;nc_params[4]=FZ;\
	amx_Callback(amx,n_GetVehicleVelocity,&nc_result,nc_params)

#define NC_GetVehicleZAngle(VEHICLEID,FR) \
	nc_params[0]=2;nc_params[1]=VEHICLEID;nc_params[2]=FR;\
	amx_Callback(amx,n_GetVehicleZAngle,&nc_result,nc_params)

#define NC_GivePlayerWeapon(PLAYERID,WEAPONID,AMMO) nc_params[0]=3;\
	nc_params[1]=PLAYERID;nc_params[2]=WEAPONID;nc_params[3]=AMMO;\
	amx_Callback(amx,n_GivePlayerWeapon,&nc_result,nc_params)

#define NC_PlayerTextDrawAlignment(PLAYERID,TD,ALIGNMENT) nc_params[0]=3;\
	nc_params[1]=PLAYERID;nc_params[2]=TD;nc_params[3]=ALIGNMENT;\
	amx_Callback(amx,n_PlayerTextDrawAlignment,&nc_result,nc_params)

#define NC_PlayerTextDrawColor(PLAYERID,TD,COLOR) nc_params[0]=3;\
	nc_params[1]=PLAYERID;nc_params[2]=TD;nc_params[3]=COLOR;\
	amx_Callback(amx,n_PlayerTextDrawColor,&nc_result,nc_params)

#define NC_PlayerTextDrawFont(PLAYERID,TD,FONT) nc_params[0]=3;\
	nc_params[1]=PLAYERID;nc_params[2]=TD;nc_params[3]=FONT;\
	amx_Callback(amx,n_PlayerTextDrawFont,&nc_result,nc_params)

#define NC_PlayerTextDrawHide(PLAYERID,TD) \
	nc_params[0]=2;nc_params[1]=PLAYERID;nc_params[2]=TD;\
	amx_Callback(amx,n_PlayerTextDrawHide,&nc_result,nc_params)

#define NC_PlayerTextDrawLetterSize(PLAYERID,TD,FX,FY) nc_params[0]=3;\
	nc_params[1]=PLAYERID;nc_params[2]=TD;\
	nc_params[3]=FX;nc_params[4]=FY\
	amx_Callback(amx,n_PlayerTextDrawLetterSize,&nc_result,nc_params)

#define NC_PlayerTextDrawOutline(PLAYERID,TD,OUTLINE) nc_params[0]=3;\
	nc_params[1]=PLAYERID;nc_params[2]=TD;nc_params[3]=OUTLINE;\
	amx_Callback(amx,n_PlayerTextDrawOutline,&nc_result,nc_params)

#define NC_PlayerTextDrawProportional(PLAYERID,TD,PROP) nc_params[0]=3;\
	nc_params[1]=PLAYERID;nc_params[2]=TD;nc_params[3]=PROP;\
	amx_Callback(amx,n_PlayerTextDrawProportional,&nc_result,nc_params)

#define NC_PlayerTextDrawSetShadow(PLAYERID,TD,SHADOW) nc_params[0]=3;\
	nc_params[1]=PLAYERID;nc_params[2]=TD;nc_params[3]=SHADOW;\
	amx_Callback(amx,n_PlayerTextDrawSetShadow,&nc_result,nc_params)

#define NC_PlayerTextDrawSetString(PLAYERID,TD,BUF) nc_params[0]=3;\
	nc_params[1]=PLAYERID;nc_params[2]=TD;nc_params[3]=BUF;\
	amx_Callback(amx,n_PlayerTextDrawSetString,&nc_result,nc_params)

#define NC_PlayerTextDrawShow(PLAYERID,TD) \
	nc_params[0]=2;nc_params[1]=PLAYERID;nc_params[2]=TD;\
	amx_Callback(amx,n_PlayerTextDrawShow,&nc_result,nc_params)

#define NC_SendClientMessage(PLAYERID,COLOR,BUF) \
	nc_params[0]=3;\
	nc_params[1]=PLAYERID;nc_params[2]=COLOR;nc_params[3]=BUF;\
	amx_Callback(amx,n_SendClientMessage,&nc_result,nc_params)

#define NC_SendClientMessageToAll(COLOR,BUF) \
	nc_params[0]=2;nc_params[1]=COLOR;nc_params[2]=BUF;\
	amx_Callback(amx,n_SendClientMessageToAll,&nc_result,nc_params)

#define NC_SendRconCommand(BUF) nc_params[0]=1;nc_params[1]=BUF;\
	amx_Callback(amx,n_SendRconCommand,&nc_result,nc_params)

#define NC_SetCameraBehindPlayer(PLAYERID) \
	nc_params[0]=1;nc_params[1]=PLAYERID;\
	amx_Callback(amx,n_SetCameraBehindPlayer,&nc_result,nc_params)

#define NC_SetPlayerFacingAngle(PLAYERID,FR) nc_params[0]=2;\
	nc_params[1]=PLAYERID;nc_params[2]=FR;\
	amx_Callback(amx,n_SetPlayerFacingAngle,&nc_result,nc_params)

#define NC_SetPlayerPos __USE__natives_NC_SetPlayerPos__

#define NC_SetPlayerRaceCheckpoint(PLAYERID,TYPE,FX,FY,FZ,FNX,FNY,FNZ,FSIZE) \
	nc_params[0]=9;nc_params[1]=PLAYERID;nc_params[2]=TYPE;\
	nc_params[3]=FX;nc_params[4]=FY;nc_params[5]=FZ;nc_params[6]=FNX;\
	nc_params[7]=FNY;nc_params[8]=FNZ;nc_params[9]=FSIZE;\
	amx_Callback(amx,n_SetPlayerRaceCheckpoint,&nc_result,nc_params)

#define NC_SetVehicleToRespawn(VEHICLEID) \
	nc_params[0]=1;nc_params[1]=VEHICLEID;\
	amx_Callback(amx,n_SetVehicleToRespawn,&nc_result,nc_params)

#define NC_ShowPlayerDialog __USE__dialog_NC_ShowPlayerDialog__

#define NC_SpawnPlayer __USE__natives_NC_SpawnPlayer__

#define NC_cache_delete(ID) nc_params[0]=2;nc_params[1]=ID;nc_params[2]=1;\
	amx_Callback(amx,n_cache_delete,&nc_result,nc_params)

#define NC_cache_get_row_count_(RESULT) nc_params[0]=1;nc_params[1]=1;\
	amx_Callback(amx,n_cache_get_row_count,RESULT,nc_params)
#define NC_cache_get_row_count() NC_cache_get_row_count_(&nc_result)

#define NC_cache_get_field_int(ROW,COL,INTREF) nc_params[0]=2;\
	nc_params[1]=ROW;nc_params[2]=COL;\
	amx_Callback(amx,n_cache_get_row_int,INTREF,nc_params)

#define NC_cache_get_field_flt_cell(ROW,COL,CELLREF) nc_params[0]=2;\
	nc_params[1]=ROW;nc_params[2]=COL;\
	amx_Callback(amx,n_cache_get_row_float,CELLREF,nc_params)

#define NC_cache_get_field_flt(ROW,COL,FLOAT) \
	NC_cache_get_field_flt_cell(ROW,COL,&nc_result);\
	FLOAT=amx_ctof(nc_result)

#define NC_cache_get_field_str(ROW,COL,BUF) nc_params[0]=3;\
	nc_params[1]=ROW;nc_params[2]=COL;nc_params[3]=BUF;\
	amx_Callback(amx,n_cache_get_row,&nc_result,nc_params)

/*note: don't call with 0,0,0 - it tends to mess up stuff after a while?*/
#define NC_gettime(HOUR,MIN,SEC) nc_params[0]=3;\
	nc_params[1]=HOUR;nc_params[2]=MIN;nc_params[3]=SEC;\
	amx_Callback(amx,n_gettime,&nc_result,nc_params)

#define NC_mysql_query_(BUF,RESULT) nc_params[0]=3;nc_params[1]=1;\
	nc_params[2]=BUF;nc_params[3]=1;\
	amx_Callback(amx,n_mysql_query,RESULT,nc_params)
#define NC_mysql_query(BUF) NC_mysql_query_(BUF,&nc_result)

#define NC_mysql_tquery_nocb(BUF) nc_params[0]=4;nc_params[1]=1;\
	nc_params[2]=BUF;nc_params[3]=nc_params[4]=emptystringa;\
	amx_Callback(amx,n_mysql_tquery,&nc_result,nc_params)

#define NC_random_(MAX,RESULT) nc_params[0]=1;nc_params[1]=MAX;\
	amx_Callback(amx,n_random,(cell*)RESULT,nc_params)
#define NC_random(MAX) NC_random_(MAX,&nc_result)

#define NC_ssocket_connect(SOCKET,HOST,PORT) nc_params[0]=3;\
	nc_params[1]=SOCKET;nc_params[2]=HOST;nc_params[3]=PORT;\
	amx_Callback(amx,n_ssocket_connect,&nc_result,nc_params)

#define NC_ssocket_create(TYPE,RESULT) nc_params[0]=1;nc_params[1]=TYPE;\
	amx_Callback(amx,n_ssocket_create,(cell*)RESULT,nc_params)

#define NC_ssocket_destroy(SOCKET) nc_params[0]=1;nc_params[1]=SOCKET;\
	amx_Callback(amx,n_ssocket_destroy,&nc_result,nc_params)

#define NC_ssocket_listen(SOCKET,PORT) nc_params[0]=2;\
	nc_params[1]=SOCKET;nc_params[2]=PORT;\
	amx_Callback(amx,n_ssocket_listen,&nc_result,nc_params)

#define NC_ssocket_send(SOCKET,DATA,LEN) nc_params[0]=3;\
	nc_params[1]=SOCKET;nc_params[2]=DATA;nc_params[3]=LEN;\
	amx_Callback(amx,n_ssocket_send,&nc_result,nc_params)

#define NC_tickcount() nc_params[0]=0;\
	amx_Callback(amx,n_tickcount,&nc_result,nc_params)

/**
Done here to do stuff, like streaming maps, anticheat?
*/
int natives_NC_SetPlayerPos(AMX *amx, int playerid, struct vec3 pos);
/**
SpawnPlayer kills players that are in vehicles, and spawns them with a bottle.

So this wrapper does SetPlayerPos (directly) if needed, because that will
eject a player.
*/
int natives_NC_SpawnPlayer(AMX *amx, int playerid);

/**
Alternative for GetPlayerPos to get it directly into a vec3 struct.

Will use buf32, buf64, buf144.
*/
int natives_NC_GetPlayerPos(AMX *amx, int playerid, struct vec3 *pos);
