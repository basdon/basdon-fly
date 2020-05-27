
/* vim: set filetype=c ts=8 noexpandtab: */

/*internal use*/
#define MISSION_STAGE_NOMISSION -1
#define MISSION_STAGE_PRELOAD	1 /*taxiing to CP*/
#define MISSION_STAGE_LOAD	2 /*loading timer*/
#define MISSION_STAGE_POSTLOAD	4 /*loading done, but waiting for create query*/
#define MISSION_STAGE_FLIGHT	8 /*flight*/
#define MISSION_STAGE_UNLOAD	16 /*unloading timer*/

/*states saved in db*/
#define MISSION_STATE_INPROGRESS 1
#define MISSION_STATE_ABANDONED  2
#define MISSION_STATE_CRASHED    4
#define MISSION_STATE_FINISHED   8
#define MISSION_STATE_DECLINED   16
#define MISSION_STATE_DIED       32
#define MISSION_STATE_SERVER_ERR 64

#define MISSION_CANCEL_FINE 5000

#define MISSION_WEATHERBONUS_RAINY     1250
#define MISSION_WEATHERBONUS_FOGGY     2250
#define MISSION_WEATHERBONUS_SANDSTORM 3250

#define MISSION_WEATHERBONUS_DEVIATION 500

#define MISSION_LOAD_UNLOAD_TIME 2200
#define MISSION_CHECKPOINT_SIZE 11.0f
#define MISSION_CHECKPOINT_TYPE 2

/*dodo*/
#define MISSION_TYPE_PASSENGER_S 1
/*shamal, beagle*/
#define MISSION_TYPE_PASSENGER_M 2
/*at-400, andro*/
#define MISSION_TYPE_PASSENGER_L 4
/*beagle*/
#define MISSION_TYPE_CARGO_S 8
/*nevada*/
#define MISSION_TYPE_CARGO_M 16
/*andro*/
#define MISSION_TYPE_CARGO_L 32
/*(news,police) maverick, raindance, leviathan, sparrow*/
#define MISSION_TYPE_HELI 64
/*cargobob*/
#define MISSION_TYPE_HELI_CARGO 128
/*hunter, cargobob*/
#define MISSION_TYPE_MIL_HELI 256
/*hydra, rustler*/
#define MISSION_TYPE_MIL 512
/*andro*/
#define MISSION_TYPE_AWACS 1024
/*stuntplane*/
#define MISSION_TYPE_STUNT 2048
/*cropduster*/
#define MISSION_TYPE_CROPD 4096
/*skimmer*/
#define MISSION_TYPE_PASSENGER_WATER 8192

#define PASSENGER_MISSIONTYPES \
	(MISSION_TYPE_PASSENGER_S | MISSION_TYPE_PASSENGER_M | \
	MISSION_TYPE_PASSENGER_L | MISSION_TYPE_PASSENGER_WATER | \
	MISSION_TYPE_HELI)
/*all heli missions, including cargo*/
#define HELI_MISSIONTYPES \
	(MISSION_TYPE_HELI | MISSION_TYPE_HELI_CARGO | MISSION_TYPE_MIL_HELI)
#define MIL_MISSIONTYPES \
	(MISSION_TYPE_MIL | MISSION_TYPE_MIL_HELI)

/*excluding zero term*/
#define MAX_MSP_NAME (9)

#ifdef DEV
/**
Toggles showing closest missionpoint. Will send a message to all players when
called.

@see dev_missions_update_closest_point
*/
void dev_missions_toggle_closest_point();
/**
Dev function that will set every player's race checkpoint to whatever
missionpoint is closest to them.
*/
void dev_missions_update_closest_point();
#endif /*DEV*/

/**
Add mission distance. Has no effect when player is not on a mission.
*/
void missions_add_distance(int playerid, float distance_in_m);
/**
Callback for the query that inserts a new mission on mission creation.
*/
void missions_cb_create(void* data);
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
void missions_create_tracker_socket();
void missions_destroy_tracker_socket();
/**
Free missionpoints from airport data.
*/
void missions_freepoints();
/**
Gets distance in meters for player to next mission cp, -1 if not on a mission.
*/
int missions_get_distance_to_next_cp(int playerid, struct vec3 *frompos);
/**
Loads mission points and inits variables.
*/
void missions_init();
/**
Check if player is currently doing a mission.
*/
int missions_is_player_on_mission(int playerid);
void missions_on_player_connect(int playerid);
void missions_on_player_death(int playerid);
void missions_on_player_disconnect(int playerid);
/**
@return non-zero if the event was handled
*/
int missions_on_player_enter_race_checkpoint(int playerid);
void missions_on_player_state_changed(int playerid, int from, int to);
/**
Call when a vehicle is destroyed or respawned.
*/
void missions_on_vehicle_destroyed_or_respawned(struct dbvehicle *veh);
/**
Call when the vehicle was refueled.
*/
void missions_on_vehicle_refueled(int vehicleid, float refuelamount);
/**
Call when the vehicle was repaired.
*/
void missions_on_vehicle_repaired(int vehicleid, float fixamount, float newhp);
void missions_on_vehicle_stream_in(int vehicleid, int forplayerid);
/**
Call when weather changed to add dangerous weather bonus to missions if needed.
*/
void missions_on_weather_changed(int weather);
/**
Sends flight tracker message.
*/
void missions_send_tracker_data(
	int playerid, int vehicleid, float hp,
	struct vec3 *vpos, struct vec3 *vvel, int engine);
/**
Gets mission stage for given player, one of MISSION_STAGE_ constants.
*/
int missions_get_stage(int playerid);
/**
Updates satisfaction for player mission.

No effect when player is not on a (passenger) mission or when passed vehicle
is not the mission vehicle.
*/
void missions_update_satisfaction(int pid, int vid, struct quat *vrot);
