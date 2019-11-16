
/* vim: set filetype=c ts=8 noexpandtab: */

extern int p_OnPlayerCommandTextHash;
extern int p_REMOVEME_isafk;
extern int p_REMOVEME_isplaying;

extern cell pc_result;

/* PC = PublicCall */

#define PC_OnPlayerCommandTextHash(PLAYERID,HASH,CMDTEXT) \
	amx_Push(amx,CMDTEXT);amx_Push(amx,HASH);amx_Push(amx,PLAYERID);\
	amx_Exec(amx,&pc_result,p_OnPlayerCommandTextHash)

#define PC_REMOVEME_isafk(PLAYERID) amx_Push(amx,PLAYERID);\
	amx_Exec(amx,&pc_result,p_REMOVEME_isafk)

#define PC_REMOVEME_isplaying(PLAYERID) amx_Push(amx,PLAYERID);\
	amx_Exec(amx,&pc_result,p_REMOVEME_isplaying)

/**
Finds required exported symbols.
*/
int publics_find();