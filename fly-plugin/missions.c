#define TRACKER_PORT 7766

#define MAX_SPEED_SQ ((3.0f / VEL_TO_KPH) * (3.0f / VEL_TO_KPH))

static struct TEXTDRAW td_satisfaction = { "textdraw", 50, NULL };

/**
Keeps track how much missions a playerid has started and/or stopped (number increased on mission start AND stop).
Pointers to missions must not be passed in callback data, since they could be free'd before that happens.
Instead, callbacks should pass this number, and if it is equal to the current number it means that the
activemission[playerid] is still the same mission as it was when the async func was started.
*/
static int number_missions_started_stopped[MAX_PLAYERS];

struct MISSION {
	int id;
	int missiontype;
	struct MISSIONPOINT *startpoint, *endpoint;
	float distance, actualdistanceM;
	int passenger_satisfaction;
	struct dbvehicle *veh;
	/**
	The vehicleid of the vehicle the mission was started with.
	*/
	int vehicleid;
	/**
	Reincarnation value of vehicleid when the mission was started.

	Use in combination with vehicleid to check if vehicle at the end is
	the same vehicle the mission was started with.
	*/
	int vehicle_reincarnation;
	time_t starttime;
	short lastvehiclehp, damagetaken;
	float lastfuel, fuelburned;
	short weatherbonus;
};

/**
The selected option in the job help screen, value should be 0-2.
*/
static char mission_help_option[MAX_PLAYERS];
/**
Available mission point types for player.
Usages are a.o. for in job help/map screen and to show mission point enexes.
*/
static int missions_available_msptype_mask[MAX_PLAYERS];
/**
Mission point index for mission indicator slots.
*/
static short missionpoint_indicator_index[MAX_PLAYERS][MAX_MISSION_INDICATORS];
#define MISSIONPOINT_INDICATOR_STATE_FREE 0
#define MISSIONPOINT_INDICATOR_STATE_USED 1
#define MISSIONPOINT_INDICATOR_STATE_AVAILABLE 2 /*indicator created, but it can be discarded*/
/**
See MISSIONPOINT_INDICATOR_STATE_* defs.
*/
static char missionpoint_indicator_state[MAX_PLAYERS][MAX_MISSION_INDICATORS];
/**
Holds the index of the missionpoint the player is in range of, -1 when none.
When not -1, the missionpoint is always valid for the vehicle the player is driver in.
*/
static short active_msp_index[MAX_PLAYERS];
/**
Holds the index of the missionpoint that has an active checkpoint for player, for locating purposes.
When none, -1.
*/
static short locating_msp_index[MAX_PLAYERS];
/**
See {@code MISSION_STAGE} definitions.
*/
static char mission_stage[MAX_PLAYERS];
static struct MISSION *activemission[MAX_PLAYERS];
/*whether one flightdata data packet with afk flag has been sent already*/
static char tracker_afk_packet_sent[MAX_PLAYERS];
/**
Tracker socket handle.
*/
static int tracker;

/*Help menu textdraws*/
#define NUM_HELP_TEXTDRAWS (17)
static struct TEXTDRAW td_jobhelp_keyhelp = { "keyhelp", TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_jobhelp_header = { "header", TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_jobhelp_optsbg = { "optsbg", TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_jobhelp_text = { "text", TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_jobhelp_greenbtnbg = { "greenbtnbg", TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_jobhelp_bluebtnbg = { "bluebtnbg", TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_jobhelp_redbtnbg = { "redbtnbg", TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_jobhelp_optselbg = { "optselbg", TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_jobhelp_enexgreen = { "enexgreen", TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_jobhelp_txtgreen = { "txtgreen", TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_jobhelp_actiongreen = { "actiongreen", TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_jobhelp_enexblue = { "enexblue", TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_jobhelp_txtblue = { "txtblue", TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_jobhelp_actionblue = { "actionblue", TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_jobhelp_enexred = { "enexred", TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_jobhelp_txtred = { "txtred", TEXTDRAW_ALLOC_AS_NEEDED, NULL };
static struct TEXTDRAW td_jobhelp_actionred = { "actionred", TEXTDRAW_ALLOC_AS_NEEDED, NULL };

static
int missions_get_weatherbonus_for_weather(int weather)
{
	switch (weather) {
	case WEATHER_SF_RAINY:
	case WEATHER_CS_RAINY:
		return MISSION_WEATHERBONUS_RAINY + NC_random(MISSION_WEATHERBONUS_DEVIATION);
	case WEATHER_SF_FOGGY:
		return MISSION_WEATHERBONUS_FOGGY + NC_random(MISSION_WEATHERBONUS_DEVIATION);
	case WEATHER_DE_SANDSTORMS:
		return MISSION_WEATHERBONUS_SANDSTORM + NC_random(MISSION_WEATHERBONUS_DEVIATION);
	default:
		return 0;
	}
}

static
int missions_get_initial_weatherbonus()
{
	register int a, b, c;

	a = missions_get_weatherbonus_for_weather(weather.current);
	b = missions_get_weatherbonus_for_weather(weather.upcoming);
	c = missions_get_weatherbonus_for_weather(weather.locked);
	if (b > a) {
		a = b;
	}
	if (c > a) {
		return c;
	}
	return a;
}

/**
Must only be called if the player has a valid active missionpoint.
*/
static
void missions_get_current_msp_and_mission_type(int playerid, struct MISSIONPOINT **msp, unsigned int *mission_type)
{
	*msp = &missionpoints[active_msp_index[playerid]];
	*mission_type = missions_available_msptype_mask[playerid] & (*msp)->type;
}

static
void missions_hide_jobmap_set_stage_set_controllable(int playerid)
{
	jobmap_hide(playerid);
	mission_stage[playerid] = MISSION_STAGE_NOMISSION;
	TogglePlayerControllable(playerid, 1);
}

/**
Must only be called if the player has a valid active missionpoint.
*/
static
void missions_show_jobmap_set_stage_set_controllable(int playerid)
{
	struct MISSIONPOINT *msp;
	unsigned int mission_type;

	missions_get_current_msp_and_mission_type(playerid, &msp, &mission_type);
	jobmap_show(playerid, mission_type, msp);
	mission_stage[playerid] = MISSION_STAGE_JOBMAP;
	TogglePlayerControllable(playerid, 0);
}

/**
@return mask of MISSION_POINT_* values
*/
static
int msptype_mask_to_point_mask(int msptype_mask)
{
	int point_mask;

	point_mask = 0;
	if (msptype_mask & PASSENGER_MISSIONTYPES) {
		point_mask |= MISSION_POINT_PASSENGERS;
	}
	if (msptype_mask & CARGO_MISSIONTYPES) {
		point_mask |= MISSION_POINT_CARGO;
	}
	if (msptype_mask & ~(PASSENGER_MISSIONTYPES | CARGO_MISSIONTYPES)) {
		point_mask |= MISSION_POINT_SPECIAL;
	}

	return point_mask;
}

/**
@return mask of MISSION_TYPE_* values
*/
static
int missions_get_vehicle_model_msptype_mask(int model)
{
	switch (model) {
	case MODEL_ANDROM:
		return MISSION_TYPE_PASSENGER_L | MISSION_TYPE_CARGO_L;
	case MODEL_AT400:
		return MISSION_TYPE_PASSENGER_L | MISSION_TYPE_CARGO_L;
	case MODEL_BEAGLE:
		return MISSION_TYPE_PASSENGER_M | MISSION_TYPE_CARGO_M;
	case MODEL_CARGOBOB:
		return MISSION_TYPE_MIL_HELI | MISSION_TYPE_HELI_CARGO;
	case MODEL_DODO:
		return MISSION_TYPE_PASSENGER_S | MISSION_TYPE_CARGO_S;
	case MODEL_LEVIATHN:
		return MISSION_TYPE_HELI | MISSION_TYPE_HELI_CARGO;
	case MODEL_MAVERICK:
		return MISSION_TYPE_HELI;
	case MODEL_NEVADA:
		return MISSION_TYPE_PASSENGER_M | MISSION_TYPE_CARGO_M;
	case MODEL_RAINDANC:
		return MISSION_TYPE_HELI | MISSION_TYPE_HELI_CARGO;
	case MODEL_SKIMMER:
		return MISSION_TYPE_PASSENGER_WATER;
	case MODEL_POLMAV:
		return MISSION_TYPE_HELI;
	case MODEL_SPARROW:
		return MISSION_TYPE_HELI;
	case MODEL_SHAMAL:
		return MISSION_TYPE_PASSENGER_M;
	case MODEL_VCNMAV:
		return MISSION_TYPE_HELI;
	case MODEL_HUNTER:
		return MISSION_TYPE_MIL_HELI;
	case MODEL_HYDRA:
		return MISSION_TYPE_MIL;
	case MODEL_RUSTLER:
		return MISSION_TYPE_MIL;
	default:
		return 0;
	}
}

/**
TODO: prioritize mission points that have the least amount of active missions

@return NULL if none applicable for given type mast
*/
static
struct MISSIONPOINT *missions_get_random_msp(struct AIRPORT *airport, unsigned int mission_type_mask)
{
	register struct MISSIONPOINT *tmp_msp;
	struct MISSIONPOINT *applicable_missionpoints[MAX_MISSIONPOINTS_PER_AIRPORT];
	int num_applicable_msp, mspidx;

