
/* vim: set filetype=c ts=8 noexpandtab: */

/**
Hides the ILS text for passengers of given vehicle.
*/
void panel_hide_ils_for_passengers(AMX*, int vehicleid);
/**
Hides the VOR bar for passengers of given vehicle.
*/
void panel_hide_vor_bar_for_passengers(AMX*, int vehicleid);
void panel_on_gamemode_init(AMX*);
void panel_on_player_connect(AMX*, int playerid);
void panel_on_player_state_change(AMX*, int playerid, int from, int to);
void panel_on_player_was_afk(AMX*, int playerid);
/**
Shows the ILS text for passengers of given vehicle.
*/
void panel_show_ils_for_passengers(AMX*, int vehicleid);
/**
Remove player from list of players that need panel updates.

No effect if given player is not a panel player.
*/
void panel_remove_panel_player(int playerid);
/**
Resets nav indicators for all players in given vehicle.
*/
void panel_reset_nav_for_passengers(AMX*, int vehicleid);
/**
Show the VOR bar for passengers of given vehicle.
*/
void panel_show_vor_bar_for_passengers(AMX*, int vehicleid);
/**
Panel loop. Call from timer100.
*/
void panel_timed_update(AMX*);