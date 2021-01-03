#define MAX_PLAYERS (50)
#define MAX_PLAYER_NAME (24)
#define INVALID_PLAYER_ID (0xFFFF)
#define MAX_VEHICLES (2000)
#define INVALID_VEHICLE_ID ((short) 0xFFFF)
#define INVALID_3DTEXT_ID ((short) 0xFFFF)
#define MAX_OBJECTS (1000)
#define INVALID_OBJECT_ID (0xFFFF)

#define MAX_GANG_ZONES (1024)

#define LIMIT_DIALOG_CAPTION (64)
#define LIMIT_DIALOG_INFO (4096)
/* button len limit is not defined, using 32 */
#define LIMIT_DIALOG_BUTTON (32)

#define DIALOG_STYLE_MSGBOX 0
#define DIALOG_STYLE_INPUT 1
#define DIALOG_STYLE_LIST 2
#define DIALOG_STYLE_PASSWORD 3
#define DIALOG_STYLE_TABLIST 4
#define DIALOG_STYLE_TABLIST_HEADERS 5

#define TD_ALIGNMENT_LEFT 1
#define TD_ALIGNMENT_CENTER 2
#define TD_ALIGNMENT_RIGHT 3

#define SPECIAL_ACTION_NONE 0
#define SPECIAL_ACTION_DUCK 1
#define SPECIAL_ACTION_USEJETPACK 2
#define SPECIAL_ACTION_ENTER_VEHICLE 3
#define SPECIAL_ACTION_EXIT_VEHICLE 4
#define SPECIAL_ACTION_DANCE1 5
#define SPECIAL_ACTION_DANCE2 6
#define SPECIAL_ACTION_DANCE3 7
#define SPECIAL_ACTION_DANCE4 8
#define SPECIAL_ACTION_HANDSUP 10
#define SPECIAL_ACTION_USECELLPHONE 11
#define SPECIAL_ACTION_SITTING 12
#define SPECIAL_ACTION_STOPUSECELLPHONE 13
#define SPECIAL_ACTION_DRINK_BEER 20
#define SPECIAL_ACTION_SMOKE_CIGGY 21
#define SPECIAL_ACTION_DRINK_WINE 22
#define SPECIAL_ACTION_DRINK_SPRUNK 23
#define SPECIAL_ACTION_CUFFED 24
#define SPECIAL_ACTION_CARRY 25
#define SPECIAL_ACTION_PISSING 68

#define PLAYER_STATE_NONE (0)
#define PLAYER_STATE_ONFOOT (1)
#define PLAYER_STATE_DRIVER (2)
#define PLAYER_STATE_PASSENGER (3)
#define PLAYER_STATE_EXIT_VEHICLE (4) // (used internally)
#define PLAYER_STATE_ENTER_VEHICLE_DRIVER (5) // (used internally)
#define PLAYER_STATE_ENTER_VEHICLE_PASSENGER (6) // (used internally)
#define PLAYER_STATE_WASTED (7)
#define PLAYER_STATE_SPAWNED (8)
#define PLAYER_STATE_SPECTATING (9)

#define MAPICON_LOCAL 0
#define MAPICON_GLOBAL 1
#define MAPICON_LOCAL_CHECKPOINT 2
#define MAPICON_GLOBAL_CHECKPOINT 3

#define NO_TEAM 255

#define KEY_ACTION 1
#define KEY_CROUCH 2
#define KEY_FIRE 4
#define KEY_SPRINT 8
#define KEY_VEHICLE_ACCELERATE KEY_SPRINT
#define KEY_SECONDARY_ATTACK 16
#define KEY_VEHICLE_ENTER_EXIT KEY_SECONDARY_ATTACK
#define KEY_JUMP 32
#define KEY_VEHICLE_BRAKE KEY_JUMP
#define KEY_LOOK_RIGHT 64
#define KEY_HANDBRAKE 128
#define KEY_AIM KEY_HANDBRAKE
#define KEY_LOOK_LEFT 256
#define KEY_LOOK_BEHIND 320
#define KEY_SUBMISSION 512
#define KEY_WALK 1024
#define KEY_ANALOG_UP 2048
#define KEY_ANALOG_DOWN 4096
#define KEY_ANALOG_LEFT 8192
#define KEY_ANALOG_RIGHT 16384
#define KEY_YES 65536
#define KEY_NO 131072
#define KEY_CTRL_BACK 262144
/*
#define KEY_UP -128
#define KEY_DOWN 128
#define KEY_LEFT -128
#define KEY_RIGHT 128
*/

#define RACE_CP_TYPE_ARROW 0
#define RACE_CP_TYPE_FINISH 1
#define RACE_CP_TYPE_NORMAL 2
#define RACE_CP_TYPE_AIR_NORMAL 3
#define RACE_CP_TYPE_AIR_FINISH 4
#define RACE_CP_TYPE_AIR_ROTATE_MAYBE 5
#define RACE_CP_TYPE_AIR_UP_DOWN_GONE 6
#define RACE_CP_TYPE_AIR_UP_DOWN1 7
#define RACE_CP_TYPE_AIR_UP_DOWN2 8
