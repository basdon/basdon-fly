
/* vim: set filetype=c ts=8 noexpandtab: */

struct dbvehicle {
	int id;
	int model;
	int owneruserid;
	float x, y, z, r;
	int col1, col2;
	float odo;
	char *ownerstring;
	unsigned char ownerstringowneroffset;
	unsigned char needsodoupdate;
	short spawnedvehicleid;
};

struct vehicle {
	struct dbvehicle *dbvehicle;
};

extern struct vehicle gamevehicles[MAX_VEHICLES];
