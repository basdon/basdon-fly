
/* vim: set filetype=c ts=8 noexpandtab: */

/**
Shows and updates zone textdraw for player, to be called from OnPlayerSpawn.
*/
void zones_on_player_spawn(AMX *amx, int playerid, struct vec3 pos);
/**
Check if a single player their zone has changed, and update it if needed.

Should also be called when OnPlayerSetPos is called.
*/
void zones_update(AMX *amx, int playerid, struct vec3 pos);