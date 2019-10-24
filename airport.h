
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
	struct missionpoint *missionpoints;
};

extern struct AIRPORT *airports;
extern int numairports;

