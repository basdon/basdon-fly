#define VINEWOOD_CAM_POS_X 1496.7052f
#define VINEWOOD_CAM_POS_Y -883.7934f
#define VINEWOOD_CAM_POS_Z 59.9061f
#define VINEWOOD_CAM_LAT_X 1395.9752f
#define VINEWOOD_CAM_LAT_Y -787.6342f
#define VINEWOOD_CAM_LAT_Z 82.1637f

static
void class_init()
{
	/*we skip doing AddPlayerClass and just set numAvailableSpawns directly*/
	/*we overwrote SAMP's OnPlayerRequestClass anyways, so the spawninfo data that AddPlayerClass adds isn't needed*/
	samp_pNetGame->numAvailableSpawns = SETTING__NUM_CLASSES;
}

static
void class_on_player_connect(int playerid)
{
	classid[playerid] = 0;
	nc_params[0] = 2;
	nc_params[1] = playerid;
	nc_params[2] = 0x888888FF;
	NC(n_SetPlayerColor);
}

static
void class_on_player_request_class(int playerid, int _classid)
{
	if (_classid < 0 || SETTING__NUM_CLASSES <= _classid) {
		_classid = classid[playerid];
	} else {
		classid[playerid] = _classid;
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
	aren't logged in yet, so use SetPlayerPos to hide the player and
	don't do class thingies.*/
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

	nc_params[2] = CLASS_COLORS[_classid];
	NC(n_SetPlayerColor);

	/*This facing angle may seem a bit strange but it's adjusted for when the dancing animation is applied.*/
	nc_paramf[2] = 236.0f;
	NC(n_SetPlayerFacingAngle);

	GameTextForPlayer(playerid, 0x800000, 3, (char*) CLASS_NAMES[_classid]);
}

static
int class_on_player_request_spawn(int playerid)
{
	HideGameTextForPlayer(playerid);
	return 1;
}
