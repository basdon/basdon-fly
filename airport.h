
/* vim: set filetype=c ts=8 noexpandtab: */

struct RUNWAY {
	char id[4];
	float heading, headingr;
	struct vec3 pos;
	int nav;
	/**
	Should be one of the RUNWAY_TYPE_ constants.
	*/
	int type;
};

struct AIRPORT {
	int id;
	struct vec3 pos;
	char code[4 + 1];
	char enabled;
	char beacon[4 + 1];
	char name[MAX_AIRPORT_NAME + 1];
	int missiontypes;
	int flags;
	struct RUNWAY *runways, *runwaysend;
	struct MISSIONPOINT *missionpoints;
	int num_missionpts;
};

extern struct AIRPORT *airports;
extern int numairports;

/**
The /beacons command, shows a dialog with list of all airport beacons.
*/
int airport_cmd_beacons(CMDPARAMS);
/**
The /nearest command, shows a list dialog with airports.

Airports are sorted by distance from player. Choosing an airport shows an
additional dialog with information about the selected airport.
*/
int airport_cmd_nearest(CMDPARAMS);
/**
Clears all data and frees all allocated memory.
*/
void airports_destroy();
/**
Loads airports and runways (and init other things).
*/
void airports_init(AMX*);
/**
Call when getting a response from DIALOG_AIRPORT_NEAREST, to show info dialog.
*/
void airport_list_dialog_response(AMX*, int playerid, int response, int idx);
/**
Cleanup stored stuff for player when they disconnect.
*/
void airport_on_player_disconnect(int playerid);
/**
To normalize vehicle hp.
*/
void anticheat_on_player_enter_vehicle(AMX*, int pid, int vid, int ispassenger);
/**
Timed check to remove players from vehicle they shouldn't be in.

Call from 1000 timer.
*/
void anticheat_disallowed_vehicle_1s(AMX *amx, int playerid);
/**
Stops the player from entering a vehicle they shouldn't be in.
*/
void anticheat_on_player_enter_vehicle(AMX*, int pid, int vid, int ispassenger);

