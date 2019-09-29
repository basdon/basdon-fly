
/* vim: set filetype=c ts=8 noexpandtab: */

int publics_find(AMX *amx);

extern int p_OnPlayerCommandTextHash;

extern cell pc_result;

/* PC = PublicCall */

#define PC_OnPlayerCommandTextHash(PLAYERID,HASH,CMDTEXT) \
	amx_Push(amx,CMDTEXT);amx_Push(amx,HASH);amx_Push(amx,PLAYERID);\
	amx_Exec(amx,&pc_result,p_OnPlayerCommandTextHash)
