#define TRACKER_PORT 7766

#define MAX_UNLOAD_SPEED_KNOTS (35.0f)
#define MAX_UNLOAD_SPEED_VEL (MAX_UNLOAD_SPEED_KNOTS / VEL_TO_KTS)
#define MAX_UNLOAD_SPEED_SQ_VEL (MAX_UNLOAD_SPEED_VEL * MAX_UNLOAD_SPEED_VEL)

const static char *SATISFACTION_TEXT_FORMAT = "Passenger~n~Satisfaction: %d%%";
const static char *LOADING = "~p~Loading...";
const static char *UNLOADING = "~p~Unloading...";

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
The selected option in the job help screen, value should be 0-NUM_PRESET_MISSION_LOCATIONS.
*/
static char mission_map_option[MAX_PLAYERS];
/**
Available mission point types for player.
Usages are a.o. for in job help screen (after converting to point type) and to show mission point enexes.
*/
static char missions_available_msptype_mask[MAX_PLAYERS];
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
See {@code MISSION_STAGE} definitions.
*/
static char mission_stage[MAX_PLAYERS];
static struct MISSION *activemission[MAX_PLAYERS];
/**
Description for every mission type.
Example: "Andromada/AT-400" which would go together with {@link mission_type_names} at same index: "Passengers (L)"
Index is the bit of MISSION_TYPE_* definitions.
*/
static char *mission_type_text[NUM_MISSION_TYPES];
static char textpool_mission_type[434];
/*whether one flightdata data packet with afk flag has been sent already*/
static char tracker_afk_packet_sent[MAX_PLAYERS];
/**
Tracker socket handle.
*/
static int tracker;
/**
Player textdraw handles for passenger satisfaction indicator.
*/
static int ptxt_satisfaction[MAX_PLAYERS];

