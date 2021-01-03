
/* vim: set filetype=c ts=8 noexpandtab: */

struct playerdata {
	char ip[16];
	char name[MAX_PLAYER_NAME + 1];
	char normname[MAX_PLAYER_NAME + 1];
	char namelen;
	int groups;
};

extern struct playerdata *pdata[MAX_PLAYERS];

void pdata_init();
/**
Call after changing pdata->name so it can recalculate normalized name and len.
*/
void pdata_on_name_updated(int playerid);
/**
Initializes player data for a player.
*/
void pdata_on_player_connect(int playerid);
/**
Destroys player data for a player.
*/
void pdata_on_player_disconnect(int playerid);
