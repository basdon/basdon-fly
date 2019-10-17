
/* vim: set filetype=c ts=8 noexpandtab: */

#define ISINVALIDAIRPORT(X) ((X).code[0] == 0)

struct RUNWAY {
	char id[4];
	float heading, headingr;
	struct vec3 pos;
	int nav;
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