	/*Collect all applicable missionpoints.*/
	num_applicable_msp = 0;
	for (mspidx = 0; mspidx < airport->num_missionpts; mspidx++) {
		tmp_msp = airport->missionpoints + mspidx;
		if (tmp_msp->type & mission_type_mask) {
			applicable_missionpoints[num_applicable_msp++] = tmp_msp;
		}
	}

	if (!num_applicable_msp) {
		return NULL;
	}

	/*Select one.*/
	mspidx = 0;
	if (num_applicable_msp > 1) {
		mspidx = NC_random(num_applicable_msp);
	}
	return applicable_missionpoints[mspidx];
}

/**
Also resets the player's {@link mission_stage} and sets player controllable.
*/
static
void missions_jobhelp_hide(int playerid)
{
	mission_stage[playerid] = MISSION_STAGE_NOMISSION;
	TogglePlayerControllable(playerid, 1);
	textdraws_hide_consecutive(playerid, NUM_HELP_TEXTDRAWS, TEXTDRAW_MISSIONHELP_BASE);
}

/**
@param newselection new selected option
@param direction to go in, in case the selected option is invalid
*/
static
void mission_help_update_selection_ensure_available(int playerid, int newselection, int direction)
{
	int point_mask;
	float optdif;

	assert(direction != 0);

	mission_help_option[playerid] = newselection;
invalid:
	if (mission_help_option[playerid] > 2) {
		mission_help_option[playerid] = 0;
	} else if (mission_help_option[playerid] < 0) {
		mission_help_option[playerid] = 2;
	}
	point_mask = msptype_mask_to_point_mask(missions_available_msptype_mask[playerid]);
	if (!((point_mask >> mission_help_option[playerid]) & 1)) {
		mission_help_option[playerid] += direction;
		goto invalid;
	}

	optdif = td_jobhelp_bluebtnbg.rpcdata->y - td_jobhelp_greenbtnbg.rpcdata->y;
	td_jobhelp_optselbg.rpcdata->y = td_jobhelp_greenbtnbg.rpcdata->y + mission_help_option[playerid] * optdif;
	textdraws_show(playerid, 1, &td_jobhelp_optselbg);
}

/**
Ensure to update {@link mission_help_option} first.
Also sets the player's {@link mission_stage} and sets player uncontrollable.
*/
static
void missions_jobhelp_show(int playerid, int point_mask)
{
	mission_stage[playerid] = MISSION_STAGE_HELP;
	TogglePlayerControllable(playerid, 0);

	assert(point_mask);

	/*Remember textdraw string length is allocated as needed using the strlen from the text in the file.*/
	if (point_mask & MISSION_POINT_PASSENGERS) {
		strcpy(td_jobhelp_actiongreen.rpcdata->text, "~w~Find nearest point");
		td_jobhelp_greenbtnbg.rpcdata->box_color = 0xff333333;
	} else {
		strcpy(td_jobhelp_actiongreen.rpcdata->text, "~r~Wrong vehicle");
		td_jobhelp_greenbtnbg.rpcdata->box_color = 0xff202020;
	}
	if (point_mask & MISSION_POINT_CARGO) {
		strcpy(td_jobhelp_actionblue.rpcdata->text, "~w~Find nearest point");
		td_jobhelp_bluebtnbg.rpcdata->box_color = 0xff333333;
	} else {
		strcpy(td_jobhelp_actionblue.rpcdata->text, "~r~Wrong vehicle");
		td_jobhelp_bluebtnbg.rpcdata->box_color = 0xff202020;
	}
	if (point_mask & MISSION_POINT_SPECIAL) {
		strcpy(td_jobhelp_actionred.rpcdata->text, "~w~Find nearest point");
		td_jobhelp_redbtnbg.rpcdata->box_color = 0xff333333;
	} else {
		strcpy(td_jobhelp_actionred.rpcdata->text, "~r~Wrong vehicle");
		td_jobhelp_redbtnbg.rpcdata->box_color = 0xff202020;
	}

	/*Showing all but the "selection option background" textdraw.*/
	textdraws_show(playerid, 16,
		&td_jobhelp_keyhelp, &td_jobhelp_header, &td_jobhelp_optsbg,
		&td_jobhelp_text, &td_jobhelp_greenbtnbg, &td_jobhelp_bluebtnbg,
		&td_jobhelp_redbtnbg, &td_jobhelp_enexgreen, &td_jobhelp_txtgreen,
		&td_jobhelp_actiongreen, &td_jobhelp_enexblue, &td_jobhelp_txtblue,
		&td_jobhelp_actionblue, &td_jobhelp_enexred, &td_jobhelp_txtred,
		&td_jobhelp_actionred);

	mission_help_update_selection_ensure_available(playerid, mission_help_option[playerid], 1);
}

int missions_is_player_on_mission(int playerid)
{
	return activemission[playerid] != NULL;
}

void missions_create_tracker_socket()
{
	tracker = NC_ssocket_create(SOCKET_UDP);
	if (tracker == SOCKET_INVALID_SOCKET) {
		logprintf("failed to create flighttracker socket");
	} else {
		B144("127.0.0.1");
		NC_ssocket_connect(tracker, buf144a, TRACKER_PORT);
		*buf32 = 0x04594C46;
		NC_ssocket_send(tracker, buf32a, 4);
	}
}

void missions_destroy_tracker_socket()
{
	if (tracker != SOCKET_INVALID_SOCKET) {
		*buf32 = 0x05594C46;
		NC_ssocket_send(tracker, buf32a, 4);
	}
}

static
void missions_update_missionpoint_indicators(int playerid, float player_x, float player_y, float player_z)
{
#define AIRPORT_RANGE_SQ (1500.0f * 1500.0f)
#define POINT_RANGE_SQ (500.0f * 500.0f)

	struct RPCDATA_CreateObject rpcdata_CreateObject;
	struct RPCDATA_DestroyObject rpcdata_DestroyObject;
	struct BitStream bitstream;
	struct MISSIONPOINT *msp;
	int airportidx, indicatoridx, missionptidx, idxtouse;
	int player_new_active_msp_index;
	float dx, dy, dz;

	/*change now out-of-range ones to AVAILABLE*/
	for (indicatoridx = 0; indicatoridx < MAX_MISSION_INDICATORS; indicatoridx++) {
		if (missionpoint_indicator_state[playerid][indicatoridx] == MISSIONPOINT_INDICATOR_STATE_USED) {
			dx = missionpoints[missionpoint_indicator_index[playerid][indicatoridx]].pos.x - player_x;
			dy = missionpoints[missionpoint_indicator_index[playerid][indicatoridx]].pos.y - player_y;
			if (dx * dx + dy * dy > POINT_RANGE_SQ + 250.0f) {
				/*No real need to destroy (unless its type doesn't match the player's available types anymore,
				but the last loop of this method will deal with that).*/
				missionpoint_indicator_state[playerid][indicatoridx] = MISSIONPOINT_INDICATOR_STATE_AVAILABLE;
#ifdef MISSIONS_LOG_POINT_INDICATOR_ALLOC
				printf("%d %d available %d\n", indicatoridx, missionpoint_indicator_index[playerid][indicatoridx], time_timestamp());
#endif
			}
		}
	}

	/*check for new ones in range*/
	for (airportidx = 0; airportidx < numairports; airportidx++) {
		dx = airports[airportidx].pos.x - player_x;
		dy = airports[airportidx].pos.y - player_y;
		if (dx * dx + dy * dy < AIRPORT_RANGE_SQ) {
			msp = airports[airportidx].missionpoints;
			for (missionptidx = airports[airportidx].num_missionpts; missionptidx > 0; missionptidx--) {
				if (!(msp->type & missions_available_msptype_mask[playerid])) {
					goto next;
				}

				dx = msp->pos.x - player_x;
				dy = msp->pos.y - player_y;
				if (dx * dx + dy * dy > POINT_RANGE_SQ) {
					goto next;
				}

				idxtouse = -1;
				for (indicatoridx = 0; indicatoridx < MAX_MISSION_INDICATORS; indicatoridx++) {
					if (missionpoint_indicator_state[playerid][indicatoridx] != MISSIONPOINT_INDICATOR_STATE_USED) {
						idxtouse = indicatoridx;
					} else if (missionpoint_indicator_index[playerid][indicatoridx] == msp - missionpoints) {
						/*This missionpoint is already created.*/
						goto next;
					}
				}

				if (idxtouse != -1) {
					rpcdata_CreateObject.objectid = OBJECT_MISSION_INDICATOR_BASE + idxtouse;
					/*This relies on the types being 1, 2, 4.*/
					/*Passenger 1 19606*/
					/*Cargo 2 19607*/
					/*Special 4 19605*/
					rpcdata_CreateObject.modelid = 19607 - (msp->point_type & 1) - ((msp->point_type >> 1) & 0x2);
					rpcdata_CreateObject.x = msp->pos.x;
					rpcdata_CreateObject.y = msp->pos.y;
					rpcdata_CreateObject.z = msp->pos.z;
					rpcdata_CreateObject.rx = 0.0f;
					rpcdata_CreateObject.ry = 0.0f;
					rpcdata_CreateObject.rz = 0.0f;
					rpcdata_CreateObject.drawdistance = 1000.0f;
					rpcdata_CreateObject.no_camera_col = 0;
					rpcdata_CreateObject.attached_object_id = -1;
					rpcdata_CreateObject.attached_vehicle_id = -1;
					rpcdata_CreateObject.num_materials = 0;
					bitstream.ptrData = &rpcdata_CreateObject;
					bitstream.numberOfBitsUsed = sizeof(rpcdata_CreateObject) * 8;
					SAMP_SendRPCToPlayer(RPC_CreateObject, &bitstream, playerid, 2);
					missionpoint_indicator_state[playerid][idxtouse] = MISSIONPOINT_INDICATOR_STATE_USED;
					missionpoint_indicator_index[playerid][idxtouse] = msp - missionpoints;
#ifdef MISSIONS_LOG_POINT_INDICATOR_ALLOC
					printf("%d %d created %d\n", idxtouse, msp-missionpoints, time_timestamp());
#endif
					goto next;
				}
next:
				msp++;
			}
		}
	}

	/*delete AVAILABLE ones when they don't apply anymore*/
	for (indicatoridx = 0; indicatoridx < MAX_MISSION_INDICATORS; indicatoridx++) {
		if (missionpoint_indicator_state[playerid][indicatoridx] != MISSIONPOINT_INDICATOR_STATE_FREE &&
			!(missionpoints[missionpoint_indicator_index[playerid][indicatoridx]].type & missions_available_msptype_mask[playerid]))
		{
			rpcdata_DestroyObject.objectid = OBJECT_MISSION_INDICATOR_BASE + indicatoridx;
			bitstream.ptrData = &rpcdata_DestroyObject;
			bitstream.numberOfBitsUsed = sizeof(rpcdata_DestroyObject);
			SAMP_SendRPCToPlayer(RPC_DestroyObject, &bitstream, playerid, 2);
			missionpoint_indicator_state[playerid][indicatoridx] = MISSIONPOINT_INDICATOR_STATE_FREE;
#ifdef MISSIONS_LOG_POINT_INDICATOR_ALLOC
			printf("%d %d destroyed %d\n", indicatoridx, missionpoint_indicator_index[playerid][indicatoridx], time_timestamp());
#endif
		}
	}

	/*update active_msp_index and show 'press ...' text if needed*/
	if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER) {
		for (indicatoridx = 0; indicatoridx < MAX_MISSION_INDICATORS; indicatoridx++) {
			if (missionpoint_indicator_state[playerid][indicatoridx] == MISSIONPOINT_INDICATOR_STATE_USED) {
				msp = &missionpoints[missionpoint_indicator_index[playerid][indicatoridx]];
				dx = msp->pos.x - player_x;
				dy = msp->pos.y - player_y;
				dz = msp->pos.z - player_z;
				if (dx * dx + dy * dy + dz * dz < MISSION_CP_RAD_SQ) {
					if (active_msp_index[playerid] == missionpoint_indicator_index[playerid][indicatoridx]) {
						return;
					}

					player_new_active_msp_index = missionpoint_indicator_index[playerid][indicatoridx];
					if (activemission[playerid]) {
						if (player_new_active_msp_index == activemission[playerid]->endpoint - missionpoints) {
							GameTextForPlayer(playerid, 3000, 3,
								"~w~Press ~b~~k~~CONVERSATION_YES~~w~ to unload,~n~or type ~b~/w");
						}
						active_msp_index[playerid] = player_new_active_msp_index;
						return;
					}

					/*TODO: sometimes when entering/exiting vehicle, this text doesn't show even though code gets hit*/
					GameTextForPlayer(playerid, 3000, 3,
						"~w~Press ~b~~k~~CONVERSATION_YES~~w~ to view missions,~n~or type ~b~/w");
					goto active_msp_changed;
				}
			}
		}
	}
	if (active_msp_index[playerid] == -1) {
		return;
	}
	player_new_active_msp_index = -1;
active_msp_changed:
	if (player_new_active_msp_index == locating_msp_index[playerid] && locating_msp_index[playerid] != -1) {
		locating_msp_index[playerid] = -1;
		DisablePlayerRaceCheckpoint(playerid);
	}
	if (mission_stage[playerid] == MISSION_STAGE_JOBMAP) {
		missions_hide_jobmap_set_stage_set_controllable(playerid);
	}
	/*this assignment has to be done *after* the jobmap_hide call*/
	active_msp_index[playerid] = player_new_active_msp_index;

#undef AIRPORT_RANGE_SQ
#undef POINT_RANGE_SQ
}

static
void missions_dispose()
{
	free(td_jobhelp_keyhelp.rpcdata);
	free(td_jobhelp_header.rpcdata);
	free(td_jobhelp_optsbg.rpcdata);
	free(td_jobhelp_text.rpcdata);
	free(td_jobhelp_greenbtnbg.rpcdata);
	free(td_jobhelp_bluebtnbg.rpcdata);
	free(td_jobhelp_redbtnbg.rpcdata);
	free(td_jobhelp_optselbg.rpcdata);
	free(td_jobhelp_enexgreen.rpcdata);
	free(td_jobhelp_txtgreen.rpcdata);
	free(td_jobhelp_actiongreen.rpcdata);
	free(td_jobhelp_enexblue.rpcdata);
	free(td_jobhelp_txtblue.rpcdata);
	free(td_jobhelp_actionblue.rpcdata);
	free(td_jobhelp_enexred.rpcdata);
	free(td_jobhelp_txtred.rpcdata);
	free(td_jobhelp_actionred.rpcdata);
}

static
void missions_init()
{
	/*textdraws*/
	textdraws_load_from_file("jobsatisfaction", TEXTDRAW_JOBSATISFACTION, 1, &td_satisfaction);

	textdraws_load_from_file("jobhelp", TEXTDRAW_MISSIONHELP_BASE, NUM_HELP_TEXTDRAWS,
		/*remember to free their rpcdata in missions_dispose*/
		&td_jobhelp_keyhelp, &td_jobhelp_optsbg, &td_jobhelp_header,
		&td_jobhelp_text, &td_jobhelp_greenbtnbg, &td_jobhelp_bluebtnbg,
		&td_jobhelp_redbtnbg, &td_jobhelp_optselbg, &td_jobhelp_enexgreen,
		&td_jobhelp_txtgreen, &td_jobhelp_actiongreen, &td_jobhelp_enexblue,
		&td_jobhelp_txtblue, &td_jobhelp_actionblue, &td_jobhelp_enexred,
		&td_jobhelp_txtred, &td_jobhelp_actionred);
	textdraws_set_textbox_properties(td_jobhelp_keyhelp.rpcdata);

	/*missionpoints are loaded in airport.c*/

	/*end unfinished dangling flights*/
	atoc(buf144,
		"UPDATE flg "
		"SET state="EQ(MISSION_STATE_SERVER_ERR)" "
		"WHERE state="EQ(MISSION_STATE_INPROGRESS)"",
		144);
	NC_mysql_tquery_nocb(buf144a);
}

void missions_player_traveled_distance_in_vehicle(int playerid, int vehicleid, float distance_in_m)
{
	/*The state of the mission is not being checked. Being on a mission and in the mission
	vehicle is enough reason to add the traveled distance to the total distance*/
	if (activemission[playerid] != NULL && activemission[playerid]->vehicleid == vehicleid) {
		activemission[playerid]->actualdistanceM += distance_in_m;
	}
}

/**
Append a pay description and colorized, formatted number to given buffer.

@return amount of characters appended
*/
static
int missions_append_pay(char *buf, char *description, int amount)
{
	char tmp[12], *ptmp;
	int len, p = sprintf(buf, "%s", description);
	if (amount < 0) {
		if (amount < -2147483647) {
			amount = -2147483647;
		}
		amount = -amount;
		buf[p++] = '{'; buf[p++] = 'F'; buf[p++] = '6'; buf[p++] = '0';
		buf[p++] = '0'; buf[p++] = '0'; buf[p++] = '0'; buf[p++] = '}';
		buf[p++] = '-';
	} else {
		if (amount > 2147483647) {
			amount = 2147483647;
		}
		buf[p++] = '{'; buf[p++] = '0'; buf[p++] = '0'; buf[p++] = 'F';
		buf[p++] = '6'; buf[p++] = '0'; buf[p++] = '0'; buf[p++] = '}';
	}
	buf[p++] = '$';
	sprintf(tmp, "%d", amount);
	ptmp = tmp;
	len = strlen(tmp);
	while (len-- > 0) {
		buf[p++] = *(ptmp++);
		if (len % 3 == 0 && len) {
			buf[p++] = ',';
		}
	}
	buf[p++] = '\n';
	return p;
}

/**
@return speed in units/s
*/
static
float missions_get_vehicle_maximum_speed(int model)
{
	/* max horizontal speed is about 81.6 */
	switch (model) {
	case MODEL_DODO: return 140.0f / 270.0f * 81.6f;
	case MODEL_BEAGLE: return 125.0f / 270.0f * 81.6f;
	case MODEL_NEVADA: return 185.0f / 270.0f * 81.6f;
	case MODEL_MAVERICK:
	case MODEL_VCNMAV: return 154.0f / 270.0f * 81.6f;
	case MODEL_RAINDANC: return 113.0f / 270.0f * 81.6f;
	case MODEL_LEVIATHN: return 108.0f / 270.0f * 81.6f;
	case MODEL_POLMAV: return 154.0f / 270.0f * 81.6f;
	case MODEL_SPARROW: return 106.0f / 270.0f * 81.6f;
	case MODEL_HUNTER: return 190.0f / 270.0f * 81.6f;
	case MODEL_CARGOBOB: return 126.0f / 270.0f * 81.6f;
	case MODEL_RUSTLER: return 235.0f / 270.0f * 81.6f;
	case MODEL_SKIMMER: return 135.0f / 270.0f * 81.6f;
	default:
		logprintf("mission_get_vehicle_maximum_speed: "
			"unk model: %d", model);
	case MODEL_SHAMAL:
	case MODEL_HYDRA:
	case MODEL_ANDROM:
	case MODEL_AT400:
		return 81.6f;
	}
}

/**
Gets the pay multiplier for missions done with given vehicle model.
*/
static
float missions_get_vehicle_paymp(int model)
{
	const float heli_mp = 1.18f;

	switch (model) {
	case MODEL_DODO: return 270.0f / 140.0f;
	case MODEL_SHAMAL: return 1.0f;
	case MODEL_BEAGLE: return 270.0f / 125.0f;
	case MODEL_AT400: return 1.15f;
	case MODEL_ANDROM: return 1.1f;
	case MODEL_NEVADA: return 270.0f / 185.0f;
	case MODEL_MAVERICK:
	case MODEL_VCNMAV: return heli_mp * 270.0f / 154.0f;
	case MODEL_RAINDANC: return heli_mp * 270.0f / 113.0f;
	case MODEL_LEVIATHN: return heli_mp * 270.0f / 108.0f;
	case MODEL_POLMAV: return heli_mp * 270.0f / 154.0f;
	case MODEL_SPARROW: return heli_mp * 270.0f / 106.0f;
	case MODEL_HUNTER: return heli_mp * 270.0f / 190.0f;
	case MODEL_CARGOBOB: return heli_mp * 270.0f / 126.0f;
	case MODEL_HYDRA: return 0.9f;
	case MODEL_RUSTLER: return 270.0f / 235.0f;
	case MODEL_SKIMMER: return 270.0f / 135.0f;
	default:
		logprintf("mission_get_vehicle_paymp: unk model: %d", model);
		return 1.0f;
	}
}

/**
Calculates airport tax for given missiontype at given airport.
*/
static
int calculate_airport_tax(struct AIRPORT *ap, int missiontype)
{
	struct MISSIONPOINT *msp;
	struct RUNWAY *rnw;
	int i;
	int costpermsp, tax = 500, chargernws = 0;

	switch (missiontype) {
	case MISSION_TYPE_PASSENGER_WATER:
		/* skimmer, also for runways (lights maintenance etc) */
		costpermsp = 15;
		chargernws = 1;
		break;
	case MISSION_TYPE_PASSENGER_S:
		/*dodo is excluded from runway cost*/
		costpermsp = 20;
		break;
	case MISSION_TYPE_PASSENGER_M:
		costpermsp = 30;
		chargernws = 1;
		break;
	case MISSION_TYPE_PASSENGER_L:
		costpermsp = 50;
		chargernws = 1;
		break;
	case MISSION_TYPE_CARGO_S:
	case MISSION_TYPE_CARGO_M:
	case MISSION_TYPE_CARGO_L:
		costpermsp = 40;
		chargernws = 1;
		break;
	case MISSION_TYPE_HELI:
	case MISSION_TYPE_HELI_CARGO:
		costpermsp = 30;
		break;
	case MISSION_TYPE_MIL_HELI:
	case MISSION_TYPE_MIL:
	default:
		/* military is govt, and no tax for special missions */
		return 0;
	}

	/*missionpoint cost for every missionpoint this missiontype has*/
	msp = ap->missionpoints;
	for (i = ap->num_missionpts; i > 0; i--) {
		if (msp->type & missiontype) {
			tax += costpermsp;
		}
		msp++;
	}

	/*for helis: 40 per helipad*/
	/*for others: 50 per runway + 15 if VOR + 15 if ILS*/
	rnw = ap->runways;
	while (rnw != ap->runwaysend) {
		if (missiontype & HELI_MISSIONTYPES) {
			if (rnw->type == RUNWAY_TYPE_HELIPAD) {
				tax += 40;
			}
		} else {
			if (chargernws && rnw->type == RUNWAY_TYPE_RUNWAY) {
				tax += 50;
				if (rnw->nav & NAV_VOR) {
					tax += 15;
				}
				if (rnw->nav & NAV_ILS) {
					tax += 15;
				}
			}
		}
		rnw++;
	}
	return tax;
}

/**
Cleanup a mission and free memory. Does not query. Does send a tracker msg. Resets {@code mission_stage}.

Player must be on a mission or get segfault'd.

Call when ending a mission.
*/
static
void missions_cleanup(int playerid)
{
	struct vec3 pos;
	struct MISSION *mission;

	mission = activemission[playerid];

	/* flight tracker packet 3 */
	buf32[0] = 0x03594C46;
	buf32[1] = mission->id;
	NC_ssocket_send(tracker, buf32a, 8);

	if (mission->missiontype & PASSENGER_MISSIONTYPES) {
		textdraws_hide_consecutive(playerid, 1, TEXTDRAW_JOBSATISFACTION);
	}

	if (GetPlayerVehicleID(playerid) != mission->vehicleid) {
		SetVehicleObjectiveForPlayer(mission->vehicleid, playerid, 0);
	}

	free(mission);
	activemission[playerid] = NULL;
	mission_stage[playerid] = MISSION_STAGE_NOMISSION;
	number_missions_started_stopped[playerid]++;

	GetPlayerPos(playerid, &pos);
	kneeboard_update_all(playerid, &pos);
}

/**
player must be on a mission or get segfault'd.

@param reason one of MISSION_STATE_ constants
*/
static
void missions_end_unfinished(int playerid, int reason)
{
	struct MISSION *mission;

	mission = activemission[playerid];
	csprintf(buf144,
	        "UPDATE flg "
		"SET state=%d,tlastupdate=UNIX_TIMESTAMP(),adistance=%f "
		"WHERE id=%d",
	        reason,
		mission->actualdistanceM,
	        mission->id);
	NC_mysql_tquery_nocb(buf144a);

	missions_cleanup(playerid);
}

void missions_on_vehicle_destroyed_or_respawned(struct dbvehicle *veh)
{
	struct MISSION *mission;
	int i, playerid;

	for (i = 0; i < playercount; i++) {
		playerid = players[i];
		mission = activemission[playerid];
		if (mission != NULL && mission->veh == veh) {
			SendClientMessage(playerid, COL_WARN,
				WARN" Your mission vehicle has been destroyed, "
				"your mission has been aborted and you have "
				"been fined $"EQ(MISSION_CANCEL_FINE)".");
			NC_DisablePlayerRaceCheckpoint(playerid);
			money_take(playerid, MISSION_CANCEL_FINE);
			missions_end_unfinished(playerid, MISSION_STATE_ABANDONED);
			return;
		}
	}
}

void missions_on_player_connect(int playerid)
{
	int i;

	for (i = 0; i < MAX_MISSION_INDICATORS; i++) {
		missionpoint_indicator_state[playerid][i] = MISSIONPOINT_INDICATOR_STATE_FREE;
	}
	missions_available_msptype_mask[playerid] = -1;
	active_msp_index[playerid] = -1;
	locating_msp_index[playerid] = -1;
	mission_help_option[playerid] = 0;
	mission_stage[playerid] = MISSION_STAGE_NOMISSION;
	activemission[playerid] = NULL;
	number_missions_started_stopped[playerid]++;
}

void missions_on_player_disconnect(int playerid)
{
	struct MISSION *miss;

	if ((miss = activemission[playerid]) != NULL) {
		missions_end_unfinished(playerid, MISSION_STATE_ABANDONED);
	}
}

static
int missions_format_satisfaction_text(int satisfaction, char *out_buf)
{
	return sprintf(out_buf, "Passenger~n~Satisfaction: %d%%", satisfaction);
}

struct MISSION_CB_DATA {
	int player_cc;
	int number_missions_started_stopped;
	int query_time;
};

/**
Starts the FLIGHT stage of the mission.

May be called from a timer.
*/
static
int missions_start_flight(void *data)
{
	struct MISSION_CB_DATA *mission_cb_data;
	int playerid;
	struct MISSION *mission;
	struct vec3 pos;

	mission_cb_data = (struct MISSION_CB_DATA*) data;
	playerid = PLAYER_CC_GETID(mission_cb_data->player_cc);
	if (!PLAYER_CC_CHECK(mission_cb_data->player_cc, playerid) ||
		mission_cb_data->number_missions_started_stopped != number_missions_started_stopped[playerid])
	{
		free(mission_cb_data);
		return TIMER_NO_REPEAT;
	}

	mission = activemission[playerid];

	if (prefs[playerid] & PREF_WORK_AUTONAV) {
		nav_navigate_to_airport(playerid, mission->veh->spawnedvehicleid, mission->veh->model, mission->endpoint->ap);
	}

	if (mission->missiontype & PASSENGER_MISSIONTYPES) {
		td_satisfaction.rpcdata->text_length = missions_format_satisfaction_text(100, td_satisfaction.rpcdata->text);
		textdraws_show(playerid, 1, &td_satisfaction);
	}

	mission_stage[playerid] = MISSION_STAGE_FLIGHT;
	SetPlayerRaceCheckpointNoDir(playerid, RACE_CP_TYPE_NORMAL, &mission->endpoint->pos, MISSION_CP_RAD);
	HideGameTextForPlayer(playerid);
	TogglePlayerControllable(playerid, 1);
	GetPlayerPos(playerid, &pos);
	kneeboard_update_all(playerid, &pos);

	csprintf(buf144,
	        "UPDATE flg "
		"SET tload=UNIX_TIMESTAMP(),tlastupdate=UNIX_TIMESTAMP() "
		"WHERE id=%d",
	        mission->id);
	NC_mysql_tquery_nocb(buf144a);

	free(mission_cb_data);
	return TIMER_NO_REPEAT;
}

/**
Called when mission create query has been executed.
*/
static
void missions_querycb_create(void *data)
{
	struct MISSION_CB_DATA *mission_cb_data;
	int playerid;
	struct MISSION *mission;
	int loading_time_left;

	mission_cb_data = (struct MISSION_CB_DATA*) data;
	playerid = PLAYER_CC_GETID(mission_cb_data->player_cc);
	if (!PLAYER_CC_CHECK(mission_cb_data->player_cc, playerid) ||
		mission_cb_data->number_missions_started_stopped != number_missions_started_stopped[playerid])
	{
		free(mission_cb_data);
		return;
	}

	mission = activemission[playerid];
	mission->id = NC_cache_insert_id();

	/* flight tracker packet 1 */
	buf32[0] = 0x01594C46;
	buf32[1] = mission->id;
	*((float*) (cbuf32 + 8)) = model_fuel_capacity(mission->veh->model);
	*((short*) (cbuf32 + 12)) = mission->veh->model;
	cbuf32[14] = pdata[playerid]->namelen;
	memset(cbuf32 + 15, 0, 24); /* don't leak random data */
	strcpy(cbuf32 + 15, pdata[playerid]->name);
	/*buf32 is len 32 * 4 so 40 is fine*/
	NC_ssocket_send(tracker, buf32a, 40);

	loading_time_left = mission_cb_data->query_time + MISSION_LOAD_UNLOAD_TIME - time_timestamp();
	if (loading_time_left > 100) {
		timer_set(loading_time_left, missions_start_flight, mission_cb_data);
	} else {
		missions_start_flight(mission_cb_data);
	}
	/*The missions_start_flight func will free the passed mission_cb_data.*/
}

/**
Starts a mission for given player.
*/
static
void missions_start_mission(int playerid, struct MISSIONPOINT *startpoint, struct MISSIONPOINT *endpoint, int missiontype)
{
	struct MISSION_CB_DATA *cbdata;
	struct MISSION *mission;
	struct dbvehicle *veh;
	struct vec3 player_position;
	int vehicleid, i;
	float vhp, dx, dy;

	if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER) {
		SendClientMessage(playerid, COL_WARN, WARN"You must be the driver of a vehicle ");
		return;
	}

