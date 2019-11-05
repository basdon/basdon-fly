
/* vim: set filetype=c ts=8 noexpandtab: */

#define AC_IF_DISALLOWED_VEHICLE 0
#define INFRACTIONTYPES 1

/**
Call when a player is in a vehicle it can not be in.

Should be called at most once per second, player will be kicked when it happens
too often.
*/
void anticheat_disallowed_vehicle_1s(AMX *amx, int playerid);
void anticheat_log(AMX *amx, int playerid, int eventtype, char *info);
/**
To be called in timer5000.
*/
void anticheat_decrease_infractions();