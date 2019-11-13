
/* vim: set filetype=c ts=8 noexpandtab: */

/**
The /metar command shows information about the current weather.

Alias /weather.
*/
int timecyc_cmd_metar(CMDPARAMS);
#ifdef DEV
/**
Dev /fweather <weather> command to force weather now.
*/
int timecyc_cmd_dev_fweather(CMDPARAMS);
/**
Dev /tweather <weather> command to change weather to given id, with transition.
*/
int timecyc_cmd_dev_tweather(CMDPARAMS);
/**
Dev /nweather to change the weather, as if it's called by the timer.
*/
int timecyc_cmd_dev_nweather(CMDPARAMS);
#endif /*DEV*/
/**
Starts the time and weather system.
*/
void timecyc_init(AMX*);
void timecyc_on_player_connect(int playerid);
void timecyc_on_player_death(AMX*, int playerid);
void timecyc_on_player_request_class(AMX*, int playerid);
void timecyc_on_player_update(AMX*, int playerid);
void timecyc_on_player_was_afk(AMX*, int playerid);
/**
Let the clock tick. This will also call most timed functions.
*/
void timecyc_tick(AMX*);
