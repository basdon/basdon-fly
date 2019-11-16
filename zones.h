
/* vim: set filetype=c ts=8 noexpandtab: */

/**
Hides the zone textdraw.

Call in OnPlayerDeath
*/
void zones_hide_text(int playerid);
void zones_init();
int zones_cmd_loc(CMDPARAMS);
void zones_on_player_connect(int playerid);
/**
Shows and updates zone textdraw for player, to be called from OnPlayerSpawn.
*/
void zones_on_player_spawn(int playerid, struct vec3 pos);
/**
Check if a single player their zone has changed, and update it if needed.

Should also be called when OnPlayerSetPos is called.
*/
void zones_update(int playerid, struct vec3 pos);
/**
Update player zone for all players, should be called periodically.
*/
void zones_update_for_all();
