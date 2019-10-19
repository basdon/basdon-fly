
/* vim: set filetype=c ts=8 noexpandtab: */

#define _CRT_SECURE_NO_DEPRECATE
#include "common.h"
#include "a_samp.h"
#include "class.h"
#include "game_sa.h"

#define VINEWOOD_CAM_POS_X 1496.7052f
#define VINEWOOD_CAM_POS_Y -883.7934f
#define VINEWOOD_CAM_POS_Z 59.9061f
#define VINEWOOD_CAM_LAT_X 1395.9752f
#define VINEWOOD_CAM_LAT_Y -787.6342f
#define VINEWOOD_CAM_LAT_Z 82.1637f

const int CLASSMAPPING[] = {
	CLASS_PILOT,
	CLASS_RESCUE,
	CLASS_ARMY,
	CLASS_AID,
	CLASS_TRUCKER,
};

static const char *CLASS_NAMES[] = {
	"~p~Pilot",
	"~b~~h~~h~Rescue worker",
	"~g~~h~Army",
	"~r~~h~~h~Aid worker",
	"~y~Trucker",
};

static const int CLASS_COLORS[] = {
	0xa86efcff,
	0x7087ffff,
	0x519c42ff,
	0xff3740ff,
	0xe2c063ff,
};

int classid[MAX_PLAYERS];
int classidx[MAX_PLAYERS];

void class_init(AMX *amx)
{
	nc_params[0] = 11;
	*((float*) (nc_params + 2)) = 1488.5236f;
	*((float*) (nc_params + 3)) = -873.1125f;
	*((float*) (nc_params + 4)) = 59.3885f;
	*((float*) (nc_params + 5)) = 232.0f;
	nc_params[6] = WEAPON_CAMERA;
	nc_params[7] = 3036;
	nc_params[8] = nc_params[9] = nc_params[10] = nc_params[11] = 0;
	nc_params[1] = 61;
	NC(n_AddPlayerClass);
	nc_params[1] = 275;
	NC(n_AddPlayerClass);
	nc_params[1] = 287;
	NC(n_AddPlayerClass);
	NC(n_AddPlayerClass);
	nc_params[1] = 133;
	NC(n_AddPlayerClass);
}

void class_on_player_connect(AMX *amx, int playerid)
{
	classid[playerid] = CLASSMAPPING[classidx[playerid] = 0];
	nc_params[0] = 2;
	nc_params[1] = playerid;
	nc_params[2] = 0x888888FF;
	NC(n_SetPlayerColor);
}

/**
Class selection, sets camera, dance moves, shows class name.

May also be called by login code, after player is logged in.

@param classid class id or -1 to use already set class id
*/
void class_on_player_request_class(AMX *amx, int playerid, int _classid)
{
	int class_index;
	if (_classid < 0 || numclasses <= _classid) {
		class_index = classidx[playerid];
	} else {
		classidx[playerid] = class_index = _classid;
		classid[playerid] = CLASSMAPPING[class_index];
	}

	nc_params[0] = 4;
	nc_params[1] = playerid;
	*((float*) (nc_params + 2)) = VINEWOOD_CAM_POS_X;
	*((float*) (nc_params + 3)) = VINEWOOD_CAM_POS_Y;
	*((float*) (nc_params + 4)) = VINEWOOD_CAM_POS_Z;
	NC(n_SetPlayerCameraPos);
	*((float*) (nc_params + 2)) = VINEWOOD_CAM_LAT_X;
	*((float*) (nc_params + 3)) = VINEWOOD_CAM_LAT_Y;
	*((float*) (nc_params + 4)) = VINEWOOD_CAM_LAT_Z;
	NC(n_SetPlayerCameraLookAt);

	/*This is also called when the player is connecting, but then they
	aren't logged in yet, so SetPlayerPos to hide the player and don't
	do class thingies.*/
	PC_REMOVEME_isplaying(playerid);
	if (!pc_result) {
		/*position behind the vinewood sign, to hide during login*/
		*((float*) (nc_params + 2)) = 1415.386f;
		*((float*) (nc_params + 3)) = -807.9211f;
		*((float*) (nc_params + 4)) = 85.0615f;
	} else {
		*((float*) (nc_params + 2)) = 1486.2727f;
		*((float*) (nc_params + 3)) = -874.0833f;
		*((float*) (nc_params + 4)) = 58.8885f;
	}
	NC(n_SetPlayerPos_);

	/*special action should be done _after_ setting position*/
	/*Note: while this shouldn't be done when the player has not logged in
	yet, calling this doesn't work on first invocation (which normally
	would happen in class_on_player_request_class), so execute it here
	already so the animation can 'preload' and works on request class.*/
	nc_params[0] = 2;
	nc_params[2] = SPECIAL_ACTION_DANCE1;
	NC(n_SetPlayerSpecialAction);

	if (!pc_result) {
		/*don't do classy thingies when not logged in*/
		return;
	}

	nc_params[2] = CLASS_COLORS[class_index];
	NC(n_SetPlayerColor);

	/*facing angle only looks good when the dancing animation is applied*/
	*((float*) (nc_params + 2)) = 236.0f;
	NC(n_SetPlayerFacingAngle);

	amx_SetUString(buf32, CLASS_NAMES[class_index], 32);
	nc_params[2] = buf32a;
	nc_params[3] = 0x800000;
	nc_params[4] = 3;
	NC(n_GameTextForPlayer);
}

int class_on_player_request_spawn(AMX *amx, int playerid)
{
	static const char *NO_TRUCK = WARN"Trucker class is not available yet.";

	if (classid[playerid] == CLASS_TRUCKER) {
		amx_SetUString(buf144, NO_TRUCK, 144);
		NC_SendClientMessage(playerid, COL_WARN, buf144a);
		return 0;
	}
	common_hide_gametext_for_player(amx, playerid);
	return 1;
}