#define NUM_HELP_TEXTDRAWS (17)
static struct TEXTDRAW td_jobhelp_keyhelp = { "keyhelp", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobhelp_header = { "header", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobhelp_optsbg = { "optsbg", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobhelp_text = { "text", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobhelp_greenbtnbg = { "greenbtnbg", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobhelp_bluebtnbg = { "bluebtnbg", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobhelp_redbtnbg = { "redbtnbg", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobhelp_optselbg = { "optselbg", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobhelp_enexgreen = { "enexgreen", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobhelp_txtgreen = { "txtgreen", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobhelp_actiongreen = { "actiongreen", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobhelp_enexblue = { "enexblue", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobhelp_txtblue = { "txtblue", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobhelp_actionblue = { "actionblue", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobhelp_enexred = { "enexred", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobhelp_txtred = { "txtred", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobhelp_actionred = { "actionred", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };

#define NUM_MAP_TEXTDRAWS (28)
static struct TEXTDRAW td_jobmap_keyhelp = { "keyhelp", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobmap_header = { "header", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobmap_optsbg = { "optsbg", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobmap_mapbg = { "mapbg", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobmap_island_mainland = { "mapsqfull", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobmap_island_octa = { "mapsqmintopleft", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobmap_island_lima = { "mapsqminbotright", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
#if AIRPORT_CODE_LEN != 4
#error "mapmin/mapmax text alloc depend on airport code being 4 chars at most"
#endif
static struct TEXTDRAW td_jobmap_from = { "mapmin", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobmap_to = { "mapmax", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobmap_opt1bg = { "opt1bg", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobmap_opt2bg = { "opt2bg", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobmap_opt3bg = { "opt3bg", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobmap_opt4bg = { "opt4bg", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobmap_opt5bg = { "opt5bg", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobmap_opt6bg = { "opt6bg", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobmap_opt7bg = { "opt7bg", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobmap_opt8bg = { "opt8bg", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobmap_opt9bg = { "opt9bg", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
static struct TEXTDRAW td_jobmap_optselbg = { "optselbg", TEXTDRAW_ALLOC_AS_NEEDED, 0, NULL };
/*Checking for 8 because the last one is the random entry.*/
#if NUM_PRESET_MISSION_LOCATIONS != 8
#error "num option textdraws does not match, hurr durr"
#endif
/*Very scientific text measurement system: "Some very long airport name gate P1 - Very special mission type (M) - 100000m"*/
#define JOBMAP_ENTRY_MAX_TEXT_LENGTH (100)
static struct TEXTDRAW td_jobmap_opt1multiline = { "opt1multiline", 0, 0, NULL }; /*This one is just for measurements.*/
static struct TEXTDRAW td_jobmap_opt1 = { "opt1", JOBMAP_ENTRY_MAX_TEXT_LENGTH, 0, NULL };
static struct TEXTDRAW td_jobmap_opt2 = { "opt2", JOBMAP_ENTRY_MAX_TEXT_LENGTH, 0, NULL };
static struct TEXTDRAW td_jobmap_opt3 = { "opt3", JOBMAP_ENTRY_MAX_TEXT_LENGTH, 0, NULL };
static struct TEXTDRAW td_jobmap_opt4 = { "opt4", JOBMAP_ENTRY_MAX_TEXT_LENGTH, 0, NULL };
static struct TEXTDRAW td_jobmap_opt5 = { "opt5", JOBMAP_ENTRY_MAX_TEXT_LENGTH, 0, NULL };
static struct TEXTDRAW td_jobmap_opt6 = { "opt6", JOBMAP_ENTRY_MAX_TEXT_LENGTH, 0, NULL };
static struct TEXTDRAW td_jobmap_opt7 = { "opt7", JOBMAP_ENTRY_MAX_TEXT_LENGTH, 0, NULL };
static struct TEXTDRAW td_jobmap_opt8 = { "opt8", JOBMAP_ENTRY_MAX_TEXT_LENGTH, 0, NULL };
static struct TEXTDRAW td_jobmap_opt9 = { "opt9", JOBMAP_ENTRY_MAX_TEXT_LENGTH, 0, NULL };

static float map_text_x_base, map_text_x_unit;
static float map_text_y_base, map_text_y_unit;
static float map_text_multiline_offset;

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

static
void missions_init_type_text()
{
	int type_index;
	int type;
	int vehiclemodel;
	char *position;

	position = textpool_mission_type;
	for (type_index = 0; type_index < NUM_MISSION_TYPES; type_index++) {
		mission_type_text[type_index] = position;
		type = 1 << type_index;
		for (vehiclemodel = VEHICLE_MODEL_MIN; vehiclemodel <= VEHICLE_MODEL_MAX; vehiclemodel++) {
			if ((type & missions_get_vehicle_model_msptype_mask(vehiclemodel))) {
				position += sprintf(position, "%s/", vehnames[vehiclemodel - VEHICLE_MODEL_MIN]);
			}
		}
		/*Replace the last / with a zero term (and now position is right after that zero term)*/
		position[-1] = 0;
	}

	if (position + 1 > textpool_mission_type - 1 + sizeof(textpool_mission_type)) {
#if DEV
		printf("need %d more bytes\n", position - textpool_mission_type - 1 - sizeof(textpool_mission_type));
#endif
		assert(0);
	}
}

/**
Makes sure every possible location of the missionpoint is set.

All locations are reset.
*/
static
void missions_update_mission_locations(struct MISSIONPOINT *msp)
{
	int applicable_typeidx[NUM_MISSION_TYPES];
	int num_applicable_typeidx;
	int applicable_airports[MAX_AIRPORTS];
	struct MISSIONPOINT *applicable_missionpoints[MAX_MISSIONPOINTS_PER_AIRPORT];
	struct MISSIONPOINT *tmp_msp;
	int num_applicable_airports, num_applicable_msp;
	int airportidx, locationidx, mspidx, typeidx;

	for (locationidx = 0; locationidx < NUM_PRESET_MISSION_LOCATIONS; locationidx++) {
		/*Collect all applicable airports.*/
		num_applicable_airports = 0;
		for (airportidx = 0; airportidx < numairports; airportidx++) {
			if (msp->ap != airports + airportidx && (airports[airportidx].missiontypes & msp->type)) {
				applicable_airports[num_applicable_airports++] = airportidx;
			}
		}

		/*Select one of the airports.*/
		if (num_applicable_airports == 0) {
			/*TODO replace with logprintf*/
			printf("msp id %d\n", msp->id);
			/*This func is called on boot for all missionpoints,
			so if this ever fails it will be at boot and never at a random time.*/
			assert(((void) "no other airport for msp", 0));
		}
		airportidx = 0;
		if (num_applicable_airports > 1) {
			airportidx = NC_random(num_applicable_airports);
		}
		airportidx = applicable_airports[airportidx];

		/*Collect all applicable missionpoints (for the selected airport).*/
		num_applicable_msp = 0;
		for (mspidx = 0; mspidx < airports[airportidx].num_missionpts; mspidx++) {
			tmp_msp = airports[airportidx].missionpoints + mspidx;
			/*No need to check if this is the same msp, since it's a different airport.*/
			if (tmp_msp->type & msp->type) {
				applicable_missionpoints[num_applicable_msp++] = tmp_msp;
			}
		}

		assert(num_applicable_airports != 0);

		/*Select one.*/
		mspidx = 0;
		if (num_applicable_msp > 1) {
			mspidx = NC_random(num_applicable_msp);
		}
		msp->missionlocations[locationidx] = applicable_missionpoints[mspidx];

		/*Decide on the type (since the type can still be mixed).*/
		num_applicable_typeidx = 0;
		for (typeidx = 0; typeidx < NUM_MISSION_TYPES; typeidx++) {
			if (msp->type & msp->missionlocations[locationidx]->type & (1 << typeidx)) {
				applicable_typeidx[num_applicable_typeidx++] = typeidx;
			}
		}
		typeidx = 0;
		if (num_applicable_typeidx > 1) {
			typeidx = NC_random(num_applicable_typeidx);
		}
		msp->missiontypeindices[locationidx] = applicable_typeidx[typeidx];
	}
}

/**
Also resets the player's {@link mission_stage} and sets player controllable.
*/
static
void missions_jobmap_hide(int playerid)
{
	mission_stage[playerid] = MISSION_STAGE_NOMISSION;
	TogglePlayerControllable(playerid, 1);
	textdraws_hide_consecutive(playerid, NUM_MAP_TEXTDRAWS, TEXTDRAW_MISSIONMAP_BASE);
}

/**
@param newselection new selected option
@param direction to go in, in case the selected option is invalid
*/
static
void mission_map_update_selection_ensure_available(int playerid, int newselection, int direction)
{
	struct MISSIONPOINT *msp, *tomsp;
	int mission_typeidx;
	int preset_option_idx;
	float optdif;

	assert(direction != 0);

	mission_map_option[playerid] = newselection;
invalid:
	/*Number of options is amount of preset locations + the one random option.*/
	if (mission_map_option[playerid] > NUM_PRESET_MISSION_LOCATIONS + 1) {
		mission_map_option[playerid] = 0;
	} else if (mission_map_option[playerid] < 0) {
		mission_map_option[playerid] = NUM_PRESET_MISSION_LOCATIONS + 1;
	}
	/*The first option is the 'random destination', and is always available.*/
	if (mission_map_option[playerid]) {
		msp = &missionpoints[active_msp_index[playerid]];
		/*-1 because 0 is the 'random destination' option*/
		preset_option_idx = mission_map_option[playerid] - 1;
		mission_typeidx = msp->missiontypeindices[preset_option_idx];
		if (!((1 << mission_typeidx) & missions_available_msptype_mask[playerid])) {
			mission_map_option[playerid] += direction;
			goto invalid;
		}
		tomsp = msp->missionlocations[preset_option_idx];
		strcpy(td_jobmap_to.rpcdata->text, tomsp->ap->code);
		td_jobmap_to.rpcdata->x = map_text_x_base + map_text_x_unit * (tomsp->pos.x + 20000.0f);
		td_jobmap_to.rpcdata->y = map_text_y_base + map_text_y_unit * (-tomsp->pos.y + 20000.0f);
	} else {
		strcpy(td_jobmap_to.rpcdata->text, "????");
		td_jobmap_to.rpcdata->x = map_text_x_base + map_text_x_unit * 20000.0f;
		td_jobmap_to.rpcdata->y = map_text_y_base + map_text_y_unit * 20000.0f;
	}

	optdif = td_jobmap_opt2bg.rpcdata->y - td_jobmap_opt1bg.rpcdata->y;
	td_jobmap_optselbg.rpcdata->y = td_jobmap_opt1bg.rpcdata->y + mission_map_option[playerid] * optdif;
	textdraws_show(playerid, 2, &td_jobmap_optselbg, &td_jobmap_to);
}

/**
Also sets the player's {@link mission_stage} and sets player uncontrollable.
Requires active_msp_index to be valid.
*/
static
void missions_jobmap_show(int playerid)
{
	/*Opt1 is the 'random' entry, which is always available.*/
	static struct TEXTDRAW *optiontexts[] = {
		&td_jobmap_opt2, &td_jobmap_opt3, &td_jobmap_opt4,
		&td_jobmap_opt5, &td_jobmap_opt6, &td_jobmap_opt7,
		&td_jobmap_opt8, &td_jobmap_opt9,
	};
	static struct TEXTDRAW *optionbgs[] = {
		&td_jobmap_opt2bg, &td_jobmap_opt3bg, &td_jobmap_opt4bg,
		&td_jobmap_opt5bg, &td_jobmap_opt6bg, &td_jobmap_opt7bg,
		&td_jobmap_opt8bg, &td_jobmap_opt9bg,
	};

	struct RPCDATA_ShowTextDraw *txtrpc;
	struct MISSIONPOINT *msp, *tomsp;
	float dx, dy, dz;
	int distance;
	int jobidx;
	int is_available;
	float original_text_y_locations[NUM_PRESET_MISSION_LOCATIONS];

	msp = &missionpoints[active_msp_index[playerid]];
	mission_stage[playerid] = MISSION_STAGE_JOBMAP;
	TogglePlayerControllable(playerid, 0);

	for (jobidx = 0; jobidx < NUM_PRESET_MISSION_LOCATIONS; jobidx++) {
		tomsp = msp->missionlocations[jobidx];
		dx = msp->pos.x - tomsp->pos.x;
		dy = msp->pos.y - tomsp->pos.y;
		dz = msp->pos.z - tomsp->pos.z;
		distance = (int) sqrt(dx * dx + dy * dy + dz * dz);
		is_available = missions_available_msptype_mask[playerid] & (1 << msp->missiontypeindices[jobidx]);
		txtrpc = optiontexts[jobidx]->rpcdata;
		original_text_y_locations[jobidx] = txtrpc->y;
		txtrpc->text_length = sprintf(txtrpc->text,
			"~w~%s - %s - %dm",
			tomsp->ap->name,
			mission_type_names[msp->missiontypeindices[jobidx]],
			distance);
		if (is_available) {
			optionbgs[jobidx]->rpcdata->box_color = 0xff333333;
		} else {
			optionbgs[jobidx]->rpcdata->box_color = 0xff202020;
			txtrpc->y += map_text_multiline_offset;
			txtrpc->text[1] = 'r';
			txtrpc->text_length += sprintf(txtrpc->text + txtrpc->text_length,
				"~n~Requires %s",
				mission_type_text[msp->missiontypeindices[jobidx]]);
		}
#ifdef DEV
		if (optiontexts[jobidx]->rpcdata->text_length >= JOBMAP_ENTRY_MAX_TEXT_LENGTH) {
			/*TODO replace with logprintf thing once patched*/
			printf("msp id %d\n", tomsp->id);
			assert(((void) "mission entry text too long", 0));
		}
#endif
	}

	strcpy(td_jobmap_from.rpcdata->text, msp->ap->code);
	td_jobmap_from.rpcdata->x = map_text_x_base + map_text_x_unit * (msp->pos.x + 20000.0f);
	td_jobmap_from.rpcdata->y = map_text_y_base + map_text_y_unit * (-msp->pos.y + 20000.0f);

	/*Showing all but "selection option background" and "to" textdraws.*/
	textdraws_show(playerid, NUM_MAP_TEXTDRAWS - 2,
		&td_jobmap_keyhelp, &td_jobmap_header, &td_jobmap_optsbg, &td_jobmap_mapbg,
		&td_jobmap_island_mainland, &td_jobmap_island_octa, &td_jobmap_island_lima,
		&td_jobmap_from, /*&td_jobmap_to, */&td_jobmap_opt1bg, &td_jobmap_opt2bg, &td_jobmap_opt3bg,
		&td_jobmap_opt4bg, &td_jobmap_opt5bg, &td_jobmap_opt6bg, &td_jobmap_opt7bg,
		&td_jobmap_opt8bg, &td_jobmap_opt9bg, /*&td_jobmap_optselbg, */&td_jobmap_opt1,
		&td_jobmap_opt2, &td_jobmap_opt3, &td_jobmap_opt4, &td_jobmap_opt5, &td_jobmap_opt6,
		&td_jobmap_opt7, &td_jobmap_opt8, &td_jobmap_opt9);

	mission_map_update_selection_ensure_available(playerid, mission_map_option[playerid], 1);

	for (jobidx = 0; jobidx < NUM_PRESET_MISSION_LOCATIONS; jobidx++) {
		txtrpc->y = original_text_y_locations[jobidx];
	}
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

	struct MISSIONPOINT *msp;
	int airportidx, indicatoridx, missionptidx, idxtouse;
	float dx, dy, dz;

	/*TODO: as long as missionhelp/map is option, this shouldn't update (because the active_point must not change*/

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
					rpcdata_CreateObject.rotx = 0.0f;
					rpcdata_CreateObject.roty = 0.0f;
					rpcdata_CreateObject.rotz = 0.0f;
					rpcdata_CreateObject.drawdistance = 1000.0f;
					SAMP_SendRPCToPlayer(RPC_CreateObject, &bitstream_create_object, playerid, 2);
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
			SAMP_SendRPCToPlayer(RPC_DestroyObject, &bitstream_destroy_object, playerid, 2);
			missionpoint_indicator_state[playerid][indicatoridx] = MISSIONPOINT_INDICATOR_STATE_FREE;
#ifdef MISSIONS_LOG_POINT_INDICATOR_ALLOC
			printf("%d %d destroyed %d\n", indicatoridx, missionpoint_indicator_index[playerid][indicatoridx], time_timestamp());
#endif
		}
	}

