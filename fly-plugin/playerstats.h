
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

void playerstats_init();
/**
Must be called from timer100
*/
void playerstats_check_for_afk();
void playerstats_on_player_connect(int playerid);
void playerstats_on_player_disconnect(int playerid);
void playerstats_on_player_update(int playerid);