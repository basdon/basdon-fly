
/* vim: set filetype=c ts=8 noexpandtab: */

struct dbvehicle {
	int id;
	int model;
	int owneruserid;
	float x, y, z, r;
	int col1, col2;
	char *ownerstring;
	char ownerstringowneroffset;
};

struct vehicle {
	struct dbvehicle *dbvehicle;
};