	/*update active_msp_index and show 'press ...' text if needed*/
	if (NC_GetPlayerVehicleSeat(playerid) != 0) {
		goto force_no_active_msp;
	}
	for (indicatoridx = 0; indicatoridx < MAX_MISSION_INDICATORS; indicatoridx++) {
		if (missionpoint_indicator_state[playerid][indicatoridx] == MISSIONPOINT_INDICATOR_STATE_USED) {
			msp = &missionpoints[missionpoint_indicator_index[playerid][indicatoridx]];
			dx = msp->pos.x - player_x;
			dy = msp->pos.y - player_y;
			dz = msp->pos.z - player_z;
			if (dx * dx + dy * dy + dz * dz < MISSION_CHECKPOINT_SIZE_SQ) {
				if (active_msp_index[playerid] != missionpoint_indicator_index[playerid][indicatoridx]) {
					active_msp_index[playerid] = missionpoint_indicator_index[playerid][indicatoridx];
					/*TODO: sometimes when entering/exiting vehicle, this text doesn't show even though code gets hit*/
					GameTextForPlayer(playerid, 3000, 3,
						"~w~Press ~b~~k~~CONVERSATION_YES~~w~ to view missions,~n~or type ~b~/w");
				}
				goto have_active_msp;
			}
		}
	}
force_no_active_msp:
	active_msp_index[playerid] = -1;
have_active_msp:
	;

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