	vehicleid = GetPlayerVehicleID(playerid);
	veh = gamevehicles[vehicleid].dbvehicle;
	if (!veh || !(missions_get_vehicle_model_msptype_mask(veh->model) & missiontype)) {
		SendClientMessage(playerid, COL_WARN, WARN"Wrong vehicle type!");
		return;
	}

	for (i = 0; i < playercount; i++) {
		if ((mission = activemission[players[i]]) != NULL && mission->veh == veh) {
			SendClientMessage(playerid, COL_WARN, WARN"This vehicle is already used in a mission!");
			return;
		}
	}

	GetPlayerPos(playerid, &player_position);
	dx = player_position.x - startpoint->pos.x;
	dy = player_position.y - startpoint->pos.y;
	if (dx * dx + dy * dy > MISSION_CP_RAD_SQ) {
		SendClientMessage(playerid, COL_WARN, WARN"Get inside the mission point first!");
		return;
	}

	vhp = GetVehicleHealth(vehicleid);

	dx = startpoint->pos.x - endpoint->pos.x;
	dy = startpoint->pos.y - endpoint->pos.y;

	mission_stage[playerid] = MISSION_STAGE_LOAD;
	activemission[playerid] = mission = malloc(sizeof(struct MISSION));
	number_missions_started_stopped[playerid]++;
	mission->id = -1;
	mission->missiontype = missiontype;
	mission->startpoint = startpoint;
	mission->endpoint = endpoint;
	mission->distance = sqrt(dx * dx + dy * dy);
	mission->actualdistanceM = 0.0f;
	mission->passenger_satisfaction = 100;
	mission->veh = veh;
	mission->vehicleid = vehicleid;
	mission->vehicle_reincarnation = gamevehicles[vehicleid].reincarnation;
	mission->starttime = time(NULL);
	mission->lastvehiclehp = (short) vhp;
	mission->damagetaken = 0;
	mission->lastfuel = veh->fuel;
	mission->fuelburned = 0.0f;
	mission->weatherbonus = missions_get_initial_weatherbonus();

