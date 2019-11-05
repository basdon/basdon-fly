
/* vim: set filetype=c ts=8 noexpandtab: */

/**
Add mission distance.

No effect when player is not on a mission.
*/
void missions_add_distance(int playerid, float distance_in_m);
void missions_create_tracker_socket(AMX*);
void missions_destroy_tracker_socket(AMX*);
/**
Gets mission stage for given player, one of MISSION_STAGE_ constants.
*/
void missions_on_player_connect(AMX*, int playerid);
void missions_on_player_death(AMX*, int playerid);
/**
Sends flight tracker message.
*/
void missions_send_tracker_data(
	AMX*, int playerid, int vehicleid, float hp,
	struct vec3 *vpos, struct vec3 *vvel, int afk, int engine);
int missions_get_stage(int playerid);
/**
Updates satisfaction for player mission.

No effect when player is not on a (passenger) mission or when passed vehicle
is not the mission vehicle.
*/
void missions_update_satisfaction(AMX*, int pid, int vid, struct quat *vrot);