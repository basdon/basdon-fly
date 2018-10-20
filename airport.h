
/* vim: set filetype=c ts=8 noexpandtab: */

#define ISINVALIDAIRPORT(X) ((X).code[0] == 0)

struct runway {
	char id[4];
	float heading, headingr;
	struct vec3 pos;
	int nav;
	struct runway *next;
};

struct airport {
	struct vec3 pos;
	char code[4 + 1];
	char enabled;
	char beacon[4 + 1];
	char name[MAX_AIRPORT_NAME + 1];
	struct runway *runways;
};

extern struct airport *airports;
extern int airportscount;

