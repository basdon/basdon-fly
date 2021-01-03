
/* vim: set filetype=c ts=8 noexpandtab: */

#define CMDPARAMS const int playerid,const char *cmdtext,int parseidx

/*
Checks incoming command and calls handler if one found and group matched.
*/
int cmd_check(const int playerid, const char *cmdtext);
/*
Gets next int parameter in cmdtext after parseidx.

Preceding whitespace(s) are skipped.
On match, parseidx is the index right after the value, so either space or \0.

@return non-zero on success, with int in value parameter.
*/
int cmd_get_int_param(const char *cmdtext, int *parseidx, int *value);
/*
Gets next player parameter in cmdtext after parseidx.

Preceding whitespace(s) are skipped.
On match, parseidx is the index right after the value, so either space or \0.
If a valid player id was given but id is not taken, INVALID_PLAYER_ID is used.

@return non-zero on success, with playerid in playerid parameter.
*/
int cmd_get_player_param(const char *cmdtext, int *parseidx, int *playerid);
/*
Gets next string parameter in cmdtext after parseidx.

Preceding whitespace(s) are skipped.
On match, parseidx is the index right after the value, so either space or \0.

@return non-zero on success, with filled in buffer.
*/
int cmd_get_str_param(const char* cmdtext, int *parseidx, char *buf);
/*
Precalcs all command hashes.
*/
void cmd_init();
