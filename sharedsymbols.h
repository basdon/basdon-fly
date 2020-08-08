
/* vim: set filetype=c ts=8 noexpandtab: */

#define MAX_PLAYERS (50)
#ifdef MAX_PLAYER_NAME
#assert MAX_PLAYER_NAME == 24
#else
#define MAX_PLAYER_NAME (24)
#endif
#ifdef INVALID_PLAYER_ID
#assert INVALID_PLAYER_ID == 0xFFFF
#else
#define INVALID_PLAYER_ID (0xFFFF)
#endif
#ifdef MAX_VEHICLES
#assert MAX_VEHICLES == 2000
#else
#define MAX_VEHICLES (2000)
#endif
#ifdef INVALID_VEHICLE_ID
#assert INVALID_VEHICLE_ID == 0xFFFF
#else
#define INVALID_VEHICLE_ID ((short) 0xFFFF)
#endif
#ifdef INVALID_3DTEXT_ID
#assert INVALID_3DTEXT_ID == 0xFFFF
#else
#define INVALID_3DTEXT_ID ((short) 0xFFFF)
#endif
#ifdef MAX_OBJECTS
#assert MAX_OBJECTS == 1000
#else
#define MAX_OBJECTS (1000)
#endif
#ifdef INVALID_OBJECT_ID
#assert INVALID_OBJECT_ID == 0xFFFF
#else
#define INVALID_OBJECT_ID (0xFFFF)
#endif

#define MAX_GANG_ZONES (1024)

#define LIMIT_DIALOG_CAPTION (64)
#define LIMIT_DIALOG_INFO (4096)
/* button len limit is not defined, using 32 */
#define LIMIT_DIALOG_BUTTON (32)
