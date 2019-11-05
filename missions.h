
/* vim: set filetype=c ts=8 noexpandtab: */

/**
Add mission distance. No effect when player is not on a mission.
*/
void missions_add_distance(int playerid, float distance_in_m);
void missions_create_tracker_socket(AMX*);
void missions_destroy_tracker_socket(AMX*);
/**
Sends flight tracker message.
*/
void missions_send_tracker_data(
	AMX*, int playerid, int vehicleid, float hp,
	struct vec3 *vpos, struct vec3 *vvel, int afk, int engine);
/**
Gets mission stage for given player, one of MISSION_STAGE_ constants.
*/
int missions_get_stage(int playerid);