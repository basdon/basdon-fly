
/* vim: set filetype=c ts=8 noexpandtab: */

struct playerdata {
	char ip[16];
	char name[MAX_PLAYER_NAME + 1];
	char normname[MAX_PLAYER_NAME + 1];
	char namelen;
	/**
	User id of player.

	May be -1 in rare cases when login failed to create a guest account,
	so always check in db queries to use NULL instead when needed.
	*/
	int userid;
	int groups;
};

extern struct playerdata *pdata[MAX_PLAYERS];

void pdata_init();
/**
Initializes player data for a player.

@param playerid playerid for which to initialize their data
*/
void pdata_init_player(int playerid);
/**
Refetches the player's name and stores it in the plugin.
Will crash the server if pdata memory hasn't been allocated.

@param playerid player of which to refetch the name
*/
void pdata_update_name(int playerid);