	csprintf(buf144, "UPDATE msp SET o=o+1 WHERE i=%d", startpoint->id);
	NC_mysql_tquery_nocb(buf144a);
	csprintf(buf144, "UPDATE msp SET p=p+1 WHERE i=%d", endpoint->id);
	NC_mysql_tquery_nocb(buf144a);

	sprintf(cbuf4096_,
	        "INSERT INTO flg(player,vehicle,missiontype,fapt,tapt,fmsp,"
		"tmsp,distance,tstart,tlastupdate) "
		"VALUES(IF(%d<1,NULL,%d),%d,%d,%d,%d,%d,%d,%.4f,"
		"UNIX_TIMESTAMP(),UNIX_TIMESTAMP())",
	        userid[playerid],
	        userid[playerid],
		veh->id,
		missiontype,
		startpoint->ap->id,
		endpoint->ap->id,
		startpoint->id,
		endpoint->id,
		mission->distance);
	cbdata = malloc(sizeof(struct MISSION_CB_DATA));
	cbdata->player_cc = MK_PLAYER_CC(playerid);
	cbdata->number_missions_started_stopped = number_missions_started_stopped[playerid];
	cbdata->query_time = time_timestamp();
	common_mysql_tquery(cbuf4096_, missions_querycb_create, cbdata);

