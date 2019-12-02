
/* vim: set filetype=c ts=8 noexpandtab: */

#define MAX_AIRPORT_NAME (24)

#define APT_FLAG_TOWERED (0x1)

#define RUNWAY_SURFACE_ASPHALT (1)
#define RUNWAY_SURFACE_DIRT (2)
#define RUNWAY_SURFACE_CONCRETE (3)
#define RUNWAY_SURFACE_GRASS (4)
#define RUNWAY_SURFACE_WATER (5)

#define RUNWAY_TYPE_RUNWAY (1)
#define RUNWAY_TYPE_HELIPAD (2)

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
void airports_init();
/**
Call when getting a response from DIALOG_AIRPORT_NEAREST, to show info dialog.
*/
void airport_list_dialog_response(int playerid, int response, int idx);
/**
Cleanup stored stuff for player when they disconnect.
*/
void airport_on_player_disconnect(int playerid);
