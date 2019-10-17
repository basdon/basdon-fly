
/* vim: set filetype=c ts=8 noexpandtab: */

struct SPAWN {
	/**
	Spawn name, this should be the airport name.
	*/
	char *name;
	float x, y, z, r;
};

/**
All available spawn positions.

Filled in by airports_init.
May be NULL.
*/
extern struct SPAWN *spawns;
/**
Amount of available spawns.

Filled in by airports_init.
May be 0.
*/
extern int numspawns;

/**
Function to init spawn stuff.

Mainly the text of the list dialog that are shown to the players.
Called by airports_init.
May only be called if there are actually spawns.
*/
void spawn_init();

