
/* vim: set filetype=c ts=8 noexpandtab: */

int natives_find(AMX *amx);

extern int n_CreatePlayerObject;
extern int n_DestroyPlayerObject;
extern int n_DisablePlayerRaceCheckpoint;
extern int n_GetPlayerPos;
extern int n_SendClientMessage;
extern int n_SendClientMessageToAll;
extern int n_SetPlayerRaceCheckpoint;
extern int n_cache_delete;
extern int n_cache_get_row;
extern int n_cache_get_row_count;
extern int n_cache_get_row_float;
extern int n_cache_get_row_int;
extern int n_mysql_query;
extern int n_printf;
extern int n_random;
extern cell *nc_params;
extern cell nc_result;

/* NC = NativeCall */

#define NC(NATIVE) amx_Callback(amx,NATIVE,&nc_result,nc_params)

#define NC_CreatePlayerObject(PLAYERID,MODELID,FX,FY,FZ,FRX,FRY,FRZ,FDISTANCE) \
	nc_params[0]=9;nc_params[1]=PLAYERID;nc_params[2]=MODELID;\
	nc_params[3]=FX;nc_params[4]=FY;nc_params[5]=FZ;nc_params[6]=FRX;\
	nc_params[7]=FRY;nc_params[8]=FRZ;nc_params[9]=FDISTANCE;\
	amx_Callback(amx,n_CreatePlayerObject,&nc_result,nc_params)

#define NC_DestroyPlayerObject(PLAYERID,OBJECTID) \
	nc_params[0]=2;nc_params[1]=PLAYERID;nc_params[2]=OBJECTID;\
	amx_Callback(amx,n_DestroyPlayerObject,&nc_result,nc_params)

#define NC_DisablePlayerRaceCheckpoint(PLAYERID) \
	nc_params[0]=1;nc_params[1]=PLAYERID;\
	amx_Callback(amx,n_DisablePlayerRaceCheckpoint,&nc_result,nc_params)

#define NC_GetPlayerPos(PLAYERID,FREFX,FREFY,FREFZ) nc_params[0]=4;\
	nc_params[1]=PLAYERID;nc_params[2]=FREFX;nc_params[3]=FREFY;\
	nc_params[4]=FREFZ;\
	amx_Callback(amx,n_GetPlayerPos,&nc_result,nc_params)

#define NC_SendClientMessage(PLAYERID,COLOR,BUF) \
	nc_params[0]=3;\
	nc_params[1]=PLAYERID;nc_params[2]=COLOR;nc_params[3]=BUF;\
	amx_Callback(amx,n_SendClientMessage,&nc_result,nc_params)

#define NC_SendClientMessageToAll(COLOR,BUF) \
	nc_params[0]=2;nc_params[1]=COLOR;nc_params[2]=BUF;\
	amx_Callback(amx,n_SendClientMessageToAll,&nc_result,nc_params)

#define NC_SetPlayerRaceCheckpoint(PLAYERID,TYPE,FX,FY,FZ,FNX,FNY,FNZ,FSIZE) \
	nc_params[0]=9;nc_params[1]=PLAYERID;nc_params[2]=TYPE;\
	nc_params[3]=FX;nc_params[4]=FY;nc_params[5]=FZ;nc_params[6]=FNX;\
	nc_params[7]=FNY;nc_params[8]=FNZ;nc_params[9]=FSIZE;\
	amx_Callback(amx,n_SetPlayerRaceCheckpoint,&nc_result,nc_params)

#define NC_cache_delete(ID) nc_params[0]=2;nc_params[1]=ID;nc_params[2]=1;\
	amx_Callback(amx,n_cache_delete,&nc_result,nc_params)

#define NC_cache_get_row_count_(RESULT) nc_params[0]=1;nc_params[1]=1;\
	amx_Callback(amx,n_cache_get_row_count,RESULT,nc_params)
#define NC_cache_get_row_count() NC_cache_get_row_count_(&nc_result)

#define NC_cache_get_row_int(ROW,COL,INTREF) nc_params[0]=2;\
	nc_params[1]=ROW;nc_params[2]=COL;\
	amx_Callback(amx,n_cache_get_row_int,INTREF,nc_params)

#define NC_cache_get_row_flt(ROW,COL,FLOATREF) nc_params[0]=2;\
	nc_params[1]=ROW;nc_params[2]=COL;\
	amx_Callback(amx,n_cache_get_row_float,FLOATREF,nc_params)

#define NC_cache_get_row_str(ROW,COL,BUF) nc_params[0]=3;\
	nc_params[1]=ROW;nc_params[2]=COL;nc_params[3]=BUF;\
	amx_Callback(amx,n_cache_get_row,&nc_result,nc_params)

#define NC_mysql_query_(BUF,RESULT) nc_params[0]=3;nc_params[1]=1;\
	nc_params[2]=BUF;nc_params[3]=1;\
	amx_Callback(amx,n_mysql_query,RESULT,nc_params)
#define NC_mysql_query(BUF) NC_mysql_query_(BUF,&nc_result)

#define NC_random(MAX) nc_params[0]=1;nc_params[1]=MAX;\
	amx_Callback(amx,n_random,&nc_result,nc_params)
