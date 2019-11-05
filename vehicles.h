
/* vim: set filetype=c ts=8 noexpandtab: */

struct dbvehicle {
	int id;
	int model;
	int owneruserid;
	float x, y, z, r;
	int col1, col2;
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
	Recreation is needed when some vehicle properties change, like spawn
	position or colors.
	*/
	char need_recreation;
};

extern struct vehicle gamevehicles[MAX_VEHICLES];

float model_fuel_capacity(int modelid);

int veh_NC_CreateVehicle(AMX *amx, int model, float x, float y, float z,
		         float r, int col1, int col2, int respawn_delay,
		         int addsiren);
int veh_create_from_dbvehicle(AMX *amx, struct dbvehicle *veh);
int veh_NC_DestroyVehicle(AMX *amx, int vehicleid);
int veh_OnVehicleSpawn(AMX *amx, int vehicleid);
/**
Check if given player can modify a vehicle (park, spray, ..).
Player can modify if they own the vehicle or have admin rights.
*/
int veh_can_player_modify_veh(int playerid, struct dbvehicle *veh);
/**
To be called from OnPlayerEnterVehicle
*/
void veh_on_player_enter_vehicle(AMX*, int pid, int vid, int ispassenger);
void veh_on_player_state_change(AMX*, int, int, int);
void veh_init();
/**
Check if a player is allowed to be in given vehicle.

Player are always allowed to be in unknown vehicles, which means veh will never
be NULL when this functions returns 0.

@param veh may be NULL.
*/
int veh_is_player_allowed_in_vehicle(int playerid, struct dbvehicle *veh);
void veh_timed_1s_update(AMX*);
void veh_timed_panel_update(AMX*);