	free(td_jobmap_keyhelp.rpcdata);
	free(td_jobmap_header.rpcdata);
	free(td_jobmap_optsbg.rpcdata);
	free(td_jobmap_mapbg.rpcdata);
	free(td_jobmap_island_mainland.rpcdata);
	free(td_jobmap_island_octa.rpcdata);
	free(td_jobmap_island_lima.rpcdata);
	free(td_jobmap_from.rpcdata);
	free(td_jobmap_to.rpcdata);
	free(td_jobmap_opt1bg.rpcdata);
	free(td_jobmap_opt2bg.rpcdata);
	free(td_jobmap_opt3bg.rpcdata);
	free(td_jobmap_opt4bg.rpcdata);
	free(td_jobmap_opt5bg.rpcdata);
	free(td_jobmap_opt6bg.rpcdata);
	free(td_jobmap_opt7bg.rpcdata);
	free(td_jobmap_opt8bg.rpcdata);
	free(td_jobmap_opt9bg.rpcdata);
	free(td_jobmap_optselbg.rpcdata);
	/*Don't free opt1multiline, it's already free'd right after loading.*/
	free(td_jobmap_opt1.rpcdata);
	free(td_jobmap_opt2.rpcdata);
	free(td_jobmap_opt3.rpcdata);
	free(td_jobmap_opt4.rpcdata);
	free(td_jobmap_opt5.rpcdata);
	free(td_jobmap_opt6.rpcdata);
	free(td_jobmap_opt7.rpcdata);
	free(td_jobmap_opt8.rpcdata);
	free(td_jobmap_opt9.rpcdata);
}

/**
Size the textdraws that are used for their box to indicate islands on the jobmap.
*/
static
void missions_init_jobmap_islands()
{
	float map_coords_fontheight_base, map_coords_fontheight_unit;
	float map_coords_y_base, map_coords_y_unit;
	float map_coords_x_base, map_coords_x_unit;
	float boxwidth_unit;
	float boxwidth_min, boxwidth_max;
	struct ZONE *zone;
	int zoneindex, maxzoneindex;
	int regionindex;
	int octa_region_found;
	float min_x, min_y, max_x, max_y;

	map_text_x_base = td_jobmap_from.rpcdata->x;
	map_text_x_unit = (td_jobmap_to.rpcdata->x - td_jobmap_from.rpcdata->x) / 40000.0f;

	map_text_y_base = td_jobmap_from.rpcdata->y;
	map_text_y_unit = (td_jobmap_to.rpcdata->y - td_jobmap_from.rpcdata->y) / 40000.0f;

	map_coords_x_base = td_jobmap_island_octa.rpcdata->x;
	map_coords_x_unit = (td_jobmap_island_lima.rpcdata->x - td_jobmap_island_octa.rpcdata->x) / 40000.0f;

	map_coords_y_base = td_jobmap_island_octa.rpcdata->y;
	map_coords_y_unit = (td_jobmap_island_lima.rpcdata->y - td_jobmap_island_octa.rpcdata->y) / 40000.0f;

	/*actual box width is the relation of x (font position) to box_width*/
	boxwidth_min = td_jobmap_island_octa.rpcdata->box_width - td_jobmap_island_octa.rpcdata->x;
	boxwidth_max = td_jobmap_island_mainland.rpcdata->box_width - td_jobmap_island_mainland.rpcdata->x;
	boxwidth_unit = (boxwidth_max - boxwidth_min) / 40000.0f;

	map_coords_fontheight_base = td_jobmap_island_octa.rpcdata->font_height;
	map_coords_fontheight_unit = (td_jobmap_island_mainland.rpcdata->font_height - map_coords_fontheight_base) / 40000.0f;

	td_jobmap_island_mainland.rpcdata->x = map_coords_x_base + map_coords_x_unit * (20000.0f - 3000.0f);
	td_jobmap_island_mainland.rpcdata->box_width = td_jobmap_island_mainland.rpcdata->x + boxwidth_unit * 6000.0f;
	td_jobmap_island_mainland.rpcdata->y = map_coords_y_base + map_coords_y_unit * (20000.0f - 3000.0f);
	td_jobmap_island_mainland.rpcdata->font_height = map_coords_fontheight_base + map_coords_fontheight_unit * 6000.0f;

	octa_region_found = 0;
	for (regionindex = 0; regionindex < regioncount; regionindex++) {
		if (regions[regionindex].zone.id == ZONE_OCTA) {
			zoneindex = regions[regionindex].minzone;
			maxzoneindex = regions[regionindex].maxzone;
			octa_region_found = 1;
			min_x = min_y = float_pinf;
			max_x = max_y = float_ninf;
			for (; zoneindex < maxzoneindex; zoneindex++) {
				zone = &zones[zoneindex];
				if (zone->min_x < min_x) {
					min_x = zone->min_x;
				}
				if (zone->min_y < min_y) {
					min_y = zone->min_y;
				}
				if (zone->max_x > max_x) {
					max_x = zone->max_x;
				}
				if (zone->max_y > max_y) {
					max_y = zone->max_y;
				}
			}
			assert(min_x < max_x);
			assert(min_y < max_y);

			td_jobmap_island_octa.rpcdata->x = map_coords_x_base + map_coords_x_unit * (20000.0f + min_x);
			td_jobmap_island_octa.rpcdata->box_width = td_jobmap_island_octa.rpcdata->x + boxwidth_unit * (max_x - min_x);
			td_jobmap_island_octa.rpcdata->y = map_coords_y_base + map_coords_y_unit * (20000.0f + -max_y);
			td_jobmap_island_octa.rpcdata->font_height = map_coords_fontheight_base + map_coords_fontheight_unit * (max_y - min_y);

			/*TODO this is temp until lima gets added*/
			td_jobmap_island_lima.rpcdata->x = td_jobmap_island_octa.rpcdata->x;
			td_jobmap_island_lima.rpcdata->box_width = td_jobmap_island_octa.rpcdata->box_width;
			td_jobmap_island_lima.rpcdata->y = td_jobmap_island_octa.rpcdata->y;
			td_jobmap_island_lima.rpcdata->font_height = td_jobmap_island_octa.rpcdata->font_height;
			break;
		}
	}
	assert(octa_region_found);
}

static
void missions_init()
{
	int i;

	missions_init_type_text();

	for (i = 0; i < nummissionpoints; i++) {
		missions_update_mission_locations(&missionpoints[i]);
	}

	/*varinit*/
	for (i = 0; i < MAX_PLAYERS; i++) {
		activemission[i] = NULL;
	}

	/*textdraws*/
	textdraws_load_from_file("jobhelp", TEXTDRAW_MISSIONHELP_BASE, NUM_HELP_TEXTDRAWS,
		/*remember to free their rpcdata in missions_dispose*/
		&td_jobhelp_keyhelp, &td_jobhelp_header, &td_jobhelp_optsbg,
		&td_jobhelp_text, &td_jobhelp_greenbtnbg, &td_jobhelp_bluebtnbg,
		&td_jobhelp_redbtnbg, &td_jobhelp_optselbg, &td_jobhelp_enexgreen,
		&td_jobhelp_txtgreen, &td_jobhelp_actiongreen, &td_jobhelp_enexblue,
		&td_jobhelp_txtblue, &td_jobhelp_actionblue, &td_jobhelp_enexred,
		&td_jobhelp_txtred, &td_jobhelp_actionred);
	textdraws_set_textbox_properties(td_jobhelp_keyhelp.rpcdata);

	/*Num +1 for the opt1multiline, which is just loaded for measurements.*/
	textdraws_load_from_file("jobmap", TEXTDRAW_MISSIONMAP_BASE, NUM_MAP_TEXTDRAWS + 1,
		/*remember to free their rpcdata in missions_dispose*/
		&td_jobmap_keyhelp, &td_jobmap_header, &td_jobmap_optsbg, &td_jobmap_mapbg,
		&td_jobmap_island_mainland, &td_jobmap_island_octa, &td_jobmap_island_lima,
		&td_jobmap_from, &td_jobmap_to, &td_jobmap_opt1bg, &td_jobmap_opt2bg, &td_jobmap_opt3bg,
		&td_jobmap_opt4bg, &td_jobmap_opt5bg, &td_jobmap_opt6bg, &td_jobmap_opt7bg,
		&td_jobmap_opt8bg, &td_jobmap_opt9bg, &td_jobmap_optselbg, &td_jobmap_opt1,
		&td_jobmap_opt2, &td_jobmap_opt3, &td_jobmap_opt4, &td_jobmap_opt5, &td_jobmap_opt6,
		&td_jobmap_opt7, &td_jobmap_opt8, &td_jobmap_opt9, &td_jobmap_opt1multiline);
	textdraws_set_textbox_properties(td_jobmap_keyhelp.rpcdata);
	map_text_multiline_offset = td_jobmap_opt1multiline.rpcdata->y - td_jobmap_opt1.rpcdata->y;
	free(td_jobmap_opt1multiline.rpcdata);

	missions_init_jobmap_islands();

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
Find a random endpoint for given missiontype, skipping airport of startpoint.

@param startpoint may be NULL
@return NULL on no applicable points found
*/
static
struct MISSIONPOINT *missions_get_random_endpoint(int missiontype, struct MISSIONPOINT *startpoint)
{
	struct MISSIONPOINT *msp, **possible_missionpts;
	struct AIRPORT *airport, **possible_airports;
	int num_possible_airports, num_possible_missionpts;
	int i;

	if (startpoint == NULL) {
		return NULL;
	}

	i = numairports;
	airport = airports;
	possible_airports = malloc(sizeof(struct AIRPORT*) * numairports);
	num_possible_airports = 0;
	while (i--) {
		if ((airport->missiontypes & missiontype) &&
			airport != startpoint->ap)
		{
			possible_airports[num_possible_airports++] = airport;
		}
		airport++;
	}

	switch (num_possible_airports) {
	case 0: airport = NULL; break;
	case 1: airport = possible_airports[0]; break;
	default:
		airport = possible_airports[NC_random(num_possible_airports)];
		break;
	}

	free(possible_airports);
	if (airport == NULL) {
		return NULL;
	}

	/*got a random airport, now mission points*/
	msp = airport->missionpoints;
	possible_missionpts = malloc(sizeof(int*) * airport->num_missionpts);
	num_possible_missionpts = 0;
	for (i = airport->num_missionpts; i > 0; i--) {
		if (msp->type & missiontype) {
			possible_missionpts[num_possible_missionpts++] = msp;
		}
		msp++;
	}

	switch (num_possible_missionpts) {
	case 0:
		/*should not happen, since only airports that has the wanted
		missiontype have been selected*/
		msp = NULL;
		break;
	case 1:
		msp = possible_missionpts[0];
		break;
	default:
		msp = possible_missionpts[NC_random(num_possible_missionpts)];
		break;
	}
	free(possible_missionpts);
	return msp;
}

/**
TODO REMOVE
Find a random startpoint for given missiontype.

Startpoint is chosen based on least amount of currently active missions, then
distance.

@return NULL on no applicable points found
*/
static
struct MISSIONPOINT *missions_get_startpoint(int missiontype, struct vec3 *ppos)
{
	struct AIRPORT *airport;
	struct MISSIONPOINT *msp, **free_missionpts;
	float dx, dy, dz, dist, shortest_distance;
	int i, num_free_missionpts;

	shortest_distance = float_pinf;
	airport = NULL;
	i = numairports;
	while (i--) {
		if (airports[i].missiontypes & missiontype) {
			dx = airports[i].pos.x - ppos->x;
			dy = airports[i].pos.y - ppos->y;
			dz = airports[i].pos.z - ppos->z;
			dist = dx * dx + dy * dy + dz * dz;
			if (dist < shortest_distance) {
				shortest_distance = dist;
				airport = airports + i;
			}
		}
	}

	if (airport == NULL) {
		return NULL;
	}

	/*first select mission points with least amount of active missions*/
	free_missionpts = malloc(sizeof(int*) * airport->num_missionpts);
	num_free_missionpts = 0;
	msp = airport->missionpoints;
	for (i = airport->num_missionpts; i > 0; i--) {
		if (msp->type & missiontype) {
			free_missionpts[num_free_missionpts++] = msp;
		}
		msp++;
	}

	/*then choose the missionpoint that is the closest to the player*/
	shortest_distance = float_pinf;
	while (num_free_missionpts--) {
		dx = free_missionpts[num_free_missionpts]->pos.x - ppos->x;
		dy = free_missionpts[num_free_missionpts]->pos.y - ppos->y;
		dz = free_missionpts[num_free_missionpts]->pos.z - ppos->z;
		dist = dx * dx + dy * dy + dz * dz;
		if (dist < shortest_distance) {
			shortest_distance = dist;
			msp = free_missionpts[num_free_missionpts];
		}
	}

	free(free_missionpts);
	return msp;
}

/**
Cleanup a mission and free memory. Does not query. Does send a tracker msg.

Player must be on a mission or get segfault'd.

Call when ending a mission.
*/
static
void missions_cleanup(int playerid)
{
	struct MISSION *mission;

	mission = activemission[playerid];

	/* flight tracker packet 3 */
	buf32[0] = 0x03594C46;
	buf32[1] = mission->id;
	NC_ssocket_send(tracker, buf32a, 8);

	if (mission->missiontype & PASSENGER_MISSIONTYPES) {
		NC_PlayerTextDrawHide(playerid, ptxt_satisfaction[playerid]);
	}

	free(mission);
	activemission[playerid] = NULL;

	/*this might not be needed if mission was finished normally and player
	uses the auto work setting, but that's just a minor optimization*/
	kneeboard_reset_show(playerid);
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
			B144(WARN" Your mission vehicle has been destroyed, "
				"your mission has been aborted and you have "
				"been fined $"EQ(MISSION_CANCEL_FINE)".");
			NC_SendClientMessage(playerid, COL_WARN, buf144a);
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
	mission_help_option[playerid] = 0;
	mission_map_option[playerid] = 0;
	mission_stage[playerid] = MISSION_STAGE_NOMISSION;
	activemission[playerid] = NULL;

	NC_PARS(4);
	nc_params[1] = playerid;
	nc_paramf[2] = 88.0f;
	nc_paramf[3] = 425.0f;
	nc_params[4] = underscorestringa;
	nc_params[2] = NC(n_CreatePlayerTextDraw);
	ptxt_satisfaction[playerid] = nc_params[2];
	nc_paramf[3] = 0.3f;
	nc_paramf[4] = 1.0f;
	NC(n_PlayerTextDrawLetterSize);

	NC_PARS(3);
	nc_params[3] = 255;
	NC(n_PlayerTextDrawBackgroundColor);
	nc_params[3] = 2;
	NC(n_PlayerTextDrawAlignment);
	nc_params[3] = 1;
	NC(n_PlayerTextDrawFont);
	NC(n_PlayerTextDrawSetProportional);
	NC(n_PlayerTextDrawSetOutline);
	nc_params[3] = -1;
	NC(n_PlayerTextDrawColor);
}

void missions_on_player_death(int playerid)
{
	int stopreason, vehicleid;
	float hp;

	switch (mission_stage[playerid]) {
	case MISSION_STAGE_NOMISSION:
		break;
	case MISSION_STAGE_HELP:
		missions_jobhelp_hide(playerid); /*sets controllable and mission stage*/
		break;
	case MISSION_STAGE_JOBMAP:
		missions_jobmap_hide(playerid); /*sets controllable and mission stage*/
		break;
	default:
		NC_DisablePlayerRaceCheckpoint(playerid);
		stopreason = MISSION_STATE_DIED;
		vehicleid = NC_GetPlayerVehicleID(playerid);
		if (vehicleid) {
			hp = anticheat_GetVehicleHealth(vehicleid);
			if (hp <= 200.0f) {
				stopreason = MISSION_STATE_CRASHED;
			}
		}
		missions_end_unfinished(playerid, stopreason);
		mission_stage[playerid] = MISSION_STAGE_NOMISSION;
		break;
	}
}

void missions_on_player_disconnect(int playerid)
{
	struct MISSION *miss;

	if ((miss = activemission[playerid]) != NULL) {
		missions_end_unfinished(playerid, MISSION_STATE_ABANDONED);
	}
}

struct MISSION_CB_DATA {
	int player_cc;
	struct MISSION *mission;
};

struct MISSION_LOAD_UNLOAD_DATA {
	struct MISSION_CB_DATA cbdata;
	char isload;
	float vehiclehp;
};

/**
Starts the FLIGHT stage of the mission.

This is called either by missions_cb_create or missions_after_load, which ever
comes last (query is started when missions starts and might be ongoing during
PRELOAD/LOAD stages).
*/
static
void missions_start_flight(int playerid, struct MISSION *mission)
{
	if (prefs[playerid] & PREF_WORK_AUTONAV) {
		nav_navigate_to_airport(
			mission->veh->spawnedvehicleid,
			mission->veh->model,
			mission->endpoint->ap);
	}

	if (mission->missiontype & PASSENGER_MISSIONTYPES) {
		csprintf(buf32, SATISFACTION_TEXT_FORMAT, 100);
		NC_PARS(3);
		nc_params[1] = playerid;
		nc_params[2] = ptxt_satisfaction[playerid];
		nc_params[3] = buf32a;
		NC(n_PlayerTextDrawSetString);
		NC_PARS(2);
		NC(n_PlayerTextDrawShow);
	}

	mission_stage[playerid] = MISSION_STAGE_FLIGHT;

	NC_PARS(9);
	nc_params[1] = playerid;
	nc_params[2] = MISSION_CHECKPOINT_TYPE;
	nc_paramf[3] = nc_paramf[6] = mission->endpoint->pos.x;
	nc_paramf[4] = nc_paramf[7] = mission->endpoint->pos.y;
	nc_paramf[5] = nc_paramf[8] = mission->endpoint->pos.z;
	nc_paramf[9] = MISSION_CHECKPOINT_SIZE;
	NC(n_SetPlayerRaceCheckpoint);

	common_hide_gametext_for_player(playerid);
	NC_TogglePlayerControllable(playerid, 1);

	csprintf(buf144,
	        "UPDATE flg "
		"SET tload=UNIX_TIMESTAMP(),tlastupdate=UNIX_TIMESTAMP() "
		"WHERE id=%d",
	        mission->id);
	NC_mysql_tquery_nocb(buf144a);

	/*kneeboard*/
	NC_PARS(3);
	nc_params[1] = playerid;
	nc_params[2] = kneeboard_ptxt_info[playerid];
	nc_params[3] = buf4096a;
	csprintf(buf4096,
		"~w~Flight from %s to ~r~%s~w~.",
		mission->startpoint->ap->name,
		mission->endpoint->ap->name);
	NC(n_PlayerTextDrawSetString);
}

/**
Called when mission create query has been executed.
*/
static
void missions_querycb_create(void* d)
{
	struct MISSION_CB_DATA *data;
	struct MISSION *mission;
	int playerid;

	data = (struct MISSION_CB_DATA*) d;
	playerid = PLAYER_CC_GETID(data->player_cc);
	if (!PLAYER_CC_CHECK(data->player_cc, playerid) ||
		activemission[playerid] != data->mission)
	{
		goto ret;
	}

	mission = data->mission;
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

	if (mission_stage[playerid] == MISSION_STAGE_POSTLOAD) {
		/*loading timer already done, start flight stage*/
		missions_start_flight(playerid, mission);
	}

ret:
	free(d);
}

static const char
	*NOTYPES = WARN"This vehicle can't complete any type of missions!",
	*NOMISS = WARN"There are no missions available for this type "
		"of vehicle!",
	*VEHUSED = WARN"This vehicle is already used in a mission!",
	*MUSTBEDRIVER = WARN"You must be the driver of a vehicle "
		"before starting work!";

/**
Starts a mission for given player that is in given vehicle.
*/
static
void missions_start_mission(int playerid)
{
	struct MISSION_CB_DATA *cbdata;
	struct MISSION *mission;
	struct MISSIONPOINT *startpoint, *endpoint;
	struct dbvehicle *veh;
	struct vec3 ppos;
	int vehicleid, missiontype, i;
	float vhp, dx, dy;

	NC_PARS(1);
	nc_params[1] = playerid;
	if (NC(n_GetPlayerVehicleSeat) != 0) {
		B144((char*) MUSTBEDRIVER);
		goto err;
	}
	vehicleid = NC(n_GetPlayerVehicleID);

	if ((veh = gamevehicles[vehicleid].dbvehicle) == NULL) {
		goto unknownvehicle;
	}

	for (i = 0; i < playercount; i++) {
		if ((mission = activemission[players[i]]) != NULL &&
			mission->veh == veh)
		{
			B144((char*) VEHUSED);
			goto err;
		}
	}

	switch (veh->model) {
	case MODEL_DODO: missiontype = MISSION_TYPE_PASSENGER_S; break;
	case MODEL_SHAMAL:
	case MODEL_BEAGLE: missiontype = MISSION_TYPE_PASSENGER_M; break;
	case MODEL_AT400:
	case MODEL_ANDROM: missiontype = MISSION_TYPE_PASSENGER_L; break;
	case MODEL_NEVADA: missiontype = MISSION_TYPE_CARGO_M; break;
	case MODEL_MAVERICK:
	case MODEL_VCNMAV:
	case MODEL_RAINDANC:
	case MODEL_LEVIATHN:
	case MODEL_POLMAV:
	case MODEL_SPARROW: missiontype = MISSION_TYPE_HELI; break;
	case MODEL_HUNTER:
	case MODEL_CARGOBOB: missiontype = MISSION_TYPE_MIL_HELI; break;
	case MODEL_HYDRA:
	case MODEL_RUSTLER: missiontype = MISSION_TYPE_MIL; break;
	case MODEL_SKIMMER: missiontype = MISSION_TYPE_PASSENGER_WATER; break;
	default:
unknownvehicle:
		B144((char*) NOTYPES);
		goto err;
	}

	common_GetPlayerPos(playerid, &ppos);

	startpoint = missions_get_startpoint(missiontype, &ppos);
	endpoint = missions_get_random_endpoint(missiontype, startpoint);
	/*endpoint will also be NULL when startpoint is NULL*/
	if (endpoint == NULL) {
		B144((char*) NOMISS);
		goto err;
	}

	vhp = anticheat_GetVehicleHealth(vehicleid);

	dx = startpoint->pos.x - endpoint->pos.x;
	dy = startpoint->pos.y - endpoint->pos.y;

	mission_stage[playerid] = MISSION_STAGE_PRELOAD;
	activemission[playerid] = mission = malloc(sizeof(struct MISSION));
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
	mission->weatherbonus = 0;

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
	cbdata->mission = mission;
	cbdata->player_cc = MK_PLAYER_CC(playerid);
	common_mysql_tquery(cbuf4096_, missions_querycb_create, cbdata);

	tracker_afk_packet_sent[playerid] = 0;

	NC_PARS(9);
	nc_params[1] = playerid;
	nc_params[2] = 2;
	nc_paramf[3] = mission->startpoint->pos.x;
	nc_paramf[4] = mission->startpoint->pos.y;
	nc_paramf[5] = mission->startpoint->pos.z;
	nc_paramf[6] = nc_paramf[7] = nc_paramf[8] = 0.0f;
	nc_paramf[9] = MISSION_CHECKPOINT_SIZE;
	NC(n_SetPlayerRaceCheckpoint);

	csprintf(buf144,
		"Flight from %s (%s) %s to %s (%s) %s",
		mission->startpoint->ap->name,
		mission->startpoint->ap->code,
		mission->startpoint->name,
		mission->endpoint->ap->name,
		mission->endpoint->ap->code,
		mission->endpoint->name);
	NC_SendClientMessage(playerid, COL_MISSION, buf144a);

	if (prefs[playerid] & PREF_WORK_AUTONAV) {
		nav_navigate_to_airport(
			mission->veh->spawnedvehicleid,
			mission->veh->model,
			mission->startpoint->ap);
	}

	/*kneeboard*/
	NC_PARS(3);
	nc_params[1] = playerid;
	nc_params[2] = kneeboard_ptxt_info[playerid];
	nc_params[3] = buf4096a;
	csprintf(buf4096,
		"~w~Flight from ~r~%s~w~ to %s.",
		mission->startpoint->ap->name,
		mission->endpoint->ap->name);
	NC(n_PlayerTextDrawSetString);

	return;
err:
	NC_SendClientMessage(playerid, COL_WARN, buf144a);
}

/**
Called from timer callback for mission load checkpoint.
*/
static
void missions_after_load(int playerid, struct MISSION *mission)
{
	if (mission->id != -1) {
		/*create query finished, the mission can be started*/
		missions_start_flight(playerid, mission);
	} else {
		/*otherwise missions_querycb_create will start it*/
		mission_stage[playerid] = MISSION_STAGE_POSTLOAD;
	}
}

/**
Called from timer callback for mission unload checkpoint.
*/
static
void missions_after_unload(int playerid, struct MISSION *miss, float vehhp)
{
	float paymp, mintime;
	int ptax, psatisfaction, pdistance, pbonus = 0, ptotal, pdamage, pcheat;
	int totaltime, duration_h, duration_m, extra_damage_taken;
	int i;
	char *dlg, *dlgbase;

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
		"%s completed flight #%d from %s (%s) to %s (%s) in %dh%02dm",
		pdata[playerid]->name,
	        miss->id,
		miss->startpoint->ap->name,
	        miss->startpoint->ap->code,
	        miss->endpoint->ap->name,
	        miss->endpoint->ap->code,
	        duration_h,
	        duration_m);
	echo_on_flight_finished(cbuf4096);
	atoci(buf4096, i);
	NC_PARS(3);
	nc_params[2] = COL_MISSION;
	nc_params[3] = buf4096a;
	i = playercount;
	while (i--) {
		if (prefs[players[i]] & PREF_SHOW_MISSION_MSGS) {
			nc_params[1] = players[i];
			NC(n_SendClientMessage);
		}
	}

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

	dlg = dlgbase = malloc(4096); /*TODO: something :)*/
	dlg += sprintf(dlg,
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
		dlg += sprintf(dlg,
		             "{ffffff}Passenger Satisfaction:\t"
			     ""ECOL_MISSION"%d%%\n",
		             miss->passenger_satisfaction);
	}
	*dlg++ = '\n';
	*dlg++ = '\n';
	if (ptax) {
		dlg += missions_append_pay(dlg,
			"{ffffff}Airport Tax:\t\t",
			ptax);
	}
	if (miss->weatherbonus) {
		dlg += missions_append_pay(dlg,
			"{ffffff}Weather bonus:\t\t",
			miss->weatherbonus);
	}
	dlg += missions_append_pay(dlg, "{ffffff}Distance Pay:\t\t", pdistance);
	if (miss->missiontype & PASSENGER_MISSIONTYPES) {
		if (psatisfaction > 0) {
			dlg += missions_append_pay(dlg,
				"{ffffff}Satisfaction Bonus:\t",
				psatisfaction);
		} else {
			dlg += missions_append_pay(dlg,
				"{ffffff}Satisfaction Penalty:\t",
				psatisfaction);
		}
	}
	if (pdamage) {
		dlg += missions_append_pay(dlg,
			"{ffffff}Damage Penalty:\t",
			pdamage);
	}
	if (pcheat) {
		dlg += missions_append_pay(dlg,
			"{ffffff}Cheat Penalty:\t\t",
			pcheat);
	}
	if (pbonus) {
		dlg += missions_append_pay(dlg,
			"{ffffff}Bonus:\t\t\t",
			pbonus);
	}
	dlg += missions_append_pay(dlg,
		"\n\n\t{ffffff}Total Pay: ",
		ptotal);
	*--dlg = 0;
	dialog_ShowPlayerDialog(
		playerid,
		DIALOG_DUMMY,
		DIALOG_STYLE_MSGBOX,
		"Flight Overview",
		dlgbase,
		"Close", "",
		TRANSACTION_MISSION_OVERVIEW);
	free(dlgbase);

	missions_cleanup(playerid);
}

/**
Callback for timer set when player enters mission unload checkpoint.
*/
static
int missions_after_load_unload(void *d)
{
	struct MISSION_LOAD_UNLOAD_DATA *data;
	int playerid;

	data = (struct MISSION_LOAD_UNLOAD_DATA*) d;
	playerid = PLAYER_CC_GETID(data->cbdata.player_cc);
	if (!PLAYER_CC_CHECK(data->cbdata.player_cc, playerid) ||
		activemission[playerid] != data->cbdata.mission)
	{
		goto ret;
	}

	/*TODO: what if vehicle is destroyed?*/

	if (data->isload) {
		/*don't hide gametext or toggle player controllable,
		query might still be ongoing*/
		missions_after_load(playerid, data->cbdata.mission);
	} else {
		common_hide_gametext_for_player(playerid);
		NC_TogglePlayerControllable(playerid, 1);
		missions_after_unload(playerid,
			data->cbdata.mission, data->vehiclehp);
	}
ret:
	free(d);
	return 0; /*non-repeating timer*/
}

int missions_on_player_enter_race_checkpoint(int playerid)
{
	static const char
		*WRONGVEHICLE = WARN"You must be in the starting vehicle "
				"to continue!",
		*TOOFAST = WARN"You need to slow down to load/unload! "
				"Re-enter the checkpoint.";

	struct MISSION_LOAD_UNLOAD_DATA *lddata;
	struct MISSION *mission;
	struct vec3 cppos, vpos, vvel;
	struct dbvehicle *veh;
	int vehicleid, vv;

	if ((mission = activemission[playerid]) == NULL ||
		NC_GetPlayerVehicleSeat(playerid) != 0)
	{
		return 0;
	}

	switch (mission_stage[playerid]) {
	case MISSION_STAGE_PRELOAD: cppos = mission->startpoint->pos; break;
	case MISSION_STAGE_FLIGHT: cppos = mission->endpoint->pos; break;
	default: return 0;
	}

	vehicleid = veh_GetPlayerVehicle(playerid, &vv, &veh);

	common_GetVehiclePos(vehicleid, &vpos);
	/*this check might not be needed, but just in case the checkpoint
	position changes...*/
	if (common_dist_sq(cppos, vpos) >
		MISSION_CHECKPOINT_SIZE * MISSION_CHECKPOINT_SIZE * 2.0f)
	{
		return 0;
	}

	if (vehicleid != mission->vehicleid ||
		vv != mission->vehicle_reincarnation ||
		veh->id != mission->veh->id)
	{
		B144((char*) WRONGVEHICLE);
		goto reterr;
	}

	common_GetVehicleVelocity(vehicleid, &vvel);
	if (common_vectorsize_sq(vvel) > MAX_UNLOAD_SPEED_SQ_VEL)
	{
		B144((char*) TOOFAST);
		goto reterr;
	}

	NC_DisablePlayerRaceCheckpoint(playerid);
	NC_TogglePlayerControllable(playerid, 0);
	if (prefs[playerid] & PREF_WORK_AUTONAV) {
		nav_reset_for_vehicle(vehicleid);
		panel_reset_nav_for_passengers(vehicleid);
	}
	lddata = malloc(sizeof(struct MISSION_LOAD_UNLOAD_DATA));
	lddata->cbdata.mission = mission;
	lddata->cbdata.player_cc = MK_PLAYER_CC(playerid);

	switch (mission_stage[playerid]) {
	case MISSION_STAGE_PRELOAD:
		mission_stage[playerid] = MISSION_STAGE_LOAD;
		lddata->isload = 1;
		B144((char*) LOADING);
		break;
	case MISSION_STAGE_FLIGHT:
		mission_stage[playerid] = MISSION_STAGE_UNLOAD;
		lddata->isload = 0;
		lddata->vehiclehp = anticheat_GetVehicleHealth(vehicleid);
		if (lddata->vehiclehp < 251.0f) {
			NC_SetVehicleHealth(vehicleid, 300.0f);
		}
		NC_PlayerTextDrawHide(playerid, ptxt_satisfaction[playerid]);
		B144((char*) UNLOADING);
		break;
	}
	NC_GameTextForPlayer(playerid, buf144a, 0x8000000, 3);

	timer_set(MISSION_LOAD_UNLOAD_TIME, missions_after_load_unload, lddata);
	return 1;
reterr:
	NC_SendClientMessage(playerid, COL_WARN, buf144a);
	return 1;
}

void missions_on_player_state_changed(int playerid, int from, int to)
{
	struct MISSION *mission;
	struct VEHICLEPARAMS vpars;
	struct vec3 pos;
	int vehicleid;

	if (to == PLAYER_STATE_ONFOOT || from == PLAYER_STATE_ONFOOT) {
		vehicleid = NC_GetPlayerVehicleID(playerid);
		if (vehicleid) {
			missions_available_msptype_mask[playerid] = missions_get_vehicle_model_msptype_mask(NC_GetVehicleModel(vehicleid));
		} else {
			missions_available_msptype_mask[playerid] = -1;
		}

		/*This could be unnecessary, unless the player gets jacked or teleported.
		(They can't exit because it'd trigger the keystate change to hide the ui)*/
		if (mission_stage[playerid] == MISSION_STAGE_HELP) {
			missions_jobhelp_hide(playerid); /*sets controllable and mission state*/
		} else if (mission_stage[playerid] == MISSION_STAGE_JOBMAP) {
			missions_jobmap_hide(playerid); /*sets controllable and mission state*/
		}

		common_GetPlayerPos(playerid, &pos);
		missions_update_missionpoint_indicators(playerid, pos.x, pos.y, pos.z);
	}

	if (from == PLAYER_STATE_DRIVER &&
		(mission = activemission[playerid]) != NULL &&
		mission->vehicleid == lastvehicle[playerid])
	{
		NC_PARS(4);
		nc_params[1] = lastvehicle[playerid];
		nc_params[2] = playerid;
		nc_params[3] = 1;
		nc_params[4] = 0;
		NC(n_SetVehicleParamsForPlayer);
		B144(WARN" Get back in your vehicle!");
		NC_SendClientMessage(playerid, COL_WARN, buf144a);
	} else if (to == PLAYER_STATE_DRIVER &&
		(mission = activemission[playerid]) != NULL &&
		mission->vehicleid == NC_GetPlayerVehicleID(playerid))
	{
		/*TODO: update this mess "objective can only be disabled by disabling it globally"*/

		/*SA:MP wiki: Vehicles must be respawned for the 'objective' to
		be removed. This can be circumvented somewhat using
		Get/SetVehicleParamsEx which do not require the vehicle to be
		respawned.*/
		common_GetVehicleParamsEx(mission->vehicleid, &vpars);
		/*probably not needed since the global object is not set,
		but just in case*/
		vpars.objective = 0;
		common_SetVehicleParamsEx(mission->vehicleid, &vpars);
	}
}

void missions_on_vehicle_stream_in(int vehicleid, int forplayerid)
{
	struct MISSION *mission;

	mission = activemission[forplayerid];
	if (mission != NULL && mission->vehicleid == vehicleid) {
		NC_PARS(4);
		nc_params[1] = vehicleid;
		nc_params[2] = forplayerid;
		nc_params[3] = 1;
		nc_params[4] = 0;
		NC(n_SetVehicleParamsForPlayer);
	}
}

void missions_on_weather_changed(int weather)
{
	int bonusvalue, i;

	switch (weather) {
	case WEATHER_SF_RAINY:
	case WEATHER_CS_RAINY:
		bonusvalue = MISSION_WEATHERBONUS_RAINY;
		break;
	case WEATHER_SF_FOGGY:
	case WEATHER_UNDERWATER:
		bonusvalue = MISSION_WEATHERBONUS_FOGGY;
		break;
	case WEATHER_DE_SANDSTORMS:
		bonusvalue = MISSION_WEATHERBONUS_SANDSTORM;
		break;
	default:
		return;
	}
	bonusvalue += NC_random(MISSION_WEATHERBONUS_DEVIATION);
	i = playercount;
	while (i--) {
		if (activemission[players[i]] != NULL) {
			activemission[players[i]]->weatherbonus += bonusvalue;
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
The /cancelmission cmd, stops current mission for the player, for a fee.

Aliases: /s
*/
static
int missions_cmd_cancelmission(CMDPARAMS)
{
	static const char *NOMISSION = WARN"You're not on an active mission.";

	struct MISSION *mission;

	if ((mission = activemission[playerid]) != NULL) {
		if (mission_stage[playerid] == MISSION_STAGE_LOAD || mission_stage[playerid] == MISSION_STAGE_UNLOAD) {
			NC_TogglePlayerControllable(playerid, 1);
			common_hide_gametext_for_player(playerid);
		} else {
			NC_DisablePlayerRaceCheckpoint(playerid);
		}
		money_take(playerid, MISSION_CANCEL_FINE);
		missions_end_unfinished(playerid, MISSION_STATE_DECLINED);
	} else {
		B144((char*) NOMISSION);
		NC_SendClientMessage(playerid, COL_WARN, buf144a);
	}
	return 1;
}

/**
The /mission cmd, starts a new mission

Aliases: /w /m
*/
static
int missions_cmd_mission(CMDPARAMS)
{
	struct vec3 vel;
	struct dbvehicle *veh;
	int vehicleid;

	switch (mission_stage[playerid]) {
	case MISSION_STAGE_HELP:
	case MISSION_STAGE_JOBMAP:
		return 1;
	case MISSION_STAGE_NOMISSION:
		if (active_msp_index[playerid] != -1) {
			missions_jobmap_show(playerid); /*sets controllable and mission state*/
			return 1;
		}

		nc_params[1] = playerid;
		vehicleid = NC(n_GetPlayerVehicleID);
		veh = gamevehicles[vehicleid].dbvehicle;
		if (!veh || NC_GetPlayerVehicleSeat(playerid) != 0) {
			SendClientMessage(playerid, COL_WARN, WARN"Get in a vehicle first!");
			return 1;
		}

		/*Checking speed for two reasons:
		- this command will freeze the player, meaning they will drop from the sky when flying
		- the player should stop completely before starting a mission*/
		common_GetVehicleVelocity(vehicleid, &vel);
		/*Ignoring z velocity because it would be annoying with skimmers riding heavy waves.*/
		if (vel.x * vel.x + vel.y * vel.y > (3.0f / VEL_TO_KPH) * (3.0f / VEL_TO_KPH)) {
			SendClientMessage(playerid, COL_WARN, WARN"Stop the vehicle first!");
			return 1;
		}

		missions_available_msptype_mask[playerid] = missions_get_vehicle_model_msptype_mask(veh->model);
		if (missions_available_msptype_mask[playerid] == 0) {
			SendClientMessage(playerid, COL_WARN, WARN"This vehicle can't do any missions!");
			return 1;
		}

		/*For SOME reason, when many enexes are shown, the preview can be messed up
		(as in the type of messed up you see when only model is on screen).
		Deleting all enexes before showing the textdraw would help, but it wouldn't work
		unless there's a game update in between the deletion of the objects and the textdraw creation.
		But that would add too much delay (and complexity), so I'm just letting it be messed up atm..*/

		missions_jobhelp_show(playerid, msptype_mask_to_point_mask(missions_available_msptype_mask[playerid]));
		return 1;
	default:
		SendClientMessage(playerid, COL_WARN,
			WARN"You're already working! Use /s to stop your current work first ($"EQ(MISSION_CANCEL_FINE)" fee).");
		return 1;
	}
}

static
void missions_driversync_udkeystate_change(int playerid, short udkey)
{
	if (mission_stage[playerid] == MISSION_STAGE_HELP) {
		if (udkey < 0) {
			mission_help_update_selection_ensure_available(playerid, mission_help_option[playerid] - 1, -1);
			PlayerPlaySound(playerid, 1053);
		} else if (udkey > 0) {
			mission_help_update_selection_ensure_available(playerid, mission_help_option[playerid] + 1, 1);
			PlayerPlaySound(playerid, 1052);
		}
	} else if (mission_stage[playerid] == MISSION_STAGE_JOBMAP) {
		if (udkey < 0) {
			mission_map_update_selection_ensure_available(playerid, mission_map_option[playerid] - 1, -1);
			PlayerPlaySound(playerid, 1053);
		} else if (udkey > 0) {
			mission_map_update_selection_ensure_available(playerid, mission_map_option[playerid] + 1, 1);
			PlayerPlaySound(playerid, 1052);
		}
	}
}

void missions_driversync_keystate_change(int playerid, int oldkeys, int newkeys)
{
	if (mission_stage[playerid] == MISSION_STAGE_HELP) {
		/*Since player is set to not be controllable, these use on-foot controls even though the player is in-vehicle.*/
		if (KEY_JUST_DOWN(KEY_VEHICLE_ENTER_EXIT)) {
			PlayerPlaySound(playerid, 1084);
			missions_jobhelp_hide(playerid); /*sets controllable and mission state*/
		} else if (KEY_JUST_DOWN(KEY_SPRINT)) {
			PlayerPlaySound(playerid, 1083);
			/*TODO: LOCATE MISSION*/
			missions_jobhelp_hide(playerid); /*sets controllable and mission state*/
		}
	} else if (mission_stage[playerid] == MISSION_STAGE_JOBMAP) {
		/*Since player is set to not be controllable, these use on-foot controls even though the player is in-vehicle.*/
		if (KEY_JUST_DOWN(KEY_VEHICLE_ENTER_EXIT)) {
			PlayerPlaySound(playerid, 1084);
			missions_jobmap_hide(playerid); /*sets controllable and mission state*/
		} else if (KEY_JUST_DOWN(KEY_SPRINT)) {
			PlayerPlaySound(playerid, 1083);
			missions_jobmap_hide(playerid); /*sets controllable and mission state*/
			/*TODO: START MISSION (doublecheck vehicle and player position first!!)*/
		}
	} else if (mission_stage[playerid] == MISSION_STAGE_NOMISSION && KEY_JUST_DOWN(KEY_YES) && active_msp_index[playerid] != -1) {
		missions_jobmap_show(playerid); /*sets controllable and mission state*/
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
			csprintf(buf32, SATISFACTION_TEXT_FORMAT, miss->passenger_satisfaction);
			NC_PlayerTextDrawSetString(pid, ptxt_satisfaction[pid], buf32a);
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
