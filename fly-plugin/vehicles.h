
/* vim: set filetype=c ts=8 noexpandtab: */

#define VEHICLE_RESPAWN_DELAY_MS (5 * 60 * 1000)

struct dbvehicle {
	int id;
	short model;
	/**
	userid of the player that owns this vehicle, or 0 if public
	*/
	int owneruserid;
	struct vec4 pos;
	/*TODO: isn't this data in SampVehicle struct already? since it has spawn colors and current colors*/
	unsigned char col1, col2;
	float odoKM;
	char *owner_name;
	char *owner_label_bits_data;
	short owner_label_bits_length;
	unsigned char needsodoupdate;
	/**
	Vehicleid of the spawned vehicle that reflects this db vehicle.
	If this db vehicle is not spawned, this value should be 0.
	*/
	short spawnedvehicleid;
};

struct vehicle {
	struct dbvehicle *dbvehicle;
};

extern struct vehicle gamevehicles[MAX_VEHICLES];
/**
TODO: decide where this goes
*/
extern float playerodoKM[MAX_PLAYERS];

/**
Creates a new vehicle and commits it to db.

Think twice before doing this.

Vehicle will be created and returned right away but the insert query will be
still running, so it is in some way possible that the vehicle will actually not
persist, although there won't be any feedback on this.
*/
struct dbvehicle *veh_create_new_dbvehicle(int model, struct vec4 *pos);
/**
Commits odo of next vehicle in update queue to db.

To be called in 5 second timer, or to be called in a loop until it returns 0 in
OnGameModeExit.

@return 0 if there was nothing to commit
*/
int veh_commit_next_vehicle_odo_to_db();
/**
Free memory
*/
void veh_dispose();
/**
Convenience method to get a player's vehicle data.

@return vehicleid
*/
int veh_GetPlayerVehicle(int playerid, struct dbvehicle **veh);
void veh_init();
/**
Check if a player is allowed to be in given vehicle.

Player are always allowed to be in unknown vehicles, which means veh will never
be NULL when this functions returns 0.

@param veh may be NULL.
*/
int veh_is_player_allowed_in_vehicle(int playerid, struct dbvehicle *veh);
float model_fuel_capacity(short modelid);
float model_fuel_usage(short modelid);
int veh_DestroyVehicle(int vehicleid);
void veh_load_user_model_stats(int playerid);
void veh_on_player_connect(int playerid);
void veh_on_player_disconnect(int playerid);
/**
To be called from OnPlayerEnterVehicle
*/
void veh_on_player_enter_vehicle(int pid, int vid, int ispassenger);
/**
Stuff to do when a player is now driver of a vehicle.

To be called from OnPlayerStateChange with new state being driver, or when
calling PutPlayerInVehicle when the player was already a driver (because
this won't trigger a OnPlayerStateChange.
*/
void veh_on_player_now_driving(int playerid, int vehicleid);
void veh_on_vehicle_spawn(struct dbvehicle *veh);
void veh_on_vehicle_stream_in(int vehicleid, int forplayerid);
void veh_on_vehicle_stream_out(int vehicleid, int forplayerid);
void veh_save_user_model_stats(int playerid);
/**
To be called after a player logs in.
*/
void veh_spawn_player_vehicles(int playerid);
/**
Update vehicle related things like ODO, fuel, ...

To be called every second.
*/
void veh_timed_1s_update();
void veh_timed_panel_update();
