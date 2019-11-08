
/* vim: set filetype=c ts=8 noexpandtab: */

/**
The /pm cmd. /cmd <id|playername> <message>

Sends a pm to given player.
*/
int pm_cmd_pm(CMDPARAMS);
/**
The /r cmd. /r <message>

Sends a reply pm to the player that last sent a pm to the invoker.
*/
int pm_cmd_r(CMDPARAMS);
/**
Resets the last pm target for given player.
*/
void pm_on_player_connect(int playerid);
/**
Change every player that has given player as pm target to invalid target id.
*/
void pm_on_player_disconnect(int playerid);