
/* vim: set filetype=c ts=8 noexpandtab: */

#include "common.h"
#include "a_samp.h"
#include "class.h"
#include "game_sa.h"
#include "login.h"
#include "zones.h"

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

const int CLASS_SKINS[] = {
	61,
	275,
	287,
	287,
	133,
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

void class_init()
{
	int i;

	nc_params[0] = 11;
	nc_paramf[2] = 1488.5236f;
	nc_paramf[3] = -873.1125f;
	nc_paramf[4] = 59.3885f;
	nc_paramf[5] = 232.0f;
	nc_params[6] = SPAWN_WEAPON_1;
	nc_params[7] = SPAWN_AMMO_1;
	nc_params[8] = nc_params[10] = SPAWN_WEAPON_2_3;
	nc_params[9] = nc_params[11] = SPAWN_AMMO_2_3;

	for (i = 0; i < NUMCLASSES; i++) {
		nc_params[1] = CLASS_SKINS[i];
		NC(n_AddPlayerClass);
	}
}

void class_on_player_connect(int playerid)
{
	classid[playerid] = CLASSMAPPING[classidx[playerid] = 0];
	nc_params[0] = 2;
	nc_params[1] = playerid;
	nc_params[2] = 0x888888FF;
	NC(n_SetPlayerColor);
}

void class_on_player_request_class(int playerid, int _classid)
{
	int class_index;
	if (_classid < 0 || NUMCLASSES <= _classid) {
		class_index = classidx[playerid];
	} else {
		classidx[playerid] = class_index = _classid;
		classid[playerid] = CLASSMAPPING[class_index];
	}

	NC_PARS(4);
	nc_params[1] = playerid;
	nc_paramf[2] = VINEWOOD_CAM_POS_X;
	nc_paramf[3] = VINEWOOD_CAM_POS_Y;
	nc_paramf[4] = VINEWOOD_CAM_POS_Z;
	NC(n_SetPlayerCameraPos);
	nc_paramf[2] = VINEWOOD_CAM_LAT_X;
	nc_paramf[3] = VINEWOOD_CAM_LAT_Y;
	nc_paramf[4] = VINEWOOD_CAM_LAT_Z;
	NC(n_SetPlayerCameraLookAt);

	/*This is also called when the player is connecting, but then they
	aren't logged in yet, so SetPlayerPos to hide the player and don't
	do class thingies.*/
	if (!ISPLAYING(playerid)) {
		/*position behind the camera, to hide during login*/
		nc_paramf[2] = 1500.9938f;
		nc_paramf[3] = -891.3679f;
		nc_paramf[4] = 58.7611f;
	} else {
		nc_paramf[2] = 1486.2727f;
		nc_paramf[3] = -874.0833f;
		nc_paramf[4] = 58.8885f;
	}
	NC(n_SetPlayerPos_);

	/*special action should be done _after_ setting position*/
	/*Note: while this shouldn't be done when the player has not logged in
	yet, calling this doesn't work on first invocation (which normally
	would happen in class_on_player_request_class), so execute it here
	already so the animation can 'preload' and works on request class.*/
	NC_PARS(2);
	nc_params[2] = SPECIAL_ACTION_DANCE1;
	NC(n_SetPlayerSpecialAction);

	if (!ISPLAYING(playerid)) {
		/*don't do classy thingies when not logged in*/
		return;
	}

	nc_params[2] = CLASS_COLORS[class_index];
	NC(n_SetPlayerColor);

	/*facing angle only looks good when the dancing animation is applied*/
	nc_paramf[2] = 236.0f;
	NC(n_SetPlayerFacingAngle);

	NC_PARS(4);
	amx_SetUString(buf32, CLASS_NAMES[class_index], 32);
	nc_params[2] = buf32a;
	nc_params[3] = 0x800000;
	nc_params[4] = 3;
	NC(n_GameTextForPlayer);

	/*Can happen, eg when doing /respawn (uses TogglePlayerSpectating,
	which won't hide it.*/
	zones_hide_text(playerid);
}

int class_on_player_request_spawn(int playerid)
{
	static const char *NO_TRUCK = WARN"Trucker class is not available yet.";

	if (classid[playerid] == CLASS_TRUCKER) {
		amx_SetUString(buf144, NO_TRUCK, 144);
		NC_SendClientMessage(playerid, COL_WARN, buf144a);
		return 0;
	}
	common_hide_gametext_for_player(playerid);
	return 1;
}
