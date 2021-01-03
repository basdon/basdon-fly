
/* vim: set filetype=c ts=8 noexpandtab: */

/**
Starts the time and weather system.
*/
void timecyc_init();
void timecyc_on_player_connect(int playerid);
void timecyc_on_player_death(int playerid);
void timecyc_on_player_request_class(int playerid);
void timecyc_on_player_update(int playerid);
void timecyc_on_player_was_afk(int playerid);
/**
Syncs timecyc's tick function.
*/
void timecyc_reset();
/**
Let the clock tick. This will also call most timed functions.
*/
void timecyc_tick();
