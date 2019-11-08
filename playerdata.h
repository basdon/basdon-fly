
/* vim: set filetype=c ts=8 noexpandtab: */

struct playerdata {
	char ip[16];
	char name[MAX_PLAYER_NAME + 1];
	char normname[MAX_PLAYER_NAME + 1];
	char namelen;
	int userid, groups;
};

extern struct playerdata *pdata[MAX_PLAYERS];

/**
Gets given user id or NULL as string (useful for SQL formatting).

TODO: remove?
*/
void useridornull(int playerid, char *storage);
void pdata_init();
/**
Initializes player data for a player.

@param amx abstract machine
@param playerid playerid for which to initialize their data
*/
void pdata_init_player(AMX*, int playerid);
/**
Refetches the player's name and stores it in the plugin.
Will crash the server if pdata memory hasn't been allocated.

@param amx abstract machine
@param playerid player of which to refetch the name
*/
void pdata_update_name(AMX*, int playerid);