	tracker_afk_packet_sent[playerid] = 0;

	sprintf(cbuf144,
		"Flight from %s (%s) %s to %s (%s) %s",
		mission->startpoint->ap->name,
		mission->startpoint->ap->code,
		mission->startpoint->name,
		mission->endpoint->ap->name,
		mission->endpoint->ap->code,
		mission->endpoint->name);
	SendClientMessage(playerid, COL_MISSION, cbuf144);

	TogglePlayerControllable(playerid, 0);
	GameTextForPlayer(playerid, 0x800000, 3, "~p~Loading");
	kneeboard_update_all(playerid, &player_position);
	locating_msp_index[playerid] = -1;
}

struct MISSION_UNLOAD_DATA {
	int player_cc;
	int number_missions_started_stopped;
	float vehiclehp;
};

/**
Called from timer callback for mission unload checkpoint.
*/
static
int missions_after_unload(void *data)
{
	struct DIALOG_INFO dialog;
	struct MISSION_UNLOAD_DATA *mission_cb_data;
	int playerid;
	struct MISSION *miss;
	float vehhp, paymp, mintime;
	int ptax, psatisfaction, pdistance, pbonus = 0, ptotal, pdamage, pcheat;
	int totaltime, duration_h, duration_m, extra_damage_taken;
	int i;
	char *dialoginfo;
	short missionmsg_playerids[MAX_PLAYERS];
	int num_missionmsg_playerids;

	mission_cb_data = (struct MISSION_UNLOAD_DATA*) data;
	playerid = PLAYER_CC_GETID(mission_cb_data->player_cc);
	if (!PLAYER_CC_CHECK(mission_cb_data->player_cc, playerid) ||
		mission_cb_data->number_missions_started_stopped != number_missions_started_stopped[playerid])
	{
		free(mission_cb_data);
		return TIMER_NO_REPEAT;
	}

	vehhp = mission_cb_data->vehiclehp;
	miss = activemission[playerid];

	HideGameTextForPlayer(playerid);
	TogglePlayerControllable(playerid, 1);

	pcheat = 0;
	psatisfaction = 0;

	extra_damage_taken = (int) (miss->lastvehiclehp - (short) vehhp);
	if (extra_damage_taken < 0) {
		pcheat -= 250000;
		sprintf(cbuf144,
			"flg(#%d) vhh: was: %hd now: %.0f",
			miss->id,
			miss->lastvehiclehp,
			vehhp);
		anticheat_log(playerid, AC_VEH_HP_MISSION_INCREASE, cbuf144);
	} else {
		miss->damagetaken += extra_damage_taken;
	}
	miss->lastvehiclehp = (short) vehhp;
	miss->fuelburned += miss->lastfuel - miss->veh->fuel;

	totaltime = (int) difftime(time(NULL), miss->starttime);
	duration_m = totaltime % 60;
	duration_h = (totaltime - duration_m) / 60;

	/* don't use adistance because it also includes z changes */
	mintime = miss->distance;
	mintime /= missions_get_vehicle_maximum_speed(miss->veh->model);
	if (totaltime < (int) mintime) {
		pcheat -= 250000;
		sprintf(cbuf144,
			"flg(#%d) too fast: min: %d actual: %d",
			miss->id,
			(int) mintime,
			totaltime);
		anticheat_log(playerid, AC_MISSION_TOOFAST, cbuf144);
	}

	paymp = missions_get_vehicle_paymp(miss->veh->model);
	ptax = -calculate_airport_tax(miss->endpoint->ap, miss->missiontype);
	pdistance = 500 + (int) (miss->distance * 1.935f);
	pdistance = (int) (pdistance * paymp);
	if (miss->missiontype & PASSENGER_MISSIONTYPES) {
		if (miss->passenger_satisfaction == 100) {
			psatisfaction = 500;
		} else {
			psatisfaction = miss->passenger_satisfaction - 100;
			psatisfaction *= 30;
		}
	}
	pdamage = -3 * miss->damagetaken;

	ptotal = miss->weatherbonus + psatisfaction + pdistance + pbonus;
	ptotal += ptax + pdamage + pcheat;
	money_give(playerid, ptotal);

	i = sprintf(cbuf4096,
		"%s completed flight #%d from %s (%s) to %s (%s) in %dh%02dm (%s)",
		pdata[playerid]->name,
	        miss->id,
		miss->startpoint->ap->name,
	        miss->startpoint->ap->code,
	        miss->endpoint->ap->name,
	        miss->endpoint->ap->code,
	        duration_h,
	        duration_m,
	        vehnames[miss->veh->model - VEHICLE_MODEL_MIN]);
	echo_send_generic_message(ECHO_PACK12_FLIGHT_MESSAGE, cbuf4096);
	num_missionmsg_playerids = 0;
	for (i = 0; i < playercount; i++) {
		if (prefs[players[i]] & PREF_SHOW_MISSION_MSGS) {
			missionmsg_playerids[num_missionmsg_playerids] = players[i];
			num_missionmsg_playerids++;
		}
	}
	SendClientMessageToBatch(missionmsg_playerids, num_missionmsg_playerids, COL_MISSION, cbuf4096);

	csprintf(buf4096,
		"UPDATE flg SET tunload=UNIX_TIMESTAMP(),"
		"tlastupdate=UNIX_TIMESTAMP(),"
		"state=%d,fuel=%f,ptax=%d,pweatherbonus=%d,psatisfaction=%d,"
		"pdistance=%d,pdamage=%d,pcheat=%d,pbonus=%d,ptotal=%d,"
		"satisfaction=%d,adistance=%f,paymp=%f,damage=%d "
		"WHERE id=%d",
		MISSION_STATE_FINISHED,
		miss->fuelburned,
		ptax,
		miss->weatherbonus,
		psatisfaction,
		pdistance,
		pdamage,
		pcheat,
		pbonus,
		ptotal,
		miss->passenger_satisfaction,
		miss->actualdistanceM,
		paymp,
		miss->damagetaken,
		miss->id);
	NC_mysql_tquery_nocb(buf4096a);

	dialog_init_info(&dialog);
	dialoginfo = dialog.info;
	dialoginfo += sprintf(dialoginfo,
	             "{ffffff}Flight:\t\t\t"ECOL_MISSION"#%d\n"
	             "{ffffff}Origin:\t\t\t"ECOL_MISSION"%s\n"
	             "{ffffff}Destination:\t\t"ECOL_MISSION"%s\n"
	             "{ffffff}Distance (pt to pt):\t"ECOL_MISSION"%.0fm\n"
	             "{ffffff}Distance (actual):\t"ECOL_MISSION"%.0fm\n"
	             "{ffffff}Duration:\t\t"ECOL_MISSION"%dh%02dm\n"
	             "{ffffff}Fuel Burned:\t\t"ECOL_MISSION"%.1fL\n"
	             "{ffffff}Vehicle pay category:\t"ECOL_MISSION"%.1fx\n",
	             miss->id,
	             miss->startpoint->ap->name,
	             miss->endpoint->ap->name,
	             miss->distance,
	             miss->actualdistanceM,
	             duration_h,
	             duration_m,
		     miss->fuelburned,
	             paymp);
	if (miss->missiontype & PASSENGER_MISSIONTYPES) {
		dialoginfo += sprintf(dialoginfo,
		             "{ffffff}Passenger Satisfaction:\t"
			     ""ECOL_MISSION"%d%%\n",
		             miss->passenger_satisfaction);
	}
	*dialoginfo++ = '\n';
	*dialoginfo++ = '\n';
	if (ptax) {
		dialoginfo += missions_append_pay(dialoginfo, "{ffffff}Airport Tax:\t\t", ptax);
	}
	if (miss->weatherbonus) {
		dialoginfo += missions_append_pay(dialoginfo, "{ffffff}Weather bonus:\t\t", miss->weatherbonus);
	}
	dialoginfo += missions_append_pay(dialoginfo, "{ffffff}Distance Pay:\t\t", pdistance);
	if (miss->missiontype & PASSENGER_MISSIONTYPES) {
		if (psatisfaction > 0) {
			dialoginfo += missions_append_pay(dialoginfo, "{ffffff}Satisfaction Bonus:\t", psatisfaction);
		} else {
			dialoginfo += missions_append_pay(dialoginfo, "{ffffff}Satisfaction Penalty:\t", psatisfaction);
		}
	}
	if (pdamage) {
		dialoginfo += missions_append_pay(dialoginfo, "{ffffff}Damage Penalty:\t", pdamage);
	}
	if (pcheat) {
		dialoginfo += missions_append_pay(dialoginfo, "{ffffff}Cheat Penalty:\t\t", pcheat);
	}
	if (pbonus) {
		dialoginfo += missions_append_pay(dialoginfo, "{ffffff}Bonus:\t\t\t", pbonus);
	}
	dialoginfo += missions_append_pay(dialoginfo, "\n\n\t{ffffff}Total Pay: ", ptotal);
	*--dialoginfo = 0;
	dialog.transactionid = DLG_TID_MISSION_OVERVIEW;
	dialog.caption = "Flight Overview";
	dialog.button1 = "Close";
	dialog_show(playerid, &dialog);

	missions_cleanup(playerid);

	free(mission_cb_data);
	return TIMER_NO_REPEAT;
}

static
void missions_start_unload(int playerid)
{
	struct MISSION_UNLOAD_DATA *cbdata;
	struct MISSION *mission;
	struct vec3 vvel;
	struct dbvehicle *veh;
	int vehicleid, vv;

	if (GetPlayerState(playerid) != PLAYER_STATE_DRIVER) {
		return;
	}

	mission = activemission[playerid];

	if (mission == NULL) {
		return;
	}

	vehicleid = veh_GetPlayerVehicle(playerid, &vv, &veh);
	if (vehicleid != mission->vehicleid || vv != mission->vehicle_reincarnation || veh->id != mission->veh->id) {
		SendClientMessage(playerid, COL_WARN, WARN"You must be in the starting vehicle to continue!");
		return;
	}

	GetVehicleVelocityUnsafe(vehicleid, &vvel);
	/*Ignoring z velocity because it would be annoying with skimmers riding heavy waves.*/
	if (vvel.x * vvel.x + vvel.y * vvel.y > MAX_SPEED_SQ) {
		SendClientMessage(playerid, COL_WARN, WARN"Stop the vehicle first!");
		return;
	}

	mission_stage[playerid] = MISSION_STAGE_UNLOAD;
	cbdata = malloc(sizeof(struct MISSION_UNLOAD_DATA));
	cbdata->number_missions_started_stopped = number_missions_started_stopped[playerid];
	cbdata->player_cc = MK_PLAYER_CC(playerid);
	cbdata->vehiclehp = GetVehicleHealth(vehicleid);

	textdraws_hide_consecutive(playerid, 1, TEXTDRAW_JOBSATISFACTION);
	GameTextForPlayer(playerid, 0x8000000, 3, "~p~Unloading");
	TogglePlayerControllable(playerid, 0); /*Needs to be after getting vehicle health, it repairs the vehicle.*/
	DisablePlayerRaceCheckpoint(playerid);
	if (prefs[playerid] & PREF_WORK_AUTONAV) {
		nav_reset_for_vehicle(vehicleid);
	}

	timer_set(MISSION_LOAD_UNLOAD_TIME, missions_after_unload, cbdata);
}

static
void missions_on_player_state_change(int playerid, int from, int to)
{
	struct MISSION *mission;
	struct vec3 pos;
	int vehicleid;

	if (to == PLAYER_STATE_WASTED) {
		/*This basically handles on_player_death.*/
		switch (mission_stage[playerid]) {
		case MISSION_STAGE_NOMISSION:
			break;
		case MISSION_STAGE_HELP:
			missions_jobhelp_hide(playerid); /*sets controllable and mission stage*/
			break;
		case MISSION_STAGE_JOBMAP:
			missions_hide_jobmap_set_stage_set_controllable(playerid);
			break;
		default:
			DisablePlayerRaceCheckpoint(playerid);
			vehicleid = GetPlayerVehicleID(playerid);
			if (vehicleid && GetVehicleHealth(vehicleid) < 250.0f) {
				missions_end_unfinished(playerid, MISSION_STATE_CRASHED);
			} else {
				missions_end_unfinished(playerid, MISSION_STATE_DIED);
			}
			break;
		}

		if (locating_msp_index[playerid] != -1) {
			locating_msp_index[playerid] = -1;
			DisablePlayerRaceCheckpoint(playerid);
		}
		return;
	}

	if (to == PLAYER_STATE_ONFOOT || from == PLAYER_STATE_ONFOOT) {
		vehicleid = GetPlayerVehicleID(playerid);
		if (vehicleid) {
			missions_available_msptype_mask[playerid] = missions_get_vehicle_model_msptype_mask(GetVehicleModel(vehicleid));
		} else {
			missions_available_msptype_mask[playerid] = -1;
		}

		/*This could be unnecessary, unless the player gets jacked or teleported.
		(They can't exit because it'd trigger the keystate change to hide the ui)*/
		if (mission_stage[playerid] == MISSION_STAGE_HELP) {
			missions_jobhelp_hide(playerid); /*sets controllable and mission state*/
		} else if (mission_stage[playerid] == MISSION_STAGE_JOBMAP) {
			missions_hide_jobmap_set_stage_set_controllable(playerid);
		}

		GetPlayerPos(playerid, &pos);
		missions_update_missionpoint_indicators(playerid, pos.x, pos.y, pos.z);
	}

	if (from == PLAYER_STATE_DRIVER &&
		(mission = activemission[playerid]) != NULL &&
		mission->vehicleid == lastvehicle_asdriver[playerid])
	{
		SetVehicleObjectiveForPlayer(lastvehicle_asdriver[playerid], playerid, 1);
		SendClientMessage(playerid, COL_WARN, WARN"Get back in your vehicle!");
	} else if (to == PLAYER_STATE_DRIVER &&
		(mission = activemission[playerid]) != NULL &&
		mission->vehicleid == GetPlayerVehicleID(playerid))
	{
		SetVehicleObjectiveForPlayer(mission->vehicleid, playerid, 0);
	}
}

void missions_on_vehicle_stream_in(int vehicleid, int forplayerid)
{
	struct MISSION *mission;

	mission = activemission[forplayerid];
	if (mission != NULL && mission->vehicleid == vehicleid) {
		SetVehicleObjectiveForPlayer(vehicleid, forplayerid, 1);
	}
}

void missions_on_weather_changed(int weather)
{
	register struct MISSION *mission;
	register int i;
	int bonusvalue;

	bonusvalue = missions_get_weatherbonus_for_weather(weather);
	i = playercount;
	while (i--) {
		mission = activemission[players[i]];
		if (mission && mission->weatherbonus < bonusvalue) {
			mission->weatherbonus = bonusvalue;
		}
	}
}

void missions_send_tracker_data(
	int playerid, int vehicleid, float hp,
	struct vec3 *vpos, struct vec3 *vvel, int engine, float pitch, float roll)
{
	struct MISSION *mission;
	unsigned char flags;
	short spd, alt, hpv, pitch10, roll10;

