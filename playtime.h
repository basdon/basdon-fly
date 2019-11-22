
/* vim: set filetype=c ts=8 noexpandtab: */

/**
Flag denoting if a player is afk.

NOTE: players in the class selection screen are marked as AFK
use lastupdate instead when needed
*/
extern char isafk[MAX_PLAYERS];
/**
Value holding timestamp of last update per player.
*/
extern unsigned long lastupdate[MAX_PLAYERS];

void playtime_init();
/**
Must be called from timer100
*/
void playtime_check_for_afk();
void playtime_on_player_connect(int playerid);
void playtime_on_player_disconnect(int playerid);
void playtime_on_player_update(int playerid);