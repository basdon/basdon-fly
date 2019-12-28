
/* vim: set filetype=c ts=8 noexpandtab: */

#define CLASS_PILOT 1
#define CLASS_RESCUE 2
#define CLASS_ARMY 4
#define CLASS_AID 8
#define CLASS_TRUCKER 16

#define NUMCLASSES /*5*/ 4

#define SPAWN_WEAPON_1 WEAPON_CAMERA
#define SPAWN_AMMO_1 3036
#define SPAWN_WEAPON_2_3 0
#define SPAWN_AMMO_2_3 0

/**
From class index (SAMP) to class id (CLASS_ constants)
*/
extern const int CLASSMAPPING[NUMCLASSES];
/**
Skin ID per class.
*/
extern const int CLASS_SKINS[NUMCLASSES];
/**
Class id of players (CLASS_ constants).
*/
extern int classid[MAX_PLAYERS];
/**
Class index of players (SA-MP's classid).
*/
extern int classidx[MAX_PLAYERS];

void class_init();
void class_on_player_connect(int playerid);
/**
Class selection, sets camera, dance moves, shows class name.

May also be called by login code, after player is logged in.

@param classid class id or -1 to use already set class id
*/
void class_on_player_request_class(int playerid, int _classid);
/**
Hides class selection gametext.
*/
int class_on_player_request_spawn(int playerid);
