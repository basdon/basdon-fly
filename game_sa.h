
/* vim: set filetype=c ts=8 noexpandtab: */

#define MODEL_TOTAL (611-400)

#define PLANE 0x1
#define HELI 0x2

#define WEAPON_BRASSKNUCKLE 1
#define WEAPON_GOLFCLUB 2
#define WEAPON_NITESTICK 3
#define WEAPON_KNIFE 4
#define WEAPON_BAT 5
#define WEAPON_SHOVEL 6
#define WEAPON_POOLSTICK 7
#define WEAPON_KATANA 8
#define WEAPON_CHAINSAW 9
#define WEAPON_DILDO 10
#define WEAPON_DILDO2 11
#define WEAPON_VIBRATOR 12
#define WEAPON_VIBRATOR2 13
#define WEAPON_FLOWER 14
#define WEAPON_CANE 15
#define WEAPON_GRENADE 16
#define WEAPON_TEARGAS 17
#define WEAPON_MOLTOV 18
#define WEAPON_COLT45 22
#define WEAPON_SILENCED 23
#define WEAPON_DEAGLE 24
#define WEAPON_SHOTGUN 25
#define WEAPON_SAWEDOFF 26
#define WEAPON_SHOTGSPA 27
#define WEAPON_UZI 28
#define WEAPON_MP5 29
#define WEAPON_AK47 30
#define WEAPON_M4 31
#define WEAPON_TEC9 32
#define WEAPON_RIFLE 33
#define WEAPON_SNIPER 34
#define WEAPON_ROCKETLAUNCHER 35
#define WEAPON_HEATSEEKER 36
#define WEAPON_FLAMETHROWER 37
#define WEAPON_MINIGUN 38
#define WEAPON_SATCHEL 39
#define WEAPON_BOMB 40
#define WEAPON_SPRAYCAN 41
#define WEAPON_FIREEXTINGUISHER 42
#define WEAPON_CAMERA 43
#define WEAPON_NVIS 44
#define WEAPON_IRVIS 45
#define WEAPON_PARACHUTE 46
/*?*/
#define WEAPON_FAKEPISTOL 47
/*Only usable in SendDeathMessage*/
#define WEAPON_VEHICLE 49
/*Only usable in SendDeathMessage*/
#define WEAPON_HELIBLADES 50
/*Only usable in SendDeathMessage*/
#define WEAPON_EXPLOSION 51
/*Only usable in SendDeathMessage*/
#define WEAPON_DROWN 53
/*Only usable in SendDeathMessage*/
#define WEAPON_COLLISION 54
/*Only usable in SendDeathMessage*/
#define WEAPON_CONNECT 200
/*Only usable in SendDeathMessage*/
#define WEAPON_DISCONNECT 201
/*Only usable in SendDeathMessage*/
#define WEAPON_SUICIDE 255

extern char *vehnames[];
extern unsigned int vehicleflags[MODEL_TOTAL];

/**
Check if the given vehicle model is an air vehicle (plane or helicopter).
*/
int game_is_air_vehicle(int model);
/**
Check if the given vehicle model is a helicopter.
*/
int game_is_heli(int model);
/**
Check if the given vehicle model is a plane.
*/
int game_is_plane(int model);

