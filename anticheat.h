
/* vim: set filetype=c ts=8 noexpandtab: */

#define AC_IF_DISALLOWED_VEHICLE 0
#define INFRACTIONTYPES 1

#define AC_FLOOD_DECLINE_PER_100 3
#define AC_FLOOD_AMOUNT_DIALOG 10
#define AC_FLOOD_AMOUNT_CHAT 30
#define AC_FLOOD_AMOUNT_CMD 30
#define AC_FLOOD_LIMIT 100
#define AC_FLOOD_WARN_THRESHOLD \
	(AC_FLOOD_LIMIT - AC_FLOOD_AMOUNT_CHAT - AC_FLOOD_AMOUNT_CHAT / 2)

/**
Decreate flood count for all players.

To be called from timer 100.
*/
void anticheat_decrease_flood();
/**
Decrease all infractions for all players based on each infraction's decrement.

To be called in timer5000.
*/
void anticheat_decrease_infractions();
/**
Call when a player is in a vehicle it can not be in.

Should be called at most once per second, player will be kicked when it happens
too often.
*/
void anticheat_disallowed_vehicle_1s(int playerid);
/**
Add given flood amount to given player's flood value.

Player will be kicked on excess flood.

@returns non-zero if player is kicked as result
*/
int anticheat_flood(int playerid, int amount);
/**
Log some anticheat related thing. Uses buf4096 after info is copied.

@param eventtype one of AC_ definitions
@param info primitively escaped into db query
*/
void anticheat_log(int playerid, int eventtype, char *info);
void anticheat_on_player_connect(int playerid);
/**
Stops the player from entering a vehicle they shouldn't be in.
*/
void anticheat_on_player_enter_vehicle(int pid, int vid, int ispassenger);
/**
@return zero if the message should be stopped from being sent to players.
*/
int anticheat_on_player_text(int playerid);
/**
Gets vehicle hp, after checking for unnacceptable values and handling offenders.

Only works on valid vehicles!
*/
float anticheat_GetVehicleHealth(int vehicleid);
