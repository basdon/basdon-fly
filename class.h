
/* vim: set filetype=c ts=8 noexpandtab: */

#define CLASS_PILOT 1
#define CLASS_RESCUE 2
#define CLASS_ARMY 4
#define CLASS_AID 8
#define CLASS_TRUCKER 16

#define numclasses 5

/**
From class index to class id
*/
extern const int CLASSMAPPING[numclasses];
/**
Class id of players.
*/
extern int classid[MAX_PLAYERS];
/**
Class index of players.
*/
extern int classidx[MAX_PLAYERS];

