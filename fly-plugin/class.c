struct vec3 camera_vinewood_pos = { 1496.7052f, -883.7934f, 59.9061f };
struct vec3 camera_vinewood_at = { 1395.9752f, -787.6342f, 82.1637f };
struct vec3 player_pos_behind_camera = { 1500.9938f, -891.3679f, 58.7611f };
struct vec3 player_pos_infront_camera = { 1486.2727f, -874.0833f, 58.8885f };

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
	SetPlayerColor(playerid, 0x888888FF);
}

static
void class_on_player_request_class(int playerid, int _classid)
{
	if (_classid < 0 || SETTING__NUM_CLASSES <= _classid) {
		_classid = classid[playerid];
	} else {
		classid[playerid] = _classid;
	}

	SetPlayerCameraPos(playerid, &camera_vinewood_pos);
	SetPlayerCameraLookAt(playerid, &camera_vinewood_at, CAMERA_JUMPCUT);
	/*This is also called when the player is connecting, but then they
	aren't logged in yet, so use SetPlayerPos to hide the player and
	don't do class thingies.*/
	/*When not logged in, set player position behind the camera so to hide the player during login*/
	/*Using SetPlayerPosRaw because we don't need streaming/mapicon etc etc updates when setting pos here*/
	SetPlayerPosRaw(playerid, ISPLAYING(playerid) ? &player_pos_infront_camera : &player_pos_behind_camera);

	/*special action should be done _after_ setting position*/
	/*Note: while this doesn't need to be done when the player has not logged in
	yet (because you can't see the player yet in that case), calling this doesn't
	work on first invocation (which normally would happen in class_on_player_request_class),
	so execute it here already so the animation can 'preload' and works on request class.*/
	SetPlayerSpecialAction(playerid, SPECIAL_ACTION_DANCE1);

	if (!ISPLAYING(playerid)) {
		/*don't do classy thingies when not logged in*/
		return;
	}

	SetPlayerColor(playerid, CLASS_COLORS[_classid]);
	/*This facing angle may seem a bit strange but it's adjusted for when the dancing animation is applied.*/
	SetPlayerFacingAngle(playerid, 236.0f);

	GameTextForPlayer(playerid, 0x800000, 3, (char*) CLASS_NAMES[_classid]);
}

static
int class_on_player_request_spawn(int playerid)
{
	HideGameTextForPlayer(playerid);
	return 1;
}
