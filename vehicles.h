
/* vim: set filetype=c ts=8 noexpandtab: */

#define VEHICLE_RESPAWN_DELAY 300
#define MAX_SERVICE_MAP_ICONS 4

struct dbvehicle {
	int id;
	short model;
	/**
	userid of the player that owns this vehicle, or 0 if public
	*/
	int owneruserid;
	struct vec4 pos;
	unsigned char col1, col2;
	float odoKM;
	float fuel;
	char *ownerstring;
	unsigned char ownerstringowneroffset;
	unsigned char needsodoupdate;
	/**
	Vehicleid of the spawned vehicle that reflects this db vehicle.
	If this db vehicle is not spawned, this value should be 0.
	*/
	short spawnedvehicleid;
};

struct vehicle {
	struct dbvehicle *dbvehicle;
	/**
	Number holding the amount of times this vehicle has been respawned.
	Can be used to check if a vehicleid is still assigned to the same
	vehicle as earlier.
	*/
	short reincarnation;
	/**
	Flag denoting if this vehicle needs to be recreated. This should be
	checked whenever the vehicle is respawned (see OnVehicleSpawn).
	Recreation is needed when the spawn rotation changed, because that can't
	be set when there is no passenger...
	*/
	char need_recreation;
};

extern struct vehicle gamevehicles[MAX_VEHICLES];
/**
Last vehicle id the player was in as driver.
*/
extern int lastvehicle[MAX_PLAYERS];
/**
TODO: decide where this goes
*/
extern float playerodoKM[MAX_PLAYERS];

/**
Should be called in OnGameModeInit.
*/
void veh_create_global_textdraws();
/**
Creates a new vehicle and commits it to db.

Think twice before doing this.
*/
struct dbvehicle *veh_create_new_dbvehicle(int model, struct vec4 *pos);
/**
Should be called in OnPlayerConnect.
*/
void veh_create_player_textdraws(int playerid);
/**
Check if given player can modify a vehicle (park, spray, ..).
Player can modify if they own the vehicle or have admin rights.
*/
int veh_can_player_modify_veh(int playerid, struct dbvehicle *veh);
/**
Commits odo of next vehicle in update queue to db.

To be called in 5 second timer, or to be called in a loop until it returns 0 in
OnGameModeExit.

@return 0 if there was nothing to commit
*/
int veh_commit_next_vehicle_odo_to_db();
/**
Create a vehicle from given dbvehicle.

Will create the vehicle with bogus position and colors, then respawns the
vehicle. The rotation is instantly correct because that can't be changed
afterwards.

@return vehicle id (might be INVALID_VEHICLE_ID)
*/
int veh_create(struct dbvehicle *veh);
/**
Free memory
*/
void veh_dispose();
/**
Convenience method to get a player's vehicle data.

@param reinc will be set to reincarnation value (may be NULL if not needed)
@return vehicleid
*/
int veh_GetPlayerVehicle(int playerid, int *reinc, struct dbvehicle **veh);
void veh_init();
/**
Check if a player is allowed to be in given vehicle.

Player are always allowed to be in unknown vehicles, which means veh will never
be NULL when this functions returns 0.

@param veh may be NULL.
*/
int veh_is_player_allowed_in_vehicle(int playerid, struct dbvehicle *veh);
float model_fuel_capacity(short modelid);
int veh_DestroyVehicle(int vehicleid);
void veh_on_player_connect(int playerid);
void veh_on_player_disconnect(int playerid);
/**
To be called from OnPlayerEnterVehicle
*/
void veh_on_player_enter_vehicle(int pid, int vid, int ispassenger);
/**
Used check if pilots are still controlling the plane and engine key.
*/
void veh_on_player_key_state_change(int playerid, int oldkeys, int newkeys);
/**
Stuff to do when a player is now driver of a vehicle.

To be called from OnPlayerStateChange with new state being driver, or when
calling PutPlayerInVehicle when the player was already a driver (because
this won't trigger a OnPlayerStateChange.
*/
void veh_on_player_now_driving(int playerid, int vehicleid);
void veh_on_player_state_change(int, int, int);
void veh_on_vehicle_spawn(struct dbvehicle *veh);
void veh_on_vehicle_stream_in(int vehicleid, int forplayerid);
void veh_on_vehicle_stream_out(int vehicleid, int forplayerid);
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
/**
Updates vehicle and player odo.

Given player must be driver of given vehicle.

@param pos the position of the given vehicle
*/
void veh_update_odo(int playerid, int vehicleid, struct vec3 pos);
