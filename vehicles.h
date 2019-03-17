
/* vim: set filetype=c ts=8 noexpandtab: */

struct dbvehicle {
	int id;
	int model;
	int owneruserid;
	float x, y, z, r;
	int col1, col2;
	char *ownerstring;
	unsigned char ownerstringowneroffset;
	short spawnedvehicleid;
};

struct vehicle {
	struct dbvehicle *dbvehicle;
};

extern struct vehicle gamevehicles[MAX_VEHICLES];
