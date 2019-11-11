
/* vim: set filetype=c ts=8 noexpandtab: */

#define MISSION_LOAD_UNLOAD_TIME 2200
#define MISSION_CHECKPOINT_SIZE 11.0f

/*excluding zero term*/
#define MAX_MSP_NAME (9)

#ifdef DEV
/**
Toggles showing closest missionpoint. Will send a message to all players when
called.

@see dev_missions_update_closest_point
*/
void dev_missions_toggle_closest_point(AMX*);
/**
Dev function that will set every player's race checkpoint to whatever
missionpoint is closest to them.
*/
void dev_missions_update_closest_point(AMX*);
#endif /*DEV*/

/**
Add mission distance. Has no effect when player is not on a mission.
*/
void missions_add_distance(int playerid, float distance_in_m);
/**
Callback for the query that inserts a new mission on mission creation.
*/
void missions_cb_create(AMX *amx, void* data);
/**
The /automission cmd, toggles automatically starting new mission on finish.

Aliases: /autow
*/
int missions_cmd_automission(CMDPARAMS);
/**
The /cancelmission cmd, stops current mission for the player, for a fee.

Aliases: /s
*/
int missions_cmd_cancelmission(CMDPARAMS);
/**
The /mission cmd, starts a new mission

Aliases: /w /m
*/
int missions_cmd_mission(CMDPARAMS);
void missions_create_tracker_socket(AMX*);
void missions_destroy_tracker_socket(AMX*);
/**
Free missionpoints from airport data.
*/
void missions_freepoints();
/**
Loads mission points and inits variables.
*/
void missions_init(AMX*);
/**
Check if player is currently doing a mission.
*/
int missions_is_player_on_mission(int playerid);
void missions_on_player_connect(AMX*, int playerid);
void missions_on_player_death(AMX*, int playerid);
void missions_on_player_disconnect(AMX*, int playerid);
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
/**
Updates satisfaction for player mission.

No effect when player is not on a (passenger) mission or when passed vehicle
is not the mission vehicle.
*/
void missions_update_satisfaction(AMX*, int pid, int vid, struct quat *vrot);