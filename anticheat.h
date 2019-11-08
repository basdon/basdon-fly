
/* vim: set filetype=c ts=8 noexpandtab: */

#define AC_IF_DISALLOWED_VEHICLE 0
#define INFRACTIONTYPES 1

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
void anticheat_disallowed_vehicle_1s(AMX*, int playerid);
void anticheat_log(AMX*, int playerid, int eventtype, char *info);
void anticheat_on_player_connect(int playerid);
/**
Gets vehicle hp, after checking for unnacceptable values and handling offenders.
*/
float anticheat_NC_GetVehicleHealth(AMX*, int vehicleid);