	if ((mission = activemission[playerid]) == NULL ||
		mission->veh->spawnedvehicleid != vehicleid ||
		(isafk[playerid] && tracker_afk_packet_sent[playerid]))
	{
		return;
	}

	tracker_afk_packet_sent[playerid] = flags = isafk[playerid] == 1;
	if (engine) {
		flags |= 2;
	}

	hpv = (short) hp;
	alt = (short) vpos->z;
	spd = (short) (VEL_TO_KTS * sqrt(vvel->x * vvel->x + vvel->y * vvel->y + vvel->z * vvel->z));
	pitch10 = (short) (pitch * 10.0f);
	roll10 = (short) (roll * 10.0f);

	/* flight tracker packet 2 */
	buf32[0] = 0x02594C46;
	buf32[1] = mission->id;
	cbuf32[8] = flags;
	cbuf32[9] = (char) mission->passenger_satisfaction;
	memcpy(cbuf32 + 10, &spd, 2);
	memcpy(cbuf32 + 12, &alt, 2);
	memcpy(cbuf32 + 14, &hpv, 2);
	memcpy(cbuf32 + 16, &mission->veh->fuel, 4);
	memcpy(cbuf32 + 20, &vpos->x, 4);
	memcpy(cbuf32 + 24, &vpos->y, 4);
	memcpy(cbuf32 + 28, &pitch10, 2);
	memcpy(cbuf32 + 30, &roll10, 2);
	NC_ssocket_send(tracker, buf32a, 32);
}

/**
To be called from the /s command handler.
*/
static
void missions_process_cancel_request_by_player(int playerid)
{
	struct MISSION *mission;

	if ((mission = activemission[playerid]) != NULL) {
		if (mission_stage[playerid] == MISSION_STAGE_LOAD || mission_stage[playerid] == MISSION_STAGE_UNLOAD) {
			NC_TogglePlayerControllable(playerid, 1);
			HideGameTextForPlayer(playerid);
		} else {
			NC_DisablePlayerRaceCheckpoint(playerid);
		}
		money_take(playerid, MISSION_CANCEL_FINE);
		missions_end_unfinished(playerid, MISSION_STATE_DECLINED);
	} else {
		SendClientMessage(playerid, COL_WARN, WARN"You're not on an active mission.");
	}
}

/**
To be called from the /w command handler.
*/
static
void missions_engage_help_or_map(int playerid)
{
	struct vec3 vel;
	struct dbvehicle *veh;
	int vehicleid;

	switch (mission_stage[playerid]) {
	case MISSION_STAGE_NOMISSION:
		if (active_msp_index[playerid] != -1) {
			missions_show_jobmap_set_stage_set_controllable(playerid);
			break;
		}

		vehicleid = GetPlayerVehicleID(playerid);
		veh = gamevehicles[vehicleid].dbvehicle;
		if (!veh || GetPlayerState(playerid) != PLAYER_STATE_DRIVER) {
			SendClientMessage(playerid, COL_WARN, WARN"Get in a vehicle first!");
			break;
		}

		/*Checking speed for two reasons:
		- this command will freeze the player, meaning they will drop from the sky when flying
		- the player should stop completely before starting a mission*/
		GetVehicleVelocityUnsafe(vehicleid, &vel);
		/*Ignoring z velocity because it would be annoying with skimmers riding heavy waves.*/
		if (vel.x * vel.x + vel.y * vel.y > MAX_SPEED_SQ) {
			SendClientMessage(playerid, COL_WARN, WARN"Stop the vehicle first!");
			break;
		}

		missions_available_msptype_mask[playerid] = missions_get_vehicle_model_msptype_mask(veh->model);
		if (missions_available_msptype_mask[playerid] == 0) {
			SendClientMessage(playerid, COL_WARN, WARN"This vehicle can't do any missions!");
			break;
		}

		/*For SOME reason, when many enexes are shown, the preview can be messed up
		(as in the type of messed up you see when only model is on screen).
		Deleting all enexes before showing the textdraw would help, but it wouldn't work
		unless there's a game update in between the deletion of the objects and the textdraw creation.
		But that would add too much delay (and complexity), so I'm just letting it be messed up atm..*/

		missions_jobhelp_show(playerid, msptype_mask_to_point_mask(missions_available_msptype_mask[playerid]));
		break;
	case MISSION_STAGE_FLIGHT:
		if (active_msp_index[playerid] == activemission[playerid]->endpoint - missionpoints) {
			missions_start_unload(playerid);
			break;
		}
		SetPlayerRaceCheckpointNoDir(playerid, RACE_CP_TYPE_NORMAL, &activemission[playerid]->endpoint->pos, MISSION_CP_RAD);
		SendClientMessage(playerid, COL_WARN,
			WARN"You're already working! Use /s to stop your current work first ($"EQ(MISSION_CANCEL_FINE)" fee).");
		break;
	}
}

static
void missions_stoplocate(int playerid)
{
	if (locating_msp_index[playerid] != -1) {
		locating_msp_index[playerid] = -1;
		DisablePlayerRaceCheckpoint(playerid);
	}
}

static
void missions_locate_closest_mission(int playerid, int point_type_mask)
{
	struct MISSIONPOINT *msp;
	struct dbvehicle *veh;
	struct vec3 pos;
	int mspindex;
	int closest_index;
	float dx, dy;
	float closest_distance_sq, distance_sq;
	int vehicleid;
	int msptypemask;

	GetPlayerPos(playerid, &pos);
	closest_index = -1;
	closest_distance_sq = float_pinf;
	for (mspindex = 0; mspindex < nummissionpoints; mspindex++) {
		msptypemask = missionpoints[mspindex].type;
		if ((msptypemask & missions_available_msptype_mask[playerid]) &&
			(msptype_mask_to_point_mask(msptypemask) & point_type_mask))
		{
			dx = missionpoints[mspindex].pos.x - pos.x;
			dy = missionpoints[mspindex].pos.y - pos.y;
			distance_sq = dx * dx + dy * dy;
			if (distance_sq < closest_distance_sq) {
				closest_distance_sq = distance_sq;
				closest_index = mspindex;
			}
		}
	}

	if (closest_index == -1) {
		SendClientMessage(playerid, COL_WARN, WARN"No mission points available for this vehicle.");
	} else {
		locating_msp_index[playerid] = closest_index;
		msp = &missionpoints[closest_index];
		SetPlayerRaceCheckpointNoDir(playerid, RACE_CP_TYPE_NORMAL, &msp->pos, MISSION_CP_RAD);
		if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER) {
			vehicleid = GetPlayerVehicleID(playerid);
			veh = gamevehicles[vehicleid].dbvehicle;
			if (veh) {
				nav_navigate_to_airport(playerid, vehicleid, veh->model, msp->ap);
			}
		}
	}
}

static
void missions_driversync_udkeystate_change(int playerid, short udkey)
{
	if (mission_stage[playerid] == MISSION_STAGE_HELP) {
		if (udkey < 0) {
			mission_help_update_selection_ensure_available(playerid, mission_help_option[playerid] - 1, -1);
			PlayerPlaySound(playerid, MISSION_HELP_MOVE_UP_SOUND);
		} else if (udkey > 0) {
			mission_help_update_selection_ensure_available(playerid, mission_help_option[playerid] + 1, 1);
			PlayerPlaySound(playerid, MISSION_HELP_MOVE_DOWN_SOUND);
		}
	} else if (mission_stage[playerid] == MISSION_STAGE_JOBMAP) {
		if (udkey) {
			jobmap_move_updown(playerid, udkey);
		}
	}
}

static
void missions_driversync_keystate_change(int playerid, int oldkeys, int newkeys)
{
	register struct MISSIONPOINT *tomsp;
	struct MISSIONPOINT *frommsp;
	struct AIRPORT *selected_airport;
	unsigned int mission_type;

	if (mission_stage[playerid] == MISSION_STAGE_HELP) {
		/*Since player is set to not be controllable, these use on-foot controls even though the player is in-vehicle.*/
		if (KEY_JUST_DOWN(KEY_VEHICLE_ENTER_EXIT)) {
			PlayerPlaySound(playerid, MISSION_HELP_CANCEL_SOUND);
			missions_jobhelp_hide(playerid); /*sets controllable and mission state*/
		} else if (KEY_JUST_DOWN(KEY_SPRINT)) {
			PlayerPlaySound(playerid, MISSION_HELP_ACCEPT_SOUND);
			missions_jobhelp_hide(playerid); /*sets controllable and mission state*/
			missions_locate_closest_mission(playerid, (1 << mission_help_option[playerid]));
		}
	} else if (mission_stage[playerid] == MISSION_STAGE_JOBMAP) {
		/*Since player is set to not be controllable, these use on-foot controls even though the player is in-vehicle.*/
		if (KEY_JUST_DOWN(KEY_VEHICLE_ENTER_EXIT)) {
			PlayerPlaySound(playerid, MISSION_JOBMAP_CANCEL_SOUND);
			missions_hide_jobmap_set_stage_set_controllable(playerid); /*sets controllable and mission state*/
		} else if (KEY_JUST_DOWN(KEY_SPRINT)) {
			PlayerPlaySound(playerid, MISSION_JOBMAP_ACCEPT_SOUND);
			selected_airport = jobmap_do_accept_button(playerid);
			if (selected_airport && active_msp_index[playerid] != -1) {
				missions_hide_jobmap_set_stage_set_controllable(playerid);
				missions_get_current_msp_and_mission_type(playerid, &frommsp, &mission_type);
				tomsp = missions_get_random_msp(selected_airport, mission_type);
				if (tomsp) {
					missions_start_mission(playerid, frommsp, tomsp, mission_type);
				} else {
					logprintf("failed to get random msp from airport id %d on jobmap (mission type %08x)",
						selected_airport->id, mission_type);
					SendClientMessage(playerid, COL_WARN, WARN"Something went wrong, try again");
				}
			}
		}
	} else if (KEY_JUST_DOWN(KEY_YES) && active_msp_index[playerid] != -1) {
		if (mission_stage[playerid] == MISSION_STAGE_NOMISSION) {
			HideGameTextForPlayer(playerid); /*The key or /w help text might still be showing.*/
			missions_show_jobmap_set_stage_set_controllable(playerid);
		} else if (mission_stage[playerid] == MISSION_STAGE_FLIGHT &&
			active_msp_index[playerid] == activemission[playerid]->endpoint - missionpoints)
		{
			missions_start_unload(playerid);
		}
	}
}

void missions_on_vehicle_refueled(int vehicleid, float refuelamount)
{
	struct MISSION *miss;
	int i;

	for (i = 0; i < playercount; i++) {
		if ((miss = activemission[players[i]]) != NULL &&
			miss->veh->spawnedvehicleid == vehicleid)
		{
			miss->fuelburned += refuelamount;
			miss->lastfuel = miss->veh->fuel;
			return;
		}
	}
}

void missions_on_vehicle_repaired(int vehicleid, float fixamount, float newhp)
{
	struct MISSION *miss;
	int i;

	for (i = 0; i < playercount; i++) {
		if ((miss = activemission[players[i]]) != NULL &&
			miss->veh->spawnedvehicleid == vehicleid)
		{
			miss->damagetaken += (short) fixamount;
			miss->lastvehiclehp = (short) newhp;
			return;
		}
	}
}

void missions_update_satisfaction(int pid, int vid, float pitch, float roll)
{
	struct RPCDATA_TextDrawSetString rpcdata;
	struct MISSION *miss;
	int last_satisfaction;
	float pitchlimit, rolllimit;

	if (mission_stage[pid] == MISSION_STAGE_FLIGHT &&
		(miss = activemission[pid]) != NULL &&
		(miss->missiontype & PASSENGER_MISSIONTYPES) &&
		miss->veh->spawnedvehicleid == vid &&
		miss->passenger_satisfaction != 0)
	{
		if (game_is_heli(miss->veh->model)) {
			pitchlimit = 50.0f;
			rolllimit = 40.0f;
		} else {
			pitchlimit = 20.0f;
			rolllimit = 36.0f;
		}
		last_satisfaction = miss->passenger_satisfaction;

		if (pitch < 0.0f) {
			pitch = -pitch;
		}
		if (pitch > pitchlimit) {
			miss->passenger_satisfaction -= (int) ((pitch - pitchlimit) / 2.0f);
			if (miss->passenger_satisfaction < 0) {
				miss->passenger_satisfaction = 0;
			}
		}

		if (roll < 0.0f) {
			roll = -roll;
		}
		if (roll > rolllimit) {
			miss->passenger_satisfaction -= (int) ((roll - rolllimit) / 2.0f);
			if (miss->passenger_satisfaction < 0) {
				miss->passenger_satisfaction = 0;
			}
		}

		if (last_satisfaction != miss->passenger_satisfaction) {
			rpcdata.textdrawid = td_satisfaction.rpcdata->textdrawid;
			rpcdata.text_length = (short) missions_format_satisfaction_text(miss->passenger_satisfaction, rpcdata.text);
			SendRPCToPlayer(pid, RPC_TextDrawSetString, &rpcdata, 2 + 2 + rpcdata.text_length, 2);
		}
	}
}

int missions_get_distance_to_next_cp(int playerid, struct vec3 *frompos)
{
	struct MISSION *miss;
	struct vec3 cp;

	if ((miss = activemission[playerid]) == NULL) {
		return -1;
	}

	if (mission_stage[playerid] < MISSION_STAGE_FLIGHT) {
		cp = miss->startpoint->pos;
	} else {
		cp = miss->endpoint->pos;
	}

	cp.x -= frompos->x;
	cp.y -= frompos->y;
	cp.z -= frompos->z;
	return (int) sqrt(common_vectorsize_sq(cp));
}

static
int missions_format_kneeboard_info_text(int playerid, char *dest)
{
	if (mission_stage[playerid] > MISSION_STAGE_LOAD) {
		return sprintf(dest, "~w~Flight from %s to ~r~%s~w~.",
			activemission[playerid]->startpoint->ap->name,
			activemission[playerid]->endpoint->ap->name);
	} else if (mission_stage[playerid] >= MISSION_STAGE_PRELOAD) {
		return sprintf(dest, "~w~Flight from ~r~%s~w~ to %s.",
			activemission[playerid]->startpoint->ap->name,
			activemission[playerid]->endpoint->ap->name);
	} else {
		return sprintf(dest, "%s", "~w~Not on a job, type ~y~/w~w~ to find a missionpoint.");
	}
}
